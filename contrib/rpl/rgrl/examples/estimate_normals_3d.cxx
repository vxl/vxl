// BeginLatex
//
// The program reads in a set of 3D points and approximate the normal
// directions at these points.
//
// At each point, a set of neighboring points (size=5) are extraced,
// and a plane is fit by minimizing the orthogonal distances of the
// point set to the plane.
//
// The in_file format:
//
//    total_number_of_point
//    x y z
//    x y z
//    ...
//
// The out_file format:
//    total_number_of_point
//    x y z nx ny nz
//    x y z nx ny nz
//    ...
//
//
// EndLatex

#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/algo/vnl_scatter_3x3.h>

#include <rrel/rrel_muset_obj.h>
#include <rrel/rrel_tukey_obj.h>
#include <rsdl/rsdl_kd_tree.h>

typedef vnl_vector_fixed<double,3>       vector_3d;

vector_3d
approximate_normal(vcl_vector< vector_3d > const & points)
{
  // Compute the center of the points
  //
  vector_3d center(0.0, 0.0, 0.0);
  int num_pts = points.size();
  for ( int i=0; i<num_pts; i++) {
    center += points[i];
  }
  center /= num_pts;

  // Compute the scatter matrix
  //
  vnl_scatter_3x3<double> scatter_matrix;

  for ( int i=0; i<num_pts;i++) {
    vector_3d shifted_pt = points[i] - center;
    scatter_matrix.add_outer_product(shifted_pt);
  }

  // Using Lagrange Multiplier, the normal of the plane corresponds to
  // the eigenvector of the least eigenvalue
  //
  vector_3d normal = scatter_matrix.minimum_eigenvector();

  return normal;
}

int
main( int argc, char* argv[] )
{
  if ( argc < 3 ) {
    vcl_cerr << "Missing Parameters\n"
             << "Usage: " << argv[0]
             << " InputFeatureFile OutputFeatureFile\n";
    return 1;
  }
  const char*  in_filename=   argv[1];
  const char*  out_filename = argv[2];

  vcl_vector< vector_3d > points;
  vcl_ifstream istr( in_filename );

  if ( !istr ) {
    vcl_cerr<<"ERROR: Cannot open "<<in_filename<<vcl_endl;
    return 0;
  }

  vector_3d location;

  int total;
  istr >> total;
  for (int i = 0; i<total; ++i) {
    istr >> location[0] >> location[1] >> location[2];
    points.push_back( location );
  }
  istr.close();


  // Store all the points in the kd-tree
  //
  const unsigned int nc = 3, na = 0;
  vcl_vector<rsdl_point> search_pts(total);
  for (int i = 0; i<total; ++i) {
    search_pts[i].resize( nc, na );
    search_pts[i].set_cartesian(points[i]);
  }
  rsdl_kd_tree kd_tree( search_pts );


  // For each data point, generate the normal to the plane. It first
  // collects a set of nearby points from the kd_tree. It then fits
  // the best plane using the set of points. The normal to the plane
  // is taken as the normal to the data point.
  //
  vcl_ofstream ostr( out_filename );
  vcl_vector< vector_3d > near_neighbors;
  vcl_vector<rsdl_point> near_neighbor_pts;
  vcl_vector<int> near_neighbor_indices;
  ostr<<total<<vcl_endl;
  int num_nb = 4;
  for (int i = 0; i<total; ++i) {
    rsdl_point query_pt( 3, 0);
    query_pt.set_cartesian(points[i]);
    kd_tree.n_nearest(query_pt, num_nb, near_neighbor_pts, near_neighbor_indices);
    near_neighbors.clear();
    for (int j = 0; j<num_nb; ++j)
      near_neighbors.push_back(points[near_neighbor_indices[j]]);

    vector_3d normal = approximate_normal(near_neighbors);
    ostr<<points[i]<<' '<<normal<<vcl_endl;
  }
  ostr.close();

  return 0;
}
