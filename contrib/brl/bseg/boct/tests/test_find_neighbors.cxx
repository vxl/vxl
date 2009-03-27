#include <testlib/testlib_test.h>

#include <boct/boct_tree.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <boct/boct_test_util.h>

//: this function computes the neighbors in brute force way by going through all leaf nodes and checking if the codes of the opposit faces is the same. 
//: This means that checking if the X_HIGH of one and X_LOW of other have the same x code and viceversa.
void brute_force_test_neighbor(boct_tree_cell_base_sptr cell,
                               vcl_vector<boct_tree_cell_base_sptr> leaf_nodes, 
                               boct_tree_cell_base::FACE_IDX face,
                               int max_level, 
                               vcl_vector<boct_tree_cell_base_sptr> & neighbors)
{
    double cellsize=(double)(1<<cell->level())/(double)(1<<(max_level-1));

    vgl_box_3d<double> cellbox(cell->code_.get_point(max_level),cellsize,cellsize,cellsize,vgl_box_3d<double>::min_pos);
    for(unsigned i=0;i<leaf_nodes.size();i++)
    {
        double neighborcellsize=(double)(1<<leaf_nodes[i]->level())/(double)(1<<(max_level-1));
        vgl_box_3d<double> neighborcellbox(leaf_nodes[i]->code_.get_point(max_level),neighborcellsize,neighborcellsize,neighborcellsize,vgl_box_3d<double>::min_pos);

        vgl_box_3d<double> intersectionbox=vgl_intersection(cellbox,neighborcellbox);
        if(!intersectionbox.is_empty())
        {

            switch(face)
            {
            case boct_tree_cell_base::NONE:

                break;

            case boct_tree_cell_base::X_LOW:
                {
                    if(intersectionbox.min_y()!=intersectionbox.max_y() && intersectionbox.min_z()!=intersectionbox.max_z())
                    if(intersectionbox.min_x()==intersectionbox.max_x()&& intersectionbox.min_x()==cellbox.min_x())
                        neighbors.push_back(leaf_nodes[i]);
                    break;
                }
            case boct_tree_cell_base::X_HIGH:
                {
                    if(intersectionbox.min_y()!=intersectionbox.max_y() && intersectionbox.min_z()!=intersectionbox.max_z())
                        if(intersectionbox.min_x()==intersectionbox.max_x()==cellbox.max_x())
                            neighbors.push_back(leaf_nodes[i]);
                    break;
                }
            case boct_tree_cell_base::Y_LOW:
                {
                    if(intersectionbox.min_x()!=intersectionbox.max_x() && intersectionbox.min_z()!=intersectionbox.max_z())
                        if(intersectionbox.min_y()==intersectionbox.max_y()==cellbox.min_y())
                            neighbors.push_back(leaf_nodes[i]);
                    break;
                }
            case boct_tree_cell_base::Y_HIGH:
                {
                    if(intersectionbox.min_x()!=intersectionbox.max_x() && intersectionbox.min_z()!=intersectionbox.max_z())
                        if(intersectionbox.min_y()==intersectionbox.max_y()==cellbox.max_y())
                            neighbors.push_back(leaf_nodes[i]);
                    break;
                }
            case boct_tree_cell_base::Z_LOW:
                {
                    if(intersectionbox.min_x()!=intersectionbox.max_x() && intersectionbox.min_y()!=intersectionbox.max_y())
                        if(intersectionbox.min_z()==intersectionbox.max_z()==cellbox.min_z())
                            neighbors.push_back(leaf_nodes[i]);
                    break;
                }
            case boct_tree_cell_base::Z_HIGH:
                {
                    if(intersectionbox.min_x()!=intersectionbox.max_x() && intersectionbox.min_y()!=intersectionbox.max_y())
                        if(intersectionbox.min_z()==intersectionbox.max_z()==cellbox.max_z())
                            neighbors.push_back(leaf_nodes[i]);
                    break;
                }
            default:
                break;

            }
        }

    }

}

MAIN( test_find_neighbors )
{
  START ("Find Neighbors");
  short nlevels=10;
  boct_tree<vgl_point_3d<double> > * block=new boct_tree<vgl_point_3d<double> >(nlevels);
  
  //: two layer tree;
  block->split();
  vgl_point_3d<double> p1(0.1,0.1,0.1);
  boct_tree_cell_base_sptr cell=block->locate_point(p1);

  vcl_vector<boct_tree_cell_base_sptr> n;
  cell->find_neighbors(boct_tree_cell_base::X_HIGH,n,10);

  //: ground truth for the code of the neighbor 
  boct_loc_code gt_code;
  gt_code.set_code((cell->get_code().x_loc_|1<<(cell->level())),cell->get_code().y_loc_, cell->get_code().z_loc_);
  
  TEST("Returns the correct  Neighbor for X_HIGH",gt_code.x_loc_,n[0]->get_code().x_loc_);

  vgl_point_3d<double> p_x_low(0.6,0.1,0.1);
  boct_tree_cell_base_sptr cell_xlow=block->locate_point(p_x_low);

  n.clear();
  cell_xlow->find_neighbors(boct_tree_cell_base::X_LOW,n,10);

  //: ground truth for the code of the neighbor 
  boct_loc_code gt_code_x_low;
  gt_code_x_low.set_code((cell_xlow->get_code().x_loc_-(1<<(cell_xlow->level()))),cell->get_code().y_loc_, cell->get_code().z_loc_);



  TEST("Returns the correct  Neighbor for X_LOW",gt_code_x_low.x_loc_,n[0]->get_code().x_loc_);


  //: neighbors on a random tree
  boct_tree<vgl_point_3d<double> >* tree3=new boct_tree<vgl_point_3d<double> >(nlevels);
  create_random_configuration_tree(tree3);
  tree3->print();
  vcl_vector<boct_tree_cell_base_sptr> leaf_nodes=tree3->leaf_cells();
  vgl_point_3d<double> p_z_low(0.6,0.1,0.1);
  boct_tree_cell_base_sptr cell_zlow=tree3->locate_point(p_z_low);


  n.clear();
  cell_zlow->find_neighbors(boct_tree_cell_base::X_LOW,n,10);

  vcl_vector<boct_tree_cell_base_sptr> n_brute_force;

  brute_force_test_neighbor(cell_zlow,leaf_nodes,boct_tree_cell_base::X_LOW,nlevels, n_brute_force);


  TEST("Returns the correct # of Neighbors",n.size(),n_brute_force.size());
  int cnt=0;
  for(unsigned i=0;i<n.size();i++)
  {
    for(unsigned j=0;j<n_brute_force.size();j++)
    {
        if(n[i]->code_.x_loc_==n_brute_force[j]->code_.x_loc_&&
           n[i]->code_.y_loc_==n_brute_force[j]->code_.y_loc_&&
           n[i]->code_.z_loc_==n_brute_force[j]->code_.z_loc_)
        cnt++;
    }
  }
  TEST("Returns the Correct Neighbors",n.size(),cnt);




  SUMMARY();

  
}