#!/usr/bin/env bash
# Exit on error
set -uo pipefail

# shellcheck source=/dev/null
. ci/functions.sh
# Error handling
trap 'printf "\n\nERROR at $0 line $LINENO. Exiting.\n\n"' ERR

# Function to check if a file is a shell script
is_shell_script() {
    local file="$1"
    # Explicitly exclude PowerShell script files
    if [[ "${file}" == *.ps1 || "${file}" == *.psm1 ]]; then
        return 1
    fi
    # Check if it's a .sh file
    if [[ "${file}" == *.sh ]]; then
        return 0
    fi
    # Check if it has a shebang indicating shell script
    if head -1 "${file}" 2>/dev/null | grep -q '^#!/.*sh'; then
        return 0
    fi
    return 1
}

# Wrapper to avoid set -e issues
check_shell_script() {
    local result
    is_shell_script "$1"
    result=$?
    return "${result}"
}

# Script to print shellcheck issues for a given file or all .sh files in a directory
# Usage: ./shellcheck_printer.sh [file_path | directory_path]
# If no argument, checks all .sh files in current directory recursively

target="${1:-.}"

if [[ -f "${target}" ]]; then
    # Single file
    check_shell_script "${target}"
    result=$?
    if [[ ${result} -ne 0 ]]; then
        fatal 1 "Error: File '${target}' is not a shell script."
    fi
    files=("${target}")
elif [[ -d "${target}" ]]; then
    # Directory, find all files recursively and filter shell scripts
    if command -v git >/dev/null 2>&1 && git rev-parse --git-dir >/dev/null 2>&1; then
        # Use git ls-files to get non-ignored files, but ignore deleted paths
        mapfile -t all_files < <(git ls-files --cached --others --exclude-standard | sed 's|^|./|' | if [[ "${target}" != "." ]]; then grep "^${target}"; else cat; fi | while IFS= read -r file; do
            if [[ -f "${file}" ]]; then
                printf '%s\n' "${file}"
            fi
        done || true)
    else
        # Fallback if git not available or not in git repo
        mapfile -t all_files < <(find "${target}" -type f -not -path '*/.git/*' -not -path '*/venv/*' || true)
    fi
    files=()
    for file in "${all_files[@]}"; do
        if check_shell_script "${file}"; then
            files+=("${file}")
        fi
    done
    if [[ ${#files[@]} -eq 0 ]]; then
        finish_code 0 "No shell scripts found in '${target}'."
    fi
else
    fatal 1 "Error: '${target}' is not a valid file or directory."
fi

header "Running shellcheck on ${#files[@]} file(s)"

if [[ ${#files[@]} -eq 0 ]]; then
    finish_code 0 "No shell scripts found in '${target}'."
fi

if command -v shellcheck >/dev/null 2>&1; then
    info "Running shellcheck locally on all files"
    if shellcheck --help 2>&1 | grep -q -- '^-j\|--jobs'; then
        jobs="$(nproc)"
        if shellcheck -j "${jobs}" -o all -- "${files[@]}" 2>&1; then
            finish_code 0 "No shellcheck issues found in any files."
        fi
    else
        if shellcheck -o all -- "${files[@]}" 2>&1; then
            finish_code 0 "No shellcheck issues found in any files."
        fi
    fi
else
    info "shellcheck not found, using Docker to run shellcheck"
    #shellcheck disable=2312
    if docker run --rm -v "$(pwd):/workdir" -w /workdir koalaman/shellcheck:stable -o all "${files[@]}" 2>&1; then
        finish_code 0 "No shellcheck issues found in any files."
    fi
fi

fatal 1 "Shellcheck issues detected in one or more files."
