#!/usr/bin/env python

from __future__ import print_function

import os
from itertools import islice
#import math
#import fileinput
#import re
#import numpy
#from xml.dom.minidom import parseString
#from xml.dom.minidom import Document

#try:
#  import vtktools
#except:
#  pass

#def parse_s(str):
#    """Parse a .s file. Makes a dict vals that you use like:
#    vals['maxp'][0] etc."""
#
#    vars = []
#    vals = {}
#    fieldsize = 13 # how big is each number in the list?
#    fields_per_line = 12 # how many variables printed out per line
#    no_lines = 1 # how many lines correspond to each timestep
#    if os.stat(str)[6] == 0:
#      raise Exception("Error: %s must not be empty!" % str)
#
#    f = open(str, "r")
#    for line in f:
#        if line.startswith("@@"):
#            var_count = int(line[2:])
#    f.close()
#
#    # OK. Fix for bizarre behaviour when compiled with Sun compiler.
#    # I'm not going near study.F to try to find what causes this:
#    # ugly hack time!
#    expected_line_pattern = []
#    if var_count <= fields_per_line: expected_line_pattern = [var_count]
#    else:
#      tmpvarcount = var_count
#      while tmpvarcount > fields_per_line:
#        expected_line_pattern.append(fields_per_line)
#        tmpvarcount = tmpvarcount - fields_per_line
#      if tmpvarcount > 0: expected_line_pattern.append(tmpvarcount)
#
#    # now let's see if it matches the expected pattern
#    linecount = 0
#    f = open(str, "r")
#    actual_line_pattern = []
#    for line in f:
#      if not line.startswith("@"):
#        linecount = linecount + 1
#        actual_line_pattern.append(len(line[:-1]) / fieldsize)
#
#    f.close()
#
#    expected_line_pattern = expected_line_pattern * (linecount / len(expected_line_pattern))
#
#    no_lines = int(math.ceil(var_count / float(fields_per_line)))
#    lines = []
#
#    f = open(str, "r")
#
#    for line in f:
#        if line.startswith("@("):
#            numnames = line.split() # (NUM):NAME
#            for numname in numnames:
#                name = numname.strip().split(':')[-1].lower() # I love python
#                vars.append(name)
#                vals[name] = []
#        elif not line.startswith("@"):
#            lines.append(line)
#
#    i = 0
#    while i < len(lines):
#        line = ""
#        if actual_line_pattern[i] < expected_line_pattern[i]:
#          print("Warning: .s file is not formatted as advertised. Skipping line %s." % i)
#          i = i + 1
#          continue
#
#        for j in range(i, i+no_lines):
#            line += lines[j][:-1]
#
#        line += '\n'
#        for var in vars:
#            k = vars.index(var)
#            try:
#              vals[var].append(float(line[fieldsize*k:fieldsize*(k+1)]))
#            except ValueError:
#              num = line[fieldsize*k:fieldsize*(k+1)]
#              reverse_num = num[::-1]
#              reverse_num = reverse_num.replace('-', '-e', 1).replace('+', '+e', 1)
#              vals[var].append(float(reverse_num[::-1]))
#
#        i += no_lines
#
#    return vals

#if __name__ == "__main__":
#    import sys
#    var = parse_s(sys.argv[1])
#    print(str(var))

#def compare_variables(reference, current, error, zerotol=1.0e-14):
#    """This takes in an array for a particular variable
#    (e.g. kinetic energy) containing the values of that
#    variable at the timesteps. It compares the output
#    of the current run against a reference run,
#    checking that the relative error is within the bound
#    specified."""
#
#    assert len(reference) == len(current)
#    relerrs = []
#
#    for i in range(len(current)):
#       if abs(reference[i]) > zerotol: # decide if reference[i] is "0.0" or not
#           diff = abs(reference[i] - current[i])
#           relerr = (diff / abs(reference[i]))
#           relerrs.append(relerr)
#       else:
#           relerrs.append(abs(current[i])) # not really a relative error but however
#
#    maxerr = max(relerrs)
#    print("Asserting max relative error is smaller than", error)
#    print("max relative error: %s; index: %s" % (maxerr, relerrs.index(maxerr)))
#    assert maxerr < error
#
#def compare_variable(reference, current, error, zerotol=1.0e-14):
#    """Compares current value with reference value. Relative error
#    should be smaller than 'error'. If the reference is within
#    zerotol however, an absolute error will be used."""
#
#    diff = abs(reference - current)
#    if abs(reference) > zerotol: # decide if reference is "0.0" or not
#       relerr = (diff / abs(reference))
#       print("Asserting relative error is smaller than", error)
#       print("relative error: " + str(relerr))
#       assert(relerr < error)
#    else:
#       print("Asserting absolute error is smaller than", error)
#       print("absolute error: "+ str(diff))
#       assert(diff < error)


def compare_output(current, base, skip):
  """
  Compare output files. Return True if they are identical an False otherwise.
  """
  with open(current) as f1, open(base) as f2:
    f1 = islice(f1, skip, None)
    f2 = islice(f2, skip, None)
    return all(x == y for x, y in zip(f1, f2))
