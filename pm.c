#include "pm.h"
#include "cpu.h"

/*---------------------------------------------------------------------------*/
/*!Reset power management

   Initializes PMR register by clearing it.                                  */
/*---------------------------------------------------------------------------*/
void pm_reset()
{

    cpu_state.sprs[SPR_PMR] = 0;

}				/* pm_reset() */