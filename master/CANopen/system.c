#include <master_header.h>

#define CAN_TIMERUSEC_MIN        100    // Minimum background timer period in microseconds.

void can_sleep(int32 microseconds)
{
    struct timespec nsl;

    if (microseconds <= 0) return;
    nsl.tv_sec = microseconds / 1000000;
    nsl.tv_nsec = 1000 * (microseconds % 1000000);
    while (nanosleep(&nsl, &nsl) == -1) {
        if (errno != EINTR) break;
    }
}

void can_init_system_timer(void (*handler)(void))
{
    struct itimerval val;
    struct sigaction sigact;

    if (CAN_TIMERUSEC < CAN_TIMERUSEC_MIN) {
        set_error_field(CAN_EMCYREG_TIMERFAIL, 0);
        return;
    }
    sigact.sa_handler = (void *)handler;
    sigfillset(&sigact.sa_mask);    // Block all other signals during execution of the handler
    sigact.sa_flags = SA_NODEFER;  // The signal is NOT self-blocked
    if (sigaction(SIGALRM, &sigact, NULL) < 0) {
        set_error_field(CAN_EMCYREG_TIMERFAIL, 0);
        return;
    }
    val.it_value.tv_sec = 0;
    val.it_value.tv_usec = CAN_TIMERUSEC;
    val.it_interval.tv_sec = 0;
    val.it_interval.tv_usec = CAN_TIMERUSEC;
    if (setitimer(ITIMER_REAL, &val, NULL) < 0) {
        set_error_field(CAN_EMCYREG_TIMERFAIL, 0);
    }
}

void can_cancel_system_timer(void)
{
    struct itimerval val;
    struct sigaction sigact;

    sigact.sa_handler = SIG_IGN;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(SIGALRM, &sigact, NULL);
    val.it_value.tv_sec = 0;
    val.it_value.tv_usec = 0;
    val.it_interval.tv_sec = 0;
    val.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &val, NULL);
}
