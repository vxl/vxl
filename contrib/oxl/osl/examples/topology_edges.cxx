/*
  fsm
*/

#include <vcl_iostream.h>

#include <osl/osl_topology.h>
#include <osl/osl_load_topology.h>

//--------------------------------------------------------------------------------

int main(int argc, char **argv) {
  if (argc>1) {
    vcl_cerr << "extra arguments" << vcl_endl;
    return 1;
  }

  vcl_cerr << "reading topology" << vcl_endl;
  vcl_list<osl_edge*> edges;
  vcl_list<osl_Vertex*> vertices;
  osl_load_topology(vcl_cin, edges, vertices);

  vcl_cerr << "writing edges" << vcl_endl;
  vcl_cout << edges.size() << " edges" << vcl_endl;
  for (vcl_list<osl_edge*>::iterator i=edges.begin(); i!=edges.end(); ++i) {
    vcl_cout << vcl_endl;
    osl_edge const *e = *i;
    vcl_cout << e->size() << vcl_endl;
    for (int j=0; j<e->size(); ++j)
      vcl_cout << e->GetX(j) << ' ' << e->GetY(j) << vcl_endl;
  }

  return 0;
}















