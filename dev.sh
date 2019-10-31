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
    media/Makefile \
    `find src` \
    &
