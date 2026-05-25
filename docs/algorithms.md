# sqrt_bench Algorithms

`sqrt_bench` targets non-negative square roots for positive finite `float` and
`double` inputs. Version 0.1.0 implements a curated set of representative
methods rather than an exhaustive historical catalog.

## Methods

- `std_sqrt`: `std::sqrt`, used as the correctness and performance baseline.
- `pow_half`: `std::pow(x, 0.5)`, included as a common identity-style baseline.
- `exp_log`: `exp(log(x) / 2)`, included to measure the logarithmic identity.
- `newton_raphson`: Heron's method with range scaling and fixed refinement.
- `bisection`: monotonic binary search over the scaled mantissa range.
- `halley`: cubic-convergence iteration for `y * y - x = 0`.
- `goldschmidt`: inverse-square-root refinement followed by multiplication.
- `fast_inverse_sqrt`: Quake-style bit approximation with Newton refinement.
- `bit_estimate_newton`: exponent/mantissa bit estimate followed by Newton.
- `digit_by_digit`: binary digit extraction on the scaled mantissa.

## Input Policy

All public method dispatch requires `x > 0` and finite. Invalid input throws
`std::domain_error`. Negative numbers, zero, infinities, NaNs, SIMD, complex
numbers, and arbitrary precision arithmetic are outside the v1 scope.

## Scaling

Most iterative algorithms use `frexp`/`ldexp` to reduce the input to a mantissa
near `[0.5, 2)`, compute the square root there, and restore the exponent. This
keeps the algorithms away from avoidable overflow and underflow while preserving
the method's core behavior.
