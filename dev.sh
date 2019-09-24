#!/bin/sh
# . dev.sh

geany -i \
    bin/* \
    bin/utils/* \
    make/* \
    make/global/* \
    `find src -type f -name "*.c"` \
    `find src -type f -name "*.h"` \
    .travis.yml \
    dev.sh \
    README.markdown \
    todo &
