#include <vcl_fstream.h>

#include <vtol/vtol_vertex_2d_ref.h>
#include <vtol/vtol_vertex_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_ref.h>
#include <vtol/vtol_zero_chain_ref.h>
#include <vtol/vtol_zero_chain.h>
#include <vtol/vtol_one_chain_ref.h>
#include <vtol/vtol_one_chain.h>



#define Assert(x) do { if (x) vcl_cerr << "test PASSED\n"; else vcl_cerr << "test FAILED [" #x "]\n"; } while (0)


int main(int, char **)
{
  vcl_cerr << "testing one  chain" << endl;
  
  vtol_vertex_2d_ref v1 = new vtol_vertex_2d(0.0,0.0);
  vtol_vertex_2d_ref v2 = new vtol_vertex_2d(1.0,1.0);
  vtol_vertex_2d_ref v3 = new vtol_vertex_2d(2.0,2.0);
  vtol_vertex_2d_ref v4 = new vtol_vertex_2d(3.0,3.0);
 
  vtol_edge_ref e12 = new vtol_edge_2d(*v1,*v2);
  vtol_edge_ref e23 = new vtol_edge_2d(*v2,*v3);
  vtol_edge_ref e34 = new vtol_edge_2d(*v3,*v4);
  vtol_edge_ref e41 = new vtol_edge_2d(*v4,*v1);
  
  edge_list e_list;
  
  e_list.push_back(e12);
  e_list.push_back(e23);
  e_list.push_back(e34);
  e_list.push_back(e41);

  vtol_one_chain_ref oc1 = new vtol_one_chain(e_list);
  
  vcl_vector<signed char> dirs;
  dirs.push_back(1);
  dirs.push_back(1);
  dirs.push_back(1);
  dirs.push_back(1);

  vtol_one_chain_ref oc2 = new vtol_one_chain(e_list,dirs);

  vtol_one_chain_ref oc3 = new vtol_one_chain(*oc2);

  Assert(*oc2==*oc3);






  vcl_cerr << "Finished testing zero chain 2d" << endl;
  return 0;
}

