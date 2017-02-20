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

if len (sys.argv) < 3:
    sys.stderr.write ("Usage: %s TEST-DIR EXECUTABLE [EXECUTABLE-FLAGS]\n" % sys.argv [0])
    exit (1)

num_tests = 0
failed_tests = 0

tests_dir = os.path.realpath (os.path.dirname (__file__))

test_dir = os.path.join (tests_dir, sys.argv [1])
exe = sys.argv [2]
flags = sys.argv [3:]

def run_exe(sl, args):
    return subprocess.check_output ([exe] + flags + [sl] + [str (x) for x in args], universal_newlines=True)

for fn in [os.path.join (test_dir, n) for n in os.listdir (test_dir)]:
    if not os.path.isfile (fn):
        continue
    is_outfile = False
    if fn.endswith ('.tests'):
        fn_base = os.path.basename (fn) [:-6]
    elif fn.endswith ('.out'):
        fn_base = os.path.basename (fn) [:-4]
        is_outfile = True
    else:
        continue
    sl_name = '%s.sl' % fn_base
    sl_path = os.path.join (test_dir, sl_name)
    if not os.path.isfile (sl_path):
        sl_path = os.path.join (tests_dir, '..', 'examples', sl_name)
        if not os.path.isfile (sl_path):
            print("Error: Cannot find SL file for test %s" % fn_base)
    print(fn_base)
    if is_outfile:
        with open(fn, 'r') as f:
            expected = f.read()
        num_tests += 1
        result = run_exe(sl_path, [])
        if result != expected:
            print("Failure on %s: expected \n\n%s\n\ngot\n\n%s\n" % (fn_base, expected, result))
            failed_tests += 1
    else:
        with open (fn, 'r') as f:
            while True:
                test = readtest (f)
                if not test:
                    break
                num_tests += 1
                (args, expected) = test
                print(args, expected)
                result = int (run_exe(sl_path, args))
                if result != expected:
                    print("Failure on %s with %s: expected %d, got %d" % (fn_base, args, expected, result))
                    failed_tests += 1

print("Ran %d tests - %d failures" % (num_tests, failed_tests))
if failed_tests > 0:
    exit (1)
exit (0)
