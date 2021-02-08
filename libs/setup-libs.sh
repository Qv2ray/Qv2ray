#!/bin/bash
mkdir -p deps; cd ./deps
mkdir -p downloaded; cd ./downloaded;

DEPS_OS=$1
DEPS_CATEGORY=$2

_JSON=$(curl -s https://api.github.com/repos/Qv2ray/Qv2ray-deps/releases/latest | jq ".assets[] | {browser_download_url, name}" -c | grep "$DEPS_CATEGORY-$DEPS_OS")

echo $_JSON

for data in $(echo $_JSON)
do
    NAME=$(echo $data | jq ".name" -r)
    echo "Downloading: $NAME"
    curl -sL $(echo $data | jq ".browser_download_url" -r) -o $NAME;
done

cd ..

for f in $(ls ./downloaded | grep $DEPS_OS)
do
    7z x -y ./downloaded/$f
done

if [[ "$DEPS_CATEGORY" == "tools" ]]; then
    mkdir -p ../tools
    cp -rvf ./tools ../
    rm -rvf ./tools
else
    echo "Cleaning up $DEPS_CATEGORY-$DEPS_OS"
    rm -rvf ../$DEPS_CATEGORY-$DEPS_OS/*
    mkdir -p ../$DEPS_CATEGORY-$DEPS_OS
    cp -rvf ./$DEPS_OS-$DEPS_CATEGORY/installed/$DEPS_CATEGORY-$DEPS_OS/* ../$DEPS_CATEGORY-$DEPS_OS
    rm -rvf ./$DEPS_OS-$DEPS_CATEGORY/
    cd ..
fi
