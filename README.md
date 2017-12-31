# firestorm

Evaluation of fast (and eventually distributed) vector processing for dot products.
A bench of 2048 element 32-bit floating point vectors is processed against a single
query vector.

Different operation types are implemented for evaluation:

- plain, naive for-loop
- unrolled for-loop
- hand-tuned AVX optimized for-loop
- Eigen3 backed matrix dot product
- Eigen3 backed tensor contraction (with SYCL support)

In order for Eigen3 to work with SYCL, install the [ComputeCpp library](https://www.codeplay.com/products/computesuite/computecpp).
