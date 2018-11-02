// This is brl/bbas/bgrl2/tests/test_node.h
#ifndef test_node_h_
#define test_node_h_
//:
// \file
// \brief Test code for using the templated graph class
// \author Amir Tamrakar
// \date June 18, 2005

#include <iostream>
#include <bgrl2/bgrl2_vertex.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class test_edge;

//: a test node class subclassed from bgrl2_vertex
class test_node : public bgrl2_vertex<test_edge>
{
 protected:
  int id_;

 public:
  test_node(): bgrl2_vertex<test_edge>(), id_(0) {}
  test_node(int id): bgrl2_vertex<test_edge>(), id_(id) {}
  ~test_node() override= default;

  int id() const {return id_;}
  void set_id(int id){id_ = id;}

  //: Print an ascii summary to the stream
  void print_summary(std::ostream &os) const override
  {
    os << id_ << ": [" << this->degree() << ']' << std::endl;
  }
};

#endif //test_node_h_
