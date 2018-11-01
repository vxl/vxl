// This is oxl/osl/examples/break_lines.cxx
//:
// \file
// \author fsm

#include <iostream>
#include <vul/vul_arg.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <osl/osl_edge.h>
#include <osl/osl_break_edge.h>
#include <osl/osl_load_topology.h>
#include <osl/osl_save_topology.h>

int main(int argc, char **argv)
{
  vul_arg<std::string> in ("-in", "input file (default is stdin)", "");
  vul_arg<std::string> out("-out", "output file (default is stdout)", "");
  vul_arg<double> thresh ("-thresh", "threshold", 0.2);
  vul_arg_parse(argc, argv);

  //
  std::cerr << "loading topology\n";
  std::list<osl_edge*> edges;
  std::list<osl_vertex*> vertices;
  if (in() == "")
    osl_load_topology(std::cin, edges, vertices);
  else
    osl_load_topology(in().c_str(), edges, vertices);

  //
  osl_topology_ref(edges);
  osl_topology_ref(vertices);

  //
  std::list<osl_edge*> broken;
  for (auto & edge : edges)
    osl_break_edge(edge, &broken);
  osl_topology_ref(broken);

  //
  std::cerr << "saving topology\n";
  if (out() == "")
    osl_save_topology(std::cout, broken);
  else
    osl_save_topology(out().c_str(), broken);

  //
  osl_topology_unref(edges);
  osl_topology_unref(vertices);
  osl_topology_unref(broken);

  return 0;
}
