#!/bin/sh
# . dev.sh

geany -i \
    bin/* \
    bin/utils/* \
    make/* \
    make/global/* \
    src/* \
    src/collision/* \
    src/data/* \
    src/ecs/* \
    src/files/* \
    src/general/* \
    src/graphics/* \
    src/inputs/* \
    src/math/* \
    src/memory/* \
    src/platform/* \
    src/platform/graphics/* \
    src/platform/input/* \
    src/platform/sound/* \
    src/platform/system/* \
    src/sound/* \
    src/strings/* \
    src/time/* \
    .travis.yml \
    dev.sh \
    README.markdown \
    todo &
