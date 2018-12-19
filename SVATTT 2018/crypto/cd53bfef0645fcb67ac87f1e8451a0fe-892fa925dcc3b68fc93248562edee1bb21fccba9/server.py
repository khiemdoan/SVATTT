from ec import *
from secret import flag, p, a, b, order
import sys
import string

class Unbuffered(object):
   def __init__(self, stream):
       self.stream = stream
   def write(self, data):
       self.stream.write(data)
       self.stream.flush()
   def writelines(self, datas):
       self.stream.writelines(datas)
       self.stream.flush()
   def __getattr__(self, attr):
       return getattr(self.stream, attr)
sys.stdout = Unbuffered(sys.stdout)

E = CurveFp(p, a, b)
flag = int(flag.encode('hex'),16)
# submit SVATTT2018{flag}
assert flag < order

def check(number):
    if not all(c in string.digits for c in number):
        return False
    number = int(number)
    if number < 0 or number >= p:
        return False
    return True

def calculate(number):
    n = (int(number) + 2) % p
    P = E.complete_point(0)
    Q = n * P
    if Q == INFINITY:
        return 0
    else:
        return Q.x()

if __name__ == '__main__':
  print "This year's ECC challenge is gonna be easy. Good luck :)"
  #Stage 1 - SVATTT 2017
  while 1:
    try:
      inp = raw_input('Do you wanna guess my curve?[y/N] ').lower()
      if inp == 'y' or inp == 'yes':
        inp = raw_input('How many steps you want to guess? ')
        if not check(inp):
          print "This is not a pwn challenge, you're pissing me off!\n"
          break
        else:
          steps = calculate(inp)
          print "If you take %d steps you will end up at %d" % (int(inp), steps)
      else:
        break
    except:
      break
  #Stage 2
  while 1:
    try:
      print "----- STARTING HANDSHAKE -----"
      print "Server: Give me your public key"
      inp = raw_input()
      x, y = inp.replace(' ','').split(',')
      if not check(x) or not check(y):
        print "This is not a pwn challenge, you're pissing me off. Bye!\n"
        break
      else:
        x, y = int(x), int(y)
        P = Point(E, x, y, order)
        sP = P * flag
        print "Client: Here you are\n" + str(P)
        print "Server: Here is our master key\n" + str(sP)
        inp = raw_input('Are we good?[Y/n] ').lower()
        if inp == 'n' or inp == 'no':
          continue
        print "-----  ENDING HANDSHAKE  -----"
        break
    except:
      print "Bye!"
      break