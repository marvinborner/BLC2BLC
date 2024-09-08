# BLC Experiments

Small experiment to find the smallest binary encoding of lambda terms. I
use an empirical approach with the terms located in `samples/`. The
source also serves as a reference C implementation of BLC encodings.

Of course, encodings that allow sharing of duplicate terms (e.g. in a
binary encoded DAG) can be even smaller. I’ve done that in the
[BLoC](https://github.com/marvinborner/BLoC) encoding and I will apply
the insights from this experiments there as well.

NOTE: Most of the test files don’t ever use larger indices than 9, so
don’t try to generalize the results below. BLC2, for example, only makes
sense when using many large indices.

## TL;DR

Optimizations for application chains don’t seem worth it. Optimizations
for abstraction chains give 3-5% space improvement for most larger
terms. Tricks to omit bits in closed terms always give smaller or
equivalent encodings.

    --- test/small ---
    blc: avg. 0.000000% reduction
    blc2: avg. -0.162328% reduction
    closed: avg. 32.382648% reduction
    abs: avg. -13.120658% reduction
    app_left: avg. -5.775075% reduction
    app_right: avg. 2.095019% reduction
    app_both: avg. 2.696779% reduction
    abs_app_left: avg. -18.895734% reduction

    --- test/medium ---
    blc: avg. 0.000000% reduction
    blc2: avg. -4.829910% reduction
    closed: avg. 0.160938% reduction
    abs: avg. 4.089551% reduction
    app_left: avg. -0.507696% reduction
    app_right: avg. -6.291403% reduction
    app_both: avg. -5.881188% reduction
    abs_app_left: avg. 3.581855% reduction

    --- test/large ---
    blc: avg. 0.000000% reduction
    blc2: avg. -4.473726% reduction
    closed: avg. 0.009495% reduction
    abs: avg. 3.859261% reduction
    app_left: avg. -2.043374% reduction
    app_right: avg. -4.231048% reduction
    app_both: avg. -3.729422% reduction
    abs_app_left: avg. 1.815887% reduction

## Traditional BLC

$$
\begin{aligned}
f(\lambda m)  &= 00\ f(M)\\
f(M N)        &= 01\ f(M)\ f(N)\\
f(i)          &= 1^i\ 0
\end{aligned}
$$

**Problem**: de Bruijn indices are encoded in *unary* ($O(n)$).

### Statistics

    test/medium/fib_rec42.blc: 14167
    test/medium/euler001.blc: 68690
    test/medium/harmonic_series42.blc: 38338
    test/medium/jottary.blc 4364
    test/medium/mutual_recusion.blc 9700
    test/medium/fac42.blc: 8681

### Resources:

- [Binary Lambda
  Calculus](https://tromp.github.io/cl/Binary_lambda_calculus.html)
  (John Tromp)

## Levenshtein Indices (`BLC2`)

$$
\begin{aligned}
f(\lambda M)  &= 00\ f(M)\\
f(M N)        &= 01\ f(M)\ f(N)\\
f(i)          &= \mathrm{code}(i)
\end{aligned}
$$

### Statistics

    test/medium/fib_rec42.blc: 14167 -> 14868 (-4.948119% reduction)
    test/medium/euler001.blc: 68690 -> 72011 (-4.834765% reduction)
    test/medium/harmonic_series42.blc: 38338 -> 40278 (-5.060254% reduction)
    test/medium/jottary.blc 4364 -> 4538 (-3.987168% reduction)
    test/medium/mutual_recusion.blc 9700 -> 10186 (-5.010309% reduction)
    test/medium/fac42.blc: 8681 -> 9121 (-5.068540% reduction)

### Resources

- [Levenshtein Coding](https://en.wikipedia.org/wiki/Levenshtein_coding)
  (Wikipedia)

## Mateusz (`closed`)

Mateusz Naściszewski’s encoding is a slight variation of the normal
encoding. The insight is, that by ignoring open terms, you can skip
writing a bit in several cases:

- for abstractions/applications in de Bruijn level 0
- in indices pointing to the outermost abstraction (we can end parsing
  at the $n$th `1`, otherwise the term would be open!)

The encoding is guaranteed to have less or equal bit than normal BLC
(again, this only works for closed terms).

$$
\begin{aligned}
f(t)            &= g(0, t)\\
g(0, i)         &= \text{error: open}\\
g(1, 0)         &= 1\\
g(n, 0)         &= 10\\
g(n, i)         &= 1\ g(n-1, i-1)\\
g(0, \lambda M) &= 0\ g(1, M)\\
g(n, \lambda M) &= 00\ g(n+1, M)\\
g(0, M\ N)      &= 1\ g(1, M)\\
g(n, M\ N)      &= 01\ g(n, M)\ g(n, N)
\end{aligned}
$$

(it looks more complicated than it is)

### Statistics

    test/medium/fib_rec42.blc: 14167 -> 14157 (0.070587% reduction)
    test/medium/euler001.blc: 68690 -> 68685 (0.007279% reduction)
    test/medium/harmonic_series42.blc: 38338 -> 38292 (0.119985% reduction)
    test/medium/jottary.blc 4364 -> 4349 (0.343721% reduction)
    test/medium/mutual_recusion.blc 9700 -> 9680 (0.206186% reduction)
    test/medium/fac42.blc: 8681 -> 8671 (0.115194% reduction)

### Resources

- [Implementation](https://github.com/tromp/AIT/commit/f8c7d191519bc8df4380d49934f2c9b9bdfeef19)
  (John Tromp)

## Merged Left-Apps (`app_left`)

Here I “merge” multiple left applications with a single prefix. This
uses one bit more for binary applications. However, practical lambda
terms often use many consecutive left applications!

$$
\begin{aligned}
f(\lambda M)    &= 01\ f(M)\\
f(M_1\dots M_n) &= 0^n\ 1\ f(M_1)\dots f(M_n)\\
f(i)            &= 1^i 0
\end{aligned}
$$

### Statistics

    test/medium/fib_rec42.blc: 14167 -> 14215 (-0.338816% reduction)
    test/medium/euler001.blc: 68690 -> 68919 (-0.333382% reduction)
    test/medium/harmonic_series42.blc: 38338 -> 38450 (-0.292138% reduction)
    test/medium/jottary.blc 4364 -> 4459 (-2.176902% reduction)
    test/medium/mutual_recusion.blc 9700 -> 9738 (-0.391753% reduction)
    test/medium/fac42.blc: 8681 -> 8699 (-0.207349% reduction)

## Merged Right-Apps (`app_right`)

Same as previous, but for right associative application. Useful for
Church encodings.

$$
\begin{aligned}
f(\lambda M)      &= 01\ f(M)\\
f(M_1(\dots M_n)) &= 0^n\ 1\ f(M_1)\dots f(M_n)\\
f(i)              &= 1^i 0
\end{aligned}
$$

### Statistics

    test/medium/fib_rec42.blc: 14167 -> 15029 (-6.084563% reduction)
    test/medium/euler001.blc: 68690 -> 73035 (-6.325520% reduction)
    test/medium/harmonic_series42.blc: 38338 -> 40606 (-5.915802% reduction)
    test/medium/jottary.blc 4364 -> 4719 (-8.134739% reduction)
    test/medium/mutual_recusion.blc 9700 -> 10314 (-6.329897% reduction)
    test/medium/fac42.blc: 8681 -> 9207 (-6.059210% reduction)

## Merged Bidirectional Apps (`app_both`)

One extra bit to indicate whether to use `app_left` or `app_right`:

$$
\begin{aligned}
f(\lambda M)      &= 01\ f(M)\\
f(M_1\dots M_n)   &= 0^n\ 10\ f(M_1)\dots f(M_n)\\
f(M_1(\dots M_n)) &= 0^n\ 11\ f(M_1)\dots f(M_n)\\
f(i)              &= 1^i 0
\end{aligned}
$$

### Statistics

    test/medium/fib_rec42.blc: 14167 -> 14962 (-5.611633% reduction)
    test/medium/euler001.blc: 68690 -> 72681 (-5.810162% reduction)
    test/medium/harmonic_series42.blc: 38338 -> 40381 (-5.328916% reduction)
    test/medium/jottary.blc 4364 -> 4760 (-9.074244% reduction)
    test/medium/mutual_recusion.blc 9700 -> 10260 (-5.773196% reduction)
    test/medium/fac42.blc: 8681 -> 9150 (-5.402603% reduction)

## Merged Multi-Abs

Here I “merge” multiple abstractions with a single prefix. (`abs`)

$$
\begin{aligned}
f(\lambda_1\dots \lambda_n M) &= 0^{n+1}\ 1\ f(M)\\
f(M\ N)                       &= 01\ f(M)\ f(N)\\
f(i)                          &= 1^i 0
\end{aligned}
$$

Slightly better, I can combine this with the previous trick for
applications! (`abs_app_left`)

$$
\begin{aligned}
f(\lambda_1\dots \lambda_n M) &= 0^n\ 10\ f(M)\\
f(M_1\ M_n)                   &= 0^{n-1}\ 11\ f(M_1)\dots f(M_n)\\
f(i)                          &= 1^i 0
\end{aligned}
$$

Theoretically, you could also merge them in a single step. The *trick*
would be to see applications as zeroth abstractions. Any immediate
applications inside abstractions would be handled directly by the
abstraction constructor.

However, unabstracted applications would get quite large so this
optimization doesn’t seem worth it.

$$
\begin{aligned}
f(\lambda_1\dots \lambda_n (M_1\dots M_m)) &= 0^{n+1}\ 1\ 0^{m+1}\ 1\ f(M_1)\dots f(M_n)\\
f(i)                                       &= 1^i 0
\end{aligned}
$$

### Statistics

abs:

    test/medium/fib_rec42.blc: 14167 -> 13519 (4.574010% reduction)
    test/medium/euler001.blc: 68690 -> 65627 (4.459164% reduction)
    test/medium/harmonic_series42.blc: 38338 -> 36557 (4.645521% reduction)
    test/medium/jottary.blc 4364 -> 4295 (1.581118% reduction)
    test/medium/mutual_recusion.blc 9700 -> 9285 (4.278351% reduction)
    test/medium/fac42.blc: 8681 -> 8278 (4.642322% reduction)

abs_app_left:

    test/medium/fib_rec42.blc: 14167 -> 13567 (4.235194% reduction)
    test/medium/euler001.blc: 68690 -> 65856 (4.125783% reduction)
    test/medium/harmonic_series42.blc: 38338 -> 36669 (4.353383% reduction)
    test/medium/jottary.blc 4364 -> 4390 (-0.595784% reduction)
    test/medium/mutual_recusion.blc 9700 -> 9323 (3.886598% reduction)
    test/medium/fac42.blc: 8681 -> 8296 (4.434973% reduction)
