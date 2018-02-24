#include <iostream>
#include <random>
#include <chrono>
#include <functional>
#include <memory>

#ifdef USE_PROFILER
#include <gperftools/profiler.h>
#endif

#include <spdlog/spdlog.h>
#include <logging/LoggerFactory.h>
#include "firestorm/Simd.h"
#include "firestorm/ChunkManager.h"
#include "firestorm/Worker.h"
#include "firestorm/DotProductVisitor.h"
#include "firestorm/dot_product_naive.h"
#if USE_AVX
#include "firestorm/dot_product_avx256.h"
#endif
#include "firestorm/dot_product_openmp.h"
#include "firestorm/dot_product_sse42.h"

using namespace std;
namespace spd = spdlog;

// TODO: Boost
// TODO: Boost.SIMD
// TODO: OpenMP backed loops
// TODO: determine __restrict__ keyword support from https://github.com/elemental/Elemental/blob/master/cmake/detect/CXX.cmake

const auto MS_TO_S = 1000.0F;

const size_t NUM_DIMENSIONS = 2048;
#if AVX_VERSION
const size_t NUM_VECTORS = 100000;
#else
const size_t NUM_VECTORS = 2500;
#endif

vector_t create_query_vector(const shared_ptr<spdlog::logger> &log) {
    const auto seed = 0L;
    std::default_random_engine generator(seed);
    std::normal_distribution<float> distribution(0.0f, 2.0f);
    auto random = std::bind(distribution, generator);

    // Create a simple query vector
    vector_t query {NUM_DIMENSIONS};
    for (size_t i = 0; i < NUM_DIMENSIONS; ++i) {
        query.data[i] = random();
    }

#if USE_AVX
    auto norm = vec_normalize_avx256(query.data, query.dimensions);
    auto norm2 = vec_norm_avx256(query.data, query.dimensions);
#else
    auto norm = vec_normalize_naive(query.data, query.dimensions);
    auto norm2 = vec_norm_naive(query.data, query.dimensions);
#endif

    log->debug("Test vector norm before normalizing is {} ({} after that).", norm, norm2);
    return query;
}

template <typename T>
void run_test_round(const shared_ptr<spdlog::logger> &log, const size_t repetitions, float *const result, const ChunkManager &chunkManager,
                    const vector_t& query, const bytes_t chunk_size, const size_t expected_best_idx, float expected_best_score) {

    static_assert(std::is_convertible<T*, dot_product_t*>::value, "Derived type must inherit dot_product_t as public");
    const T calculate {};

    const auto vectors_per_chunk = chunk_size / (NUM_DIMENSIONS * sizeof(float));
    auto duration_ms = static_cast<size_t>(0);

    for (size_t repetition = 0; repetition < repetitions; ++repetition) {
        auto start_time = std::chrono::_V2::system_clock::now();

        // Keep track of the total sum for validation.
        auto best_match = 0.0f;
        auto best_match_idx = static_cast<size_t>(0);

        chunk_idx_t current_chunk = 0;
        auto chunk = chunkManager.get_ro(current_chunk);
        auto query_vector = query.data;

        auto float_offset = 0;
        auto remaining_vectors_per_chunk = vectors_per_chunk;

        // Run over all vectors ...
        for (size_t vector_idx = 0; vector_idx < NUM_VECTORS; ++vector_idx) {

            if (remaining_vectors_per_chunk == 0) {
                current_chunk += 1;
                chunk = chunkManager.get_ro(current_chunk);

                float_offset = 0;
                remaining_vectors_per_chunk = vectors_per_chunk;
            }

            auto ref_vector = &chunk->data[float_offset];

            --remaining_vectors_per_chunk;
            float_offset += NUM_DIMENSIONS;

            // Calculate the dot product of the 2048-element vector
            static_assert((NUM_DIMENSIONS & 31) == 0, "Vector length must be a multiple of 32 elements.");
            const auto dot_product = calculate(ref_vector, query_vector, NUM_DIMENSIONS);

            result[vector_idx] = dot_product;

            if (dot_product > best_match) {
                best_match = dot_product;
                best_match_idx = vector_idx;
            }
        }

        auto end_time = std::chrono::_V2::system_clock::now();
        auto local_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        duration_ms += local_duration_ms;

        auto local_vectors_per_second = static_cast<float>(NUM_VECTORS) * MS_TO_S / static_cast<float>(local_duration_ms);
        log->debug("- Round {}/{}: Best match: {} at {} (expected {} at {}) - duration: {} ms ({} vectors/s)",
                   repetition + 1, repetitions,
                   best_match, best_match_idx,
                   expected_best_score, expected_best_idx,
                   duration_ms, local_vectors_per_second);
    }

    const size_t num_vectors = repetitions * NUM_VECTORS;
    auto vectors_per_second = static_cast<float>(num_vectors) * MS_TO_S / static_cast<float>(duration_ms);
    log->info("- Processed {} vectors in {} ms ({} vectors/s)",
              num_vectors, duration_ms, vectors_per_second);
}

template <typename T>
void run_test_round_worker(const shared_ptr<spdlog::logger> &log, const size_t repetitions, const Worker &worker,
                    const vector_t& query, const size_t expected_best_idx, const float expected_best_score) {

    const DotProductVisitor<T> visitor {};
    auto duration_ms = static_cast<size_t>(0);

    for (size_t repetition = 0; repetition < repetitions; ++repetition) {
        auto start_time = std::chrono::_V2::system_clock::now();
        auto results = worker.create_result_buffer();

        // Keep track of the total sum for validation.
        auto best_match = 0.0f;
        auto best_match_idx = static_cast<size_t>(0);

        worker.accept(visitor, query, results);

        auto end_time = std::chrono::_V2::system_clock::now();
        auto local_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        duration_ms += local_duration_ms;

        // TODO: This should eventually be part of the worker, otherwise we're going through the lists twice.
        for (auto chunk_result : results) {
            for (size_t vector_idx = 0; vector_idx < NUM_VECTORS; ++vector_idx) {
                const auto score = chunk_result.second->scores[vector_idx];
                if (score > best_match) {
                    best_match = score;
                    best_match_idx = vector_idx;
                }
            }
        }

        auto local_vectors_per_second = static_cast<float>(NUM_VECTORS) * MS_TO_S / static_cast<float>(local_duration_ms);
        log->debug("- Round {}/{}: Best match: {} at {} (expected {} at {}) - duration: {} ms ({} vectors/s)",
                   repetition + 1, repetitions,
                   best_match, best_match_idx,
                   expected_best_score, expected_best_idx,
                   duration_ms, local_vectors_per_second);
    }

    const size_t num_vectors = repetitions * NUM_VECTORS;
    auto vectors_per_second = static_cast<float>(num_vectors) * MS_TO_S / static_cast<float>(duration_ms);
    log->info("- Processed {} vectors in {} ms ({} vectors/s)",
              num_vectors, duration_ms, vectors_per_second);
}

void what(const shared_ptr<spdlog::logger> &log) {
    const auto seed = 1337; // std::chrono::system_clock::now().time_since_epoch().count();

    std::default_random_engine generator(seed);
    std::normal_distribution<float> distribution(0.0f, 2.0f);
    auto random = std::bind(distribution, generator);

    auto expected = new float[NUM_VECTORS];
    auto result = new float[NUM_VECTORS];

    // We first create two chunk managers that will hold the vectors.
    log->info("Initializing vectors ...");
    std::shared_ptr<ChunkManager> chunkManager = std::make_shared<ChunkManager>();
    constexpr const auto target_chunk_size = 32_MB;
    constexpr size_t num_vectors = target_chunk_size / (NUM_DIMENSIONS*sizeof(float));

    // A worker is a visitor that is performs a calculation on the chunks of a
    // registered manager.
    std::unique_ptr<Worker> worker = std::make_unique<Worker>(chunkManager);

    // To simplify experiments, we require the block to exactly match our expectations
    // about vector lengths. Put differently, all bytes in the buffer can be used.
    static_assert((target_chunk_size % (sizeof(float)*NUM_DIMENSIONS)) == 0, "Chunk size must be able to fully contain all vectors.");

    std::shared_ptr<mem_chunk_t> chunk = nullptr;
    auto remaining_chunk_size = 0_B;    // number of remaining bytes in the current chunk
    size_t float_offset = 0;            // index into the current buffer, counts floats

    // TODO: Allocate separate chunk for vector norms
    // TODO: Sort out elements by NaN for unused norms, e.g. https://stackoverflow.com/questions/31818755/comparison-with-nan-using-avx

    // Keep track of the results for validation.
    auto expected_best_match = 0.0f;
    auto expected_best_match_idx = static_cast<size_t>(-1);

    // Create a random query vector.
    vector_t query = create_query_vector(log);

    // Create M vectors (1000, 10000, whatever).
    for (size_t j = 0; j < NUM_VECTORS; ++j) {

        // Initial condition, also reached during runtime:
        // If one memory chunk is "full", allocate another one.
        if (remaining_chunk_size == 0_B) {
            log->debug("Allocating chunk.");

            chunk = chunkManager->allocate(num_vectors, NUM_DIMENSIONS);
            assert(chunk != nullptr);

            remaining_chunk_size = target_chunk_size;
            float_offset = 0;

            worker->assign_chunk(chunk->index);
        }

        // Some progress printing.
        if (j % 2500 == 0) {
            log->info("- {}/{}", j, NUM_VECTORS);
        }

        auto a = &chunk->data[float_offset];
        const auto *const b = &query.data[0];

        assert(float_offset < NUM_VECTORS*NUM_DIMENSIONS);
        assert(remaining_chunk_size >= sizeof(float)*NUM_DIMENSIONS);

        remaining_chunk_size -= (sizeof(float)*NUM_DIMENSIONS);
        float_offset += NUM_DIMENSIONS;

        // Create a vector.
        for (size_t i = 0; i < NUM_DIMENSIONS; ++i) {
            a[i] = random();
        }
        vec_normalize_naive(a, chunk->dimensions);

        // Obtain the expected result.
        expected[j] = 0;
        for (size_t i = 0; i < NUM_DIMENSIONS; ++i) {
            expected[j] += a[i] * b[i];
        }

        if (expected[j] > expected_best_match) {
            expected_best_match = expected[j];
            expected_best_match_idx = j;
        }
    }
    log->info("- {}/{}", NUM_VECTORS, NUM_VECTORS);
    log->info("Vectors initialized"); // TODO: Add timing

    // Worker test
#if USE_AVX
    DotProductVisitor<dot_product_avx256_t> visitor;
#else
    DotProductVisitor<dot_product_unrolled_8_t> visitor;
#endif

    const size_t repetitions = 20;

#if USE_AVX

    log->info("dot_product_avx256");
    run_test_round<dot_product_avx256_t>(log, result, *chunkManager, query, target_chunk_size,
                                         expected_best_match_idx, expected_best_match);

    log->info("dot_product_avx256 (Worker)");
    run_test_round_worker<dot_product_avx256_t>(log, repetitions, *worker, query, expected_best_match_idx, expected_best_match);

#endif

#if USE_OPENMP

    log->info("dot_product_openmp");
    run_test_round<dot_product_openmp_t>(log, repetitions, result, *chunkManager, query, target_chunk_size,
                                         expected_best_match_idx, expected_best_match);

    log->info("dot_product_openmp (Worker)");
    run_test_round_worker<dot_product_openmp_t>(log, repetitions, *worker, query, expected_best_match_idx, expected_best_match);

#endif

#if USE_SSE == 4

    log->info("dot_product_sse42");
    run_test_round<dot_product_sse42_t>(log, repetitions, result, *chunkManager, query, target_chunk_size,
                                        expected_best_match_idx, expected_best_match);

    log->info("dot_product_sse42 (Worker)");
    run_test_round_worker<dot_product_sse42_t>(log, repetitions, *worker, query, expected_best_match_idx, expected_best_match);

#endif

    log->info("dot_product_unrolled_8");
    run_test_round<dot_product_unrolled_8_t>(log, repetitions, result, *chunkManager, query, target_chunk_size,
                                             expected_best_match_idx, expected_best_match);

    log->info("dot_product_unrolled_8 (Worker)");
    run_test_round_worker<dot_product_unrolled_8_t>(log, repetitions, *worker, query, expected_best_match_idx, expected_best_match);

    log->info("dot_product_naive");
    run_test_round<dot_product_naive_t>(log, repetitions, result, *chunkManager, query, target_chunk_size,
                                        expected_best_match_idx, expected_best_match);

    log->info("dot_product_naive (Worker)");
    run_test_round_worker<dot_product_naive_t>(log, repetitions, *worker, query, expected_best_match_idx, expected_best_match);

    log->info("Cleaning up ...");
    delete[] expected;
    delete[] result;

    log->info("Done.");
}

void print_exception(const std::exception& e, int level = 0) {
    std::cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
    try {
        std::rethrow_if_nested(e);
    } catch(const std::exception& e) {
        print_exception(e, level+1);
    } catch(...) {}
}

unique_ptr<LoggerFactory> configure_logging() {
    // TODO: https://github.com/gabime/spdlog/wiki/1.-QuickStart
    try
    {
        auto factory = make_unique<LoggerFactory>();
        factory->setAsync()
                .addConsole(spdlog::level::debug);

        return factory;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Initialization of the logging subsystem failed: " << ex.what() << std::endl;
        print_exception(ex);
        return nullptr;
    }
}

int main() {
    auto loggerFactory = configure_logging();
    if (loggerFactory == nullptr) {
        return 1;
    }

    auto logger = loggerFactory->createLogger("firestorm");
    logger->info("Firestorm starting.");

    auto benchmarkLogger = loggerFactory->createLogger("benchmark", spdlog::level::debug);

#if USE_PROFILER
    ProfilerState state {};
    ProfilerGetCurrentState(&state);
    logger->info("Profiling enabled: {}", state.enabled ? "yes" : "no");
#endif

    // TODO: Use cpu_features
    // TODO: Report OpenMP support

    if (avx2_enabled()) {
        logger->info("AVX2 support: enabled");
    } else {
        logger->info("AVX2 support: disabled ({} on machine)", avx2_available() ? "available" : "missing");
    }

    if (avx_enabled()) {
        logger->info("AVX support: enabled");
    } else {
        logger->info("AVX support: disabled ({} on machine)", avx_available() ? "available" : "missing");
    }

    if (sse42_enabled()) {
        logger->info("SSE4.2 support: enabled");
    } else {
        logger->info("SSE4.2 support: disabled ({} on machine)", sse42_available() ? "available" : "missing");
    }

    if (!avx2_enabled() && !avx_enabled() && !sse42_enabled()) {
        logger->info("AVX/AVX2 or SSE4.2 support is required for optimal performance.");
    }

    what(benchmarkLogger);

    return 0;
}
