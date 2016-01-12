#include "bocl_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "bocl_processes.h"
#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>

void bocl_register::register_datatype()
{

    REGISTER_DATATYPE( bocl_device_sptr );

}

void bocl_register::register_process()
{
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bocl_init_manager_process,"boclInitManagerProcess");
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bocl_get_device_process,"boclGetDeviceProcess");
    REG_PROCESS_FUNC_CONS2(bocl_info_process);

}
