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

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_vector.h>

#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_zero_chain_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_one_chain_2d.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_two_chain_2d.h>
#include <vtol/vtol_block_2d.h>

static void test_topology_2d(void);

static void test_topology_2d(void)
{
  // OK we want to make two faces and put them into a block
  bool tmp;
  double tmpd;
  int tmpi;

  cout << "Creating vertices" << endl;

  vtol_vertex_2d_ref v1=new vtol_vertex_2d(0.0,0.0);
  vtol_vertex_2d_ref v2=new vtol_vertex_2d(0.0,1.0);
  vtol_vertex_2d_ref v3=new vtol_vertex_2d(1.0,1.0);
  vtol_vertex_2d_ref v4=new vtol_vertex_2d(1.0,0.0);
  //  v1->describe();
  
  cout << "Creating faces" << endl;

  //  vcl_vector<vtol_vertex_2d*> verts;
  vcl_vector<vtol_vertex_2d_ref> verts;
  verts.push_back(v1);
  verts.push_back(v2);
  verts.push_back(v3);
  cout<<"verts filled"<<endl;
  
  vtol_face_2d_ref f1=new vtol_face_2d(verts);

  cout<<"Face f1 created"<<endl;

  // f1->describe();

  verts.clear();
  
  verts.push_back(v3);
  verts.push_back(v4);
  verts.push_back(v1);

  vtol_face_2d_ref f2=new vtol_face_2d(verts);
  
  cout << "Creating a block" << endl;
  
  vcl_vector<vtol_face_2d_ref> faces;
  
  faces.push_back(f1);
  faces.push_back(f2);
  cout<<"faces filled"<<endl;
  
  vtol_block_2d_ref b1=new vtol_block_2d(faces);
  cout<<"Block b1 created"<<endl;
  //  b1->describe();

  
  /////////////// begin to test the vertex 
 
  cout << "************** test vertex *************" << endl;
  cout << " topology_type: " << endl;

  tmpi=v1->topology_type();
  //  cout << v1->topology_type() << endl; 
  
  cout << " x() y() (0,0):" << endl;
  tmpd=v1->x();
  tmpd=v1->y();
  //  cout << v1->x() << " " << v1->y() << endl;
   
  cout << " v1 == v1 " << endl;
  tmp=(*v1) == (*v1);
  //  cout << ((*v1) == (*v1)) << endl;

  cout << " v1 == v2 " << endl;
  //  cout << ((*v1) == (*v2)) << endl;
  tmp=(*v1) == (*v2);

  cout << "is coneceted v1 & v2 - yes" << endl;
  //  cout << v1->is_connected(*v2) << endl; 
  tmp=v1->is_connected(*v2);

  cout << "is coneceted v2 & v4 - no " << endl;
  //  cout << v2->is_connected(*v4) << endl;
  tmp=v2->is_connected(*v4);

  cout << "is_endpointp - yes" << endl;
  vtol_edge_2d_ref ed=v1->new_edge(*v2);
  //  cout << v1->is_endpointp(*ed) << endl;
  tmp=v1->is_endpointp(*ed);

  cout << "is_endpointp - no" << endl;
  //  cout << v3->is_endpointp(*ed) << endl;
  tmp=v3->is_endpointp(*ed);

   /////////////// begin to test the zero chain 
 
  cout << "************** test zero_chain  *************" << endl;
  
  vtol_zero_chain_2d_ref zc=ed->zero_chain();

  cout << "Test zc->v0 == v1 - yes" << endl;
  //  cout << (zc->v0() == v1) << endl;
  tmp=zc->v0()==v1;

  cout << "length - 2" << endl;
  //  cout << zc->length() << endl;
  tmpi=zc->length();

  cout << "*zc == *zc" << endl;
  //  cout << (*zc == *zc) << endl;
  tmp=*zc==*zc;

  cout << "describe zero chain" << endl;
  //  zc->describe();
  

  
  /////////////// begin to test the edge 
  
  cout << "************** test edge  *************" << endl;
  cout << " get_v1 == v1 && get_v2 == v2  - yes" << endl;
  //  cout << ((ed->v1() == v1) && (ed->v2() == v2)) << endl;
  tmp=(ed->v1() == v1) && (ed->v2() == v2);

  cout << "*ed == *ed - yes" << endl;
  //  cout << (*ed == *ed) << endl;
  tmp=*ed == *ed;

  cout << "Share vertex with ed2 - yes" << endl;
  vtol_edge_2d_ref ed2 = v2->new_edge(*v3);
    //cout << ed->share_vertex_with(*ed2) << endl;
  tmp=ed->share_vertex_with(*ed2);

  cout << "Share vertex with ed3 - no" << endl;
  vtol_edge_2d_ref ed3=v3->new_edge(*v4);
  //cout << ed->share_vertex_with(*ed3) << endl;
  tmp=ed->share_vertex_with(*ed3);

  cout << "Is endpoint v1 - yes " << endl;
  //  cout << ed->is_endpoint(*v1) << endl;
  tmp=ed->is_endpoint(*v1);

  cout << "is endpoint2 v1 - no " << endl;
  //  cout << ed->is_endpoint2(*v1) << endl;
  tmp=ed->is_endpoint2(*v1);

  cout << "is endpoint1 v1 - yes " << endl;
  //  cout << ed->is_endpoint1(*v1) << endl;
  tmp=ed->is_endpoint1(*v1);

  cout << "other_endpoint(v1) == v2 - yes" << endl;
  //  cout << (ed->other_endpoint(*v1)==v2) << endl;
  tmp=ed->other_endpoint(*v1)==v2;
  

  cout << "describe edge" << endl;
  //  ed->describe();

  
  
  /////////////// begin to test the one_chain 
  cout << "************** test one chain  *************" << endl;
  vtol_one_chain_2d_ref oc1=f1->get_one_chain(0);
  vtol_one_chain_2d_ref oc2=f2->get_one_chain(0);
  
  cout << "Get number of edges - 3 " << endl;
  //  cout << oc1->num_edges() << endl;
  tmpi=oc1->num_edges();

  cout << " *oc1 == *oc1  - yes" << endl;
  //  cout << (*oc1 == *oc1) << endl;
  tmp=*oc1==*oc1;

  cout << "*oc1 == *oc2  - no " << endl;
  //  cout << (*oc1 == *oc2) << endl;
  tmp=*oc1 == *oc2;

  
  /////////////// begin to test the face 
  cout << "************** test face  *************" << endl;

  cout << "Test the number of edges - 3 " << endl;
  //  cout << f1->get_num_edges() << endl;
  tmpi=f1->get_num_edges();


  cout << "Shared edge with f2 - yes " << endl;
  //  cout << f1->shares_edge_with(*f2) << endl;
  tmp=f1->shares_edge_with(*f2);


  
  /////////////// begin to test the two chain  
  cout << "************** test two face  *************" << endl;
  
  vtol_two_chain_2d_ref tc=b1->get_boundary_cycle();
  
  cout << "num_faces - 2" << endl;
  //  cout << tc->num_faces() << endl;
  tmpi=tc->num_faces();


  ///////////////// begin to test the block 

  cout << "************** test block  *************" << endl;
  //  b1->print(); 

  //////////////////////// Test accessors

  vertex_list_2d *vl;
  zero_chain_list_2d *zcl;
  edge_list_2d *el;
  one_chain_list_2d *ocl;
  face_list_2d *fl;
  two_chain_list_2d *tcl;
  block_list_2d *bl;

  cout <<" Accessors: vertices, zero_chains ... blocks " << endl;

  cout << endl;
  cout <<" Vertex expects: " << endl;
  cout << "1 3 3 2 2 1 1 - gets:  " << endl;
  vl=v1->vertices();
  //  cout << vl->size() << " ";
  delete vl;
  zcl=v1->zero_chains();
 // cout << zcl->size() << " ";
  delete zcl;
  el=v1->edges();
 // cout << el->size() << " ";
  delete el;
  ocl=v1->one_chains();
//  cout << ocl->size() << " ";
  delete ocl;
  fl=v1->faces();
//  cout << fl->size() << " ";
  delete fl;
  tcl=v1->two_chains();
//  cout << tcl->size() << " ";
  delete tcl;
  bl=v1->blocks();
//  cout << bl->size() << " ";
  delete bl;

  cout << endl;
  cout <<" zero_chain expects: " << endl;
  cout << "2 1 1 1 1 1 1 - gets:  " << endl;
  vl=zc->vertices();
//  cout << vl->size() << " ";
  delete vl;
  zcl=zc->zero_chains();
//  cout << zcl->size() << " ";
  delete zcl;
  el=zc->edges();
//  cout << el->size() << " ";
  delete el;
  ocl=zc->one_chains();
//  cout << ocl->size() << " ";
  delete ocl;
  fl=zc->faces();
//  cout << fl->size() << " ";
  delete fl;
  tcl=zc->two_chains();
//  cout << tcl->size() << " ";
  delete tcl;
  bl=zc->blocks();
//  cout << bl->size() << " "<< endl;
  delete bl;

  vtol_edge_2d_ref ed13= v1->new_edge(*v3);

  cout << endl;
  cout <<" edge expects: " << endl;
  cout << "2 1 1 2 2 1 1 - gets:  " << endl;
  vl=ed13->vertices();
//  cout << vl->size() << " ";
  delete vl;
  zcl=ed13->zero_chains();
//  cout << zcl->size() << " ";
  delete zcl;
  el=ed13->edges();
//  cout << el->size() << " ";
  delete el;
  ocl=ed13->one_chains();
//  cout << ocl->size() << " ";
  delete ocl;
  fl=ed13->faces();
//  cout << fl->size() << " ";
  delete fl;
  tcl=ed13->two_chains();
//  cout << tcl->size() << " ";
  delete tcl;
  bl=ed13->blocks();
//  cout << bl->size() << " "<< endl;
  delete bl;


  cout << endl;
  cout <<" one chain expects: " << endl;
  cout << "3 3 3 1 1 1 1 - gets:  " << endl;
  vl=oc1->vertices();
//  cout << vl->size() << " ";
  delete vl;
  zcl=oc1->zero_chains();
//  cout << zcl->size() << " ";
  delete zcl;
  el=oc1->edges();
//  cout << el->size() << " ";
  delete el;
  ocl=oc1->one_chains();
//  cout << ocl->size() << " ";
  delete ocl;
  fl=oc1->faces();
//  cout << fl->size() << " ";
  delete fl;
  tcl=oc1->two_chains();
//  cout << tcl->size() << " ";
  delete tcl;
  bl=oc1->blocks();
//  cout << bl->size() << " "<< endl;
  delete bl;


  cout << endl;
  cout <<" face expects: " << endl;
  cout << "3 3 3 1 1 1 1 - gets:  " << endl;
  vl=f1->vertices();
//  cout << vl->size() << " ";
  delete vl;
  zcl=f1->zero_chains();
//  cout << zcl->size() << " ";
  delete zcl;
  el=f1->edges();
//  cout << el->size() << " ";
  delete el;
  ocl=f1->one_chains();
//  cout << ocl->size() << " ";
  delete ocl;
  fl=f1->faces();
//  cout << fl->size() << " ";
  delete fl;
  tcl=f1->two_chains();
//  cout << tcl->size() << " ";
  delete tcl;
  bl=f1->blocks();
//  cout << bl->size() << " "<< endl;
  delete bl;

  cout << endl;
  cout <<" two chain expects: " << endl;
  cout << "4 5 5 2 2 1 1 - gets:  " << endl;
  vl=tc->vertices();
//  cout << vl->size() << " ";
  delete vl;
  zcl=tc->zero_chains();
//  cout << zcl->size() << " ";
  delete zcl;
  el=tc->edges();
//  cout << el->size() << " ";
  delete el;
  ocl=tc->one_chains();
//  cout << ocl->size() << " ";
  delete ocl;
  fl=tc->faces();
//  cout << fl->size() << " ";
  delete fl;
  tcl=tc->two_chains();
//  cout << tcl->size() << " ";
  delete tcl;
  bl=tc->blocks();
//  cout << bl->size() << " "<< endl;
  delete bl;

  cout << endl;
  cout <<" block expects: " << endl;
  cout << "4 5 5 2 2 1 1 - gets:  " << endl;
  vl=b1->vertices();
//  cout << vl->size() << " ";
  delete vl;
  zcl=b1->zero_chains();
//  cout << zcl->size() << " ";
  delete zcl;
  el=b1->edges();
//  cout << el->size() << " ";
  delete el;
  ocl=b1->one_chains();
//  cout << ocl->size() << " ";
  delete ocl;
  fl=b1->faces();
//  cout << fl->size() << " ";
  delete fl;
  tcl=b1->two_chains();
//  cout << tcl->size() << " ";
  delete tcl;
  bl=b1->blocks();
//  cout << bl->size() << " "<< endl;
  delete bl;

  ///////////////////////  Delete the block

  cout << "************ Deleting the Block ****************" << endl;
  //  b1->un_protect();
  b1=0;
  cout << "Done" << endl;
}

int main(int argc,
         char *argv[])
{
  cout<<"***************************************************"<<endl;
  cout<<"******************* test 2D topology **************"<<endl;
  cout<<"***************************************************"<<endl;
  test_topology_2d();
  return 0;
}
