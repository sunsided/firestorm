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
the sampling information.

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

### Cache and branch prediciton profiling

Read [here](http://valgrind.org/docs/manual/cg-manual.html) for further information.