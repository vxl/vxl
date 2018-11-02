//:
// \file
#include <iostream>
#include <queue>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

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

#define BOXM2_SCENE_HAS_DIMENSIONS

//: computes the log2 of a number and returns it.
// power_of_2 is set to true if \p x is an exact power of two.
// returns -1 when \p x is zero.
int mylog2(unsigned x, bool& power_of_2)
{
  unsigned orig_x = x;
  int l = -1; // mylog2(0) will return -1
  while (x != 0u) {
    x >>= 1u;
    ++l;
  }
  if (orig_x > std::pow(2.0,l)) {
    power_of_2 = false;
    ++l;
  }
  else
    power_of_2 = true;
  return l;
}


template <class T_loc, class T_data>
void insert(boct_tree<T_loc,T_data>*& tree, boct_tree<T_loc,T_data>*& subtree, vgl_box_3d<double> cell_bb)
{
  // find the leaf node in the tree to insert subtree
  boct_tree_cell<T_loc,T_data>* node = tree->locate_point(cell_bb.centroid());
  if (!node)
    std::cerr << "The node could NOT be FOUND\n";
  if (node->children()) {
    std::cerr << "The node is not a leaf node! Cannot insert here\n";
    return;   // it should have been a leaf node
  }

  boct_tree_cell<T_loc,T_data>*  sub_root = subtree->root();
  node->insert_subtree(sub_root);
}

// converts the bit tree to boct_octree representation
template <class T_loc, class T_data>
void convert_tree(boct_bit_tree2 const& bit_tree, boct_tree<T_loc,T_data>*& tree,
                  boxm2_data<BOXM2_NORMAL>* normal_data)
{
  // check if root has any child, if not return the empty tree
  tree = new boct_tree<T_loc,T_data>(4, 1);

  // if root has no child return a root node filled with data
  if (!bit_tree.bit_at(0)) {
    int data_idx = bit_tree.get_data_index(0,false);
    boxm2_data_traits<BOXM2_NORMAL>::datatype data = normal_data->data()[data_idx];
    tree->root()->set_data(vnl_vector_fixed<float, 3> (data[0], data[1], data[2]));
    return;
  }

  std::queue<boct_tree_cell<T_loc,T_data>*> Q;
  boct_tree_cell<T_loc,T_data>* root = tree->root();
  Q.push(root);

  // go through the bits and create cells
  int data_idx=bit_tree.get_data_index(0,false);
  for (unsigned i=0; i<73; i++) {
    int parent_idx = (i-1)/8;
    if (i==0 || bit_tree.bit_at(parent_idx)) {
      if (!Q.empty()) {
        boxm2_data_traits<BOXM2_NORMAL>::datatype data = normal_data->data()[data_idx];
        vnl_vector_fixed<float, 3> normal(data[0], data[1], data[2]);
        boct_tree_cell<T_loc,T_data>* ptr = Q.front();
        ptr->set_data(normal);
        Q.pop();
        if (bit_tree.bit_at(i)) {
          ptr->split();
          boct_tree_cell<T_loc,T_data>* children = ptr->children();
          for (unsigned j=0; j<8; j++) {
            Q.push(&children[j]);
          }
        }
        data_idx++;
      }
    }
  }

  // some of the leaf nodes are still in the queue, fill them with data
  while (!Q.empty()) {
    if (data_idx > bit_tree.get_data_index(0,false)+585)
      std::cerr << "ERROR! exceeded!\n";
    boxm2_data_traits<BOXM2_NORMAL>::datatype data = normal_data->data()[data_idx];
    vnl_vector_fixed<float, 3> normal(data[0], data[1], data[2]);
    boct_tree_cell<T_loc,T_data>* ptr = Q.front();
    ptr->set_data(normal);
    Q.pop();
    data_idx++;
  }
}

int main(int argc, char** argv)
{
  std::cout<<"Converting boxm2 scene to boxm scene"<<std::endl;
  vul_arg<std::string> boxm2_file("-scene", "scene filename", "");
  vul_arg<std::string> boxm_dir("-out", "output directory", "");
  vul_arg_parse(argc, argv);

  boxm2_scene scene2(boxm2_file());

  //initialize a block and data cache
  boxm2_lru_cache::create(&scene2);
  boxm2_cache_sptr cache = boxm2_cache::instance();

  bgeo_lvcs lvcs = scene2.lvcs();
  //vgl_point_3d<double> origin = scene2.local_origin();
  vgl_box_3d<double> world = scene2.bounding_box();
  std::map<boxm2_block_id, boxm2_block_metadata> blocks = scene2.blocks();
  std::map<boxm2_block_id, boxm2_block_metadata>::iterator iter = blocks.begin();

  typedef boct_tree<short, vnl_vector_fixed<float, 3> > tree_type;
  vgl_vector_3d<unsigned int> block_nums = scene2.scene_dimensions();
  vgl_point_3d<int> min_block_index;
  vgl_point_3d<double> origin;
  scene2.min_block_index(min_block_index, origin);
  vgl_vector_3d<double> ww(world.width()/block_nums.x(),world.height()/block_nums.y(),world.depth()/block_nums.z());


  boxm_scene<tree_type> scene(lvcs, origin, ww, block_nums,false,true,true);
  std::string scene_path=boxm_dir(); //"C:/data/boxm2/downtown/boxm_scene";
  scene.set_paths(scene_path,"block");
  scene.set_appearance_model(VNL_FLOAT_3);

  int x_dim;
  while (iter != blocks.end()) {
    boxm2_block_metadata metadata = iter ->second;
    boxm2_block_id id = iter->first;
    boxm2_block *     block     = cache->get_block(iter->first);
    std::cout<<" DATA buffers "<< block->num_buffers()<<std::endl;

    boxm2_data_base * data_base  = cache->get_data_base(iter->first,boxm2_data_traits<BOXM2_NORMAL>::prefix());
    boxm2_data<BOXM2_NORMAL> *data =new boxm2_data<BOXM2_NORMAL>(data_base->data_buffer(),data_base->buffer_length(),data_base->block_id());

    // compute the block bounding box etc
    vgl_vector_3d<unsigned int> dim = block->sub_block_num();
    vgl_vector_3d<double> subdim=block->sub_block_dim();

    // block dimensions should be equal
    bool equal = (dim.x()==dim.y());
    equal = equal && (dim.x()==dim.z());
    assert(equal);

    // the block dimensions should be the power of 2 for octree creation
    bool pow_of_2;
    x_dim = mylog2((double)dim.x(),pow_of_2);
    assert(pow_of_2);

    double p[3];
    p[0] = metadata.local_origin_.x();
    p[1] = metadata.local_origin_.y();
    p[2] = metadata.local_origin_.z();
    vgl_box_3d<double> block_bb(p, subdim.x()*dim.x(), subdim.y()*dim.y(), subdim.z()*dim.z(), vgl_box_3d<double>::min_pos);
    tree_type* block_tree=new tree_type(x_dim+4, x_dim+1); // FIX take max of 3 dims instead
    block_tree->set_bbox(block_bb);
    block_tree->init_cells(0);

    int block_i = id.i() - min_block_index.x();
    int block_j = id.j() - min_block_index.y();
    int block_k = id.k() - min_block_index.z();

    scene.load_block(block_i, block_j, block_k);
    // go over the subtrees
    for (unsigned z=0; z<dim.z(); z++) {
      for (unsigned y=0; y<dim.y(); y++) {
        for (unsigned x=0; x<dim.x(); x++) {
          boxm2_block::uchar16 tree = block->trees()[x][y][z];
          boct_bit_tree2 bit_tree((unsigned char*)tree.data_block());
          if (bit_tree.num_cells() >= 1) {
            tree_type* octree;
            convert_tree(bit_tree,octree,data);
            int n1=bit_tree.num_cells();
            int n2=octree->all_cells().size();
            if (n1 != n2) {
              std::cerr << x << ',' << y << ',' << z << '\n'
              << "ERROR! The converted tree is not right, should have " << n1 << " nodes instead of " << n2 << '\n';
            }

            // all three dimensions should be same now, so we can use the one value
            double cell_dim=1./dim.x();
            p[0] = x*cell_dim;
            p[1] = y*cell_dim;
            p[2] = z*cell_dim;
            vgl_box_3d<double> cell_bb(p, cell_dim, cell_dim, cell_dim, vgl_box_3d<double>::min_pos);
            insert(block_tree,octree, cell_bb);

            // since the subtree is added to the main tree, we can delete the root node now
            delete octree;
          }
        }
      }
    }
    boxm_block<tree_type>* block_new = scene.get_block(block_i, block_j, block_k);
    block_new->init_tree(block_tree);
    scene.write_active_block();
    iter++;
  }
  scene.set_octree_levels(x_dim+4, x_dim+1);
  scene.write_scene("boxm_scene.xml");
}
