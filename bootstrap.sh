#! /bin/sh -e
CFLAGS="-g" ./build.sh

if [ ! -d .tup ]; then
	./build/tup init
fi
./build/tup
echo "Build complete. If ./tup works, you can remove the 'build' directory."
