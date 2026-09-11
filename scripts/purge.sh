#!/usr/bin/env bash

# Exit on error
set -eEuo pipefail

# shellcheck source=/dev/null
. ci/functions.sh

# Error handling
trap 'err "ERROR at $0 line ${BASH_LINENO[0]}. Exiting."; exit 1' ERR

info "Clean build objs and temporary files"
run ./scripts/clean-all.sh

info "Clean binaries and build products"
rm -rf ./reports


