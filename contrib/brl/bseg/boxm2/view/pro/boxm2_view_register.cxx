#include "boxm2_view_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>

#include "boxm2_view_processes.h"

void boxm2_view_register::register_datatype()
{
}

void boxm2_view_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_view_expected_image_process,"boxm2ViewExpectedImageProcess");
}
