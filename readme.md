# BLC Experiments

Small experiment to find the smallest binary encoding of lambda terms. I
use an empirical approach with the terms located in `samples/`. The
source also serves as a reference C implementation of BLC encodings.

Of course, encodings that allow sharing of duplicate terms (e.g. in a
binary encoded DAG) can yield even smaller encodings. I’ve done that in
the [BLoC](https://github.com/marvinborner/BLoC) encoding and I will
apply the insights from this experiments there as well.

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

TODO

### Resources:

- [Binary Lambda
  Calculus](https://tromp.github.io/cl/Binary_lambda_calculus.html)
  (John Tromp)

## Levenshtein Indices (BLC2)

$$
\begin{aligned}
f(\lambda M)  &= 00\ f(M)\\
f(M N)        &= 01\ f(M)\ f(N)\\
f(i)          &= \mathrm{code}(i)
\end{aligned}
$$

### Statistics

TODO (compare with blc)

### Resources

- [Levenshtein Coding](https://en.wikipedia.org/wiki/Levenshtein_coding)
  (Wikipedia)

## Mateusz Naściszewski

$$
\begin{aligned}
f(t)            &= g(0, t)\\
g(0, i)         &= \text{undefined}\\
g(1, 0)         &= 1\\
g(n, 0)         &= 10\\
g(n, i)         &= 1\ g(n-1, i-1)\\
g(0, \lambda M) &= 0\ g(1, M)\\
g(n, \lambda M) &= 00\ g(n+1, M)\\
g(0, M\ N)      &= 1\ g(1, M)\\
g(n, M\ N)      &= 01\ g(n, M)\ g(n, N)
\end{aligned}
$$

### Statistics

TODO

### Resources

- [Implementation](https://github.com/tromp/AIT/commit/f8c7d191519bc8df4380d49934f2c9b9bdfeef19)
  (John Tromp)

## Merged Left-Apps

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

TODO

### Resources

- None?

## Merged Multi-Abs

Here I “merge” multiple abstractions with a single prefix.

$$
\begin{aligned}
f(\lambda_1\dots \lambda_n M) &= 0^{n+1}\ 1\ f(M)\\
f(M\ N)                       &= 01\ f(M)\ f(N)\\
f(i)                          &= 1^i 0
\end{aligned}
$$

Slightly better, I can combine this with the previous technique!

$$
\begin{aligned}
f(\lambda_1\dots \lambda_n M) &= 0^n\ 11\ f(M)\\
f(M_1\ M_n)                   &= 0^n\ 10\ f(M_1)\dots f(M_n)\\
f(i)                          &= 1^i 0
\end{aligned}
$$

Or, merge it directly! The *trick* is to see applications as zeroth
abstractions. Any immediate applications inside abstractions are handled
directly by the abstraction constructor.

$$
\begin{aligned}
f(\lambda_1\dots \lambda_n (M_1\dots M_m)) &= 0^n\ 1\ 0^m\ 1\ f(M_1)\dots f(M_n)\\
f(i)                                       &= 1^i 0
\end{aligned}
$$

### Statistics

TODO

### Resources

- None?
