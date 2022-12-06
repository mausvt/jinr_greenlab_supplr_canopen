from supplr import supplr_cli_lib
from supplr_db import api_db
from flask import Flask, render_template, request, jsonify
from time import sleep


PATH_DB = supplr_cli_lib.get_app_path() + '/supplr_canopen/gui/supplr_db/supplr_db.db'
INIT_DB = False

app = Flask(__name__)

@app.route("/")
def monitor():
    global INIT_DB
    nodes = supplr_cli_lib.get_nodes()
    if not INIT_DB:
        INIT_DB = True
        api_db.init_db(PATH_DB, nodes)
    return render_template('monitor.html', nodes=nodes)

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

@app.route("/reset_board/<node>")
def reset_board(node):
    node = int(node)
    supplr_cli_lib.reset(node)
    print(f'Board #{node} reseted!')
    return jsonify({'reset': 'ok'})

@app.route("/set_channels", methods=['GET', 'POST'])
def set_channels():
    if request.method == 'POST':
        data = request.get_json()
        # print(data)
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

@app.route("/can_status")
def can_status():
    READY = 0
    READ = 1
    SET = 2
    INIT = 3
    status_list = []
    for i in range(10):
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



if __name__ == "__main__":
    app.run(host="localhost", port=5099, debug=True)
