import os

# Server address
IP = "server_address:port"

# Board settings
REF = "2.500"   # Reference voltagge. Available values: "1.250", "2.048", "2.500", "4.096"

WORK_DIR =  os.getenv("SUPPLR_CANOPEN_DIR")
PATH_YAML = WORK_DIR + "/supplr_canopen/config.yaml"
