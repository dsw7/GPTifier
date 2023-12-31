#!/bin/bash

INCLUDE_DIR=$1

if [ -z "$INCLUDE_DIR" ]; then
  echo "Usage: $0 </path/to/include/dir>"
  exit 1
fi

if [ ! -d "$INCLUDE_DIR" ]; then
  echo "$INCLUDE_DIR does not exist"
  exit 1
fi

UNAME_OS=$(uname -s)

if [ "$UNAME_OS" = "Darwin" ]; then
  OWNER=$(stat -f "%Su" "$INCLUDE_DIR")
elif [ "$UNAME_OS" = "Linux" ]; then
  OWNER=$(stat -c %U "$INCLUDE_DIR")
else
  echo "Unrecognized OS: $UNAME_OS"
  exit 1
fi

if [ "$OWNER" = "root" ]; then
  if [ ! $(id -u) = 0 ]; then
    echo "Directory $INCLUDE_DIR is owned by root"
    echo "Elevated privileges will be required to install header files into this directory"
    exit 1
  fi
fi

URL_JSON="https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp"
URL_TOML="https://raw.githubusercontent.com/marzer/tomlplusplus/master/toml.hpp"

SRC_JSON="/tmp/json.hpp"
SRC_TOML="/tmp/toml.hpp"

echo "Downloading nlohmann/json.hpp"
STATUS_JSON=$(curl -o /dev/null -s -w "%{http_code}" "$URL_JSON")

if [ "$STATUS_JSON" -eq 200 ]; then
  curl "$URL_JSON" -o "$SRC_JSON"
else
  echo "Request failed with status code $STATUS_JSON"
  exit 1
fi

echo "Downloading TOML++ toml.hpp"
STATUS_TOML=$(curl -o /dev/null -s -w "%{http_code}" "$URL_TOML")

if [ "$STATUS_TOML" -eq 200 ]; then
  curl "$URL_TOML" -o "$SRC_TOML"
else
  echo "Request failed with status code $STATUS_TOML"
  exit 1
fi

mv -v "$SRC_JSON" "$INCLUDE_DIR"
mv -v "$SRC_TOML" "$INCLUDE_DIR"
