// This is sdet_hyp_tree_graph.h
#ifndef sdet_hyp_tree_graph_h
#define sdet_hyp_tree_graph_h
//:
//\file
//\brief Link graph structure
//\author Amir Tamrakar
//\date 03/15/08
//
//\verbatim
//  Modifications
//\endverbatim

#include <vcl_vector.h>
#include <vcl_list.h>
#include <vcl_set.h>

#include "sdet_edgel.h"
#include "sdet_curvelet.h"


//: a node in the hypothesis tree
class sdet_hyp_tree_node
{
public:
  int tree_id; //id of the hyp tree it belongs to
  bool dir;   //direction of the tree (forward or reverse)
  sdet_curvelet* cvlet;
  sdet_hyp_tree_node* parent; //parent node (needed for tree traversal)
  vcl_list<sdet_hyp_tree_node*> children;

  //: constructors
  sdet_hyp_tree_node(int id=-1, bool newdir=true): tree_id(id), dir(newdir), cvlet(0), parent(0){}
  sdet_hyp_tree_node(sdet_curvelet* cvlet_, int id=-1, bool newdir=true): tree_id(id), dir(newdir), cvlet(cvlet_), parent(0){}

  //: destructor
  ~sdet_hyp_tree_node()
  { 
    //cvlets are cloned from the CM to assign to the HTs so delete them
    delete cvlet;
    cvlet = 0; 

    //first remove this node from the parent's list (if it has a parent)
    if (parent)
      parent->children.remove(this);
    
    parent = 0; //remove pointer to the parent
    
    //delete all children
    while (children.size()>0)
      delete children.front();
  }

  //: is this a leaf node?
  bool is_leaf() { return children.size()==0; }

  //construct a child node from the given curvelet and return a pointer to the node
  sdet_hyp_tree_node* add_child(sdet_curvelet* cvlet)
  {
    sdet_hyp_tree_node* new_node = new sdet_hyp_tree_node(cvlet, tree_id, dir);
    children.push_back(new_node);
    new_node->parent = this;

    return new_node;
  }

};

//: a curve hypothesis tree
class sdet_hyp_tree
{
public:
  int tree_id;
  sdet_hyp_tree_node* root;
  bool resolved;
  sdet_hyp_tree_node* best_path; 
  double least_cost;

  //: default constructor
  sdet_hyp_tree(int id=-1): tree_id(id), root(0), resolved(false), best_path(0), least_cost(1000)  {}

  //: constructor given a root node
  sdet_hyp_tree(sdet_hyp_tree_node* new_root, int id=-1) : 
    tree_id(id), root(new_root), resolved(false), best_path(0), least_cost(1000) {}

  //: destructor
  ~sdet_hyp_tree(){ delete_tree(); }

  //: DFS iterator
  class iterator
  {
    public:
      //: start iterator at a defined node
      iterator(sdet_hyp_tree_node* ptr) : ptr_(ptr) { if (ptr) cur_path_.push_back(ptr->cvlet); }
      //: copy constructor
      iterator(const iterator& other) : ptr_(other.ptr_), cur_path_(other.cur_path_) {}
      
      //operators
      sdet_hyp_tree_node* operator*(){ return ptr_; }
      bool operator==(const iterator& other) { return ptr_ == other.ptr_; }
      bool operator!=(const iterator& other) { return ptr_ != other.ptr_; }

      //: dummy routine (this is not a real decrement operator, it just goes to its parent node)
      void operator--(int) //post decrement operator
      {
        ptr_ = ptr_->parent;
        cur_path_.pop_back();
      }

      //: DFS routine
      void operator++(int) //post increment operator
      {
        //if the current node has no parents, it must be the root node, go oto its first child
        if (ptr_->parent==0){
          if (ptr_->children.size()==0){ //only one element in the tree
            ptr_ = 0; //set the pointer
            cur_path_.clear();
            return;
          }

          ptr_ = ptr_->children.front();
          cur_path_.push_back(ptr_->cvlet);
          return;
        }

        //if this is a leaf node, back track through the parents to find the next valid node
        if (ptr_->children.size()==0){
          sdet_hyp_tree_node* cur = ptr_;
          sdet_hyp_tree_node* parent = cur->parent;

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
          vcl_list<sdet_hyp_tree_node*>::iterator nit = parent->children.begin();
          for (; nit != parent->children.end(); nit++){
            if ((*nit)==cur){
              nit++;
              
              //assert(cur->cvlet == cur_path_.back());
              cur_path_.pop_back(); //first remove the current cvlet from the path

              ptr_ = (*nit);
              cur_path_.push_back(ptr_->cvlet);
              return;
            }
          }
        }

        //if this is a node in the middle of the tree, simply go to its first child, i.e., go to the next depth
        ptr_ = ptr_->children.front();
        cur_path_.push_back(ptr_->cvlet);
      }

      //: return the current path
      vcl_vector<sdet_curvelet*>& get_cur_path() { return cur_path_; }

    protected:
      sdet_hyp_tree_node* ptr_;             //this is the node that the iterator is currently pointing to
      vcl_vector<sdet_curvelet*> cur_path_; // this is the path from point where the iterator was initialized to the current node
  };

  //: Return an iterator to the first element
  iterator begin() { return iterator(root); }

  //: Return an iterator to a null pointer (only at the very last leaf node)
  iterator end() { return iterator(0); }

  //we could also use a path_iterator where the iterator steps through the independent paths instead of nodes

  //also a reverse iterator that traverses from a given node the the root, while compiling the curvelet list would be useful

  void delete_tree(){ if (root) delete root; root = 0; resolved=false;  best_path=0; least_cost=1000; }

  void delete_subtree(iterator& it)
  {
    sdet_hyp_tree_node* node_to_del = *it;

    //move the iterator to its parent first
    it--;

    //now delete the subtree
    delete node_to_del;
  }

  //traverse through all the nodes and print all alternate paths from the root to the leaves
  void print_all_paths()
  {
    //print all the paths and costs
    vcl_cout << "Current Hypothesis tree: TREE ID = " << this->tree_id << "----------------------------------------------------------" << vcl_endl;
    
    //old implementation (with recursion)
    //vcl_list<sdet_curvelet*> CF;
    //next_node(root, CF);

    //new implementation; using iterators
    iterator pit = begin();
    for ( ; pit != end(); pit++){
      if ((*pit)->is_leaf()){
        //print out the current CF list
        vcl_vector<sdet_curvelet*> CF_list = pit.get_cur_path();

        vcl_cout << ":: ";
        for (unsigned i=0; i<CF_list.size(); i++){
          for (unsigned j=0; j<CF_list[i]->edgel_chain.size(); j++)
            vcl_cout << CF_list[i]->edgel_chain[j]->id << " ";
          vcl_cout << "* ";
        }
        
        //also write out the cost
        vcl_cout << vcl_endl;
      }
    }
  }

  //: recursively traverse the hyp. tree and print out list every time a leaf node is reached
  void next_node(sdet_hyp_tree_node* cur_node, vcl_list<sdet_curvelet*> &CF)
  {
    //add current curvelet to the CF list
    CF.push_back(cur_node->cvlet);

    //check to see if this is a leaf node
    if (cur_node->children.size()==0){
      //print out the current CF list
      vcl_cout << ":: ";
      vcl_list<sdet_curvelet*>::iterator cvit = CF.begin();
      for (; cvit!=CF.end(); cvit++){
        for (unsigned i=0; i<(*cvit)->edgel_chain.size(); i++)
          vcl_cout << (*cvit)->edgel_chain[i]->id << " ";
        vcl_cout << "* ";
      }
      
      //also write out the cost
      vcl_cout << vcl_endl;

      CF.pop_back();
      return;
    }

    //go to its children
    vcl_list<sdet_hyp_tree_node*>::iterator nit = cur_node->children.begin();
    for (; nit!=cur_node->children.end(); nit++)
      next_node((*nit), CF);

    CF.pop_back();
  }
};

//: An HT graph (the nodes inthis graph are the hyp trees)
class sdet_HT_graph
{
public:
  vcl_vector<sdet_hyp_tree*> nodes;
  vcl_vector<vcl_set<int> > CPL_links; //various links to other HT nodes (completion type)
  vcl_vector<vcl_set<int> > CPT_links; //various links to other HT nodes (competition type)

  sdet_HT_graph():nodes(0), CPL_links(0), CPT_links(0){}
  ~sdet_HT_graph(){ clear(); }

  void clear()
  {
    for (unsigned i=0; i<nodes.size(); i++){
      nodes[i]->delete_tree(); 
    }
    nodes.clear(); 
    CPL_links.clear();
    CPT_links.clear();
  }

  void insert_node(sdet_hyp_tree* new_node){nodes.push_back(new_node);  }
  void insert_CPL_link(int HT1, int HT2) { CPL_links[HT1].insert(HT2); CPL_links[HT2].insert(HT1); } 
  void remove_CPL_link(int HT1, int HT2) { CPL_links[HT1].erase(HT2); CPL_links[HT2].erase(HT1); }

  void insert_CPT_link(int HT1, int HT2) { CPT_links[HT1].insert(HT2); CPT_links[HT2].insert(HT1); }
  void remove_CPT_link(int HT1, int HT2) { CPT_links[HT1].erase(HT2); CPT_links[HT2].erase(HT1); }

  //: this function is a hack (nneds to be updated)
  void resize_links(){ CPL_links.resize(nodes.size()); CPT_links.resize(nodes.size()); }

  void clear_all_CPT_links() { CPT_links.clear(); CPT_links.resize(nodes.size()); }
};

//: class to store edgel curve segment labels(eventually needs to be part of the link graph)
class sdet_edgel_labels
{
public:
  vcl_vector<vcl_set<sdet_hyp_tree_node*> > labels; //contains the hyp tree nodes
  vcl_vector<bool> claimed;                          //flags to mark edgels that ahve been claimed already

  sdet_edgel_labels(): labels(0), claimed(0){}
  ~sdet_edgel_labels(){ labels.clear(); claimed.clear(); }

  void resize(unsigned size){ 
    if (labels.size()!= size){ 
      labels.resize(size); 
      claimed.resize(size);
    }
  }

  void clear() { labels.clear(); claimed.clear(); }

  void clear_labels(){
    for (unsigned i=0; i<labels.size(); i++){
      labels[i].clear();
      claimed[i] = false;
    }
  }

};

//: class to represent a path between two HT root nodes
class sdet_HTG_link_path
{
public:
  sdet_hyp_tree* src;
  sdet_hyp_tree* tgt;
  double cost;
  vcl_vector<sdet_curvelet*> cvlets;

  sdet_HTG_link_path(sdet_hyp_tree* sHT=0, sdet_hyp_tree* tHT=0): src(sHT), tgt(tHT), cost(0.0), cvlets(0) {}

};

inline bool path_metric_less_than(sdet_HTG_link_path* p1, sdet_HTG_link_path* p2)
{
  return p1->cost < p2->cost;
}

#endif // sdet_hyp_tree_graph_h
