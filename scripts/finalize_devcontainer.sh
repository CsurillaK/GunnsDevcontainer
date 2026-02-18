SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

# Avoid "dubious ownership" git error
git config --global --add safe.directory ${WORKSPACE_DIR}/gunns

# Correct for Windows line endings
git config --global core.autocrlf true

# Make scripts executable by VSCode tasks
chown $(whoami) ${SCRIPT_DIR}/*.sh || echo "Warning: chown failed, check file ownership"
chmod +x ${SCRIPT_DIR}/*.sh || echo "Warning: chmod failed, check file permissions"

# Install dependencies missing from Dockerfile
pip3 install --user matplotlib