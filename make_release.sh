#!/bin/sh
test -n "$srcdir" || srcdir=$(dirname "$0")
test -n "$srcdir" || srcdir=.

cd $srcdir

PROJECT=json-glib
VERSION=$(git describe --abbrev=0)

grep "version: '${VERSION}'" meson.build || { echo "*** Missing version bump (required version: ${VERSION}) ***"; exit 1; }

NAME="${PROJECT}-${VERSION}"

rm -f "${NAME}.tar"

echo "Creating git tree archive…"
git archive --prefix="${NAME}/" --format=tar HEAD > ${NAME}.tar

echo "Compressing archive…"
xz -f "${NAME}.tar"
