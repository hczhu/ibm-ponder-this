#!/usr/bin/python

def CountBits(x):
  res = 0
  while x > 0:
    res += x&1
    x >>= 1
  return res

def SumAndCountMod4(a, b):
  res = ''
  c = 5 - a - b
  for x in range(1<<5):
    x >>= c
    v = (CountBits(x&((1<<b) - 1)) + (x>>b)) & 3
    res += str(v)
  return res

def Sum(a, b):
  res = ''
  c = 5 - a - b
  for x in range(1<<5):
    x >>= c
    v = ((x&((1<<b) - 1)) + (x>>b)) & 3
    res += str(v)
  return res

def Set(conds):
  res = ''
  for x in range(1<<5):
    value = 0
    for cond in conds:
      value += cond(x)
    res += str(value)
  return res
  
input= 'abcdefghijkl'  
assert len(input) == 12

def Index(alpha):
  return 1 + ord(alpha) - ord('a')

def carry(x):
  prefix = ((x>>3)&1) + (x>>4)
  suffix = (x>>1)&3
  if (prefix == 1 and suffix == 0) or (prefix == 2 and suffix <= 1):
    return 1
  return 0


answer = [
  'abcde' + SumAndCountMod4(0, 5), # m, n
  'mnfga' + SumAndCountMod4(2, 2), # o, p
  'ophia' + SumAndCountMod4(2, 2), # q, r
  'qrjkl' + SumAndCountMod4(2, 3), # s, t
  
  'abmno' + Set([
      lambda x: carry(x),
      lambda x: 1 if ((x>>2)&1) == 1 and (x&1) == 0 else 0
    ]
  ), # u, v
  
  'oqrst' + Set([
      lambda x: 1 if ((x>>2)&3) > (x&3) else 0,
      lambda x: 1 if (x>>4) == 1 and ((x>>3)&1) == 0 else 0
    ]
  ), # w, x
  'uvwxa' + Sum(2, 2), # y, z
]

output = [Index('y'), Index('z'), Index('s'), Index('t')]

print '\n'.join(answer)

print ' '.join(str(idx) for idx in output)


def evaluate(x):
  # print x
  ram = [-1] * 27
  for i in range(12):
    ram[12 - i] = x&1
    x >>= 1
  pointer = 13
  for gate in answer:
    y = 0
    for i in range(5):
      assert ram[Index(gate[i])] >= 0
      assert ram[Index(gate[i])] <= 1
      y = (y<<1) ^ ram[Index(gate[i])]
    # print y, gate, int(gate[5 + y])
    ram[pointer] = int(gate[5 + y])>>1
    pointer += 1
    ram[pointer] = int(gate[5 + y])&1
    pointer += 1
  res = 0
  for idx in output:
    res = (res << 1) ^ ram[idx]
  return res

for x in range(1<<12):
  # print x, CountBits(x), evaluate(x)
  assert CountBits(x) == evaluate(x)


