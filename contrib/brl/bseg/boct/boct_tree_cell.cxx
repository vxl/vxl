#include "boct_tree_cell.h"

#include <vcl_iostream.h>

boct_tree_cell::boct_tree_cell(const boct_loc_code& code, short level)
{
    level_=level;
    code_=code;
    parent_=NULL;
    children_=NULL;
}

bool boct_tree_cell::is_leaf()
{
  return false;
}
  
const boct_loc_code& 
boct_tree_cell::get_code()
{
  return code_;
}
  
boct_tree_cell* boct_tree_cell::traverse(boct_loc_code code)
{
  // remove this
  return this;
}
//: this is always going down the tree
boct_tree_cell* boct_tree_cell::traverse_to_level(boct_loc_code * code, short level)
{
    if(level<0)
        return NULL;
    boct_tree_cell* curr_cell=this;
    while(level<curr_cell->level() && curr_cell->children())
    {
        //boct_loc_code tempcode=curr_cell->get_code();
        short child_index=code->child_index(curr_cell->level());
        curr_cell=curr_cell->children()+child_index;
    }
    return curr_cell;
}
  
bool boct_tree_cell::split()
{
  
  // create new children
  children_ = (boct_tree_cell*) malloc(sizeof(boct_tree_cell)*8);
  short child_level = level_-1;
  for (unsigned i=0; i<8; i++) {
    children_[i].level_ = child_level;
    children_[i].parent_ = this;
    children_[i].children_ = 0;
    children_[i].code_ = code_.child_loc_code(i, child_level);
  }
  return false;
}
boct_tree_cell * boct_tree_cell::get_common_ancestor(short binarydiff)
{
    short curr_level=level_;
    boct_tree_cell * curr_cell=this;
    while(binarydiff & (1<< (curr_level)))
    {
        curr_cell=curr_cell->parent_;
        curr_level++;
    }
    return curr_cell;
}
 
void  boct_tree_cell::find_neighbors(FACE_IDX face,vcl_vector<boct_tree_cell*> & neighbors,short max_level)
{
    short cellsize=1<<this->level_;

    switch(face)
    {
    case NONE:
             
        break;

    case X_LOW:
         {
            break;
         }
    case X_HIGH:
         {
             if((this->code_.x_loc_+cellsize)>=(1<<max_level-1))
                 return ;
             short xhighcode=this->code_.x_loc_+cellsize;
             short diff=this->code_.x_loc_^xhighcode;
             //: set the code for the neighboring point
             boct_loc_code neighborcode;
             neighborcode.set_code(xhighcode,this->code_.y_loc_,this->code_.z_loc_);

             boct_tree_cell* commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell * neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);

             //: TODO code to get the cells which are further down the tree. 
             

             break;
         }

    default:
        break;

    }
}
void boct_tree_cell::print()
{
  vcl_cout << "LEVEL=" << level_ << vcl_endl; 
  vcl_cout << " code=" << code_;
  if (!children_)
    vcl_cout << "LEAF " << vcl_endl << vcl_endl;
  else {
    for (unsigned i=0; i<8; i++) {
      children_[i].print();
    }
  }
}