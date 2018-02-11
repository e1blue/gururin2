#!/bin/bash

FILES=""
FILES=${FILES}" src_client/main/core/plugin/pluginSecretCoded.h"
FILES=${FILES}" src_client/platform_android/gradle.properties"
FILES=${FILES}" src_data/secret/createSecret.h"

# --------------------------------

git update-index --skip-worktree ${FILES}
#git update-index --no-skip-worktree ${FILES}

# --------------------------------

