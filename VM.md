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

      ...     | ... |
       4      |   0 |
       3      |   0 |    <--- value stack pointer
       2      | 789 |
       1      | 456 |
       0      | 123 |
              +-----+

Note that in this diagram, the array "starts" at the bottom and grows
upwards, like a stack.  The value stack pointer is `3`, because that's
the number of arguments to the program.

In our first instruction we want to add the first and second arguments
to the program, and put the result at the top of our "stack".  This is
the instruction to do that:

    0    Add   $0, $-3, $-2

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

      ...     | ... |
       4      |   0 |
       3      | 579 |    <--- value stack pointer
       2      | 789 |
       1      | 456 |
       0      | 123 |
              +-----+
