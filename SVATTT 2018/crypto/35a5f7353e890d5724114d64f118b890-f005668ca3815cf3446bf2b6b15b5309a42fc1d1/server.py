from Crypto.Util import number
from Crypto.PublicKey import RSA
from hashlib import sha1
from os import urandom as rand
import gmpy2

class RSA_key:
    def __init__(self, magic, size=2048):
        self._key = RSA.generate(size)
        self._magic = magic

    def size(self, bytes=True):
        bits = number.size(self._key.n)
        if bytes:
            return gmpy2.c_div(bits, 8)
        return bits

    def b2i(self, x):
        return int.from_bytes(x, 'big')

    # textbook RSA
    def encrypt(self, m):
        if type(m) == bytes:
            m = self.b2i(m)
        return gmpy2.powmod(m, self._key.e, self._key.n)

    def decrypt(self, c):
        if type(c) == bytes:
            c = self.b2i(c)
        return gmpy2.powmod(c, self._key.d, self._key.n)

# (https://en.wikipedia.org/wiki/Mask_generation_function#Example_Code)
def I2OSP(integer, size=4):
    return bytes([(integer >> (8 * i)) & 0xFF for i in range(size - 1, -1, -1)])

def MGF1(integer, length, H=sha1):
    counter = 0
    output = b''
    while len(output) < length:
        c = I2OSP(counter, 4)
        output += H(integer + c).digest()
        counter += 1
    return output[:length]

def xor(a, b):
    return bytes([x ^ y for x, y in zip(a, b)])

def goEnc(pk, m, H=sha1):
    mlen = len(m)
    k = pk.size()
    lhash = H(pk._magic).digest()
    hlen = len(lhash)

    if mlen > k - 2*hlen - 2:
        raise ValueError('message too long')

    padding    = bytes([0x00] * (k - mlen - 2*hlen - 2))
    text       = lhash + padding + b'\x01' + m
    seed       = rand(hlen)
    textmask   = MGF1(seed, k - hlen - 1)
    maskedtext = xor(text, textmask)
    seedmask   = MGF1(maskedtext, hlen)
    maskedseed = xor(seed, seedmask)
    em         = b'\x00' + maskedseed + maskedtext

    # encryption
    c = pk.encrypt(em)
    return hex(c)

def goDec(sk, c, H=sha1):
    k = sk.size()
    lhash = H(sk._magic).digest()
    hlen = len(lhash)

    # decryption
    m = sk.decrypt(c)

    em = int(m).to_bytes(k, byteorder='big')
    if em[0] != 0x00:
        raise ValueError('Sum ting wong?')

    maskedseed = em[1:1+hlen]
    maskedtext = em[1+hlen:]
    seedmask   = MGF1(maskedtext, hlen)
    seed       = xor(maskedseed, seedmask)
    textmask   = MGF1(seed, k - hlen - 1)
    text       = xor(maskedtext, textmask)
    _lhash     = text[:hlen]

    if _lhash != lhash:
        raise ValueError('Hash value not matched.')
        try:
            i = text.index(0x01)
        except ValueError:
            raise ValueError('No 0x01 byte found')
    else:
        return text[i+1:]

challenge = '''To make this challenge more easy,
I'll give you the encrypted flag: {flag}
Have fun!'''
menu = '''1. encrypt
2. decrypt
3. public key
'''
if __name__ == '__main__':
    rsa = RSA_key(b'tictactoe')
    file = open('flag.txt','r').read().encode('ascii')
    print(challenge.format(flag=goEnc(rsa, file)))
    while 1:
        try:
            inp = input(menu)
            if inp == '1':
                print("TO DO!")
            elif inp == '2':
                c = input('Give me somthing: ')
                c = int(c,16)
                try:
                    goDec(rsa, c)
                except ValueError as e:
                    print(str(e))
                else:
                    print('OK')
            elif inp == '3':
                print('Here you are:\nn:', hex(rsa._key.n),'\ne:', hex(rsa._key.e))
            else:
                print('Bye!')
                break
        except ValueError as e:
            print(str(e))