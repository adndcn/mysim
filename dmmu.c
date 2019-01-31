#include "dmmu.h"

oraddr_t dmmu_translate(oraddr_t virtaddr, int write_access)
{
	int i;

	if (!(cpu_state.sprs[SPR_SR] & SPR_SR_DME) ||
		!(cpu_state.sprs[SPR_UPR] & SPR_UPR_DMP))
	{
		/*data_ci = (virtaddr >= 0x80000000);*/
		return virtaddr;
	}
	


	return 0;
}


