#this is a script used to check Bignum operations

#!/usr/bin/env python3

import ast
from ast import literal_eval

def hextodec(a):
    # a should be in big-endian
    a.reverse()
    t = 0
    for i in a:
        t *= 256**4
        t += i
    return t

def dectohex(a):        
        t = []
        while a != 0:
                t.append(a%(256**4))
                a >>= 8
        #output is little-endian
        return t

def egcd(a, b):
        if a == 0:
                return (b, 0, 1)
        else:
                g, y, x = egcd(b % a, a)
                return (g, x - (b // a) * y, y)

def modinv(a, m):
        g, x, y = egcd(a, m)
        if g != 1:
                print (a)
                raise Exception('modular inverse does not exist')
        else:
                return x % m
try:
    ok = 0
    not_ok = 0
    while True:
        while input() != "BIGNUM_DEBUG_EQUATION_START":            
            pass
        x = hextodec(literal_eval(input()))
        op = input()
        y = hextodec(literal_eval(input()))
        mod = input()
        _prime= hextodec(literal_eval(input()))
        eq = input()
        C = hextodec(literal_eval(input()))

#         equation = str(x) + " "+op +" "+ str(y) +" "+ eq +" "+ str(C)
        equation = "(" + str(x) + " "+op +" "+ str(y) +") "+ mod + " " + str(_prime) + " " + eq +" "+ str(C)
        if not eval(equation):        
            not_ok+=1
            print(equation)
            print("Should be: ", eval("("+str(x) + op + str(y) + ")"+mod + str(_prime)))
        else:
            ok+=1            


#        res = (x * y * 340282366604025813480104233352430616576) % 340282366762482138434845932244680310783

#        if res != C:
#           print("==========WRONG:")
#        print(x, "*", y, "* 340282366604025813480104233352430616576 mod 340282366762482138434845932244680310783")
#        print(C)
#        print(res)
#       inv = modinv(x, _prime)

#        if inv != C:
#           print("==========WRONG:")
#        print(x, "^(-1) mod", _prime)
#        print(C)
#        print(inv)
except EOFError:
    pass
print("Correct equations: ", ok)
print("Incorrect equations: ", not_ok)
