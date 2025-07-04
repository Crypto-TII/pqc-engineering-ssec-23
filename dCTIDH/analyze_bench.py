#!/usr/bin/env python3

import sys
import math
import statistics

def mean(d):
  return sum(d)*1.0/len(d)

def deviation(d):
  m = mean(d)
  return math.sqrt(sum((x-m)**2 for x in d)*1.0/len(d))

keys = set()
validate = {}
action = {}
torsion = {}
mults = {}
sqs = {}
adds = {}
num = 0

for line in sys.stdin:
  line = line.split()
  if len(line) < 11: continue
  if line[3] != 'mulsq': continue
  if line[5] != 'sq': continue
  if line[7] != 'addsub': continue
  if line[9] != 'cycles': continue

  if line[2] == 'validate':
    target = validate
  elif line[2] == 'action':
    target = action
  elif line[2] == 'torsionpoint':
    target = torsion
  elif line[2] == 'mults':
    target = mults
  elif line[2] == 'sqs':
    target = sqs
  elif line[2] == 'adds':
    target = adds
  else:
    continue

  mul = int(line[4])
  sq = int(line[6])
  addsub = int(line[8])
  Mcyc = 0.000001*int(line[10])
  mulsq = mul+sq
  combo185 = mul+0.8*sq+0.05*addsub

  keys.add(int(line[1]))
  for k in int(line[1]),'total':
    for name,value in (
      ('Mcyc',Mcyc),
      ('mulsq',mulsq),
      ('sq',sq),
      ('addsub',addsub),
      ('mul',mul),
      ('combo185',combo185),
    ):
      if (k,name) not in target:
        target[k,name] = []
      target[k,name] += [value]

  num += 1

for k in sorted(keys)+['total']:
  for targetname,target in (
    ('validate',validate),
    ('action',action),
    ('torsionpoint',torsion),
    ('mults',mults),
    ('sqs',sqs),
    ('adds',adds),
  ):
    if target == {}:
      continue
    output = '%s %s' % (k,targetname)
    output += ' experiments %d' % len(target[k,'Mcyc'])
    for name in ('Mcyc','mulsq','sq','addsub','mul','combo185'):
      x = target[k,name]
      format = ' %s %.0f+-%.0f'
      if name == 'Mcyc':
        format = ' %s %.5f+-%.5f'
      output += format%(name,statistics.median(x),deviation(x))
    print(output)