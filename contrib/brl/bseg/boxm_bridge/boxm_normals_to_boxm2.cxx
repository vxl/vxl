#include <queue>
#include <iostream>
#include "boxm_to_boxm2.h"
//:
// \file
//
//  boxm_normals_to_boxm2.cxx
//
// \author
//  Created by David Borton on 11/28/11.
//  Copyright (c) 2011 Brown University. All rights reserved.
//

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//executable args
#include <vul/vul_arg.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>

//boct files
#include <boct/boct_tree.h>
#include <boct/boct_bit_tree2.h>

//boxm2 files
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/io/boxm2_sio_mgr.h>
#include <boxm2/io/boxm2_lru_cache.h>

// boxm files
#include <boxm/boxm_scene.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/boxm_sample.h>


//: extracts the tree cell's data into an array format for boxm2 representation
template <class T_loc, class T_data>
void convert_data(boct_tree<T_loc,T_data>* tree,
                  boct_tree_cell<T_loc,T_data>* tree_cell,
                  boct_tree_cell<T_loc, bool>* valid_cell,
                  boct_tree_cell<T_loc, float>* alpha_cell,
                  boxm2_data_traits<BOXM2_NORMAL>::datatype* normals_arr,
                  boxm2_data_traits<BOXM2_POINT>::datatype* points_arr,
                  boxm2_data_traits<BOXM2_ALPHA>::datatype* alpha_arr,
                  int& data_idx)
{
  // go through the tree, in depth first order to collect data
  std::queue<boct_tree_cell<T_loc,T_data>*> Q;
  std::queue<boct_tree_cell<T_loc,float>*> alpha_Q;
  std::queue<boct_tree_cell<T_loc,bool>*> valid_Q;

  Q.push(tree_cell);
  alpha_Q.push(alpha_cell);
  valid_Q.push(valid_cell);

  while (!Q.empty()) {
    boct_tree_cell<T_loc,T_data>* ptr = Q.front();
    boct_tree_cell<T_loc, bool>* valid_ptr = valid_Q.front();
    boct_tree_cell<T_loc, float> alpha_ptr = alpha_Q.front();
    vgl_point_3d<double> point = tree->global_centroid(tree_cell);

    if (ptr && valid_ptr && alpha_ptr)
    {
      if (valid_ptr->data()) {
        T_data normal = tree_cell->data();

        normals_arr[data_idx][0] = normal[0];
        normals_arr[data_idx][1] = normal[2];
        normals_arr[data_idx][2] = normal[3];
        normals_arr[data_idx][3] = 0.0f;

        points_arr[data_idx][0] = point.x();
        points_arr[data_idx][1] = point.y();
        points_arr[data_idx][2] = point.z();
        points_arr[data_idx][3] = 0.0f;

        alpha_arr[data_idx]=alpha_cell->data();
      }
      else {
        normals_arr[data_idx][0] = 0.0f;
        normals_arr[data_idx][1] = 0.0f;
        normals_arr[data_idx][2] = 0.0f;
        normals_arr[data_idx][3] = 0.0f;

        points_arr[data_idx][0] = point.x();
        points_arr[data_idx][1] = point.y();
        points_arr[data_idx][2] = point.z();
        points_arr[data_idx][3] = 0.0f;

        alpha_arr[data_idx]=alpha_cell->data();
      }

      data_idx++;

      if (!ptr->is_leaf()) {
        boct_tree_cell<T_loc,T_data>* children = ptr->children();
        boct_tree_cell<T_loc, bool>* valid_children = valid_ptr->children();
        boct_tree_cell<T_loc, float>* alpha_chidren = alpha_ptr->children();
        for (unsigned j=0; j<8; j++) {
          Q.push(&children[j]);
          alpha_Q.push(&alpha_chidren[j]);
          valid_Q.push(&valid_children[j]);
        }
      }
    }
    Q.pop();
    alpha_Q.pop();
    valid_Q.pop();
  }
}

//: recursively sets the bits based on the octree structure
template <class T_loc, class T_data>
void set_bits(boct_bit_tree2*& bit_tree, int idx, unsigned int child_idx, boct_tree_cell<T_loc,T_data> & cell)
{
  if (cell.code_.level == 0)
    return;

  if (!cell.is_leaf()) {
    int bit_idx=idx*8+1+child_idx;
    bit_tree->set_bit_at(bit_idx,true);
    for (unsigned i=0; i<8; i++)
      set_bits(bit_tree, bit_idx, i, cell.children()[i]);
  }
}

//: converts the bit tree to boct_octree representation
template <class T_loc, class T_data>
void convert_to_bittree(boct_tree_cell<T_loc,T_data>* tree_cell, boct_bit_tree2*& bit_tree)
{
  bit_tree = new boct_bit_tree2();
  // first set all the bits to 0
  for (unsigned i=0; i<73; i++)
    bit_tree->set_bit_at(i,false);

  // empty tree
  if (!tree_cell)
    return;

  // only root node
  if (tree_cell->is_leaf())
    return;

  int idx=0;
  bit_tree->set_bit_at(idx,true); // root
  for (unsigned i=0; i<8; i++) {  // level root-1
    set_bits(bit_tree, idx, i, tree_cell->children()[i]);
  }
}

template <class T_loc, class T_data>
void convert_scene( boxm_scene<boct_octree<T_loc, T_data> > &scene,
                    boxm_scene<boct_octree<T_loc, bool> > &valid_scene,
                    boxm_scene<boct_octree<T_loc, float> > &alpha_scene,
                    boxm2_scene &new_scene)
{
  std::map<boxm2_block_id, boxm2_block_metadata> new_blocks;
  unsigned int dim=sub_block_dim();
  typedef boct_octree<T_loc, T_data> tree_type;
  boxm_block_iterator<tree_type > iter(&scene);
  boxm_block_iterator<tree_type > validd_iter(&valid_scene);
  boxm_block_iterator<tree_type > alpha_iter(&alpha_scene);

  while (!iter.end()) {
    vgl_point_3d<int> idx = iter.index();
    scene.load_block(idx);
    boxm_block<tree_type >* block = scene.get_block(idx);
    vgl_box_3d<double> block_bb = block->bounding_box();
    std::cout<<block_bb<<std::endl;
    tree_type * tree = block->get_tree();

    valid_scene.load_block(idx);
    boxm_block<boct_tree<short, bool> >* valid_block = valid_scene.get_block(idx);
    boct_tree<short, bool> * valid_tree = valid_block->get_tree();

    alpha_scene.load_block(idx);
    boxm_block<boct_tree<short, float> >* alpha_block = alpha_scene.get_block(idx);
    boct_tree<short, float> * alpha_tree = alpha_block->get_tree();

    // create metadata for the block
    boxm2_block_id block_id(idx.x(),idx.y(),idx.z());
    vgl_vector_3d<double> sub_block_dim(1.0/dim,1.0/dim,1.0/dim);
    vgl_vector_3d<unsigned> sub_block_num(dim,dim,dim);
    vgl_vector_3d<double> real_block_dim(sub_block_dim.x()*scene.block_dim().x(),
                                         sub_block_dim.y()*scene.block_dim().y(),
                                         sub_block_dim.z()*scene.block_dim().z());
    boxm2_block_metadata metadata(block_id, block->bounding_box().min_point(),
                                  real_block_dim,sub_block_num,1,4,650.0,0.001);
    boxm2_block new_block(metadata);
    new_blocks[block_id]=metadata;

    // find the total number of cells to figure out data array sizes, as the first step
    int data_size=0;
    for (unsigned z=0; z<dim; z++) {
      for (unsigned y=0; y<dim; y++) {
        for (unsigned x=0; x<dim; x++) {
          double cell_dim=1./dim;
          double p[3];
          p[0] = x*cell_dim;
          p[1] = y*cell_dim;
          p[2] = z*cell_dim;
          vgl_box_3d<double> cell_bb(p, cell_dim, cell_dim, cell_dim, vgl_box_3d<double>::min_pos);
          boct_tree_cell<T_loc,T_data >* node = tree->locate_point_at_level(cell_bb.centroid(),3);
          if (!node)
            std::cout << "The node COULD not be FOUND" << std::endl;
          else {
            std::vector<boct_tree_cell<T_loc,T_data> > *> children;
            node->all_children(children);
            int n2= children.size()+1;
            data_size += n2;
          }
        }
      }
    }

    // allocate data array
    boxm2_data_traits<BOXM2_NORMAL>::datatype* normals_arr=new  boxm2_data_traits<BOXM2_NORMAL>::datatype[data_size];
    boxm2_data_traits<BOXM2_POINT>::datatype* points_arr=new  boxm2_data_traits<BOXM2_POINT>::datatype[data_size];
    boxm2_data_traits<BOXM2_ALPHA>::datatype* alpha_arr = new boxm2_data_traits<BOXM2_ALPHA>::datatype[data_size];

    // divide the blocks
    int data_idx=0;
    boxm2_array_3d<boxm2_block::uchar16>& trees = new_block.trees();
    for (unsigned z=0; z<dim; z++) {
      for (unsigned y=0; y<dim; y++) {
        for (unsigned x=0; x<dim; x++) {
          double cell_dim=1./dim;
          double p[3];
          p[0] = x*cell_dim;
          p[1] = y*cell_dim;
          p[2] = z*cell_dim;
          vgl_box_3d<double> cell_bb(p, cell_dim, cell_dim, cell_dim, vgl_box_3d<double>::min_pos);
          boct_tree_cell<T_loc, T_data >* node = tree->locate_point_at_level(cell_bb.centroid(),3);
          boct_tree_cell<T_loc, bool>* valid_node = valid_tree->locate_point_at_level(cell_bb.centroid(), 3);
          boct_tree_cell<T_loc, float> *alpha_node = alpha_tree->locate_point_at_level(cell_bb.centroid(), 3);

          if (!(node && valid_node && alpha_node) )
            std::cout << "One of the nodes COULD not be FOUND" << std::endl;
          else {
            std::vector<boct_tree_cell<T_loc, T_data > *> children;
            node->all_children(children);
            int n2= children.size()+1;

            boct_bit_tree2* bit_tree;
            convert_to_bittree(node, bit_tree);
            int n1=bit_tree->num_cells();
            int start=data_idx;
            convert_data(tree, node, valid_node, alpha_node, normals_arr, points_arr, alpha_arr,  data_idx);
            // set the data index at the tree
            bit_tree->set_data_ptr(start, false);
            // set the tree at the block
            boxm2_block::uchar16& t=trees[x][y][z];
            t.set(bit_tree->get_bits());
            if (n1 != n2) {
              std::cout << x << ',' << y << ',' << z << '\n'
                       << "ERROR! The converted tree is not right, should have " << n1 << " nodes instead of " << n2 << std::endl;
            }
          }
        }
      }
    }
    char* b1 = reinterpret_cast<char *>(normals_arr);
    boxm2_data<BOXM2_NORMAL> *normals_data=new boxm2_data<BOXM2_NORMAL>(b1,data_size*sizeof(boxm2_data_traits<BOXM2_NORMAL>::datatype),block_id);
    char* b2 = reinterpret_cast<char *>(pointa_arr);
    boxm2_data<BOXM2_POINT> *points_data=new boxm2_data<BOXM2_POINT>(b2,data_size*sizeof(boxm2_data_traits<BOXM2_POINT>::datatype),block_id);
    char* b3 = reinterpret_cast<char *>(alpha_arr);
    boxm2_data<BOXM2_ALPHA> *alpha_data = new boxm2_data<BOXM2_ALPHA>(b3, data_size*sizeof(boxm2_data_traits<BOXM2_ALPHA>::datatype),block_id);

    boxm2_sio_mgr::save_block(new_scene.data_path(), &new_block);
    boxm2_sio_mgr::save_block_data(new_scene.data_path(), block_id, normals_data);
    boxm2_sio_mgr::save_block_data(new_scene.data_path(), block_id, points_data);
    boxm2_sio_mgr::save_block_data(new_scene.data_path(), block_id, alpha_data);

    delete [] normals_arr;
    delete [] points_arr;
    delete [] alpha_arr;
    iter++;
  }
  new_scene.set_blocks(new_blocks);
  new_scene.save_scene();
}

int main(int argc, char** argv)
{
  std::cout << "Converting boxm scene with normals to boxm2 normals and points" << std::endl;
  vul_arg<std::string> scene_path("-normals_scene", "scene filename", "");
  vul_arg<std::string> scene_path("-valid_scene", "scene filename", "");
  vul_arg<std::string> scene_path("-alpha_scene", "scene filename", "");
  vul_arg<std::string> out_dir("-out", "output directory", "");
  vul_arg<unsigned int> sub_block_dim("-dim", "sub block dimensions", 64);
  vul_arg_parse(argc, argv);

  boxm_scene_parser parser;
  boxm_scene_base_sptr n_scene_ptr=new boxm_scene_base();
  n_scene_ptr->load_scene(normals_scene(), parser);

  if (n_scene_ptr->appearence_model() == VNL_FLOAT_3)
  {
    typedef boct_tree<short, VNL_FLOAT_3 > tree_type;
    boxm_scene<tree_type > scene;
    scene.load_scene(normals_scene());

    boxm_scene<boct_tree<short, bool> > valid_scene;
    valid_scene.load_scene(valid_scene());

    boxm_scene<boct_tree<short, float> > alpha_scene;
    alpha_scene.load_scene(alpha_scene());

    // create a boxm2 scene
    boxm2_scene new_scene;
    new_scene.set_data_path(out_dir());
    new_scene.set_local_origin(scene.origin());
    new_scene.set_lvcs(scene.lvcs());
    new_scene.set_xml_path(out_dir()+"/scene.xml");

    //initialize a block
    boxm2_lru_cache::create(&new_scene);

    convert_scene(scene, valid_scene, alpha_scene, new_scene);
  }
}
