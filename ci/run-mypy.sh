#!/usr/bin/env bash
# Exit on error
set -euo pipefail

# shellcheck source=/dev/null
. ci/functions.sh
# Error handling
trap 'printf "\n\nERROR at $0 line $LINENO. Exiting.\n\n"' ERR

header "Running mypy type checks"

if command -v mypy >/dev/null 2>&1; then
    # Use local mypy
    info "Using local mypy"
    mypy src
else
    # Fall back to Docker
    info "Using Docker mypy"
	#shellcheck disable=2312
    run docker run --rm -v "$(pwd):/workdir" -w /workdir python:3.13-slim sh -c "pip install mypy && mypy ./"
fi

info "Mypy checks completed successfully"
