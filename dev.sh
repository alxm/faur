#!/bin/sh
# . dev.sh

geany -i \
    `find . -type f -not \( \
           -path "./.git/*" \
        -o -path "*/__pycache__/*" \
        -o -name "*.gif" \
        -o -name "*.png" \
    \)` &
