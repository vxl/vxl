#include <vbl/vbl_triple.h>
#include <vsl/vsl_vector_io.h>
#include <mbl/mbl_data_wrapper.txx>

typedef vcl_vector<vbl_triple<double,int,int> > vec_triple_dii;
MBL_DATA_WRAPPER_INSTANTIATE( vec_triple_dii );
