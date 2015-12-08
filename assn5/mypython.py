#!/usr/bin/env python3
from string import ascii_lowercase
from random import choice, randint

for i in range(1,4):
	s = ''.join([choice(ascii_lowercase) for _ in range(10)])
	print(s)
	with open('file{}'.format(i), "w") as f: f.write(s)

x = [randint(1,43) for _ in range(2)]
print('{} x {} = {}'.format(x[0], x[1], x[0]*x[1]))
