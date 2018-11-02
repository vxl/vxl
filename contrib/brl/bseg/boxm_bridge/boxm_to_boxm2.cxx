#include <iostream>
#include <queue>
#include "boxm_to_boxm2.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//executable args
#include <vul/vul_arg.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>

//boct files
#include <boct/boct_tree.h>
#include <boct/boct_bit_tree.h>

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


//: extracts the parameters of a boxm_sample for boxm2 representation
void deconstruct_sample(boxm_sample<BOXM_APM_MOG_GREY> sample,
                        float &alpha,
                        boxm2_data_traits<BOXM2_MOG3_GREY>::datatype& data,
                        boxm2_data_traits<BOXM2_NUM_OBS>::datatype& num_obs)
{
  alpha=sample.alpha;

  typedef boxm_apm_traits<BOXM_APM_MOG_GREY>::gauss_type_sf1 gauss_type_sf1;

  bsta_num_obs<bsta_mixture_fixed<gauss_type_sf1, 3> > obs = sample.appearance();
  unsigned int nmix = obs.num_observations*100.0;
  num_obs[3] = nmix;

  for (unsigned i=0; i<obs.num_components(); i++) {
    gauss_type_sf1  mf=obs.distribution(i);
    auto w = (unsigned char)std::floor(obs.weight(i)*255.0);
    unsigned int  n = (unsigned char)mf.num_observations;
    auto m = (unsigned char)std::floor(mf.mean()*255.0);
    auto v = (unsigned char)std::floor(mf.var()*255.0);
    num_obs[i]=n;
    data[i*3]=m;
    data[i*3+1]=v;
    if (i<2)
      data[i*3+2]=w;
  }
}

//: extracts the tree cell's data into an array format for boxm2 representation
template <class T_loc, class T_data>
void convert_data(boct_tree_cell<T_loc,T_data>* tree_cell,
                  float* alpha,
                  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype* data,
                  boxm2_data_traits<BOXM2_NUM_OBS>::datatype* num_obs,
                  int& data_idx)
{
    // go through the tree, in depth first order to collect data
    std::queue<boct_tree_cell<T_loc,T_data>*> Q;
    Q.push(tree_cell);
    while (!Q.empty()) {
        boct_tree_cell<T_loc,T_data>* ptr = Q.front();
        {
            deconstruct_sample(ptr->data(),alpha[data_idx],data[data_idx],num_obs[data_idx]);
            data_idx++;
            if (!ptr->is_leaf()) {
                boct_tree_cell<T_loc,T_data>* children = ptr->children();
                for (unsigned j=0; j<8; j++) {
                    Q.push(&children[j]);
                }
            }
        }
        Q.pop();
    }
}

//: recursively sets the bits based on the octree structure
template <class T_loc, class T_data>
void set_bits(boct_bit_tree*& bit_tree, int idx, unsigned int child_idx, boct_tree_cell<T_loc,T_data> & cell)
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
void convert_to_bittree(boct_tree_cell<T_loc,T_data>* tree_cell, boct_bit_tree*& bit_tree)
{
  bit_tree = new boct_bit_tree();
  // first set all the bits to 0
  for (unsigned i=0; i<73; i++)
    bit_tree->set_bit_at(i,false);

  // empty tree
  if (!tree_cell)
    return;

  // only root node
  if (tree_cell->is_leaf()) {
    return;
  }

  int idx=0;
  bit_tree->set_bit_at(idx,true); // root
  for (unsigned i=0; i<8; i++) {  // level root-1
    set_bits(bit_tree, idx, i, tree_cell->children()[i]);
  }
}

int main(int argc, char** argv)
{
  std::cout << "Converting boxm scene to boxm2 Scene" << std::endl;
  vul_arg<std::string> scene_path("-scene", "scene filename", "");
  vul_arg<std::string> out_dir("-out", "output directory", "");
  vul_arg<unsigned int> sub_block_dim("-dim", "sub block dimensions", 64);
  vul_arg_parse(argc, argv);

  typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
  boxm_scene<tree_type > scene;
  scene.load_scene(scene_path());

  // create a boxm2 scene
  boxm2_scene new_scene;
  new_scene.set_data_path(out_dir());
  new_scene.set_local_origin(scene.origin());
  new_scene.set_lvcs(scene.lvcs());
  new_scene.set_xml_path(out_dir()+"/scene.xml");

  //initialize a block
  boxm2_lru_cache::create(&new_scene);

  std::map<boxm2_block_id, boxm2_block_metadata> new_blocks;
  unsigned int dim=sub_block_dim();
  if (scene.appearence_model() == BOXM_APM_MOG_GREY) {
    boxm_block_iterator<tree_type > iter(&scene);
    while (!iter.end()) {
      vgl_point_3d<int> idx = iter.index();
      scene.load_block(idx);
      boxm_block<tree_type >* block = scene.get_block(idx);
      vgl_box_3d<double> block_bb = block->bounding_box();
      std::cout<<block_bb<<std::endl;
      tree_type * tree = block->get_tree();

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
            boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> >* node = tree->locate_point_at_level(cell_bb.centroid(),3);
            if (!node)
              std::cout << "The node COULD not be FOUND" << std::endl;
            else {
              std::vector<boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> > *> children;
              node->all_children(children);
              int n2= children.size()+1;
              data_size += n2;
            }
          }
        }
      }

      // allocate data array
      auto* data_arr=new  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype[data_size];
      auto* num_obs_arr=new  boxm2_data_traits<BOXM2_NUM_OBS>::datatype[data_size];
      auto* alpha_arr = new float[data_size];

      // divide the blocks
      int data_idx=0;
      boxm2_array_3d<boxm2_block::uchar16> trees = new_block.trees_copy();
      for (unsigned z=0; z<dim; z++) {
        for (unsigned y=0; y<dim; y++) {
          for (unsigned x=0; x<dim; x++) {
            double cell_dim=1./dim;
            double p[3];
            p[0] = x*cell_dim;
            p[1] = y*cell_dim;
            p[2] = z*cell_dim;
            vgl_box_3d<double> cell_bb(p, cell_dim, cell_dim, cell_dim, vgl_box_3d<double>::min_pos);
            boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> >* node = tree->locate_point_at_level(cell_bb.centroid(),3);
            if (!node)
              std::cout << "The node COULD not be FOUND" << std::endl;
            else {
              std::vector<boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> > *> children;
              node->all_children(children);
              int n2= children.size()+1;

              boct_bit_tree* bit_tree;
              convert_to_bittree(node, bit_tree);
              int n1=bit_tree->num_cells();
              int start=data_idx;
              convert_data(node, alpha_arr, data_arr, num_obs_arr, data_idx);
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
      new_block.set_trees(trees);
      char* b1 = reinterpret_cast<char *>(alpha_arr);
      boxm2_data<BOXM2_ALPHA> *alpha_data=new boxm2_data<BOXM2_ALPHA>(b1,data_size*sizeof(float),block_id);
      char* b2 = reinterpret_cast<char *>(data_arr);
      boxm2_data<BOXM2_MOG3_GREY> *mog_data=new boxm2_data<BOXM2_MOG3_GREY>(b2,data_size*sizeof(boxm2_data_traits<BOXM2_MOG3_GREY>::datatype),block_id);
      char* b3 = reinterpret_cast<char *>(num_obs_arr);
      boxm2_data<BOXM2_NUM_OBS> *num_data=new boxm2_data<BOXM2_NUM_OBS>(b3,data_size*sizeof(boxm2_data_traits<BOXM2_NUM_OBS>::datatype),block_id);

      boxm2_sio_mgr::save_block(new_scene.data_path(), &new_block);
      boxm2_sio_mgr::save_block_data(new_scene.data_path(), block_id, alpha_data);
      boxm2_sio_mgr::save_block_data(new_scene.data_path(), block_id, mog_data);
      boxm2_sio_mgr::save_block_data(new_scene.data_path(), block_id, num_data);

      delete [] alpha_arr;
      delete [] data_arr;
      delete [] num_obs_arr;
      iter++;
    }
    new_scene.set_blocks(new_blocks);
    new_scene.save_scene();
  }
}
