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
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_vector.h>

#include <vtol/vtol_vertex_3d_ref.h>
#include <vtol/vtol_zero_chain_3d_ref.h>
#include <vtol/vtol_edge_3d_ref.h>
#include <vtol/vtol_one_chain_3d_ref.h>
#include <vtol/vtol_face_3d_ref.h>
#include <vtol/vtol_two_chain_3d_ref.h>
#include <vtol/vtol_block_3d_ref.h>

#include <vtol/vtol_vertex_3d.h>
#include <vtol/vtol_zero_chain_3d.h>
#include <vtol/vtol_edge_3d.h>
#include <vtol/vtol_one_chain_3d.h>
#include <vtol/vtol_face_3d.h>
#include <vtol/vtol_two_chain_3d.h>
#include <vtol/vtol_block_3d.h>

static inline void ASSERT(bool b) { cout << ((b) ? " PASSED\n" : " *** FAILED\n"); }

static void test_topology_3d(void);

static void test_topology_3d(void)
{
  // OK we want to make two faces and put them into a block
  
  cout << "Creating vertices" << endl;

  vtol_vertex_3d *v1=new vtol_vertex_3d(0.0,0.0,0.0);
  vtol_vertex_3d *v2=new vtol_vertex_3d(0.0,1.0,1.0);
  vtol_vertex_3d *v3=new vtol_vertex_3d(1.0,1.0,0.0);
  vtol_vertex_3d *v4=new vtol_vertex_3d(1.0,0.0,0.0);
  v1->describe(cout, 8);

  cout << "Creating faces" << endl;

  vcl_vector<vtol_vertex_3d *> verts;
  
  verts.push_back(v1);
  verts.push_back(v2);
  verts.push_back(v3);
  cout<<"verts filled"<<endl;

  vtol_face_3d *f1=new vtol_face_3d(&verts);
  cout<<"Face f1 created"<<endl;

  f1->describe(cout, 8);

  verts.clear();
  
  verts.push_back(v3);
  verts.push_back(v4);
  verts.push_back(v1);


  vtol_face_3d *f2=new vtol_face_3d(&verts);
  
  cout << "Creating a block" << endl;
  
  vcl_vector<vtol_face_3d *> faces;
  
  faces.push_back(f1);
  faces.push_back(f2);
  cout<<"faces filled"<<endl;
  
  vtol_block_3d *b1=new vtol_block_3d(faces);
  cout<<"Block b1 created"<<endl;
  b1->describe(cout, 8);

  /////////////// begin to test the vertex 
 
  cout << "************** test vertex *************" << endl;
  cout << "topology_type: ";
  cout << v1->topology_type(); 
  ASSERT(v1->topology_type() == 1);
  
  cout << "x() y() z() : ";
  cout << '(' << v1->x() << ',' << v1->y() << ',' << v1->z() << ')';
  ASSERT(v1->x() == 0 && v1->y() == 0 && v1->z() == 0);
   
  cout << "v1 == v1 ";
  ASSERT((*v1) == (*v1));

  cout << "v1 != v2 ";
  ASSERT(! ((*v1) == (*v2)));
  
  cout << "is_connected v1 & v2";
  ASSERT(v1->is_connected(v2));

  cout << "! is_connected v2 & v4";
  ASSERT(! v2->is_connected(v4));
  
  cout << "is_endpointp";
  vtol_edge_3d *ed=v1->new_edge(v2);
  ASSERT(v1->is_endpointp(*ed));

  cout << "! is_endpointp";
  ASSERT(! v3->is_endpointp(*ed));
 

   /////////////// begin to test the zero_chain 
 
  cout << "************** test zero_chain  *************" << endl;
  
  vtol_zero_chain_3d *zc=ed->get_zero_chain();

  cout << "zc->v0() == v1";
  ASSERT((zc->v0() == v1));

  cout << "length: ";
  cout << zc->length();
  ASSERT(zc->length() == 2);

  cout << "*zc == *zc";
  ASSERT(*zc == *zc);

  cout << "describe zero_chain" << endl;
  zc->describe(cout, 8);
  
  
  /////////////// begin to test the edge 
  
  cout << "************** test edge  *************" << endl;
  cout << "get_v1 == v1 && get_v2 == v2";
  ASSERT((ed->get_v1() == v1) && (ed->get_v2() == v2));

  cout << "*ed == *ed";
  ASSERT(*ed == *ed);
  
  cout << "Share vertex with ed2";
  vtol_edge_3d *ed2=v2->new_edge(v3);
  ASSERT(ed->share_vertex_with(ed2));

  cout << "! Share vertex with ed3";
  vtol_edge_3d *ed3=v3->new_edge(v4);
  ASSERT(! ed->share_vertex_with(ed3));
  
  cout << "is_endpoint v1";
  ASSERT(ed->is_endpoint(v1));

  cout << "! is_endpoint2 v1";
  ASSERT(! ed->is_endpoint2(v1));

  cout << "is_endpoint1 v1";
  ASSERT(ed->is_endpoint1(v1));
  
  cout << "other_endpoint(v1) == v2";
  ASSERT(ed->other_endpoint(v1)==v2);
  
  cout << "describe edge" << endl;
  ed->describe(cout, 8);


  /////////////// begin to test the one_chain 
  cout << "************** test one_chain  *************" << endl;
  vtol_one_chain_3d *oc1=f1->get_one_chain(0);
  vtol_one_chain_3d *oc2=f2->get_one_chain(0);
  
  cout << "Get number of edges: ";
  cout << oc1->num_edges();
  ASSERT(oc1->num_edges() == 3);
  
  cout << "*oc1 == *oc1";
  ASSERT(*oc1 == *oc1);

  cout << "*oc1 != *oc2";
  ASSERT(! (*oc1 == *oc2));


  /////////////// begin to test the face 
  cout << "************** test face  *************" << endl;

  cout << "Test the number of edges: ";
  cout << f1->get_num_edges();
  ASSERT(f1->get_num_edges() == 3);
  
  cout << "Shared edge with f2";
  ASSERT(f1->shares_edge_with(f2));
  
  
  /////////////// begin to test the two_chain  
  cout << "************** test two_chain  *************" << endl;
  
  vtol_two_chain_3d *tc=b1->get_boundary_cycle();
  
  cout << "num_faces: ";
  cout << tc->num_faces();
  ASSERT(tc->num_faces() == 2);
  
  ///////////////// begin to test the block 

  cout << "************** test block  *************" << endl;
  b1->describe(cout, 8); 

  //////////////////////// Test accessors 
  cout << "Accessors: vertices, zero_chains ... blocks" << endl << endl;

  vertex_list_3d *vl;
  zero_chain_list_3d *zcl;
  edge_list_3d *el;
  one_chain_list_3d *ocl;
  face_list_3d *fl;
  two_chain_list_3d *tcl;
  block_list_3d *bl;

  cout <<"Vertex expects: ";
  cout << "1 3 3 2 2 1 1 - gets:  ";
  vl=v1->vertices(); cout << vl->size() << ' ';
  zcl=v1->zero_chains(); cout << zcl->size() << ' ';
  el=v1->edges(); cout << el->size() << ' ';
  ocl=v1->one_chains(); cout << ocl->size() << ' ';
  fl=v1->faces(); cout << fl->size() << ' ';
  tcl=v1->two_chains(); cout << tcl->size() << ' ';
  bl=v1->blocks(); cout << bl->size() << ' ';
  ASSERT(vl->size() == 1 && zcl->size() == 3 && el->size() == 3 && ocl->size() == 2 && fl->size() == 2 && tcl->size() == 1 && bl->size() == 1);
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  cout <<"zero_chain expects: ";
  cout << "2 1 1 1 1 1 1 - gets:  ";
  vl=zc->vertices(); cout << vl->size() << ' ';
  zcl=zc->zero_chains(); cout << zcl->size() << ' ';
  el=zc->edges(); cout << el->size() << ' ';
  ocl=zc->one_chains(); cout << ocl->size() << ' ';
  fl=zc->faces(); cout << fl->size() << ' ';
  tcl=zc->two_chains(); cout << tcl->size() << ' ';
  bl=zc->blocks(); cout << bl->size() << ' ';
  ASSERT(vl->size() == 2 && zcl->size() == 1 && el->size() == 1 && ocl->size() == 1 && fl->size() == 1 && tcl->size() == 1 && bl->size() == 1);
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  vtol_edge_3d* ed13= v1->new_edge(v3);

  cout <<"edge expects: ";
  cout << "2 1 1 2 2 1 1 - gets:  ";
  vl=ed13->vertices(); cout << vl->size() << ' ';
  zcl=ed13->zero_chains(); cout << zcl->size() << ' ';
  el=ed13->edges(); cout << el->size() << ' ';
  ocl=ed13->one_chains(); cout << ocl->size() << ' ';
  fl=ed13->faces(); cout << fl->size() << ' ';
  tcl=ed13->two_chains(); cout << tcl->size() << ' ';
  bl=ed13->blocks(); cout << bl->size() << ' ';
  ASSERT(vl->size() == 2 && zcl->size() == 1 && el->size() == 1 && ocl->size() == 2 && fl->size() == 2 && tcl->size() == 1 && bl->size() == 1);
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;


  cout <<"one_chain expects: ";
  cout << "3 3 3 1 1 1 1 - gets:  ";
  vl=oc1->vertices(); cout << vl->size() << ' ';
  zcl=oc1->zero_chains(); cout << zcl->size() << ' ';
  el=oc1->edges(); cout << el->size() << ' ';
  ocl=oc1->one_chains(); cout << ocl->size() << ' ';
  fl=oc1->faces(); cout << fl->size() << ' ';
  tcl=oc1->two_chains(); cout << tcl->size() << ' ';
  bl=oc1->blocks(); cout << bl->size() << ' ';
  ASSERT(vl->size() == 3 && zcl->size() == 3 && el->size() == 3 && ocl->size() == 1 && fl->size() == 1 && tcl->size() == 1 && bl->size() == 1);
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;


  cout <<"face expects: ";
  cout << "3 3 3 1 1 1 1 - gets:  ";
  vl=f1->vertices(); cout << vl->size() << ' ';
  zcl=f1->zero_chains(); cout << zcl->size() << ' ';
  el=f1->edges(); cout << el->size() << ' ';
  ocl=f1->one_chains(); cout << ocl->size() << ' ';
  fl=f1->faces(); cout << fl->size() << ' ';
  tcl=f1->two_chains(); cout << tcl->size() << ' ';
  bl=f1->blocks(); cout << bl->size() << ' ';
  ASSERT(vl->size() == 3 && zcl->size() == 3 && el->size() == 3 && ocl->size() == 1 && fl->size() == 1 && tcl->size() == 1 && bl->size() == 1);
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  cout <<"two_chain expects: ";
  cout << "4 5 5 2 2 1 1 - gets:  ";
  vl=tc->vertices(); cout << vl->size() << ' ';
  zcl=tc->zero_chains(); cout << zcl->size() << ' ';
  el=tc->edges(); cout << el->size() << ' ';
  ocl=tc->one_chains(); cout << ocl->size() << ' ';
  fl=tc->faces(); cout << fl->size() << ' ';
  tcl=tc->two_chains(); cout << tcl->size() << ' ';
  bl=tc->blocks(); cout << bl->size() << ' ';
  ASSERT(vl->size() == 4 && zcl->size() == 5 && el->size() == 5 && ocl->size() == 2 && fl->size() == 2 && tcl->size() == 1 && bl->size() == 1);
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  cout <<"block expects: ";
  cout << "4 5 5 2 2 1 1 - gets:  ";
  vl=b1->vertices(); cout << vl->size() << ' ';
  zcl=b1->zero_chains(); cout << zcl->size() << ' ';
  el=b1->edges(); cout << el->size() << ' ';
  ocl=b1->one_chains(); cout << ocl->size() << ' ';
  fl=b1->faces(); cout << fl->size() << ' ';
  tcl=b1->two_chains(); cout << tcl->size() << ' ';
  bl=b1->blocks(); cout << bl->size() << ' ';
  ASSERT(vl->size() == 4 && zcl->size() == 5 && el->size() == 5 && ocl->size() == 2 && fl->size() == 2 && tcl->size() == 1 && bl->size() == 1);
  delete vl; delete zcl; delete el; delete ocl; delete fl; delete tcl; delete bl;

  cout << "Done" << endl;
}

int main(int argc,
         char *argv[])
{
  cout<<"***************************************************"<<endl;
  cout<<"******************* test 3D topology **************"<<endl;
  cout<<"***************************************************"<<endl;
  test_topology_3d();
  return 0;
}
