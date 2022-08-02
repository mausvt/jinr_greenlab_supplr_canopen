#include <master_header.h>
#include <syslog.h>

#if CHECK_VERSION_CANLIB(3, 0, 1)

#if CAN_APPLICATION_MODE == MASTER

void set_error_field(unsigned16 errorcode, unsigned16 addinf)
{
    syslog(LOG_INFO, "set_error_field: code %d, addinf: %d", errorcode, addinf);
#if CAN_OBJECT_EMCY == TRUE
    master_emcy(errorcode);        // 2.3.3
#endif
}

#endif

#endif
