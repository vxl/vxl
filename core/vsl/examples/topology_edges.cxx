/*
  fsm@robots.ox.ac.uk
*/

#include <vcl_iostream.h>

#include <vsl/vsl_topology.h>
#include <vsl/vsl_load_topology.h>

//--------------------------------------------------------------------------------

int main(int argc, char **argv) {
  if (argc>1) {
    cerr << "extra arguments" << endl;
    return 1;
  }

  cerr << "reading topology" << endl;
  vcl_list<vsl_Edge*> edges;
  vcl_list<vsl_Vertex*> vertices;
  vsl_load_topology(cin, edges, vertices);

  cerr << "writing edges" << endl;
  cout << edges.size() << " edges" << endl;
  for (vcl_list<vsl_Edge*>::iterator i=edges.begin(); i!=edges.end(); ++i) {
    cout << endl;
    vsl_Edge const *e = *i;
    cout << e->size() << endl;
    for (unsigned j=0; j<e->size(); ++j)
      cout << e->GetX(j) << ' ' << e->GetY(j) << endl;
  }

  return 0;
}















