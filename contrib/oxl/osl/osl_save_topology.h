// This is oxl/osl/osl_save_topology.h
#ifndef osl_save_topology_h_
#define osl_save_topology_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
// .NAME osl_save_topology
// .INCLUDE osl/osl_save_topology.h
// .FILE osl_save_topology.cxx
// \author fsm@robots.ox.ac.uk

#include <vcl_iosfwd.h>
#include <osl/osl_edge.h>

void osl_save_topology(char const *, vcl_list<osl_edge*> const &, vcl_list<osl_vertex*> const &);
void osl_save_topology(char const *, vcl_list<osl_edge*> const &);

void osl_save_topology(vcl_ostream &   , vcl_list<osl_edge*> const &, vcl_list<osl_vertex*> const &);
void osl_save_topology(vcl_ostream &   , vcl_list<osl_edge*> const &);

#endif // osl_save_topology_h_
