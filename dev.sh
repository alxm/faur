#!/bin/sh
# . dev.sh

geany -i \
    `find . \
        \( \
               -type f \
            -o -type l \
        \) \
        -not \( \
               -path "./.git/*" \
            -o -path "*/__pycache__/*" \
            -o -name "*.gif" \
            -o -name "*.png" \
        \)` \
        &
