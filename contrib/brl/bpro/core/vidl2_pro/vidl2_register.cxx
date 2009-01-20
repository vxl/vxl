#include "vidl2_register.h"

#include "vidl2_open_istream_process.h"
#include "vidl2_open_ostream_process.h"
#include "vidl2_get_frame_istream_process.h"
#include "vidl2_close_istream_process.h"
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
  REG_PROCESS(vidl2_open_istream_process, bprb_batch_process_manager);
  REG_PROCESS(vidl2_open_ostream_process, bprb_batch_process_manager);
  REG_PROCESS(vidl2_get_frame_istream_process, bprb_batch_process_manager);
  REG_PROCESS(vidl2_close_istream_process, bprb_batch_process_manager);
#ifdef HAS_DIRECTSHOW
  REG_PROCESS(vidl2_open_dshow_istream_process, bprb_batch_process_manager);
#endif
}
