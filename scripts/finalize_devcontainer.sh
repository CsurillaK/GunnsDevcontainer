SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

# Avoid "dubious ownership" git error
git config --global --add safe.directory ${WORKSPACE_DIR}/gunns

# Make scripts executable by VSCode tasks
chmod +x ${SCRIPT_DIR}/*.sh