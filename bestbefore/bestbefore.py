#!/usr/bin/env python
# Spotify bestbefore puzzle by tony@lazarew.me

import datetime
import itertools
import sys


class DecodeError(Exception):
    pass


# Data converters/placeholders
def PH_YEAR(y):
    'year'
    if 0 <= y <= 999:
        return y + 2000
    elif 2000 <= y <= 2999:
        return y
    else:
        raise DecodeError


def PH_MONTH(m):
    'month'
    if 1 <= m <= 12:
        return m
    else:
        raise DecodeError


def PH_DAY(d):
    'day'
    if 1 <= d <= 31:
        return d
    else:
        raise DecodeError


def calc_date(numbers):
    """
    numbers -- list of numbers which are supposed to form a date
    """
    valid = []
    for fmt in itertools.permutations((PH_YEAR, PH_MONTH, PH_DAY)):
        try:
            decoded = dict((
                (ph.__doc__, ph(numbers[i]))
                for i, ph in enumerate(fmt)))
        except DecodeError:
            continue

        try:
            date = datetime.date(**decoded)
        except ValueError:
            continue

        valid.append(date)

    if valid:
        date = min(valid)
        return "%d-%02d-%02d" % (date.year, date.month, date.day)
    return None


def main():
    input_data = sys.stdin.read().strip()
    numbers = [int(x) for x in input_data.split("/")]

    assert len(numbers) == 3
    for n in numbers:
        assert 0 <= n <= 2999

    date = calc_date(numbers)
    if date:
        print(date)
    else:
        print("%s is illegal" % input_data)


if __name__ == "__main__":
    main()
