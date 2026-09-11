#!/usr/bin/env bash
# Exit on error
set -eEuo pipefail

# shellcheck source=/dev/null
. ci/functions.sh

# Error handling
trap 'err "ERROR at $0 line ${BASH_LINENO[0]}. Exiting."; exit 1' ERR

info "Looking for all test directories ...."

# Find test dirs, exclude .vscode-server and .dependencies
test_dirs=$(find . -type d -name "test" -not -path "./.vscode-server/*" -not -path "./.dependencies/*" -not -path "./scripts/*" | sed 's|^\./||' | sort)

# List all the detected tests
separator "Detected Tests"
echo "${test_dirs}"

info "Checking if all tests are listed in tests.json ..."
tests_json="scripts/test/tests.json"
for dir in ${test_dirs}; do
    # check that the directrory is listed in tests.json
    path_found=$(jq -r --arg path "./${dir}" '
      [ .serial[]?.path, .parallel[]?.path ]
      | map(select(. == $path))
      | .[0] // empty' "${tests_json}")
    if [[ -z "${path_found}" ]]; then
      err "Missing from tests.json: ${dir}"
    fi
done

