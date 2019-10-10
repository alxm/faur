#!/bin/sh
# . dev.sh

geany -i \
    * \
    bin/* \
    bin/utils/* \
    make/* \
    make/global/* \
    `find src` \
    &
