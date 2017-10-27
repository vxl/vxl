#include "brip_register.h"
#include "brip_processes.h"


#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_batch_process_manager.h>

void brip_register::register_datatype()
{
}

void brip_register::register_process()
{
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brip_extrema_process, "bripExtremaProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brip_blobwise_mutual_info_process, "bripBlobwiseMutualInfoProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brip_blobwise_kl_div_process, "bripBlobwiseKLDivProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brip_blob_intersection_process, "bripBlobIntersectionProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brip_solve_gain_offset_process, "bripSolveGainOffsetProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brip_truncate_nitf_bit_process, "bripTruncateNITFBitProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brip_solve_gain_offset_constraints_process, "bripSolveGainOffsetConstraintsProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brip_image_mutual_info_process,"bripImageMutualInfoProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, brip_phase_correlation_process, "bripPhaseCorrelationProcess");
}
