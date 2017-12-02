#include <mbl/mbl_clusters.hxx>
#include <vnl/vnl_vector.h>
#include <mbl/mbl_vector_distance.h>

MBL_CLUSTERS_INSTANTIATE(vnl_vector<double>,
                         mbl_vector_distance<double>);
