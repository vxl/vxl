#include <iostream>
#include <testlib/testlib_test.h>
#include <imesh/algo/imesh_kd_tree.h>
#include <imesh/imesh_mesh.h>
#include <imesh/algo/imesh_intersect.h>
#include <imesh/imesh_operations.h>
#include "test_share.h"
#include <imesh/algo/imesh_transform.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>

void test_closest_point(const imesh_mesh& mesh, const std::vector<vgl_point_3d<double> >& pts)
{
  std::unique_ptr<imesh_kd_tree_node> kd_tree = imesh_build_kd_tree(mesh);

  bool same_tri = true;
  bool same_pt = true;
  for (const auto & pt : pts) {
    vgl_point_3d<double> cp1, cp2;
    unsigned int ind1 = imesh_kd_tree_closest_point(pt,mesh,kd_tree,cp1);
    unsigned int ind2 = imesh_closest_point(pt,mesh,cp2);
    if (ind1 != ind2) {
      if (std::abs((cp1-pt).length() - (cp2-pt).length()) > 1e-8) {
        same_tri = false;
        std::cout << "kd tree closest point to "<<pt<<" at index "<< ind1<<std::endl
                 << "exhaustive closest point to "<<pt<<" at index "<< ind2<<std::endl;
      }
    }
    if ((cp1-cp2).length() > 1e-8)
    {
      if (std::abs((cp1-pt).length() - (cp2-pt).length()) > 1e-8) {
        same_pt = false;
        std::cout << "kd tree closest point to "<<pt<<" is "<<cp1<<std::endl
                 << "exhaustive closest point to "<<pt<<" is "<<cp2<<std::endl;
      }
    }
  }
  TEST("Same closest triangle",same_tri,true);
  TEST("Same closest point",same_pt,true);
}


static void test_kd_tree()
{
  std::vector<vgl_point_3d<double> > pts;
  pts.emplace_back(0,0,0);
  pts.emplace_back(1.5,0,0.5);
  pts.emplace_back(0.1,.2,0);
  pts.emplace_back(3,-3,3);
  imesh_mesh cube;
  make_cube(cube);
  imesh_transform_inplace(cube, vgl_rotation_3d<double>(0,.1,vnl_math::pi_over_4));
  imesh_quad_subdivide(cube);
  imesh_triangulate(cube);

  test_closest_point(cube,pts);

  std::vector<imesh_kd_tree_queue_entry> dists;
  std::unique_ptr<imesh_kd_tree_node> kd_tree = imesh_build_kd_tree(cube);
  vgl_point_3d<double> cp;
  imesh_kd_tree_closest_point(pts[3],cube,kd_tree,cp,&dists);
  unsigned int leaf_count = 0;
  std::vector<imesh_kd_tree_node*> internals;
  for (auto & dist : dists) {
    if (dist.node_->is_leaf())
      ++leaf_count;
    else
      internals.push_back(dist.node_);
  }
  for (unsigned int i=0; i<internals.size(); ++i) {
    if (internals[i]->is_leaf())
      ++leaf_count;
    else {
      internals.push_back(internals[i]->left_.get());
      internals.push_back(internals[i]->right_.get());
    }
  }
  TEST("Remainder covers all nodes",leaf_count,cube.num_faces());
}

TESTMAIN(test_kd_tree);
