#include "bbgm_register.h"
#include "bbgm_load_image_of_process.h"
#include "bbgm_save_image_of_process.h"
#include "bbgm_update_dist_image_process.h"
#include "bbgm_update_dist_image_stream_process.h"
#include "bbgm_display_dist_image_process.h"
#include "bbgm_measure_process.h"
#include "bbgm_local_frame_trans_process.h"
#include <bbgm/bbgm_image_sptr.h>

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>


void bbgm_register::register_datatype()
{
  REGISTER_DATATYPE( bbgm_image_sptr );
}

void bbgm_register::register_process()
{
  REG_PROCESS(bbgm_load_image_of_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_save_image_of_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_update_dist_image_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_update_dist_image_stream_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_display_dist_image_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_measure_process, bprb_batch_process_manager);
  REG_PROCESS(bbgm_local_frame_trans_process, bprb_batch_process_manager);
}
