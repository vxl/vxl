#ifndef vsl_save_topology_h_
#define vsl_save_topology_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vcl/vcl_iosfwd.h>
#include <vsl/vsl_edge.h>

void vsl_save_topology(char const *, vcl_list<vsl_edge*> const &, vcl_list<vsl_vertex*> const &);
void vsl_save_topology(char const *, vcl_list<vsl_edge*> const &);

void vsl_save_topology(ostream &   , vcl_list<vsl_edge*> const &, vcl_list<vsl_vertex*> const &);
void vsl_save_topology(ostream &   , vcl_list<vsl_edge*> const &);

#endif
