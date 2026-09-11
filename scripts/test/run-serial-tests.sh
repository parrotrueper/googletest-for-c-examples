#!/usr/bin/env bash
# Exit on error
set -euo pipefail

# shellcheck source=/dev/null
. ci/functions.sh
# Error handling
trap 'printf "\n\nERROR at $0 line $LINENO. Exiting.\n\n"' ERR

# shellcheck source=/dev/null
source ./scripts/test/.testenv
# shellcheck source=/dev/null
source ./scripts/test/should-lint.sh

# environment variables for gtest
export GTEST_COLOR
export GTEST_OUTPUT_ON_FAILURE
export GTEST_OUTPUT
export GTEST_BREAK_ON_FAILURE

# directory locations
#--------------------
report_dir="./reports"
benchmark="no"
build_all="yes"
debugging="no"
test_list=./scripts/test/tests.json
target=""
workers=12
linting=$(should_lint)
is_docker=$(is_in_container)

usage() {
    cat <<Endofmessage

Run off-target serial tests

Use: $0 [-d] [-j <workers>] -t <target|ALL>

optional arguments:
    -d don't clean up after build, use for debugging
    -j worker count for ALL-serial runs (default: 1)
    -t test target to run
    -h display this help and exit

Endofmessage
}

# parse input arguments
#----------------------
ParseArgs(){
    local arg

    while [[ $# -gt 0 ]]; do
        arg="$1"
        case ${arg} in
            -d)
                debugging="yes"
                shift
                ;;
            -j)
                shift
                if [[ $# -eq 0 ]]; then
                    fatal 1 'Missing argument for -j'
                fi
                if [[ ! "$1" =~ ^[1-9][0-9]*$ ]]; then
                    fatal 1 "invalid worker count: $1"
                fi
                workers="$1"
                shift
                ;;
            -t)
                shift
                if [[ $# -eq 0 ]]; then
                    fatal 1 'Missing argument for -t'
                fi
                build_all="no"
                have_key="$(jq --arg key "$1" '.serial | has($key)' "${test_list}")"
                if [[ "true" = "${have_key}" ]]; then
                    target="$1"
                else
                    case $1 in
                        ALL|ALL-serial)
                            build_all="yes"
                            ;;
                        *)
                            fatal 1 "unknown ${arg}"
                            ;;
                    esac
                fi
                shift
                ;;
            -h)
                usage
                exit 0
                ;;
            -* )
                fatal 1 "unknown ${arg}"
                ;;
            *)
                fatal 1 "unknown ${arg}"
                ;;
        esac
    done
}

# the test runner
#----------------
funcNinja(){
    local tst_path="$1"
    local tst_target="$2"
    local skip_test="$3"
    local have_pytests="$4"
    local cleanup_now="${5:-yes}"
    local here_dir
    local build_dir
    local start_ts
    local end_ts
    local elapsed

    start_ts=$(date +%s)

    if [[ "${tst_path}" = "null" ]] || [[ "${skip_test}" = "null" ]]; then
        fatal 1 "path=${tst_path}, skip=${skip_test}"
    fi

    if [[ ${skip_test} = "no" ]]; then
        here_dir=$(realpath ./)
        build_dir=$(realpath "./${tst_path}/build_${tst_target}")

        header "${tst_path}"

        run rm -rf "${build_dir}"
        run mkdir "${build_dir}"
        run cd "${build_dir}"
            make_cmd="cmake"
            make_cmd+=" -G Ninja"
            make_cmd+=" -DCMAKE_C_COMPILER=${cc_ver:?}"
            make_cmd+=" -DCMAKE_CXX_COMPILER=${cppc_ver:?}"
            make_cmd+=" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../"
            eval "${make_cmd}"
            ninja -j "${workers}" -k0
            # check for python tests first
            if [[ "${have_pytests}" = "yes" ]]; then
                info "Running Python tests (ctest -R '_py$')"
                if ! ctest --stop-on-failure --output-on-failure -R '_py$'; then
                    err "Python tests failed. Re-running failed tests with verbose output"
                    ctest --rerun-failed --output-on-failure -VV || true
                    end_ts=$(date +%s)
                    elapsed=$((end_ts - start_ts))
                    err "TIME ${tst_target} ${elapsed}s"
                    return 1
                fi
            fi
        # check if we need to run the linter
        if [[ "${debugging}" = "no" ]] && [[ "${linting}" = "yes" ]]; then
            ninja lint
        fi
        # check if we need to clean up the build dir
        run cd "${here_dir}"
        if [[ "${debugging}" = "no" ]] && [[ "${cleanup_now}" = "yes" ]]; then
            run rm -rf "${build_dir}"
        fi
        # print the time it took to run the test
        end_ts=$(date +%s)
        elapsed=$((end_ts - start_ts))
        info "TIME ${tst_target} ${elapsed}s"
    else
        if [[ "${build_all}" == "yes" ]]; then
            warn "TEST DID NOT RUN [${tst_path}]"
        fi
    fi
}

funcRunOffTgtTests(){
  if [[ ${build_all} = "yes" ]]; then
    declare -a keys=()
    declare -a running_pids=()
    declare -A pid_to_target=()
    declare -A pid_to_log=()
    declare -a passed_targets=()
    declare -a failed_targets=()
    declare -a cleanup_dirs=()

    keys_raw="$(jq -r '.serial | keys[]' "${test_list}")"
    mapfile -t keys <<<"${keys_raw}"
    info "Running ${#keys[@]} serial targets with ${workers} workers"

    for key in "${keys[@]}"; do
        test_path="$(jq -r ".serial.${key}.path" "${test_list}")"
        pytests="$(jq -r ".serial.${key}.pytests" "${test_list}")"
        log_file="${report_dir}/serial_${key}.log"
        cleanup_dirs+=("./${test_path}/build_${key}")

        info "START ${key} -> ${test_path} (log: ${log_file})"
        skip="$(jq -r ".serial.${key}.skip" "${test_list}")"
        (
            funcNinja "${test_path}" "${key}" "${skip}" "${pytests}" "no"
        ) >"${log_file}" 2>&1 &

        pid=$!
        running_pids+=("${pid}")
        pid_to_target["${pid}"]="${key}"
        pid_to_log["${pid}"]="${log_file}"

        running_jobs=$(jobs -rp | wc -l)
        while [[ ${running_jobs} -ge ${workers} ]]; do
            sleep 0.2
            running_jobs=$(jobs -rp | wc -l)
        done
    done

    for pid in "${running_pids[@]}"; do
        if wait "${pid}"; then
            passed_targets+=("${pid_to_target[${pid}]}")
            info "PASS ${pid_to_target[${pid}]}"
        else
            failed_targets+=("${pid_to_target[${pid}]}")
            err "FAIL ${pid_to_target[${pid}]} (log: ${pid_to_log[${pid}]})"
        fi
    done

    info "Serial summary: ${#passed_targets[@]} passed, ${#failed_targets[@]} failed"
    if [[ ${#passed_targets[@]} -ne 0 ]]; then
        info "Passed targets: ${passed_targets[*]}"
    fi
    if [[ ${#failed_targets[@]} -ne 0 ]]; then
        err "Failed targets: ${failed_targets[*]}"
        for tgt in "${failed_targets[@]}"; do
            log_path="${report_dir}/serial_${tgt}.log"
            err "Failure log: ${log_path}"
            err "------ ${tgt} log output ------"
            cat "${log_path}" || true
            err "------ end ${tgt} log output ------"
        done
        if [[ "${debugging}" = "no" ]]; then
            for cleanup_dir in "${cleanup_dirs[@]}"; do
                rm -rf "${cleanup_dir}"
            done
        fi
        return 1
    else
        info "Cleaning up log files"
        for tgt in "${passed_targets[@]}"; do
            log_path="${report_dir}/serial_${tgt}.log"
            rm -f "${log_path}"
        done
    fi

    if [[ "${debugging}" = "no" ]]; then
        for cleanup_dir in "${cleanup_dirs[@]}"; do
            rm -rf "${cleanup_dir}"
        done
    fi

    if [[ ${benchmark} = "yes" ]]; then
        serial_time_report="${report_dir}/serial_slowest_targets.txt"
        serial_logs=("${report_dir}"/serial_*.log)
        if [[ ${#serial_logs[@]} -gt 0 ]]; then
            printf 'Slowest serial targets (by elapsed seconds):\n' > "${serial_time_report}"
            grep -h 'TIME ' "${serial_logs[@]}" | sed -E 's/.*TIME ([^ ]+) ([0-9]+)s.*/\1 \2/' | sort -k2 -nr | awk '{printf("%-25s %ss\n", $1, $2)}' >> "${serial_time_report}"
            info "Slowest serial target report written to ${serial_time_report}"
        fi
    fi

  else
    info "${target}"
    test_path="$(jq -r ".serial.${target}.path" "${test_list}")"
    skip="$(jq -r ".serial.${target}.skip" "${test_list}")"
    pytests="$(jq -r ".serial.${target}.pytests" "${test_list}")"
    run funcNinja "${test_path}" "${target}" "${skip}" "${pytests}" "yes"
  fi
}

# script starts here
#===================
main(){
    on_pipeline=$(is_ci)
    if [[ "${on_pipeline}" = "yes" ]] || [[ "${is_docker}" = "yes" ]]; then
        ParseArgs "$@"
        mkdir -p "${report_dir}"
        funcRunOffTgtTests
    else
        err "Do not invoke this script directly!"
        warn "use ./scripts/run-tests.sh"
        fatal 1 "Aborting..."
    fi
    info "On pipeline: ${on_pipeline}, linting: ${linting}, debugging: ${debugging}, build_all: ${build_all}, workers: ${workers}"
}

main "$@"
