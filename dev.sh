#!/bin/sh
# . dev.sh

geany -i \
    * \
    .gitignore \
    bin/* \
    bin/utils/* \
    make/* \
    make/global/* \
    `find src` \
    &
