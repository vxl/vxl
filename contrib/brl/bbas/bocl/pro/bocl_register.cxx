#include "bocl_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "bocl_processes.h"

void bocl_register::register_datatype()
{  

}

void bocl_register::register_process()
{
    REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bocl_init_process,"boclInitProcess");
}
