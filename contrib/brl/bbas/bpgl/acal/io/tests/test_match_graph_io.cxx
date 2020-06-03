// acal/io/tests/test_match_graph_io.cxx
#include "test_generic_io.h"
#include <acal/io/acal_io_match_graph.h>


// helper: create affine cameras with defaults.
// This function uses the vpgl_affine_camera constructor:
// vpgl_affine_camera<T>(vgl_vector_3d<T> ray, vgl_vector_3d<T> up,
//                       vgl_point_3d<T> stare_pt, T u0, T v0, T su, T sv)
template <class T>
vpgl_affine_camera<T>
make_affine_camera(T rayx, T rayy, T rayz,
                   T upx = T(0), T upy = T(0), T upz = T(1),
                   T ptx = T(0), T pty = T(0), T ptz = T(0),
                   T u0 = T(0), T v0 = T(0), T su = T(1), T sv = T(1))
{
  return vpgl_affine_camera<T>(vgl_vector_3d<T>(rayx, rayy, rayz),
                               vgl_vector_3d<T>(upx, upy, upz),
                               vgl_point_3d<T>(ptx, pty, ptz),
                               u0, v0, su, sv);
}


// helper: update incidence_matrix with points & pairs of cams
void
update_incidence_matrix(
    std::map<size_t, std::map<size_t, std::vector<acal_match_pair> > >& incidence_matrix,
    std::vector<vgl_point_3d<double> > const& pts,
    std::vector<std::pair<size_t, size_t> > const& pairs,
    std::map<size_t, vpgl_affine_camera<double> > const& cams)
{
  for (auto pair : pairs) {
    size_t i = pair.first, j = pair.second;

    std::vector<acal_match_pair> vec;
    for (size_t k = 0; k < pts.size(); k++) {
      auto pti = cams.at(i).project(pts[k]);
      auto ptj = cams.at(j).project(pts[k]);
      acal_match_pair mp(acal_corr(k, pti), acal_corr(k, ptj));
      vec.push_back(mp);
    }

    incidence_matrix[i][j] = vec;
  }
}


static void
test_match_graph_io()
{
  // match_params io test
  match_params mp(10, 10, 0.5f, 0.5f);
  test_generic_io(mp, "match_params");

  // acal_match_graph io test

  // TODO: DRY test_match_graph

  // -----
  // Test graph containing two connected components
  //   component "A" = 0-1-2-3
  //   component "B" = 4-5-6
  // For each image/camera, we project 3d points into the 2d image space
  // to serve as feature correspondences. We then process the correspondences
  // into the acal_match_graph to confirm expected operation
  // -----

  // images (not actual paths)
  std::map<size_t, std::string> image_paths = {
      {0, "image0.tif"},
      {1, "image1.tif"},
      {2, "image2.tif"},
      {3, "image3.tif"},
      {4, "image4.tif"},
      {5, "image5.tif"},
      {6, "image6.tif"},
  };

  // cameras
  std::map<size_t, vpgl_affine_camera<double> > cams = {

      // camsA
      {0, make_affine_camera<double>( 1,  0, 0)},
      {1, make_affine_camera<double>( 0,  1, 0)},
      {2, make_affine_camera<double>(-1,  0, 0)},
      {3, make_affine_camera<double>( 0, -1, 0)},

      // camsB
      {4, make_affine_camera<double>( 1,  1, 0)},
      {5, make_affine_camera<double>(-1,  1, 0)},
      {6, make_affine_camera<double>(-1, -1, 0)},
  };

  // incidence matrix
  std::map<size_t, std::map<size_t, std::vector<acal_match_pair> > > incidence_matrix;

  // connected component "A"
  std::vector<vgl_point_3d<double> > ptsA = {
    vgl_point_3d<double>(1,0,0),
    vgl_point_3d<double>(0,1,0),
    vgl_point_3d<double>(0,0,1),
    vgl_point_3d<double>(0,0,-1),
  };

  std::vector<std::pair<size_t, size_t> > pairsA = {{0,1},{1,2},{2,3},{3,0}};
  update_incidence_matrix(incidence_matrix, ptsA, pairsA, cams);

  // connected component "B"
  std::vector<vgl_point_3d<double> > ptsB = {
    vgl_point_3d<double>(2,0,0),
    vgl_point_3d<double>(0,2,0),
    vgl_point_3d<double>(0,0,2),
  };

  std::vector<std::pair<size_t, size_t> > pairsB = {{4,5},{5,6},{6,4}};
  update_incidence_matrix(incidence_matrix, ptsB, pairsB, cams);


  acal_match_graph mg;
  mg.image_paths_ = image_paths;
  mg.all_acams_ = cams;
  mg.load_incidence_matrix(incidence_matrix);
  mg.find_connected_components();
  mg.compute_focus_tracks();
  mg.compute_match_trees();
  mg.validate_match_trees_and_set_metric();

  test_generic_io(mg, "match_graph");
}

TESTMAIN(test_match_graph_io);
