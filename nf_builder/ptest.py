import json
import sys
import argparse
parser = argparse.ArgumentParser()
parser.add_argument('-app', default='app.json', type=argparse.FileType('r'))
parser.add_argument('-cmp', type=argparse.FileType('r'))
parser.add_argument('-out', default='Makefile', type=argparse.FileType('w'))
args = parser.parse_args()
if not args.app is None:
    js = args.app.read()
    jso = json.loads(js)

