#!/usr/bin/env python
# S***y bilateral projects puzzle by tony@lazarew.me

import os
import sys


FRIEND_ID = 1009


def calc_invitees(teams, friend_id=FRIEND_ID):
    """
    teams -- list of employee pairs, each pair represents a team.  First item
        is an employee from Stockholm, second -- from London
    friend_id -- employee ID of the friend we want to promote
    """
    employees = {}
    for i, team in enumerate(teams):
        for x in team:
            employees.setdefault(x, set()).add(i)

    ranks = sorted(
        employees.keys(),
        key=lambda k: (len(employees[k]), k == friend_id, os.urandom(3)),
        reverse=True)

    invitees = []
    prev = set()
    for employee in ranks:
        projects = len(employees[employee]) - len(employees[employee] & prev)
        prev |= employees[employee]
        if projects:
            invitees.append(employee)

    return invitees


def main():
    lines = sys.stdin.read().strip().split("\n")
    m = int(lines[0])
    teams = [(int(l.split()[0]), int(l.split()[1])) for l in lines[1:]]

    assert 1 <= m <= 10000
    assert len(teams) == m
    for i, j in teams:
        assert 1000 <= i <= 1999
        assert 2000 <= j <= 2999

    invitees = calc_invitees(teams)

    print(len(invitees))
    for i in invitees:
        print(i)


if __name__ == "__main__":
    main()
