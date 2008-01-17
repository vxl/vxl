// This is brl/bbas/vidl2/vidl2_ffmpeg_ostream_params.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   3 Jan 2006
//
//-----------------------------------------------------------------------------

#include "vidl2_ffmpeg_ostream_params.h"

#include <vidl2/vidl2_config.h>

#if VIDL2_HAS_FFMPEG

extern "C" {
#include <ffmpeg/avformat.h>
}

//-----------------------------------------------------------------------------

//: Many of these default values come from ffmpeg.c
vidl2_ffmpeg_ostream_params::vidl2_ffmpeg_ostream_params()
  : encoder_( DEFAULT ),
    file_format_( GUESS ),
    frame_rate_( 25 ),
    bit_rate_( 5000 ),
    ni_( 640 ),
    nj_( 480 ),
    video_bit_rate_tolerance_( 4000*1000 ),
    frame_aspect_ratio_( 0.0f ),
    intra_only_( 0 ),
    gop_size_( 12 ),
    video_qscale_( 0 ),
    same_quality_( 0 ),
    mb_decision_( FF_MB_DECISION_SIMPLE ),
    mb_cmp_( FF_CMP_SAD ),
    ildct_cmp_( FF_CMP_VSAD ),
    sub_cmp_( FF_CMP_SAD ),
    cmp_( FF_CMP_SAD ),
    pre_cmp_( FF_CMP_SAD ),
    pre_me_( 0 ),
    lumi_mask_( 0.0f ),
    dark_mask_( 0.0f ),
    scplx_mask_( 0.0f ),
    tcplx_mask_( 0.0f ),
    p_mask_( 0.0f ),
    qns_( 0 ),
    use_4mv_( 0 ),
    use_obmc_( 0 ),
    use_loop_( 0 ),
    use_aic_( 0 ),
    use_aiv_( 0 ),
    use_umv_( 0 ),
    use_ss_( 0 ),
    use_alt_scan_( 0 ),
    use_trell_( 0 ),
    use_scan_offset_( 0 ),
    use_part_( 0 ),
    closed_gop_( 0 ),
    use_qpel_( 0 ),
    use_qprd_( 0 ),
    use_cbprd_( 0 ),
    b_frames_( 0 ),
    do_interlace_dct_( 0 ),
    do_interlace_me_( 0 ),
    video_qmin_( 2 ),
    video_qmax_( 31 ),
    video_lmin_( 2*FF_QP2LAMBDA ),
    video_lmax_( 31*FF_QP2LAMBDA ),
    video_mb_qmin_( 2 ),
    video_mb_qmax_( 31 ),
    video_qdiff_( 3 ),
    video_qblur_( 0.5f ),
    video_qcomp_( 0.5f ),
    video_rc_eq_( "tex^qComp" ),
    debug_( 0 ),
    debug_mv_( 0 ),
    video_rc_buffer_size_( 0 ),
    video_rc_buffer_aggressivity_( 1.0f ),
    video_rc_max_rate_( 0 ),
    video_rc_min_rate_( 0 ),
    video_rc_initial_cplx_( 0 ),
    video_b_qfactor_( 1.25f ),
    video_b_qoffset_( 1.25f ),
    video_i_qfactor_( -0.8f ),
    video_i_qoffset_( 0.0f ),
    video_intra_quant_bias_( FF_DEFAULT_QUANT_BIAS ),
    video_inter_quant_bias_( FF_DEFAULT_QUANT_BIAS ),
    dct_algo_( FF_DCT_FASTINT ),
    idct_algo_( FF_DCT_AUTO ),
    me_threshold_( 0 ),
    mb_threshold_( 0 ),
    intra_dc_precision_( 8 ),
    strict_( 0 ),
    error_rate_( 0 ),
    noise_reduction_( 0 ),
    sc_threshold_( 0 ),
    coder_( 0 ),
    context_( 0 ),
    predictor_( 0 ),
#if 0
    // not supported by older versions of FFMPEG
    video_profile_( FF_PROFILE_UNKNOWN ),
    video_level_( FF_LEVEL_UNKNOWN ),
#endif
    me_range_( 0 ),
    do_psnr_( 0 ),
    packet_size_( 0 ),
    me_method_( ME_EPZS ),
    do_pass_( 0 )
{
}

#else // VIDL2_HAS_FFMPEG

vidl2_ffmpeg_ostream_params::vidl2_ffmpeg_ostream_params()
{
}

#endif
