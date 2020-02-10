# Tableaux SAT

SAT solver based on tableaux method.

## Install

To compile the sources makefile, which is present in the repository, can be used. The executable will be placed in a build directory: `build/tableaux-sat`

## Tutorial

Tableaux SAT solver

### 1. Satisfiability of a Formula

### 2. Satisfiability of a Theory

Input format: Each formula is on separate line. The first line contains a formula to prove in the theory. Following line contain axioms of the theory.

### 3. Dot Language Output

## Connectives

| Symbol | Connective | Priority |
| :----: | :--------: | :------: |
|  `~`   |  not (¬)   |    1     |
|  `&`   |  and (∧)   |    2     |
|  `|`   |   or (∨)   |    2     |
|  `-`   |   if (→)   |    3     |
|  `=`   |  iff (↔)   |    3     |

Priority can be modified using parantheses `(`, `)`.

## Examples

Input format:

### 1. Satisfiability of a Formula

```
$ tableaux-sat
~(p | q) = (~p & ~q)
```

```
SATISFIABLE
p = 1
```

### 2. Satisfiability of a Theory

```
$ tableaux-sat -T
s
q - p
r - q
(r - p) - s
```

```
UNSATISFIABLE
```

### 3. Dot Language Output

```
$ tableaux-sat -T --dot
p
~q
p | q
```

```
TODO
```

Image generated from the output by [graphviz](https://www.graphviz.org/):

<img src="examples/example-1.png" width="200" alt="Example 1" />

Note that nodes on contradictory branches are marked by an asterisk.
