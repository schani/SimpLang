# Assignment 1

Write a simple interpreter that interprets off the syntax tree.  Don't
optimize this interpreter - we will write much faster interpreters in
later exercises.

The interpreter is only expected to handle correct inputs correctly.
Incorrect programs may lead to arbitrary results, including crashes.

## Assignment 1.1

Write a lexical analyzer (also called "scanner" or "tokenizer").  It
reads a file as input and outputs one line per token.  All whitespace
in the input file is ignored, apart from when it separates tokens that
would otherwise be a single token.

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
output directly while parsing.  I recommend writing a
[recursive descent parser](https://en.wikipedia.org/wiki/Recursive_descent_parser).

## Assignment 1.3

Write an interpreter for the expressions with integers and `if` from
the previous assignment.  An integer evaluates to its value.  An `if`
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

You'll have an easy time if you write the interpreter as a recursive
function.

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

## Assignment 1.5

Extend the interpreter to handle the operators.  Note that all values
are 64 bit signed integers, using two's complement arithmetic.
Overflow is not handled, i.e. if it occurs, the result is whatever
fits into 64 bits.  In Python this can be implemented easily by using
the `numpy.int64` type.

Here's how the operators work:

* `!`_x_ is `1` if _x_ is `0`.  It's `0` if _x_ is not `0`.

* `-`_x_ is the arithmetic negation of _x_.

* _x_`&&`_y_ is `0` if either _x_ or _y_ are `0`, otherwise `1`.  Note that we will later need
  [short-circuit evaluation](https://en.wikipedia.org/wiki/Short-circuit_evaluation),
  so if _x_ is `0`, don't evaluate _y_.

* _x_`||`_y_ is `0` if both _x_ and _y_ are `0`, otherwise `1`.
  Short-circuit evaluation applies here, too.

* _x_`<`_y_ is `1` if _x_ is less than _y_, otherwise `0`.

* _x_`==`_y_ is `1` if _x_ is equal to _y_, otherwise `0`.

* _x_`+`_y_ is the sum of _x_ and _y_.

* _x_`*`_y_ is the product of _x_ and _y_.

Input:

    (1+2)

Output:

	3

Input:

	(1+-2)

Output:

	-1

Input:

    (9223372036854775807+1)

Output:

    -9223372036854775808

Input:

	((9223372036854775807+1)*2)

Output:

	0

Input:

	((1 && 2) + (0 || 3))

Output:

	2

Input:

    !!123

Output:

	1

## Assignment 1.6

Extend the parser to parse `let` constructs, and to allow identifiers
as expressions.  The grammar is now:

	expr = integer
		 | ident
         | "if" expr "then" expr "else" expr "end"
		 | "let" bindings "in" expr "end"
		 | unop expr
		 | "(" expr binop expr ")"
	bindings = binding {"and" binding}
	binding = ident "=" expr
    unop = "!" | "-"
	binop = "&&" | "||" | "<" | "==" | "+" | "*"

The curly braces in the grammar mean that the enclosed sequence can be
repeated zero or more times, so `x {"and" x}` means `x` or `x "and" x`
or `x "and" x "and" x`, and so on.

Input:

    let a = 1 and
        b = (a + 1)
	in
	    (a + b)
	end

Output:

    let
        a
          1
        b
          +
            a
            1
      +
        a
        b

## Assignment 1.7

Extend the interpreter to handle `let`.

Input:

    let a = 1 and
        b = (a + 1)
	in
	    (a + b)
	end

Output:

	3

Input:

	-let a = 10
	in
		(let a = 1 and
			a = (a + 1)
		in
			a
		end + a)
	end

Output:

	-12

Note that in this example there are three different variables with the
same name, `a`, each with a different scope.  We can rename them to
get an expression with the same semantics, to make clear which one is
which:

	-let a1 = 10
	in
		(let a2 = 1 and
			a3 = (a2 + 1)
		in
			a3
		end + a1)
	end

Your recursive interpretation function will now require not only the
AST to evaluate, but also the "environment" in which to evaluate it
in.  The environment contains the variable bindings.  For example, the
expression `(a+b)` evalutes to `3` in the environment `{a=>1, b=>2}`,
but to `7` in the environment `{a=>3,b=>4}`.  Think about what data
structure to use for the environment before starting to code.  It will
have to be able to handle the introduction of new variables with the
same name as existing ones, without overwriting the old values,
because they might be needed later, like in the example above.

## Assignment 1.8

Extend the parser to recognize `loop` and `recur`.  The grammar is
now:

	expr = integer
		 | ident
         | "if" expr "then" expr "else" expr "end"
		 | ("let" | "loop") bindings "in" expr "end"
		 | "recur" arg {arg}
		 | unop expr
		 | "(" expr binop expr ")"
	bindings = binding {"and" binding}
	binding = ident "=" expr
	arg = "(" expr ")"
    unop = "!" | "-"
	binop = "&&" | "||" | "<" | "==" | "+" | "*"

Input:

    loop x=1 in recur (x) end

Output:

    loop
	    x
		  1
	  recur
	    x

## Assignment 1.9

Extend the interpreter to handle `loop` and `recur`.  Note that
running a loop must not consume more memory the more often it recurs.
For example:

    loop a = 100 and
		 b = 0
	in
	    if (a == 0) then
	        b
		else
			recur ((a+-1)) ((b+1))
		end
	end

gives

    100

If instead `a` is initialized with `1000000` it must not use more
memory than it does with `100`.

The easiest way to achieve this, with a recursive interpreter, is to
let the interpretation function return either a result, or a value
that indicates that the innermost `loop` is to be rerun, including the
new values to be bound.  If your programming language supports
exceptions, use can also use them instead.

See the [README](README.md) on details of which positions `recur` can
be used in.

Input:

    loop n = 10 and
         fac = 1
	in
		if (n == 1) then
			fac
		else
			recur ((n+-1)) ((fac*n))
		end
	end

Output:

	3628800

The double parentheses look a little weird here, but they're necessary
because our grammar is still a bit simplified.  We'll fix that in the
next assignment.

Make sure that you really handle short-circuit evaluation of `&&` and
`||`.  This expression:

    (0 && loop x=1 in recur (x) end)

must produce `0`, and not loop infinitely.  The same goes for `if`:

    if 1 then 1 else loop x=1 in recur (x) end end

must produce `1`.

## Assignment 1.10

Implement operator precendence in the parser.  The grammar changes a
little to:

	expr = primary {binop primary}
	primary = integer
	        | ident
            | "if" expr "then" expr "else" expr "end"
		    | ("let" | "loop") bindings "in" expr "end"
		    | "recur" arg {arg}
		    | unop primary
	        | "(" expr ")"
	bindings = binding {"and" binding}
	binding = ident "=" expr
	arg = "(" expr ")"
    unop = "!" | "-"
	binop = "&&" | "||" | "<" | "==" | "+" | "*"

Now when you parse an expression you can end up with a list of
primaries, separated by binary operators, and you'll have to build a
tree out of that, according to operator precendence.  Here are all the
binary operators, in increasing order of precedence:

	&&, ||
	<, ==
	+
	*

All operators associate left to right, i.e. `a<b<c` is equivalent to
`(a<b)<c`, as opposed to `a<(b<c)`.  Precendence means that `a+b*c` is
equivalent to `a+(b*c)`, i.e. `*` has higher "priority".  One way to
implement precendence is the
[shunting yard algorithm](https://en.wikipedia.org/wiki/Shunting-yard_algorithm),
but there is a more straightforward way you might discover.

Make sure your interpreter works with the parser changes.  Depending
on how you designed your AST, you might not have to change it at all
to make it work with this improved parser.

We can now write the factorial implementation from the last assignment
without the double parentheses:

    loop n = 10 and
         fac = 1
	in
		if n == 1 then
			fac
		else
			recur (n+-1) (fac*n)
		end
	end

## Assignment 1.11

Extend the parser to recognize a single function.  We're adding these
rules to our grammar:

    function = "let" ident params "=" expr "end"
    params = ident {ident}

The top-level rule is now `function`, so we can parse

    let main n =
		loop n = n and
			 fac = 1
		in
			if n == 1 then
				fac
			else
				recur (n+-1) (fac*n)
			end
	    end
	end

Since the top-level rule is `function` now, all the examples before
this assignment won't work anymore (because they're not contained
within a function).  That's ok.  If you'd like to still be able to use
those examples, you should preserve the ability to parse an `expr`
that's not contained in a function.

Input:

    let main a b =
	    a + b
    end

Output:

    function
	    main
	      a
		  b
      +
	    a
		b

## Assignment 1.12

Make your interpreter run the `main` function and pass command line
arguments in as the arguments to `main`.  If you run

    python my-interpreter.py examples/add.sl 1 2

it should output

	3

To run the function all you should have to do is to make an
environment that contains bindings for all the arguments, and then
evaluate the body with that environment.

If you want your interpreter to also run function-less expressions,
like all the examples before assignment 1.11, you could do it by
parsing an `expr` if no function arguments are given on the command
line.

## Assignment 1.13

We are missing one last thing, and that's being able to define
multiple functions and to call them.  Extend your parser to the final
grammar:

	program = function {function}
    function = "let" ident params "=" expr "end"
    params = ident {ident}
	expr = primary {binop primary}
	primary = integer
	        | ident
            | "if" expr "then" expr "else" expr "end"
		    | ("let" | "loop") bindings "in" expr "end"
		    | ("recur" | ident) arg {arg}
		    | unop primary
	        | "(" expr ")"
	bindings = binding {"and" binding}
	binding = ident "=" expr
	arg = "(" expr ")"
    unop = "!" | "-"
	binop = "&&" | "||" | "<" | "==" | "+" | "*"

Input:

	let add a b =
	  a + b
	end

	let main a b =
	  add (a) (b)
	end

Output:

	function
		bitset
		  x
		  i
	  loop
		  x
			x
		  i
			i
		if
		  <
			i
			63
		  recur
			*
			  x
			  2
			+
			  i
			  1
		  <
			x
			0
	function
		main
		  x
		  i
	  bitset
		x
		i

## Assignment 1.14

Extend the interpreter to correctly process function calls.  In
assignment 1.12 you already wrote code to call a function.  All you
need to do now, when a function is to be called, is to find out which
one it is, via its name.

Your interpreter should now be able to run all the programs in the
`examples` folder.  Congratulations, you've completed assignment 1!

# Assignment 2

Write an interpreter for the virtual machine.

## Assignment 2.1

Identify which operations you need to implement for the value array
and the value stack pointer: go through all VM instructions and check
how they operate with the value array and the value stack pointer.

Implement a value array and value stack pointer that support those
operations.  Write a few simple tests to make sure they work.

## Assignment 2.2

Implement a simple program that takes as command line arguments one or
more numbers, puts them on the value stack array, sets the value stack
pointer accordingly (i.e. to point to the slot after the numbers, like
in the VM description), and then does the effect of the instruction

    0    Add $0, $-3, $-2

using the operations implemented in previous assignment.  Note that
you don't have to parse the instruction, just write Python code that
operates on the value array like that instruction would.  Then write
code that checks that the value array contains the correct values.

## Assignment 2.3

Write a parser for VM files.  See the syntax section and the example
code in the VM documentation.  Figure out a representation that will
make interpreting easy.

Here's how you can split an instruction line into its components:

    import re
    re.split("[\\s,]+", line.strip())

## Assignment 2.4

Implement an interpreter for the instructions `Add` and `Set`.  Also
implement `Return`, but make it end the program right away and print
the result, i.e. don't worry about the call stack yet.

Parse and interpret this program:

    0    Add $0, $-3, $-2
    1    Add $0, $-1, $0
    2    Set $1, 10
    3    Add $0, $0, $1
	4    Return $0

It takes three arguments.  If you give it the numbers `123`, `456`,
and `789`, the result it prints should be `1378`.

## Assignment 2.5

Implement all the other instructions that only operate on the value
array: `Move`, `Multiply`, `Negate`, `Not`, `LessThan`, `Equals`.
Write one or more test programs to check that they work.

## Assignment 2.6

Implement `Jump` and `JumpIfZero`.  This program takes one argument
and should return the argument's factorial:

    0   Set $0, 1
    1   Set $1, 2
    2   LessThan $2, $-1, $1
    3   JumpIfZero $2, 5
    4   Return $0
    5   Multiply  $0, $0, $1
    6   Set $3, 1
    7   Add $1, $1, $3
    8   Jump 2

## Assignment 2.7

Implement `Call` and `Return`, with a call stack.  Most of the work is
done by the `Return` instruction, which also has to look at the `Call`
instruction it returns to, to determine how much it has to adjust the
value stack pointer, and which slot to store the result in.

Your VM is now complete.  Try it out with
`examples/nextprime-simple.sbc`.  It takes one arguments and produces
the next prime number after that number.

# Assignment 3

Write a compiler for Simplang that generates code for the virtual
machine.

There are many ways to compile a Simplang program to VM code, so we
cannot specify a required output anymore.  The generated VM code must,
when executed on the VM, produce the same results as the Simplang
program, given the same inputs.

## Assignment 3.1

Write a compiler for programs that consist of only a `main` function,
and which returns either an integer literal or one of the arguments.

For examples:

    let main x y z =
	    123
	end

and

	let main x y z =
	    y
	end

## Assignment 3.2

Extend the compiler to handle expressions containing the operators
`!`, `<`, `==`, `+`, `*`, and `-`.

## Assignment 3.3

Implement compilation of expressions containing the operators `&&` and
`||`.  Note that these require short-circuit evaluation, i.e., if the
result is already determined once the left operand has been evaluated,
the right operand must not be evaluated.

## Assignment 3.4

Handle `if` expressions in the compiler.

## Assignment 3.5

Implement `let` expressions in the compiler.

## Assignment 3.6

Implement `loop` and `recur`.

## Assignment 3.7

Implement calling other functions.

The compiler is now complete.  Make sure it works on all the programs
in `examples/`.

# Assignment 4

Write a compiler for Simplang that generates x86-64 assembly code.
