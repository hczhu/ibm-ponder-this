#!/usr/bin/python

import numpy
import itertools

def verify(teams, N):
  n = len(teams)
  feasible = [False] * (N + n + 1)
  for m in xrange(n + 1):
    for st in itertools.combinations(teams, m):
      s = sum(st)
      if s <= N: feasible[len(st) + N - s] = True
  for i in xrange(N):
    assert feasible[i + 1], 'Failed for number of teams: %s'%(i + 1)
  print teams

N = 78
verify([2, 3, 5, 8, 16, 24, 54, N], N)
