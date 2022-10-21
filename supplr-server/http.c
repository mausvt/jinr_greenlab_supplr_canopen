#include <stdio.h>
#include <ulfius.h>
#include <string.h>
#include <syslog.h>
#include <jansson.h>
#include <common.h>
#include <http.h>
#include <time.h>

#define RESP_TIMEOUT 1

// get the voltage from the in-memory state and respond to a user
static int callback_get_voltage(const struct _u_request * request, struct _u_response * response, void * _config) {
    http_config_t * config = (http_config_t *) _config;
    json_t * json_body = NULL;
    int rsize;
    time_t req_timestamp;
    time_t current_timestamp;
    req_t req;
    resp_t resp;
    // get channel number from the url /api/voltage/:node/:channel
    int node = atoi(u_map_get(request->map_url, "node"));
    int channel = atoi(u_map_get(request->map_url, "channel"));
    syslog(LOG_INFO, "Trying to get voltage for node: %d, --- channel: %d", node, channel);
    req.type = GetVoltage;
    req.subindex = channel;
    req.node = node;
    write(config->req_fd, &req, sizeof(req_t));
    time(&req_timestamp);
    while (TRUE) {
        usleep(100000);
        rsize = read(config->resp_fd, &resp, sizeof(resp_t));
        if (rsize == sizeof(resp_t)){
            break;
        }
        time(&current_timestamp);
        if (current_timestamp - req_timestamp > RESP_TIMEOUT) {
            syslog(LOG_ERR, "CAN response timeout");
            json_body = json_object();
            json_object_set_new(json_body, "error", json_string("CAN response timeout"));
            ulfius_set_json_body_response(response, 408, json_body);
            json_decref(json_body);
            return U_CALLBACK_CONTINUE;
        }
    }
    // create response data formatted like '{"node": node, "channel": channel, "voltage": voltage}'
    json_body = json_object();
    // resp.node return strange Node ID (Ex: Node ID = 2, return: 22312).
    resp.node = req.node;
    json_object_set_new(json_body, "node", json_integer(resp.node));
    json_object_set_new(json_body, "channel", json_integer(resp.subindex));
    json_object_set_new(json_body, "ADC_code", json_integer(resp.value));
    ulfius_set_json_body_response(response, 200, json_body);
    syslog(LOG_INFO, "Get voltage response, %s", json_dumps(json_body, 0));
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
}

// get the ref voltage and respond to a user
static int callback_get_ref_voltage(const struct _u_request * request, struct _u_response * response, void * _config) {
    http_config_t * config = (http_config_t *) _config;
    json_t * json_body = NULL;
    int rsize;
    time_t req_timestamp;
    time_t current_timestamp;
    req_t req;
    resp_t resp;
    // get node number from the url /api/ref_voltage/:node
    int node = atoi(u_map_get(request->map_url, "node"));
    int channel = 1;
    syslog(LOG_INFO, "Trying to get ref voltage for node: %d, --- subindex: %d", node, channel);
    req.type = GetRefVoltage;
    req.subindex = channel;
    req.node = node;
    write(config->req_fd, &req, sizeof(req_t));
    time(&req_timestamp);
    while (TRUE) {
        usleep(100000);
        rsize = read(config->resp_fd, &resp, sizeof(resp_t));
        if (rsize == sizeof(resp_t)){
            break;
        }
        time(&current_timestamp);
        if (current_timestamp - req_timestamp > RESP_TIMEOUT) {
            syslog(LOG_ERR, "CAN response timeout");
            json_body = json_object();
            json_object_set_new(json_body, "error", json_string("CAN response timeout"));
            ulfius_set_json_body_response(response, 408, json_body);
            json_decref(json_body);
            return U_CALLBACK_CONTINUE;
        }
    }
    // create response data formatted like '{"node": node, "channel": channel, "ref_voltage": ref_voltage}'
    json_body = json_object();
    resp.node = req.node;
    json_object_set_new(json_body, "node", json_integer(resp.node));
    json_object_set_new(json_body, "subindex", json_integer(resp.subindex));
    json_object_set_new(json_body, "ref_voltage", json_integer(resp.value));
    ulfius_set_json_body_response(response, 200, json_body);
    syslog(LOG_INFO, "Get ref voltage response, %s", json_dumps(json_body, 0));
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
}

// get the ext voltage and respond to a user
static int callback_get_ext_voltage(const struct _u_request * request, struct _u_response * response, void * _config) {
    http_config_t * config = (http_config_t *) _config;
    json_t * json_body = NULL;
    int rsize;
    time_t req_timestamp;
    time_t current_timestamp;
    req_t req;
    resp_t resp;
    // get channel number from the url /api/ext_voltage/:node
    int node = atoi(u_map_get(request->map_url, "node"));
    int channel = 2;
    syslog(LOG_INFO, "Trying to get ext voltage for node: %d, --- subindex: %d", node, channel);
    req.type = GetExtVoltage;
    req.subindex = channel;
    req.node = node;
    write(config->req_fd, &req, sizeof(req_t));
    time(&req_timestamp);
    while (TRUE) {
        usleep(100000);
        rsize = read(config->resp_fd, &resp, sizeof(resp_t));
        if (rsize == sizeof(resp_t)){
            break;
        }
        time(&current_timestamp);
        if (current_timestamp - req_timestamp > RESP_TIMEOUT) {
            syslog(LOG_ERR, "CAN response timeout");
            json_body = json_object();
            json_object_set_new(json_body, "error", json_string("CAN response timeout"));
            ulfius_set_json_body_response(response, 408, json_body);
            json_decref(json_body);
            return U_CALLBACK_CONTINUE;
        }
    }
    // create response data formatted like '{"node": node, "channel": channel, "ext_voltage": ext_voltage}'
    json_body = json_object();
    resp.node = req.node;
    json_object_set_new(json_body, "node", json_integer(resp.node));
    json_object_set_new(json_body, "subindex", json_integer(resp.subindex));
    json_object_set_new(json_body, "ext_voltage", json_integer(resp.value));
    ulfius_set_json_body_response(response, 200, json_body);
    syslog(LOG_INFO, "Get ext voltage response, %s", json_dumps(json_body, 0));
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
}

// Reset board
static int callback_reset_node(const struct _u_request * request, struct _u_response * response, void * _config) {
    http_config_t * config = (http_config_t *) _config;
    json_t * json_body = NULL;
    int rsize;
    time_t req_timestamp;
    time_t current_timestamp;
    req_t req;
    resp_t resp;
    // get channel number from the url /api/reset/:node
    int node = atoi(u_map_get(request->map_url, "node"));
    syslog(LOG_INFO, "Reset node: %d", node);
    req.type = ResetNode;
    req.subindex = 0;
    req.node = node;
    write(config->req_fd, &req, sizeof(req_t));
    json_body = json_object();
    resp.node = req.node;
    json_object_set_new(json_body, "node", json_integer(resp.node));
    ulfius_set_json_body_response(response, 200, json_body);
    syslog(LOG_INFO, "Get Reset node, %s", json_dumps(json_body, 0));
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
}

// Reset can network
static int callback_reset_can_network(const struct _u_request * request, struct _u_response * response, void * _config) {
    http_config_t * config = (http_config_t *) _config;
    json_t * json_body = NULL;
    int rsize;
    time_t req_timestamp;
    time_t current_timestamp;
    req_t req;
    resp_t resp;
    // get channel number from the url /api/reset_can_network
    syslog(LOG_INFO, "Reset can network!");
    req.type = ResetCanNetwork;
    req.subindex = 0;
    req.node = 0;
    write(config->req_fd, &req, sizeof(req_t));
    json_body = json_object();
    resp.node = req.node;
    ulfius_set_json_body_response(response, 200, json_body);
    syslog(LOG_INFO, "Get Reset can network, %s", json_dumps(json_body, 0));
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
}

// get the mezzanine temperature and respond to a user
static int callback_mez_temp(const struct _u_request * request, struct _u_response * response, void * _config) {
    http_config_t * config = (http_config_t *) _config;
    json_t * json_body = NULL;
    int rsize;
    int subindex;
    time_t req_timestamp;
    time_t current_timestamp;
    req_t req;
    resp_t resp;
    // get mezzanine temperature from the url /api/mez_temp/:node/:mez_num
    int node = atoi(u_map_get(request->map_url, "node"));
    int mez_num = atoi(u_map_get(request->map_url, "mez_num"));
    syslog(LOG_INFO, "Trying to get mez.temperature for node: %d, --- mez.number: %d", node, mez_num);
    if (mez_num == 1){
        subindex = 3;
    } else if (mez_num == 2){
        subindex = 4;
    } else if (mez_num == 3){
        subindex = 5;
    }
    else if (mez_num == 4){
        subindex = 6;
    }
    req.type = GetMezTemp;
    req.subindex = subindex;
    req.node = node;
    write(config->req_fd, &req, sizeof(req_t));
    time(&req_timestamp);
    while (TRUE) {
        usleep(100000);
        rsize = read(config->resp_fd, &resp, sizeof(resp_t));
        if (rsize == sizeof(resp_t)){
            break;
        }
        time(&current_timestamp);
        if (current_timestamp - req_timestamp > RESP_TIMEOUT) {
            syslog(LOG_ERR, "CAN response timeout");
            json_body = json_object();
            json_object_set_new(json_body, "error", json_string("CAN response timeout"));
            ulfius_set_json_body_response(response, 408, json_body);
            json_decref(json_body);
            return U_CALLBACK_CONTINUE;
        }
    }
    // create response data formatted like '{"node": node, "subindex": subindex, "mez_temp": mez_temp}'
    json_body = json_object();
    resp.node = req.node;
    json_object_set_new(json_body, "node", json_integer(resp.node));
    json_object_set_new(json_body, "subindex", json_integer(resp.subindex));
    json_object_set_new(json_body, "mez_temp", json_integer(resp.value));
    ulfius_set_json_body_response(response, 200, json_body);
    syslog(LOG_INFO, "Get mez.temperature response, %s", json_dumps(json_body, 0));
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
}

// parse request and put the voltage to the pipe
static int callback_set_voltage(const struct _u_request * request, struct _u_response * response, void * _config) {
    http_config_t * config = (http_config_t *) _config;

    // get channel number from the url /api/voltage/:channel  ---> /api/voltage/:node/:channel
    int channel = atoi(u_map_get(request->map_url, "channel"));
    int node = atoi(u_map_get(request->map_url, "node"));
    syslog(LOG_INFO, "Trying to get voltage for node: %d, --- channel: %d", node, channel);

    // get voltage from the request data,
    // data must json formatted as follows '{"voltage": "0x3FFF"}' or '{"voltage": 16383}'
    unsigned16 DAC_code;
    json_t * json_body = ulfius_get_json_body_request(request, NULL);
    syslog(LOG_INFO, "Set voltage request body, %s\n", json_dumps(json_body, 0));
    json_t * j_voltage_dac = json_object_get(json_body, "DAC_code");
    if (j_voltage_dac == NULL){
        ulfius_set_string_body_response(response, 400, "Wrong request body. Voltage must be set\n");
        json_decref(json_body);
        return U_CALLBACK_CONTINUE;
    }
    if (json_is_integer(j_voltage_dac)){
        DAC_code = (unsigned16)json_integer_value(j_voltage_dac);
    } else if (json_is_string(j_voltage_dac)){
        DAC_code = (unsigned16)strtol(json_string_value(j_voltage_dac), NULL, 0);
    } else {
        ulfius_set_string_body_response(response, 400, "Wrong request body. Voltage must be integer or hexadecimal string\n");
        json_decref(json_body);
        return U_CALLBACK_CONTINUE;
    }

    // create set voltage operation and write it to the pipe
    req_t req = { SetVoltage, node, channel, DAC_code };
    write(config->req_fd, &req, sizeof(req_t));
    ulfius_set_string_body_response(response, 200, "Ok\n");
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
}

void * start_http_server(void * _config) {
    struct _u_instance instance;
    http_config_t * config = (http_config_t *)_config;
    printf("Running on http://localhost:%d (Press CTRL+C to quit)\n", config->port);
    syslog(LOG_INFO,"Running on http://localhost:%d (Press CTRL+C to quit)\n", config->port);

    if (ulfius_init_instance(&instance, config->port, NULL, NULL) != U_OK) {
        syslog(LOG_ERR, "Error while initializing http server.\nPlease set <ServerAddress> parameter in configuration file properly!");
        exit(EXIT_FAILURE);
    }

    ulfius_add_endpoint_by_val(&instance, "POST", "/api", "/voltage/:node/:channel", 0, &callback_set_voltage, config);

    ulfius_add_endpoint_by_val(&instance, "GET", "/api", "/voltage/:node/:channel", 0, &callback_get_voltage, config);
    ulfius_add_endpoint_by_val(&instance, "GET", "/api", "/ref_voltage/:node", 0, &callback_get_ref_voltage, config);
    ulfius_add_endpoint_by_val(&instance, "GET", "/api", "/ext_voltage/:node", 0, &callback_get_ext_voltage, config);
    ulfius_add_endpoint_by_val(&instance, "GET", "/api", "/mez_temp/:node/:mez_num", 0, &callback_mez_temp, config);
    ulfius_add_endpoint_by_val(&instance, "GET", "/api", "/reset/:node", 0, &callback_reset_node, config);
    ulfius_add_endpoint_by_val(&instance, "GET", "/api", "/reset_can_network", 0, &callback_reset_can_network, config);

    if (ulfius_start_framework(&instance) == U_OK) {
        while (TRUE)
        {
            syslog(LOG_INFO, "Supplr main loop is running on http://localhost:%d",config->port);
            usleep(3e+07);
        }
    } else {
        syslog(LOG_ERR, "Error while starting web server");
        exit(EXIT_FAILURE);
    }

    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);
    return NULL;
}
