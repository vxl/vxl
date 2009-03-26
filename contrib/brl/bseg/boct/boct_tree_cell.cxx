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
  if (children_==NULL)
    return true;
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
             short xlowcode=this->code_.x_loc_-(short)(1);
             if(xlowcode<0)
                 return;
             short diff=this->code_.x_loc_^xlowcode;

             boct_loc_code neighborcode;
             neighborcode.set_code(xlowcode,this->code_.y_loc_,this->code_.z_loc_);

             boct_tree_cell* commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell * neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);
             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell*> leafnodes;
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

             boct_tree_cell* commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell * neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);
             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell*> leafnodes;
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

             boct_tree_cell* commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell * neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);
             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell*> leafnodes;
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

             boct_tree_cell* commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell * neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);

             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell*> leafnodes;
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

             boct_tree_cell* commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell * neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);

             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell*> leafnodes;
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

             boct_tree_cell* commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell * neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);

             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell*> leafnodes;
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

void boct_tree_cell::leaf_children(vcl_vector<boct_tree_cell*>& v)
{
  if (!children_)
    return;

  for(unsigned i=0; i<8; i++) {
    if (children_[i].is_leaf())
      v.push_back(&children_[i]);
    else
      children_[i].leaf_children(v);
  }
}

void boct_tree_cell::print()
{
  vcl_cout << "LEVEL=" << level_; 
  vcl_cout << " code=" << code_;
  if (!children_)
    vcl_cout << " LEAF " << vcl_endl;
  else {
    vcl_cout << vcl_endl;
    for (unsigned i=0; i<8; i++) {
      children_[i].print();
    }
  }
}