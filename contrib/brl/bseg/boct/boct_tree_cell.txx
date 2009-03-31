#ifndef boct_tree_cell_txx_
#define boct_tree_cell_txx_

#include "boct_tree_cell.h"

#include <vcl_iostream.h>

template<class T>
boct_tree_cell<T>::boct_tree_cell(const boct_loc_code& code, short level)
{
    level_=level;
    code_=code;
    parent_=NULL;
    children_=NULL;
}


template<class T>
bool boct_tree_cell<T>::is_leaf()
{
  if (children_== NULL)
    return true;
  return false;
}
  
template<class T>
const boct_loc_code& 
boct_tree_cell<T>::get_code()
{
  return code_;
}

template<class T>
boct_tree_cell<T>::~boct_tree_cell()
{
  if (children_ != NULL) {
   free(children_);
  }
}
  
  
//: this is always going down the tree
template<class T>
boct_tree_cell<T>* boct_tree_cell<T>::traverse_to_level(boct_loc_code * code, short level)
{
    if(level<0)
        return NULL;
    boct_tree_cell<T>* curr_cell=this;
    while(level<curr_cell->level() && curr_cell->children())
    {
        //boct_loc_code tempcode=curr_cell->get_code();
        short child_index=code->child_index(curr_cell->level());
        curr_cell=curr_cell->children()+child_index;
    }
    return curr_cell;
}
 
  

template <class T>
bool boct_tree_cell<T>::split()
{
  // create new children if there is none
  if (is_leaf()) {
	children_ = (boct_tree_cell<T>*) malloc(sizeof(boct_tree_cell<T>)*8);
	short child_level = level_-1;
	for (unsigned i=0; i<8; i++) {	
	  children_[i].code_ = code_.child_loc_code(i, child_level);
	  children_[i].level_ = child_level;
      children_[i].set_parent(this);
      children_[i].data_ = this->data_;
      children_[i].children_ = 0;
	}
    return true;
  }
  return false;
}

template <class T>
boct_tree_cell<T>* boct_tree_cell<T>::get_common_ancestor(short binarydiff)
{
    short curr_level=level_;
    boct_tree_cell<T>* curr_cell=this;
    while(binarydiff & (1<< (curr_level)))
    {
        curr_cell=curr_cell->parent_;
        curr_level++;
    }
    return curr_cell;
}
 
template <class T>
void  boct_tree_cell<T>::find_neighbors(FACE_IDX face,
                                     vcl_vector<boct_tree_cell<T>*> & neighbors,
                                     short max_level)
{
    short cellsize=1<<this->level_;

    switch(face)
    {
    case NONE:
             
        break;

    case X_LOW:
         {
             short xlowcode=this->code_.x_loc_-(short)(1);
             if(xlowcode<0)
                 return;
             short diff=this->code_.x_loc_^xlowcode;

             boct_loc_code neighborcode;
             neighborcode.set_code(xlowcode,this->code_.y_loc_,this->code_.z_loc_);

             boct_tree_cell<T>* commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell<T>* neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);
             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell<T>*> leafnodes;
                 neighborcell->leaf_children(leafnodes);

                 for (unsigned int i=0;i<leafnodes.size();i++)
                 {
                     if(leafnodes[i]->code_.x_loc_^xlowcode)
                         neighbors.push_back(leafnodes[i]);
                 }

             }

            break;
         }
    case X_HIGH:
         {
             if((this->code_.x_loc_+cellsize)>=(1<<(max_level-1)))
                 return ;
             short xhighcode=this->code_.x_loc_+cellsize;
             short diff=this->code_.x_loc_^xhighcode;
             //: set the code for the neighboring point
             boct_loc_code neighborcode;
             neighborcode.set_code(xhighcode,this->code_.y_loc_,this->code_.z_loc_);

             boct_tree_cell<T>* commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell<T>* neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);
             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell<T>*> leafnodes;
                 neighborcell->leaf_children(leafnodes);

                 for (unsigned int i=0;i<leafnodes.size();i++)
                 {
                     if(leafnodes[i]->code_.x_loc_^xhighcode)
                         neighbors.push_back(leafnodes[i]);
                 }

             }

             break;
         }
    case Y_LOW:
         {
             short ylowcode=this->code_.y_loc_-(short)(1);
             if(ylowcode<0)
                 return;
             short diff=this->code_.y_loc_^ylowcode;

             boct_loc_code neighborcode;
             neighborcode.set_code(this->code_.x_loc_,ylowcode,this->code_.z_loc_);

             boct_tree_cell<T>* commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell<T>* neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);
             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell<T>*> leafnodes;
                 neighborcell->leaf_children(leafnodes);

                 for (unsigned int i=0;i<leafnodes.size();i++)
                 {
                     if(leafnodes[i]->code_.y_loc_^ylowcode)
                         neighbors.push_back(leafnodes[i]);
                 }

             }



            break;
         }
    case Y_HIGH:
         {
             if((this->code_.y_loc_+cellsize)>=(1<<(max_level-1)))
                 return ;
             short yhighcode=this->code_.y_loc_+cellsize;
             short diff=this->code_.y_loc_^yhighcode;
             //: set the code for the neighboring point
             boct_loc_code neighborcode;
             neighborcode.set_code(this->code_.x_loc_,yhighcode,this->code_.z_loc_);

             boct_tree_cell<T>* commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell<T>* neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);

             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell<T>*> leafnodes;
                 neighborcell->leaf_children(leafnodes);

                 for (unsigned int i=0;i<leafnodes.size();i++)
                 {
                     if(leafnodes[i]->code_.y_loc_^yhighcode)
                         neighbors.push_back(leafnodes[i]);
                 }

             }
             

             break;
         }
    case Z_LOW:
         {
             short zlowcode=this->code_.z_loc_-(short)(1);
             if(zlowcode<0)
                 return;
             short diff=this->code_.z_loc_^zlowcode;

             boct_loc_code neighborcode;
             neighborcode.set_code(this->code_.x_loc_,this->code_.y_loc_,zlowcode);

             boct_tree_cell<T>* commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell<T>* neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);

             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell<T>*> leafnodes;
                 neighborcell->leaf_children(leafnodes);

                 for (unsigned int i=0;i<leafnodes.size();i++)
                 {
                     if(leafnodes[i]->code_.z_loc_^zlowcode)
                         neighbors.push_back(leafnodes[i]);
                 }

             }


            break;
         }
    case Z_HIGH:
         {
             if((this->code_.z_loc_+cellsize)>=(1<<(max_level-1)))
                 return ;
             short zhighcode=this->code_.z_loc_+cellsize;
             short diff=this->code_.z_loc_^zhighcode;
             //: set the code for the neighboring point
             boct_loc_code neighborcode;
             neighborcode.set_code(this->code_.x_loc_,this->code_.y_loc_,zhighcode);

             boct_tree_cell<T>* commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell<T>* neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);

             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell<T>*> leafnodes;
                 neighborcell->leaf_children(leafnodes);

                 for (unsigned int i=0;i<leafnodes.size();i++)
                 {
                     if(leafnodes[i]->code_.z_loc_^zhighcode)
                         neighbors.push_back(leafnodes[i]);
                 }

             }
             

             break;
         }
    default:
        break;

    }
}

template <class T>
void boct_tree_cell<T>::leaf_children(vcl_vector<boct_tree_cell<T>*>& v)
{
  if (!children_)
    return;

  for(unsigned i=0; i<8; i++) {
    if (children_[i].is_leaf()) {
      v.push_back(&children_[i]);
    } else {
      children_[i].leaf_children(v);
    }
  }
}

template <class T>
void boct_tree_cell<T>::print()
{
  vcl_cout << "LEVEL=" << level_; 
  vcl_cout << " code=" << code_;
  vcl_cout << " parent=" << parent_;
  vcl_cout << " data=" << data_;
  if (is_leaf())
    vcl_cout << " LEAF " << vcl_endl;
  else {
    vcl_cout << vcl_endl;
    for (unsigned i=0; i<8; i++) {
      children_[i].print();
    }
  }
}

template <class T>
void vsl_b_write(vsl_b_ostream & os, boct_tree_cell<T>& cell)
{
  const short io_version_no = 0;

  vsl_b_write(os, io_version_no);
  vsl_b_write(os, cell.level());
  vsl_b_write(os, cell.code_);
  T data = cell.data();
  vsl_b_write(os, data);
  boct_tree_cell<T>* children = cell.children();
  bool leaf=true;
  if (!cell.is_leaf()) {
    leaf = false;
    vsl_b_write(os, leaf);
    for(unsigned i=0; i<8; i++)
      vsl_b_write(os, children[i]);
  } else // no children
      vsl_b_write(os, leaf);
}

template <class T>
void vsl_b_read(vsl_b_istream & is, boct_tree_cell<T>& c, boct_tree_cell<T>* parent)
{
  if (!is) return;

  short version;
  vsl_b_read(is, version);
  switch (version)
  {
   case (0):
    short level;
    vsl_b_read(is, level);

    boct_loc_code code;
    vsl_b_read(is, code);

    c = boct_tree_cell<T>(code, level);
    T data;
    vsl_b_read(is, data);
    c.set_data(data);
    c.set_parent(parent);
    bool leaf;
    vsl_b_read(is, leaf);
    if (!leaf) {
      c.split();
      boct_tree_cell<T>* children = c.children();
      for (unsigned i=0; i<8; i++) {
        children[i].set_parent(&c);
        vsl_b_read(is, children[i], &c);
      }
    } 
	break;
  }
  
}

#define BOCT_TREE_CELL_INSTANTIATE(T) \
template class boct_tree_cell<T>; \
template void vsl_b_read(vsl_b_istream &, boct_tree_cell<T>&, boct_tree_cell<T>*); \
template void vsl_b_write(vsl_b_ostream &, boct_tree_cell<T>&);

#endif