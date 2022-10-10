import csv
import requests
import yaml
from time import sleep
from os.path import abspath

from supplr import convert_voltage
from supplr import errors

ATTEMPTS = 5
NODE_ID_MAX = 128
CALIB_PATH = "/supplr_canopen/cmd/calib_files"
CONFIG = "/supplr_canopen/config.yaml"


def get_app_path():
    path = abspath(__file__)
    path_split = path.split('/')
    app_path = ''
    for i, word in enumerate(path_split):
        app_path += word
        if i>len(path_split)-6:
            break
        app_path += '/'
    return app_path

def get_config_path():
    app_path = get_app_path()
    path_to_config = app_path + CONFIG
    return path_to_config

def get_calib_path():
    app_path = get_app_path()
    path_to_dir_calib = app_path + CALIB_PATH
    return path_to_dir_calib

def convert_node_to_board_sn(node):
    if node<NODE_ID_MAX:
        data = parse_yaml(get_config_path())
        errors.error_control(data)
        for fnode in data["Nodes"]:
            if node == fnode["NodeID"]:
                return fnode["BoardSN"]
        return -6
    return node

def parse_yaml(path_to_config):
    try:
        with open(path_to_config, "r") as stream:
            data = yaml.safe_load(stream)
            return data
    except:
        return -1

def check_boards(board_sn):
    board_sn = convert_node_to_board_sn(board_sn)
    data = parse_yaml(get_config_path())
    errors.error_control(data)
    available_boards = []
    for item in data["Nodes"]:
        available_boards.append(item["BoardSN"])
    if board_sn in available_boards:
        return 0
    else:
        return -6

def check_channel(channel):
    if 1<channel>128:
        return -2

def check_voltage(voltage):
    ref = parse_yaml(get_config_path())['RefVoltage']
    if ref=="1.250":
        if voltage < 1 or voltage > 60:
            return -5
    elif ref=="2.500":
        if voltage < 1 or voltage > 123:
            return -5
    elif ref=="4.096":
        if voltage < 1 or voltage > 203:
            return -5

def get_node(board_sn):
    data = parse_yaml(get_config_path())
    errors.error_control(data)
    for node in data["Nodes"]:
        if board_sn == node["BoardSN"]:
            return node["NodeID"]
    return -6

def get_server_address():
    path_to_config = get_config_path()
    server_address = parse_yaml(path_to_config)['ServerAddress']
    return server_address

def find_volt_to_bit(board_sn, channel, voltage):
    try:
        cal_dir = get_calib_path()
        ref = parse_yaml(get_config_path())['RefVoltage']
        errors.error_control(cal_dir)
        file_name = cal_dir + "/board_" + str(board_sn) + "_ref" + ref + "/board_"+str(board_sn)+"_channel_"+str(channel)+"_rec.txt"
        bit = convert_voltage.volt_to_bit(file_name, voltage)
        errors.error_control(bit)
        return bit
    except:
        return -3

def find_ADC_to_volt_channel(board_sn, channel, ADC_code):
    try:
        if ADC_code<20000:
            return 0.3
        cal_dir = get_calib_path()
        ref = parse_yaml(get_config_path())['RefVoltage']
        errors.error_control(cal_dir)
        file_name = cal_dir + "/board_" + str(board_sn) + "_ref" + ref + "/board_"+str(board_sn)+"_channel_"+str(channel)+"_rec.txt"
        voltage = convert_voltage.ADC_code_to_volt(file_name, ADC_code)
        errors.error_control(voltage)
        return voltage
    except:
        return -4

def set_channel(board_sn, channel, voltage):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    errors.error_control(check_channel(channel))
    errors.error_control(check_voltage(voltage))
    DAC_code = find_volt_to_bit(board_sn, channel, voltage)
    errors.error_control(DAC_code)
    node = get_node(board_sn)
    errors.error_control(node)
    IP = parse_yaml(get_config_path())['ServerAddress']
    data = {"node": node, "channel": channel, "DAC_code": str(DAC_code)}
    url ="http://" + IP + "/api/voltage/" + str(node) + "/" + str(channel)
    with requests.post(url, json = data) as resp:
        message = f"status code: {resp.status_code}"
        if resp.status_code != 200:
            print(f"ERROR: {message}")
            return 0

def set_channel_bit(board_sn, channel, DAC_code):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    errors.error_control(check_channel(channel))
    node = get_node(board_sn)
    errors.error_control(node)
    IP = parse_yaml(get_config_path())['ServerAddress']
    data = {"node": node, "channel": channel, "DAC_code": str(DAC_code)}
    url ="http://" + IP + "/api/voltage/" + str(node) + "/" + str(channel)
    with requests.post(url, json = data) as resp:
        message = f"status code: {resp.status_code}"
        if resp.status_code != 200:
            print(f"ERROR: {message}")
            return 0

def set_channels_bit(board_sn, DAC_code):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    IP = parse_yaml(get_config_path())['ServerAddress']
    for channel in range(1,NODE_ID_MAX + 1):
        data = {"node": node, "channel": channel, "DAC_code": str(DAC_code)}
        url ="http://" + IP + "/api/voltage/" + str(node) + "/" + str(channel)
        with requests.post(url, json = data) as resp:
            message = f"status code: {resp.status_code}"
            if resp.status_code != 200:
                print(f"ERROR: {message}")
                return 0

def set_channels(board_sn, voltage):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    errors.error_control(check_voltage(voltage))
    node = get_node(board_sn)
    errors.error_control(node)
    channels = range(1, NODE_ID_MAX + 1)
    IP = parse_yaml(get_config_path())['ServerAddress']
    print("Wait...")
    for channel in channels:
        DAC_code = find_volt_to_bit(board_sn, channel, voltage)
        if DAC_code<0:
            print(f"Problem: {channel}")
        data = {"node": node, "channel": channel, "DAC_code": str(DAC_code)}
        url ="http://" + IP + "/api/voltage/" + str(node) + "/" + str(channel)
        with requests.post(url, json = data) as resp:
            message = f"status code: {resp.status_code}"
            if resp.status_code != 200:
                print(f"ERROR: {message}")
                return 0
    sleep(15)
    print("Done!")

def read_channel(board_sn, channel):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    errors.error_control(check_channel(channel))
    node = get_node(board_sn)
    errors.error_control(node)
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/voltage/" + str(node) + "/" + str(channel)
    with requests.get(url) as resp:
        response = resp.json()
        if "error" in response:
            errors.error_control(-7)
        message = f"status code: {resp.status_code}"
        if resp.status_code != 200:
            print(f"ERROR: {message}")
            return 0
    ADC_code = response["ADC_code"]
    voltage = find_ADC_to_volt_channel(board_sn, channel, ADC_code)
    errors.error_control(voltage)
    return round(voltage,3)

def read_channel_adc_code(board_sn, channel):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    errors.error_control(check_channel(channel))
    node = get_node(board_sn)
    errors.error_control(node)
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/voltage/" + str(node) + "/" + str(channel)
    for attempt in range(ATTEMPTS):
        with requests.get(url) as resp:
            response = resp.json()
            if "error" in response:
                sleep(1)
                continue
            message = f"status code: {resp.status_code}"
            if resp.status_code != 200:
                print(f"ERROR: {message}")
                return 0
        ADC_code = response["ADC_code"]
        break
    return ADC_code

def read_channels_adc_code(board_sn):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    IP = parse_yaml(get_config_path())['ServerAddress']
    for channel in range(1, NODE_ID_MAX + 1):
        url ="http://" + IP + "/api/voltage/" + str(node) + "/" + str(channel)
        for attempt in range(ATTEMPTS):
            with requests.get(url) as resp:
                response = resp.json()
                if "error" in response:
                    sleep(1)
                    continue
                message = f"status code: {resp.status_code}"
                if resp.status_code != 200:
                    print(f"ERROR: {message}")
                    return 0
            ADC_code = response["ADC_code"]
            break
        print(f"ch: {channel:4}    ADC code: {ADC_code:10}")

def read_channels(board_sn):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    IP = parse_yaml(get_config_path())['ServerAddress']
    channels = range(1,NODE_ID_MAX + 1)
    for channel in channels:
        url ="http://" + IP + "/api/voltage/" + str(node) + "/" + str(channel)
        for attempt in range(ATTEMPTS):
            with requests.get(url) as resp:
                response = resp.json()
                if "error" in response:
                    # restart_can_network('supplr-server')
                    continue
                message = f"status code: {resp.status_code}"
                if resp.status_code != 200:
                    print(f"ERROR: {message}")
                    return 0
            ADC_code = response["ADC_code"]
            voltage = find_ADC_to_volt_channel(board_sn, channel, ADC_code)
            break
        print(f"{channel:3}   {round(voltage,3):7} V")
    return 0

def ref_voltage(board_sn):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/ref_voltage/" + str(node)
    for i in range(3):
        with requests.get(url) as resp:
            response = resp.json()
            if "error" in response:
                errors.error_control(-7)
            message = f"status code: {resp.status_code}"
            if resp.status_code != 200:
                msg = f"ERROR: {message}"
                raise Exception(msg)
        ref_voltage = response["ref_voltage"]/1000
        if 1.0<ref_voltage<4.5:
            break
    return ref_voltage

def hv_supply_voltage(board_sn):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/ext_voltage/" + str(node)
    for i in range(3):
        with requests.get(url) as resp:
            response = resp.json()
            if "error" in response:
                errors.error_control(-7)
            message = f"status code: {resp.status_code}"
            if resp.status_code != 200:
                print(f"ERROR: {message}")
                return 0
        ext_voltage = response["ext_voltage"]/1000
        if 1<ext_voltage<250: #HV power supply range: 1-250
            break
    return ext_voltage

def calc_temp(ADC_code):
    temp = 25 + (650 - (ADC_code*2048/int(0x7FFFFF)))/2.2
    return temp

def mez_temp(board_sn, mez_num):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/mez_temp/" + str(node) + "/" + str(mez_num)
    with requests.get(url) as resp:
        response = resp.json()
        message = f"status code: {resp.status_code}"
        if resp.status_code != 200:
            print(f"ERROR: {message}")
            return 0
    ADC_code = response["mez_temp"]
    return round(calc_temp(ADC_code),2)

def reset(board_sn):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/reset/" + str(node)
    with requests.get(url) as resp:
        response = resp.json()
        if "error" in response:
            print(f"Board: {board_sn} reseted!")
            return 0

def reset_network():
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/reset_can_network"
    with requests.get(url) as resp:
        response = resp.json()
        message = f"status code: {resp.status_code}"
        if resp.status_code != 200:
            print(f"ERROR: {message}")
            return 0

def set_channel_file(board_sn, file):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    with open(file) as f:
        rows = csv.reader(f, delimiter=",")
        print("Wait...")
        for i,row in enumerate(rows):
            channel = int(row[0])
            voltage = float(row[1])
            set_channel(node, channel, voltage)
        sleep(i*0.1)

def read_channel_file(board_sn, file):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    with open(file) as f:
        rows = csv.reader(f, delimiter=",")
        for row in rows:
            channel = int(row[0])
            print_read_channel(node, channel)

def print_ref_voltage(board_sn):
    print(f"Ref. voltage: {ref_voltage(board_sn)} V")

def print_hv_supply_voltage(board_sn):
    print(f"HV power supply: {hv_supply_voltage(board_sn)} V")

def print_mez_temp(board_sn):
    for mez_num in range(1,5):
        print(f"mez {mez_num}: {mez_temp(board_sn, mez_num)}°C")

def print_read_channel(board_sn, channel):
    print(f"channel: {channel:3}   Voltage: {read_channel(board_sn, channel):7} V")

def server_status():
    url ="http://" + get_server_address()
    try:
        r = requests.head(url)
        return True
    except:
        return False
        
