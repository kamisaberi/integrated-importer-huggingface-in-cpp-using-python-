#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# The path to the requirements.txt file is now the first argument
if [ -z "$1" ]; then
    echo "Error: Path to requirements.txt was not provided."
    exit 1
fi
REQUIREMENTS_FILE="$1"

VENV_DIR="cpp_venv"
PYTHON_EXE="$VENV_DIR/bin/python3"
PIP_EXE="$VENV_DIR/bin/pip"

# Check if the virtual environment directory exists
if [ ! -d "$VENV_DIR" ]; then
    echo "--- Python virtual environment not found. Creating one at '$VENV_DIR'... ---"
    python3 -m venv "$VENV_DIR"
    echo "--- Virtual environment created. ---"
else
    echo "--- Python virtual environment already exists. ---"
fi

# Install/update dependencies from the provided requirements.txt path
echo "--- Installing/updating Python dependencies from '$REQUIREMENTS_FILE'... ---"
"$PIP_EXE" install -r "$REQUIREMENTS_FILE"

echo "--- Python setup is complete. ---"