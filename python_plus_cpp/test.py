import sys, random
sys.path.append('./build/lib.linux-i686-3.3/')

import delser
inst = delser.Delser(0)

for x in range(1000000):
    inst.check_key(inst.make_key(x))

#x.check_key("string")
