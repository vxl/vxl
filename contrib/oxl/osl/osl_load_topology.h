// This is oxl/osl/osl_load_topology.h
#ifndef osl_load_topology_h_
#define osl_load_topology_h_
// .NAME osl_load_topology
// .INCLUDE osl/osl_load_topology.h
// .FILE osl_load_topology.cxx
// \author fsm

#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <osl/osl_edge.h>

void osl_load_topology(char const *f, std::list<osl_edge*> &, std::list<osl_vertex*> &);
void osl_load_topology(std::istream &f   , std::list<osl_edge*> &, std::list<osl_vertex*> &);

#endif // osl_load_topology_h_
