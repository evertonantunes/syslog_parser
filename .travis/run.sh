#!/bin/bash

set -e
set -x

if [[ "$(uname -s)" == 'Darwin' ]]; then
    if which pyenv > /dev/null; then
        eval "$(pyenv init -)"
    fi
    pyenv activate conan
fi

conan remote clean
conan remote add remote_repository $REPOSITORY
conan user $USER -r remote_repository -p $PASSWORD

conan install . --build=missing
conan upload boost/1.66.0@conan/stable -r=remote_repository --all

mkdir -p ./build
cd ./build

cmake ..
cmake --build . --target all
cmake --build . --target test

