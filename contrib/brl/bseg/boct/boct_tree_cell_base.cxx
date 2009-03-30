#include "boct_tree_cell.h"

#include <vcl_iostream.h>


boct_tree_cell_base::boct_tree_cell_base(const boct_loc_code& code, short level)
{
    level_=level;
    code_=code;
    parent_=NULL;
    children_=NULL;
}


bool boct_tree_cell_base::is_leaf()
{
  if (children_==NULL)
    return true;
  return false;
}
  

const boct_loc_code& 
boct_tree_cell_base::get_code()
{
  return code_;
}


boct_tree_cell_base::~boct_tree_cell_base()
{
  if (children_)
    delete[] children_;
}
  
  
//: this is always going down the tree

boct_tree_cell_base_sptr boct_tree_cell_base::traverse_to_level(boct_loc_code * code, short level)
{
    if(level<0)
        return NULL;
    boct_tree_cell_base_sptr curr_cell=this;
    while(level<curr_cell->level() && curr_cell->children())
    {
        //boct_loc_code tempcode=curr_cell->get_code();
        short child_index=code->child_index(curr_cell->level());
        curr_cell=curr_cell->children()+child_index;
    }
    return curr_cell;
}
 
  

bool boct_tree_cell_base::split()
{
  
  // create new children
  children_ = (boct_tree_cell_base*) malloc(sizeof(boct_tree_cell_base)*8);
  short child_level = level_-1;
  for (unsigned i=0; i<8; i++) {
    children_[i] = boct_tree_cell_base(code_.child_loc_code(i, child_level), child_level);
    children_[i].set_parent(this);
  }
  return false;
}


boct_tree_cell_base_sptr boct_tree_cell_base::get_common_ancestor(short binarydiff)
{
    short curr_level=level_;
    boct_tree_cell_base_sptr curr_cell=this;
    while(binarydiff & (1<< (curr_level)))
    {
        curr_cell=curr_cell->parent_;
        curr_level++;
    }
    return curr_cell;
}
 
 
void  boct_tree_cell_base::find_neighbors(FACE_IDX face,
                                     vcl_vector<boct_tree_cell_base_sptr> & neighbors,
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

             boct_tree_cell_base_sptr commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell_base_sptr neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);
             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell_base_sptr> leafnodes;
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

             boct_tree_cell_base_sptr commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell_base_sptr neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);
             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell_base_sptr> leafnodes;
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

             boct_tree_cell_base_sptr commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell_base_sptr neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);
             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell_base_sptr> leafnodes;
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

             boct_tree_cell_base_sptr commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell_base_sptr neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);

             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell_base_sptr> leafnodes;
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

             boct_tree_cell_base_sptr commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell_base_sptr neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);

             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell_base_sptr> leafnodes;
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

             boct_tree_cell_base_sptr commonancestor=get_common_ancestor(diff);
             
             if(commonancestor==NULL)
                 return;
             //: at the same or greater level ( towards the root)   
             boct_tree_cell_base_sptr neighborcell=commonancestor->traverse_to_level(&neighborcode,this->level_);
                

             if(!neighborcell->children())
                 neighbors.push_back(neighborcell);

             else
             {
                 //: TODO code to get the cells which are further down the tree. 
                 //: This is naive implementation of getting all the leaf nodes from a node and 
                 //: test each of them if  they are neighbors.
                 vcl_vector<boct_tree_cell_base_sptr> leafnodes;
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


void boct_tree_cell_base::leaf_children(vcl_vector<boct_tree_cell_base_sptr>& v)
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


void boct_tree_cell_base::print()
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

void vsl_b_write(vsl_b_ostream & os, boct_tree_cell_base& cell)
{
  const short io_version_no = 1;

  vsl_b_write(os, io_version_no);
  vsl_b_write(os, cell.level());
  vsl_b_write(os, cell.code_);
  vsl_b_write(os, cell.parent());
  boct_tree_cell_base* children = cell.children();
  if (children) {
    for(unsigned i=0; i<8; i++)
      vsl_b_write(os, children[i]);
  }
}

void vsl_b_read(vsl_b_istream & is, boct_tree_cell_base& c)
{

}