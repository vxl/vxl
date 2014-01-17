#include "sdet_register.h"
#include "sdet_processes.h"
#include "processes/sdet_detect_edges_process.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>
#include <sdet/sdet_texture_classifier.h>

void sdet_register::register_datatype()
{
  REGISTER_DATATYPE(sdet_texture_classifier_sptr);
}

void sdet_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_detect_edges_process, "sdetDetectEdgesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_detect_edge_tangent_process, "sdetDetectEdgesTangentProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_detect_edge_tangent_interp_process, "sdetDetectEdgesTangentInterpProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_write_edge_file_process, "sdetWriteEdgeFileProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_detect_edge_line_fitted_process, "sdetDetectEdgesLineFittedProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_detect_third_order_edges_process, "sdetDetectThirdOrderEdgesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_detect_third_order_edges_dt_process, "sdetDetectThirdOrderEdgesDTProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_texture_classifier_process, "sdetTextureClassifierProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_texture_classifier_kernel_margin_process, "sdetTextureClassifierKernelMarginProcess");
  REG_PROCESS_FUNC_CONS_FIN(bprb_func_process, bprb_batch_process_manager, sdet_texture_training_process, "sdetTextureTrainingProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_create_texture_classifier_process, "sdetCreateTextureClassifierProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_print_texton_dict_process, "sdetPrintTextonDictProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_exp_img_classifier_process, "sdetExpImgClassifierProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_save_texture_classifier_process, "sdetSaveTextureClassifierProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_load_texture_classifier_process, "sdetLoadTextureClassifierProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_segment_image_process, "sdetSegmentImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_segment_image_using_edge_map_process, "sdetSegmentUsingEdgesProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_segment_image_using_height_map_process, "sdetSegmentUsingHeightMapProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_segment_image_using_height_map_process2, "sdetSegmentUsingHeightMapProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_texture_classifier_process2, "sdetTextureClassifierProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_extract_filter_bank_process, "sdetExtractFilterBankProcess");
  REG_PROCESS_FUNC_CONS_FIN(bprb_func_process, bprb_batch_process_manager, sdet_texture_training_process2, "sdetTextureTrainingProcess2");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_load_texture_dictionary_process, "sdetLoadTextureDictionaryProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_add_to_filter_bank_process, "sdetAddtoFilterBankProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_texture_classifier_roc_process, "sdetTextureClassifierROCProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, sdet_add_to_filter_bank_process2, "sdetAddtoFilterBankProcess2"); 
  
}
