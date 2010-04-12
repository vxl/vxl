#include "bsta_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <bsta/bsta_histogram_sptr.h>

#include "bsta_processes.h"

void bsta_register::register_datatype()
{
  REGISTER_DATATYPE( bsta_histogram_sptr );
}

void bsta_register::register_process()
{
}
