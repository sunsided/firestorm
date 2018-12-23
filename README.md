# firestorm

Evaluation of fast (and eventually distributed) vector processing for dot products.
A bench of 2048 element 32-bit floating point vectors is processed against a single
query vector.

Different operation types are implemented for evaluation:

- plain, naive for-loop
- 8-fold unrolled for-loop
- hand-tuned AVX optimized for-loop
- hand-tuned SSE4.2 optimized for-loop
- OpenMP SIMD optimized for-loop

## CMake Options

These options configure the CMake build. To enable an option `OPTION`,
set `-DOPTION=ON`; to disable, use `-DOPTION=OFF`.
To build in release mode with link-time optimizations enabled, call e.g.

```bash
cmake -DCMAKE_BUILD_TYPE=Release \
      -DFSTM_ENABLE_LTO=ON \
      ..
```

### Build and linker options

| Option               | Default | Description |
|----------------------|---------|-------------|
| FSTM\_ENABLE\_LTO      | ON      | Enables link-time optimization (if available). |
| FSTM\_ENABLE\_CCACHE   | ON      | Enables `ccache` support when building (if available). |

### Google Performance Tools

| Option               | Default | Description |
|----------------------|---------|-------------|
| FSTM\_WITH\_PROFILER   | OFF     | Builds with performance profiler support. |
| FSTM\_WITH\_TCMALLOC   | ON      | Builds with `tcmalloc` support. |

### Functionality and performance options

| Option               | Default | Description |
|----------------------|---------|-------------|
| FSTM\_WITH\_FAST\_MATH  | OFF     | Enables fast math optimizations (if available). |
| FSTM\_WITH\_OPENMP      | ON      | Enables OpenMP support (if available). |
| FSTM\_WITH\_SIMD\_AVX2  | OFF     | Builds with AVX2 support |
| FSTM\_WITH\_SIMD\_AVX   | OFF     | Builds with AVX support |
| FSTM\_WITH\_SIMD\_SSE42 | ON      | Builds with SSE 4.2 support |

### Development options

| Option               | Default | Description |
|----------------------|---------|-------------|
| FSTM\_BUILD\_TESTS     | ON      | Builds unit tests. |

## Unit Tests

Unit tests are provided by means of [googletest](https://github.com/google/googletest).
To enable CTest testing functionality and building of the unit tests project,
configure CMake with the `-DFSTM_BUILD_TESTS=1` option,
e.g. using

```bash
mkdir build && cd build
cmake -DFSTM_BUILD_TEST=1 ..
make test
``` 

## CPU Profiling

### Google Performance Tools

Support for [Gperftools](https://github.com/gperftools/gperftools) is available in the project. In order
to profile using its CPU Profiler, enable support in CMake,
then run the program with the following environment variables set:

```bash
CPUPROFILE=/tmp/firestorm.prof
CPUPROFILE_FREQUENCY=1000
```

This will create the file specified in the `CPUPROFILE` containing
the sampling information. To display the profiling data, call

```bash
pprof --web firestorm firestorm.prof
```

Or, if `kcachegrind` is available:

```bash
pprof --callgrind firestorm firestorm.prof > firestorm.callgrind
kcachegrind firestorm.callgrind
```

If the file is empty, the application likely didn't exit normally.
More interesting information can be found [here](http://gernotklingler.com/blog/gprof-valgrind-gperftools-evaluation-tools-application-level-cpu-profiling-linux/).

### Valgrind

In order to profile the call graph with Valgrind and KCachegrind,
run the application using

```bash
valgrind --tool=callgrind ./firestorm
```

Which will create an output file, e.g. `callgrind.out.18360`.
You can then visualize the results using that file with

```bash
kcachegrind callgrind.out.18360
```

### Cache and branch prediction profiling

Read [here](http://valgrind.org/docs/manual/cg-manual.html) for further information.

## Reading material

- [hardware_destructive_interference_size](http://en.cppreference.com/w/cpp/thread/hardware_destructive_interference_size)
- [C++11 threads, affinity and hyperthreading](https://eli.thegreenplace.net/2016/c11-threads-affinity-and-hyperthreading/)
- [Gallery of Processor Cache Effects](http://igoro.com/archive/gallery-of-processor-cache-effects/)
