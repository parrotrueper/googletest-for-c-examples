#!/usr/bin/env bash
# Exit on error
set -eEuo pipefail

# shellcheck source=/dev/null
. ci/functions.sh

# Error handling
trap 'err "ERROR at $0 line ${BASH_LINENO[0]}. Exiting."; exit 1' ERR

# shellcheck source=/dev/null
. .env

run touch .bash_history

# shellcheck disable=SC2093
run_cmd="exec docker run"
run_cmd+=" --rm"
run_cmd+=" --tty"
run_cmd+=" --interactive"
run_cmd+=" --privileged"
run_cmd+=" --env TERM=xterm-256color"
run_cmd+=" --pid=host"
run_cmd+=" -e DISPLAY=${DISPLAY}"
run_cmd+=" --network host"
run_cmd+=" --hostname ${DKR_FW_IMAGE_NAME:?}"
run_cmd+=" --volume /home/${USER}/.ssh:/home/${DKR_FW_USER:?}/.ssh:ro"
run_cmd+=" --volume /etc/timezone:/etc/timezone:ro"
run_cmd+=" --volume /usr/share/zoneinfo/Europe/London:/etc/localtime:ro"
run_cmd+=" --volume /dev:/dev"
run_cmd+=" --volume $(pwd):/workspace/"
run_cmd+=" ${DKR_FW_IMAGE_NAME}"
run_cmd+=" /bin/bash"
echo "${run_cmd}"
run eval "${run_cmd}"



