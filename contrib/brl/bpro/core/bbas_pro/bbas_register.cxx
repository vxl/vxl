#include "bbas_register.h"
#include "bbas_processes.h"


#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>

void bbas_register::register_datatype()
{
}

void bbas_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_display_sun_index_process, "bradDisplaySunIndexProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brad_sun_dir_bin_process, "bradSunDirBinProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager,brad_save_sun_index_process , "bradSaveSunIndexProcess");

}
