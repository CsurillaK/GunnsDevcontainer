NETWORK_FILEPATH="$1"

# Validate input argument
if [[ ! -f "$NETWORK_FILEPATH" || "${NETWORK_FILEPATH##*.}" != "xml" ]]; then
    echo "Error: '$NETWORK_FILEPATH' must be an existing XML GUNNS drawing."
    exit 1
fi
NETWORK_DIR="$(dirname "$NETWORK_FILEPATH")"
NETWORK_FILE="$(basename "$NETWORK_FILEPATH" .xml)"
NETWORK_FILEBASEPATH="${NETWORK_DIR}/${NETWORK_FILE}"

python3 "${GUNNS_HOME}/draw/netexport.py" "$NETWORK_FILEPATH"

if [[ $? -ne 0 ]]; then
    echo ""
    echo "Error: Failed to build network '$NETWORK_FILEPATH'."
    exit 1
fi

SOURCE_DIR="${NETWORK_DIR}/src"
rm -rf $SOURCE_DIR
mkdir -p $SOURCE_DIR
mv "${NETWORK_FILEBASEPATH}.cpp" $SOURCE_DIR
mv "${NETWORK_FILEBASEPATH}.hh" $SOURCE_DIR

# echo "src/" > "${NETWORK_DIR}/.gitignore"

rm -f "${NETWORK_FILEPATH}.bak"