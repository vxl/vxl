// This is sdet_EHT.h
#ifndef sdet_EHT_h
#define sdet_EHT_h
//:
//\file
//\brief This class stores an edgel hypothesis tree (EHT)
//\author Amir Tamrakar
//\date 08/08/08
//
//\verbatim
//  Modifications
//\endverbatim

#include <vcl_vector.h>
#include <vcl_list.h>
#include <vcl_set.h>

#include "sdet_edgel.h"
#include "sdet_curvelet.h"

//: A hyp tree made of edgels
class sdet_EHT_node
{
public :
  sdet_edgel* e;
  sdet_EHT_node* parent;
  vcl_list<sdet_EHT_node*> children;

  sdet_EHT_node(sdet_edgel* new_e): e(new_e), parent(0), children(0) {}
  ~sdet_EHT_node()
  {
    if (parent!=0){
      parent->children.remove(this);
      parent = 0;
    }

    while(children.size()>0)
      delete children.front();

    e = 0;
  }

  void add_child(sdet_EHT_node* new_node)
  {
    children.push_back(new_node);
    new_node->parent = this;
  }

};

class sdet_EHT
{
public:
  sdet_EHT_node* root;

  //: constructor 1
  sdet_EHT(sdet_EHT_node* new_root=0): root(new_root) {}
  
  //: constructor 2
  sdet_EHT(sdet_edgel* root_e){ root = new sdet_EHT_node(root_e); }

  ~sdet_EHT(){ delete_tree(); }

  void delete_tree(){ delete root; root = 0;}

  //: DFS iterator
  class path_iterator
  {
    public:
      //: start iterator at a defined node
      path_iterator(sdet_EHT_node* ptr) : ptr_(ptr) { if (ptr) cur_path_.push_back(ptr->e); }

      //: copy constructor
      path_iterator(const path_iterator& other) : ptr_(other.ptr_), cur_path_(other.cur_path_) {}
      
      //operators
      sdet_EHT_node* operator*(){ return ptr_; }
      bool operator==(const path_iterator& other) { return ptr_ == other.ptr_; }
      bool operator!=(const path_iterator& other) { return ptr_ != other.ptr_; }

      //: dummy routine (this is not a real decrement operator, it just goes to its parent node)
      void operator--(int) //post decrement operator
      {
        ptr_ = ptr_->parent;
        cur_path_.pop_back();
      }

      //: DFS routine
      void operator++(int) //post increment operator
      {
        inc_ptr();

        while (ptr_ && ptr_->children.size()>0)
          inc_ptr();
      }
      
      void inc_ptr()
      {
        if (!ptr_)
          return;

        //if the current node has no parents, it must be the root node, go oto its first child
        if (ptr_->parent==0){
          if (ptr_->children.size()==0){ //only one element in the tree
            ptr_ = 0; //set the pointer
            cur_path_.clear();
            return;
          }

          ptr_ = ptr_->children.front();
          cur_path_.push_back(ptr_->e);
          return;
        }

        //if this is a leaf node, back track through the parents to find the next valid node
        if (ptr_->children.size()==0){
          sdet_EHT_node* cur = ptr_;
          sdet_EHT_node* parent = cur->parent;

          while (cur == parent->children.back()){
            //assert(cur->cvlet == cur_path_.back());
            cur_path_.pop_back(); //remove the current cvlet from the path

            //move one step higher on the tree
            cur = parent;
            parent = cur->parent;

            if (parent==0){ //we have reached the root node again so terminate
              ptr_ = 0;
              cur_path_.clear();
              return;
            }
          }

          //next valid one can be set (find the current child on the parents list and set it to the next)
          vcl_list<sdet_EHT_node*>::iterator nit = parent->children.begin();
          for (; nit != parent->children.end(); nit++){
            if ((*nit)==cur){
              nit++;
              
              //assert(cur->cvlet == cur_path_.back());
              cur_path_.pop_back(); //first remove the current cvlet from the path

              ptr_ = (*nit);
              cur_path_.push_back(ptr_->e);
              return;
            }
          }
        }

        //if this is a node in the middle of the tree, simply go to its first child, i.e., go to the next depth
        ptr_ = ptr_->children.front();
        cur_path_.push_back(ptr_->e);
      }

      //: return the current path
      vcl_vector<sdet_edgel*>& get_cur_path() { return cur_path_; }

    protected:
      sdet_EHT_node* ptr_;             //this is the node that the iterator is currently pointing to
      vcl_vector<sdet_edgel*> cur_path_; // this is the path from point where the iterator was initialized to the current node
  };

  //: Return an iterator to the first element
  path_iterator path_begin() { path_iterator it(root); it++; return it; }

  //: Return an iterator to a null pointer (only at the very last leaf node)
  path_iterator path_end() { return path_iterator(0); }

  //delete a subtree pointed to by the given iterator 
  void delete_subtree(path_iterator& it)
  {
    sdet_EHT_node* node_to_del = *it;

    //move the iterator to its parent first (or the node from where this path bifurcates
    it--;
    while ((*it)->children.size()==1 && (*it)!=root){
      node_to_del = (*it);
      it--;      
    }

    //now delete the subtree
    delete node_to_del;

  }
};

#endif // sdet_EHT_h
