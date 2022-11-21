import csv
import requests
import yaml
from time import sleep
from os.path import abspath

from supplr import convert_voltage
from supplr import errors

ATTEMPTS = 4
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

def server_status():
    url ="http://" + get_server_address()
    try:
        r = requests.head(url)
        return True
    except:
        return False

def available_server():
    status = server_status()
    if not status:
        return -9

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

def check_ip():
    IP = parse_yaml(get_config_path())['ServerAddress']
    try:
        port = int(IP.split(':')[1])
        if port>10000 or port<5000:
            return -8
    except:
        return -8

def get_node(board_sn):
    data = parse_yaml(get_config_path())
    errors.error_control(data)
    for node in data["Nodes"]:
        if board_sn == node["BoardSN"]:
            return node["NodeID"]
    return -6

def get_nodes():
    path_to_config = get_config_path()
    data = parse_yaml(path_to_config)
    errors.error_control(data)
    nodes = []
    for node in data["Nodes"]:
        nodes.append(node["NodeID"])
    return sorted(nodes)

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
        if ADC_code<10000:
            return 0
        cal_dir = get_calib_path()
        ref = parse_yaml(get_config_path())['RefVoltage']
        errors.error_control(cal_dir)
        file_name = cal_dir + "/board_" + str(board_sn) + "_ref" + ref + "/board_"+str(board_sn)+"_channel_"+str(channel)+"_rec.txt"
        voltage = convert_voltage.ADC_code_to_volt(file_name, ADC_code)
        errors.error_control(voltage)
        return voltage
    except:
        return -4

def set_channel_bit(board_sn, channel, DAC_code): 
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    errors.error_control(check_ip())
    errors.error_control(available_server())
    ch_list = [{'ch': channel, 'value': DAC_code}]
    data = {'node': node, 'board_sn': board_sn, 'data':ch_list, 'can_status': -1}
    for item in data['data']:
        channel = item['ch']
        voltage = item['value']
        item['value'] = DAC_code
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/voltages/" + str(node)
    with requests.post(url, json = data) as resp:
        message = f"status code: {resp.status_code}"
        if resp.status_code != 200:
            print(f"ERROR: {message}")
            return 0
        response = resp.json()
        data['can_status'] = response['can_status']
    status = {'can_status': data['can_status']}
    return status

def set_channels(board_sn, ch_list):   # ch_list = [{'ch': 1, 'value': 2.0}, {'ch': 2, 'value': 3.0}, {'ch': 3, 'value': 4.0}]
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    errors.error_control(check_ip())
    errors.error_control(available_server())
    data = {'node': node, 'board_sn': board_sn, 'data':ch_list, 'can_status': -1}
    for item in data['data']:
        channel = item['ch']
        voltage = item['value']
        DAC_code = find_volt_to_bit(board_sn, channel, voltage)
        item['value'] = DAC_code
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/voltages/" + str(node)
    with requests.post(url, json = data) as resp:
        message = f"status code: {resp.status_code}"
        if resp.status_code != 200:
            print(f"ERROR: {message}")
            return 0
        response = resp.json()
        data['can_status'] = response['can_status']
    status = {'can_status': data['can_status']}
    return status

def read_channels_adc_code(board_sn):
    SLEEP = 2
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    errors.error_control(check_ip())
    IP = parse_yaml(get_config_path())['ServerAddress']
    ch_list = [ch for ch in range(1,129)]
    data_ch = []
    for ch in ch_list:
        data_ch.append({'ch': ch, 'value': -1})
    data = {'node': node, 'board_sn': board_sn, 'data': data_ch, 'can_status': -1}
    for item in data['data']:
        channel = item['ch']
        url ="http://" + IP + "/api/voltage/" + str(node) + "/" + str(channel)
        try:
            with requests.get(url) as resp:
                response = resp.json()
                ADC_code = response["value"]
                item['value'] = ADC_code
                data['can_status'] = response['can_status']
        except BaseException as e:
            sleep(SLEEP)
    return data

def read_channel_adc_code(board_sn, channel):
    SLEEP = 2
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    errors.error_control(check_ip())
    IP = parse_yaml(get_config_path())['ServerAddress']
    data = {'node': node, 'board_sn': board_sn, 'ch': channel, 'value': -1, 'can_status': -1}
    channel = data['ch']
    url ="http://" + IP + "/api/voltage/" + str(node) + "/" + str(channel)
    try:
        with requests.get(url) as resp:
            response = resp.json()
            ADC_code = response["value"]
            data['value'] = ADC_code
            data['can_status'] = response['can_status']
    except BaseException as e:
        sleep(SLEEP)
    return data

def read_channel(board_sn, channel):
    SLEEP = 2
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    errors.error_control(check_ip())
    data = {'node': node, 'board_sn': board_sn, 'ch': channel, 'value': -1, 'can_status': -1}
    channel = data['ch']
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/voltage/" + str(node) + "/" + str(channel)
    try:
        with requests.get(url) as resp:
            response = resp.json()
            ADC_code = response["value"]
            voltage = find_ADC_to_volt_channel(board_sn, channel, ADC_code)
            data['value'] = round(voltage,4)
            data['can_status'] = response['can_status']
    except BaseException as e:
        sleep(SLEEP)
    return data

def read_channels(board_sn, ch_list):   # ch_list = [1,2,3,16,17,18,105,...]
    SLEEP = 2
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    errors.error_control(check_ip())
    IP = parse_yaml(get_config_path())['ServerAddress']
    data_ch = []
    for ch in ch_list:
        data_ch.append({'ch': ch, 'value': -1})
    data = {'node': node, 'board_sn': board_sn, 'data': data_ch, 'can_status': -1}
    for item in data['data']:
        channel = item['ch']
        url ="http://" + IP + "/api/voltage/" + str(node) + "/" + str(channel)
        try:
            with requests.get(url) as resp:
                response = resp.json()
                ADC_code = response["value"]
                voltage = find_ADC_to_volt_channel(board_sn, channel, ADC_code)
                item['value'] = round(voltage,4)
                data['can_status'] = response['can_status']
        except BaseException as e:
            sleep(SLEEP)
    return data

def ref_voltage(board_sn):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    errors.error_control(check_ip())
    errors.error_control(available_server())
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/ref_voltage/" + str(node)
    data = {'node': node, 'board_sn': board_sn, 'value': -1, 'can_status': -1}
    try:
        with requests.get(url) as resp:
            response = resp.json()
            data['value'] = response['value']/1000
            data['can_status'] = response['can_status']
    except:
        return data
    return data

def ext_voltage(board_sn):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    errors.error_control(check_ip())
    errors.error_control(available_server())
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/ext_voltage/" + str(node)
    data = {'node': node, 'board_sn': board_sn, 'value': -1, 'can_status': -1}
    try:
        with requests.get(url) as resp:
            response = resp.json()
            data["value"] = response["value"]/1000
            data['can_status'] = response['can_status']
    except:
        return data
    return data

def calc_temp(ADC_code):
    temp = 25 + (650 - (ADC_code*2048/int(0x7FFFFF)))/2.2
    return temp

def mez_temp(board_sn, mez_num):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    errors.error_control(check_ip())
    errors.error_control(available_server())
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/mez_temp/" + str(node) + "/" + str(mez_num)
    data = {'node': node, 'board_sn': board_sn, 'mez': mez_num, 'value': -1, 'can_status': -1}
    try:
        with requests.get(url) as resp:
            response = resp.json()
            message = f"status code: {resp.status_code}"
            if resp.status_code != 200:
                print(f"ERROR: {message}")
                return 0
            data['value'] = round(calc_temp(response["value"]),2)
            data['can_status'] = response['can_status']
    except:
        return data
    return data

def mez_temps(board_sn):
    board_sn = convert_node_to_board_sn(board_sn)
    node = get_node(board_sn)
    data_mez = []
    for mez_num in range(1,5):
        data_mez.append({'mez': mez_num, 'value': -1})
    data = {'node': node, 'board_sn': board_sn, 'data': data_mez, 'can_status': -1}
    for mez in data['data']:
        resp = mez_temp(board_sn, mez['mez'])
        mez['value'] = resp['value']
        data['can_status'] = resp['can_status']
    return data

def reset(board_sn):
    board_sn = convert_node_to_board_sn(board_sn)
    errors.error_control(check_boards(board_sn))
    node = get_node(board_sn)
    errors.error_control(node)
    errors.error_control(check_ip())
    errors.error_control(available_server())
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/reset/" + str(node)
    data = {'can_status': -1}
    try:
        with requests.get(url) as resp:
            response = resp.json()
            if "error" in response:
                print(f"Board: {board_sn} reseted!")
                return data
            data['can_status'] = response['can_status']
    except:
        return data
    return data

def reset_network():
    errors.error_control(check_ip())
    errors.error_control(available_server())
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/reset_can_network"
    data = {'can_status': -1}
    try:
        with requests.get(url) as resp:
            response = resp.json()
            message = f"status code: {resp.status_code}"
            if resp.status_code != 200:
                print(f"ERROR: {message}")
                return 0
            data['can_status'] = response['can_status']
    except:
        return data
    return data

def set_channel_file(board_sn, file):
    DELAY = 0.1
    print(f'Setting voltages...')
    try:
        with open(file) as f:
            rows = csv.reader(f, delimiter=",")
            row_count = 0
            for row in rows:
                row_count += 1
                channel = int(row[0])
                voltage = float(row[1])
                cli_set_channel(board_sn, channel, voltage)
            sleep(row_count*DELAY)
    except:
        print(f'File not found!')

def read_channel_file(board_sn, file):
    try:
        with open(file) as f:
            rows = csv.reader(f, delimiter=",")
            for row in rows:
                channel = int(row[0])
                cli_read_channel(board_sn, channel)
    except:
        print(f'File not found!')


def cli_ref_voltage(board_sn):
    flag_set = True
    flag_init = True
    flag_read = True
    data = ref_voltage(board_sn)
    while data['can_status'] in [1,2,3]:
        if data['can_status'] == 1 and flag_read:
            flag_read = False
            print(f'Another reading now. Wait, please.')
        elif data['can_status'] == 2 and flag_set:
            flag_set = False
            print(f'Setting voltage. Wait, please.')
        elif data['can_status'] == 3 and flag_init:
            flag_init = False
            print(f'CAN network initialization. Wait, please.')
        data = ref_voltage(board_sn)
        sleep(1)
    value = data['value']
    print(f"Ref. voltage: {value} V")
    return 0


def cli_ext_voltage(board_sn):
    flag_set = True
    flag_init = True
    flag_read = True
    data = ext_voltage(board_sn)
    while data['can_status'] in [1,2,3]:
        if data['can_status'] == 1 and flag_read:
            flag_read = False
            print(f'Another reading now. Wait, please.')
        elif data['can_status'] == 2 and flag_set:
            flag_set = False
            print(f'Setting voltage. Wait, please.')
        elif data['can_status'] == 3 and flag_init:
            flag_init = False
            print(f'CAN network initialization. Wait, please.')
        data = ext_voltage(board_sn)
        sleep(1)
    print(f"HV power supply: {ext_voltage(board_sn)['value']} V")
    return 0

def cli_mez_temps(board_sn):
    flag_set = True
    flag_init = True
    flag_read = True
    data = mez_temps(board_sn)
    while data['can_status'] in [1,2,3]:
        if data['can_status'] == 1 and flag_read:
            flag_read = False
            print(f'Another reading now. Wait, please.')
        elif data['can_status'] == 2 and flag_set:
            flag_set = False
            print(f'Setting voltage. Wait, please.')
        elif data['can_status'] == 3 and flag_init:
            flag_init = False
            print(f'CAN network initialization. Wait, please.')
        data = mez_temps(board_sn)
        sleep(1)
    for item in data['data']:
        print(f'mez {item["mez"]}: {item["value"]}°C')
    return 0

def cli_read_channel(board_sn, channel):
    flag_set = True
    flag_init = True
    flag_read = True
    data = read_channels(board_sn, [channel])
    while data['can_status'] in [1,2,3]:
        if data['can_status'] == 1 and flag_read:
            flag_read = False
            print(f'Another reading now. Wait, please.')
        elif data['can_status'] == 2 and flag_set:
            flag_set = False
            print(f'Setting voltage. Wait, please.')
        elif data['can_status'] == 3 and flag_init:
            flag_init = False
            print(f'CAN network initialization. Wait, please.')
        data = read_channels(board_sn, [channel])
        sleep(1)
    voltage = data['data'][0]['value']
    if voltage in [0, -4]:
        data['data'][0]['value'] = 0.3
    print(f"channel: {channel:3}   Voltage: {data['data'][0]['value']:8} V")
    return 0

def cli_read_channels(board_sn):
    for channel in range(1,129):
        cli_read_channel(board_sn, channel)
    return 0

def cli_set_channel(board_sn, channel, voltage):
    data = [{'ch': channel, 'value': voltage}]
    set_channels(board_sn, data)

def cli_set_channels(board_sn, voltage):
    data = []
    print(f'Setting voltages...')
    for channel in range(1,129):
        data.append({'ch': channel, 'value': voltage})
    set_channels(board_sn, data)
    sleep(15)

def get_can_status():
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/get_can_status"
    try:
        with requests.get(url) as resp:
            response = resp.json()
            return response['can_status']
    except:
        print("Reload server!")

def set_can_status_set():
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/set_can_status_set"
    with requests.get(url) as resp:
        response = resp.json()
        return response['can_status']

def set_can_status_ready():
    IP = parse_yaml(get_config_path())['ServerAddress']
    url ="http://" + IP + "/api/set_can_status_ready"
    with requests.get(url) as resp:
        response = resp.json()
        return response['can_status']

