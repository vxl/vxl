#include "bvpl_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include "bvpl_processes.h"
#include <bvpl/kernels/bvpl_kernel.h>
#include <bvpl/util/bvpl_corner_pair_finder.h>
#include <bvpl/util/bvpl_bundler_features_2d.h>

void bvpl_register::register_datatype()
{
  REGISTER_DATATYPE( bvpl_kernel_sptr );
  REGISTER_DATATYPE( bvpl_kernel_vector_sptr );
  REGISTER_DATATYPE( bvpl_corner_pairs_sptr );
  REGISTER_DATATYPE( bvpl_bundler_features_2d_sptr );
}

void bvpl_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_neighborhood_operator_process, "bvplNeighborhoodOperatorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_vector_operator_process, "bvplVectorOperatorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_edge2d_kernel_process, "bvplCreateEdge2dKernelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_edge3d_kernel_process, "bvplCreateEdge3dKernelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_gauss3d_x_kernel_process, "bvplCreateGauss3dXKernelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_gauss3d_xx_kernel_process, "bvplCreateGauss3dXXKernelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_edge2d_kernel_vector_process, "bvplCreateEdge2dKernelVectorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_edge3d_kernel_vector_process, "bvplCreateEdge3dKernelVectorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_corner2d_kernel_vector_process, "bvplCreateCorner2dKernelVectorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_corner2d_kernel_process, "bvplCreateCorner2dKernelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_gauss3d_xx_kernel_vector_process, "bvplCreateGauss3dXXKernelVectorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_convert_direction_to_hue_process, "bvplConvertDirectionToHueProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_convert_id_to_hue_process, "bvplConvertIdToHueProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_compare_surface_and_normal_process, "bvplCompareSurfaceAndNormalProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_susan_opinion_operator_process, "bvplSusanOpinionOperatorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_kernel_id_to_axis_process, "bvplKernelIdToAxisProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_non_max_suppression_process, "bvplNonMaxSuppressionProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_visualize_response_process, "bvplVisualizeResponseProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_suppress_and_combine_process, "bvplSuppressAndCombineProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_extract_top_response_process, "bvplExtractTopResponseProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_operate_ocp_and_app_process, "bvplOperateOcpAndAppProcess");

  //util
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_find_corner_pairs_process,"bvplFindCornerPairsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_visualize_corner_pairs_process, "bvplVisualizeCornerPairsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_wc_kernel_vector_process, "bvplCreateWCKernelVectorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_convert_pair_to_hue_process, "bvplConvertPairToHueProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_load_pair_grid_process, "bvplLoadPairGridProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_find_pairs_process_process, "bvplFindPairsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_pair_to_float_process, "bvplPairToFloatProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_bundler_features_2d_compute_process, "bvplBundlerFeatures2dComputeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_bundler_features_2d_write_txt_process, "bvplBundlerFeatures2dWriteTxtProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_bundler_features_2d_read_bin_process, "bvplBundlerFeatures2dReadBinProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_bundler_features_2d_write_bin_process, "bvplBundlerFeatures2dWriteBinProcess");
}
