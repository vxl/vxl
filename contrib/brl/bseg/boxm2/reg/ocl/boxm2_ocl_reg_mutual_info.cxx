#include "boxm2_ocl_reg_mutual_info.h"
//:
// \file
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <boxm2/boxm2_block_metadata.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boct/boct_bit_tree.h>
#include <brip/brip_mutual_info.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_kernel.h>

#include <vcl_algorithm.h>

typedef vnl_vector_fixed<unsigned char,16> uchar16;

//: compute mutual information for a given transformation
