//*****************************************************************************
// File name: example2d.cxx
// Description: Use of topology in 2D
//-----------------------------------------------------------------------------
// Language: C++
//
// Version |Date      | Author                   |Comment
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/06/08| François BERTEL          |Split of 2D and 3D
// --------+----------+--------------------------+-----------------------------
// 1.0     |2000/05/01| Peter TU                 |Creation
//*****************************************************************************

#include <vcl_iostream.h>
#include <vcl_vector.h>

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
#include <vtol/vtol_two_chain_sptr.h>
#include <vtol/vtol_block.h>
#include <vtol/vtol_block_sptr.h>

int main()
{
  // We want to make 4 vertices and put them into two faces of 3 vertices each
  vcl_cout << "Creating vertices\n";

  vtol_vertex_sptr v1=new vtol_vertex_2d(0.0,0.0);

  vtol_vertex_sptr v2=new vtol_vertex_2d(0.0,1.0);
  vtol_vertex_sptr v3=new vtol_vertex_2d(1.0,1.0);
  vtol_vertex_sptr v4=new vtol_vertex_2d(1.0,0.0);

  v1->describe(vcl_cout, 8);

  vcl_cout << "Creating faces\n";

  vcl_vector<vtol_vertex_sptr> verts;
  // or equivalently:    vertex_list verts;
  verts.push_back(v1);
  verts.push_back(v2);
  verts.push_back(v3);

  vcl_cout<<"verts filled\n";

  vtol_face_sptr f1=new vtol_face_2d(verts);

  vcl_cout<<"Face f1 created\n";

  f1->describe(vcl_cout, 8);

  verts.clear();

  verts.push_back(v3);
  verts.push_back(v4);
  verts.push_back(v1);

  vtol_face_sptr f2=new vtol_face_2d(verts);

  vcl_cout<<"Face f2 created\n";

  f2->describe(vcl_cout, 8);

  vcl_cout << "Creating a block\n";

  vcl_vector<vtol_face_sptr> faces;
  // Or equivalently:    face_list faces;

  faces.push_back(f1);
  faces.push_back(f2);
  vcl_cout<<"faces filled\n";

  vtol_block_sptr b1=new vtol_block(faces);
  vcl_cout<<"Block b1 created\n";
  b1->describe(vcl_cout, 8);

  vcl_cout <<"Accessors: vertices(), zero_chains() ... blocks()\n\n";

  vcl_cout <<"Sizes of superiors: vertex expects       1 3 3 2 2 1 1 - gets  ";
  vertex_list *vl=v1->vertices(); vcl_cout << vl->size() << ' ';
  zero_chain_list *zcl=v1->zero_chains(); vcl_cout << zcl->size() << ' ';
  edge_list *el=v1->edges(); vcl_cout << el->size() << ' ';
  one_chain_list *ocl=v1->one_chains(); vcl_cout << ocl->size() << ' ';
  face_list *fl=v1->faces(); vcl_cout << fl->size() << ' ';
  two_chain_list *tcl=v1->two_chains(); vcl_cout << tcl->size() << ' ';
  block_list *bl=v1->blocks(); vcl_cout << bl->size() << '\n';
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  vtol_edge_sptr ed=v1->new_edge(*v2);
  vtol_zero_chain_sptr zc=ed->zero_chain();
  vcl_cout <<"                    zero_chain expects   2 1 1 1 1 1 1 - gets  ";
  vl=zc->vertices(); vcl_cout << vl->size() << ' ';
  zcl=zc->zero_chains(); vcl_cout << zcl->size() << ' ';
  el=zc->edges(); vcl_cout << el->size() << ' ';
  ocl=zc->one_chains(); vcl_cout << ocl->size() << ' ';
  fl=zc->faces(); vcl_cout << fl->size() << ' ';
  tcl=zc->two_chains(); vcl_cout << tcl->size() << ' ';
  bl=zc->blocks(); vcl_cout << bl->size() << '\n';
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  vtol_edge_sptr ed13= v1->new_edge(*v3);
  vcl_cout <<"                    edge expects         2 2 1 2 2 1 1 - gets  ";
  vl=ed13->vertices(); vcl_cout << vl->size() << ' ';
  zcl=ed13->zero_chains(); vcl_cout << zcl->size() << ' ';
  el=ed13->edges(); vcl_cout << el->size() << ' ';
  ocl=ed13->one_chains(); vcl_cout << ocl->size() << ' ';
  fl=ed13->faces(); vcl_cout << fl->size() << ' ';
  tcl=ed13->two_chains(); vcl_cout << tcl->size() << ' ';
  bl=ed13->blocks(); vcl_cout << bl->size() << '\n';
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  vtol_one_chain_sptr oc1=f1->get_one_chain(0);
  vcl_cout <<"                    one_chain expects    3 6 3 1 1 1 1 - gets  ";
  vl=oc1->vertices(); vcl_cout << vl->size() << ' ';
  zcl=oc1->zero_chains(); vcl_cout << zcl->size() << ' ';
  el=oc1->edges(); vcl_cout << el->size() << ' ';
  ocl=oc1->one_chains(); vcl_cout << ocl->size() << ' ';
  fl=oc1->faces(); vcl_cout << fl->size() << ' ';
  tcl=oc1->two_chains(); vcl_cout << tcl->size() << ' ';
  bl=oc1->blocks(); vcl_cout << bl->size() << '\n';
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  vcl_cout <<"                    face expects         3 6 3 1 1 1 1 - gets  ";
  vl=f1->vertices(); vcl_cout << vl->size() << ' ';
  zcl=f1->zero_chains(); vcl_cout << zcl->size() << ' ';
  el=f1->edges(); vcl_cout << el->size() << ' ';
  ocl=f1->one_chains(); vcl_cout << ocl->size() << ' ';
  fl=f1->faces(); vcl_cout << fl->size() << ' ';
  tcl=f1->two_chains(); vcl_cout << tcl->size() << ' ';
  bl=f1->blocks(); vcl_cout << bl->size() << '\n';
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  return 0;
}
