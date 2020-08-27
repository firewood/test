#!/usr/bin/env python3

import os
import re

for filename in os.listdir():
  if re.match('.*\.py$', filename):
    with open(filename) as f:
      s = f.read()
    if re.match('^#', s):
      continue
    print("Convert: " + filename)
    while re.match('^\n\n', s):
      s = s.replace('\n\n', '\n', 1)
    s = '#!/usr/bin/env python3\n' + s
    with open(filename, mode='w') as f:
      f.write(s)
    os.chmod(filename, 0o755)
