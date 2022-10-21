import sys

ERRORS = {
        -1:  {"name": "YAML_ERROR", "description": "ERROR: config.yaml file not found! Check the path!"},
        -2:  {"name": "CHANNEL_ERROR", "description": "ERROR: Operating channels 1-128!"},
        -3:  {"name": "FIND_VOLTAGE", "description": "ERROR: Convert voltage to DAC code problem! Please check calib files!"},
        -4:  {"name": "FIND_VOLTAGE", "description": "ERROR: Convert voltage to ADC code problem! Please check calib files!"},
        -5:  {"name": "VOLTAGE_RANGE", "description": "ERROR: Incorrect voltage!"},
        -6:  {"name": "BOARD_INFO", "description": "ERROR: No information about power unit in config.yaml!"},
        -7:  {"name": "BOARD_CONNECTED", "description": "ERROR: Power unit disconected!"},
        -8:  {"name": "IP_ADDRESS", "description": "ERROR: Wrong IP or PORT! Please check config.yaml (Examples: 'localhost:5000' or '159.93.127.19:5001')"}
        }

def error_control(mistake):
    if isinstance(mistake, dict):
        return 0
    if mistake in ERRORS:
        print(ERRORS[mistake]["description"])
        sys.exit()
