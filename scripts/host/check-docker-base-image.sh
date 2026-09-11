#!/usr/bin/env bash
# Exit on error
set -euo pipefail

# shellcheck source=/dev/null
. ci/functions.sh

bail_if_in_container "$0"

# shellcheck source=/dev/null
. .env

docker pull "${CI_BASE_IMAGE:?CI_BASE_IMAGE is not set in .env file}"
