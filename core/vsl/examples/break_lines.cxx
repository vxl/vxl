/*
  fsm@robots.ox.ac.uk
*/
#include <vbl/vbl_arg.h>

#include <vsl/vsl_edge.h>
#include <vsl/vsl_break_edge.h>
#include <vsl/vsl_load_topology.h>
#include <vsl/vsl_save_topology.h>

int main(int argc, char **argv)
{
  vbl_arg<vcl_string> in ("-in", "input file (default is stdin)", "");
  vbl_arg<vcl_string> out("-out", "output file (default is stdout)", "");
  vbl_arg<double> thresh ("-thresh", "threshold", 0.2);
  vbl_arg_parse(argc, argv);
    
  //
  vcl_cerr << "loading topology" << vcl_endl;
  vcl_list<vsl_edge*> edges;
  vcl_list<vsl_vertex*> vertices;
  if (in() == "")
    vsl_load_topology(vcl_cin, edges, vertices);
  else
    vsl_load_topology(in().c_str(), edges, vertices);

  //
  vsl_topology_ref(edges);
  vsl_topology_ref(vertices);
  
  //
  vcl_list<vsl_edge*> broken;
  for (vcl_list<vsl_edge*>::iterator i=edges.begin(); i!=edges.end(); ++i)
    vsl_break_edge(*i, &broken);
  vsl_topology_ref(broken);
  
  //
  vcl_cerr << "saving topology" << vcl_endl;
  if (out() == "")
    vsl_save_topology(vcl_cout, broken);
  else
    vsl_save_topology(out().c_str(), broken);

  //
  vsl_topology_unref(edges);
  vsl_topology_unref(vertices);
  vsl_topology_unref(broken);

  return 0;
}
