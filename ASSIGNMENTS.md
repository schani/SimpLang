# Assignment 1

Write a simple interpreter that interprets off the syntax tree.  Don't
optimize this interpreter - we will write much faster interpreters in
later exercises.

The interpreter is only expected to handle correct inputs correctly.
Incorrect programs may lead to arbitrary results, including crashes.

## Assignment 1.1

Write a lexical analyzer.  It reads a file as input and outputs one
line per token.  All whitespace in the input file is ignored.  Each
line in the output has two parts, separated by a space:

1. The type of token.  This can be `keyword`, `identifier`,
   `operator`, or `integer`.

2. The token itself.

Keywords are `let`, `and`, `in`, `if`, `then`, `else`, `recur`,
`loop`, `end`.  Operators are `(`, `)`, `=`, `&&`, `||`, `!`, `<`,
`==`, `+`, `*`, `-`.  Identifiers can contain only letters, digits,
and the underscore, but cannot start with a digit.  Integers are
sequences of digits.

The analyzer is not expected to handle incorrect input.

Example input:

    let a = 1 and
		b = a+-1
	in
		b
	end

Expected output:

    keyword let
	identifier a
	operator =
	integer 1
	keyword and
	identifier b
	operator =
	identifier a
	operator +
	operator -
	integer 1
	keyword in
	identifier b
	keyword end

More example inputs are in the `examples` folder.

## Assignment 1.2

Write a parser for `if` expressions.  The grammar to be recognized is
this (in [BNF](https://en.wikipedia.org/wiki/Extended_Backus–Naur_Form)):

    expr = integer
         | "if" expr "then" expr "else" expr "end"


The output must show the structure of the input via indentation.  A
few examples:

Input:

    123

Output:

    123

Input:

    if 1 then 2 else 3 end

Output:

    if
      1
	  2
	  3

Input:

    if if 1 then 2 else 3 end then 4 else if 5 then 6 else 7 end end

Output:

    if
      if
	    1
	    2
		3
      4
	  if
	    5
	    6
		7

To work towards an interpreter and compiler it is beneficial to first
parse the input into an intermediate data structure (an
[abstract syntax tree](https://en.wikipedia.org/wiki/Abstract_syntax_tree)),
and then generate the output from that tree, instead of generating the
output directly while parsing.
