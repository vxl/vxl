#ifndef vsl_load_topology_h_
#define vsl_load_topology_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vsl_load_topology
// .INCLUDE vsl/vsl_load_topology.h
// .FILE vsl_load_topology.cxx
// \author fsm@robots.ox.ac.uk

#include <vcl/vcl_iosfwd.h>
#include <vsl/vsl_edge.h>

void vsl_load_topology(char const *f, vcl_list<vsl_edge*> &, vcl_list<vsl_vertex*> &);
void vsl_load_topology(istream &f   , vcl_list<vsl_edge*> &, vcl_list<vsl_vertex*> &);

#endif // vsl_load_topology_h_
