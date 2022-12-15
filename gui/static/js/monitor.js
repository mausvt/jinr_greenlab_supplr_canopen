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
                            if (value>1.2) {
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

function update_channels(node){
    $.ajax({
            type: "GET",
            url:"/update_channels/"+node,
            context: document.body,
            success: function(data) {
            }
    }
)
};

function update_channels_auto(){
    $.ajax({
            type: "GET",
            url:"/av_nodes",
            context: document.body,
            success: function(data) {
                let nodes = data['nodes'];
                for (let node_ind=0; node_ind<nodes.length; node_ind++) {
                    console.log("READING: " + nodes[node_ind])
                    update_channels(nodes[node_ind]);
                }
            }
    }
)};

function reset_board(node){
    $.ajax({
            type: "GET",
            url:"/reset_board/"+node,
            context: document.body,
            success: function(data) {
                console.log("Reseted: "+node);
            }
    })
    sleep(1000);
    update_channels(node);
};

function av_nodes(){
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

function sleep(millis) {
    var t = (new Date()).getTime();
    var i = 0;
    while (((new Date()).getTime() - t) < millis) {
        i++;
    }
}

function set_channels(node){
    $('#exampleModal-'+node).modal('hide')
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
    sleep(3000);
    update_channels(node);
};

function clean_form(node){
    $("#"+node+"-value-all-ch").val("")
    for (let ch=1; ch<129; ch ++){
        $("#"+node+"-"+ch+"-value").val("");
    }

}

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
    $.ajax({
            type: "GET",
            url:"/av_nodes",
            context: document.body,
            success: function(data) {
                let nodes = data['nodes'];
                // console.log(nodes);
                for (let node_ind=0; node_ind<nodes.length; node_ind++) {
                    $("#button_update-"+nodes[node_ind]).attr("disabled", true);
                    $("#button_set-"+nodes[node_ind]).attr("disabled", true);
                    $("#button_reset-"+nodes[node_ind]).attr("disabled", true);

                }
            }
    }
)};

function buttons_able(){
    $.ajax({
            type: "GET",
            url:"/av_nodes",
            context: document.body,
            success: function(data) {
                let nodes = data['nodes'];
                // console.log(nodes);
                for (let node_ind=0; node_ind<nodes.length; node_ind++) {
                    $("#button_update-"+nodes[node_ind]).attr("disabled", false);
                    $("#button_set-"+nodes[node_ind]).attr("disabled", false);
                    $("#button_reset-"+nodes[node_ind]).attr("disabled", false);

                }
            }
    }
)};

can_status();
setInterval(av_nodes, 1000);
setInterval(can_status, 500);
