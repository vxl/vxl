#include <mbl/mbl_cluster_tree.txx>
#include <vnl/vnl_vector.h>
#include <vcl_cmath.h>

#include <mbl/mbl_vector_distance.h>

MBL_CLUSTER_TREE_INSTANTIATE(vnl_vector<double>,
                         mbl_vector_distance<double>);
