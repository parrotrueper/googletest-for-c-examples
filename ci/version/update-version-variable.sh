#!/usr/bin/env bash
# Exit on error
set -euo pipefail

# shellcheck source=/dev/null
. ci/functions.sh
# Error handling
trap 'printf "\n\nERROR at $0 line $LINENO. Exiting.\n\n"' ERR


# Call update-version-variable.sh <version variable name> <new value>
key=$1
new_value=$2
is_pipeline=$(is_ci)
# shellcheck source=/dev/null
var_file="./ci/version/version_vars.json"

# load the file into a variable so we can use jq to parse it
repo_vars=$(cat "${var_file}")

#shellcheck disable=2312
if [[ $(jq --arg key "${key}" 'has($key)' <<< "${repo_vars}") == "false" ]]; then
	fatal 1 "${key} does not exist in ${var_file}. Cannot update version variable."
fi

orig_value=$(jq -r ".${key}" "${var_file}")
#shellcheck disable=2312
info "updating key ${key} from ${orig_value} to ${new_value}"
# we only update variables when we are on the specified branch, typically main
# which is used for testing
can_update="no"
# get the branch name
branch_name=$(git rev-parse --abbrev-ref HEAD)
branch_can_update=$(jq -r '.VERSION_LOCK.branch' "${var_file}")

if [[ "${is_pipeline}" = "yes" ]]; then
	info "Running in a pipeline"
  if [[ "${branch_name}" == "${branch_can_update}" ]]; then
      can_update="yes"
  fi
else
    if [[ "${key}" = "TEST_VERSION" ]]; then
        can_update="yes"
    else
        info "Running on local host -- no update will be made to repository variables"
    fi
fi


if [[ "${can_update}" == "yes" ]]; then

  info "updating version variable ${key} to ${new_value}"
  # now replace the value in version_vars.json with the new value
  jq --arg key "${key}" --arg new_value "${new_value}" \
    '(.[$key]) |= $new_value' "${var_file}" > "${var_file}.tmp" && mv "${var_file}.tmp" "${var_file}"


else
  info "Not on ${branch_can_update} branch, skipping update of version variable ${key}"
fi




