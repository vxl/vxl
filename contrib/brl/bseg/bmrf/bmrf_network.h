// This is contrib/brl/bseg/bmrf/bmrf_network.h
#ifndef bmrf_network_h_
#define bmrf_network_h_
//:
// \file
// \brief A Markov Random Field (MRF) network
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 1/13/04
//
// The MRF network is templated on the type of node, and maintains a vector
// of all nodes in the network.
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_deque.h>
#include <vcl_set.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_ref_count.h>
#include "bmrf_node_sptr.h"
#include "bmrf_network_sptr.h"

//: The MRF network
class bmrf_network : public vbl_ref_count
{
public:
  //: Constructor
  bmrf_network();

  //: Destructor
  ~bmrf_network(){}

  //: Returns the number of nodes in the network;
  int size();

  //: Returns the probability that the entire network is correct
  double probability();

  //: Returns all the nodes in frame \param frame
  vcl_vector<bmrf_node_sptr> nodes_in_frame(int frame) const;

private:
  //: The vector of nodes in the network
  vcl_vector<bmrf_node_sptr> nodes_;

public:
  class iterator
  {
  public:
    //: Constructor
    iterator( bmrf_network_sptr network, bmrf_node_sptr node ) : network_(network), curr_node_(node) {}

    //: Destructor
    virtual ~iterator() {}

    //: Increment
    iterator& operator++ () { next_node(); return *this; }

    //: Increment the current node 
    virtual void next_node() = 0;

    //: Dereference
    bmrf_node_sptr operator -> () const { return curr_node_; }

    //: Equality comparison
    bool operator == (const iterator& rhs) { return rhs.curr_node_ == this->curr_node_; }
    
  protected:
    bmrf_network_sptr network_;
    bmrf_node_sptr curr_node_;
  };

  // Depth first search iterator
  class depth_iterator : iterator
  {
  public:
    //: Constructor
    depth_iterator( bmrf_network_sptr network, bmrf_node_sptr node ) : iterator(network, node){}

    //: Increment the current node
    void next_node();
      
  protected:
    vcl_deque<bmrf_node_sptr> eval_queue_;
    vcl_set<bmrf_node_sptr> visited_;
  };

  // Breadth first search iterator
  class breadth_iterator : iterator
  {
  public:
    //: Constructor
    breadth_iterator( bmrf_network_sptr network, bmrf_node_sptr node ) : iterator(network, node){}

    //: Increment the current node
    void next_node();

  protected:
    vcl_deque<bmrf_node_sptr> eval_queue_;
    vcl_set<bmrf_node_sptr> visited_;
  };

  //: Depth first search begin iterator
  depth_iterator depth_begin(bmrf_node_sptr node) { return depth_iterator(this, node); }
  //: Depth first search end iterator
  depth_iterator depth_end()   { return depth_iterator(this, NULL); }

  //: Breadth first search begin iterator
  breadth_iterator breadth_begin(bmrf_node_sptr node) { return breadth_iterator(this, node); }
  //: Breadth first search end iterator
  breadth_iterator breadth_end()   { return breadth_iterator(this, NULL); }

};

#endif // bmrf_network_h_
