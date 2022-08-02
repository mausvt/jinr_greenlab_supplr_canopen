#include <syslog.h>
#include <master_header.h>

void master_event(unsigned8 cls, unsigned8 type, int16 code, int32 info)
{
    node_event(EVENT_NODE_MASTER, cls, type, code, info);
}

void node_event(cannode node, unsigned8 cls, unsigned8 type, int16 code, int32 info)
{
    struct eventlog ev;
    // syslog(LOG_DEBUG, "Logging event: node_id: %d cls: %d type: %d code: %d info: %d", node, cls, type, code, info);

    memset(&ev, 0, sizeof(struct eventlog));
    ev.node = node;
    ev.cls = cls;
    ev.type = type;
    ev.code = code;
    ev.info = info;
    log_event(&ev);
}
