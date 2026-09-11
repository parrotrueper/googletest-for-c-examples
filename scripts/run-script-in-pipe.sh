#!/usr/bin/env bash
# Exit on error
set -euo pipefail

# Helper script, spins up the docker container and runs the command or script
# specified.

# shellcheck source=/dev/null
. ci/functions.sh
# Error handling
trap 'printf "\n\nERROR at $0 line $LINENO. Exiting.\n\n"' ERR

if [[ $# -eq 0 ]]; then
	script="/bin/bash"
else
	script=$*
fi

# shellcheck source=/dev/null
. .env

run_cmd="docker run"
run_cmd+=" --hostname fwpipe"
run_cmd+=" --rm"
# check if stdin or stdout are currently open
if [[ -t 0 ]] && [[ -t 1 ]]; then
    run_cmd+=" -it"
else
    run_cmd+=" -i"
fi
run_cmd+=" --user ${USER_ID:?}:${GROUP_ID:?}"
run_cmd+=" --volume $(pwd):/workspace/"
run_cmd+=" ${CI_BASE_IMAGE:?}"
run_cmd+=" ${script}"

info "${run_cmd}"
#echo "${run_cmd}"
eval "${run_cmd}"
return_value=$?
if [[ ${return_value} -ne 0 ]]; then
	fatal 1 "${return_value}" "${script} exited with error code ${return_value}"
fi
