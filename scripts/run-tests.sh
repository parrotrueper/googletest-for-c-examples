#!/usr/bin/env bash
# Exit on error
set -eEuo pipefail

# shellcheck source=/dev/null
. ci/functions.sh

trap 'err "ERROR at $0 line ${BASH_LINENO[0]}. Exiting."; exit 1' ERR

on_pipeline=$(is_ci)
is_docker=$(is_in_container)
container_cmd=()
debugging_cmd=()
test_code=""
test_list=./scripts/test/tests.json
workers=8

usage() {
    cat <<Endofmessage

    Run off target tests

    Use: $0 [-h] [-d] [-j <workers>] -t <test_code>

    optional arguments:

        -h display this help and exit
        -d don't clean up after build, use for debugging
        -j worker count for ALL-serial runs (default: 8)

    TARGET:

        -t select the test to run, given a test code

Endofmessage

    jq -r '.serial | keys[]' "${test_list}" | while IFS= read -r key; do
        printf '            %s\n' "${key}"
    done

    cat <<Endofmessage

            ALL,                   runs full suite of tests

Endofmessage
}

parse_args() {
    if [[ $# -eq 0 ]]; then
        usage
        fatal 1 'Please specify a target'
    fi
    while [[ $# -gt 0 ]]; do
        case $1 in
            -d)
                debugging_cmd=(-d)
                shift
                ;;
            -j)
                shift
                if [[ $# -eq 0 ]]; then
                    usage
                    fatal 1 'Missing argument for -j'
                fi
                if [[ ! "$1" =~ ^[1-9][0-9]*$ ]]; then
                    usage
                    fatal 1 "Invalid worker count: $1"
                fi
                workers="$1"
                shift
                ;;
            -h)
                usage
                exit 0
                ;;
            -t)
                shift
                if [[ $# -eq 0 ]]; then
                    fatal 1 'Missing argument for -t'
                fi
                test_code=$1
                shift
                ;;
            -*|*)
                usage
                fatal 1 "Unknown option: $1"
                ;;
        esac
    done
}

check_test_dependencies() {
    if [[ ! -f ./scripts/test/.testenv ]]; then
        run ./scripts/test/generate-test-env.sh
    fi

    # generate the version files for serial and parallel tests that require them
    if [[ ${test_code} == ALL ]]; then
        for key in $(jq -r '.serial | keys[]' "${test_list}"); do
            run ./scripts/test/create-test-ver-file.sh -t "${key}"
        done
    else
        run ./scripts/test/create-test-ver-file.sh -t "${test_code}"
    fi
}

determine_runner() {
    local serial_has

    if [[ -z ${test_code} ]]; then
        usage
        fatal 1 'Please specify a target'
    fi

    case ${test_code} in
        ALL)
            runner=serial
            return
            ;;
        *)
            ;;
    esac

    serial_has=$(jq --arg key "${test_code}" '.serial | has($key)' "${test_list}")

    if [[ ${serial_has} == true ]]; then
        runner=serial
        return
    fi

    fatal 1 "Unknown test code: ${test_code}"
}

set_container_cmd() {
    if [[ "${is_docker}" = "yes" ]] || [[ "${on_pipeline}" == "yes" ]]; then
        # shellcheck source=/dev/null
        source /etc/bash.bashrc
        info 'Already in container or pipeline'
        container_cmd=()
    else
        info 'On host pipeline container'
        container_cmd=(./scripts/run-script-in-pipe.sh)
    fi
}

format_target_for_test() {
    if [[ "${on_pipeline}" == "yes" ]]; then
        return
    fi

    case ${test_code} in
        ALL)
            return
            ;;
        *)
            ;;
    esac

    local test_path=""
    local format_target=""

    if [[ ${runner} == serial ]]; then
        test_path=$(jq -r ".serial.${test_code}.path" "${test_list}")
    fi

    if [[ -z "${test_path}" ]] || [[ "${test_path}" == "null" ]]; then
        warn "Skipping pre-test clang-format: no path for ${test_code}"
        return
    fi

    if [[ "${test_path}" == */test_support/test ]]; then
        format_target="${test_path%/test_support/test}"
    elif [[ "${test_path}" == */test ]]; then
        format_target="${test_path%/test}"
    else
        format_target="${test_path}"
    fi

    if [[ ! -d "${format_target}" ]] && [[ ! -f "${format_target}" ]]; then
        warn "Skipping pre-test clang-format: ${format_target} does not exist"
        return
    fi

    header "clang-format (${test_code})"
    run "${container_cmd[@]}" ./scripts/do-clang-format.sh -t "${format_target}"
}

func_run_test() {
    case ${test_code} in
        ALL)
            header 'Full test suite'
            run "${container_cmd[@]}" ./scripts/test/run-serial-tests.sh "${debugging_cmd[@]}" -j "${workers}" -t ALL
            ;;
        *)
            if [[ ${runner} == serial ]]; then
                run "${container_cmd[@]}" ./scripts/test/run-serial-tests.sh "${debugging_cmd[@]}" -j "${workers}" -t "${test_code}"
            fi
            ;;
    esac
}

main() {
    parse_args "$@"

    if [[ "${on_pipeline}" == "yes" ]]; then
        info 'Running in a pipeline'
    else
        info 'Running local'
    fi

    check_test_dependencies
    determine_runner
    set_container_cmd
    format_target_for_test
    func_run_test
}

main "$@"
