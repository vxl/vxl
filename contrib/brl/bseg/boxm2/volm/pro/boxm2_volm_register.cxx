#include "boxm2_volm_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <boxm2/volm/boxm2_volm_locations_sptr.h>
#include "boxm2_volm_processes.h"

void boxm2_volm_register::register_datatype()
{
  REGISTER_DATATYPE( boxm2_volm_loc_hypotheses_sptr );
}

void boxm2_volm_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_load_hypotheses_process, "boxm2LoadHypothesesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_index_process, "boxm2IndexHypothesesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_get_hypothesis_process, "boxm2GetHypothesisProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_hypotheses_process, "boxm2CreateHypothesesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_add_hypothesis_process, "boxm2AddHypothesisProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_save_hypotheses_process, "boxm2SaveHypothesesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_visualize_index_process, "boxm2VisualizeIndicesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_partition_hypotheses_process, "boxm2PartitionHypsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_hypotheses_kml_process, "boxm2HypoKmlProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_create_index_process2, "boxm2IndexHypothesesProcess2");
}
