#include "betr_register.h"
#include "betr_processes.h"
#include <betr/betr_site.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include "betr_processes.h"

void betr_register::register_datatype()
{
  REGISTER_DATATYPE( betr_site_sptr );
}

void betr_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, betr_create_site_process, "betrCreateSiteProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, betr_add_event_object_process, "betrAddEventObjectProcess");
}
