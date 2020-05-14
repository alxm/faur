#!/bin/sh
# . dev.sh

geany -i \
    .gitignore \
    .travis.yml \
    media/CC-* media/*.html \
    `find bin make src -type f -not -path "*__pycache__*"` \
    * \
    &
