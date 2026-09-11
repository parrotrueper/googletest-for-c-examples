#!/usr/bin/env bash
# Exit on error
set -eEuo pipefail

# shellcheck source=/dev/null
. ci/functions.sh

# Error handling
trap 'err "ERROR at $0 line ${BASH_LINENO[0]}. Exiting."; exit 1' ERR

target="all"

# Help function
#--------------
# shellcheck disable=SC2317  # Don't warn about unreachable commands in this function
usage() {
    cat <<Endofmessage
    Run clang-format on all source files (default) or on a specified directory,
    or file.
    Use: $0  [-h] [-t <directory_or_file>]

    optional arguments:

        -h display this help and exit

        -t specify a file or directory to run clang-format on

Endofmessage
}

# parse input arguments
#----------------------
ParseArgs() {
    while [[ $# -gt 0 ]]; do
        arg="$1"
        case ${arg} in
        -h)
            usage
            exit 0
            ;;
        -t)
            shift
            if [[ $# -eq 0 ]]; then
                fatal 1 'Missing argument for -t'
            fi
            target="$1"
            ;;
        -* | *)
            echo "Unknown argument: ${arg}"
            usage
            exit 1
            ;;
        esac
        shift
    done
}


fix_unsigned_decorators() {
    local file="$1"
    perl -pi -e 's/\b(0x[0-9A-Fa-f]+)u\b/$1U/g; s/(?<!%)\b([0-9]+)u\b/$1U/g' "${file}"
}

do_clang_fix() {
    local jobs
    jobs="$(nproc 2>/dev/null || echo 1)"

    local exclude_paths=(
        '*/HAL'
    )

    local find_excludes=( )
    for exclude in "${exclude_paths[@]}"; do
        find_excludes+=( -path "${exclude}" -prune -o )
    done

    find "$@" "${find_excludes[@]}" -type f \( -iname '*.h' -o -iname '*.c' -o -iname '*.cpp' \) -print0 \
        | xargs -0 -r -n 1 -P "${jobs}" clang-format -i

    # shellcheck disable=SC2016
    find "$@" "${find_excludes[@]}" -type f \( -iname '*.h' -o -iname '*.c' -o -iname '*.cpp' \) -print0 \
        | xargs -0 -r -n 1 -P "${jobs}" perl -pi -e 's/\b(0x[0-9A-Fa-f]+)u\b/$1U/g; s/(?<!%)\b([0-9]+)u\b/$1U/g'
}

fix_all_source_files() {
    run do_clang_fix \
        lib/common \
        lib/drivers \
        example/application
}

# script starts here
#===================
main() {
    ParseArgs "$@"
    if [[ "${target}" = "all" ]]; then
        run fix_all_source_files
    elif [[ -d "${target}" ]]; then
        run do_clang_fix "${target}"
    elif [[ -f "${target}" ]]; then
        run clang-format -i "${target}"
        run fix_unsigned_decorators "${target}"
    else
        fatal 1 "${target} is not a valid file or directory."
    fi

}

main "$@"
