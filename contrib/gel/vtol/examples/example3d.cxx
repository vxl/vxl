//*****************************************************************************
// File name: example3d.cxx
// Description: Use of topology in 3D
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

#include <vtol/vtol_vertex_3d.h>
#include <vtol/vtol_zero_chain_3d.h>
#include <vtol/vtol_edge_3d.h>
#include <vtol/vtol_one_chain_3d.h>
#include <vtol/vtol_face_3d.h>
#include <vtol/vtol_two_chain_3d.h>
#include <vtol/vtol_block_3d.h>

static inline void ASSERT(bool b) { vcl_cout << ((b) ? " PASSED\n" : " *** FAILED\n"); }

static void test_topology_3d(void);

static void test_topology_3d(void)
{
  // OK we want to make two faces and put them into a block

  vcl_cout << "Creating vertices\n";

  vtol_vertex_3d *v1=new vtol_vertex_3d(0.0,0.0,0.0);
  vtol_vertex_3d *v2=new vtol_vertex_3d(0.0,1.0,1.0);
  vtol_vertex_3d *v3=new vtol_vertex_3d(1.0,1.0,0.0);
  vtol_vertex_3d *v4=new vtol_vertex_3d(1.0,0.0,0.0);
  v1->describe(vcl_cout, 8);

  vcl_cout << "Creating faces\n";

  vcl_vector<vtol_vertex_3d *> verts;

  verts.push_back(v1);
  verts.push_back(v2);
  verts.push_back(v3);
  vcl_cout<<"verts filled\n";

  vtol_face_3d *f1=new vtol_face_3d(&verts);
  vcl_cout<<"Face f1 created\n";

  f1->describe(vcl_cout, 8);

  verts.clear();

  verts.push_back(v3);
  verts.push_back(v4);
  verts.push_back(v1);


  vtol_face_3d *f2=new vtol_face_3d(&verts);

  vcl_cout << "Creating a block\n";

  vcl_vector<vtol_face_3d *> faces;

  faces.push_back(f1);
  faces.push_back(f2);
  vcl_cout<<"faces filled\n";

  vtol_block_3d *b1=new vtol_block_3d(faces);
  vcl_cout<<"Block b1 created\n";
  b1->describe(vcl_cout, 8);

  /////////////// begin to test the vertex

  vcl_cout << "************** test vertex *************\n";
  vcl_cout << "topology_type: ";
  vcl_cout << v1->topology_type();
  ASSERT(v1->topology_type() == 1);

  vcl_cout << "x() y() z() : ";
  vcl_cout << '(' << v1->x() << ',' << v1->y() << ',' << v1->z() << ')';
  ASSERT(v1->x() == 0 && v1->y() == 0 && v1->z() == 0);

  vcl_cout << "v1 == v1 ";
  ASSERT((*v1) == (*v1));

  vcl_cout << "v1 != v2 ";
  ASSERT(! ((*v1) == (*v2)));

  vcl_cout << "is_connected v1 & v2";
  ASSERT(v1->is_connected(v2));

  vcl_cout << "! is_connected v2 & v4";
  ASSERT(! v2->is_connected(v4));

  vcl_cout << "is_endpointp";
  vtol_edge_3d *ed=v1->new_edge(v2);
  ASSERT(v1->is_endpointp(*ed));

  vcl_cout << "! is_endpointp";
  ASSERT(! v3->is_endpointp(*ed));


   /////////////// begin to test the zero_chain

  vcl_cout << "************** test zero_chain  *************\n";

  vtol_zero_chain_3d *zc=ed->get_zero_chain();

  vcl_cout << "zc->v0() == v1";
  ASSERT((zc->v0() == v1));

  vcl_cout << "length: ";
  vcl_cout << zc->length();
  ASSERT(zc->length() == 2);

  vcl_cout << "*zc == *zc";
  ASSERT(*zc == *zc);

  vcl_cout << "describe zero_chain\n";
  zc->describe(vcl_cout, 8);


  /////////////// begin to test the edge

  vcl_cout << "************** test edge  *************\n";
  vcl_cout << "get_v1 == v1 && get_v2 == v2";
  ASSERT((ed->get_v1() == v1) && (ed->get_v2() == v2));

  vcl_cout << "*ed == *ed";
  ASSERT(*ed == *ed);

  vcl_cout << "Share vertex with ed2";
  vtol_edge_3d *ed2=v2->new_edge(v3);
  ASSERT(ed->share_vertex_with(ed2));

  vcl_cout << "! Share vertex with ed3";
  vtol_edge_3d *ed3=v3->new_edge(v4);
  ASSERT(! ed->share_vertex_with(ed3));

  vcl_cout << "is_endpoint v1";
  ASSERT(ed->is_endpoint(v1));

  vcl_cout << "! is_endpoint2 v1";
  ASSERT(! ed->is_endpoint2(v1));

  vcl_cout << "is_endpoint1 v1";
  ASSERT(ed->is_endpoint1(v1));

  vcl_cout << "other_endpoint(v1) == v2";
  ASSERT(ed->other_endpoint(v1)==v2);

  vcl_cout << "describe edge\n";
  ed->describe(vcl_cout, 8);


  /////////////// begin to test the one_chain
  vcl_cout << "************** test one_chain  *************\n";
  vtol_one_chain_3d *oc1=f1->get_one_chain(0);
  vtol_one_chain_3d *oc2=f2->get_one_chain(0);

  vcl_cout << "Get number of edges: ";
  vcl_cout << oc1->num_edges();
  ASSERT(oc1->num_edges() == 3);

  vcl_cout << "*oc1 == *oc1";
  ASSERT(*oc1 == *oc1);

  vcl_cout << "*oc1 != *oc2";
  ASSERT(! (*oc1 == *oc2));


  /////////////// begin to test the face
  vcl_cout << "************** test face  *************\n";

  vcl_cout << "Test the number of edges: ";
  vcl_cout << f1->get_num_edges();
  ASSERT(f1->get_num_edges() == 3);

  vcl_cout << "Shared edge with f2";
  ASSERT(f1->shares_edge_with(f2));


  /////////////// begin to test the two_chain
  vcl_cout << "************** test two_chain  *************\n";

  vtol_two_chain_3d *tc=b1->get_boundary_cycle();

  vcl_cout << "num_faces: ";
  vcl_cout << tc->num_faces();
  ASSERT(tc->num_faces() == 2);

  ///////////////// begin to test the block

  vcl_cout << "************** test block  *************\n";
  b1->describe(vcl_cout, 8);

  //////////////////////// Test accessors
  vcl_cout << "Accessors: vertices, zero_chains ... blocks\n\n";

  vertex_list_3d *vl;
  zero_chain_list_3d *zcl;
  edge_list_3d *el;
  one_chain_list_3d *ocl;
  face_list_3d *fl;
  two_chain_list_3d *tcl;
  block_list_3d *bl;

  vcl_cout <<"Vertex expects: ";
  vcl_cout << "1 3 3 2 2 1 1 - gets:  ";
  vl=v1->vertices(); vcl_cout << vl->size() << ' ';
  zcl=v1->zero_chains(); vcl_cout << zcl->size() << ' ';
  el=v1->edges(); vcl_cout << el->size() << ' ';
  ocl=v1->one_chains(); vcl_cout << ocl->size() << ' ';
  fl=v1->faces(); vcl_cout << fl->size() << ' ';
  tcl=v1->two_chains(); vcl_cout << tcl->size() << ' ';
  bl=v1->blocks(); vcl_cout << bl->size() << ' ';
  ASSERT(vl->size() == 1 && zcl->size() == 3 && el->size() == 3 &&
         ocl->size() == 2 && fl->size() == 2 && tcl->size() == 1 &&
         bl->size() == 1);
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  vcl_cout <<"zero_chain expects: ";
  vcl_cout << "2 1 1 1 1 1 1 - gets:  ";
  vl=zc->vertices(); vcl_cout << vl->size() << ' ';
  zcl=zc->zero_chains(); vcl_cout << zcl->size() << ' ';
  el=zc->edges(); vcl_cout << el->size() << ' ';
  ocl=zc->one_chains(); vcl_cout << ocl->size() << ' ';
  fl=zc->faces(); vcl_cout << fl->size() << ' ';
  tcl=zc->two_chains(); vcl_cout << tcl->size() << ' ';
  bl=zc->blocks(); vcl_cout << bl->size() << ' ';
  ASSERT(vl->size() == 2 && zcl->size() == 1 && el->size() == 1 &&
         ocl->size() == 1 && fl->size() == 1 && tcl->size() == 1 &&
         bl->size() == 1);
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  vtol_edge_3d* ed13= v1->new_edge(v3);

  vcl_cout <<"edge expects: ";
  vcl_cout << "2 1 1 2 2 1 1 - gets:  ";
  vl=ed13->vertices(); vcl_cout << vl->size() << ' ';
  zcl=ed13->zero_chains(); vcl_cout << zcl->size() << ' ';
  el=ed13->edges(); vcl_cout << el->size() << ' ';
  ocl=ed13->one_chains(); vcl_cout << ocl->size() << ' ';
  fl=ed13->faces(); vcl_cout << fl->size() << ' ';
  tcl=ed13->two_chains(); vcl_cout << tcl->size() << ' ';
  bl=ed13->blocks(); vcl_cout << bl->size() << ' ';
  ASSERT(vl->size() == 2 && zcl->size() == 1 && el->size() == 1 &&
         ocl->size() == 2 && fl->size() == 2 && tcl->size() == 1 &&
         bl->size() == 1);
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;


  vcl_cout <<"one_chain expects: ";
  vcl_cout << "3 3 3 1 1 1 1 - gets:  ";
  vl=oc1->vertices(); vcl_cout << vl->size() << ' ';
  zcl=oc1->zero_chains(); vcl_cout << zcl->size() << ' ';
  el=oc1->edges(); vcl_cout << el->size() << ' ';
  ocl=oc1->one_chains(); vcl_cout << ocl->size() << ' ';
  fl=oc1->faces(); vcl_cout << fl->size() << ' ';
  tcl=oc1->two_chains(); vcl_cout << tcl->size() << ' ';
  bl=oc1->blocks(); vcl_cout << bl->size() << ' ';
  ASSERT(vl->size() == 3 && zcl->size() == 3 && el->size() == 3 &&
         ocl->size() == 1 && fl->size() == 1 && tcl->size() == 1 &&
         bl->size() == 1);
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;


  vcl_cout <<"face expects: ";
  vcl_cout << "3 3 3 1 1 1 1 - gets:  ";
  vl=f1->vertices(); vcl_cout << vl->size() << ' ';
  zcl=f1->zero_chains(); vcl_cout << zcl->size() << ' ';
  el=f1->edges(); vcl_cout << el->size() << ' ';
  ocl=f1->one_chains(); vcl_cout << ocl->size() << ' ';
  fl=f1->faces(); vcl_cout << fl->size() << ' ';
  tcl=f1->two_chains(); vcl_cout << tcl->size() << ' ';
  bl=f1->blocks(); vcl_cout << bl->size() << ' ';
  ASSERT(vl->size() == 3 && zcl->size() == 3 && el->size() == 3 &&
         ocl->size() == 1 && fl->size() == 1 && tcl->size() == 1 &&
         bl->size() == 1);
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  vcl_cout <<"two_chain expects: ";
  vcl_cout << "4 5 5 2 2 1 1 - gets:  ";
  vl=tc->vertices(); vcl_cout << vl->size() << ' ';
  zcl=tc->zero_chains(); vcl_cout << zcl->size() << ' ';
  el=tc->edges(); vcl_cout << el->size() << ' ';
  ocl=tc->one_chains(); vcl_cout << ocl->size() << ' ';
  fl=tc->faces(); vcl_cout << fl->size() << ' ';
  tcl=tc->two_chains(); vcl_cout << tcl->size() << ' ';
  bl=tc->blocks(); vcl_cout << bl->size() << ' ';
  ASSERT(vl->size() == 4 && zcl->size() == 5 && el->size() == 5 &&
         ocl->size() == 2 && fl->size() == 2 && tcl->size() == 1 &&
         bl->size() == 1);
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  vcl_cout <<"block expects: ";
  vcl_cout << "4 5 5 2 2 1 1 - gets:  ";
  vl=b1->vertices(); vcl_cout << vl->size() << ' ';
  zcl=b1->zero_chains(); vcl_cout << zcl->size() << ' ';
  el=b1->edges(); vcl_cout << el->size() << ' ';
  ocl=b1->one_chains(); vcl_cout << ocl->size() << ' ';
  fl=b1->faces(); vcl_cout << fl->size() << ' ';
  tcl=b1->two_chains(); vcl_cout << tcl->size() << ' ';
  bl=b1->blocks(); vcl_cout << bl->size() << ' ';
  ASSERT(vl->size() == 4 && zcl->size() == 5 && el->size() == 5 &&
         ocl->size() == 2 && fl->size() == 2 && tcl->size() == 1 &&
         bl->size() == 1);
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  vcl_cout << "Done\n";
}

int main(int argc,
         char *argv[])
{
  vcl_cout<<"***************************************************\n";
  vcl_cout<<"******************* test 3D topology **************\n";
  vcl_cout<<"***************************************************\n";
  test_topology_3d();
  return 0;
}
