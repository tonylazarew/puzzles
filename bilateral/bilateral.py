#!/usr/bin/env python
# S***y bilateral projects puzzle by tony@lazarew.me

import collections
import sys


class Matching(object):
    """I perform calculation for minimum vertex cover of the employee graph.
    I'm based on:
        * http://en.wikipedia.org/wiki/Koenig's_theorem_(graph_theory)
        * http://en.wikipedia.org/wiki/Hopcroft-Karp_algorithm
    """
    def __init__(self, pairs):
        self.sthlm = set()
        self.ldn = set()
        self.neighbors = {}

        self.match = {}
        self.dist = {}
        self.q = collections.deque()

        for i, j in pairs:
            self.sthlm.add(i)
            self.ldn.add(j)
            self.neighbors.setdefault(i, set()).add(j)
            self.neighbors.setdefault(j, set()).add(i)

    def koenig(self):
        """Returns the minimum vertex cover based on the matching found by
        Hopcroft-Karp algorithm.

        In order to promote our swedish friend into the list of invitees we just
        start off from free vertices in Sthlm's half of the graph.  This way in
        situations when both Ldn and Sthlm employees are appropriate we choose
        a Sthlm's one, whether it is our friend or not.  That is the only legal
        way to get him an invitation.
        """
        self.t = set()
        for i in self.sthlm:
            if self.match[i] is None:
                self.t.add(i)
                self.vertex_cover(i)

        return list((self.sthlm - self.t) | (self.ldn & self.t))

    def vertex_cover(self, v):
        for u in self.neighbors[v]:
            if u in self.t:
                continue
            m = self.match[u]
            if m and m != v:
                self.t.add(u)
                self.t.add(m)
                self.vertex_cover(m)

    def hopcroft_karp(self):
        """Finds the maximum matching for the bipartite graph.
        """
        for employee in (self.sthlm | self.ldn):
            self.match[employee] = None
            self.dist[employee] = None

        matches = 0

        while self.breadth_first_search():
            for i in self.sthlm:
                if not self.match[i] and self.depth_first_search(i):
                    matches += 1

        return matches

    def breadth_first_search(self):
        for i in self.sthlm:
            if not self.match[i]:
                self.dist[i] = 0
                self.q.append(i)
            else:
                self.dist[i] = None

        self.dist[None] = None

        while self.q:
            i = self.q.popleft()
            if not i:
                continue

            for j in self.neighbors[i]:
                m = self.match[j]
                if self.dist[m] is None:
                    self.dist[m] = self.dist[i] + 1
                    self.q.append(m)

        return self.dist[None] is not None

    def depth_first_search(self, i):
        if not i:
            return True

        for j in self.neighbors[i]:
            p = self.match[j]
            if self.dist[p] == self.dist[i] + 1 \
               and self.depth_first_search(p):
                self.match[i], self.match[j] = j, i
                return True

        self.dist[i] = None
        return False


def main():
    lines = sys.stdin.read().strip().split("\n")
    m = int(lines[0])
    teams = [(int(l.split()[0]), int(l.split()[1])) for l in lines[1:]]

    assert 1 <= m <= 10000
    assert len(teams) == m
    for i, j in teams:
        assert 1000 <= i <= 1999
        assert 2000 <= j <= 2999

    m = Matching(teams)
    m.hopcroft_karp()
    invitees = m.koenig()

    print(len(invitees))
    for i in invitees:
        print(i)


if __name__ == "__main__":
    main()
