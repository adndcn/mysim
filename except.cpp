#include "except.h"
#include "spr-defs.h"
#include "cpu.h"

EXCEPT::EXCEPT(CPU *cpu_pointer)
{
    cpu = cpu_pointer;
}

EXCEPT::~EXCEPT()
{
    
}

void EXCEPT::except_handle(oraddr_t except, oraddr_t ea)
{
    oraddr_t except_vector;

    /* context switches abort load link / store conditional sequences */
    // cpu_state.loadlock_active = 0;

    // if (debug_ignore_exception(except))
    //     return;

    /* In the dynamic recompiler, this function never returns, so this is not
   * needed.  Ofcourse we could set it anyway, but then all code that checks
   * this variable would break, since it is never reset */
    except_pending = 1;

    except_vector =
        except + (cpu->sprs[SPR_SR] & SPR_SR_EPH ? 0xf0000000 : 0x00000000);

    cpu->pcnext = except_vector;

    cpu->sprs[SPR_EEAR_BASE] = ea;
    cpu->sprs[SPR_ESR_BASE] = cpu->sprs[SPR_SR];

    cpu->sprs[SPR_SR] &= ~SPR_SR_OVE; /* Disable overflow flag exception. */

    cpu->sprs[SPR_SR] |= SPR_SR_SM;                  /* SUPV mode */
    cpu->sprs[SPR_SR] &= ~(SPR_SR_IEE | SPR_SR_TEE); /* Disable interrupts. */

    /* Address translation is always disabled when starting exception. */
    cpu->sprs[SPR_SR] &= ~SPR_SR_DME;

    switch (except)
    {
        /* EPCR is irrelevent */
    case EXCEPT_RESET:
        break;
        /* EPCR is loaded with address of instruction that caused the exception */
    case EXCEPT_ITLBMISS:
    case EXCEPT_IPF:
        cpu->sprs[SPR_EPCR_BASE] = ea - (cpu->delay_insn ? 4 : 0);
        break;
    case EXCEPT_BUSERR:
    case EXCEPT_DPF:
    case EXCEPT_ALIGN:
    case EXCEPT_ILLEGAL:
    case EXCEPT_DTLBMISS:
    case EXCEPT_RANGE:
    case EXCEPT_TRAP:
        /* All these exceptions happen during a simulated instruction */
        cpu->sprs[SPR_EPCR_BASE] =
            cpu->pc - (cpu->delay_insn ? 4 : 0);
        break;
        /* EPCR is loaded with address of next not-yet-executed instruction */
    case EXCEPT_SYSCALL:
        cpu->sprs[SPR_EPCR_BASE] =
            (cpu->pc + 4) - (cpu->delay_insn ? 4 : 0);
        break;
        /* These exceptions happen AFTER (or before) an instruction has been
       * simulated, therefore the pc already points to the *next* instruction */
    case EXCEPT_TICK:
    case EXCEPT_INT:
        cpu->sprs[SPR_EPCR_BASE] =
            cpu->pc - (cpu->delay_insn ? 4 : 0);
        /* If we don't update the pc now, then it will only happen *after* the next
       * instruction (There would be serious problems if the next instruction just
       * happens to be a branch), when it should happen NOW. */
        cpu->pc = cpu->pcnext;
        cpu->pcnext += 4;
        break;
    }

    /* Address trnaslation is here because run_sched_out_of_line calls
   * eval_insn_direct which checks out the immu for the address translation but
   * if it would be disabled above then there would be not much point... */
    cpu->sprs[SPR_SR] &= ~SPR_SR_IME;

    /* Complex/simple execution strictly don't need this because of the
   * next_delay_insn thingy but in the dynamic execution modell that doesn't
   * exist and thus cpu->delay_insn would stick in the exception handler
   * causeing grief if the first instruction of the exception handler is also in
   * the delay slot of the previous instruction */
    cpu->delay_insn = 0;
}