#ifndef boct_tree_cell_txx_
#define boct_tree_cell_txx_
//:
// \file
#include "boct_tree_cell.h"

#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for std::malloc and std::free

template<class T_loc,class T_data>
boct_tree_cell<T_loc,T_data>::boct_tree_cell(const boct_loc_code<T_loc>& code)
{
  code_=code;
  parent_=NULL;
  children_=NULL;
  vis_node_=0;
}

template<class T_loc,class T_data>
bool boct_tree_cell<T_loc,T_data>::is_leaf()
{
  return children_== NULL;
}

template<class T_loc,class T_data>
const boct_loc_code<T_loc>
boct_tree_cell<T_loc,T_data>::get_code()
{
  return code_;
}

template<class T_loc,class T_data>
boct_tree_cell<T_loc,T_data>::~boct_tree_cell()
{
  delete_children();
  if (vis_node_) {
    delete vis_node_;
    vis_node_ = NULL;
  }
}

template<class T_loc,class T_data>
boct_tree_cell<T_loc,T_data>* boct_tree_cell<T_loc,T_data>::clone(boct_tree_cell<T_loc,T_data>* parent)
{
  boct_tree_cell<T_loc,T_data>* cell = new boct_tree_cell<T_loc,T_data>(this->code_);
  cell->data_ = this->data();
  cell->parent_ = parent;
  if (this->is_leaf())
    cell->children_=NULL;
  else {
    cell->split();
    for (unsigned i=0; i<8; i++) {
      boct_tree_cell<T_loc,T_data>* c=this->children_[i].clone(cell);
      cell->children_[i] = *c;
      c->children_=NULL;
      delete c;
    }
  }
  return cell;
}

template<class T_loc,class T_data>
void boct_tree_cell<T_loc,T_data>::delete_children()
{
  if (!is_leaf()) {
    // first delete all non-leaf children before deleting the cell
    for (unsigned i=0; i<8; i++) {
      children_[i].delete_children();
#ifdef DEBUG
      vcl_cout << "Deleting children" << vcl_endl;
#endif
    }
    delete [] children_;
    children_=NULL;
    if (vis_node_) {
      delete vis_node_;
      vis_node_ = NULL;
    }
  }
}

// this is always going down the tree
template<class T_loc,class T_data>
boct_tree_cell<T_loc,T_data>* boct_tree_cell<T_loc,T_data>::traverse_to_level(boct_loc_code<T_loc> * code, short level)
{
  if (level<0)
    return NULL;
  boct_tree_cell<T_loc,T_data>* curr_cell=this;
  while (level<curr_cell->level() && curr_cell->children())
  {
    //boct_loc_code tempcode=curr_cell->get_code();
    short child_index=code->child_index(curr_cell->level());
    curr_cell=curr_cell->children()+child_index;
  }
  return curr_cell;
}

// this is always going down the tree
template<class T_loc,class T_data>
boct_tree_cell<T_loc,T_data>* boct_tree_cell<T_loc,T_data>::traverse(boct_loc_code<T_loc> & code)
{
  if (code.level<0)
    return NULL;
  boct_tree_cell<T_loc,T_data>* curr_cell=this;
  while (code.level<curr_cell->level() && !curr_cell->is_leaf())
  {
    //boct_loc_code tempcode=curr_cell->get_code();
    short child_index=code.child_index(curr_cell->level());
    curr_cell=curr_cell->children()+child_index;
  }
  return curr_cell;
}

// this is always going down the tree
template<class T_loc,class T_data>
boct_tree_cell<T_loc,T_data>* boct_tree_cell<T_loc,T_data>::traverse_force(boct_loc_code<T_loc> & code)
{
  if (code.level<0)
    return NULL;
  boct_tree_cell<T_loc,T_data>* curr_cell=this;
  while (code.level<curr_cell->level() && !curr_cell->is_leaf())
  {
    boct_loc_code<T_loc> curr_code=curr_cell->get_code();

    short child_bit= 1 << (curr_cell->level()-1);

    short index=0;
    if (code.x_loc_-curr_code.x_loc_>=child_bit)
      index+=1;
    if (code.y_loc_-curr_code.y_loc_>=child_bit)
      index+=2;
    if (code.z_loc_-curr_code.z_loc_>=child_bit)
      index+=4;
#if 0
    short child_index=code.child_index(curr_cell->level());
#endif
    curr_cell=curr_cell->children()+index;
  }
  return curr_cell;
}

template<class T_loc,class T_data>
bool boct_tree_cell<T_loc,T_data>::split()
{
  // create new children if there is none
  if (is_leaf()) {
    children_ = (boct_tree_cell<T_loc,T_data>*) new boct_tree_cell<T_loc,T_data>[8];
    short child_level = this->level()-1;

    // make sure that it does not go below level 0, which is the min possible level
    if (child_level < 0) {
      vcl_cout << "boct_tree_cell: Cannot split the cell, already at the min level" << vcl_endl;
      return false;
    }
    for (unsigned i=0; i<8; i++) {
      children_[i].code_ = code_.child_loc_code(i, child_level);
      children_[i].code_.level = child_level;
      children_[i].set_parent(this);
      children_[i].data_ = this->data_;
      children_[i].children_ = 0;
    }
    return true;
  }
  else
    return false;
}

template<class T_loc,class T_data>
bool boct_tree_cell<T_loc,T_data>::split(T_data data)
{
  // split the cell first
  if (split()) {
    for (unsigned i=0; i<8; i++) {
      children_[i].data_ = data;
    }
    return true;
  }
  else
    return false;
}

template<class T_loc,class T_data>
boct_tree_cell<T_loc,T_data>* boct_tree_cell<T_loc,T_data>::get_common_ancestor(short binarydiff)
{
  short curr_level=this->level();
  boct_tree_cell<T_loc,T_data>* curr_cell=this;
  while (binarydiff & (1<< (curr_level)))
  {
    curr_cell=curr_cell->parent_;
    curr_level++;
  }
  return curr_cell;
}

template<class T_loc,class T_data>
void  boct_tree_cell<T_loc,T_data>::find_neighbors(boct_face_idx face,
                                                   vcl_vector<boct_tree_cell<T_loc,T_data>*> & neighbors,
                                                   short root_level)
{
  short cellsize=1<<this->level();

  switch (face)
  {
    case NONE:
      break;

    case X_LOW:
    {
      short xlowcode=this->code_.x_loc_-(short)(1);
      if (xlowcode<0)
        return;
      short diff=this->code_.x_loc_^xlowcode;

      boct_loc_code<T_loc> neighborcode;
      neighborcode.set_code(xlowcode,this->code_.y_loc_,this->code_.z_loc_);

      boct_tree_cell<T_loc,T_data>* commonancestor=get_common_ancestor(diff);

      if (commonancestor==NULL)
        return;
      // at the same or greater level ( towards the root)
      boct_tree_cell<T_loc,T_data>* neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level());

      if (!neighborcell->children())
        neighbors.push_back(neighborcell);
      else
      {
        // TODO code to get the cells which are further down the tree.
        // This is naive implementation of getting all the leaf nodes from a node and
        // test each of them if  they are neighbors.
        vcl_vector<boct_tree_cell<T_loc,T_data>*> leafnodes;
        neighborcell->leaf_children(leafnodes);

        for (unsigned int i=0;i<leafnodes.size();i++)
        {
          short leafnode_cellsize=1<<leafnodes[i]->level();
          if (leafnodes[i]->code_.x_loc_+leafnode_cellsize==this->code_.x_loc_)
            neighbors.push_back(leafnodes[i]);
        }
      }

      break;
    }
    case X_HIGH:
    {
      if ((this->code_.x_loc_+cellsize)>=(1<<(root_level)))
        return ;
      short xhighcode=this->code_.x_loc_+cellsize;
      short diff=this->code_.x_loc_^xhighcode;
      // set the code for the neighboring point
      boct_loc_code<T_loc> neighborcode;
      neighborcode.set_code(xhighcode,this->code_.y_loc_,this->code_.z_loc_);

      boct_tree_cell<T_loc,T_data>* commonancestor=get_common_ancestor(diff);

      if (commonancestor==NULL)
        return;
      // at the same or greater level ( towards the root)
      boct_tree_cell<T_loc,T_data>* neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level());

      if (!neighborcell->children())
        neighbors.push_back(neighborcell);
      else
      {
        // TODO code to get the cells which are further down the tree.
        // This is naive implementation of getting all the leaf nodes from a node and
        // test each of them if  they are neighbors.
        vcl_vector<boct_tree_cell<T_loc,T_data>*> leafnodes;
        neighborcell->leaf_children(leafnodes);

        for (unsigned int i=0;i<leafnodes.size();i++)
        {
          if (leafnodes[i]->code_.x_loc_==xhighcode)
            neighbors.push_back(leafnodes[i]);
        }
      }

      break;
    }
    case Y_LOW:
    {
      short ylowcode=this->code_.y_loc_-(short)(1);
      if (ylowcode<0)
        return;
      short diff=this->code_.y_loc_^ylowcode;

      boct_loc_code<T_loc> neighborcode;
      neighborcode.set_code(this->code_.x_loc_,ylowcode,this->code_.z_loc_);

      boct_tree_cell<T_loc,T_data>* commonancestor=get_common_ancestor(diff);

      if (commonancestor==NULL)
        return;
      // at the same or greater level ( towards the root)
      boct_tree_cell<T_loc,T_data>* neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level());

      if (!neighborcell->children())
        neighbors.push_back(neighborcell);
      else
      {
        // TODO code to get the cells which are further down the tree.
        // This is naive implementation of getting all the leaf nodes from a node and
        // test each of them if  they are neighbors.
        vcl_vector<boct_tree_cell<T_loc,T_data>*> leafnodes;
        neighborcell->leaf_children(leafnodes);

        for (unsigned int i=0;i<leafnodes.size();i++)
        {
          short leafnode_cellsize=1<<leafnodes[i]->level();
          if (leafnodes[i]->code_.y_loc_+leafnode_cellsize==this->code_.y_loc_)
            neighbors.push_back(leafnodes[i]);
        }
      }
      break;
    }
    case Y_HIGH:
    {
      if ((this->code_.y_loc_+cellsize)>=(1<<(root_level)))
        return ;
      short yhighcode=this->code_.y_loc_+cellsize;
      short diff=this->code_.y_loc_^yhighcode;
      // set the code for the neighboring point
      boct_loc_code<T_loc> neighborcode;
      neighborcode.set_code(this->code_.x_loc_,yhighcode,this->code_.z_loc_);

      boct_tree_cell<T_loc,T_data>* commonancestor=get_common_ancestor(diff);

      if (commonancestor==NULL)
        return;
      // at the same or greater level ( towards the root)
      boct_tree_cell<T_loc,T_data>* neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level());

      if (!neighborcell->children())
        neighbors.push_back(neighborcell);

      else
      {
        // TODO code to get the cells which are further down the tree.
        // This is naive implementation of getting all the leaf nodes from a node and
        // test each of them if  they are neighbors.
        vcl_vector<boct_tree_cell<T_loc,T_data>*> leafnodes;
        neighborcell->leaf_children(leafnodes);

        for (unsigned int i=0;i<leafnodes.size();i++)
        {
          if (leafnodes[i]->code_.y_loc_==yhighcode)
            neighbors.push_back(leafnodes[i]);
        }
      }
      break;
    }
    case Z_LOW:
    {
      short zlowcode=this->code_.z_loc_-(short)(1);
      if (zlowcode<0)
        return;
      short diff=this->code_.z_loc_^zlowcode;

      boct_loc_code<T_loc> neighborcode;
      neighborcode.set_code(this->code_.x_loc_,this->code_.y_loc_,zlowcode);

      boct_tree_cell<T_loc,T_data>* commonancestor=get_common_ancestor(diff);

      if (commonancestor==NULL)
        return;
      // at the same or greater level ( towards the root)
      boct_tree_cell<T_loc,T_data>* neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level());

      if (!neighborcell->children())
        neighbors.push_back(neighborcell);
      else
      {
        // TODO code to get the cells which are further down the tree.
        // This is naive implementation of getting all the leaf nodes from a node and
        // test each of them if  they are neighbors.
        vcl_vector<boct_tree_cell<T_loc,T_data>*> leafnodes;
        neighborcell->leaf_children(leafnodes);

        for (unsigned int i=0;i<leafnodes.size();i++)
        {
          short leafnode_cellsize=1<<leafnodes[i]->level();
          if (leafnodes[i]->code_.z_loc_+leafnode_cellsize==this->code_.z_loc_)
            neighbors.push_back(leafnodes[i]);
        }
      }
      break;
    }
    case Z_HIGH:
    {
      if ((this->code_.z_loc_+cellsize)>=(1<<(root_level)))
        return ;
      short zhighcode=this->code_.z_loc_+cellsize;
      short diff=this->code_.z_loc_^zhighcode;
      // set the code for the neighboring point
      boct_loc_code<T_loc> neighborcode;
      neighborcode.set_code(this->code_.x_loc_,this->code_.y_loc_,zhighcode);

      boct_tree_cell<T_loc,T_data>* commonancestor=get_common_ancestor(diff);

      if (commonancestor==NULL)
        return;
      // at the same or greater level ( towards the root)
      boct_tree_cell<T_loc,T_data>* neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level());

      if (!neighborcell->children())
        neighbors.push_back(neighborcell);

      else
      {
        // TODO code to get the cells which are further down the tree.
        // This is naive implementation of getting all the leaf nodes from a node and
        // test each of them if  they are neighbors.
        vcl_vector<boct_tree_cell<T_loc,T_data>*> leafnodes;
        neighborcell->leaf_children(leafnodes);

        for (unsigned int i=0;i<leafnodes.size();i++)
        {
          if (leafnodes[i]->code_.z_loc_==zhighcode)
            neighbors.push_back(leafnodes[i]);
        }
      }
      break;
    }
    default:
      break;
  }
}

template<class T_loc,class T_data>
bool  boct_tree_cell<T_loc,T_data>::find_neighbor(boct_face_idx face,
                                                  boct_tree_cell<T_loc,T_data>* &neighbor,
                                                  short root_level)
{
  short cellsize=1<<this->level();

  switch (face)
  {
    case NONE:
      break;

    case X_LOW:
    {
      short xlowcode=this->code_.x_loc_-(short)(1);
      if (xlowcode<0)
        return false;
      short diff=this->code_.x_loc_^xlowcode;

      boct_loc_code<T_loc> neighborcode;
      neighborcode.set_code(xlowcode,this->code_.y_loc_,this->code_.z_loc_);

      boct_tree_cell<T_loc,T_data>* commonancestor=get_common_ancestor(diff);

      if (commonancestor==NULL)
        return false;
      // at the same or greater level ( towards the root)
      neighbor=commonancestor->traverse_to_level(&neighborcode,this->level());

      break;
    }
    case X_HIGH:
    {
      if ((this->code_.x_loc_+cellsize)>=(1<<root_level))
        return false ;
      short xhighcode=this->code_.x_loc_+cellsize;
      short diff=this->code_.x_loc_^xhighcode;
      // set the code for the neighboring point
      boct_loc_code<T_loc> neighborcode;
      neighborcode.set_code(xhighcode,this->code_.y_loc_,this->code_.z_loc_);

      boct_tree_cell<T_loc,T_data>* commonancestor=get_common_ancestor(diff);

      if (commonancestor==NULL)
        return false;
      // at the same or greater level ( towards the root)
      neighbor=commonancestor->traverse_to_level(&neighborcode,this->level());


      break;
    }
    case Y_LOW:
    {
      short ylowcode=this->code_.y_loc_-(short)(1);
      if (ylowcode<0)
        return false;
      short diff=this->code_.y_loc_^ylowcode;

      boct_loc_code<T_loc> neighborcode;
      neighborcode.set_code(this->code_.x_loc_,ylowcode,this->code_.z_loc_);

      boct_tree_cell<T_loc,T_data>* commonancestor=get_common_ancestor(diff);

      if (commonancestor==NULL)
        return false;
      // at the same or greater level ( towards the root)
      neighbor=commonancestor->traverse_to_level(&neighborcode,this->level());

      break;
    }
    case Y_HIGH:
    {
      if ((this->code_.y_loc_+cellsize)>=(1<<(root_level)))
        return false ;
      short yhighcode=this->code_.y_loc_+cellsize;
      short diff=this->code_.y_loc_^yhighcode;
      // set the code for the neighboring point
      boct_loc_code<T_loc> neighborcode;
      neighborcode.set_code(this->code_.x_loc_,yhighcode,this->code_.z_loc_);

      boct_tree_cell<T_loc,T_data>* commonancestor=get_common_ancestor(diff);

      if (commonancestor==NULL)
        return false;
      // at the same or greater level ( towards the root)
      neighbor=commonancestor->traverse_to_level(&neighborcode,this->level());

      break;
    }
    case Z_LOW:
    {
      short zlowcode=this->code_.z_loc_-(short)(1);
      if (zlowcode<0)
        return false;
      short diff=this->code_.z_loc_^zlowcode;

      boct_loc_code<T_loc> neighborcode;
      neighborcode.set_code(this->code_.x_loc_,this->code_.y_loc_,zlowcode);

      boct_tree_cell<T_loc,T_data>* commonancestor=get_common_ancestor(diff);

      if (commonancestor==NULL)
        return false;
      // at the same or greater level ( towards the root)
      neighbor=commonancestor->traverse_to_level(&neighborcode,this->level());

      break;
    }
    case Z_HIGH:
    {
      if ((this->code_.z_loc_+cellsize)>=(1<<(root_level)))
        return false ;
      short zhighcode=this->code_.z_loc_+cellsize;
      short diff=this->code_.z_loc_^zhighcode;
      // set the code for the neighboring point
      boct_loc_code<T_loc> neighborcode;
      neighborcode.set_code(this->code_.x_loc_,this->code_.y_loc_,zhighcode);

      boct_tree_cell<T_loc,T_data>* commonancestor=get_common_ancestor(diff);

      if (commonancestor==NULL)
        return false;
      // at the same or greater level ( towards the root)
      neighbor=commonancestor->traverse_to_level(&neighborcode,this->level());


      break;
    }
    default:
      break;
  }
  return true;
}

//: adds a pointer for each leaf children to v
template<class T_loc,class T_data>
void boct_tree_cell<T_loc,T_data>::leaf_children(vcl_vector<boct_tree_cell<T_loc,T_data>*>& v)
{
  // the cell itself is a leaf
  if (!children_)
    return;

  for (unsigned i=0; i<8; i++) {
    if (children_[i].is_leaf()) {
      v.push_back(&children_[i]);
    }
    else {
      children_[i].leaf_children(v);
    }
  }
}

//: adds a pointer to vector v, for each leaf children at a particular level
template<class T_loc,class T_data>
void boct_tree_cell<T_loc,T_data>::leaf_children_at_level(vcl_vector<boct_tree_cell<T_loc,T_data>*>& v, short target_level)
{
  short curr_level = code_.level;

  if (curr_level > target_level+1) {
    for (unsigned i=0; i<8; i++) {
      if (!children_[i].is_leaf())
        children_[i].leaf_children_at_level(v,target_level);
    }
  }

  if (curr_level == target_level+1) {
    for (unsigned i=0; i<8; i++) {
      if (children_[i].is_leaf())
        v.push_back(&children_[i]);
    }
  }
}

//: adds a pointer to vector v, for each children at a particular level
template<class T_loc,class T_data>
void boct_tree_cell<T_loc,T_data>::children_at_level(vcl_vector<boct_tree_cell<T_loc,T_data>*>& v, short target_level)
{
  short curr_level = code_.level;

  if (curr_level > target_level+1) {
    for (unsigned i=0; i<8; i++) {
      if (!children_[i].is_leaf())
        children_[i].children_at_level(v,target_level);
    }
  }
  else if (curr_level == target_level+1) {
    for (unsigned i=0; i<8; i++) {
       v.push_back(&children_[i]);
    }
  }
  else{
    vcl_cerr << "Inconsintent case in cell::children_at_level\n";
    return;
  }
}

template<class T_loc,class T_data>
void boct_tree_cell<T_loc,T_data>::print()
{
  vcl_cout << "LEVEL=" << this->level()
           << " code=" << code_
           << " parent=" << parent_;
#if 0
  vcl_cout << " data=" << data_;
#endif
  if (is_leaf())
    vcl_cout << " LEAF" << vcl_endl;
  else {
    vcl_cout << vcl_endl;
    for (unsigned i=0; i<8; i++) {
      children_[i].print();
    }
  }
}

template<class T_loc,class T_data>
void vsl_b_write(vsl_b_ostream & os, boct_tree_cell<T_loc,T_data>& cell)
{
  vsl_b_write(os, boct_tree_cell<T_loc,T_data>::version_no());
  vsl_b_write(os, cell.code_);
  vsl_b_write(os, cell.data());
  boct_tree_cell<T_loc,T_data>* children = cell.children();
  bool leaf=true;
  if (!cell.is_leaf()) {
    leaf = false;
    vsl_b_write(os, leaf);
    for (unsigned i=0; i<8; i++)
      vsl_b_write(os, children[i]);
  }
  else // no children
      {
      vsl_b_write(os, leaf);
      }
}

template<class T_loc,class T_data>
void vsl_b_read(vsl_b_istream & is, boct_tree_cell<T_loc,T_data>& c, boct_tree_cell<T_loc,T_data>* parent)
{
  if (!is) return;

  short version;
  vsl_b_read(is,version);
  switch (version)
  {
    case 1: {
     vsl_b_read(is, c.code_);
     T_data data;
     vsl_b_read(is, data);
     c.set_data(data);
     c.set_parent(parent);
     c.set_vis_node(NULL);
     bool leaf;
     vsl_b_read(is, leaf);
     if (!leaf) {
       c.split();
       boct_tree_cell<T_loc,T_data>* children = c.children();
       for (unsigned i=0; i<8; i++) {
         children[i].set_parent(&c);
         vsl_b_read(is, children[i], &c);
       }
     }
     break;
    }
    default:
     vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boct_tree<T>&)\n"
              << "           Unknown version number "<< version << '\n';
     is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
     return;
  }
}

template<class T_loc,class T_data>
vcl_ostream& operator <<(vcl_ostream &s, boct_tree_cell<T_loc,T_data>& cell)
{
  s <<  cell.code_ ;
  return s;
}

#define BOCT_TREE_CELL_INSTANTIATE(T_loc,T_data) \
template class boct_tree_cell<T_loc,T_data >; \
template vcl_ostream& operator <<(vcl_ostream &s, boct_tree_cell<T_loc,T_data >& cell);\
template void vsl_b_read(vsl_b_istream &, boct_tree_cell<T_loc,T_data >&, boct_tree_cell<T_loc,T_data >*); \
template void vsl_b_write(vsl_b_ostream &, boct_tree_cell<T_loc,T_data >&); \
template class boct_cell_vis_graph_node<T_loc,T_data >
#endif // boct_tree_cell_txx_
