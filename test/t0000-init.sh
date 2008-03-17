#! /bin/sh -e

for i in object delete modify create; do
	if [ ! -d ".tup/$i" ]; then
		echo ".tup/$i not created!" 1>&2
		exit 1
	fi
done

if find .tup/object -type d | wc -l | grep 257 > /dev/null; then
	:
else
	echo "Expecting 256 subdirectories in .tup/object/ !" 1>&2
	exit 1
fi
