#ifndef vtol_topology_io_h
#define vtol_topology_io_h

// .NAME vtol_topology_io
// .LIBRARY vtol
// .HEADER gel package
// .INCLUDE vtol/vtol_topology_io.h
// .FILE vtol_topology_io.cxx
//
// .SECTION Description
// This class will write a list of topology objects into
// an XML document.

#include <vtol/vtol_topology_object_sptr.h>
#include <vcl_list.h>
#include <vcl_ostream.h>
#include <vcl_iostream.h>

class vtol_vertex_2d;
class vtol_zero_chain;
class vtol_edge_2d;
class vtol_one_chain;
class vtol_face_2d;
class vtol_two_chain;
class vtol_block;


//: \brief topological xml writer

class vtol_topology_io
{
 public:

  //: default constructor

  vtol_topology_io();

  //: default destructor

  ~vtol_topology_io();

  //: write a list of topology objects

  void write(vcl_list<vtol_topology_object_sptr> &topo_objects,
	     vcl_ostream &strm=vcl_cout);

 private:

  // write out the topology objects

  void write_vertex_2d(vtol_vertex_2d *v, vcl_ostream &strm=vcl_cout);

  void write_zero_chain(vtol_zero_chain *zc, vcl_ostream &strm=vcl_cout);

  void write_edge_2d(vtol_edge_2d *e, vcl_ostream &strm=vcl_cout);

  void write_one_chain(vtol_one_chain *oc, vcl_ostream &strm=vcl_cout);

  void write_face_2d(vtol_face_2d *f, vcl_ostream &strm=vcl_cout);

  void write_two_chain(vtol_two_chain *tc, vcl_ostream &strm=vcl_cout);

  void write_block(vtol_block *b, vcl_ostream &strm=vcl_cout);

  // write out the dtd

  void write_dtd(vcl_ostream &strm);

};
#endif
