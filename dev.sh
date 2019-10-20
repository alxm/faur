#!/bin/sh
# . dev.sh

geany -i \
    * \
    .gitignore \
    .travis.yml \
    bin/* \
    bin/utils/* \
    make/* \
    make/global/* \
    `find src` \
    &
