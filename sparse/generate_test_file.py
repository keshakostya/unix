#!/usr/bin/env python3
import itertools


if __name__ == '__main__':
    data = list(itertools.repeat(0, 4*1024*1024+1))
    data[0] = 1
    data[10000] = 1
    data[-1] = 1

    with open('build/fileA', 'wb') as f:
        f.write(bytearray(data))
