import csv
import requests
import yaml
from time import sleep

from supplr import convert_voltage
from supplr import errors
from supplr.config import IP, PATH_YAML, REF


ATTEMPTS = 5
NODE_ID_MAX = 128

def convert_node_to_board_sn(node):
    if node<NODE_ID_MAX:
        data = parse_yaml(PATH_YAML)
        errors.error_control(data)
        for fnode in data["Nodes"]:
            if node == fnode["NodeID"]:
                return fnode["BoardSN"]
        return -6
    return node

def parse_yaml(path_to_yaml):
    try:
        with open(path_to_yaml, "r") as stream:
            data = yaml.safe_load(stream)
            return data
    except:
        return -1

def check_boards(board_sn):
    board_sn = convert_node_to_board_sn(board_sn)
    data = parse_yaml(PATH_YAML)
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
    if voltage < 1 or voltage > 120:
        return -5

def get_node(board_sn):
    data = parse_yaml(PATH_YAML)
    errors.error_control(data)
    for node in data["Nodes"]:
        if board_sn == node["BoardSN"]:
            return node["NodeID"]
    return -6

# for CANOPEN (mapping: channel)
def find_volt_to_bit(board_sn, channel, voltage):
    try:
        cal_dir = parse_yaml(PATH_YAML)["CalibFolder"]
        errors.error_control(cal_dir)
        file_name = cal_dir + "/board_" + str(board_sn) + "_ref" + REF + "/board_"+str(board_sn)+"_channel_"+str(channel)+"_rec.txt"
        bit = convert_voltage.volt_to_bit(file_name, voltage)
        errors.error_control(bit)
        return bit
    except:
        return -3

# for CANOPEN (mapping: channel)
def find_ADC_to_volt_channel(board_sn, channel, ADC_code):
    try:
        if ADC_code<15000:
            return 0.3
        cal_dir = parse_yaml(PATH_YAML)["CalibFolder"]
        errors.error_control(cal_dir)
        file_name = cal_dir + "/board_" + str(board_sn) + "_ref" + REF + "/board_"+str(board_sn)+"_channel_"+str(channel)+"_rec.txt"
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
    for channel in range(1,129):
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
    channels = range(1,129)
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
    for channel in range(1,129):
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
    channels = range(1,129)
    for channel in channels:
        url ="http://" + IP + "/api/voltage/" + str(node) + "/" + str(channel)
        for attempt in range(ATTEMPTS):
            with requests.get(url) as resp:
                response = resp.json()
                if "error" in response:
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
    url ="http://" + IP + "/api/ref_voltage/" + str(node)
    with requests.get(url) as resp:
        response = resp.json()
        if "error" in response:
            errors.error_control(-7)
        message = f"status code: {resp.status_code}"
        if resp.status_code != 200:
            print(f"ERROR: {message}")
            return 0
    ref_voltage = response["ref_voltage"]/1000
    return ref_voltage

def hv_supply_voltage(board_sn):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    url ="http://" + IP + "/api/ext_voltage/" + str(node)
    with requests.get(url) as resp:
        response = resp.json()
        if "error" in response:
            errors.error_control(-7)
        message = f"status code: {resp.status_code}"
        if resp.status_code != 200:
            print(f"ERROR: {message}")
            return 0
    ext_voltage = response["ext_voltage"]/1000
    return ext_voltage

def calc_temp(ADC_code):
    temp = 25 + (650 - (ADC_code*2048/int(0x7FFFFF)))/2.2
    return temp

def mez_temp(board_sn, mez_num):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
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
    url ="http://" + IP + "/api/reset/" + str(node)
    with requests.get(url) as resp:
        response = resp.json()
        if "error" in response:
            print(f"Board: {board_sn} reseted!")
            return 0

def reset_network():
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
