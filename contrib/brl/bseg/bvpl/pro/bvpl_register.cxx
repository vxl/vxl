#include "bvpl_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include "bvpl_processes.h"
#include <bvpl/bvpl_kernel_factory.h>

void bvpl_register::register_datatype()
{
  REGISTER_DATATYPE( bvpl_kernel_sptr );
  REGISTER_DATATYPE( bvpl_kernel_vector_sptr );
}

void bvpl_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_neighborhood_operator_process, "bvplNeighborhoodOperatorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_vector_operator_process, "bvplVectorOperatorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_edge2d_kernel_process, "bvplCreateEdge2dKernelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_edge3d_kernel_process, "bvplCreateEdge3dKernelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_gauss3d_xx_kernel_process, "bvplCreateGauss3dXXKernelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_edge2d_kernel_vector_process, "bvplCreateEdge2dKernelVectorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_edge3d_kernel_vector_process, "bvplCreateEdge3dKernelVectorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_gauss3d_xx_kernel_vector_process, "bvplCreateGauss3dXXKernelVectorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_convert_direction_to_hue_process, "bvplConvertDirectionToHueProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_compare_surface_and_normal_process, "bvplCompareSurfaceAndNormalProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_susan_opinion_operator_process, "bvplSusanOpinionOperatorProcess");


}
