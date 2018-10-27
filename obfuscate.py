#!/usr/bin/python3

import re
import sys

source = sys.stdin.read().strip()

def obfuscate_print(match):
    string = match.group(1)
    replacement = ''
    last_char = ''
    for c in string:
        if ord(c) >= 97 and ord(c) <= 122 and last_char != '\\':
            replacement += chr(ord(c) - 1)
        else:
            replacement += c
        last_char = c
    return 'print("%s"' % replacement

def obfuscate_strings(match):
    string = match.group(1)
    replacement = ''
    last_char = ''
    for c in string:
        if ord(c) >= 97 and ord(c) <= 122 and last_char != '\\':
            replacement += chr(ord(c) - 1)
        else:
            replacement += c
        last_char = c
    return '    "%s",' % replacement

source = re.sub(r'print\("([^"]*)"', obfuscate_print, source)
source = re.sub(r'^    "([^"]*)",$', obfuscate_strings, source, flags=re.M)
print(source)
