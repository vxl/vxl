#include<mbl/mbl_data_collector.txx>
#include<vnl/vnl_vector.h>
#include<vsl/vsl_binary_loader.txx>
MBL_DATA_COLLECTOR_INSTANTIATE(vnl_vector<double>);
VSL_BINARY_LOADER_INSTANTIATE(mbl_data_collector<vnl_vector<double> >);
