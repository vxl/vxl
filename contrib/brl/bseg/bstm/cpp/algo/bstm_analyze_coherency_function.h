#ifndef bstm_analyze_coherency_function_h_
#define bstm_analyze_coherency_function_h_
//:
// \file

#include <bstm/bstm_time_block.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_data_traits.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/basic/boxm2_array_3d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iostream.h>
#include <bstm/io/bstm_cache.h>

#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>
#include <vcl_map.h>
#include <vcl_fstream.h>

class bstm_analyze_coherency_function
{
 public:

   typedef unsigned char uchar;
   typedef unsigned short ushort;
   typedef vnl_vector_fixed<uchar, 16> uchar16;
   typedef vnl_vector_fixed<uchar, 8> uchar8;
   typedef vnl_vector_fixed<ushort, 4> ushort4;

   //: "default" constructor does all the work
   bstm_analyze_coherency_function(bstm_block* blk, bstm_block_metadata blk_data, bstm_time_block* blk_t, bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::datatype *app,
                                       bstm_data_traits<BSTM_ALPHA>::datatype * alpha, double init_local_time, double end_local_time, vgl_box_3d<double> box,
                                       float p_threshold, vcl_ofstream & output_file);

};


#endif // bstm_analyze_coherency_function_h_
