// This is gel/vtol/tests/test_edge_2d.cxx
#include <vul/vul_timer.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_zero_chain_sptr.h>
#include <vtol/vtol_zero_chain.h>

#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }


int main(int, char **)
{
  int success=0, failures=0;

  vcl_cout << "testing topology timing \n";

  vul_timer t;
  //: make a network of s^2 vertices and edges on a grid
  int s = 100;
  //: make the vertices
  vcl_vector<vtol_vertex_2d_sptr> verts(s*s);
  int yoff = 0;
  t.mark();
  for(int y = 0; y<s; y++)
    {
      yoff = s*y;
      for(int x = 0; x<s; x++)
        {
          int off = x+yoff;
          vtol_vertex_2d_sptr v = new vtol_vertex_2d(x,y);        
          verts[off]=v;
        }
    }
  vcl_cout << "Time to construct " << s*s << " vertices " << t.real() 
           << " msecs\n";
  vcl_vector<vtol_edge_2d_sptr> edges(2*s*s);
  int nedges = 0;
  t.mark();
	for(int y = 1; y<s; y++)
	  {
		 yoff = s*y;
		 for(int x = 1; x<s; x++)
        {
          int off = x+yoff;
          vtol_edge_2d_sptr eh = new vtol_edge_2d(*verts[off-1], *verts[off]);
		  edges[nedges]=eh;
          vtol_edge_2d_sptr ev = new vtol_edge_2d(*verts[off-s], *verts[off]);
		  edges[nedges+1]=ev;
          nedges += 2;
        }
    }
  vcl_cout << "Time to construct " << nedges << " edges " << t.real() 
           << " msecs\n";
  t.mark();
  int n_vedges = 0;
  for(vcl_vector<vtol_vertex_2d_sptr>::iterator vit = verts.begin();
      vit != verts.end(); vit++)
    {
      vcl_vector<vtol_edge_sptr>* vedges = (*vit)->edges();
      n_vedges += vedges->size();
      delete vedges;
    }
  vcl_cout << "Time to access " << n_vedges << " vertex edges " << t.real() 
           << " msecs\n";
  vcl_cout << "finished testing topology timing \n\n";
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}

