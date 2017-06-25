#!/usr/bin/env bash
TERM=linux
export TERM

set -e
cd $(dirname $0)
./setup.sh
venv/bin/python3 iac.py "$@"
