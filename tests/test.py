#!/usr/bin/python

import sys
import os
import subprocess

def readnumbers (f):
    while True:
        l = f.readline ()
        if not l:
            break
        l = l.rstrip ()
        if l == '':
            continue
        return [int (c) for c in l.split ()]
    return None

def readtest (f):
    args = readnumbers (f)
    if not args:
        return None
    result = readnumbers (f) [0]
    return (args, result)

if len (sys.argv) < 2:
    sys.stderr.write ("Usage: %s EXE-FILE [ARGS]\n" % sys.argv [0])
    exit (1)

num_tests = 0
failed_tests = 0

exe = sys.argv [1]
flags = sys.argv [2:]
for fn in os.listdir ('.'):
    if not os.path.isfile (fn):
        continue
    fn = os.path.basename (fn)
    if not fn.endswith ('.tests'):
        continue
    slname = '../examples/%s.sl' % (fn [:-6])
    print fn
    f = open (fn, 'r')
    while True:
        test = readtest (f)
        if not test:
            break
        num_tests += 1
        (args, expected) = test
        print args, expected
        result = int (subprocess.check_output (['mono', exe] + flags + [slname] + [str (x) for x in args]))
        if result != expected:
            print "Failure on %s with %s: expected %d, got %d" % (fn, args, expected, result)
            failed_tests += 1
    f.close ()

print "Ran %d tests - %d failures" % (num_tests, failed_tests)
if failed_tests > 0:
    exit (1)
exit (0)
