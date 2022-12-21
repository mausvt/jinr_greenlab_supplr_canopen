function get_voltages(node){
    $.ajax({
            type: "GET",
            url:"/get_voltages/"+node,
            context: document.body,
            success: function(data) {
                    let board_index = 0;
                    for (let ch_index=0; ch_index<128; ch_index++) {
                        let ch = data[board_index]['data'][ch_index]['channel'];
                        let value = data[board_index]['data'][ch_index]['value'];
                        if (value == null) {
                            value = 'N/A';
                            $("#"+node+"-"+ch).html("<font color='red'>" + value);
                        } else {
                            if (value>1.0) {
                                $("#"+node+"-"+ch+"-bg").css('background-color', 'rgb(60,179,113)');
                                $("#"+node+"-"+ch).html("<font color='#2F4F4F'>" + value);
                            } else {
                                $("#"+node+"-"+ch+"-bg").css('background-color', 'rgb(239, 235, 235, 0.891)');
                                $("#"+node+"-"+ch).html("<font color='blue'>" + value);
                            }

                        }
                    }
                }
    }
)};

function update_data(){
    $.ajax({
            type: "GET",
            url:"/av_nodes",
            context: document.body,
            success: function(data) {
                let nodes = data['nodes'];
                for (let node_ind=0; node_ind<nodes.length; node_ind++) {
                    get_voltages(nodes[node_ind]);
                }
            }
    }
)};

function read_channels(node){
    console.log('Read channels for node #'+node)
    $.ajax({
            type: "GET",
            url:"/update_channels/"+node,
            context: document.body,
            success: function(data) {
            }
    }
)
};

function read_channels_after_setting(node){
    $.ajax({
            type: "GET",
            url:"/update_channels_after_setting/"+node,
            context: document.body,
            success: function(data) {
                console.log('Read channels for node #'+node)
            }
    }
)
};

function reset_board(node){
    buttons_disabled();
    console.log("Reset node #"+node);
    $.ajax({
            type: "GET",
            url:"/reset_board/"+node,
            context: document.body,
            success: function(data) {
            }
    })
    read_channels_after_setting(node);
};

function get_nodes(){
    var res;
    $.ajax({
            async: false,
            type: "GET",
            url:"/av_nodes",
            dataType: 'json',
            context: document.body,
            success: function(data) {
                res = data['nodes'];
            }
    })
    return res;
};

function get_config_files() {
    var res;
    $.ajax({
        type: "GET",
        url:"/config_files",
        context: document.body,
        async: false,
        success: function(data) {
            res = data['config_files'];
    }
    })
    return res;
}

function draw_config_files(){
    let config_files = get_config_files();
    let nodes = get_nodes();
    for (let node_i=0; node_i<nodes.length; node_i++) {
        for (let i=0; i<config_files.length; i++) {
            var elem_avai = document.getElementById(nodes[node_i] + '-' + config_files[i]);
            if (!elem_avai) {
                var elem = document.createElement("li");
                elem.innerHTML = '<button id="' + nodes[node_i] + '-' + config_files[i] + '" class="dropdown-item btn btn-link btn-lg" onclick="buttons_disabled(); set_config_channels(this);" type="button" name="submit">' + config_files[i] + '</button>';
                document.getElementById("config_menu_"+nodes[node_i]).appendChild(elem);
            }
        }
    }
}


function set_config_channels(obj) {
    let button_id = obj.id;
    let node = Number(button_id.split('-')[0]);
    let data = {
        id: button_id
    };
    var data_json = JSON.stringify(data)
    console.log("Setting channels for node #"+node);
    $.ajax({
        type: "POST",
        url:"/set_config_channels",
        data: data_json,
        headers: {
            'Accept': 'application/json',
            'Content-Type': 'application/json'
        },
        dataType: "json",
        context: document.body,
        success: function(data) {
        }
    })
    let nodes = get_nodes();
    for (let node_i=0; node_i<nodes.length; node_i++) {
        close_set_voltage_modal(nodes[node_i]);
    }
    buttons_disabled();
    read_channels_after_setting(node)
}

function set_channels(node){
    let data_ch = [];
    for (let ch=1; ch<129; ch++) {
        data_ch.push({'ch': ch, 'value': document.getElementById(node+'-'+ch+'-value').value});
    }
    let data = {
        node: JSON.stringify(node),
        value_all_ch: document.getElementById(node+'-value-all-ch').value,
        value_per_ch: data_ch
    };
    var data_json = JSON.stringify(data);
    console.log("Setting channels for node #"+node);
    $.ajax({
            type: "POST",
            url:"/set_channels",
            data: data_json,
            headers: {
                'Accept': 'application/json',
                'Content-Type': 'application/json'
            },
            dataType: "json",
            context: document.body,
            success: function(data) {
            }
    }
    )
    close_set_voltage_modal(node);
    buttons_disabled()
    read_channels_after_setting(node);
};

function save_config() {
    console.log('Config saved')
    let data_ch = [];
    for (let ch=1; ch<129; ch++) {
        data_ch.push({'ch': ch, 'value': document.getElementById('config_value_' +ch).value});
    }
    let data = {
        config_name: document.getElementById('config_name').value,
        config_value_all_ch: document.getElementById('config_value_all_ch').value,
        config_value_per_ch: data_ch
    };
    var data_json = JSON.stringify(data);
    $.ajax({
            type: "POST",
            url:"/save_config",
            data: data_json,
            headers: {
                'Accept': 'application/json',
                'Content-Type': 'application/json'
            },
            dataType: "json",
            context: document.body,
            success: function(data) {
            }
    })
    close_config_modal_window()
};

function can_status(){
    $.ajax({
            type: "GET",
            url:"/can_status",
            context: document.body,
            success: function(data) {
                let status = data['can_status'];
                if (status == 1) {
                    $("#can_status").html("<font color='red'>READING</font>");
                    buttons_disabled();
                }
                else if (status == 2) {
                    $("#can_status").html("<font color='blue'>SETTING</font>");
                    buttons_disabled();
                }
                else if (status == 3) {
                    $("#can_status").html("<font color='orange'>INIT</font>");
                    buttons_disabled();
                }
                else if (status == 0) {
                    $("#can_status").html("<font color='green'>READY</font>");
                    buttons_able();
                }
                else {
                    $("#can_status").html("<font color='red'>N/A</font>");
                    buttons_disabled();
                }
            }
    }
)};

function buttons_disabled(){
    let nodes = get_nodes();
    for (let node_ind=0; node_ind<nodes.length; node_ind++) {
        $("#button_update-"+nodes[node_ind]).attr("disabled", true);
        $("#button_set-"+nodes[node_ind]).attr("disabled", true);
        $("#button_reset-"+nodes[node_ind]).attr("disabled", true);
        $("#button_setting-"+nodes[node_ind]).attr("disabled", true);
        $("#clean-"+nodes[node_ind]).attr("disabled", true);
    }
}

function buttons_able(){
    let nodes = get_nodes();
    for (let node_ind=0; node_ind<nodes.length; node_ind++) {
        $("#button_update-"+nodes[node_ind]).attr("disabled", false);
        $("#button_set-"+nodes[node_ind]).attr("disabled", false);
        $("#button_reset-"+nodes[node_ind]).attr("disabled", false);
        $("#button_setting-"+nodes[node_ind]).attr("disabled", false);
        $("#clean-"+nodes[node_ind]).attr("disabled", false);
    }
}

function close_set_voltage_modal(node){
    $('#SetVoltageModal-'+node).modal('hide')
}

function close_config_modal_window(){
    $('#create_config_window').modal('hide')
}

function clean_form(node){
    $("#"+node+"-value-all-ch").val("")
    for (let ch=1; ch<129; ch ++){
        $("#"+node+"-"+ch+"-value").val("");
    }
}


can_status();
draw_config_files();
setInterval(update_data, 1000);
setInterval(can_status, 2000);
