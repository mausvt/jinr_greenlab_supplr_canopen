from supplr import supplr_cli_lib
from supplr_db import api_db
from flask import Flask, render_template, request, jsonify
from time import sleep
import csv
import os



PATH_DB = supplr_cli_lib.get_app_path() + '/supplr_canopen/gui/supplr_db/supplr_db.db'
PRESETS_FOLDER = supplr_cli_lib.get_app_path() + '/supplr_canopen/cmd/presets'

INIT_DB = False
VERY_IMPORT_PARAM = 300


app = Flask(__name__)

@app.route("/")
def monitor():
    global INIT_DB
    nodes = supplr_cli_lib.get_nodes()
    config_files = get_config_files()
    if not INIT_DB:
        INIT_DB = True
        api_db.init_db(PATH_DB, nodes)
    return render_template('monitor.html', nodes=nodes, config_files=config_files)

@app.route("/av_nodes")
def av_nodes():
    nodes = supplr_cli_lib.get_nodes()
    return jsonify(nodes=nodes)

@app.route("/get_voltages/<node>")
def get_voltages(node):
    node = int(node)
    data = []
    board_data = {}
    board_data['data'] = api_db.get_node_data(PATH_DB, int(node))
    board_data['node'] = int(node)
    data.append(board_data)
    return jsonify(data)

@app.route("/update_channels/<node>")
def update_channels(node):
    channels = [ch for ch in range(1,129)]
    node = int(node)
    print(f'Read board #{node}')
    for ch in channels:
        flag_set = True
        flag_init = True
        flag_read = True
        data = supplr_cli_lib.read_channel(node, ch)
        while data['can_status'] in [1,2,3]:
            if data['can_status'] == 1 and flag_read:
                flag_read = False
                print(f'Another reading now. Wait, please.')
                sleep(1)
                print('done!')
            elif data['can_status'] == 2 and flag_set:
                flag_set = False
                print(f'Setting voltage. Wait, please.')
                sleep(1)
                print('done!')
            elif data['can_status'] == 3 and flag_init:
                flag_init = False
                print(f'CAN network initialization. Wait, please.')
                sleep(1)
                print('done!')
            data = supplr_cli_lib.read_channel(node, ch)
        if data['value'] in [0, -4, -1]:
            data['value'] = 0.3
        api_db.update_node_event(PATH_DB, node, ch, round(data['value'],2))
    return jsonify({'read': 'ok', 'node': node})

@app.route("/update_channels_after_setting/<node>")
def update_channels_after_setting(node):
    can_status_counter()
    channels = [ch for ch in range(1,129)]
    node = int(node)
    print(f'Read board #{node}')
    for ch in channels:
        flag_set = True
        flag_init = True
        flag_read = True
        data = supplr_cli_lib.read_channel(node, ch)
        while data['can_status'] in [1,2,3]:
            if data['can_status'] == 1 and flag_read:
                flag_read = False
                print(f'Another reading now. Wait, please.')
                sleep(1)
                print('done!')
            elif data['can_status'] == 2 and flag_set:
                flag_set = False
                print(f'Setting voltage. Wait, please.')
                sleep(1)
                print('done!')
            elif data['can_status'] == 3 and flag_init:
                flag_init = False
                print(f'CAN network initialization. Wait, please.')
                sleep(1)
                print('done!')
            data = supplr_cli_lib.read_channel(node, ch)
        if data['value'] in [0, -4, -1]:
            data['value'] = 0.3
        api_db.update_node_event(PATH_DB, node, ch, round(data['value'],2))
    return jsonify({'read': 'ok', 'node': node})

@app.route("/reset_board/<node>")
def reset_board(node):
    node = int(node)
    supplr_cli_lib.reset(node)
    print(f'Board #{node} reseted!')
    can_status_counter()
    return jsonify({'reset': 'ok'})

@app.route("/set_channels", methods=['GET', 'POST'])
def set_channels():
    if request.method == 'POST':
        data = request.get_json()
        if data['value_all_ch'] != '':
            node = int(data['node'])
            value = float(data['value_all_ch'])
            supplr_cli_lib.cli_set_channels(node, value)
        else:
            for item in data['value_per_ch']:
                if item['value'] != '':
                    try:
                        node = int(data['node'])
                        ch = int(item['ch'])
                        value = float(item['value'])
                        supplr_cli_lib.cli_set_channel(node, ch, value)
                    except:
                        print('SETTING CHANNEL WRONG!')
    return {'set': 'ok'}

@app.route("/save_config", methods=['GET', 'POST'])
def save_config():
    if request.method == 'POST':
        data = request.get_json()
        try:
            empty_all_ch = True
            empty_per_ch = True
            file_name = data['config_name']
            if file_name == '':
                return {'save': 'error'}
            file_name = file_name.replace(' ', '_')
            file_path = PRESETS_FOLDER + '/' + file_name + '.txt'
            data_per_ch = data['config_value_per_ch']
            data_all_ch = data['config_value_all_ch']
            for item in data_per_ch:
                if item['value'] != '':
                    empty_per_ch = False
            if data_all_ch != '':
                empty_all_ch = False
            if not empty_per_ch or not empty_all_ch:
                csv_file = open(file_path, mode='w')
                with csv_file:
                    writer = csv.writer(csv_file)
                    if not empty_all_ch:
                        for ch in range(1,129):
                            writer.writerow([ch,data_all_ch])
                        return {'save': 'ok'}
                    elif not empty_per_ch:
                        for item in data_per_ch:
                            if item['value'] == '':
                                item['value'] = 0
                            writer.writerow([item['ch'], item['value']])
                        return {'save': 'ok'}
                    else:
                        return {'save': 'error'}
            else:
                return {'save': 'empty'}
        except:
            {'save': 'error'}

@app.route("/can_status")
def can_status():
    READY = 0
    READ = 1
    SET = 2
    INIT = 3
    status_list = []
    for i in range(VERY_IMPORT_PARAM):
        can_status = supplr_cli_lib.get_can_status()
        status_list.append(can_status)
    if READ in status_list:
        return {'can_status': 1}
    elif SET in status_list:
        return {'can_status': 2}
    elif INIT in status_list:
        return {'can_status': 3}
    elif READY in status_list:
        return {'can_status': 0}
    else:
        return {'can_status': -1}

@app.route("/config_files")
def config_files():
    config_files_list = get_config_files()
    return jsonify({'config_files': config_files_list})

@app.route("/set_config_channels/<config_name>")
def set_config_channels(config_name):
    data_list = []
    data = {'data': data_list}
    if request.method == 'GET':
        file_name = config_name + '.txt'
        file_path = PRESETS_FOLDER + '/' + file_name
        with open(file_path, newline='') as File:
            reader = csv.reader(File)
            for row in reader:
                ch = int(row[0])
                value = float(row[1])
                data_list.append({'channel': ch, 'value': value})

        return jsonify(data)

def get_config_files():
    files = os.listdir(PRESETS_FOLDER)
    files_name = []
    for file in files:
        files_name.append(file.split('.')[0])
    return files_name

def can_status_counter():
    READY = 0
    READ = 1
    SET = 2
    INIT = 3
    active = True
    while active:
        can_status_list = []
        for i in range(VERY_IMPORT_PARAM):
            can_status = supplr_cli_lib.get_can_status()
            can_status_list.append(can_status)
        if (READ in can_status_list) or (SET in can_status_list) or (INIT in can_status_list):
            sleep(1)
        else:
            active=False

if __name__ == "__main__":
    get_config_files()
    address = supplr_cli_lib.get_server_address()
    host = address.split(':')[0]
    port=int(address.split(':')[1])+1
    app.run(host=host, port=port, debug=True)
