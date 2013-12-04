#! /bin/sh

echo "1..1"
echo "# Start of abicheck tests"

cpp -P ${cppargs} ${srcdir:-.}/json-glib/json-glib.symbols | sed -e '/^$/d' -e 's/ G_GNUC.*$//' -e 's/ PRIVATE//' -e 's/ DATA//' | sort > expected-abi

nm -D -g --defined-only .libs/libjson-glib-1.0.so | cut -d ' ' -f 3 | egrep -v '^(__bss_start|_edata|_end)' | sort > actual-abi
diff -u expected-abi actual-abi && rm -f expected-abi actual-abi && echo "ok 1 expected-abi"

echo "# End of abicheck tests"
