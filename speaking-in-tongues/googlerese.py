#!/usr/bin/env python
# Google Code Jam (Qual Round 2012) by tony@lazarew.me
# http://code.google.com/codejam/contest/1460488/dashboard

import itertools
import sys

decode_seq = tuple("ynficwlbkuomxsevzpdrjgthaq")
decode = lambda x: 97 <= ord(x) <= 122 and chr(decode_seq.index(x) + 97) or x

def main():
    for i, case in enumerate(sys.stdin.read().strip().split("\n")[1:]):
        print("Case #%d: %s" % (i + 1, "".join((decode(x) for x in case))))

if __name__ == "__main__":
    main()
