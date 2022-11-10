#include <stdio.h>
#include <ulfius.h>
#include <string.h>
#include <syslog.h>
#include <jansson.h>
#include <stdlib.h>
#include <common.h>
#include <http.h>
#include <time.h>
#include <master_header.h>
#include <can.h>
#include <pthread.h>

#define RESP_TIMEOUT 1
#define ERR_ATT 10

void resp_can_busy(json_t * json_body, struct _u_response * response) {
    json_body = json_object();
    json_object_set_new(json_body, "node", json_integer(-1));
    json_object_set_new(json_body, "subindex", json_integer(-1));
    json_object_set_new(json_body, "value", json_integer(-1));
    json_object_set_new(json_body, "can_status", json_integer(can_status));
    ulfius_set_json_body_response(response, 200, json_body);
    json_decref(json_body);
}

void resp_json_data(json_t * json_body, read_st data, struct req_t req, struct _u_response * response) {
    data.node = req.node;
    json_body = json_object();
    json_object_set_new(json_body, "node", json_integer(data.node));
    json_object_set_new(json_body, "subindex", json_integer(data.subindex));
    json_object_set_new(json_body, "value", json_integer(data.value));
    json_object_set_new(json_body, "can_status", json_integer(can_status));
    ulfius_set_json_body_response(response, 200, json_body);
    json_decref(json_body);

}

// get the voltage from the in-memory state and respond to a user
static int callback_get_voltage(const struct _u_request * request, struct _u_response * response, void * _config) {
    int rsize;
    int node = atoi(u_map_get(request->map_url, "node"));
    int channel = atoi(u_map_get(request->map_url, "channel"));
    http_config_t * config = (http_config_t *) _config;
    json_t * json_body = NULL;
    time_t req_timestamp;
    time_t current_timestamp;
    req_t req = {.type = GetVoltage, .subindex = channel, .node = node, .n_set = 1};
    resp_st resp;
    if (can_status != READY) {
        resp_can_busy(json_body, response);
        return U_CALLBACK_CONTINUE;
    }
    can_status = READING;
    write(config->req_fd, &req, sizeof(req_t));
    time(&req_timestamp);
    int count_err = 0;
    while (TRUE) {
        usleep(100000);
        rsize = read(config->resp_fd, &resp, sizeof(resp_st));
        if (rsize == sizeof(resp_st)){
            break;
        }
        time(&current_timestamp);
        if (current_timestamp - req_timestamp > RESP_TIMEOUT) {
            if (can_status == SETTING || can_status == INIT) {
                syslog(LOG_WARNING, "%s : CAN is not READY for reading [%d]", func_name[req.type], can_status);
                resp_json_data(json_body, resp.get_volt, req, response);
                return U_CALLBACK_CONTINUE;
            }
            count_err ++;
            syslog(LOG_ERR, "%s : CAN response timeout: %d/10 [%d]", func_name[req.type], count_err, can_status);
            if (count_err == ERR_ATT) {
                count_err = 0;
                resp_json_data(json_body, resp.get_volt, req, response);
                stop_can_master();
                exit(EXIT_FAILURE);
            }
        }
    }
    can_status = READY;
    resp_json_data(json_body, resp.get_volt, req, response);
    return U_CALLBACK_CONTINUE;
}

// get the ref voltage and respond to a user
static int callback_get_ref_voltage(const struct _u_request * request, struct _u_response * response, void * _config) {
    int rsize;
    int node = atoi(u_map_get(request->map_url, "node"));
    int channel = 1;
    http_config_t * config = (http_config_t *) _config;
    json_t * json_body = NULL;
    time_t req_timestamp;
    time_t current_timestamp;
    req_t req = {.type = GetRefVoltage, .subindex = channel, .node = node};
    resp_st resp;
    if (can_status != READY) {
        resp_can_busy(json_body, response);
        return U_CALLBACK_CONTINUE;
    }
    can_status = READING;
    write(config->req_fd, &req, sizeof(req_t));
    time(&req_timestamp);
    int count_err = 0;
    while (TRUE) {
        usleep(100000);
        rsize = read(config->resp_fd, &resp, sizeof(resp_st));
        if (rsize == sizeof(resp_st)){
            break;
        }
        time(&current_timestamp);
        if (current_timestamp - req_timestamp > RESP_TIMEOUT) {
            if (can_status == SETTING || can_status == INIT) {
                syslog(LOG_WARNING, "%s : CAN is not READY for reading [%d]", func_name[req.type], can_status);
                resp_json_data(json_body, resp.get_volt, req, response);
                return U_CALLBACK_CONTINUE;
            }
            count_err ++;
            syslog(LOG_ERR, "%s : CAN response timeout: %d/10 [%d]", func_name[req.type], count_err, can_status);
            if (count_err == ERR_ATT) {
                count_err = 0;
                resp_json_data(json_body, resp.ref_volt, req, response);
                stop_can_master();
                exit(EXIT_FAILURE);
            }
        }
    }
    can_status = READY;
    resp_json_data(json_body, resp.ref_volt, req, response);
    return U_CALLBACK_CONTINUE;
}

// get the ext voltage and respond to a user
static int callback_get_ext_voltage(const struct _u_request * request, struct _u_response * response, void * _config) {
    int rsize;
    int node = atoi(u_map_get(request->map_url, "node"));
    int channel = 2;
    http_config_t * config = (http_config_t *) _config;
    json_t * json_body = NULL;
    time_t req_timestamp;
    time_t current_timestamp;
    req_t req = {.type = GetExtVoltage, .subindex = channel, .node = node, .n_set = 1};
    resp_st resp;
    if (can_status != READY) {
        resp_can_busy(json_body, response);
        return U_CALLBACK_CONTINUE;
    }
    can_status = READING;
    write(config->req_fd, &req, sizeof(req_t));
    time(&req_timestamp);
    int count_err = 0;
    while (TRUE) {
        usleep(100000);
        rsize = read(config->resp_fd, &resp, sizeof(resp_st));
        if (rsize == sizeof(resp_st)){
            break;
        }
        time(&current_timestamp);
        if (current_timestamp - req_timestamp > RESP_TIMEOUT) {
            if (can_status == SETTING || can_status == INIT) {
                syslog(LOG_WARNING, "%s : CAN is not READY for reading [%d]", func_name[req.type], can_status);
                resp_json_data(json_body, resp.get_volt, req, response);
                return U_CALLBACK_CONTINUE;
            }
            count_err ++;
            syslog(LOG_ERR, "%s : CAN response timeout: %d/10 [%d]", func_name[req.type], count_err, can_status);
            if (count_err == ERR_ATT) {
                count_err = 0;
                resp_json_data(json_body, resp.ext_volt, req, response);
                stop_can_master();
                exit(EXIT_FAILURE);
            }
        }
    }
    can_status = READY;
    resp_json_data(json_body, resp.ext_volt, req, response);
    return U_CALLBACK_CONTINUE;
}

// get the mezzanine temperature and respond to a user
static int callback_mez_temp(const struct _u_request * request, struct _u_response * response, void * _config) {
    int rsize;
    int subindex;
    int node = atoi(u_map_get(request->map_url, "node"));
    int mez_num = atoi(u_map_get(request->map_url, "mez_num"));
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
    http_config_t * config = (http_config_t *) _config;
    json_t * json_body = NULL;
    time_t req_timestamp;
    time_t current_timestamp;
    req_t req = {.type = GetMezTemp, .node = node, .subindex = subindex, .n_set = 1};
    resp_st resp;
    if (can_status != READY) {
        resp_can_busy(json_body, response);
        return U_CALLBACK_CONTINUE;
    }
    can_status = READING;
    write(config->req_fd, &req, sizeof(req_t));
    time(&req_timestamp);
    int count_err = 0;
    while (TRUE) {
        usleep(100000);
        rsize = read(config->resp_fd, &resp, sizeof(resp_st));
        if (rsize == sizeof(resp_st)){
            break;
        }
        time(&current_timestamp);
        if (current_timestamp - req_timestamp > RESP_TIMEOUT) {
            if (can_status == SETTING || can_status == INIT) {
                syslog(LOG_WARNING, "%s : CAN is not READY for reading [%d]", func_name[req.type], can_status);
                resp_json_data(json_body, resp.get_volt, req, response);
                return U_CALLBACK_CONTINUE;
            }
            count_err ++;
            syslog(LOG_ERR, "%s : CAN response timeout: %d/10 [%d]", func_name[req.type], count_err, can_status);
            if (count_err == ERR_ATT) {
                count_err = 0;
                resp_json_data(json_body, resp.mez_temp, req, response);
                stop_can_master();
                exit(EXIT_FAILURE);
            }
        }
    }
    can_status = READY;
    resp_json_data(json_body, resp.mez_temp, req, response);
    return U_CALLBACK_CONTINUE;
}

// Reset board
static int callback_reset_node(const struct _u_request * request, struct _u_response * response, void * _config) {
    int rsize;
    int node = atoi(u_map_get(request->map_url, "node"));
    http_config_t * config = (http_config_t *) _config;
    json_t * json_body = NULL;
    time_t req_timestamp;
    time_t current_timestamp;
    req_t req = {.type = ResetNode, .node = node, .subindex = 0, .n_set = 1};
    resp_st resp;
    can_status = INIT;
    write(config->req_fd, &req, sizeof(req_t));
    can_status = READY;
    syslog(LOG_DEBUG, "%s - CAN status: %s [%d]", func_name[req.type], can_status_name[can_status], can_status);
    json_body = json_object();
    resp.reset_node.node = req.node;
    json_object_set_new(json_body, "node", json_integer(resp.reset_node.node));
    json_object_set_new(json_body, "can_status", json_integer(can_status));
    ulfius_set_json_body_response(response, 200, json_body);
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
}

// Reset can network
static int callback_reset_can_network(const struct _u_request * request, struct _u_response * response, void * _config) {
    int rsize;
    http_config_t * config = (http_config_t *) _config;
    json_t * json_body = NULL;
    time_t req_timestamp;
    time_t current_timestamp;
    req_t req = {.type = ResetCanNetwork, .node = 0, .subindex = 0, .n_set = 1};
    resp_st resp;
    can_status = INIT;
    write(config->req_fd, &req, sizeof(req_t));
    can_status = READY;
    json_body = json_object();
    json_object_set_new(json_body, "can_status", json_integer(can_status));
    resp.reset_can_network.node = req.node;
    ulfius_set_json_body_response(response, 200, json_body);
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
}

// Get CAN status
static int callback_get_can_status(const struct _u_request * request, struct _u_response * response, void * _config) {
    http_config_t * config = (http_config_t *) _config;
    json_t * json_body = NULL;
    json_body = json_object();
    json_object_set_new(json_body, "can_status", json_integer(can_status));
    ulfius_set_json_body_response(response, 200, json_body);
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
}

// Set CAN status SETTING
static int callback_set_can_status_set(const struct _u_request * request, struct _u_response * response, void * _config) {
    can_status = SETTING;
    http_config_t * config = (http_config_t *) _config;
    json_t * json_body = NULL;
    json_body = json_object();
    json_object_set_new(json_body, "can_status", json_integer(can_status));
    ulfius_set_json_body_response(response, 200, json_body);
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
}

// Set CAN status READY
static int callback_set_can_status_ready(const struct _u_request * request, struct _u_response * response, void * _config) {
    can_status = READY;
    http_config_t * config = (http_config_t *) _config;
    json_t * json_body = NULL;
    json_body = json_object();
    json_object_set_new(json_body, "can_status", json_integer(can_status));
    ulfius_set_json_body_response(response, 200, json_body);
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
}

// parse request and put the voltage to the pipe
static int callback_set_voltage(const struct _u_request * request, struct _u_response * response, void * _config) {
    can_status = SETTING;
    http_config_t * config = (http_config_t *) _config;
    int channel = atoi(u_map_get(request->map_url, "channel"));
    int node = atoi(u_map_get(request->map_url, "node"));
    unsigned16 DAC_code;
    json_t * json_body = ulfius_get_json_body_request(request, NULL);
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
    req_t req = { SetVoltage, node, channel, DAC_code, 1 };
    write(config->req_fd, &req, sizeof(req_t));
    ulfius_set_string_body_response(response, 200, "Ok\n");
    json_decref(json_body);
    json_body = json_object();
    json_object_set_new(json_body, "can_status", json_integer(can_status));
    ulfius_set_json_body_response(response, 200, json_body);
    json_decref(json_body);
    can_status = READY;
    return U_CALLBACK_CONTINUE;
}

// SET VOLTAGE LIST
static int callback_set_voltage_list(const struct _u_request * request, struct _u_response * response, void * _config) {
    float SET_TIME_OUT = 0.11;
    can_status = SETTING;
    http_config_t * config = (http_config_t *) _config;
    int node = atoi(u_map_get(request->map_url, "node"));
    unsigned16 DAC_code;
    int channel;
    json_t * json_body = ulfius_get_json_body_request(request, NULL);
    json_t * data = json_object_get(json_body, "data");
    const int n_ch = json_array_size(data);
    for (int i = 0; i<n_ch; i++) {
        json_t * item = json_array_get(data, i);
        json_t * channel = json_object_get(item, "ch");
        json_t * dac_code = json_object_get(item, "value");
        int dac_code_int = (unsigned16)json_integer_value(dac_code);
        int ch_int = (unsigned16)json_integer_value(channel);
        req_t req = { SetVoltage, node, ch_int, dac_code_int, n_ch };
        write(config->req_fd, &req, sizeof(req_t));
    }
    ulfius_set_string_body_response(response, 200, "Ok\n");
    json_decref(json_body);
    json_body = json_object();
    json_object_set_new(json_body, "can_status", json_integer(can_status));
    ulfius_set_json_body_response(response, 200, json_body);
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
    ulfius_add_endpoint_by_val(&instance, "POST", "/api", "/voltages/:node", 0, &callback_set_voltage_list, config);
    ulfius_add_endpoint_by_val(&instance, "GET", "/api", "/voltage/:node/:channel", 0, &callback_get_voltage, config);
    ulfius_add_endpoint_by_val(&instance, "GET", "/api", "/ref_voltage/:node", 0, &callback_get_ref_voltage, config);
    ulfius_add_endpoint_by_val(&instance, "GET", "/api", "/ext_voltage/:node", 0, &callback_get_ext_voltage, config);
    ulfius_add_endpoint_by_val(&instance, "GET", "/api", "/mez_temp/:node/:mez_num", 0, &callback_mez_temp, config);
    ulfius_add_endpoint_by_val(&instance, "GET", "/api", "/reset/:node", 0, &callback_reset_node, config);
    ulfius_add_endpoint_by_val(&instance, "GET", "/api", "/reset_can_network", 0, &callback_reset_can_network, config);
    ulfius_add_endpoint_by_val(&instance, "GET", "/api", "/get_can_status", 0, &callback_get_can_status, config);
    ulfius_add_endpoint_by_val(&instance, "GET", "/api", "/set_can_status_set", 0, &callback_set_can_status_set, config);
    ulfius_add_endpoint_by_val(&instance, "GET", "/api", "/set_can_status_ready", 0, &callback_set_can_status_ready, config);
    if (ulfius_start_framework(&instance) == U_OK) {
        while (TRUE)
        {
            syslog(LOG_INFO, "Supplr main loop is running on http://localhost:%d - CAN Status: %d",config->port,can_status);
            usleep(1e+07);
        }
    } else {
        syslog(LOG_ERR, "Error while starting web server");
        exit(EXIT_FAILURE);
    }
    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);
    return NULL;
}
