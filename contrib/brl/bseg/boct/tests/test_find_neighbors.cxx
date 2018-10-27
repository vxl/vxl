#include <testlib/testlib_test.h>

#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_intersection.h>

#include <boct/boct_tree.h>
#include <boct/boct_test_util.h>
#include <boct/boct_tree_cell.h>

// This function computes the neighbors in a brute force way by going through all leaf nodes and checking if the codes of the opposite faces are the same.
// This means checking that the X_HIGH of one and X_LOW of other have the same x code and vice versa.
// This function is used in one of the tests.
void brute_force_test_neighbor(boct_tree_cell<short,vgl_point_3d<double> >* cell,
                               std::vector<boct_tree_cell<short,vgl_point_3d<double> >*> leaf_nodes,
                               const boct_face_idx face,
                               int max_level,
                               std::vector<boct_tree_cell<short,vgl_point_3d<double> >*> & neighbors)
{
    double cellsize=(double)(1<<cell->level())/(double)(1<<(max_level-1));

    vgl_box_3d<double> cellbox(cell->code_.get_point(max_level),cellsize,cellsize,cellsize,vgl_box_3d<double>::min_pos);
    for (auto & leaf_node : leaf_nodes)
    {
        double neighborcellsize=(double)(1<<leaf_node->level())/(double)(1<<(max_level-1));
        vgl_box_3d<double> neighborcellbox(leaf_node->code_.get_point(max_level),neighborcellsize,neighborcellsize,neighborcellsize,vgl_box_3d<double>::min_pos);

        vgl_box_3d<double> intersectionbox=vgl_intersection(cellbox,neighborcellbox);
        if (!intersectionbox.is_empty())
        {
            switch (face)
            {
              case NONE:
                break;
              case X_LOW:
                if (intersectionbox.min_y()!=intersectionbox.max_y() && intersectionbox.min_z()!=intersectionbox.max_z())
                    if (intersectionbox.min_x()==intersectionbox.max_x() && intersectionbox.min_x()==cellbox.min_x())
                        neighbors.push_back(leaf_node);
                break;
              case X_HIGH:
                if (intersectionbox.min_y()!=intersectionbox.max_y() && intersectionbox.min_z()!=intersectionbox.max_z())
                    if (intersectionbox.min_x()==intersectionbox.max_x() && intersectionbox.max_x()==cellbox.max_x())
                        neighbors.push_back(leaf_node);
                break;
              case Y_LOW:
                if (intersectionbox.min_x()!=intersectionbox.max_x() && intersectionbox.min_z()!=intersectionbox.max_z())
                    if (intersectionbox.min_y()==intersectionbox.max_y() && intersectionbox.min_y()==cellbox.min_y())
                        neighbors.push_back(leaf_node);
                break;
              case Y_HIGH:
                if (intersectionbox.min_x()!=intersectionbox.max_x() && intersectionbox.min_z()!=intersectionbox.max_z())
                    if (intersectionbox.min_y()==intersectionbox.max_y() && intersectionbox.max_y()==cellbox.max_y())
                        neighbors.push_back(leaf_node);
                break;
              case Z_LOW:
                if (intersectionbox.min_x()!=intersectionbox.max_x() && intersectionbox.min_y()!=intersectionbox.max_y())
                    if (intersectionbox.min_z()==intersectionbox.max_z() && intersectionbox.min_z()==cellbox.min_z())
                        neighbors.push_back(leaf_node);
                break;
              case Z_HIGH:
                if (intersectionbox.min_x()!=intersectionbox.max_x() && intersectionbox.min_y()!=intersectionbox.max_y())
                    if (intersectionbox.min_z()==intersectionbox.max_z() && intersectionbox.max_z()==cellbox.max_z())
                        neighbors.push_back(leaf_node);
                break;
              default:
                break;
            }
        }
    }
}

static void test_find_neighbors()
{
  short nlevels=10;
  auto * block=new boct_tree<short,vgl_point_3d<double> >(nlevels);

  // two layer tree;
  block->split();
  vgl_point_3d<double> p1(0.1,0.1,0.1);
  boct_tree_cell<short,vgl_point_3d<double> >* cell=block->locate_point(p1);

  std::vector<boct_tree_cell<short,vgl_point_3d<double> >*> n;
  cell->find_neighbors(X_HIGH,n,9);

  // ground truth for the code of the neighbor
  boct_loc_code<short> gt_code;
  gt_code.set_code((cell->get_code().x_loc_|1<<(cell->level())),cell->get_code().y_loc_, cell->get_code().z_loc_);

  TEST("Returns the correct  Neighbor for X_HIGH",gt_code.x_loc_,n[0]->get_code().x_loc_);

  vgl_point_3d<double> p_x_low(0.6,0.1,0.1);
  boct_tree_cell<short,vgl_point_3d<double> >* cell_xlow=block->locate_point(p_x_low);

  n.clear();
  cell_xlow->find_neighbors(X_LOW,n,9);
  boct_tree_cell<short,vgl_point_3d<double> >* ncell=nullptr;
  cell_xlow->find_neighbor(X_LOW,ncell,10);

  // ground truth for the code of the neighbor
  boct_loc_code<short> gt_code_x_low;
  gt_code_x_low.set_code((cell_xlow->get_code().x_loc_-(1<<(cell_xlow->level()))),cell->get_code().y_loc_, cell->get_code().z_loc_);

  TEST("Returns the correct  Neighbor for X_LOW",gt_code_x_low.x_loc_,n[0]->get_code().x_loc_);

  // neighbors on a random tree
  auto* tree3=new boct_tree<short,vgl_point_3d<double> >(nlevels);
  create_random_configuration_tree(tree3);
  //tree3->print();
  std::vector<boct_tree_cell<short,vgl_point_3d<double> >*> leaf_nodes=tree3->leaf_cells();
  vgl_point_3d<double> p_z_low(0.6,0.1,0.1);
  boct_tree_cell<short,vgl_point_3d<double> >* cell_zlow=tree3->locate_point(p_z_low);

  n.clear();
  cell_zlow->find_neighbors(Y_HIGH,n,9);

  std::vector<boct_tree_cell<short,vgl_point_3d<double> >*> n_brute_force;

  brute_force_test_neighbor(cell_zlow,leaf_nodes,Y_HIGH,nlevels, n_brute_force);

  TEST("Returns the correct # of Neighbors",n.size(),n_brute_force.size());

  unsigned int cnt=0;
  for (auto & i : n)
  {
    for (auto & j : n_brute_force)
    {
      if (i->code_.x_loc_==j->code_.x_loc_ &&
          i->code_.y_loc_==j->code_.y_loc_ &&
          i->code_.z_loc_==j->code_.z_loc_   )
      ++cnt;
    }
  }
  TEST("Returns the Correct Neighbors",n.size(),cnt);
}

TESTMAIN(test_find_neighbors);
