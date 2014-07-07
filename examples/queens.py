#!/usr/bin/python

import sys

assert len (sys.argv) == 3

n = int (sys.argv [1])
board = int (sys.argv [2])


def queen (x):
    return (board >> (x * 4)) & 15


def hline ():
    return '---'.join (['+' for x in range (n + 1)])

for y in range (n):
    print hline ()
    print '+' + '+'.join ([(queen (x) == y and ' Q ') or '   ' for x in range (n)]) + '+'
print hline ()
