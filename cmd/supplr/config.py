import os

# Server address
IP = "159.93.78.18:5000"

# Need add CANOPEN_DIR in /.bashrc !!!!
WORK_DIR =  os.getenv("CANOPEN_DIR")
PATH_YAML = WORK_DIR + "/canopen/config.yaml"
