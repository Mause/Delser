import sys, random
sys.path.append('./build/lib.linux-i686-3.3/')

import delser
inst = delser.Delser(1, [])

key = inst.make_key(1)
print(key)
inst.check_key(input('> '))
