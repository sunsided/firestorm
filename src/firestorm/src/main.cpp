#include <iostream>
#include <random>
#include <memory>

#ifdef USE_PROFILER
#include <gperftools/profiler.h>
#endif

#include <CLI/CLI.hpp>

#include <logging/LoggerFactory.h>
#include <firestorm/Simd.h>
#include <firestorm/OpenMP.h>
#include <firestorm/ChunkManager.h>
#include <firestorm/Worker.h>
#include <firestorm/DotProductVisitor.h>
#include <firestorm/dot_product_naive.h>
#if USE_AVX
#include <firestorm/dot_product_avx256.h>
#endif
#include <firestorm/dot_product_openmp.h>
#include <firestorm/dot_product_sse42.h>

#include "options/options.h"
#include "benchmark.h"

using namespace std;
namespace spd = spdlog;

// TODO: Boost
// TODO: Boost.SIMD
// TODO: determine __restrict__ keyword support from https://github.com/elemental/Elemental/blob/master/cmake/detect/CXX.cmake

void what(const shared_ptr<spdlog::logger> &log, const size_t num_vectors) {
    const auto seed = 1337; // std::chrono::system_clock::now().time_since_epoch().count();

    std::default_random_engine generator(seed);
    std::normal_distribution<float> distribution(0.0f, 2.0f);
    auto random = std::bind(distribution, generator);

    log->info("Initializing vectors ...");
    auto expected = new float[num_vectors];
    auto result = new float[num_vectors];

    // We first create a chunk manager that will hold the vectors.
    std::shared_ptr<ChunkManager> chunkManager = std::make_shared<ChunkManager>();
    constexpr const auto target_chunk_size = 32_MB;
    constexpr size_t num_vectors_per_chunk = target_chunk_size / (NUM_DIMENSIONS*sizeof(float));

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
    vector_t query = create_query_vector(NUM_DIMENSIONS);

    // Create M vectors (1000, 10000, whatever).
    for (size_t j = 0; j < num_vectors; ++j) {

        // Initial condition, also reached during runtime:
        // If one memory chunk is "full", allocate another one.
        if (remaining_chunk_size == 0_B) {
            log->debug("Allocating chunk.");

            chunk = chunkManager->allocate(num_vectors_per_chunk, NUM_DIMENSIONS);
            assert(chunk != nullptr);

            remaining_chunk_size = target_chunk_size;
            float_offset = 0;

            worker->assign_chunk(chunk->index);
        }

        // Some progress printing.
        if (j % 2500 == 0) {
            log->info("- {}/{}", j, num_vectors);
        }

        auto a = &chunk->data[float_offset];
        const auto *const b = &query.data[0];

        assert(float_offset < num_vectors*NUM_DIMENSIONS);
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
    log->info("- {}/{}", num_vectors, num_vectors);
    log->info("Vectors initialized"); // TODO: Add timing

    const size_t repetitions = 20;

#if USE_AVX

    log->info("dot_product_avx256");
    run_test_round<dot_product_avx256_t>(log, repetitions, result, *chunkManager, query, target_chunk_size,
                                         expected_best_match_idx, expected_best_match, num_vectors);

    log->info("dot_product_avx256 (Worker)");
    run_test_round_worker<dot_product_avx256_t>(log, repetitions, *worker, query, expected_best_match_idx, expected_best_match, num_vectors);

#endif

#if USE_OPENMP

    log->info("dot_product_openmp");
    run_test_round<dot_product_openmp_t>(log, repetitions, result, *chunkManager, query, target_chunk_size,
                                         expected_best_match_idx, expected_best_match, num_vectors);

    log->info("dot_product_openmp (Worker)");
    run_test_round_worker<dot_product_openmp_t>(log, repetitions, *worker, query, expected_best_match_idx, expected_best_match, num_vectors);

#endif

#if USE_SSE == 4

    log->info("dot_product_sse42");
    run_test_round<dot_product_sse42_t>(log, repetitions, result, *chunkManager, query, target_chunk_size,
                                        expected_best_match_idx, expected_best_match, num_vectors);

    log->info("dot_product_sse42 (Worker)");
    run_test_round_worker<dot_product_sse42_t>(log, repetitions, *worker, query, expected_best_match_idx, expected_best_match, num_vectors);

#endif

    log->info("dot_product_unrolled_8");
    run_test_round<dot_product_unrolled_8_t>(log, repetitions, result, *chunkManager, query, target_chunk_size,
                                             expected_best_match_idx, expected_best_match, num_vectors);

    log->info("dot_product_unrolled_8 (Worker)");
    run_test_round_worker<dot_product_unrolled_8_t>(log, repetitions, *worker, query, expected_best_match_idx, expected_best_match, num_vectors);

    log->info("dot_product_naive");
    run_test_round<dot_product_naive_t>(log, repetitions, result, *chunkManager, query, target_chunk_size,
                                        expected_best_match_idx, expected_best_match, num_vectors);

    log->info("dot_product_naive (Worker)");
    run_test_round_worker<dot_product_naive_t>(log, repetitions, *worker, query, expected_best_match_idx, expected_best_match, num_vectors);

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

unique_ptr<LoggerFactory> configure_logging(const spdlog::level::level_enum verbosity) {
    // TODO: https://github.com/gabime/spdlog/wiki/1.-QuickStart
    try
    {
        auto factory = make_unique<LoggerFactory>();
        factory->setAsync()
                .addConsole(verbosity);

        return factory;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Initialization of the logging subsystem failed: " << ex.what() << std::endl;
        print_exception(ex);
        return nullptr;
    }
}

int main(int argc, char **argv) {
    // https://cliutils.gitlab.io/CLI11Tutorial/
    CLI::App app{"firestorm vector search engine"};

    auto verbosity = spd::level::info;
#if USE_AVX
    size_t num_vectors = 100000;
#else
    size_t num_vectors = 5000;
#endif

    add_option(app, "-V,--verbosity", verbosity, "Sets the output verbosity. One of: trace, debug, info, warn, error.", true)
            ->group("Logging")
            ->envname("FSTM_VERBOSITY");

    app.add_option("-n,--vectors", num_vectors, "Sets the number of vectors to test with.", true)
            ->group("Benchmark")
            ->envname("FSTM_NUM_VECTORS");

    CLI11_PARSE(app, argc, argv);

    auto loggerFactory = configure_logging(verbosity);
    if (loggerFactory == nullptr) {
        return 1;
    }

    auto logger = loggerFactory->createLogger("firestorm");
    logger->info("Firestorm starting.");

    auto benchmarkLogger = loggerFactory->createLogger("benchmark", spdlog::level::debug);

#if USE_PROFILER
    ProfilerState state{};
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

    if (openmp_enabled()) {
        logger->info("OpenMP support: enabled (version {})", openmp_version());
    } else {
        logger->info("OpenMP support: disabled (by configuration)");
    }

    if (!avx2_enabled() && !avx_enabled() && !sse42_enabled()) {
        logger->info("AVX/AVX2 or SSE4.2 support is required for optimal performance.");
    }

    what(benchmarkLogger, num_vectors);

    return 0;
}
