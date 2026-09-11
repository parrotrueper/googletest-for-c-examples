#!/usr/bin/env bash
# Exit on error
set -euo pipefail

# shellcheck source=/dev/null
. ci/functions.sh
# Error handling
trap 'printf "\n\nERROR at $0 line $LINENO. Exiting.\n\n"' ERR

# Script to print TOML linting issues for a given file or all .toml files in a
# directory
# Usage: ./run-toml-linter.sh [file_path | directory_path]
# If no argument, checks all .toml files in current directory recursively

target="${1:-.}"

if [[ -f "${target}" ]]; then
    # Single file
    if [[ "${target}" != *.toml ]]; then
        fatal 1 "Error: File '${target}' is not a .toml file."
    fi
    files=("${target}")
elif [[ -d "${target}" ]]; then
    # Directory, find all .toml files recursively
    mapfile -t files < <(find "${target}" -type f -name "*.toml" || true)
    if [[ ${#files[@]} -eq 0 ]]; then
        info "No .toml files found in '${target}'."
        exit 0
    fi
else
    fatal 1 "Error: '${target}' is not a valid file or directory."
fi

header "Running taplo on ${#files[@]} file(s)"

has_issues=false

for file in "${files[@]}"; do
    info "Checking: ${file}"
    temp_file=$(mktemp)
    if command -v taplo >/dev/null 2>&1; then
        # Use local taplo
        if taplo check "${file}" > "${temp_file}" 2>&1; then
            pass
            rm "${temp_file}"
        else
            has_issues=true
            cat "${temp_file}" >&2
            rm "${temp_file}"
        fi
    else
        # Fall back to Docker
        # shellcheck disable=SC2312
        if docker run --rm -v "$(pwd):/workdir" -w /workdir tamasfe/taplo check "${file}" > "${temp_file}" 2>&1; then
            pass
            rm "${temp_file}"
        else
            has_issues=true
            cat "${temp_file}" >&2
            rm "${temp_file}"
        fi
    fi
done

if [[ "${has_issues}" == true ]]; then
    fatal 1 "Taplo issues detected in one or more files."
else
    info "No taplo issues found in any files."
fi
