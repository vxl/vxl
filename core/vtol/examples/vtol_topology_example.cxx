
// Example use of the vtol_topology classes 
//
// Author: Peter Tu May 1 2000

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_vector.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_block_2d.h>
#include <vtol/vtol_zero_chain_2d.h>
#include <vtol/vtol_one_chain_2d.h>
#include <vtol/vtol_two_chain_2d.h>
#include <vtol/vtol_edge_2d.h>



#include <vtol/vtol_vertex_3d.h>
#include <vtol/vtol_face_3d.h>
#include <vtol/vtol_block_3d.h>
#include <vtol/vtol_zero_chain_3d.h>
#include <vtol/vtol_one_chain_3d.h>
#include <vtol/vtol_two_chain_3d.h>
#include <vtol/vtol_edge_3d.h>


void test_topology_2d()
{
  // OK we want to make two faces and put them into a block
  
  cout << "Creating vertices" << endl;

  vtol_vertex_2d *v1 = new vtol_vertex_2d(0.0,0.0);
  vtol_vertex_2d *v2 = new vtol_vertex_2d(0.0,1.0);
  vtol_vertex_2d *v3 = new vtol_vertex_2d(1.0,1.0);
  vtol_vertex_2d *v4 = new vtol_vertex_2d(1.0,0.0);
  v1->describe();

  //  vtol_zero_chain_2d *zc = new vtol_zero_chain_2d(v1,v2);
  // zc->describe();

  // zc->un_protect();

  // return 0;


  
  cout << "Creating faces" << endl;

  vertex_list_2d verts;
  
  verts.push_back(v1);
  verts.push_back(v2);
  verts.push_back(v3);

  
  vtol_face_2d *f1 = new vtol_face_2d(verts);

  f1->describe();

  verts.clear();
  
  verts.push_back(v3);
  verts.push_back(v4);
  verts.push_back(v1);


  vtol_face_2d *f2 = new vtol_face_2d(verts);
  
  cout << "Creating a block" << endl;
  
  face_list_2d faces;
  
  faces.push_back(f1);
  faces.push_back(f2);
  
  vtol_block_2d *b1 = new vtol_block_2d(faces);
  b1->describe();

  
  /////////////// begin to test the vertex 
 
  cout << "************** test vertex *************" << endl;
  cout << " topology_type: " << endl;
 
  cout << v1->topology_type() << endl; 
  
  cout << " x() y() (0,0):" << endl;
  cout << v1->x() << " " << v1->y() << endl;
   
  cout << " v1 == v1 " << endl;
  cout << ((*v1) == (*v1)) << endl;

  cout << " v1 == v2 " << endl;
  cout << ((*v1) == (*v2)) << endl;
  
  cout << "is coneceted v1 & v2 - yes" << endl;
  cout << v1->is_connected(*v2) << endl; 

  cout << "is coneceted v2 & v4 - no " << endl;
  cout << v2->is_connected(*v4) << endl;
  
  cout << "is_endpointp - yes" << endl;
  vtol_edge_2d *ed = v1->new_edge(*v2);
  cout << v1->is_endpointp(*ed) << endl;


  cout << "is_endpointp - no" << endl;
  cout << v3->is_endpointp(*ed) << endl;
 

   /////////////// begin to test the zero chain 
 
  cout << "************** test zero_chain  *************" << endl;
  
  vtol_zero_chain_2d* zc = ed->zero_chain();

  cout << "Test zc->v0 == v1 - yes" << endl;
  cout << (zc->v0() == v1) << endl;

  cout << "length - 2" << endl;
  cout << zc->length() << endl;

  cout << "zc == zc" << endl;
  cout << (zc == zc) << endl;

  cout << "describe zero chain" << endl;
  zc->describe();
  

  
  /////////////// begin to test the edge 
  
  cout << "************** test edge  *************" << endl;
  cout << " get_v1 == v1 && get_v2 == v2  - yes" << endl;
  cout << ((ed->v1() == v1) && (ed->v2() == v2)) << endl;

  cout << "ed == ed - yes" << endl;
  cout << (ed == ed) << endl;
  
  cout << "Share vertex with ed2 - yes" << endl;
  vtol_edge_2d *ed2 = v2->new_edge(*v3);
  cout << ed->share_vertex_with(*ed2) << endl;

  cout << "Share vertex with ed3 - no" << endl;
  vtol_edge_2d *ed3 = v3->new_edge(*v4);
  cout << ed->share_vertex_with(*ed3) << endl;
  
  cout << "Is endpoint v1 - yes " << endl;
  cout << ed->is_endpoint(*v1) << endl;

  cout << "is endpoint2 v1 - no " << endl;
  cout << ed->is_endpoint2(*v1) << endl;


  cout << "is endpoint1 v1 - yes " << endl;
  cout << ed->is_endpoint1(*v1) << endl;
  
  cout << "other_endpoint(v1) == v2 - yes" << endl;
  cout << (ed->other_endpoint(*v1)==v2) << endl;
  
  cout << "describe edge" << endl;
  ed->describe();

  
  
  /////////////// begin to test the one_chain 
  cout << "************** test one chain  *************" << endl;
  vtol_one_chain_2d *oc1 = f1->get_one_chain(0);
  vtol_one_chain_2d *oc2 = f2->get_one_chain(0);
  
  cout << "Get number of edges - 3 " << endl;
  cout << oc1->num_edges() << endl;
  
  cout << " oc1 == oc1  - yes" << endl;
  cout << (oc1 == oc1) << endl;

  cout << "oc1 == oc2  - no " << endl;
  cout << (oc1 == oc2) << endl;


  
  /////////////// begin to test the face 
  cout << "************** test face  *************" << endl;

  cout << "Test the number of edges - 3 " << endl;
  cout << f1->get_num_edges() << endl;
  
  cout << "Shared edge with f2 - yes " << endl;
  cout << f1->shares_edge_with(*f2) << endl;
  
  
  /////////////// begin to test the two chain  
  cout << "************** test two face  *************" << endl;
  
  vtol_two_chain_2d *tc = b1->get_boundary_cycle();
  
  cout << "num_faces - 2" << endl;
  cout << tc->num_faces() << endl;
  
  ///////////////// begin to test the block 

  cout << "************** test block  *************" << endl;
  b1->print(); 

  //////////////////////// Test accessors 
  cout <<" Accessors: vertices, zero_chains ... blocks " << endl;

  cout << endl;
  cout <<" Vertex expects: " << endl;
  cout << "1 3 3 2 2 1 1 - gets:  " << endl;
  cout << v1->vertices()->size() << " ";
  cout << v1->zero_chains()->size() << " ";
  cout << v1->edges()->size() << " ";
  cout << v1->one_chains()->size() << " ";
  cout << v1->faces()->size() << " ";
  cout << v1->two_chains()->size() << " ";
  cout << v1->blocks()->size() << endl;


  cout << endl;
  cout <<" zero_chain expects: " << endl;
  cout << "2 1 1 1 1 1 1 - gets:  " << endl;
  cout << zc->vertices()->size() << " ";
  cout << zc->zero_chains()->size() << " ";
  cout << zc->edges()->size() << " ";
  cout << zc->one_chains()->size() << " ";
  cout << zc->faces()->size() << " ";
  cout << zc->two_chains()->size() << " ";
  cout << zc->blocks()->size() << endl;

  vtol_edge_2d* ed13 = v1->new_edge(*v3);

  cout << endl;
  cout <<" edge expects: " << endl;
  cout << "2 1 1 2 2 1 1 - gets:  " << endl;
  cout << ed13->vertices()->size() << " ";
  cout << ed13->zero_chains()->size() << " ";
  cout << ed13->edges()->size() << " ";
  cout << ed13->one_chains()->size() << " ";
  cout << ed13->faces()->size() << " ";
  cout << ed13->two_chains()->size() << " ";
  cout << ed13->blocks()->size() << endl;

  cout << endl;
  cout <<" one chain expects: " << endl;
  cout << "3 3 3 1 1 1 1 - gets:  " << endl;
  cout << oc1->vertices()->size() << " ";
  cout << oc1->zero_chains()->size() << " ";
  cout << oc1->edges()->size() << " ";
  cout << oc1->one_chains()->size() << " ";
  cout << oc1->faces()->size() << " ";
  cout << oc1->two_chains()->size() << " ";
  cout << oc1->blocks()->size() << endl;


  cout << endl;
  cout <<" face expects: " << endl;
  cout << "3 3 3 1 1 1 1 - gets:  " << endl;
  cout << f1->vertices()->size() << " ";
  cout << f1->zero_chains()->size() << " ";
  cout << f1->edges()->size() << " ";
  cout << f1->one_chains()->size() << " ";
  cout << f1->faces()->size() << " ";
  cout << f1->two_chains()->size() << " ";
  cout << f1->blocks()->size() << endl;


  cout << endl;
  cout <<" two chain expects: " << endl;
  cout << "4 5 5 2 2 1 1 - gets:  " << endl;
  cout << tc->vertices()->size() << " ";
  cout << tc->zero_chains()->size() << " ";
  cout << tc->edges()->size() << " ";
  cout << tc->one_chains()->size() << " ";
  cout << tc->faces()->size() << " ";
  cout << tc->two_chains()->size() << " ";
  cout << tc->blocks()->size() << endl;



  cout << endl;
  cout <<" block expects: " << endl;
  cout << "4 5 5 2 2 1 1 - gets:  " << endl;
  cout << b1->vertices()->size() << " ";
  cout << b1->zero_chains()->size() << " ";
  cout << b1->edges()->size() << " ";
  cout << b1->one_chains()->size() << " ";
  cout << b1->faces()->size() << " ";
  cout << b1->two_chains()->size() << " ";
  cout << b1->blocks()->size() << endl;




  ///////////////////////  Delete the block

    cout << "************ Deleting the Block ****************" << endl;
  b1->un_protect();


  cout << "Done" << endl;



}



void test_topology_3d()
{
  // OK we want to make two faces and put them into a block
  
  cout << "Creating vertices" << endl;

  vtol_vertex_3d *v1 = new vtol_vertex_3d(0.0,0.0,0.0);
  vtol_vertex_3d *v2 = new vtol_vertex_3d(0.0,1.0,1.0);
  vtol_vertex_3d *v3 = new vtol_vertex_3d(1.0,1.0,0.0);
  vtol_vertex_3d *v4 = new vtol_vertex_3d(1.0,0.0,0.0);
  v1->describe();

  
  cout << "Creating faces" << endl;

  vcl_vector<vtol_vertex_3d*> verts;
  
  verts.push_back(v1);
  verts.push_back(v2);
  verts.push_back(v3);

  
  vtol_face_3d *f1 = new vtol_face_3d(&verts);

  f1->describe();

  verts.clear();
  
  verts.push_back(v3);
  verts.push_back(v4);
  verts.push_back(v1);


  vtol_face_3d *f2 = new vtol_face_3d(&verts);
  
  cout << "Creating a block" << endl;
  
  vcl_vector<vtol_face_3d*> faces;
  
  faces.push_back(f1);
  faces.push_back(f2);
  
  vtol_block_3d *b1 = new vtol_block_3d(faces);
  b1->describe();

  
  /////////////// begin to test the vertex 
 
  cout << "************** test vertex *************" << endl;
  cout << " topology_type: " << endl;
 
  cout << v1->topology_type() << endl; 
  
  cout << " x() y() (0,0,0):" << endl;
  cout << v1->x() << " " << v1->y() << " " << v1->y() << endl;
   
  cout << " v1 == v1 " << endl;
  cout << ((*v1) == (*v1)) << endl;

  cout << " v1 == v2 " << endl;
  cout << ((*v1) == (*v2)) << endl;
  
  cout << "is coneceted v1 & v2 - yes" << endl;
  cout << v1->is_connected(v2) << endl; 

  cout << "is coneceted v2 & v4 - no " << endl;
  cout << v2->is_connected(v4) << endl;
  
  cout << "is_endpointp - yes" << endl;
  vtol_edge_3d *ed = v1->new_edge(v2);
  cout << v1->is_endpointp(*ed) << endl;


  cout << "is_endpointp - no" << endl;
  cout << v3->is_endpointp(*ed) << endl;
 

   /////////////// begin to test the zero chain 
 
  cout << "************** test zero_chain  *************" << endl;
  
  vtol_zero_chain_3d* zc = ed->get_zero_chain();

  cout << "Test zc->v0 == v1 - yes" << endl;
  cout << (zc->v0() == v1) << endl;

  cout << "length - 2" << endl;
  cout << zc->length() << endl;

  cout << "zc == zc" << endl;
  cout << (zc == zc) << endl;

  cout << "describe zero chain" << endl;
  zc->describe();
  

  
  /////////////// begin to test the edge 
  
  cout << "************** test edge  *************" << endl;
  cout << " get_v1 == v1 && get_v2 == v2  - yes" << endl;
  cout << ((ed->get_v1() == v1) && (ed->get_v2() == v2)) << endl;

  cout << "ed == ed - yes" << endl;
  cout << (ed == ed) << endl;
  
  cout << "Share vertex with ed2 - yes" << endl;
  vtol_edge_3d *ed2 = v2->new_edge(v3);
  cout << ed->share_vertex_with(ed2) << endl;

  cout << "Share vertex with ed3 - no" << endl;
  vtol_edge_3d *ed3 = v3->new_edge(v4);
  cout << ed->share_vertex_with(ed3) << endl;
  
  cout << "Is endpoint v1 - yes " << endl;
  cout << ed->is_endpoint(v1) << endl;

  cout << "is endpoint2 v1 - no " << endl;
  cout << ed->is_endpoint2(v1) << endl;


  cout << "is endpoint1 v1 - yes " << endl;
  cout << ed->is_endpoint1(v1) << endl;
  
  cout << "other_endpoint(v1) == v2 - yes" << endl;
  cout << (ed->other_endpoint(v1)==v2) << endl;
  
  cout << "describe edge" << endl;
  ed->describe();

  
  
  /////////////// begin to test the one_chain 
  cout << "************** test one chain  *************" << endl;
  vtol_one_chain_3d *oc1 = f1->get_one_chain(0);
  vtol_one_chain_3d *oc2 = f2->get_one_chain(0);
  
  cout << "Get number of edges - 3 " << endl;
  cout << oc1->num_edges() << endl;
  
  cout << " oc1 == oc1  - yes" << endl;
  cout << (oc1 == oc1) << endl;

  cout << "oc1 == oc2  - no " << endl;
  cout << (oc1 == oc2) << endl;


  
  /////////////// begin to test the face 
  cout << "************** test face  *************" << endl;

  cout << "Test the number of edges - 3 " << endl;
  cout << f1->get_num_edges() << endl;
  
  cout << "Shared edge with f2 - yes " << endl;
  cout << f1->shares_edge_with(f2) << endl;
  
  
  /////////////// begin to test the two chain  
  cout << "************** test two face  *************" << endl;
  
  vtol_two_chain_3d *tc = b1->get_boundary_cycle();
  
  cout << "num_faces - 2" << endl;
  cout << tc->num_faces() << endl;
  
  ///////////////// begin to test the block 

  cout << "************** test block  *************" << endl;
  b1->print(); 

  //////////////////////// Test accessors 
  cout <<" Accessors: vertices, zero_chains ... blocks " << endl;

  cout << endl;
  cout <<" Vertex expects: " << endl;
  cout << "1 3 3 2 2 1 1 - gets:  " << endl;
  cout << v1->vertices()->size() << " ";
  cout << v1->zero_chains()->size() << " ";
  cout << v1->edges()->size() << " ";
  cout << v1->one_chains()->size() << " ";
  cout << v1->faces()->size() << " ";
  cout << v1->two_chains()->size() << " ";
  cout << v1->blocks()->size() << endl;


  cout << endl;
  cout <<" zero_chain expects: " << endl;
  cout << "2 1 1 1 1 1 1 - gets:  " << endl;
  cout << zc->vertices()->size() << " ";
  cout << zc->zero_chains()->size() << " ";
  cout << zc->edges()->size() << " ";
  cout << zc->one_chains()->size() << " ";
  cout << zc->faces()->size() << " ";
  cout << zc->two_chains()->size() << " ";
  cout << zc->blocks()->size() << endl;

  vtol_edge_3d* ed13 = v1->new_edge(v3);

  cout << endl;
  cout <<" edge expects: " << endl;
  cout << "2 1 1 2 2 1 1 - gets:  " << endl;
  cout << ed13->vertices()->size() << " ";
  cout << ed13->zero_chains()->size() << " ";
  cout << ed13->edges()->size() << " ";
  cout << ed13->one_chains()->size() << " ";
  cout << ed13->faces()->size() << " ";
  cout << ed13->two_chains()->size() << " ";
  cout << ed13->blocks()->size() << endl;

  cout << endl;
  cout <<" one chain expects: " << endl;
  cout << "3 3 3 1 1 1 1 - gets:  " << endl;
  cout << oc1->vertices()->size() << " ";
  cout << oc1->zero_chains()->size() << " ";
  cout << oc1->edges()->size() << " ";
  cout << oc1->one_chains()->size() << " ";
  cout << oc1->faces()->size() << " ";
  cout << oc1->two_chains()->size() << " ";
  cout << oc1->blocks()->size() << endl;


  cout << endl;
  cout <<" face expects: " << endl;
  cout << "3 3 3 1 1 1 1 - gets:  " << endl;
  cout << f1->vertices()->size() << " ";
  cout << f1->zero_chains()->size() << " ";
  cout << f1->edges()->size() << " ";
  cout << f1->one_chains()->size() << " ";
  cout << f1->faces()->size() << " ";
  cout << f1->two_chains()->size() << " ";
  cout << f1->blocks()->size() << endl;


  cout << endl;
  cout <<" two chain expects: " << endl;
  cout << "4 5 5 2 2 1 1 - gets:  " << endl;
  cout << tc->vertices()->size() << " ";
  cout << tc->zero_chains()->size() << " ";
  cout << tc->edges()->size() << " ";
  cout << tc->one_chains()->size() << " ";
  cout << tc->faces()->size() << " ";
  cout << tc->two_chains()->size() << " ";
  cout << tc->blocks()->size() << endl;



  cout << endl;
  cout <<" block expects: " << endl;
  cout << "4 5 5 2 2 1 1 - gets:  " << endl;
  cout << b1->vertices()->size() << " ";
  cout << b1->zero_chains()->size() << " ";
  cout << b1->edges()->size() << " ";
  cout << b1->one_chains()->size() << " ";
  cout << b1->faces()->size() << " ";
  cout << b1->two_chains()->size() << " ";
  cout << b1->blocks()->size() << endl;




  ///////////////////////  Delete the block

    cout << "************ Deleting the Block ****************" << endl;
  b1->un_protect();


  cout << "Done" << endl;



}




void main()
{

  cout << "***************************************************" << endl;
  cout << "******************* test 2D topology **************" << endl;
  cout << "***************************************************" << endl;

  test_topology_2d();

  cout << "***************************************************" << endl;
  cout << "******************* test 3D topology **************" << endl;
  cout << "***************************************************" << endl;

  test_topology_3d();


}
