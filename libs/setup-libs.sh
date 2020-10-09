#!/bin/bash
mkdir -p deps; cd ./deps
mkdir -p downloaded; cd ./downloaded;

# QV2RAY_LIBS=tools
# QV2RAY_LIBS_ARCH=arm arm64 x64 x86

for data in $(curl -s https://api.github.com/repos/Qv2ray/Qv2ray-deps/releases/latest | jq ".assets[] | {browser_download_url, name}" -c |  grep $QV2RAY_LIBS)
do
    NAME=$(echo $data | jq ".name" -r)
    echo "Downloading: $NAME"
    curl -sL $(echo $data | jq ".browser_download_url" -r) -o $NAME;
done

cd ..

for f in $(ls ./downloaded | grep $QV2RAY_LIBS)
do
    7z x -y ./downloaded/$f
done

if [[ "$QV2RAY_LIBS" == "tools" ]]; then
    mkdir -p ../tools
    cp -rvf ./tools ../
    rm -rvf ./tools
else
    for p in $QV2RAY_LIBS_ARCH
    do
        echo "Cleaning up $p-$QV2RAY_LIBS"
        rm -rvf ../$p-$QV2RAY_LIBS/*
        mkdir -p ../$p-$QV2RAY_LIBS
        cp -rvf ./$QV2RAY_LIBS-$p/installed/$p-$QV2RAY_LIBS/* ../$p-$QV2RAY_LIBS
        rm -rvf ./$QV2RAY_LIBS-$p/
    done
    cd ..
fi
