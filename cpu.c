#include "types.h"
#include "spr-defs.h"
#include "cpu.h"


/*---------------------------------------------------------------------------*/
/*!Set a specific register with value

Implementation specific. Abort if we are given a duff register.

@param[in] regno  The register of interest
@param[in] value  The value to be set                                     */
/*---------------------------------------------------------------------------*/
void setsim_reg(unsigned int regno,
	uorreg_t value)
{
	if (regno == 0) /* gpr0 is always zero */
	{
		value = 0;
	}

	if (regno < MAX_GPRS)
	{
		cpu_state.reg[regno] = value;
	}
	else
	{
		//PRINTF("\nABORT: write out of registers\n");
		//sim_done();
	}

} /* setsim_reg() */