# Markov Random Field Partion Function Computer

* [What](#what)
* [Building](#building)
* [Usage](#usage)
* [Example](#example)
* [Fun](#fun)

## What

A Markov random field (Markov network) is a fancy name for a bunch of variables connected
together in some way.  
MarkPart is a simple terminal program that efficiently computes the
partition function of a whole Markov random field.
I mostly made up the algorithm in this program, though it is similar to variable elimination
with a fewest-neighbors heuristic (but I think mine is faster).

## Building

I've provided a Makefile which should do the job.
After you've downloaded this repository, get into its directory in a terminal and do:

```
make
```

## Usage

1. You provide a Markov random field file.
  * http://www.hlt.utdallas.edu/~vgogate/uai14-competition/modelformat.html
  * I've provided some examples in the nets/ directory of this repository.
2. You run `markpart` using that file as the command-line parameter.

## Example

```
./markpart nets/alarm.uai
```

## Fun

I wrote this program during a university course in Probabilistic Methods in AI.  
I thought it was hilarious and a bit frustrating the way statisticians, particularly
of the Bayesian variety, go out of their way to obfuscate really simple things for
no good reason.  
This program is my satirical play on that, using a different kind of obfuscation in the source code.
