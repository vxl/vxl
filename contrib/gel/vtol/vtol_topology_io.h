#ifndef vtol_topology_io_h
#define vtol_topology_io_h
//:
// \file
// \brief This class will write a list of topology objects into an XML document.

#include <list>
#include <ostream>
#include <iostream>
#include <vtol/vtol_topology_object_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_zero_chain_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vtol/vtol_two_chain_sptr.h>
#include <vtol/vtol_block_sptr.h>

//: \brief topological xml writer

class vtol_topology_io
{
 public:

  //: default constructor
  vtol_topology_io();

  //: default destructor
  ~vtol_topology_io();

  //: write a list of topology objects
  void write(std::list<vtol_topology_object_sptr> &topo_objects,
             std::ostream &strm=std::cout);

 private:

  // write out the topology objects
  void write_vertex_2d(vtol_vertex_2d_sptr const &v, std::ostream &strm=std::cout);
  void write_zero_chain(vtol_zero_chain_sptr const &zc, std::ostream &strm=std::cout);
  void write_edge_2d(vtol_edge_2d_sptr const &e, std::ostream &strm=std::cout);
  void write_one_chain(vtol_one_chain_sptr const &oc, std::ostream &strm=std::cout);
  void write_face_2d(vtol_face_2d_sptr const &f, std::ostream &strm=std::cout);
  void write_two_chain(vtol_two_chain_sptr const &tc, std::ostream &strm=std::cout);
  void write_block(vtol_block_sptr const &b, std::ostream &strm=std::cout);

  // write out the dtd
  void write_dtd(std::ostream &strm);
};

#endif // vtol_topology_io_h
