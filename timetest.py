import subprocess
import os

os.remove('./timetest.csv')
with open('test.txt', 'r') as test:
  for line in test.readlines():
    line = line.split(" ")
    with open('input.txt', 'w') as input:
      input.write('N=%s\nM=%s' % (line[0], line[1]))
    subprocess.call('./multisum')

