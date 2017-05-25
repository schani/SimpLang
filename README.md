SimpLang - A Simple Language
============================

All values are signed 64 bit integers.  All global definitions are
functions.  Calls are not tail recursive.  All functions return
values.  Functions are not first class.

Some pieces in the language grammar, like `end`, are superfluous.
They are there to make parsing and later extension easier.

# Example program

	let fac n =
		loop acc = 1 and
			 i = 2
		in
			if n < i then
        		acc
      		else
				recur (acc * i) (i + 1)
			end
		end
	end

	let main n =
		fac (n)
	end

# Control constructs

`let` introduces variable bindings.  They are only visible within the
right hand sides of later bindings in the same `let` expression and
the body of the `let` expression.  Later bindings can hide earlier
ones:

    let a = 1 and
		b = a + 1
	in
		b
	end

gives `2`.

	let a = 31415
	in
		let a = 1 and
			a = a + 1
		in
			a
		end
	end

also gives `2`, because the first `a` in the inner `let` hides the
outer `a`.

`if` works as expected.  It must have both `then` and `else` clauses.

`loop` introduces variable bindings like `let`, but it also provides a
jump target for uses of `recur` in the body.  `recur` invocations jump
to the innermost `loop` containing that `recur`, giving new values to
the bound variables.

`recur` is only legal within a `loop` and can only occur in a tail
position, i.e., where the continuation is the same as the continuation
of the containing `loop`.  In particular, a `recur` can never be the
condition of an `if`, an argument in a function call, an operand to an
operator, the right hand side of a `let` or `loop` binding.

Functions can only be used in their own definitions or in later
definitions.  All functions must have different names.

All arguments to function calls and `recur` must be parenthesized,
i.e.,

    recur (a) (-b)

is legal, while

    recur a -b

is not.

# Values

`if` and all logical operators consider `0` to be false, all other
values to be true.  Logical operators return `0` for false and `1` for
true.

# Operators

These are all the operators, in increasing order of precedence:

	&&, ||
	!
	<, ==
	+
	*
	- (unary)
	function application

`&&` and `||` are logical operators, using shortcut evaluation.  That
means `&&` will not evaluate its right hand side if the left hand side
evaluates to `0`, and `||` will only evaluate its right hand side if
its left hand side evaluates to `0`.

# Miscellaneous

Functions must take at least one argument.

# Syntax

Identifiers can contain only letters, digits, and the underscore, but
cannot start with a digit.

Function calls use ML/Haskell syntax: Function name followed by
arguments.

# Tests

Run the testsuite with

    ./tests/test.py TEST-SUITE YOUR-EXE [YOUR-EXE-ARG ...]

where `TEST-SUITE` is the name of one of the directories in `tests`,
such as `full`.
