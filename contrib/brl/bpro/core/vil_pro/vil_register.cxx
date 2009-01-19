#include "vil_register.h"
#include "vil_binary_image_op_process.h"
#include "vil_combine_grey_images_process.h"
#include "vil_convert_to_n_planes_process.h"
#include "vil_gaussian_process.h"
#include "vil_gradient_process.h"
#include "vil_image_pair_process.h"
#include "vil_load_from_ascii_process.h"
#include "vil_load_image_view_binary_process.h"
#include "vil_load_image_view_process.h"
#include "vil_map_image_binary_process.h"
#include "vil_map_image_process.h"
#include "vil_rgbi_to_grey_process.h"
#include "vil_save_image_view_binary_process.h"
#include "vil_save_image_view_process.h"
#include "vil_stretch_image_process.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>

#include <vil/vil_image_view_base.h>

void vil_register::register_datatype()
{
  REGISTER_DATATYPE(vil_image_view_base_sptr);
}

void vil_register::register_process()
{
  REG_PROCESS(vil_binary_image_op_process, bprb_batch_process_manager);
  REG_PROCESS(vil_combine_grey_images_process, bprb_batch_process_manager);
  REG_PROCESS(vil_convert_to_n_planes_process, bprb_batch_process_manager);
  REG_PROCESS(vil_gaussian_process, bprb_batch_process_manager);
  REG_PROCESS(vil_gradient_process, bprb_batch_process_manager);
  REG_PROCESS(vil_image_pair_process, bprb_batch_process_manager);
  REG_PROCESS(vil_load_from_ascii_process, bprb_batch_process_manager);
  REG_PROCESS(vil_load_image_view_binary_process, bprb_batch_process_manager);
  REG_PROCESS(vil_load_image_view_process, bprb_batch_process_manager);
  REG_PROCESS(vil_map_image_binary_process, bprb_batch_process_manager);
  REG_PROCESS(vil_map_image_process, bprb_batch_process_manager);
  REG_PROCESS(vil_rgbi_to_grey_process, bprb_batch_process_manager);
  REG_PROCESS(vil_save_image_view_binary_process, bprb_batch_process_manager);
  REG_PROCESS(vil_save_image_view_process, bprb_batch_process_manager);
  REG_PROCESS(vil_stretch_image_process, bprb_batch_process_manager);
}

