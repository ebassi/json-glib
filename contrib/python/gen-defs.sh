#!/bin/sh

python `pkg-config pygtk-2.0 --variable=codegendir`/h2def.py \
./json-glib/*.h \
> json-glib.defs.new

# Remove gtypes from the enums
./fix.pl json-glib.defs.new > json-glib.defs
rm json-glib.defs.new

# Add the BeagleHit, BeagleProperty and BeagleTimestamp pointers

