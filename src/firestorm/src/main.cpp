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

#include "options/options.h"
#include "benchmark.h"

using namespace std;
namespace spd = spdlog;

// TODO: Boost
// TODO: Boost.SIMD
// TODO: determine __restrict__ keyword support from https://github.com/elemental/Elemental/blob/master/cmake/detect/CXX.cmake

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

void report_profiler([[maybe_unused]] shared_ptr<spdlog::logger> &logger) {
#if USE_PROFILER
    ProfilerState state{};
    ProfilerGetCurrentState(&state);
    logger->info("Profiling enabled: {}", state.enabled ? "yes" : "no");
#endif
}

void report_optimizations(shared_ptr<spdlog::logger> &logger) {
    // TODO: Use cpu_features

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
}

int main(int argc, char **argv) {
    // https://cliutils.gitlab.io/CLI11Tutorial/
    CLI::App app{"firestorm vector search engine"};
    CLI::App* benchmark = app.add_subcommand("benchmark", "Runs algorithm benchmarks");

    auto verbosity = spd::level::info;
#if USE_AVX
    size_t num_vectors = 100000;
#else
    size_t num_vectors = 8192;
#endif
    size_t chunk_size_mb = 32;

    // Main options
    add_option(app, "-V,--verbosity", verbosity, "Sets the output verbosity. One of: trace, debug, info, warn, error.", true)
            ->group("Logging")
            ->envname("FSTM_VERBOSITY");

    // Benchmark options
    benchmark->add_option("-n,--vectors", num_vectors, "Sets the number of vectors to test with.", true)
            ->group("Benchmark")
            ->envname("FSTM_NUM_VECTORS");
    benchmark->add_option("-c,--chunk-size", chunk_size_mb, "Sets the vector chunk size in megabytes.", true)
            ->group("Benchmark")
            ->envname("FSTM_CHUNK_SIZE");
    add_option(*benchmark, "-V,--verbosity", verbosity, "Sets the output verbosity. One of: trace, debug, info, warn, error.", true)
            ->group("Logging")
            ->envname("FSTM_VERBOSITY");

    CLI11_PARSE(app, argc, argv);

    auto loggerFactory = configure_logging(verbosity);
    if (loggerFactory == nullptr) {
        return 1;
    }

    auto logger = loggerFactory->createLogger("firestorm");
    logger->info("firestorm VSE starting.");

    report_profiler(logger);
    report_optimizations(logger);

    // TODO: Print "unknown" hardware concurrency if zero
    logger->info("Hardware concurrency: {} threads.", thread::hardware_concurrency());

    if(benchmark->parsed()) {
        auto benchmarkLogger = loggerFactory->createLogger("benchmark", verbosity);
        run_benchmark(benchmarkLogger, num_vectors, chunk_size_mb * 1024UL * 1024UL);
    }

    return 0;
}
