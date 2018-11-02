#ifndef boxm_scene_hxx_
#define boxm_scene_hxx_
//:
// \file
#include <cmath>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include "boxm_scene.h"
//
#include "boxm_apm_traits.h"
#include "boxm_scene_parser.h"

#include <vgl/xio/vgl_xio_point_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>
#include <vgl/vgl_intersection.h>
#include <vsl/vsl_basic_xml_element.h>
#include <vsl/vsl_binary_io.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/xio/vpgl_xio_lvcs.h>

#include <vpl/vpl.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

template <class T>
boxm_scene<T>::boxm_scene(const vpgl_lvcs& lvcs,
                          const vgl_point_3d<double>& origin,
                          const vgl_vector_3d<double>& block_dim,
                          const vgl_vector_3d<unsigned>& world_dim,
                          const bool load_all_blocks,
                          const bool save_internal_nodes,
                          const bool save_platform_independent)
: lvcs_(lvcs),
  origin_(origin),
  block_dim_(block_dim),
  active_block_(vgl_point_3d<int>(-1,-1,-1)),
  save_internal_nodes_(save_internal_nodes),
  save_platform_independent_(save_platform_independent),
  load_all_blocks_(load_all_blocks)
{
  create_blocks(block_dim, world_dim);
}

template <class T>
boxm_scene<T>::boxm_scene( const vgl_point_3d<double>& origin,
                           const vgl_vector_3d<double>& block_dim,
                           const vgl_vector_3d<unsigned>& world_dim,
                           const bool load_all_blocks,
                           const bool save_internal_nodes,
                           const bool save_platform_independent)
: origin_(origin),
  block_dim_(block_dim),
  active_block_(vgl_point_3d<int>(-1,-1,-1)),
  save_internal_nodes_(save_internal_nodes),
  save_platform_independent_(save_platform_independent),
  load_all_blocks_(load_all_blocks)
{
  create_blocks(block_dim, world_dim);
}

template <class T>
boxm_scene<T>::boxm_scene(const boxm_scene& scene)
: boxm_scene_base(scene),
  lvcs_(scene.lvcs()),
  origin_(scene.origin()),
  block_dim_(scene.block_dim()),
  active_block_(vgl_point_3d<int>(-1,-1,-1)),
  save_internal_nodes_(scene.save_internal_nodes()),
  save_platform_independent_(scene.save_platform_independent()),
  load_all_blocks_(scene.load_all_blocks())
{
  int x,y,z;
  scene.block_num(x,y,z);
  create_blocks(block_dim_, vgl_vector_3d<unsigned>(x,y,z));
}


template <class T>
void boxm_scene<T>::create_blocks(const vgl_vector_3d<double>& /*block_dim*/,
                                  const vgl_vector_3d<unsigned>& world_dim)
{
  // compute the dimensions of 3D array
  unsigned x_dim = world_dim.x(); // = static_cast<int>(std::floor(world_dim.x()/block_dim.x()));
  unsigned y_dim = world_dim.y(); // = static_cast<int>(std::floor(world_dim.y()/block_dim.y()));
  unsigned z_dim = world_dim.z(); // = static_cast<int>(std::floor(world_dim.z()/block_dim.z()));

  // pointers are initialized to NULL
  blocks_ =  vbl_array_3d<boxm_block<T>*>(x_dim, y_dim, z_dim, (boxm_block<T>*)nullptr);
  for (unsigned i=0; i<x_dim; ++i) {
    for (unsigned j=0; j<y_dim; ++j) {
      for (unsigned k=0; k<z_dim; ++k) {
        create_block(i,j,k);
      }
    }
  }
}

template <class T>
boxm_scene<T>::boxm_scene(const vpgl_lvcs& lvcs,
                          const vgl_point_3d<double>& origin,
                          const vgl_vector_3d<double>& block_dim,
                          const vgl_vector_3d<unsigned>& world_dim,
                          unsigned max_level, unsigned init_level,
                          const bool load_all_blocks,
                          const bool save_internal_nodes,
                          const bool save_platform_independent)
: lvcs_(lvcs),
origin_(origin),
block_dim_(block_dim),
active_block_(vgl_point_3d<int>(-1,-1,-1)),
save_internal_nodes_(save_internal_nodes),
save_platform_independent_(save_platform_independent),
load_all_blocks_(load_all_blocks)
{
  create_blocks(block_dim, world_dim);
  set_octree_levels(max_level, init_level);
}

template <class T>
void boxm_scene<T>::create_block(unsigned i, unsigned j, unsigned k)
{
  if (blocks_(i,j,k) == NULL) {
    vgl_box_3d<double> bbox = get_block_bbox(i,j,k);
    blocks_(i,j,k) = new boxm_block<T>(bbox);
  }
}

template <class T>
boxm_scene<T>::~boxm_scene()
{
  // delete the blocks
  int x_dim, y_dim, z_dim;
  block_num(x_dim, y_dim, z_dim);
  for (int i=0; i<x_dim; i++) {
    for (int j=0; j<y_dim; j++) {
      for (int k=0; k<z_dim; k++) {
        if (blocks_(i,j,k))
          delete blocks_(i,j,k);
      }
    }
  }
}

//: Returns a scene with the same structure and initialized with given value
template<class T>
void boxm_scene<T>::clone_blocks(boxm_scene<T> &scene_out, datatype data)
{
  boxm_block_iterator<T> iter(this);
  boxm_block_iterator<T> iter_out = scene_out.iterator();
  iter.begin();
  iter_out.begin();
  while (!iter.end())
  {
    load_block(iter.index());
    scene_out.load_block(iter_out.index());
    T  *tree_out = (*iter)->get_tree()->clone_all();
    tree_out->init_cells(data);
    (*iter_out)->init_tree(tree_out);
    scene_out.write_active_block();
    ++iter;
    ++iter_out;
  }
}

//: Returns a scene with the same structure and data
template<class T>
void boxm_scene<T>::clone_blocks(boxm_scene<T> &scene_out)
{
  boxm_block_iterator<T> iter(this);
  boxm_block_iterator<T> iter_out = scene_out.iterator();
  iter.begin();
  iter_out.begin();
  while (!iter.end())
  {
    load_block(iter.index());
    T  *tree_out = (*iter)->get_tree()->clone_all();
    (*iter_out)->init_tree(tree_out);
    scene_out.write_active_block();
    ++iter;
    ++iter_out;
  }
}

template <class T>
void boxm_scene<T>::write_active_block(bool unload_block)
{
  if (valid_index(active_block_)&& (!load_all_blocks_))
  {
    int x=active_block_.x(), y=active_block_.y(), z=active_block_.z();
    std::string path = gen_block_path(x,y,z);
    vsl_b_ofstream os(path);
#ifdef DEBUG
    std::cout << "block path: " << path << '\n'
             << "Load All blocks " << load_all_blocks_ << '\n'
             << "Internal Nodes 2: " << save_internal_nodes_ << " save_platform_independent_ " << save_platform_independent_ << std::endl;
#endif
    blocks_(x,y,z)->b_write(os, save_internal_nodes_, save_platform_independent_);

    // delete the block's data
    if (unload_block)
    {
      boxm_block<T>* block = blocks_(x,y,z);
      block->delete_tree();
      block->set_tree(nullptr);
      active_block_.set(-1,-1,-1);
    }
    os.close();
  }
}

template <class T>
void boxm_scene<T>::write_active_block(bool unload_block) const
{
  assert(!unload_block); // use the "non-const" method ::write_active_block if you want to delete the block's data

  if (valid_index(active_block_)&& (!load_all_blocks_))
  {
    int x=active_block_.x(), y=active_block_.y(), z=active_block_.z();
    std::string path = gen_block_path(x,y,z);
    vsl_b_ofstream os(path);
#ifdef DEBUG
    std::cout << "block path: " << path << '\n'
             << "Load All blocks " << load_all_blocks_ << '\n'
             << "Internal Nodes 2: " << save_internal_nodes_ << " save_platform_independent_ " << save_platform_independent_ << std::endl;
#endif
    blocks_(x,y,z)->b_write(os, save_internal_nodes_, save_platform_independent_);
    os.close();
  }
}


template <class T>
void boxm_scene<T>::write_active_blocks(bool unload_block)
{
  std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >::iterator it = active_blocks_.begin();
  for (; it!=active_blocks_.end(); it++)
  {
    vgl_point_3d<int> bidx = *it;
    if (valid_index(bidx)&& (!load_all_blocks_))
    {
      int x=bidx.x(), y=bidx.y(), z=bidx.z();
      std::string path = gen_block_path(x,y,z);
      vsl_b_ofstream os(path);
  #ifdef DEBUG
      std::cout<<"Load All blocks "<<load_all_blocks_<<'\n'
              <<"Internal Nodes 2: " << save_internal_nodes_ << " save_platform_independent_ " << save_platform_independent_ << std::endl;
  #endif
      blocks_(x,y,z)->b_write(os, save_internal_nodes_, save_platform_independent_);

      // delete the block's data
      if (unload_block)
      {
        boxm_block<T>* block = blocks_(x,y,z);
        block->delete_tree();
        block->set_tree(nullptr);
        active_block_.set(-1,-1,-1);
      }
      os.close();
    }
  }
}

template <class T>
void boxm_scene<T>::write_active_blocks(bool unload_block) const
{
  assert(!unload_block); // use the "non-const" method ::write_active_blocks if you want to delete the block's data
  std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >::iterator it = active_blocks_.begin();
  for (; it!=active_blocks_.end(); it++)
  {
    vgl_point_3d<int> bidx = *it;
    if (valid_index(bidx)&& (!load_all_blocks_))
    {
      int x=bidx.x(), y=bidx.y(), z=bidx.z();
      std::string path = gen_block_path(x,y,z);
      vsl_b_ofstream os(path);
  #ifdef DEBUG
      std::cout<<"Load All blocks "<<load_all_blocks_<<'\n'
              <<"Internal Nodes 2: " << save_internal_nodes_ << " save_platform_independent_ " << save_platform_independent_ << std::endl;
  #endif
      blocks_(x,y,z)->b_write(os, save_internal_nodes_, save_platform_independent_);
      os.close();
    }
  }
}

//: Writes and unloads the specified block. It doesn't modify global variables
template <class T>
void boxm_scene<T>::write_block_thread_safe(unsigned i, unsigned j, unsigned k)
{
  if (valid_index(i,j,k)&& (!load_all_blocks_))
  {
    std::string path = gen_block_path(i,j,k);
    vsl_b_ofstream os(path);

    if (blocks_(i,j,k))
      blocks_(i,j,k)->b_write(os, save_internal_nodes_, save_platform_independent_);

    // delete the block's data
    boxm_block<T>* block = blocks_(i,j,k);
    block->delete_tree();
    block->set_tree(nullptr);

    os.close();
  }
}

template <class T>
void boxm_scene<T>::force_write_blocks()
{
  boxm_block_iterator<T> iter(this);
  iter.begin();
  while (!iter.end())
  {
    int x=iter.index().x(), y=iter.index().y(), z=iter.index().z();
    if (blocks_(x,y,z)->get_tree()!=nullptr)
    {
      std::cout<<" ? ";
      std::string path = gen_block_path(x,y,z);
      vsl_b_ofstream os(path);
      blocks_(x,y,z)->b_write(os, save_internal_nodes_, save_platform_independent_);
      os.close();
    }
    iter++;
  }
}

//: Returns the block this point resides in
template <class T>
boxm_block<T>* boxm_scene<T>::get_block(vgl_point_3d<double> const& p) const
{
  vgl_box_3d<double> world(get_world_bbox());
  if (world.contains(p)) {
    // find the block index
    unsigned i = static_cast<unsigned>((p.x()-origin_.x())/block_dim_.x());
    unsigned j = static_cast<unsigned>((p.y()-origin_.y())/block_dim_.y());
    unsigned k = static_cast<unsigned>((p.z()-origin_.z())/block_dim_.z());
    return blocks_(i,j,k);
  }
  else {
#ifdef DEBUG
    std::cerr << "Point " << p << " is out of world " << world << '\n';
#endif
    return nullptr;
  }
}


//: Returns the index of the block containing this point
template <class T>
bool boxm_scene<T>::get_block_index(vgl_point_3d<double> const& p, vgl_point_3d<int>& index)
{
  vgl_box_3d<double> world(get_world_bbox());
  if (world.contains(p)) {
    // find the block index
    unsigned i = static_cast<unsigned>((p.x()-origin_.x())/block_dim_.x());
    unsigned j = static_cast<unsigned>((p.y()-origin_.y())/block_dim_.y());
    unsigned k = static_cast<unsigned>((p.z()-origin_.z())/block_dim_.z());
    // boundary case
    double tol = 1e-9;
    if (std::abs(p.x()-world.max_x())<tol)
      i-=1;
    if (std::abs(p.y()-world.max_y())<tol)
      j-=1;
    if (std::abs(p.z()-world.max_z())<tol)
      k-=1;

    index=vgl_point_3d<int>(i,j,k);
    return true;
  }
  else {
#ifdef DEBUG
    std::cerr << "Point " << p << " is out of world " << world << '\n';
#endif
    return false;
  }
}

template <class T>
boxm_block<T>* boxm_scene<T>::get_active_block()
{
  if (valid_index(active_block_)) {
    boxm_block<T>* block = blocks_(active_block_.x(),active_block_.y(),active_block_.z());
    if (block->get_tree() == nullptr)
      load_block(active_block_.x(),active_block_.y(),active_block_.z());
    return block;
  }
  else {
    std::cerr << "index"<<active_block_<<"  is out of world\n";
    return nullptr;
  }
}

template <class T>
vgl_box_3d<double> boxm_scene<T>::get_world_bbox() const
{
  vgl_point_3d<double> min_p(origin_.x(), origin_.y(), origin_.z());
  vgl_point_3d<double> max_p(min_p.x()+block_dim_.x()*blocks_.get_row1_count(),
                             min_p.y()+block_dim_.y()*blocks_.get_row2_count(),
                             min_p.z()+block_dim_.z()*blocks_.get_row3_count());

  vgl_box_3d<double> bbox(min_p, max_p);
  return bbox;
}

template <class T>
void boxm_scene<T>::axes_length(double &x_length,double &y_length, double &z_length) const
{
  x_length = block_dim_.x()*blocks_.get_row1_count();
  y_length = block_dim_.y()*blocks_.get_row2_count();
  z_length = block_dim_.z()*blocks_.get_row3_count();
}


template <class T>
vgl_box_3d<double> boxm_scene<T>::get_block_bbox(int x, int y, int z) const
{
  vgl_point_3d<double> min_p(block_dim_.x()*x+origin_.x(), block_dim_.y()*y+origin_.y(), block_dim_.z()*z+origin_.z());
  vgl_point_3d<double> max_p(min_p.x()+block_dim_.x(), min_p.y()+block_dim_.y(), min_p.z()+block_dim_.z());

  vgl_box_3d<double> bbox(min_p, max_p);
  return bbox;
}

template <class T>
std::string boxm_scene<T>::gen_block_path(int x, int y, int z) const
{
  std::stringstream strm;

  strm << scene_path_ << '/' << block_pref_ << '_' <<  x << '_' << y << '_' << z << ".bin";

  return strm.str();
}

template <class T>
bool boxm_scene<T>::discover_block(unsigned i, unsigned j, unsigned k)
{
  if (!valid_index(vgl_point_3d<int>(i,j,k)))
    return false;
  std::string block_path = gen_block_path(i,j,k);
  vsl_b_ifstream os(block_path);

  // if the binary block file is not found, return "false"
  return !os == false;
}

//: returns true if the block bin file is found on disc, false otherwise.
// If false, a new tree is create for the block
template <class T>
bool boxm_scene<T>::load_block(unsigned i, unsigned j, unsigned k)
{
  if (!valid_index(vgl_point_3d<int>(i,j,k)))
    return false;

  if (!load_all_blocks_)
  {
    // this does not save the active block so make sure to save it first
    if (valid_index(active_block_)) {
      if (!(active_block_ == vgl_point_3d<int>(i,j,k)))
      {
        int x=active_block_.x(), y=active_block_.y(), z=active_block_.z();
        boxm_block<T>* block = blocks_(x,y,z);
        block->delete_tree();
        block->set_tree(nullptr);
      }
    }
  }

  active_block_.set(i,j,k);

  if (blocks_(i,j,k)->get_tree()==nullptr) // read it from file
  {
    std::string block_path = gen_block_path(i,j,k);
    vsl_b_ifstream os(block_path);

    // if the binary block file is not found
    if (!os) {
      if (blocks_(i,j,k)->get_tree()==nullptr) {
        T* tree= new T(max_tree_level_,init_tree_level_);
        blocks_(i,j,k)->init_tree(tree);
      }
      return false;
    }
    blocks_(i,j,k)->b_read(os);
    os.close();
  }

  return true;
}


//: Load block into memory - this doesn't use global variables so user is responsible for unloading it
template <class T>
bool boxm_scene<T>::load_block_thread_safe(unsigned i, unsigned j, unsigned k)
{
  if (!valid_index(vgl_point_3d<int>(i,j,k)))
    return false;

  if (blocks_(i,j,k)->get_tree()==nullptr) // read it from file
  {
    std::string block_path = gen_block_path(i,j,k);
    vsl_b_ifstream os(block_path);

    // if the binary block file is not found
    if (!os) {
      if (blocks_(i,j,k)->get_tree()==nullptr) {
        T* tree= new T(max_tree_level_,init_tree_level_);
        blocks_(i,j,k)->init_tree(tree);
      }
      return false;
    }
    blocks_(i,j,k)->b_read(os);
    os.close();
  }

  return true;
}

//: Reads and loads all blocks into memorty
template <class T>
bool boxm_scene<T>::read_all_blocks()
{
  load_all_blocks_ = true;
  this->unload_active_blocks();

  for (unsigned block_i = 0; block_i < blocks_.get_row1_count(); block_i++){
    for (unsigned block_j = 0; block_j < blocks_.get_row2_count(); block_j++){
      for (unsigned block_k = 0; block_k < blocks_.get_row3_count(); block_k++){
        if (blocks_(block_i,block_j,block_k)->get_tree()==nullptr) // read it from file
        {
          std::string block_path = gen_block_path(block_i,block_j,block_k);
          vsl_b_ifstream os(block_path);

          // if the binary block file is not found
          if (!os) {
            if (blocks_(block_i,block_j,block_k)->get_tree()==nullptr) {
              T* tree= new T(max_tree_level_,init_tree_level_);
              blocks_(block_i,block_j,block_k)->init_tree(tree);
            }
          }
          else
            blocks_(block_i,block_j,block_k)->b_read(os);

          active_blocks_.insert(vgl_point_3d<int>(block_i, block_j, block_k));
          os.close();
        }
      }
    }
  }

  return true;
}

template <class T>
bool boxm_scene<T>::load_block_and_neighbors(unsigned i, unsigned j, unsigned k)
{
  if (!valid_index(vgl_point_3d<int>(i,j,k)))
    return false;

  active_block_.set(i,j,k);
  // Set unused blocks to null and load new blocks
  // this is to avoid rereading blocks that are already in memory
  std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >  new_active_blocks  = neighboring_blocks(vgl_point_3d<int>(i,j,k));
  new_active_blocks.insert(vgl_point_3d<int>(i,j,k));
  std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >  blocks_to_unload;
  std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >  blocks_to_load;
  bvgl_point_3d_cmp<int>  cmp;
  std::set_difference(active_blocks_.begin(), active_blocks_.end(),
                     new_active_blocks.begin(), new_active_blocks.end(),
                     std::insert_iterator<std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> > >(blocks_to_unload, blocks_to_unload.begin()),
                     cmp);

  std::set_difference(new_active_blocks.begin(), new_active_blocks.end(),
                     active_blocks_.begin(), active_blocks_.end(),
                     std::insert_iterator<std::set<vgl_point_3d<int> , bvgl_point_3d_cmp<int> > > (blocks_to_load, blocks_to_load.begin()),
                     cmp);

  active_blocks_.clear();
  active_blocks_ = new_active_blocks;

  std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >::iterator unload_it = blocks_to_unload.begin();

  for (; unload_it!=blocks_to_unload.end(); unload_it++)
  {
    boxm_block<T>* block = blocks_((*unload_it).x(),(*unload_it).y(),(*unload_it).z());
    block->delete_tree();
    block->set_tree(nullptr);
  }

  std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >::iterator load_it = blocks_to_load.begin();

  for (; load_it!=blocks_to_load.end(); load_it++)
  {
    int block_i = (*load_it).x();
    int block_j = (*load_it).y();
    int block_k = (*load_it).z();

    if (blocks_(block_i,block_j,block_k)->get_tree() == nullptr)// read it from file
    {
      std::string block_path = gen_block_path(block_i,block_j,block_k);
      vsl_b_ifstream os(block_path);

      // if the binary block file is not found
      if (!os) {
        if (blocks_(block_i,block_j,block_k)->get_tree()==nullptr) {
          T* tree= new T(max_tree_level_,init_tree_level_);
          blocks_(block_i,block_j,block_k)->init_tree(tree);
        }
        return false;
      }
      blocks_(block_i,block_j,block_k)->b_read(os);
      assert(blocks_(block_i,block_j,block_k)!=NULL);
      assert(blocks_(block_i,block_j,block_k)->get_tree()!=nullptr);

      os.close();
    }
  }

  return true;
}

//: Return the finest level in the scene
template <class T>
short boxm_scene<T>::finest_level()
{
  // iterate through the blocks requesting the finest level
  boxm_block_iterator<T > iter=this->const_iterator();
  iter.begin();
  short finest_level = this->max_tree_level_;
  while (!iter.end()) {
    if (this->load_block(iter.index().x(),iter.index().y(),iter.index().z())) {
      short this_level = get_active_block()->get_tree()->finest_level();
      if (this_level < finest_level)
        finest_level = this_level;
    }
    iter++;
  }

  return finest_level;
}


//: Return the finest level in the scene. Iterates through blocks assuming they are all in memory
template <class T>
short boxm_scene<T>::finest_level_in_memory() const
{
  if (!load_all_blocks_)
    return -1;

 // iterate through the blocks requesting the finest level
  short finest_level = this->max_tree_level_;
  for (unsigned block_i = 0; block_i < blocks_.get_row1_count(); block_i++){
    for (unsigned block_j = 0; block_j < blocks_.get_row2_count(); block_j++){
      for (unsigned block_k = 0; block_k < blocks_.get_row3_count(); block_k++){
        if (blocks_(block_i,block_j,block_k)) {
            short this_level = blocks_(block_i,block_j,block_k)->get_tree()->finest_level();
            if (this_level < finest_level)
              finest_level = this_level;
          }
      }
    }
  }

  return finest_level;
}

//: Return the length of finest-level cell in the scene
template <class T>
double  boxm_scene<T>::finest_cell_length()
{
  double local_cell_length = 1.0/(double)(1<<((this->max_tree_level_ -1) - finest_level()));

  if ((std::abs(block_dim_.x() - block_dim_.y()) > 1.0e-7)   || (std::abs(block_dim_.x() - block_dim_.z()) > 1.0e-7)){
    std::cerr << "Warning: In boxm_scene::finest_cell_length, cells aren't cubical, returning length along x direction\n"
             << block_dim_.x() <<", " << block_dim_.y()<< ", " << block_dim_.x() << '\n';
  }

  return local_cell_length * block_dim_.x();
}

//: Return the length of cells at a given level in the scene
template <class T>
double  boxm_scene<T>::cell_length(unsigned level)
{
  double local_cell_length = 1.0/(double)(1<<((this->max_tree_level_ -1) - level));

  if ((std::abs(block_dim_.x() - block_dim_.y()) > 1.0e-7)   || (std::abs(block_dim_.x() - block_dim_.z()) > 1.0e-7)){
    std::cerr << "Warning: In boxm_scene::finest_cell_length, cells aren't cubical, returning length along x direction\n"
    << block_dim_.x() <<", " << block_dim_.y()<< ", " << block_dim_.x() << '\n';
  }

  return local_cell_length * block_dim_.x();
}

//: Return the length of finest-level cell in the scene. Iterates through blocks assuming they are all in memory
template <class T>
double  boxm_scene<T>::finest_cell_length_in_memory() const
{
  double local_cell_length = 1.0/(double)(1<<((this->max_tree_level_ -1) - finest_level_in_memory()));

  if ((std::abs(block_dim_.x() - block_dim_.y()) > 1.0e-7)   || (std::abs(block_dim_.x() - block_dim_.z()) > 1.0e-7))
    std::cerr << "Warning: In boxm_scene::finest_cell_length, cells aren't cubical, returning length along x direction\n";

  return local_cell_length * block_dim_.x();
}

//: Return the number of leaf nodes in the scene
template <class T>
unsigned long boxm_scene<T>::size()
{
  // iterate through the blocks requesting the finest level
  boxm_block_iterator<T > iter=this->const_iterator();
  iter.begin();
  unsigned long size = 0;
  while (!iter.end()) {
    if (this->load_block(iter.index().x(),iter.index().y(),iter.index().z()))
    {
      size += get_active_block()->get_tree()->size();
    }

    iter++;
  }

  return size;
}

template <class T>
void boxm_scene<T>::b_read(vsl_b_istream & is)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1:
      std::string xml="";
      vsl_b_read(is, xml);
      std::cout << xml << std::endl;
      boxm_scene_parser parser;
      parse_xml_string(xml, parser);
      break;
#if 0
    default:
      std::cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boxm_scene<T>&)\n"
               << "           Unknown version number "<< version << '\n';
      is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
#endif
  }
}

template <class T>
void boxm_scene<T>::b_write(vsl_b_ostream & s) const
{
  // create an XML stream for the parameters
  std::stringstream strm;
  boxm_scene<T> scene(this->lvcs(), this->origin(), this->block_dim(), this->world_dim());
  x_write(strm, scene, "scene");
  std::string str(strm.str());

  short v = boxm_scene<T>::version_no();
  vsl_b_write(s, v);
  // write the XML as char stream
  vsl_b_write(s, str);
}

template <class T>
void boxm_scene<T>::write_scene(std::string filename)
{
  std::string fullpath=scene_path_+ "/" + filename;
  std::ofstream os(fullpath.c_str());
  x_write(os, *this, "boxm_scene");
  this->filename_ = fullpath;
  os.close();
}

template <class T>
void boxm_scene<T>::write_scene(std::string path, std::string filename)
{
  std::string fullpath= path+ "/" + filename;
  std::ofstream os(fullpath.c_str());
  x_write(os, *this, "boxm_scene");
  this->filename_ = fullpath;
  os.close();
}

template <class T>
void boxm_scene<T>::load_scene(std::string filename)
{
  boxm_scene_parser parser;
  boxm_scene_base::load_scene(filename, parser);
  this->filename_ = filename;
  parse_config(parser);
}

template <class T>
void boxm_scene<T>::load_scene(boxm_scene_parser& parser)
{
  parse_config(parser);
}

template <class T>
void x_write(std::ostream &os, boxm_scene<T>& scene, std::string name)
{
  vsl_basic_xml_element scene_elm(name);
  scene_elm.x_write_open(os);

  vsl_basic_xml_element app_model(APP_MODEL_TAG);
  app_model.add_attribute("type", boxm_apm_types::app_model_strings[scene.appearence_model()]);
  app_model.x_write(os);

  vsl_basic_xml_element bin(MULTI_BIN_TAG);
  bin.add_attribute("value", scene.multi_bin()? 1 : 0);
  bin.x_write(os);

  vsl_basic_xml_element save_nodes(SAVE_INTERNAL_NODES_TAG);
  save_nodes.add_attribute("value", scene.save_internal_nodes()? 1 : 0);
  save_nodes.x_write(os);

  vsl_basic_xml_element save_platform_independent(SAVE_PLATFORM_INDEPENDENT_TAG);
  save_platform_independent.add_attribute("value", scene.save_platform_independent()? 1 : 0);
  save_platform_independent.x_write(os);

  vsl_basic_xml_element load_all_blocks(LOAD_ALL_BLOCKS_TAG);
  load_all_blocks.add_attribute("value", scene.load_all_blocks()? 1 : 0);
  load_all_blocks.x_write(os);

  vpgl_lvcs lvcs=scene.lvcs();
  x_write(os, lvcs, LVCS_TAG);
  x_write(os, scene.origin(), LOCAL_ORIGIN_TAG);
  x_write(os, scene.block_dim(), BLOCK_DIMENSIONS_TAG);

  vsl_basic_xml_element blocks(BLOCK_NUM_TAG);
  int x_dim, y_dim, z_dim;
  scene.block_num(x_dim, y_dim, z_dim);
  blocks.add_attribute("x_dimension", x_dim);
  blocks.add_attribute("y_dimension", y_dim);
  blocks.add_attribute("z_dimension", z_dim);
  blocks.x_write(os);

  vsl_basic_xml_element paths(SCENE_PATHS_TAG);
  paths.add_attribute("path", scene.path());
  paths.add_attribute("block_prefix", scene.block_prefix());
  paths.x_write(os);

  vsl_basic_xml_element tree(OCTREE_LEVELS_TAG);
  tree.add_attribute("max", (int) scene.max_level());
  tree.add_attribute("init", (int) scene.init_level());
  tree.x_write(os);

  scene_elm.x_write_close(os);
}

template <class T>
bool boxm_scene<T>::parse_config(boxm_scene_parser& parser)
{
  vpgl_lvcs lvcs;
  parser.lvcs(lvcs);
  vgl_vector_3d<unsigned> dims = parser.block_nums();

  lvcs_ = lvcs;
  origin_ =  parser.origin();
  block_dim_ = parser.block_dim();
  create_blocks(block_dim_, dims);

  parser.paths(scene_path_, block_pref_);
  app_model_ = boxm_apm_types::str_to_enum(parser.app_model().data());
  multi_bin_ = parser.multi_bin();
  save_internal_nodes_ =parser.save_internal_nodes();
  save_platform_independent_ = parser.save_platform_independent();
  load_all_blocks_ = parser.load_all_blocks();
  pinit_=parser.p_init();
#if 0
  std::cout << "Internal Nodes 1: " << save_internal_nodes_ << std::endl;
#endif
  parser.levels(max_tree_level_, init_tree_level_);
  return true;
}

template <class T>
bool boxm_scene<T>::parse_xml_string(std::string xml, boxm_scene_parser& parser)
{
  if (xml.size() == 0) {
    std::cerr << "XML string is empty\n";
    return false;
  }
  if (!parser.parseString(xml.data())) {
    std::cerr << XML_ErrorString(parser.XML_GetErrorCode()) << " at line "
             << parser.XML_GetCurrentLineNumber() << '\n';

    return false;
  }

  vpgl_lvcs lvcs;
  parser.lvcs(lvcs);
  vgl_vector_3d<unsigned> nums = parser.block_nums();

  lvcs_ = lvcs;
  origin_ =  parser.origin();
  block_dim_ = parser.block_dim();
  create_blocks(block_dim_, nums);

  parser.paths(scene_path_, block_pref_);
  app_model_ = boxm_apm_types::str_to_enum(parser.app_model().data());
  multi_bin_ = parser.multi_bin();
  pinit_=parser.p_init();
  load_all_blocks_ = parser.load_all_blocks();

  return true;
}

template <class T>
void boxm_scene<T>::clean_scene()
{
  boxm_block_iterator<T > iter=this->iterator();
  iter.begin();
  while (!iter.end()) {
    if (this->discover_block(iter.index().x(),iter.index().y(),iter.index().z())) {
      std::string filename=this->gen_block_path(iter.index().x(),iter.index().y(),iter.index().z());
      vpl_unlink(filename.c_str());
    }
    iter++;
  }
}

template <class T>
void boxm_scene<T>::print()
{
  boxm_block_iterator<T > iter=this->const_iterator();
  iter.begin();
  while (!iter.end()) {
    if (this->load_block(iter.index().x(),iter.index().y(),iter.index().z())) {
      std::cout << "Printing Block : " <<  iter.index() << std::endl;
      get_active_block()->get_tree()->print();
    }
    ++iter;
  }
}

//: Load all blocks in between min-max indices.
template <class T>
bool boxm_scene<T>::load_blocks(vgl_point_3d<int> min_idx, vgl_point_3d<int> max_idx)
{
  if (!valid_index(min_idx) || !valid_index(max_idx))
    return false;

  std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >  new_active_blocks;

  for (int i = min_idx.x(); i <= max_idx.x(); i++)
    for (int j = min_idx.y(); j <= max_idx.y(); j++)
      for (int k = min_idx.z(); k <= max_idx.z(); k++)
        new_active_blocks.insert(vgl_point_3d<int>(i,j,k));

  // Set unused blocks to null and load new blocks
  // this is to avoid rereading blocks that are already in memory
  std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >  blocks_to_unload;
  std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >  blocks_to_load;
  bvgl_point_3d_cmp<int>  cmp;
  std::set_difference(active_blocks_.begin(), active_blocks_.end(),
                     new_active_blocks.begin(), new_active_blocks.end(),
                     std::insert_iterator<std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> > >(blocks_to_unload, blocks_to_unload.begin()),
                     cmp);

  std::set_difference(new_active_blocks.begin(), new_active_blocks.end(),
                     active_blocks_.begin(), active_blocks_.end(),
                     std::insert_iterator<std::set<vgl_point_3d<int> , bvgl_point_3d_cmp<int> > > (blocks_to_load, blocks_to_load.begin()),
                     cmp);

  active_blocks_.clear();
  active_blocks_ = new_active_blocks;

  std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >::iterator unload_it = blocks_to_unload.begin();

  for (; unload_it!=blocks_to_unload.end(); unload_it++)
  {
    boxm_block<T>* block = blocks_((*unload_it).x(),(*unload_it).y(),(*unload_it).z());
    block->delete_tree();
    block->set_tree(nullptr);
  }

  std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >::iterator load_it = blocks_to_load.begin();

  for (; load_it!=blocks_to_load.end(); load_it++)
  {
    int block_i = (*load_it).x();
    int block_j = (*load_it).y();
    int block_k = (*load_it).z();

    if (blocks_(block_i,block_j,block_k)->get_tree() == nullptr)// read it from file
    {
      std::string block_path = gen_block_path(block_i,block_j,block_k);
      vsl_b_ifstream os(block_path);

      // if the binary block file is not found
      if (!os) {
        if (blocks_(block_i,block_j,block_k)->get_tree()==nullptr) {
          T* tree= new T(max_tree_level_,init_tree_level_);
          blocks_(block_i,block_j,block_k)->init_tree(tree);
        }
        return false;
      }
      blocks_(block_i,block_j,block_k)->b_read(os);
      os.close();
    }
  }

  return true;
}

//: Unload all blocks in between min-max indices.
template <class T>
bool boxm_scene<T>::unload_blocks(vgl_point_3d<int> min_idx, vgl_point_3d<int> max_idx)
{
  if (!valid_index(min_idx) || !valid_index(max_idx))
    return false;

  for (int i = min_idx.x(); i <= max_idx.x(); i++)
    for (int j = min_idx.y(); j <= max_idx.y(); j++)
      for (int k = min_idx.z(); k <= max_idx.z(); k++)
      {
        boxm_block<T>* block = blocks_(i,j,k);
        block->delete_tree();
        block->set_tree(nullptr);
      }
  return true;
}

//: Unload active blocks.
template <class T>
void boxm_scene<T>::unload_active_blocks() const
{
  // unload active block
  if (valid_index(active_block_))
  {
    boxm_block<T>* block = blocks_(active_block_.x(), active_block_.y(),active_block_.z());
    block->delete_tree();
    block->set_tree(nullptr);
    active_block_ = vgl_point_3d<int>(-1,-1,-1);
  }

  if (active_blocks_.size()==0)
    return;

  std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >::iterator unload_it = active_blocks_.begin();

  for (; unload_it!=active_blocks_.end(); unload_it++)
  {
    boxm_block<T>* block = blocks_((*unload_it).x(),(*unload_it).y(),(*unload_it).z());
    block->delete_tree();
    block->set_tree(nullptr);
  }
  active_blocks_.clear();

  return;
}


//: Locate all cells within a 3d region, which coordinates are given in scene coordinates.
//  Use with care --- blocks need to be unloaded by user to not create memory leaks
template <class T>
void boxm_scene<T>::leaves_in_region(vgl_box_3d<double> box, std::vector<boct_tree_cell<typename T::loc_type, typename T::datatype>* > &cells)
{
  vgl_box_3d<double> valid_box = vgl_intersection(get_world_bbox(),box);
  cells.clear();

  if (valid_box.is_empty()) {
    std::cout << "Warning in boxm_scene<T>::leaves_in_region: Region does not intersect scene\n";
    return;
  }

  vgl_point_3d<double> min_point = valid_box.min_point();
  vgl_point_3d<double> max_point = valid_box.max_point();

  // load blocks intersecting the region
  vgl_point_3d<int> min_idx;
  get_block_index(min_point, min_idx);

  vgl_point_3d<int> max_idx;
  get_block_index(max_point, max_idx);
#ifdef DEBUG
  std::cout << "Loading Region beteween blocks: " << max_idx << ", " << min_idx << std::endl;
#endif
  if (!load_blocks(min_idx, max_idx))
    return;

  // traverse blocks. for each block get the cells intersects the portion of the region contained in the block
  for (int k = min_idx.z(); k <= max_idx.z(); k++)
  {
    for (int j = min_idx.y(); j <= max_idx.y(); j++)
    {
      for (int i = min_idx.x(); i <= max_idx.x();i++)
      {
        boxm_block<T>* block = blocks_(i,j,k);
        vgl_box_3d<double> local_box = vgl_intersection(get_block_bbox(i,j,k),box);

        // subtract a little from the max point because octree cell are give by a half-closed interval [...).
        // if this is not done, the endpoint may be out of bounds
        vgl_box_3d<double> local_box_exclusive(local_box.min_point().x(), local_box.min_point().y(), local_box.min_point().z(),
                                               local_box.max_point().x()- 1e-7, local_box.max_point().y()- 1e-7,local_box.max_point().z()- 1e-7);
        T *tree = block->get_tree();
        if (!tree)
          continue;
        std::vector<boct_tree_cell<loc_type, datatype>* > temp_cells;
        temp_cells.clear();
        tree->locate_region_leaves_global(local_box_exclusive, temp_cells);
        cells.insert(cells.end(), temp_cells.begin(), temp_cells.end());
      }
    }
  }

  // FIXME: I should unload the blocks
  return;
}

//: Level of the cell containing a 3d region, which coordinates are given in scene coordinates.
template <class T>
short boxm_scene<T>::level_region(vgl_box_3d<double> box)
{
  vgl_box_3d<double> valid_box = vgl_intersection(get_world_bbox(),box);

  if (valid_box.is_empty()) {
    std::cout << "Warning in boxm_scene<T>::level_region: Region does not intersect scene\n";
    return -1;
  }

  vgl_point_3d<double> min_point = valid_box.min_point();
  vgl_point_3d<double> max_point = valid_box.max_point();

  // load blocks intersecting the region
  vgl_point_3d<int> min_idx;
  get_block_index(min_point, min_idx);

  vgl_point_3d<int> max_idx;
  get_block_index(max_point, max_idx);
//#ifdef DEBUG
  std::cout << "Loading Region beteween blocks: " << max_idx << ", " << min_idx << std::endl;
//#endif
  if (!load_blocks(min_idx, max_idx))
    return -1;

  short level = -1;

  // traverse blocks. for each block get the cells intersects the portion of the region contained in the block
  for (int k = min_idx.z(); k <= max_idx.z(); k++)
  {
    for (int j = min_idx.y(); j <= max_idx.y(); j++)
    {
      for (int i = min_idx.x(); i <= max_idx.x();i++)
      {
        boxm_block<T>* block = blocks_(i,j,k);
        vgl_box_3d<double> local_box = vgl_intersection(get_block_bbox(i,j,k),box);

        std::cout << ".....In tree with box dimension: " << get_block_bbox(i,j,k).width() << ", " << get_block_bbox(i,j,k).depth() << ", " << get_block_bbox(i,j,k).height()
            << "looking for box with dim: " << box.width() << ", " << box.depth() << ", " <<box.height() <<std::endl;

        // subtract a little from the max point because octree cell are give by a half-closed interval [...).
        // if this is not done, the endpoint may be out of bounds
        vgl_box_3d<double> local_box_exclusive(local_box.min_point().x(), local_box.min_point().y(), local_box.min_point().z(),
                                               local_box.max_point().x()- 1e-7, local_box.max_point().y()- 1e-7,local_box.max_point().z()- 1e-7);
        T *tree = block->get_tree();
        if (!tree)
          continue;

        boct_tree_cell<loc_type, datatype>* region_cell = tree->locate_region_global(local_box_exclusive);
        if (region_cell->level() > level)
          level = region_cell->level();

        std::cout << "level: " << level << box <<std::endl;

      }
    }
  }
  this->unload_active_blocks();
  return level;
}




//: Return all leaf cells between an inner box and an outter box
template <class T>
void boxm_scene<T>::leaves_data_in_region(vgl_box_3d<double> box, std::vector<boct_cell_data<typename T::loc_type, typename T::datatype> > &cell_data)
{
  vgl_box_3d<double> valid_box = vgl_intersection(get_world_bbox(),box);
  cell_data.clear();

  if (valid_box.is_empty()) {
    std::cout << "Warning in boxm_scene<T>::leaves_in_region: Region does not intersect scene\n";
    return;
  }

  vgl_point_3d<double> min_point = valid_box.min_point();
  vgl_point_3d<double> max_point = valid_box.max_point();

  // load blocks intersecting the region
  vgl_point_3d<int> min_idx;
  get_block_index(min_point, min_idx);

  vgl_point_3d<int> max_idx;
  get_block_index(max_point, max_idx);
#ifdef DEBUG
  std::cout << "Loading Region beteween blocks: " << max_idx << ", " << min_idx << std::endl;
#endif
  if (!load_blocks(min_idx, max_idx))
    return;

  // traverse blocks. for each block get the cells intersects the portion of the region contained in the block
  for (int k = min_idx.z(); k <= max_idx.z(); k++)
  {
    for (int j = min_idx.y(); j <= max_idx.y(); j++)
    {
      for (int i = min_idx.x(); i <= max_idx.x();i++)
      {
        boxm_block<T>* block = blocks_(i,j,k);
        vgl_box_3d<double> local_box = vgl_intersection(get_block_bbox(i,j,k),box);

        // subtract a little from the max point because octree cell are give by a half-closed interval [...).
        // if this is not done, the endpoint may be out of bounds
        vgl_box_3d<double> local_box_exclusive(local_box.min_point().x(), local_box.min_point().y(), local_box.min_point().z(),
                                               local_box.max_point().x()- 1e-7, local_box.max_point().y()- 1e-7,local_box.max_point().z()- 1e-7);
        T *tree = block->get_tree();
        if (!tree)
          continue;
        std::vector<boct_tree_cell<loc_type, datatype>* > temp_cells;
        temp_cells.clear();
        tree->locate_region_leaves_global(local_box_exclusive, temp_cells);

        typename std::vector<boct_tree_cell<loc_type, datatype>* >::iterator temp_cell_it = temp_cells.begin();
        for(; temp_cell_it!=temp_cells.end(); temp_cell_it++){
          cell_data.push_back(boct_cell_data<loc_type, datatype>(tree->global_centroid(*temp_cell_it),
                                                                (*temp_cell_it)->level(), (*temp_cell_it)->data()));
        }
      }
    }
  }
  unload_active_blocks();
  return;
}

//: Returns the data of all cells at a given level, that are contained in the specified region
template <class T>
void boxm_scene<T>::cell_data_in_region(vgl_box_3d<double> box, std::vector<boct_cell_data<loc_type, datatype> > &cell_data, unsigned level)
{
  vgl_box_3d<double> valid_box = vgl_intersection(get_world_bbox(),box);
  cell_data.clear();

  if (valid_box.is_empty()) {
    std::cout << "Warning in boxm_scene<T>::leaves_in_region: Region does not intersect scene\n";
    return;
  }

  vgl_point_3d<double> min_point = valid_box.min_point();
  vgl_point_3d<double> max_point = valid_box.max_point();

  // load blocks intersecting the region
  vgl_point_3d<int> min_idx;
  get_block_index(min_point, min_idx);

  vgl_point_3d<int> max_idx;
  get_block_index(max_point, max_idx);
#ifdef DEBUG
  std::cout << "Loading Region beteween blocks: " << max_idx << ", " << min_idx << std::endl;
#endif
  if (!load_blocks(min_idx, max_idx))
    return;

  // traverse blocks. for each block get the cells intersects the portion of the region contained in the block
  for (int k = min_idx.z(); k <= max_idx.z(); k++)
  {
    for (int j = min_idx.y(); j <= max_idx.y(); j++)
    {
      for (int i = min_idx.x(); i <= max_idx.x();i++)
      {
        boxm_block<T>* block = blocks_(i,j,k);
        vgl_box_3d<double> local_box = vgl_intersection(get_block_bbox(i,j,k),box);

        // subtract a little from the max point because octree cell are give by a half-closed interval [...).
        // if this is not done, the endpoint may be out of bounds
        vgl_box_3d<double> local_box_exclusive(local_box.min_point().x(), local_box.min_point().y(), local_box.min_point().z(),
                                               local_box.max_point().x()- 1e-7, local_box.max_point().y()- 1e-7,local_box.max_point().z()- 1e-7);
        T *tree = block->get_tree();
        if (!tree)
          continue;
        std::vector<boct_tree_cell<loc_type, datatype>* > temp_cells;
        temp_cells.clear();
        tree->locate_region_cells_global(local_box_exclusive, temp_cells, level);

        typename std::vector<boct_tree_cell<loc_type, datatype>* >::iterator temp_cell_it = temp_cells.begin();
        for(; temp_cell_it!=temp_cells.end(); temp_cell_it++){
          cell_data.push_back(boct_cell_data<loc_type, datatype>(tree->global_centroid(*temp_cell_it),
                                                                 (*temp_cell_it)->level(), (*temp_cell_it)->data()));
        }
      }
    }
  }
  unload_active_blocks();
  return;
}

//: Return all leaf cells between an inner box and an outter box. Use with care --- blocks need to be unloaded by user
template <class T>
void boxm_scene<T>::leaves_in_hollow_region(vgl_box_3d<double> outer_box, vgl_box_3d<double> inner_box, std::vector<boct_tree_cell<loc_type, datatype>* >& cells)
{
  if (!outer_box.contains(inner_box))
    return;

  // load blocks intersecting the region
  vgl_point_3d<double> min_point = outer_box.min_point();
  vgl_point_3d<int> min_idx;
  get_block_index(min_point, min_idx);

  vgl_point_3d<double> max_point = outer_box.max_point();
  vgl_point_3d<int> max_idx;
  get_block_index(max_point, max_idx);

  if (!load_blocks(min_idx, max_idx))
    return;

  // traverse blocks. for each block get the cells intersects the portion of the region contained in the block
  for (int k = min_idx.z(); k <= max_idx.z(); k++)
  {
    for (int j = min_idx.y(); j <= max_idx.y(); j++)
    {
      for (int i = min_idx.x(); i <= max_idx.x();i++)
      {
        boxm_block<T>* block = blocks_(i,j,k);
        vgl_box_3d<double> local_box = vgl_intersection(get_block_bbox(i,j,k),outer_box);

        // subtract a little from the max point because octree cell are give by a half-closed interval [...).
        // if this is not done, the endpoint may be out of bounds
        vgl_box_3d<double> local_outer_box_exclusive(local_box.min_point().x(), local_box.min_point().y(), local_box.min_point().z(),
                                                     local_box.max_point().x()- 1e-7, local_box.max_point().y()- 1e-7,local_box.max_point().z()- 1e-7);

        vgl_box_3d<double> local_inner_box_exclusive = vgl_intersection(local_outer_box_exclusive,inner_box);

        T *tree = block->get_tree();
        if (!tree)
          continue;
        std::vector<boct_tree_cell<loc_type, datatype>* > temp_cells;
        temp_cells.clear();
        tree->locate_leaves_in_hollow_region_global(local_outer_box_exclusive, local_inner_box_exclusive, temp_cells);
        cells.insert(cells.end(), temp_cells.begin(), temp_cells.end());
      }
    }
  }

  // FIXME: I should unload the blocks
  return;
}

//: Locates and modifies the value of all cells within a 3d region, which coordinates are given in scene coordinates
template <class T>
void boxm_scene<T>::change_leaves_in_region(vgl_box_3d<double> box, const typename T::datatype &cell_data)
{
  // load blocks intersecting the region
  vgl_point_3d<double> min_point = box.min_point();
  vgl_point_3d<int> min_idx;
  get_block_index(min_point, min_idx);

  vgl_point_3d<double> max_point = box.max_point();
  vgl_point_3d<int> max_idx;
  get_block_index(max_point, max_idx);

  if (!load_blocks(min_idx, max_idx))
    return;

  // traverse blocks. for each block get the cells intersects the portion of the region contained in the block
  for (int k = min_idx.z(); k <= max_idx.z(); k++)
  {
    for (int j = min_idx.y(); j <= max_idx.y(); j++)
    {
      for (int i = min_idx.x(); i <= max_idx.x();i++)
      {
        boxm_block<T>* block = blocks_(i,j,k);
        vgl_box_3d<double> local_box = vgl_intersection(get_block_bbox(i,j,k),box);

        // subtract a little from the max point because octree cell are give by a half-closed interval [...).
        // if this is not done, the endpoint may be out of bounds
        vgl_box_3d<double> local_box_exclusive(local_box.min_point().x(), local_box.min_point().y(), local_box.min_point().z(),
                                               local_box.max_point().x()- 1e-7, local_box.max_point().y()- 1e-7,local_box.max_point().z()- 1e-7);
        T *tree = block->get_tree();
        if (!tree)
          continue;

        tree->change_leaves_in_global_region_leaves_global(local_box_exclusive, cell_data);

        std::string path = gen_block_path(i,j,k);
        vsl_b_ofstream os(path);
        block->b_write(os, save_internal_nodes_, save_platform_independent_);
      }
    }
  }

  unload_active_blocks();
  return;
}

//: Locates and modifies the value of all cells within a 3d region, which coordinates are given in scene coordinates
template <class T>
void boxm_scene<T>::change_leaves_in_regions(std::vector<vgl_box_3d<double> > boxes, const std::vector<typename T::datatype> &all_data)
{
  if (boxes.size()!= all_data.size()) {
    std::cerr << "Error in boxm_scene<T>::change_leaves_in_regions --> Input vectors don't have the same size\n";
    return;
  }

  for (unsigned bi = 0; bi < boxes.size(); bi++)
  {
    const vgl_box_3d<double> &box = boxes[bi];
    const datatype &cell_data=all_data[bi];
    // load blocks intersecting the region
    vgl_point_3d<double> min_point = box.min_point();
    vgl_point_3d<int> min_idx;
    get_block_index(min_point, min_idx);

    vgl_point_3d<double> max_point = box.max_point();
    vgl_point_3d<int> max_idx;
    get_block_index(max_point, max_idx);

    if (!load_blocks(min_idx, max_idx))
      return;

    // traverse blocks. for each block get the cells intersects the portion of the region contained in the block
    for (int k = min_idx.z(); k <= max_idx.z(); k++)
    {
      for (int j = min_idx.y(); j <= max_idx.y(); j++)
      {
        for (int i = min_idx.x(); i <= max_idx.x();i++)
        {
          boxm_block<T>* block = blocks_(i,j,k);
          vgl_box_3d<double> local_box = vgl_intersection(get_block_bbox(i,j,k),box);

          // subtract a little from the max point because octree cell are give by a half-closed interval [...).
          // if this is not done, the endpoint may be out of bounds
          vgl_box_3d<double> local_box_exclusive(local_box.min_point().x(), local_box.min_point().y(), local_box.min_point().z(),
                                                 local_box.max_point().x()- 1e-7, local_box.max_point().y()- 1e-7,local_box.max_point().z()- 1e-7);
          T *tree = block->get_tree();
          if (!tree)
            continue;

          tree->change_leaves_in_global_region_leaves_global(local_box_exclusive, cell_data);

          std::string path = gen_block_path(i,j,k);
          vsl_b_ofstream os(path);
          block->b_write(os, save_internal_nodes_, save_platform_independent_);
        }
      }
    }
  }

  unload_active_blocks();
  return;
}


//: Locate point
template <class T>
boct_tree_cell<typename T::loc_type, typename T::datatype>* boxm_scene<T>::locate_point_in_memory(vgl_point_3d<double> &p, unsigned level)
{
  // get the indices for the block containing this point
  vgl_point_3d<int> block_idx;
  if (!get_block_index(p, block_idx))
    return nullptr;

#ifdef DEBUG
  std::cout << "Requesting blocks : "<< block_idx<< '\n'
           << "Active blocks :" << std::endl;
  std::set<vgl_point_3d<int>, bvgl_point_3d_cmp<int> >::iterator it = active_blocks_.begin();
  for (; it!=active_blocks_.end(); it++) {
    std::cout << *it << std::endl;
    boxm_block<T > *block = get_block(*it);
    if (!block)
      std::cerr << " NULL block\n";
    T *tree = block->get_tree();
    if (!tree)
      std::cerr << " NULL tree\n";
  }
#endif

  // get the block, if block is not already in memory, return null
  boxm_block<T>* block = blocks_(block_idx.x(), block_idx.y(), block_idx.z());
  if (!block) {
    std::cerr << "In locate_point_in_memory: NULL block\n";
    return nullptr;
  }
  T *tree = block->get_tree();
  if (!tree) {
    std::cerr << "In locate_point_in_memory: NULL tree\n";
    return nullptr;
  }
  return tree->locate_point_global(p, (short)level);
}

/************************************************ BOXM_BLOCK_ITERATOR *******************************************/

template <class T>
boxm_block_iterator<T>& boxm_block_iterator<T>::begin()
{
  i_=j_=k_=0;
  return *this;
}

template <class T>
bool boxm_block_iterator<T>::end()
{
  int x,y,z;
  scene_->block_num(x,y,z);

  // if ((i_==x-1) && (j_==y-1) && (k_==z-1))
  bool end = (k_==z || k_ == -1);

  //:If end, release memory
  if (end)
    scene_->unload_active_blocks();

  return end;
}

template <class T>
boxm_block_iterator<T>& boxm_block_iterator<T>::operator=(const boxm_block_iterator<T>& that)
{
  this->i_ = that.i_;
  this->j_ = that.j_;
  this->k_ = that.k_;
  return *this;
}

template <class T>
bool boxm_block_iterator<T>::operator==(const boxm_block_iterator<T>& that)
{
  return (this->i_ == that.i_) && (this->j_ == that.j_) && (this->k_ == that.k_) && (this->scene_ == that.scene_);
}

template <class T>
bool boxm_block_iterator<T>::operator!=(const boxm_block_iterator<T>& that)
{
  return (this->i_ != that.i_) || (this->j_ != that.j_) == (this->k_ != that.k_) || (this->scene_ != that.scene_);
}

template <class T>
boxm_block_iterator<T>& boxm_block_iterator<T>::operator++()
{
  int x,y,z;
  scene_->block_num(x,y,z);

  if (++i_==x) {
    i_=0;
    if (++j_==y) {
      j_=0;
      ++k_;
    }
  }
  return *this;
}

template <class T>
boxm_block_iterator<T> boxm_block_iterator<T>::operator++(int)
{
  boxm_block_iterator<T> old = *this;
  ++*this;
  return old;
}

template <class T>
boxm_block_iterator<T>& boxm_block_iterator<T>::operator--()
{
  int x,y,z;

  scene_->block_num(x,y,z);
  if (--i_==-1) {
    i_=0;
    if (--j_==-1) {
      j_=0;
      k_--;
    }
  }
  return *this;
}

template <class T>
boxm_block<T>* boxm_block_iterator<T>::operator*()
{
  return scene_->get_block(i_,j_,k_);
}

template <class T>
boxm_block<T>*  boxm_block_iterator<T>::operator->()
{
  return scene_->get_block(i_,j_,k_);
}

/************************************************ BOXM_CELL_ITERATOR *******************************************/
template <class T>
boxm_cell_iterator<T>& boxm_cell_iterator<T>::begin(bool use_internal_cells)
{
  use_internal_cells_= use_internal_cells;

  // unload all blocks from memory
  block_iterator_.scene_->unload_active_blocks();
  block_iterator_.begin();

  // load active block using function pointer, retrieve pointer to all cells
  (block_iterator_.scene_->*block_loading_func_)(block_iterator_.index().x(),block_iterator_.index().y(),block_iterator_.index().z());
  T *tree = (*block_iterator_)->get_tree();
  assert(tree != nullptr);
  if(use_internal_cells_)
    cells_=tree->all_cells();
  else
    cells_ = tree->leaf_cells();
  cells_iterator_ = cells_.begin();

  std::cout << "Cell iterator: # of cells: " << cells_.size() << std::endl;

  return *this;
}

template <class T>
bool boxm_cell_iterator<T>::end()
{
  bool end = false;

  end = cells_.empty() || (block_iterator_.end() && (cells_iterator_ == cells_.end()));

  if (end)
    block_iterator_.scene_->unload_active_blocks();

  return end;
}

template <class T>
boxm_cell_iterator<T>& boxm_cell_iterator<T>::operator=(const boxm_cell_iterator<T>& /*that*/)
{
  std::cerr << "assignment operator not yet implemented\n";
  return *this;
}

template <class T>
bool boxm_cell_iterator<T>::operator==(const boxm_cell_iterator<T>& that)
{
  return (this->block_iterator_ == that.block_iterator_) && (this->cells_iterator_ == that.cells_iterator_);
}

template <class T>
bool boxm_cell_iterator<T>::operator!=(const boxm_cell_iterator<T>& that)
{
  return (this->block_iterator_ != that.block_iterator_) && (this->cells_iterator_ != that.cells_iterator_);
}

//: Prefix operator.
//  When the end of the block is reached, it writes the block to disk and loads the next one, unless the read_only flag was set
template <class T>
boxm_cell_iterator<T>& boxm_cell_iterator<T>::operator++()
{
  if (++cells_iterator_ == cells_.end())
  {
    if (!read_only_)
      block_iterator_.scene_->write_active_block(false);

    ++block_iterator_;

    if (!block_iterator_.end())
    {
      // load active block using function pointer, retrieve pointer to all cells
      (block_iterator_.scene_->*block_loading_func_)(block_iterator_.index().x(),block_iterator_.index().y(),block_iterator_.index().z());
      T *tree = (*block_iterator_)->get_tree();
      assert(tree != nullptr);

      if(use_internal_cells_)
        cells_=tree->all_cells();
      else
        cells_ = tree->leaf_cells();

      cells_iterator_ = cells_.begin();
    }
  }
  return *this;
}

template <class T>
boxm_cell_iterator<T> boxm_cell_iterator<T>::operator+=(unsigned const &rhs)
{
  // is there a more efficient way to do this whitout messing all the iterator up?
  for (unsigned i =0; i<rhs; ++i)
    ++(*this);

  return *this;
}


template <class T>
boct_tree_cell<typename boxm_cell_iterator<T>::loc_type, typename boxm_cell_iterator<T>::datatype>* boxm_cell_iterator<T>::operator*()
{
  return *cells_iterator_;
}

template <class T>
boct_tree_cell<typename boxm_cell_iterator<T>::loc_type, typename boxm_cell_iterator<T>::datatype>*  boxm_cell_iterator<T>::operator->()
{
  return *cells_iterator_;
}

template<class T>
vgl_point_3d<double> boxm_cell_iterator<T>::global_origin()
{
  return (*block_iterator_)->get_tree()->global_origin(*cells_iterator_);
}

template<class T>
vgl_point_3d<double> boxm_cell_iterator<T>::global_centroid()
{
  return (*block_iterator_)->get_tree()->global_centroid(*cells_iterator_);
}

//: Return the global length of this cell
template<class T>
double boxm_cell_iterator<T>::length()
{
  return (*block_iterator_)->get_tree()->cell_length(*cells_iterator_)*(*block_iterator_)->bounding_box().width();
}

/******************************************* I/ O *******************************************************/

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_scene<T> const &scene)
{
  scene.b_write(os);
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boxm_scene<T> const * &scene)
{
  scene->b_write(os);
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_scene<T> &scene)
{
  scene.b_read(is);
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boxm_scene<T> *&scene)
{
  scene->b_read(is);
}

#define BOXM_SCENE_INSTANTIATE(T) \
template class boxm_scene<T >; \
template void x_write(std::ostream&, boxm_scene<T >&, std::string); \
template void vsl_b_write(vsl_b_ostream & os, boxm_scene<T > const &scene); \
template void vsl_b_write(vsl_b_ostream & os, boxm_scene<T > const * &scene); \
template void vsl_b_read(vsl_b_istream & is, boxm_scene<T >  &scene); \
template void vsl_b_read(vsl_b_istream & is, boxm_scene<T > * &scene)

#define BOXM_BLOCK_ITERATOR_INSTANTIATE(T) \
template class boxm_block_iterator<T >; \
template class boxm_cell_iterator<T >

#endif
