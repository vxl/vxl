// This is oxl/osl/osl_load_topology.h
#ifndef osl_load_topology_h_
#define osl_load_topology_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
// .NAME osl_load_topology
// .INCLUDE osl/osl_load_topology.h
// .FILE osl_load_topology.cxx
// \author fsm

#include <iostream>
#include <iosfwd>
#include <vcl_compiler.h>
#include <osl/osl_edge.h>

void osl_load_topology(char const *f, std::list<osl_edge*> &, std::list<osl_vertex*> &);
void osl_load_topology(std::istream &f   , std::list<osl_edge*> &, std::list<osl_vertex*> &);

#endif // osl_load_topology_h_
