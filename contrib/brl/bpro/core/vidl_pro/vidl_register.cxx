#include "vidl_register.h"
#include "vidl_processes.h"

#include <vidl/vidl_istream_sptr.h>
#include <vidl/vidl_ostream_sptr.h>

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>

void vidl_register::register_datatype()
{
  REGISTER_DATATYPE(vidl_istream_sptr);
  REGISTER_DATATYPE(vidl_ostream_sptr);
}

void vidl_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vidl_open_istream_process, "vidlOpenIstreamProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vidl_open_ostream_process, "vidlOpenOstreamProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vidl_get_frame_istream_process, "vidlGetFrameIstreamProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vidl_put_frame_ostream_process, "vidlPutFrameOstreamProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vidl_close_istream_process, "vidlCloseIstreamProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vidl_close_ostream_process, "vidlCloseOstreamProcess");
#if defined(HAS_DIRECTSHOW) && HAS_DIRECTSHOW
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vidl_open_dshow_istream_process, "vidlOpenDshowIstreamProcess");
#endif
}
