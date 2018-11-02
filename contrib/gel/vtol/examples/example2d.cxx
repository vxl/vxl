//*****************************************************************************
// File name: example2d.cxx
// Description: Use of topology in 2D
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/08| Francois BERTEL          |Split of 2D and 3D
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/01| Peter TU                 |Creation
//*****************************************************************************

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_vertex_sptr.h>
#include <vtol/vtol_zero_chain.h>
#include <vtol/vtol_zero_chain_sptr.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_sptr.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_face_sptr.h>
#include <vtol/vtol_block.h>
#include <vtol/vtol_block_sptr.h>

int main()
{
  // We want to make 4 vertices and put them into two faces of 3 vertices each
  std::cout << "Creating vertices\n";

  vtol_vertex_sptr v1=new vtol_vertex_2d(0.0,0.0);

  vtol_vertex_sptr v2=new vtol_vertex_2d(0.0,1.0);
  vtol_vertex_sptr v3=new vtol_vertex_2d(1.0,1.0);
  vtol_vertex_sptr v4=new vtol_vertex_2d(1.0,0.0);

  v1->describe(std::cout, 8);

  std::cout << "Creating faces\n";

  std::vector<vtol_vertex_sptr> verts;
  // or equivalently:    vertex_list verts;
  verts.push_back(v1);
  verts.push_back(v2);
  verts.push_back(v3);

  std::cout<<"verts filled\n";

  vtol_face_sptr f1=new vtol_face_2d(verts);

  std::cout<<"Face f1 created\n";

  f1->describe(std::cout, 8);

  verts.clear();

  verts.push_back(v3);
  verts.push_back(v4);
  verts.push_back(v1);

  vtol_face_sptr f2=new vtol_face_2d(verts);

  std::cout<<"Face f2 created\n";

  f2->describe(std::cout, 8);

  std::cout << "Creating a block\n";

  std::vector<vtol_face_sptr> faces;
  // Or equivalently:    face_list faces;

  faces.push_back(f1);
  faces.push_back(f2);
  std::cout<<"faces filled\n";

  vtol_block_sptr b1=new vtol_block(faces);
  std::cout<<"Block b1 created\n";
  b1->describe(std::cout, 8);

  std::cout <<"Accessors: vertices(), zero_chains() ... blocks()\n\n";

  vertex_list vl;
  zero_chain_list zcl;
  edge_list el;
  one_chain_list ocl;
  face_list fl;
  two_chain_list tcl;
  block_list bl;
  std::cout <<"Sizes of superiors: vertex expects       1 3 3 2 2 1 1 - gets  ";
  v1->vertices(vl); std::cout << vl.size() << ' ';
  v1->zero_chains(zcl); std::cout << zcl.size() << ' ';
  v1->edges(el); std::cout << el.size() << ' ';
  v1->one_chains(ocl); std::cout << ocl.size() << ' ';
  v1->faces(fl); std::cout << fl.size() << ' ';
  v1->two_chains(tcl); std::cout << tcl.size() << ' ';
  v1->blocks(bl); std::cout << bl.size() << '\n';

  vtol_edge_sptr ed=v1->new_edge(v2);
  vtol_zero_chain_sptr zc=ed->zero_chain();
  std::cout <<"                    zero_chain expects   2 1 1 1 1 1 1 - gets  ";
  zc->vertices(vl); std::cout << vl.size() << ' ';
  zc->zero_chains(zcl); std::cout << zcl.size() << ' ';
  zc->edges(el); std::cout << el.size() << ' ';
  zc->one_chains(ocl); std::cout << ocl.size() << ' ';
  zc->faces(fl); std::cout << fl.size() << ' ';
  zc->two_chains(tcl); std::cout << tcl.size() << ' ';
  zc->blocks(bl); std::cout << bl.size() << '\n';

  vtol_edge_sptr ed13= v1->new_edge(v3);
  std::cout <<"                    edge expects         2 2 1 2 2 1 1 - gets  ";
  ed13->vertices(vl); std::cout << vl.size() << ' ';
  ed13->zero_chains(zcl); std::cout << zcl.size() << ' ';
  ed13->edges(el); std::cout << el.size() << ' ';
  ed13->one_chains(ocl); std::cout << ocl.size() << ' ';
  ed13->faces(fl); std::cout << fl.size() << ' ';
  ed13->two_chains(tcl); std::cout << tcl.size() << ' ';
  ed13->blocks(bl); std::cout << bl.size() << '\n';

  vtol_one_chain_sptr oc1=f1->get_one_chain(0);
  std::cout <<"                    one_chain expects    3 6 3 1 1 1 1 - gets  ";
  oc1->vertices(vl); std::cout << vl.size() << ' ';
  oc1->zero_chains(zcl); std::cout << zcl.size() << ' ';
  oc1->edges(el); std::cout << el.size() << ' ';
  oc1->one_chains(ocl); std::cout << ocl.size() << ' ';
  oc1->faces(fl); std::cout << fl.size() << ' ';
  oc1->two_chains(tcl); std::cout << tcl.size() << ' ';
  oc1->blocks(bl); std::cout << bl.size() << '\n';

  std::cout <<"                    face expects         3 6 3 1 1 1 1 - gets  ";
  f1->vertices(vl); std::cout << vl.size() << ' ';
  f1->zero_chains(zcl); std::cout << zcl.size() << ' ';
  f1->edges(el); std::cout << el.size() << ' ';
  f1->one_chains(ocl); std::cout << ocl.size() << ' ';
  f1->faces(fl); std::cout << fl.size() << ' ';
  f1->two_chains(tcl); std::cout << tcl.size() << ' ';
  f1->blocks(bl); std::cout << bl.size() << '\n';

  return 0;
}
