#include "testlib/testlib_test.h"

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vpgl/vpgl_affine_camera.h>

#include <acal/acal_match_graph.h>
#include <acal/acal_match_utils.h>


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

// main test function
static void
test_match_graph()
{
  bool success = true;

  // equality tests
  match_params mp1(10, 10, 0.5f, 0.5f), mp2(10, 10, 0.5f, 0.5f), mp3;
  TEST("match_params equality", mp1, mp2);
  TEST("match_params inequality", mp1 != mp3, true);


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

  // DEBUG: print incidence_matrix
  std::cout << "\nIncidence matrix:\n";
  for (auto const& row : incidence_matrix) {
    for (auto const& item : row.second) {
      std::cout << "image pair " << row.first << "/" << item.first << "\n";
      for (auto const& pair : item.second) {
        std::cout << "  " << pair << "\n";
      }
    }
  }
  std::cout << std::endl;

  // initialize match graph
  acal_match_graph match_graph;
  match_graph.image_paths_ = image_paths;
  match_graph.all_acams_ = cams;
  success = match_graph.load_incidence_matrix(incidence_matrix);
  TEST("acal_match_graph::load_incidence_matrix", success, true);

  // connected components
  match_graph.find_connected_components();
  std::cout << "\nacal_match_graph::find_connected_components complete" << std::endl;
  //  match_graph.print_connected_components();

  auto components = match_graph.conn_comps_;
  TEST("acal_match_graph 2 connected components", components.size(), 2);
  TEST("acal_match_graph component[0].size == 4", components[0].size(), 4);
  TEST("acal_match_graph component[1].size == 3", components[1].size(), 3);

  // focus tracks
  match_graph.compute_focus_tracks();
  std::cout << "\nacal_match_graph::compute_focus_tracks complete" << std::endl;
  // match_graph.print_focus_tracks();

  auto tracks = match_graph.focus_tracks_;
  TEST("acal_match_graph tracks[0][0].size == 4", tracks[0][0].size(), 4);
  TEST("acal_match_graph tracks[1][4].size == 3", tracks[1][4].size(), 3);

  // match trees
  match_graph.compute_match_trees();
  std::cout << "\nacal_match_graph::compute_match_trees complete" << std::endl;

  match_graph.validate_match_trees_and_set_metric();
  std::cout << "\nacal_match_graph::validate_match_trees_and_set_metric complete" << std::endl;


  // equality test
  acal_match_graph match_graph_copy;
  match_graph_copy.image_paths_ = image_paths;
  match_graph_copy.all_acams_ = cams;
  success = match_graph_copy.load_incidence_matrix(incidence_matrix);
  TEST("acal_match_graph !=", match_graph_copy != match_graph, true);

  match_graph_copy.find_connected_components();
  match_graph_copy.compute_focus_tracks();
  match_graph_copy.compute_match_trees();
  match_graph_copy.validate_match_trees_and_set_metric();

  TEST("acal_match_graph ==", match_graph_copy, match_graph);

}

TESTMAIN(test_match_graph);
