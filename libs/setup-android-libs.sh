#!/bin/fish
mkdir -p deps; cd ./deps
mkdir -p downloaded; cd ./downloaded;

set KEYWORD android
for data in (curl -s https://api.github.com/repos/Qv2ray/Qv2ray-deps/releases/latest | jq ".assets[] | {browser_download_url, name}" -c |  grep $KEYWORD)
    set NAME (echo $data | jq ".name" -r)
    echo "Downloading: $NAME"
    curl -sL (echo $data | jq ".browser_download_url" -r) -o $NAME;
end
cd ..

for f in (ls)
    7z x -y ./$f
end

mkdir final
for p in arm arm64 x64 x86
    cp -rv ./$KEYWORD-$p/installed/* ./final
end

rm -rvf ./final/vcpkg
cd ..
cp -rvf ./deps/final/* ./
