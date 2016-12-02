#!/bin/sh

for i in `seq 400`
do
    echo -n "$i - "
    wget -qO- https://web.archive.org/web/20121127222304/http://dockapps.windowmaker.org/file.php/id/$i |\
	grep -Po "(?<=<title>).*(?= - dock)"
    echo ""
done
