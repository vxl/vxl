#include "brec_register.h"
#include "brec_update_changes_process.h"
#include "brec_density_to_prob_map_process.h"
#include "brec_prob_map_roc_compute_process.h"
#include "brec_prob_map_threshold_process.h"
#include "brec_recognize_structure_process.h"
#include "brec_prob_map_supress_process.h"
#include "brec_prob_map_area_process.h"
#include "brec_change_area_process.h"
#include "brec_bayesian_update_process.h"
#include "brec_glitch_process.h"
#include "brec_glitch_overlay_process.h"
#include "brec_create_mog_image_process.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>

void brec_register::register_datatype()
{
}

void brec_register::register_process()
{
  REG_PROCESS(brec_update_changes_process, bprb_batch_process_manager);
  REG_PROCESS(brec_prob_map_roc_compute_process, bprb_batch_process_manager);
  REG_PROCESS(brec_density_to_prob_map_process, bprb_batch_process_manager);
  REG_PROCESS(brec_recognize_structure_process, bprb_batch_process_manager);
  REG_PROCESS(brec_prob_map_threshold_process, bprb_batch_process_manager);
  REG_PROCESS(brec_prob_map_supress_process, bprb_batch_process_manager);
  REG_PROCESS(brec_prob_map_area_process, bprb_batch_process_manager);
  REG_PROCESS(brec_change_area_process, bprb_batch_process_manager);
  REG_PROCESS(brec_bayesian_update_process, bprb_batch_process_manager);
  REG_PROCESS(brec_glitch_process, bprb_batch_process_manager);
  REG_PROCESS(brec_glitch_overlay_process, bprb_batch_process_manager);
  REG_PROCESS(brec_create_mog_image_process, bprb_batch_process_manager);
}

