#!/usr/bin/env bash

# PURPOSE OF THIS SCRIPT
# 1. Defines local paths for each directory,
# 2. Ensures remote directories exist (via `ssh` and `mkdir -p`),
# 3. Uses `scp` to copy entire directories (skipping top-level hidden files/dirs),
# 4. Ensures a Python venv exists on the remote and installs dependencies from requirements.txt.

# LOCAL BASE DIRECTORY
LOCAL_DIR="REDACTED"

# REMOTE CONNECTION DETAILS
REMOTE_USER="REDACTED"
REMOTE_HOST="REDACTED"
# Use the absolute path to eliminate tilde problems:
REMOTE_DIR="REDACTED"

echo "Ensuring remote directories exist..."
ssh "${REMOTE_USER}@${REMOTE_HOST}" << EOF
  mkdir -p "${REMOTE_DIR}/arduino-code"
  mkdir -p "${REMOTE_DIR}/server"
EOF

echo "Uploading entire arduino-code directory..."
scp -r "${LOCAL_DIR}/arduino-code/"* \
    "${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/arduino-code/"

echo "Uploading entire server directory..."
scp -r "${LOCAL_DIR}/server/"* \
    "${REMOTE_USER}@${REMOTE_HOST}:${REMOTE_DIR}/server/"

echo "Setting up Python virtual environment and installing dependencies..."
ssh "${REMOTE_USER}@${REMOTE_HOST}" << EOF
  # Create venv if it does not exist
  if [ ! -d "${REMOTE_DIR}/venv" ]; then
    echo "Creating Python venv at '${REMOTE_DIR}/venv'..."
    python3 -m venv "${REMOTE_DIR}/venv"
  else
    echo "Venv already exists, skipping creation."
  fi

  # Activate venv and install dependencies
  echo "Activating Python venv and installing requirements..."
  source "${REMOTE_DIR}/venv/bin/activate"
  pip install --upgrade pip
  pip install -r "${REMOTE_DIR}/server/requirements.txt"
EOF

echo "All files and directories have been pushed successfully!"
echo "Deployment complete."
