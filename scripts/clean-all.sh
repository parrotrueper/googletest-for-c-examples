#!/usr/bin/env bash
# Exit on error
set -eEuo pipefail

# shellcheck source=/dev/null
. ci/functions.sh

# Error handling
trap 'err "ERROR at $0 line ${BASH_LINENO[0]}. Exiting."; exit 1' ERR

rm -rf .cache
rm -rf .copilot
rm -rf .cortex-debug
rm -rf .dotnet
rm -rf .gnupg
rm -rf .gtest-parallel-times
rm -rf .java
rm -rf .local
rm -rf .parallel
rm -rf .nuget
rm -rf .pki
rm -rf .pytest_cache
rm -rf .stm32cubeide
rm -rf .stm32cubemx
rm -rf .stmcube
rm -rf .stmcufinder
rm -rf .swt
rm -rf .vscode-server
rm -rf STM32CubeIDE
rm -rf ./?

rm -rf config/settings.h
rm -rf config/settings.json
rm -rf config/settings.yml
rm -rf config/autoVersion.c
rm -rf config/autoVersion.h

rm -rf linter/tmp

rm -rf reports

rm -rf org.eclipse.*

rm -rf -- *.bin
rm -rf -- *.elf
rm -rf -- *.zip
rm -rf config.ini

rm -rf .settings

rm -rf pc-lint-report.txt
rm -rf .project


cleanup_build_dirs() {
  local root

  for root in \
    foo \
    bar \
    bah
  do
    find "${root}" -maxdepth 3 -type d \( -name 'build' -o -name 'build_*' \) -exec rm -rf {} +
  done
}

#cleanup_build_dirs

