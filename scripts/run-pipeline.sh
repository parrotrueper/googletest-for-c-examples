#!/usr/bin/env bash
# Exit on error
set -eEuo pipefail

# shellcheck source=/dev/null
source ci/functions.sh
# Error handling
trap 'printf "\n\nERROR at $0 line $LINENO. Exiting.\n\n"' ERR

# This script is invoked by
# ./ci/test

is_docker=$(is_in_container)

initialise() {
    header "Initialising Local Pipeline Run"
    # shellcheck source=/dev/null
    source /etc/bash.bashrc
    run ./scripts/purge.sh
    run clear
    info "Local pipeline test run"
    info "Toolchain for the pipeline"
    gcc --version
    clang-format --version
    #info $PATH

    header "Clang Format Fix"
    run ./scripts/do-clang-format.sh
}

run_linters() {
    # as per the first step in the pipeline.
    header "Run Linters"
    info "Generating dependencies"

    info "Development Environment checks and Shell Linting"
    run ./ci/run-shell-linter.sh
}


# script starts here
#===================
main() {
    on_pipeline=$(is_ci)
    if [[ "${on_pipeline}" = "yes" ]] || [[ "${is_docker}" = "yes" ]]; then
        local start_time end_time elapsed
        start_time=$(date +%s.%N)

        initialise

        run_linters

        header "Generate version file"
        run ./scripts/generate-version-file.sh
        header "Run Off-Target Tests"
        ./scripts/run-tests.sh -t ALL

        end_time=$(date +%s.%N)
        elapsed=$(awk "BEGIN { print ${end_time} - ${start_time} }")

        # here you could add a stage to push artefacts to a remote server

        header "~~~~~~~~~~~~~~~~~~~ DONE ~~~~~~~~~~~~~~~~~~~~~~~"

        printf 'pipeline completed in %.3f seconds\n' "${elapsed}"
        header "Cleanup"
        run ./scripts/purge.sh
        header "Changes left to commit"
        run git status
    else
        err "Do not invoke this script directly!"
        warn "use ./ci/test"
        fatal 1 "Aborting..."
    fi
}

main "$@"
