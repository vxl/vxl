#ifndef boct_tree_txx_
#define boct_tree_txx_

#include "boct_tree.h"
#include <vgl/vgl_box_3d.h>
#include <vcl_iostream.h>

//; constructor initializes an empty tree
template <class T_loc,class T>
boct_tree<T_loc,T>::boct_tree(short max_level, short init_levels): max_level_(max_level)
{
  // root is allocated at (max_level_-1) with code [0,0,0]
  boct_loc_code<T_loc> code;
  if (max_level_>0)
  {
    code.set_code(0,0,0);
    code.set_level(max_level_-1);
    root_=new boct_tree_cell<T_loc,T>( code);
  }
  init_levels--;
  while (init_levels > 0) {
    vcl_vector<boct_tree_cell<T_loc,T>*> cells;
    cells = leaf_cells();
    for (unsigned i=0; i<cells.size(); i++) {
      boct_tree_cell<T_loc,T>* c = static_cast<boct_tree_cell<T_loc,T>*>(cells[i]);
      c->print();
      c->split();
      }
    init_levels--;
  }
}
template <class T_loc,class T>
boct_tree<T_loc,T>::~boct_tree() 
{
  vcl_vector<boct_tree_cell<T_loc,T>*> v;
  if (root_)
    if (root_->is_leaf()) {
      delete root_;
    } else {
      root_->delete_children();
      delete root_;
    }
}

template <class T_loc,class T>
boct_tree_cell<T_loc,T>* boct_tree<T_loc,T>::locate_point(const vgl_point_3d<double>& p)
{
  short curr_level=max_level_-1;
  // convert point to location code.
  boct_loc_code<T_loc>* loccode_=new boct_loc_code<T_loc>(p, max_level_);
#if 0
  // check to see if point is contained in the octree
  if (!root_->code_.isequal(loccode_,curr_level))
    return NULL;
#endif
  // temporary pointer to traverse
  boct_tree_cell<T_loc,T>* curr_cell=root_;

  while (curr_cell->children()&& curr_level>0)
  {
    short index_child=loccode_->child_index(curr_level);
    curr_cell=curr_cell->children()+index_child;
    --curr_level;
  }
  // delete the location code constructed
  delete loccode_;
  return curr_cell;
}

template <class T_loc,class T>
boct_tree_cell<T_loc,T>* boct_tree<T_loc,T>::locate_point_at_level(const vgl_point_3d<double>& p, short level)
{
  short curr_level=max_level_-1;
  // convert point to location code.
  boct_loc_code<T_loc>* loccode_=new boct_loc_code<T_loc>(p, max_level_);

  // check to see if point is contained in the octree
  if (!root_->code_.isequal(loccode_,curr_level))
    return NULL;

  // temporary pointer to traverse
  boct_tree_cell<T_loc,T>* curr_cell=root_;

  while (curr_cell->children()&& curr_level>level)
  {
    short child_index=loccode_->child_index(curr_level);
    curr_cell=curr_cell->children()+child_index;
    --curr_level;
  }
  // delete the location code constructed
  delete loccode_;
  return curr_cell;
}

template <class T_loc,class T>
boct_tree_cell<T_loc,T>* boct_tree<T_loc,T>::locate_region(const vgl_box_3d<double>& r)
{
  boct_loc_code<T_loc>* mincode=new boct_loc_code<T_loc>(r.min_point(), max_level_);
  boct_loc_code<T_loc>* maxcode=new boct_loc_code<T_loc>(r.max_point(), max_level_);

  boct_loc_code<T_loc>* xorcode=mincode->XOR(maxcode);

  short level_x=max_level_-1;
  short level_y=max_level_-1;
  short level_z=max_level_-1;
  while (!(xorcode->x_loc_&(1<<level_x))&& level_x) level_x--;
  while (!(xorcode->y_loc_&(1<<level_y))&& level_y>level_x) level_y--;
  while (!(xorcode->z_loc_&(1<<level_z))&& level_z>level_y) level_z--;

  return locate_point_at_level(r.min_point(),level_z);
}

template <class T_loc,class T>
bool boct_tree<T_loc,T>::split()
{
  return root_->split();
}

template <class T_loc,class T>
vcl_vector<boct_tree_cell<T_loc,T>*> boct_tree<T_loc,T>::leaf_cells()
{
  vcl_vector<boct_tree_cell<T_loc,T>*> v;
  if (root_)
    if (root_->is_leaf()) {
      v.push_back(root_);
    }
    else {
      root_->leaf_children(v);
    }
  return v;
}

template <class T_loc,class T>
void boct_tree<T_loc,T>::print()
{
  vcl_cout << "Octree Max Level=" << max_level_ << vcl_endl;
  root_->print();
}

template <class T_loc,class T>
void boct_tree<T_loc,T>::b_write(vsl_b_ostream & os)
{
  // write header info
  const short io_version_no = 1;

  vsl_b_write(os, version_no());
  vsl_b_write(os, max_level_);
  if (root_)
    vsl_b_write(os, *root_);
}

template <class T_loc,class T>
void boct_tree<T_loc,T>::b_read(vsl_b_istream & is)
{
  // read header info
  if (!is) return;

  short v;
  vsl_b_read(is, v);
  switch (v)
  {
   case (1):
     //short max_level;
     vsl_b_read(is, max_level_);
     root_ = new boct_tree_cell<T_loc,T>();
     vsl_b_read(is, *root_, (boct_tree_cell<T_loc,T>*)0);
     break;
   default:
     vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boct_tree<T_loc,T>&)\n"
              << "           Unknown version number "<< v << '\n';
     is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
     return;
  }
}

#define BOCT_TREE_INSTANTIATE(T_loc,T) \
template class boct_tree<T_loc,T >; \
template void vsl_b_write(vsl_b_ostream & os, boct_tree<T_loc,T >&); \
template void vsl_b_read(vsl_b_istream & is, boct_tree<T_loc,T >&)

#endif
