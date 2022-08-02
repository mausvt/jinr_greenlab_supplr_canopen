#ifndef __MASTER_DEFUNC_H__
#define __MASTER_DEFUNC_H__

// ..CANopen/system.c
void can_sleep(int32 microseconds);
void can_init_system_timer(void (*handler)(void));
void can_cancel_system_timer(void);

// ..CANopen\master_backinit.c functions below
int16 start_can_master(char *path_config);
int16 stop_can_master(void);
void canopen_monitor(void);

// ..CANopen\master_client.c functions below
void can_sdo_client_transfer(struct sdocltappl *ca);

// ..CANopen\master_cltrans.c functions below
void can_client_sdo(canframe *cf);
void can_client_basic(struct sdoclttrans *ct);
void can_client_control(void);
void set_sdo_timeout(unsigned32 microseconds);
unsigned32 get_sdo_timeout(void);
void can_init_client(void);

// ..CANopen\master_canid.c functions below
unsigned8 check_sdo_canid(cansubind subind, canlink canid);
unsigned8 check_canid_restricted(canlink canid);

// ..CANopen\master_inout.c functions below
void push_all_can_data(void);
int16 send_can_data(canframe *cf);        // 3.0.1 API changed
void can_read_handler(canev ev);
void can_init_io(void);

// ..CANopen\master_lib.c functions below
int16 check_node_id(cannode node);
int16 check_bitrate_index(unsigned8 br);
void clear_can_data(canbyte *data);
void u16_to_canframe(unsigned16 ud, canbyte *data);
unsigned16 canframe_to_u16(canbyte *data);
void u32_to_canframe(unsigned32 ud, canbyte *data);
unsigned32 canframe_to_u32(canbyte *data);

// ..CANopen\master_obdsdo_client.c functions below
int16 find_sdo_client_recv_canid(canlink *canid);
int16 find_sdo_client_send_canid(canlink *canid);
int16 read_sdo_client_data(cansubind subind, unsigned32 *data);
int16 write_sdo_client_data(cansubind subind, unsigned32 data);
void can_init_sdo_client(void);

// ..CANopen\master_obj_sync.c functions below
int16 find_sync_recv_canid(canlink *canid);
int16 read_sync_object(canindex index, unsigned32 *data);
int16 write_sync_object(canindex index, unsigned32 data);
unsigned8 sync_window_expired(void);
void sync_received(canframe *cf);
void control_sync(void);
void can_init_sync(void);

// ..CANopen\master_sdo_proc.c functions below
void parse_sdo(struct cansdo *sd, canbyte *data);
int16 send_can_sdo(struct cansdo *sd);
void abort_can_sdo(struct sdoixs *si, unsigned32 abortcode);

// ..application/config_parser.c
int config_parser(char *path_config);

// ..application\master_application.c functions below
void reset_can_node(cannode node);
void start_can_network(void);
void init_defaults(void);
void configure(char *path_config);

// ..application\master_can_nodes functions below
void configure_can_nodes(void);

// ..application\master_events.c functions below
void consume_sync(unsigned8 sc);
void no_sync_event(void);
void consume_controller_error(canev ev);
void master_emcy(unsigned16 errorcode);
void consume_emcy(canframe *cf);
void can_cache_overflow(void);

// ..application\master_nmt_master.c functions below
void nmt_master_command(unsigned8 cs, cannode node);
void consume_nmt(canframe *cf);
void manage_master_ecp(void);

// ..application\master_pdo_process.c functions below
int16 transmit_can_pdo(cannode node, unsigned8 pn);
void receive_can_pdo(canframe *cf);
void process_sync_pdo(unsigned8 sc);
void can_init_pdo(void);

// ..application\master_sdo_transfer.c functions below
int16 read_device_object(cannode node, canindex index, cansubind subind, canbyte *data, unsigned32 datasize);
int16 write_device_object(cannode node, canindex index, cansubind subind, canbyte *data, unsigned32 datasize);

// ..CANopen/logger.c
void master_event(unsigned8 cls, unsigned8 type, int16 code, int32 info);
void node_event(cannode node, unsigned8 cls, unsigned8 type, int16 code, int32 info);

// ..CANopen/master_logfile.c
void log_event(struct eventlog *ev);

// ..application/clparser.c
struct configuration *clparser(int argc, char** argv);

// ..CANopen/master_obj_errors.c
void set_error_field(unsigned16 errorcode, unsigned16 addinf);

#endif
