#include <vcl_utility.h>
#include <vsl/vsl_vector_io.txx>
#include <vsl/vsl_pair_io.txx>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <bmrf/bmrf_node_sptr.h>

typedef vcl_pair<double,bmrf_node_sptr> pair_id;
VSL_VECTOR_IO_INSTANTIATE(pair_id);
