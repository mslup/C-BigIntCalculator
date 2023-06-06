# Calculator in C 

This repository contains a calculator program written in C, developed in 2021 as part of the Structural Programming course at the MiNI WUT Faculty. Since the project was written during the first semester of the studies, its quality in certain places may be less than ideal. 

## Description

The calculator is designed to handle operations on integer numbers, supporting calculations with numbers of up to a hundred thousand digits. The program allows numbers to be expressed in an arbitrary base within the range of 2 to 16.

The calculator supports the following operations:
- addition
- subtraction
- multiplication
- division
- modulo
- exponentiation
- base conversion

## Input and output

The program accepts input data from file named `in.txt` and yields the results in file `out.txt`.

The data in the input file should be expressed in the following format. (Newlines are required.)

### Arithmetic operations
```
[operation] [base]

[number 1]

[number 2]


```
where `operation` is one of the following: `+`, `*`, `/`, `%`, `^`. Both numbers should be represented in the same number system, specified by `base`.

### Base conversion
```
[base_in] [base_out]

[number]


```

The output file is a copy of the input file, with the result of an operation (or an error message) placed after the arguments.

### Examples

#### Valid operation
```
/ 10

9223372036854775807

2147483647
```
```
/ 10

9223372036854775807

2147483647

4294967298
```

#### Error reporting
```
/ 11

A

0
```
```
/ 11

A

0

division_by_zero
```

## Example console output
![ccalculator](https://github.com/mslup/C-BigIntCalculator/assets/132074948/4894ef10-bd24-475b-90da-2f017d934b13)
