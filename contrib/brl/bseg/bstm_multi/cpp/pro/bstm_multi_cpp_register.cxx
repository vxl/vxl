#include "bstm_multi_cpp_register.h"

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_macros.h>

#include "bstm_multi_cpp_processes.h"

void bstm_multi_cpp_register::register_datatype() {}

void bstm_multi_cpp_register::register_process() {
  REG_PROCESS_FUNC_CONS(bprb_func_process,
                        bprb_batch_process_manager,
                        bstm_multi_cpp_bstm_to_bstm_multi_scene_process,
                        "bstmMultiCppBstmToBstmMultiSceneProcess");
}
