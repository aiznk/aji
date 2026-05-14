#!/usr/bin/python
import sys
import re
import unittest


def rem(s):
    s = re.sub(r'trv_ready;\n[\s+\n|\n]', '', s)
    s = re.sub(r'\n\s+trv_cleanup;\n', '', s)
    return s


if __name__ == '__main__':
    s = sys.stdin.read()
    print(rem(s))


class Test(unittest.TestCase):
    def test_rem(self):
        s = '''abc
    trv_ready;

    def

    trv_cleanup;
'''
        print('result', rem(s))
