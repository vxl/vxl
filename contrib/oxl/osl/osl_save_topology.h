// This is oxl/osl/osl_save_topology.h
#ifndef osl_save_topology_h_
#define osl_save_topology_h_
// .NAME osl_save_topology
// .INCLUDE osl/osl_save_topology.h
// .FILE osl_save_topology.cxx
// \author fsm

#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <osl/osl_edge.h>

void osl_save_topology(char const *, std::list<osl_edge*> const &, std::list<osl_vertex*> const &);
void osl_save_topology(char const *, std::list<osl_edge*> const &);

void osl_save_topology(std::ostream &   , std::list<osl_edge*> const &, std::list<osl_vertex*> const &);
void osl_save_topology(std::ostream &   , std::list<osl_edge*> const &);

#endif // osl_save_topology_h_
