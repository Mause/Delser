import sys, random
sys.path.append('./build/lib.linux-i686-3.3/')

import delser
x=delser.Delser()
print("key;", x.make_key(random.randint(0,10)))

x.check_key("string")
