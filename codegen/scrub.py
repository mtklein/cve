#!/usr/bin/env python3
import re
import sys

NOISE_DIRECTIVE = re.compile(
    r'^\s*\.(cfi_\w*|section|subsections_via_symbols|build_version|ident|file'
    r'|align|p2align|arch|text|globl|set|long|quad|byte|ascii|sleb128|uleb128)\b'
)
NOISE_LABEL = re.compile(
    r'^(LFB|LFE|LCFI|LECIE|LSCIE|LSFDE|LASFDE|LEFDE|L\$set\$|EH_frame)'
)
INLINE_COMMENT = re.compile(r'\s*;.*$')

for line in sys.stdin:
    line = INLINE_COMMENT.sub('', line.rstrip())
    if not line.strip():
        continue
    if NOISE_DIRECTIVE.search(line):
        continue
    if NOISE_LABEL.match(line.lstrip()):
        continue
    print(line)
