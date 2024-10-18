#!/bin/bash

# Path to the CMakeLists.txt file (you can also pass it as an argument)
CMAKE_FILE="../CMakeLists.txt"

# Function to extract value from a set command in CMakeLists.txt
extract_value() {
    local variable_name=$1
    # Grep the value, then remove quotation marks (if any) using sed
    grep "set(${variable_name}" "$CMAKE_FILE" | sed -E "s/set\(${variable_name} ([^\)]+)\)/\1/" | sed 's/"//g'
}

# Extract values from the CMakeLists.txt file
MAJOR=$(extract_value "MEXLIBCZI_MAJOR")
MINOR=$(extract_value "MEXLIBCZI_MINOR")
PATCH=$(extract_value "MEXLIBCZI_PATCH")
EXT=$(extract_value "MEXLIBCZI_EXT")

# Combine the version string
if [[ -n "$EXT" ]]; then
    VERSION_STRING="${MAJOR}.${MINOR}.${PATCH}-${EXT}"
else
    VERSION_STRING="${MAJOR}.${MINOR}.${PATCH}"
fi

# Output the result
echo "$VERSION_STRING"
