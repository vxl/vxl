/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vsl_load_topology.h"
#include <vcl/vcl_cassert.h>
#include <vcl/vcl_cstdio.h> // sscanf()
#include <vcl/vcl_fstream.h>
#include <vcl/vcl_vector.h>

void vsl_load_topology(char const *f, vcl_list<vsl_edge*> &e, vcl_list<vsl_vertex*> &v) {
  ifstream file(f);
  vsl_load_topology(file, e, v);
}

#define streamok \
{ if (f.bad()) { cerr << __FILE__ ":" << __LINE__ << " stream bad at this point" << endl; return; } }

void vsl_load_topology(istream &f, vcl_list<vsl_edge*> &es, vcl_list<vsl_vertex*> &vs) {
  es.clear();
  vs.clear();
  
  char buf[1024]; // line buffer
  char tmp[1024];
  
  // check version string
  f >> vcl_ws;
  f.getline(buf, sizeof(buf));
  if (strcmp("vsl_save_topology 1.0", buf) != 0) {
    cerr << __FILE__ ": version string mismatch" << endl;
    return;
  }
  streamok;

  // read number of vertices :
  f >> vcl_ws;
  f.getline(buf, sizeof(buf));
  int numverts = -1;
  if (sscanf(buf, "%d%[ ]vertices", &numverts, tmp) != 2) {
    cerr << __FILE__ ": error reading number of vertices" << endl;
    return;
  } 
  assert(numverts >= 0);
  streamok;
  // read vertices :
  cerr << "reading " << numverts << " vertices..." << endl;
  vcl_vector<vsl_vertex*> vert(numverts+1, (vsl_vertex*)0);
  for (unsigned i=0; i<numverts; ++i) {
    int stashid;
    int id;
    float x, y;
    f >> vcl_ws >> stashid >> id >> x >> y;
    assert(stashid<vert.size() && !vert[stashid]);
    vert[stashid] = new vsl_vertex(x, y, id);

    vs.push_front(vert[stashid]);
  }
  streamok;
  
  // read number of edges :
  f >> vcl_ws;
  f.getline(buf, sizeof(buf));
  int numedges = -1;
  if (sscanf(buf, "%d%[ ]edges", &numedges, tmp) != 2) {
    cerr << __FILE__ ": error reading number of edges" << endl;
    return;
  }
  assert(numedges >= 0);
  streamok;
  // read edges :
  cerr << "reading " << numedges << " edges..." << endl;
  for (unsigned i=0; i<numedges; ++i) {
    int stashid1 = -1, stashid2 = -1;
    f >> vcl_ws >> stashid1 >> stashid2;
    assert(0<=stashid1 && stashid1<vert.size() && vert[stashid1]);
    assert(0<=stashid2 && stashid2<vert.size() && vert[stashid2]);
    
    int id; // edge id
    f >> vcl_ws >> id;
    
    vsl_edge *e = new vsl_edge(2/*dummy*/, vert[stashid1], vert[stashid2]);
    e->SetId(id);
    
    e->read_ascii(f);

    es.push_front(e);
  }
  streamok;

  // done
}
