// This is oxl/osl/examples/break_lines.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation "vul_arg.h"//otherwise "unresolved typeinfo vul_arg_base"
#endif
//:
// \file
// \author fsm

#include <vul/vul_arg.h>
#include <vcl_iostream.h>
#include <osl/osl_edge.h>
#include <osl/osl_break_edge.h>
#include <osl/osl_load_topology.h>
#include <osl/osl_save_topology.h>

int main(int argc, char **argv)
{
  vul_arg<vcl_string> in ("-in", "input file (default is stdin)", "");
  vul_arg<vcl_string> out("-out", "output file (default is stdout)", "");
  vul_arg<double> thresh ("-thresh", "threshold", 0.2);
  vul_arg_parse(argc, argv);

  //
  vcl_cerr << "loading topology\n";
  vcl_list<osl_edge*> edges;
  vcl_list<osl_vertex*> vertices;
  if (in() == "")
    osl_load_topology(vcl_cin, edges, vertices);
  else
    osl_load_topology(in().c_str(), edges, vertices);

  //
  osl_topology_ref(edges);
  osl_topology_ref(vertices);

  //
  vcl_list<osl_edge*> broken;
  for (vcl_list<osl_edge*>::iterator i=edges.begin(); i!=edges.end(); ++i)
    osl_break_edge(*i, &broken);
  osl_topology_ref(broken);

  //
  vcl_cerr << "saving topology\n";
  if (out() == "")
    osl_save_topology(vcl_cout, broken);
  else
    osl_save_topology(out().c_str(), broken);

  //
  osl_topology_unref(edges);
  osl_topology_unref(vertices);
  osl_topology_unref(broken);

  return 0;
}
