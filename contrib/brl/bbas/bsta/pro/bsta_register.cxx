#include "bsta_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <bsta/bsta_histogram_sptr.h>
#include <bsta/algo/bsta_sigma_normalizer.h>
#include <bsta/bsta_random_wrapper.h>

#include "bsta_processes.h"

void bsta_register::register_datatype()
{
  REGISTER_DATATYPE( bsta_histogram_sptr );
  REGISTER_DATATYPE( bsta_sigma_normalizer_sptr );
  REGISTER_DATATYPE( bsta_random_wrapper_sptr );
}

void bsta_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bsta_sigma_normalization_table_process, "bstaSigmaNormTableProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bsta_initialize_random_seed_process, "bstaInitializeRandomSeedProcess");
}
