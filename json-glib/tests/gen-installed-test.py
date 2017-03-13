import sys
import os
import argparse

def write_template(filename, data):
    with open(filename, 'w') as f:
        f.write(data)

def build_template(testdir, testname):
    return "[Test]\nType=session\nExec={}\n".format(os.path.join(testdir, testname))

argparser = argparse.ArgumentParser(description='Generate installed-test data.')
argparser.add_argument('--testdir', metavar='dir', required=True, help='Installed test directory')
argparser.add_argument('--testname', metavar='name', required=True, help='Installed test name')
argparser.add_argument('--outfile', metavar='file', required=True, help='Output file')
argparser.add_argument('--outdir', metavar='dir', required=True, help='Output directory')
args = argparser.parse_args()

write_template(os.path.join(args.outdir, args.outfile), build_template(args.testdir, args.testname))
