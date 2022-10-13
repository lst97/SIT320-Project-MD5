import hashlib
from binascii import unhexlify,hexlify
m = hashlib.md5()

prefix_1 = 'd131dd02c5e6eec4693d9a0698aff95c2fcab58712467eab4004583eb8fb7f8955ad340609f4b30283e488832571415a085125e8f7cdc99fd91dbdf280373c5bd8823e3156348f5bae6dacd436c919c6dd53e2b487da03fd02396306d248cda0e99f33420f577ee8ce54b67080a80d1ec69821bcb6a8839396f9652b6ff72a70'
prefix_2 = 'd131dd02c5e6eec4693d9a0698aff95c2fcab50712467eab4004583eb8fb7f8955ad340609f4b30283e4888325f1415a085125e8f7cdc99fd91dbd7280373c5bd8823e3156348f5bae6dacd436c919c6dd53e23487da03fd02396306d248cda0e99f33420f577ee8ce54b67080280d1ec69821bcb6a8839396f965ab6ff72a70'
m1=unhexlify(prefix_1)
m2=unhexlify(prefix_2)

print("m1 == m2 ? ",m1 == m2, '\n')
#4e656c736f6e204c414920323230363139333735
word = "Nelson LAI 220619375"

a = word.encode()

mm1 = hashlib.md5()
mm2 = hashlib.md5()
mm1.update(m1+a)
mm2.update(m2+a)
print (hexlify(m1+a),"\nHex:",mm1.digest().hex())
print ("\n",hexlify(m2+a),"\nHex:",mm2.digest().hex())

