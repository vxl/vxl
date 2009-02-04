#include "brec_register.h"
#include "brec_processes.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>

#include <brec/brec_part_hierarchy_sptr.h>
#include <brec/brec_part_hierarchy_learner_sptr.h>
#include <brec/brec_part_hierarchy_detector_sptr.h>

void brec_register::register_datatype()
{
  REGISTER_DATATYPE( brec_part_hierarchy_sptr );
  REGISTER_DATATYPE( brec_part_hierarchy_learner_sptr );
  REGISTER_DATATYPE( brec_part_hierarchy_detector_sptr );
}

void brec_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_bayesian_update_process, "brecBayesianUpdateProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_change_area_process, "brecChangeAreaProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_create_mog_image_process, "brecCreateMOGImageProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_density_to_prob_map_process, "brecDensityToProbMapProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_glitch_overlay_process, "brecGlitchOverlayProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_glitch_process, "brecGlitchProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_prob_map_area_process, "brecProbMapAreaProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_prob_map_roc_compute_process, "brecProbMapROCProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_prob_map_supress_process, "brecProbMapSupressProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_prob_map_threshold_process, "brecProbMapThresholdProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_recognize_structure_process, "brecRecStructureProcess");

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_update_changes_process, "brecUpdateChangesProcess");

#if 0
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_construct_bg_op_models_process, "brecConstructBGOPModelsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_construct_fg_op_models_process, "brecConstructFGOPModelsProcess");
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_recognize_structure2_process, "brecRecStructure2Process");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_save_hierarchy_process, "brecSaveHierarchyProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_create_hierarchy_process, "brecCreateHierarchyProcess");
  
  
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_learner_layer0_init_process, "brecLearnerL0InitProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_learner_layer0_fit_process, "brecLearnerL0FitProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_learner_layer0_rank_process, "brecLearnerL0RankProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_learner_layer0_update_posterior_stats_process, "brecLearnerL0UpdatePosteriorStatsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_learner_layer0_update_stats_process, "brecLearnerL0UpdateProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brec_load_hierarchy_process, "brecLoadHierarchyProcess");
#endif

}
