#ifndef boxm_ocl_utils_h_
#define boxm_ocl_utils_h_

#include "bocl/bocl_cl.h"
#include <vcl_string.h>
#include <vcl_cstddef.h>
#include <vnl/vnl_vector_fixed.h>

#include <boct/boct_tree_cell.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/boxm_apm_traits.h>

vcl_size_t RoundUp(int global_size,int group_size);
vcl_string error_to_string(cl_int  status );



void pack_cell_data(boct_tree_cell<short, boxm_sample<BOXM_APM_SIMPLE_GREY> >* cell_ptr, vnl_vector_fixed<float, 16> &data);


void pack_cell_data(boct_tree_cell<short, boxm_sample<BOXM_APM_MOG_GREY> >* cell_ptr, vnl_vector_fixed<float, 16> &data);


void pack_cell_data(boct_tree_cell<short, float> * cell_ptr, vnl_vector_fixed<float, 16> &data);



#endif
