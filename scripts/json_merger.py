#!/usr/bin/env python3

import json
import sys
import argparse
import os
from os.path import dirname, join

def merge_jsons(input_list, outfile):

	print("json_merger: merging the following json files:")

	json_list=[]
	for file in input_list:
		print("\t"+file)
		with open(file, "r") as f:
			json_list.append(json.load(f))
	
	new_json={}
	for this_json in json_list:
		for task in this_json:
			if not task in new_json:
				new_json[task]=this_json[task]
			else:
				for key in this_json[task]:
					#if key in new_json[task]:
						#print("json_merger: WARNING: overwriting "+task+" : "+key+": "+new_json[task][key]+" --> "+this_json[task][key])
					new_json[task][key] = this_json[task][key]

	output = json.dumps(new_json, indent=4,sort_keys=True)

	os.makedirs(dirname(outfile),exist_ok=True)
	with open(outfile, "w") as f:
		f.write(output)



def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("-i", "--input", nargs="*", help="list of input files", required=True)
	parser.add_argument("-o", "--output", help="output file", default=join(os.getcwd(),"merged.json"))
	args = parser.parse_args()

	merge_jsons(args.input,args.output)

if __name__ == "__main__":
	sys.exit(main())