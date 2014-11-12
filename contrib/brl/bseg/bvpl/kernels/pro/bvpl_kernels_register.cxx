#include "bvpl_kernels_register.h"

#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_func_process.h>
#include "bvpl_kernels_processes.h"
#include <bvpl/kernels/bvpl_kernel.h>
#include <bvpl/util/bvpl_corner_pair_finder.h>

void bvpl_kernels_register::register_datatype()
{
  REGISTER_DATATYPE( bvpl_kernel_sptr );
  REGISTER_DATATYPE( bvpl_kernel_vector_sptr );
}

void bvpl_kernels_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_edge2d_kernel_process, "bvplCreateEdge2dKernelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_edge3d_kernel_process, "bvplCreateEdge3dKernelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_gauss3d_x_kernel_process, "bvplCreateGauss3dXKernelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_gauss3d_xx_kernel_process, "bvplCreateGauss3dXXKernelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_edge2d_kernel_vector_process, "bvplCreateEdge2dKernelVectorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_edge3d_kernel_vector_process, "bvplCreateEdge3dKernelVectorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_corner2d_kernel_vector_process, "bvplCreateCorner2dKernelVectorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_corner2d_kernel_process, "bvplCreateCorner2dKernelProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_gauss3d_xx_kernel_vector_process, "bvplCreateGauss3dXXKernelVectorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_create_wc_kernel_vector_process, "bvplCreateWCKernelVectorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bvpl_load_taylor_kernel_process, "bvplLoadTaylorKernelProcess");
  REG_PROCESS_FUNC_CONS2(bvpl_create_gauss3d_kernel_process);
  REG_PROCESS_FUNC_CONS2(bvpl_create_generic_kernel_vector_process);
  REG_PROCESS_FUNC_CONS2(bvpl_write_generic_kernel_vector_process);
}
