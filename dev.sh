#!/bin/sh
# . dev.sh

geany -i \
    .gitignore \
    .travis.yml \
    media/Makefile \
    `find bin make src -type f -not -path "*__pycache__*"` \
    * \
    &
