#!/bin/sh

LIBDIR=../../libs/

# Check for version number
if [ "$1" = "" ]; then
	VERSION=""
else
	VERSION="-$1"
fi
BUILDDIR=presence_client_lib$VERSION

# Create the $BUILDDIR dir
if [ -d $BUILDDIR ]; then 
	rm -rf $BUILDDIR;
fi
mkdir -p $BUILDDIR/presence/packed_data

# Copy python files into the $BUILDDIR dir.
cp $LIBDIR/python_common/src/*.py $BUILDDIR/presence
cp $LIBDIR/python_common/src/packed_data/*.py $BUILDDIR/presence/packed_data
cp $LIBDIR/python_client_lib/src/presence/*.py $BUILDDIR/presence
cp setup.py $BUILDDIR/

tar cfz $BUILDDIR.tar.gz $BUILDDIR