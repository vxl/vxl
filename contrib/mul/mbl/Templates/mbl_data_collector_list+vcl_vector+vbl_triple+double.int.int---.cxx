#include <vbl/vbl_triple.h>
#include <vsl/vsl_vector_io.txx>
#include <vbl/io/vbl_io_triple.txx>
#include <mbl/mbl_data_collector_list.txx>

typedef vcl_vector<vbl_triple<double,int,int> > vec_triple_dii;
MBL_DATA_COLLECTOR_LIST_INSTANTIATE( vec_triple_dii );
