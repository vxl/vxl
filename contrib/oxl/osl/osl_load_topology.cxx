// This is oxl/osl/osl_load_topology.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "osl_load_topology.h"
#include <vcl_cassert.h>
#include <vcl_cstring.h>
#include <vcl_cstdio.h> // sscanf()
#include <vcl_fstream.h>
#include <vcl_vector.h>

void osl_load_topology(char const *f, vcl_list<osl_edge*> &e, vcl_list<osl_vertex*> &v)
{
  vcl_ifstream file(f);
  osl_load_topology(file, e, v);
}

#define streamok \
{ if (f.bad()) { vcl_cerr << __FILE__ ":" << __LINE__ << " stream bad at this point\n"; return; } }

void osl_load_topology(vcl_istream &f, vcl_list<osl_edge*> &es, vcl_list<osl_vertex*> &vs)
{
  es.clear();
  vs.clear();

  char buf[1024]; // line buffer
  char tmp[1024];

  // check version string
  f >> vcl_ws;
  f.getline(buf, sizeof(buf));
  if (vcl_strcmp("osl_save_topology 1.0", buf) != 0) {
    vcl_cerr << __FILE__ ": version string mismatch\n";
    return;
  }
  streamok;

  // read number of vertices :
  f >> vcl_ws;
  f.getline(buf, sizeof(buf));
  int numverts = -1;
  if (vcl_sscanf(buf, "%d%[ ]vertices", &numverts, tmp) != 2) {
    vcl_cerr << __FILE__ ": error reading number of vertices\n";
    return;
  }
  assert(numverts >= 0);
  streamok;
  // read vertices :
  vcl_cerr << "reading " << numverts << " vertices...\n";
  vcl_vector<osl_vertex*> vert(numverts+1, (osl_vertex*)0);
  for (int i=0; i<numverts; ++i) {
    unsigned int stashid;
    int id;
    float x, y;
    f >> vcl_ws >> vcl_hex >> stashid >> vcl_dec >> id >> x >> y;
    assert(stashid<vert.size() && !vert[stashid]);
    vert[stashid] = new osl_vertex(x, y, id);

    vs.push_front(vert[stashid]);
  }
  streamok;

  // read number of edges :
  f >> vcl_ws;
  f.getline(buf, sizeof(buf));
  int numedges = -1;
  if (vcl_sscanf(buf, "%d%[ ]edges", &numedges, tmp) != 2) {
    vcl_cerr << __FILE__ ": error reading number of edges\n";
    return;
  }
  assert(numedges >= 0);
  streamok;
  // read edges :
  vcl_cerr << "reading " << numedges << " edges...\n";
  for (int i=0; i<numedges; ++i) {
    unsigned int stashid1 = vert.size(), stashid2 = vert.size();
    f >> vcl_ws >> vcl_hex >> stashid1 >> stashid2 >> vcl_dec;
    assert(stashid1<vert.size() && vert[stashid1]);
    assert(stashid2<vert.size() && vert[stashid2]);

    int id; // edge id
    f >> vcl_ws >> id;

    osl_edge *e = new osl_edge(2/*dummy*/, vert[stashid1], vert[stashid2]);
    e->SetId(id);

    e->read_ascii(f);

    es.push_front(e);
  }
  streamok;

  // done
}
