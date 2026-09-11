#!/usr/bin/env bash
#shellcheck disable=SC2312
# Exit on error
set -euo pipefail

# Error handling
trap 'printf "\n\nERROR at $0 line $LINENO. Exiting.\n\n"' ERR

# shellcheck source=/dev/null
. ci/functions.sh

compose_env=""
if [[ -f .env ]]; then
  compose_env="--env-file .env"
fi

run docker compose "${compose_env}" -f ./ci/docker-compose.pipe.test.yml down --volumes --remove-orphans || true

info "Nuke old images so we end up with the correct image"

readarray -t images <<< "$(docker images -q 'parrotrueper/arm32_fw_tchain*')"
if [[ ${#images[@]} -gt 0 ]]; then
  docker rmi "${images[@]}" || true
fi
readarray -t images <<< "$(docker images -q 'dev_firmware_testing*')"
if [[ ${#images[@]} -gt 0 ]]; then
  docker rmi "${images[@]}" || true
fi


info "Clean up dependency dir"
rm -rf ./.dependencies
rm -rf ./.env
run scripts/setup.sh
