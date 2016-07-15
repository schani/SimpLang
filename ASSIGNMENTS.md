# Assignment 1

Write a simple interpreter that interprets off the syntax tree.  Don't
optimize this interpreter - we will write much faster interpreters in
later exercises.

The interpreter is only expected to handle correct inputs correctly.
Incorrect programs may lead to arbitrary results, including crashes.

## Assignment 1.1

Write a lexical analyzer.  It reads a file as input and outputs one
line per token.  All whitespace in the input file is ignored, apart
from separating tokens that would otherwise be single tokens.

Each line in the output has two parts, separated by a space:

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
		loopy = a+-1
	in
		loopy
	end

Expected output:

    keyword let
	identifier a
	operator =
	integer 1
	keyword and
	identifier loopy
	operator =
	identifier a
	operator +
	operator -
	integer 1
	keyword in
	identifier loopy
	keyword end

Note that tokens need not be separated by spaces, like `a+-1` in the
example above, which is four separate tokens.  Also note that
identifiers can "contain" keywords, like `loopy` above, which is not
the keyword `loop` and the identifier `y`, but just one identifier.

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

## Assignment 1.3

Write an interpreter for the integer and `if` expressions from
assignment 1.2.  An integer evaluates to its value.  An `if`
expression evaluates to its `else` expression if its condition
evaluates to zero, otherwise to its `then` expression.

Input:

    123

Output:

    123

Input:

	if 8 then 1 else 2 end

Output:

    1

Input:

	if 0 then 1 else 2 end

Output:

	2

Input:

	if
	  if 1 then 0 else 1 end
    then
	  123
    else
	  if 8 then 3 else 4 end
    end

Output:

	3

## Assignment 1.4

Extend the parser to recognize operators, without yet implementing
operator precedence.  For now we require that binary operator
expressions are always parenthesized.  The grammar is now:

    expr = integer
         | "if" expr "then" expr "else" expr "end"
		 | unop expr
		 | "(" expr binop expr ")"
    unop = "!" | "-"
	binop = "&&" | "||" | "<" | "==" | "+" | "*"

Input:

    if (1<2) then (3*4) else (5+!-if 7 then 8 else 9 end) end

Output:

    if
	  <
	    1
		2
	  *
	    3
		4
	  +
	    5
		!
		  -
		    if
		      7
			  8
			  9

Again, parse to an abstract syntax tree first, then generate the
output from the AST.  That will make the next assignment easier.
