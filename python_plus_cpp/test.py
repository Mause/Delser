import sys, random
sys.path.append('./build/lib.linux-i686-3.3/')

import delser
x=delser.Delser()
#print([y for y in dir(x) if not y.startswith('_')])
print(x)
y = x.make_key(0)
print(y)
#print("key;", y)#random.randint(0,10)))

x.check_key("string")
