import hashlib
import subprocess
import random
import string

program_path = "./md5"

tests = 100

valid_count = 0
invalid_count = 0

for N in range(0, tests):
    text_to_hash = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(N))
    proc = subprocess.Popen([program_path, text_to_hash, "--nosteps"], stdout=subprocess.PIPE)


    valid = hashlib.md5(str.encode(text_to_hash)).hexdigest()
    candidate = proc.stdout.read().split()[-1].decode('utf-8')

    print("Hashing: [", text_to_hash, "]")
    print(valid, " == ", candidate, "?", (candidate == valid), "\n")

    
    if(candidate == valid):
        valid_count += 1
    else:
        invalid_count += 1

print(valid_count, "/", tests, "TEST PASS!")