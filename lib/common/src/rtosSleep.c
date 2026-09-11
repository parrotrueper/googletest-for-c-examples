#include "targetOs.h"
#include "targetSleep.h"

void target_sleepMs(uint32_t in_milliseconds)
{
    (void)tx_thread_sleep((ULONG)(in_milliseconds));
}
