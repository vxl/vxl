#ifndef vxl_jpeg_mangle_h
#define vxl_jpeg_mangle_h

/*

This header file mangles all symbols exported from the jpeg library.
It is included in all files while building the jpeg library.  Due to
namespace pollution, no jpeg headers should be included in .h files in
itk.

The following command was used to obtain the symbol list:

nm libitkjpeg.a |grep " [TRD] "

nm libitkjpeg.a |grep " [TRD] " | awk '{ print "#define "$3" vxl_jpeg_"$3 }'

*/

#define jcopy_block_row              vxl_jpeg_jcopy_block_row
#define jcopy_sample_rows            vxl_jpeg_jcopy_sample_rows
#define jdiv_round_up                vxl_jpeg_jdiv_round_up
#define jinit_1pass_quantizer        vxl_jpeg_jinit_1pass_quantizer
#define jinit_2pass_quantizer        vxl_jpeg_jinit_2pass_quantizer
#define jinit_c_coef_controller      vxl_jpeg_jinit_c_coef_controller
#define jinit_c_main_controller      vxl_jpeg_jinit_c_main_controller
#define jinit_c_master_control       vxl_jpeg_jinit_c_master_control
#define jinit_c_prep_controller      vxl_jpeg_jinit_c_prep_controller
#define jinit_color_converter        vxl_jpeg_jinit_color_converter
#define jinit_color_deconverter      vxl_jpeg_jinit_color_deconverter
#define jinit_compress_master        vxl_jpeg_jinit_compress_master
#define jinit_d_coef_controller      vxl_jpeg_jinit_d_coef_controller
#define jinit_d_main_controller      vxl_jpeg_jinit_d_main_controller
#define jinit_d_post_controller      vxl_jpeg_jinit_d_post_controller
#define jinit_downsampler            vxl_jpeg_jinit_downsampler
#define jinit_forward_dct            vxl_jpeg_jinit_forward_dct
#define jinit_huff_decoder           vxl_jpeg_jinit_huff_decoder
#define jinit_huff_encoder           vxl_jpeg_jinit_huff_encoder
#define jinit_input_controller       vxl_jpeg_jinit_input_controller
#define jinit_inverse_dct            vxl_jpeg_jinit_inverse_dct
#define jinit_marker_reader          vxl_jpeg_jinit_marker_reader
#define jinit_marker_writer          vxl_jpeg_jinit_marker_writer
#define jinit_master_decompress      vxl_jpeg_jinit_master_decompress
#define jinit_memory_mgr             vxl_jpeg_jinit_memory_mgr
#define jinit_merged_upsampler       vxl_jpeg_jinit_merged_upsampler
#define jinit_upsampler              vxl_jpeg_jinit_upsampler
#define jpeg_CreateCompress          vxl_jpeg_jpeg_CreateCompress
#define jpeg_CreateDecompress        vxl_jpeg_jpeg_CreateDecompress
#define jpeg_abort                   vxl_jpeg_jpeg_abort
#define jpeg_abort_compress          vxl_jpeg_jpeg_abort_compress
#define jpeg_abort_decompress        vxl_jpeg_jpeg_abort_decompress
#define jpeg_add_quant_table         vxl_jpeg_jpeg_add_quant_table
#define jpeg_alloc_huff_table        vxl_jpeg_jpeg_alloc_huff_table
#define jpeg_alloc_quant_table       vxl_jpeg_jpeg_alloc_quant_table
#define jpeg_calc_jpeg_dimensions    vxl_jpeg_jpeg_calc_jpeg_dimensions
#define jpeg_calc_output_dimensions  vxl_jpeg_jpeg_calc_output_dimensions
#define jpeg_consume_input           vxl_jpeg_jpeg_consume_input
#define jpeg_copy_critical_parametersvxl_jpeg_jpeg_copy_critical_parameters
#define jpeg_core_output_dimensions  vxl_jpeg_jpeg_core_output_dimensions
#define jpeg_default_colorspace      vxl_jpeg_jpeg_default_colorspace
#define jpeg_default_qtables         vxl_jpeg_jpeg_default_qtables
#define jpeg_destroy                 vxl_jpeg_jpeg_destroy
#define jpeg_destroy_compress        vxl_jpeg_jpeg_destroy_compress
#define jpeg_destroy_decompress      vxl_jpeg_jpeg_destroy_decompress
#define jpeg_fdct_10x10              vxl_jpeg_jpeg_fdct_10x10
#define jpeg_fdct_10x5               vxl_jpeg_jpeg_fdct_10x5
#define jpeg_fdct_11x11              vxl_jpeg_jpeg_fdct_11x11
#define jpeg_fdct_12x12              vxl_jpeg_jpeg_fdct_12x12
#define jpeg_fdct_12x6               vxl_jpeg_jpeg_fdct_12x6
#define jpeg_fdct_13x13              vxl_jpeg_jpeg_fdct_13x13
#define jpeg_fdct_14x14              vxl_jpeg_jpeg_fdct_14x14
#define jpeg_fdct_14x7               vxl_jpeg_jpeg_fdct_14x7
#define jpeg_fdct_15x15              vxl_jpeg_jpeg_fdct_15x15
#define jpeg_fdct_16x16              vxl_jpeg_jpeg_fdct_16x16
#define jpeg_fdct_16x8               vxl_jpeg_jpeg_fdct_16x8
#define jpeg_fdct_1x1                vxl_jpeg_jpeg_fdct_1x1
#define jpeg_fdct_1x2                vxl_jpeg_jpeg_fdct_1x2
#define jpeg_fdct_2x1                vxl_jpeg_jpeg_fdct_2x1
#define jpeg_fdct_2x2                vxl_jpeg_jpeg_fdct_2x2
#define jpeg_fdct_2x4                vxl_jpeg_jpeg_fdct_2x4
#define jpeg_fdct_3x3                vxl_jpeg_jpeg_fdct_3x3
#define jpeg_fdct_3x6                vxl_jpeg_jpeg_fdct_3x6
#define jpeg_fdct_4x2                vxl_jpeg_jpeg_fdct_4x2
#define jpeg_fdct_4x4                vxl_jpeg_jpeg_fdct_4x4
#define jpeg_fdct_4x8                vxl_jpeg_jpeg_fdct_4x8
#define jpeg_fdct_5x10               vxl_jpeg_jpeg_fdct_5x10
#define jpeg_fdct_5x5                vxl_jpeg_jpeg_fdct_5x5
#define jpeg_fdct_6x12               vxl_jpeg_jpeg_fdct_6x12
#define jpeg_fdct_6x3                vxl_jpeg_jpeg_fdct_6x3
#define jpeg_fdct_6x6                vxl_jpeg_jpeg_fdct_6x6
#define jpeg_fdct_7x14               vxl_jpeg_jpeg_fdct_7x14
#define jpeg_fdct_7x7                vxl_jpeg_jpeg_fdct_7x7
#define jpeg_fdct_8x16               vxl_jpeg_jpeg_fdct_8x16
#define jpeg_fdct_8x4                vxl_jpeg_jpeg_fdct_8x4
#define jpeg_fdct_9x9                vxl_jpeg_jpeg_fdct_9x9
#define jpeg_fdct_float              vxl_jpeg_jpeg_fdct_float
#define jpeg_fdct_ifast              vxl_jpeg_jpeg_fdct_ifast
#define jpeg_fdct_islow              vxl_jpeg_jpeg_fdct_islow
#define jpeg_finish_compress         vxl_jpeg_jpeg_finish_compress
#define jpeg_finish_decompress       vxl_jpeg_jpeg_finish_decompress
#define jpeg_finish_output           vxl_jpeg_jpeg_finish_output
#define jpeg_free_large              vxl_jpeg_jpeg_free_large
#define jpeg_free_small              vxl_jpeg_jpeg_free_small
#define jpeg_get_large               vxl_jpeg_jpeg_get_large
#define jpeg_get_small               vxl_jpeg_jpeg_get_small
#define jpeg_has_multiple_scans      vxl_jpeg_jpeg_has_multiple_scans
#define jpeg_idct_10x10              vxl_jpeg_jpeg_idct_10x10
#define jpeg_idct_10x5               vxl_jpeg_jpeg_idct_10x5
#define jpeg_idct_11x11              vxl_jpeg_jpeg_idct_11x11
#define jpeg_idct_12x12              vxl_jpeg_jpeg_idct_12x12
#define jpeg_idct_12x6               vxl_jpeg_jpeg_idct_12x6
#define jpeg_idct_13x13              vxl_jpeg_jpeg_idct_13x13
#define jpeg_idct_14x14              vxl_jpeg_jpeg_idct_14x14
#define jpeg_idct_14x7               vxl_jpeg_jpeg_idct_14x7
#define jpeg_idct_15x15              vxl_jpeg_jpeg_idct_15x15
#define jpeg_idct_16x16              vxl_jpeg_jpeg_idct_16x16
#define jpeg_idct_16x8               vxl_jpeg_jpeg_idct_16x8
#define jpeg_idct_1x1                vxl_jpeg_jpeg_idct_1x1
#define jpeg_idct_1x2                vxl_jpeg_jpeg_idct_1x2
#define jpeg_idct_2x1                vxl_jpeg_jpeg_idct_2x1
#define jpeg_idct_2x2                vxl_jpeg_jpeg_idct_2x2
#define jpeg_idct_2x4                vxl_jpeg_jpeg_idct_2x4
#define jpeg_idct_3x3                vxl_jpeg_jpeg_idct_3x3
#define jpeg_idct_3x6                vxl_jpeg_jpeg_idct_3x6
#define jpeg_idct_4x2                vxl_jpeg_jpeg_idct_4x2
#define jpeg_idct_4x4                vxl_jpeg_jpeg_idct_4x4
#define jpeg_idct_4x8                vxl_jpeg_jpeg_idct_4x8
#define jpeg_idct_5x10               vxl_jpeg_jpeg_idct_5x10
#define jpeg_idct_5x5                vxl_jpeg_jpeg_idct_5x5
#define jpeg_idct_6x12               vxl_jpeg_jpeg_idct_6x12
#define jpeg_idct_6x3                vxl_jpeg_jpeg_idct_6x3
#define jpeg_idct_6x6                vxl_jpeg_jpeg_idct_6x6
#define jpeg_idct_7x14               vxl_jpeg_jpeg_idct_7x14
#define jpeg_idct_7x7                vxl_jpeg_jpeg_idct_7x7
#define jpeg_idct_8x16               vxl_jpeg_jpeg_idct_8x16
#define jpeg_idct_8x4                vxl_jpeg_jpeg_idct_8x4
#define jpeg_idct_9x9                vxl_jpeg_jpeg_idct_9x9
#define jpeg_idct_float              vxl_jpeg_jpeg_idct_float
#define jpeg_idct_ifast              vxl_jpeg_jpeg_idct_ifast
#define jpeg_idct_islow              vxl_jpeg_jpeg_idct_islow
#define jpeg_input_complete          vxl_jpeg_jpeg_input_complete
#define jpeg_mem_available           vxl_jpeg_jpeg_mem_available
#define jpeg_mem_dest                vxl_jpeg_jpeg_mem_dest
#define jpeg_mem_init                vxl_jpeg_jpeg_mem_init
#define jpeg_mem_src                 vxl_jpeg_jpeg_mem_src
#define jpeg_mem_term                vxl_jpeg_jpeg_mem_term
#define jpeg_natural_order           vxl_jpeg_jpeg_natural_order
#define jpeg_natural_order2          vxl_jpeg_jpeg_natural_order2
#define jpeg_natural_order3          vxl_jpeg_jpeg_natural_order3
#define jpeg_natural_order4          vxl_jpeg_jpeg_natural_order4
#define jpeg_natural_order5          vxl_jpeg_jpeg_natural_order5
#define jpeg_natural_order6          vxl_jpeg_jpeg_natural_order6
#define jpeg_natural_order7          vxl_jpeg_jpeg_natural_order7
#define jpeg_new_colormap            vxl_jpeg_jpeg_new_colormap
#define jpeg_open_backing_store      vxl_jpeg_jpeg_open_backing_store
#define jpeg_quality_scaling         vxl_jpeg_jpeg_quality_scaling
#define jpeg_read_coefficients       vxl_jpeg_jpeg_read_coefficients
#define jpeg_read_header             vxl_jpeg_jpeg_read_header
#define jpeg_read_raw_data           vxl_jpeg_jpeg_read_raw_data
#define jpeg_read_scanlines          vxl_jpeg_jpeg_read_scanlines
#define jpeg_resync_to_restart       vxl_jpeg_jpeg_resync_to_restart
#define jpeg_save_markers            vxl_jpeg_jpeg_save_markers
#define jpeg_set_colorspace          vxl_jpeg_jpeg_set_colorspace
#define jpeg_set_defaults            vxl_jpeg_jpeg_set_defaults
#define jpeg_set_linear_quality      vxl_jpeg_jpeg_set_linear_quality
#define jpeg_set_marker_processor    vxl_jpeg_jpeg_set_marker_processor
#define jpeg_set_quality             vxl_jpeg_jpeg_set_quality
#define jpeg_simple_progression      vxl_jpeg_jpeg_simple_progression
#define jpeg_start_compress          vxl_jpeg_jpeg_start_compress
#define jpeg_start_decompress        vxl_jpeg_jpeg_start_decompress
#define jpeg_start_output            vxl_jpeg_jpeg_start_output
#define jpeg_std_error               vxl_jpeg_jpeg_std_error
#define jpeg_std_message_table       vxl_jpeg_jpeg_std_message_table
#define jpeg_stdio_dest              vxl_jpeg_jpeg_stdio_dest
#define jpeg_stdio_src               vxl_jpeg_jpeg_stdio_src
#define jpeg_suppress_tables         vxl_jpeg_jpeg_suppress_tables
#define jpeg_write_coefficients      vxl_jpeg_jpeg_write_coefficients
#define jpeg_write_m_byte            vxl_jpeg_jpeg_write_m_byte
#define jpeg_write_m_header          vxl_jpeg_jpeg_write_m_header
#define jpeg_write_marker            vxl_jpeg_jpeg_write_marker
#define jpeg_write_raw_data          vxl_jpeg_jpeg_write_raw_data
#define jpeg_write_scanlines         vxl_jpeg_jpeg_write_scanlines
#define jpeg_write_tables            vxl_jpeg_jpeg_write_tables
#define jround_up                    vxl_jpeg_jround_up
#define jzero_far                    vxl_jpeg_jzero_far

#endif
