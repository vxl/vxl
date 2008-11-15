// This is basic/bgrl2/tests/test_node.h
#ifndef test_node_h_
#define test_node_h_
//:
// \file
// \brief Test code for using the templatized graph class
// \author Amir Tamrakar
// \date 06/18/05
//

#include <bgrl2/bgrl2_vertex.h>

class test_edge;

//: a test node class subclasssed from bgrl2_vertex
class test_node : public bgrl2_vertex<test_edge>
{
protected:
  int id_;

public:
  test_node(): bgrl2_vertex<test_edge>(), id_(0) {}
  test_node(int id): bgrl2_vertex<test_edge>(), id_(id) {}
  virtual ~test_node(){}

  int id(){return id_;}
  void set_id(int id){id_ = id;}

  //: Print an ascii summary to the stream
  virtual void print_summary(vcl_ostream &os) const
  {
    os << id_ << ": [" << this->degree() << "]" << vcl_endl;
  }
  
};

#endif //test_node_h_
