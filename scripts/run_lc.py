#!/usr/bin/env python3

import sys
import os
import argparse
from os.path import abspath, exists, join
import json_merger


MERGED_CONF_NAME = "merged_config.json"
LOG_NAME = "logfile.txt"
DEFAULT_CONFS_PATH="/home/daniel/alice/MyStuff/Lc_tests/configs/defaults"
WRITER_CONF="writer.json"
MINIMAL_CONF="minimal.json"
DEFAULT_CONF="default.json"
BAYES_CONF="bayes.json"
MC_CONF="mc.json"


# TODO: write AnalysisResults_tree.root with tree analyzer only when reduced tree is filled

def append_tasks(args):

  if args.analyse_tree:
    return ["o2-analysis-hf-task-candidate-selector-lc-to-k0s-p-tree", "o2-analysis-hf-task-lc-to-k0s-p-tree"]

  tasks_common = [
  "o2-analysis-event-selection",
  "o2-analysis-pid-tpc-base",
  "o2-analysis-track-to-collision-associator",
  "o2-analysis-pid-tpc-full",
  "o2-analysis-trackselection",
  "o2-analysis-pid-tof-full",
  "o2-analysis-pid-tof-base",
  "o2-analysis-timestamp"]

  tasks_hf = [
  "o2-analysis-hf-track-index-skim-creator",
  "o2-analysis-lf-lambdakzerobuilder",
  "o2-analysis-hf-task-lc-to-k0s-p",
  "o2-analysis-hf-candidate-selector-lc-to-k0s-p",
  "o2-analysis-hf-candidate-creator-cascade"]

  if not args.track_extension:
    tasks_common.append("o2-analysis-track-propagation")
  else:
    tasks_common.append("o2-analysis-trackextension")

  if args.tree_creator:
    tasks_hf.append("o2-analysis-hf-tree-creator-lc-to-k0s-p")

  if args.bayes_pid:
    tasks_hf.append("o2-analysis-pid-bayes")
    tasks_hf.append("o2-analysis-multiplicity-table")

  if args.zdc_converter:
    tasks_common.append("o2-analysis-zdc-converter")
  if args.collision_converter:
    tasks_common.append("o2-analysis-collision-converter")
  if args.mc_converter:
    tasks_common.append("o2-analysis-mc-converter")
  if args.fdd_converter:
    tasks_common.append("o2-analysis-fdd-converter")

  tasks = tasks_hf + tasks_common

  return tasks


def get_config(args):

  all_configs=[]

  if args.m:
    all_configs.append(join(DEFAULT_CONFS_PATH,MINIMAL_CONF))
    print("including config file "+MINIMAL_CONF)
  if args.d:
    all_configs.append(join(DEFAULT_CONFS_PATH,DEFAULT_CONF))
    print("including config file "+DEFAULT_CONF)
  if args.mc:
    all_configs.append(join(DEFAULT_CONFS_PATH,MC_CONF))
    print("including config file "+MC_CONF)
  if args.bayes_pid:
    all_configs.append(join(DEFAULT_CONFS_PATH,BAYES_CONF))
    print("including config file "+BAYES_CONF)

  for user_conf in args.config:
    all_configs.append(user_conf)
  if len(all_configs)==1:
    return all_configs[0]
  else:
    merged_config_name = join(args.output,MERGED_CONF_NAME)
    json_merger.merge_jsons(all_configs,merged_config_name)
    print("\nusing merged config file " + merged_config_name)
    return merged_config_name

  



def execute_command(tasks, aod, config, output_dir, tree_creator, logfile, monitoring):
  if aod[0]=="@":
    aod="@"+abspath(aod[1:])
  else:
    aod = abspath(aod)
  config = abspath(config)
  output_dir = abspath(output_dir)
  current_dir = abspath(os.getcwd())

  commandToRun ="" 
  for task in tasks:
    commandToRun += task + " --configuration json://" + config + " -b | "
  commandToRun = commandToRun[:-3]
  commandToRun += " --aod-file "+ aod
  if tree_creator:
    commandToRun += " --aod-writer-json "+join(DEFAULT_CONFS_PATH,WRITER_CONF)
  if monitoring:
    commandToRun += " --resources-monitoring 2"

  if logfile:
    commandToRun += " > " + LOG_NAME


  if not exists(output_dir):
    os.makedirs(output_dir)

  print("\nincluded tasks:")
  for task in tasks:
    print(f"{task}")
  print(" \nfull command to run:")
  print(commandToRun+"\n")
  if logfile:
    print(f"running... \n")

  os.chdir(output_dir)
  os.system(commandToRun)
  os.chdir(current_dir)
  os.system("notify-send 'TASK FINISHED'")



def main():
  parser = argparse.ArgumentParser()
  parser.add_argument("-i", "--aod", help="input AO2D file", required=True)
  parser.add_argument("-c", "--config", nargs="*", help="json config file(s)", required=True)
  parser.add_argument("-o", "--output", help="output directory", default=os.getcwd())
  parser.add_argument("--logfile", help="writer to log file", action="store_true")
  parser.add_argument("--tree-creator", dest="tree_creator", help="enable tree creator", action="store_true")
  parser.add_argument("--zdc-converter", dest="zdc_converter", help="zdc converter", action="store_true")
  parser.add_argument("--collision-converter", dest="collision_converter", help="collision converter", action="store_true")
  parser.add_argument("--mc-converter", dest="mc_converter", help="mc converter", action="store_true")
  parser.add_argument("--fdd-converter", dest="fdd_converter", help="fdd converter", action="store_true")
  parser.add_argument("--track-extension", dest="track_extension", help="track extension instead of track propagation", action="store_true")
  parser.add_argument("--m", help="add minimal config", action="store_true")
  parser.add_argument("--d", help="add default config", action="store_true")
  parser.add_argument("--mc", help="add minimal mc config", action="store_true")
  parser.add_argument("--resources-monitoring",dest="resources_monitoring", help="enable resources monitoring", action="store_true")
  parser.add_argument("--bayes-pid", dest="bayes_pid", help="run with Bayes pid", action="store_true")
  args = parser.parse_args()

  tasks = append_tasks(args)

  config = get_config(args)

  execute_command(tasks, args.aod, config, args.output, args.tree_creator, args.logfile, args.resources_monitoring)

  return 0


if __name__ == "__main__":
    sys.exit(main())