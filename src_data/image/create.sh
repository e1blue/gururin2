#!/bin/bash

cd $(dirname $0)

# ----------------------------------------------------------------

export TESTMODE=0
export RELEASEMODE=1
export OPTIMMODE=0

pushd system  >/dev/null ; sh create.sh ; popd >/dev/null

# ----------------------------------------------------------------

