#ifndef vsl_save_topology_h_
#define vsl_save_topology_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vsl_save_topology
// .INCLUDE vsl/vsl_save_topology.h
// .FILE vsl_save_topology.cxx
// \author fsm@robots.ox.ac.uk

#include <vcl_iosfwd.h>
#include <vsl/vsl_edge.h>

void vsl_save_topology(char const *, vcl_list<vsl_edge*> const &, vcl_list<vsl_vertex*> const &);
void vsl_save_topology(char const *, vcl_list<vsl_edge*> const &);

void vsl_save_topology(vcl_ostream &   , vcl_list<vsl_edge*> const &, vcl_list<vsl_vertex*> const &);
void vsl_save_topology(vcl_ostream &   , vcl_list<vsl_edge*> const &);

#endif // vsl_save_topology_h_
