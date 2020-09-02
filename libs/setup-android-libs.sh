#!/bin/fish
mkdir -p deps; cd ./deps
mkdir -p downloaded; cd ./downloaded;

set KEYWORD android

if test -f downloaded_$KEYWORD
    echo "Download cache for $KEYWORD found, skipping."
else
    for data in (curl -s https://api.github.com/repos/Qv2ray/Qv2ray-deps/releases/latest | jq ".assets[] | {browser_download_url, name}" -c |  grep $KEYWORD)
        set NAME (echo $data | jq ".name" -r)
        echo "Downloading: $NAME"
        curl -sL (echo $data | jq ".browser_download_url" -r) -o $NAME;
    end
    touch downloaded_$KEYWORD
end

cd ..

for f in (ls ./downloaded | grep $KEYWORD)
    7z x -y ./downloaded/$f
end

for p in arm arm64 x64 x86
    echo "Cleaning up $p-$KEYWORD"
    rm -rvf ../$p-$KEYWORD/*
    cp -rvf ./$KEYWORD-$p/installed/$p-$KEYWORD/* ../$p-$KEYWORD
    rm -rvf ./$KEYWORD-$p/
end
cd ..
