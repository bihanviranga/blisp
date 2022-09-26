# blisp

`blisp` is a Lisp variant. It is a work in progress.

## Installation

### Dependencies

* libedit
* [mpc](https://github.com/orangeduck/mpc)

### Building

Makefile is included. Run `make`.

## Usage

Supports mathematical operators:
```
blisp> + 2 5
7
blisp> * (+ 3 4) (+ 1 5)
42
```

Contains built-in functions:
```
blisp> list 2 3 5
{2 3 5}
blisp> head { 2 3 4 }
{2}
blisp> tail { 2 3 4 }
{3 4}
```
