/*
  fsm@robots.ox.ac.uk
*/

#include <vcl_iostream.h>

#include <vsl/vsl_topology.h>
#include <vsl/vsl_load_topology.h>

//--------------------------------------------------------------------------------

int main(int argc, char **argv) {
  if (argc>1) {
    vcl_cerr << "extra arguments" << vcl_endl;
    return 1;
  }

  vcl_cerr << "reading topology" << vcl_endl;
  vcl_list<vsl_Edge*> edges;
  vcl_list<vsl_Vertex*> vertices;
  vsl_load_topology(vcl_cin, edges, vertices);

  vcl_cerr << "writing edges" << vcl_endl;
  vcl_cout << edges.size() << " edges" << vcl_endl;
  for (vcl_list<vsl_Edge*>::iterator i=edges.begin(); i!=edges.end(); ++i) {
    vcl_cout << vcl_endl;
    vsl_Edge const *e = *i;
    vcl_cout << e->size() << vcl_endl;
    for (unsigned j=0; j<e->size(); ++j)
      vcl_cout << e->GetX(j) << ' ' << e->GetY(j) << vcl_endl;
  }

  return 0;
}















