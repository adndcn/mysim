#include "tick.h"
#include "cpu.h"

/*! Indicates if the timer is actually counting.  Needed to simulate one-shot
    mode correctly */
int tick_counting;

/*! Reset. It initializes TTCR register. */
void tick_reset(void)
{
    cpu_state.sprs[SPR_TTCR] = 0;
    cpu_state.sprs[SPR_TTMR] = 0;
    tick_counting = 0;
}