#!/usr/bin/env bash
# Exit on error
set -euo pipefail

# Error handling
trap 'printf "\n\nERROR at $0 line $LINENO. Exiting.\n\n"' ERR

# shellcheck source=/dev/null
. ci/functions.sh

# Check that jq is installed
if ! command -v jq >/dev/null 2>&1; then
    err "This script requires \"jq\". Please instal the package..."
    fatal 1 "sudo apt install jq"
fi

run scripts/host/create-env-file.sh
run scripts/host/check-docker-base-image.sh
run scripts/test/generate-test-env.sh

