# The Simple Virtual Machine

A program is a linear sequence of simple instructions.  Instructions
are numbered from `0` upwards.

The machine's state is

* The **program counter**, which is the index of the next instruction
  to be executed.  This is `0` at the start of the program.

* The **call stack**, which contains instruction addresses, and is
  used for function call and return instructions.  It is empty at the
  start of the program.

* The **value array**, which is an array of 64 bit integers, at least
  one million slots in length.  Computation instructions operate on
  individual slots in this array.  At the start of the program the
  first few slots contain the arguments to the program, and the rest
  of the slots are initialized to `0`.

* The **value stack pointer**, which is an index into the value array.
  Computation instructions don't specify indexes into the value array
  directly, but as offsets to the value stack pointer.  We'll see
  shortly how that works.  At the start of the program, the value
  stack pointer is set to the number of arguments to the program.

Let's say we run a program with the arguments `123`, `456`, and `789`.
Before the first instruction is executed, the program counter will be
`0`, and the value array will look like this:

    Index      Array

      ...     |  ... |
       4      |    0 |
       3      |    0 |    <--- value stack pointer
       2      |  789 |
       1      |  456 |
       0      |  123 |
              +------+

Note that in this diagram, the array "starts" at the bottom and grows
upwards, like a stack.  The value stack pointer is `3`, because that's
the number of arguments to the program.

In our first instruction we want to add the first and second arguments
to the program, and put the result at the top of our "stack".  This is
the instruction to do that:

     0    Add $0, $-3, $-2

The first number, `0`, is simply the index of the instruction.  It's
the first instruction in our program, so it's `0`.  The `Add`
instructions takes three arguments: The (relative) indexes of the
slots for the result of the addition, and of the two operands.  Since
the value array is always indexed relative to the value stack pointer,
which is `3`, the slot `$0` is actually at absolute index `3` (`3+0`).
The two operands, `$-3` and `$-2` are at absolute indexes `0` (`3+-3`)
and `1` (`3+-2`).  The slots at those absolute indexes, `0` and `1`,
contain the first two arguments to our program, `123` and `456`, so
the `Add` instruction adds those two numbers and writes the result
`579` into slot `$0` (absolute index `3`), giving us this array:

    Relative
	 Index      Array

      ...     |  ... |
       $1     |    0 |
       $0     |  579 |    <--- value stack pointer
       $-1    |  789 |
       $-2    |  456 |
       $-3    |  123 |
              +------+

Since the `Add` instruction doesn't modify control flow, the program
counter is incremented and is now `1`.  Our next instruction adds the
third program argument:

     1    Add $0, $-1, $0

Note that here the result slot is the same as one of the operand
slots.  The result of adding `$-1`, which is `789`, and `$0`, which is
`579`, is written into `$0`:

      ...     |  ... |
       $1     |    0 |
       $0     | 1368 |    <--- value stack pointer
       $-1    |  789 |
       $-2    |  456 |
       $-3    |  123 |
              +------+

The program counter is now `2`.  Let's say we want to add the number
`10` to that intermediate result.  The `Add` instruction only allows
us to use slots, so we have to get `10` into a slot first.  The `Set`
instruction does that:

     2    Set $1, 10

Now we have

      ...     |  ... |
	   $2     |    0 |
       $1     |   10 |
       $0     | 1368 |    <--- value stack pointer
       $-1    |  789 |
       $-2    |  456 |
       $-3    |  123 |
              +------+

And the program counter is `3`.  We can add `$0` and `$1` now:

     3    Add $0, $0, $1

and get

      ...     |  ... |
	   $2     |    0 |
       $1     |   10 |
       $0     | 1378 |    <--- value stack pointer
       $-1    |  789 |
       $-2    |  456 |
       $-3    |  123 |
              +------+

Let's write a function that takes a number and multiplies it by `5`.
First we have to talk about how arguments are passed to functions,
which is where the value stack pointer comes in.  From the perspective
of function, its arguments are at negative offsets from the value
stack pointer.  A function taking two arguments, for example, would
find those arguments in `$-2` and `$-1`.  Our function only takes one
argument, so it will be in `$-1`.  Everything at non-negative offsets
from the value stack pointer "belongs" to the function, and it can use
it for whatever purpose.  The function can not assume that those stack
slots are initialized to zero, however.

This is how our function will achieve its task: First we will set `$0`
to `5`, then we will multiply the function argument, `$-1`, with `$0`,
and store the result back to `$0`, because we don't need `5` anymore:

    12    Set $0, 5
    13    Multiply $0, $-1, $0

The last thing we're missing is how to return the result, in `$0`,
back to whoever called our function.  That's what the `Return`
instruction is for:

    14    Return $0

To finish off, let's write a loop that will call this function four
times on the result we got from instruction `3`, above.  On the last
stack diagram we see that our intermediate result is in `$0`.  We
won't need the `10` in `$1` anymore, so we can overwrite it if we have
to.  We will need to keep the loop count in a stack slot, but we can't
use `$1` for that: we saw that when a function is called, everything
above the arguments it is passed must be assumed to be overwritten by
the function.  If we use `$0` as the function argument, and use `$1`
for the loop counter, the loop counter will be overwritten every time
we call the function, which is obviously not what we want.  So we'll
use `$0` as the loop counter, and `$1` as the intermediate result,
which is also the function argument.  It follows that we need to move
the number in `$0` to `$1`, and then set `$0` to the loop counter,
`4`:

     4    Move $1, $0
     5    Set $0, 4

Here's what our stack looks like now:

      ...     |  ... |
	   $2     |    0 |
       $1     | 1378 |
       $0     |    4 |    <--- value stack pointer
       $-1    |  789 |
       $-2    |  456 |
       $-3    |  123 |
              +------+

Now we can call our function:

     6    Call 12, 2, $1

The `Call` instruction takes three arguments:

* The index of the first instruction of the function to call, in our
  case `12`.

* The number to be added to the value stack pointer.  This is how `$1`
  from the caller's perspective ends up as `$-1` from the called
  function's perspective, because we pass `2`.

* The slot where the result of the function should be stored in.

The immediate effects of the `Call` instruction are that it first
pushes the current program counter, `6`, onto the call stack.  Then it
sets the program counter to the called function's instruction index,
`12`.  Finally, it adds an offset, `2`, to the value stack pointer.
The contents of the value array are still the same, but the value
stack pointer has moved up two slots:

      ...     |  ... |
	   $0     |    0 |    <--- value stack pointer
       $-1    | 1378 |
       $-2    |    4 |
       $-3    |  789 |
       $-4    |  456 |
       $-5    |  123 |
              +------+

This is where our function executes now.  It finds its argument,
`1378`, in slot `$-1`.  When it finishes, it will return that number
times `5`, i.e., `6890`.  The `Call` instruction specifies that the
function result should be stored in slot `$1` (from the caller's
perspective), so that is what happens:

      ...     |  ... |
	   $3     |  ??? |
	   $2     |  ??? |
       $1     | 6890 |
       $0     |    4 |    <--- value stack pointer
       $-1    |  789 |
       $-2    |  456 |
       $-3    |  123 |
              +------+

Note that the value stack pointer is back where it was before the
call.  Note also that the stack slots above `$1` are now marked with
`???`.  This signifies that the caller must not assume that they
contain the same values as they did before the call, or any useful
value at all, for that matter.  That is part of the convention between
caller and called function (callee).  The `Return` instruction knows
where to return to, how to restore the value stack pointer, and which
slot to put the result in, because it pops off the corresponding
`Call` instruction's index from the call stack.

Now we need to decrement the loop counter and, if it's not zero,
repeat the loop.  We decrement by adding `-1`, which we first have to
store somewhere.  We'll use `$2`:

     7    Set $2, -1
     8    Add $0, $0, $2

Now, if `$0` is not zero, we must repeat the loop, i.e., jump back to
instruction `6`.  We don't have an instruction to jump if not zero,
but we have the instructions `JumpIfZero`, and `Jump`, which we can
employ:

     9    JumpIfZero $0, 11
    10    Jump 6

If `$0` is zero, we jump to instruction `11`.  If `$0` is not zero we
continue straight down to `10`, which jumps back to `6`, which is what
we want.  We end up in `11` when our loop is finished, at which point
we want to end the program and return the result that's in `$1`:

    11    Return $1

Note that there was never a `Call` corresponding to this `Return`.
When the "main" program returns, the program ends.

## Instructions

### Moving data

#### `Move` *DST* *SRC*

Copies the number in slot *SRC* to slot *DST*.  If *SRC* and *DST* are
the same slot, there is no effect.

#### `Set` *DST* *NUMBER*

Sets slot *DST* to *NUMBER*.

### Arithmetic

#### `Add` *DST* *SRC1* *SRC2*

Sets the slot *DST* to the sum of the numbers in slots *SRC1* and
*SRC2*.

#### `Multiply` *DST* *SRC1* *SRC2*

Sets the slot *DST* to the product of the numbers in slots *SRC1* and
*SRC2*.

#### `Negate` *DST* *SRC*

Sets the slot *DST* to the arithmetic negation of the number in slot
*SRC*.

#### `Not` *DST* *SRC*

If the number in slot *SRC* is `0`, sets the slot *DST* to `1`,
otherwise sets the slot *DST* to `0`.

### Control flow

#### `Jump` *INS*

Continues execution at the instruction with index *INS*.

#### `JumpIfZero` *SRC* *INS*

If the number in *SLOT* is zero, continues execution at the
instruction with index *INS*.  Otherwise continues execution regularly
at the next instruction.

#### `Call` *INS* *NUMBER* *DST*

* Pushes the program counter (the index of the `Call` instruction) onto
the call stack.

* Adds *NUMBER* to the value stack pointer.

* Transfers control to the instruction with index *INS*.

The slot *DST* is used by the corresponding `Return` instruction to
return the result.

#### `Return` *SRC*

* Reads the result from slot *SRC*.

* If the call stack is empty, end the program and produce that result.
Otherwise:

* Pops the index of the corresponding `Call` instruction off the call
stack.

* Decrements the value stack pointer by the *NUMBER* given in that
instruction

* Writes the result to the slot *DST* given in that instruction (using
the new, decremented value stack pointer).

* Transfers control to the instruction after the `Call` instruction
(i.e. with the `Call` instruction's index plus one).

### Comparison

#### `LessThan` *DST* *SRC1* *SRC2*

If the number in slot *SRC1* is less than the number in slot *SRC2*,
sets the slot *DST* to `1`, otherwise to `0`.

#### `Equals` *DST* *SRC1* *SRC2*

If the number in slot *SRC1* is the same as the one in *SRC2*, sets
the slot *DST* to `1`, otherwise to `0`.

## Syntax

Each line is an instruction.  Within the line, white space and commas
act as separators.  The first element in each line is the line number,
which must be ignored (it's only there to make reading VM code
easier).  The second line is the name of the instruction, then follow
the arguments.  Numbers are sequences of digits, optionally prefixed
by a minus sign.  Slots are numbers prefixed by a dollar sign.
Instruction indexes are numbers.
