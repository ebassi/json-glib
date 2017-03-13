#!/usr/bin/python
#
# Utility script to generate .pc files for JSON-GLib
# for Visual Studio builds, to be used for
# building introspection files

# Author: Fan, Chun-wei
# Date: March 10, 2016

import os
import sys

from replace import replace_multi
from pc_base import BasePCItems

def main(argv):
    base_pc = BasePCItems()

    base_pc.setup(argv)

    # Generate json-glib.pc
    replace_multi(base_pc.top_srcdir + '/json-glib/json-glib.pc.in',
                  base_pc.srcdir + '/json-glib.pc',
                  base_pc.base_replace_items)

if __name__ == '__main__':
    sys.exit(main(sys.argv))
