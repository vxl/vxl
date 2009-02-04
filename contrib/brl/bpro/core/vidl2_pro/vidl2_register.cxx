#include "vidl2_register.h"
#include "vidl2_processes.h"

#include <vidl2/vidl2_istream_sptr.h>
#include <vidl2/vidl2_ostream_sptr.h>

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>

void vidl2_register::register_datatype()
{
  REGISTER_DATATYPE(vidl2_istream_sptr);
  REGISTER_DATATYPE(vidl2_ostream_sptr);
}

void vidl2_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vidl2_open_istream_process, "vidl2OpenIstreamProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vidl2_open_ostream_process, "vidl2OpenOstreamProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vidl2_get_frame_istream_process, "vidl2GetFrameIstreamProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vidl2_close_istream_process, "vidl2CloseIstreamProcess");
#if HAS_DIRECTSHOW
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, vidl2_open_dshow_istream_process, "vidl2OpenDshowIstreamProcess");
#endif
}
