#!/usr/bin/env python
# S***y lottery puzzle by tony@lazarew.me

import math
from operator import mul
import sys


def combs(items, slots):
    if items > slots:
        return 0
    return \
        reduce(mul, xrange(items + 1, slots + 1), 1) \
        / math.factorial(slots - items)


def calc_probability(m, n, t, p):
    """
    m -- the total number of people who entered the lottery
    n -- the total number of winners drawn
    t -- the number of tickets each winner is allowed to buy
    p -- the number of people in your group
    """
    # w -- required minimum for winners
    w = int(math.ceil(float(p) / t))

    if w > n:
        return 0.

    favs = []
    total = combs(n, m)
    for i in xrange(0, min(p, n) - w + 1):
        favs.append(combs(w + i, p) * combs(n - w - i, m - p))
    return math.fsum(favs) / total


def main():
    m, n, t, p = (int(d) for d in sys.stdin.read().split(" "))

    assert 1 <= m <= 1000
    assert 1 <= n <= m
    assert 1 <= t <= 100
    assert 1 <= p <= m

    pretty = "%.10f" % calc_probability(m, n, t, p)
    if 0 < float(pretty) < 1:
        pretty = pretty.rstrip("0")
    print(pretty)


if __name__ == "__main__":
    main()
