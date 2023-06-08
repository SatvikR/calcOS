#!/bin/sh -x
find src/ -iname *.h -o -iname *.c | xargs clang-format -i