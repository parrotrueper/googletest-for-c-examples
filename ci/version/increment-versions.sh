#!/usr/bin/env bash
# Exit on error
set -euo pipefail

# shellcheck source=/dev/null
. ci/functions.sh
# Error handling
trap 'printf "\n\nERROR at $0 line $LINENO. Exiting.\n\n"' ERR

#-------------------------------------------------------------------------------
# Script that increments the version numbers for the APIs based on the commit
# message.
#
#  FORMAT: Major.Minor.Patch
#
#  Major - Increment if the API key name AND "BREAKING CHANGE" appear in the
#          commit message
#
#  Minor - Increment if commit message contains API key name
#
#  Patch - If no Major or Minor change, increment Patch, so that all API
#           builds are unique.
#-------------------------------------------------------------------------------
# shellcheck source=/dev/null
cfg_file="./config.json"

# get the commit message
commit_msg=$(git log -1 --pretty=%B)

# get the commit hash
commit_hash=$(git log -1 --pretty=%H)
# get the branch name
branch_name=$(git rev-parse --abbrev-ref HEAD)
version_lock="${branch_name}_${commit_hash}"

# version number
major=0
minor=0
patch=0

parse_version_number(){
    version_str=$1
    IFS='.' read -r major minor patch <<< "${version_str}"
}
increment_patch(){
    patch=$((patch + 1))
}
increment_minor(){
    minor=$((minor + 1))
    patch=0
}
increment_major(){
    major=$((major + 1))
    minor=0
    patch=0
}
info "Reading version numbers from repository variables"
run ./ci/version/get-pipeline-variables.sh
version_data="./ci/version/version_vars.json"
# don't update the version number again on the main branch if someone re-runs
# the pipeline.
info "Checking version lock"
upstream_version_lock=$(jq -r '.VERSION_LOCK.commit' "${version_data}")
if [[ "${upstream_version_lock}" == "${version_lock}" ]]; then
    info "Version lock matches ${version_lock}, no version increment needed"
    exit 0
fi
branch_that_incs=$(jq -r '.VERSION_LOCK.branch' "${cfg_file}")
info "Version values for ${branch_that_incs} branch"
# for each SW version key in id_prefix, check if the commit message contains
# the value of the key. If so, increment the version number accordingly.
sw_keys=$(jq -r '.id_prefix | keys[]' "${cfg_file}")
for key in ${sw_keys}; do
    version_updated="no"
    # read the current version number from the repository variable
    version_str=$(jq -r ".${key}.value" "${version_data}")
    parse_version_number "${version_str}"
    info "Current version for ${key} is ${major}.${minor}.${patch}"
    # get the value of the key
    sw_value=$(jq -r ".id_prefix.${key}" "${cfg_file}")
    # check if the commit message contains the value of the key
    if [[ "${commit_msg}" == *"${sw_value}"* ]]; then
        info "Commit message contains ${sw_value} for ${key}"
        # check if the commit message contains "BREAKING CHANGE"
        if [[ "${commit_msg}" == *"BREAKING CHANGE"* ]]; then
            info "Commit message contains BREAKING CHANGE, incrementing major version"
            increment_major
            version_updated="yes"
        else
            info "Commit message does not contain BREAKING CHANGE, incrementing minor version"
            increment_minor
            version_updated="yes"
        fi

    fi
    # if the version was not updated, increment the patch version
    if [[ "${version_updated}" == "no" ]]; then
        info "Commit message does not contain ${sw_value} for ${key}, incrementing patch version"
        increment_patch
    fi
    # create the new version string
    new_version="${major}.${minor}.${patch}"
    info "New version for ${key} is ${new_version}"
    # update the repository variable with the new version
    run ./ci/version/update-version-variable.sh "${key}" "${new_version}"

done

# ensure that a pipeline re-run does not update the version numbers again
if [[ "${branch_name}" == "${branch_that_incs}" ]]; then
    if [[ "${upstream_version_lock}" == "${version_lock}" ]]; then
        info "Version lock already recorded as ${version_lock}, no update needed"
    else
       info "Updating version lock to ${version_lock}"
       run ./ci/version/update-version-variable.sh "VERSION_LOCK" "${version_lock}"
    fi
fi
