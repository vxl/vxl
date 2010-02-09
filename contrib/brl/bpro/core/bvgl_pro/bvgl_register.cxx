#include "bvgl_register.h"
#include "bvgl_processes.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>

void bvgl_register::register_datatype()
{
}

void bvgl_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvgl_generate_mask_process, "bvglGenerateMaskProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvgl_generate_mask_process2, "bvglGenerateMaskProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvgl_set_change_type_process, "bvglSetChangeTypeProcess");
}
