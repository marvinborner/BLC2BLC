# BLC2BLC

Easily convert binary lambda calculus encodings to other binary lambda
calculus encodings!

## Encodings

See [`experiments.md`](experiments.md) for detailed explanations and
some comparisons.

Please create a PR if you know of other (better?) encodings!

## Usage

``` bash
$ make install # or make run
```

Then use `blc2blc <from> <to>`. For example:

``` bash
$ echo 001110 | blc2blc blc blc2
0011100
```
