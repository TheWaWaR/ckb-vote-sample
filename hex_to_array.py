#!/usr/bin/env python

import sys

for hex_str in sys.argv[1:]:
    hex_str = hex_str.lower()

    if hex_str.startswith('0x'):
        hex_str = hex_str[2:]

    if len(hex_str) % 2 != 0:
        print('[ERROR]: Invalid hex length: {}, hex={}'.format(len(hex_str), hex_str))
        sys.exit(-1)
    for c in hex_str:
        if c not in '0123456789abcdef':
            print('[ERROR]: Invalid char: {}, hex={}'.format(c, hex_str))
            sys.exit(-2)
    if len(hex_str) != 64:
        print('[WARNING]: hex length({}) is not 64 (32 bytes binary data)'.format(len(hex_str)))

    parts = []
    while hex_str:
        parts.append('0x{}'.format(hex_str[:2]))
        hex_str = hex_str[2:]

    print('{%s}' % ', '.join(parts))
