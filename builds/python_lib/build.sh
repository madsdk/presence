#!/bin/sh

LIBDIR=../../libs/

# Check for version number
if [ "$1" = "" ]; then
	echo "Usage: build.sh version_number";
	exit 1
else
	VERSION="$1"
fi
BUILDDIR=presence_lib-$VERSION

# sed hack...
if [ `uname -s` = "Darwin" ]; then 
	SED="sed -i \"\""
else
	SED="sed -i\"\""
fi

# Create the $BUILDDIR dir
if [ -d $BUILDDIR ]; then 
	rm -rf $BUILDDIR;
fi
mkdir -p $BUILDDIR/presence/packed_data

# Copy python files into the $BUILDDIR dir.
cp $LIBDIR/python_common/src/*.py $BUILDDIR/presence
cp $LIBDIR/python_common/src/packed_data/*.py $BUILDDIR/presence/packed_data
cp $LIBDIR/python/src/presence/*.py $BUILDDIR/presence
cp setup.py $BUILDDIR/
$SED s/VERSION/$VERSION/ $BUILDDIR/setup.py

tar cfz $BUILDDIR.tar.gz $BUILDDIR

exit 0
