// This is brl/bbas/bmsh3d/bmsh3d_mesh_mc.cxx
//---------------------------------------------------------------------
#include "bmsh3d_mesh_mc.h"
//:
// \file
// \brief Mesh
//
// \author
//  MingChing Chang  April 22, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_list.h>

#include <vnl/vnl_math.h>

#include "bmsh3d_face.h"

bmsh3d_mesh_mc::bmsh3d_mesh_mc(bmsh3d_mesh* mesh) : bmsh3d_mesh()
{
  // deep copy the vertices
  vcl_map<int, bmsh3d_vertex* > vertices = mesh->vertexmap();
  vcl_map<int, bmsh3d_vertex* >::iterator v_it;
  for (v_it = vertices.begin(); v_it != vertices.end(); v_it++) {
    bmsh3d_vertex* vertex = v_it->second;
    bmsh3d_vertex* new_vertex = new bmsh3d_vertex(vertex->get_pt().x(),vertex->get_pt().y(),vertex->get_pt().z(),vertex->id());
    this->_add_vertex(new_vertex);
  }

  // deep copy the edgemap
  vcl_map<int, bmsh3d_edge* > edgemap = mesh->edgemap();
  vcl_map<int, bmsh3d_edge* >::iterator edge_it;
  for (edge_it = edgemap.begin(); edge_it != edgemap.end(); edge_it++) {
    // create new edges
    bmsh3d_edge* E = edge_it->second;
    bmsh3d_vertex* sV = this->vertexmap(E->sV()->id());
    bmsh3d_vertex* eV = this->vertexmap(E->eV()->id());
    bmsh3d_edge* new_edge = new bmsh3d_edge(sV,eV,E->id());
    this->add_edge_incidence(new_edge);
  }

  // deep copy the faces
  vcl_map<int, bmsh3d_face* > fmap_old = mesh->facemap();
  vcl_map<int, bmsh3d_face* >::iterator face_it;
  for (face_it = fmap_old.begin(); face_it != fmap_old.end(); face_it++)
  {
    bmsh3d_face* old_face = (bmsh3d_face*) face_it->second;
    old_face->_sort_HEs_circular();
    old_face->_ifs_clear_vertices();

    bmsh3d_face_mc* new_face = this->_new_face();
    bmsh3d_halfedge* he = old_face->halfedge();
    bmsh3d_halfedge* HE = he;

    // clone the half edges
    do {
      bmsh3d_edge* E = HE->edge();
#ifdef DEBUG
      vcl_cout << E->id() << vcl_endl;
#endif
      bmsh3d_edge* new_edge = this->edgemap(E->id());
      bmsh3d_halfedge* new_he = new bmsh3d_halfedge(new_edge, new_face);
      new_face->connect_E_to_end(new_he->edge());
      HE = HE->next();
    } while (HE != he);

    this->_add_face(new_face);
  }
}

bmsh3d_mesh_mc::~bmsh3d_mesh_mc()
{
  vcl_map<int, bmsh3d_face_mc*>::iterator it = set_face_.begin();
  while (it != set_face_.end()) {
    delete it->second;
  }
}

bmsh3d_mesh_mc* bmsh3d_mesh_mc::clone() const
{
  bmsh3d_mesh_mc* mesh = new bmsh3d_mesh_mc();

  // deep copy the vertices
  vcl_map<int, bmsh3d_vertex* > vertices = this->vertexmap_;
  vcl_map<int, bmsh3d_vertex* >::iterator v_it = vertices.begin();
  while (v_it != vertices.end()) {
    bmsh3d_vertex* vertex = v_it->second;
    bmsh3d_vertex* v = mesh->_new_vertex();
    v->set_pt(vertex->get_pt());
    mesh->_add_vertex(v);
    v_it++;
  }

 // deep copy the edgemap
  vcl_map<int, bmsh3d_edge* > edgemap = this->edgemap_;
  vcl_map<int, bmsh3d_edge* >::iterator edge_it = edgemap.begin();
  while (edge_it != edgemap.end()) {
    // create new edges
    bmsh3d_edge* E = edge_it->second;
#ifdef DEBUG
    vcl_cout << "old E id=" << E->id() << vcl_endl;
#endif
    bmsh3d_edge* new_edge = /*mesh->_new_edge*/new bmsh3d_edge(mesh->vertexmap(E->sV()->id()),
                                    mesh->vertexmap(E->eV()->id()), edge_it->first);
#ifdef DEBUG
    vcl_cout << "  new edge id=" << new_edge->id() << vcl_endl
             << " v1=" << ((bmsh3d_vertex*)new_edge->sV())->get_pt()
             << " v2=" << ((bmsh3d_vertex*)new_edge->eV())->get_pt()
             << vcl_endl;
#endif
    mesh->add_edge_incidence(new_edge);
    edge_it++;
  }

  // deep copy the faces
  vcl_map<int, bmsh3d_face* > fmap = this->facemap_;
  vcl_map<int, bmsh3d_face* >::iterator face_it = fmap.begin();
  while (face_it != fmap.end()) {
    bmsh3d_face_mc* face = (bmsh3d_face_mc*) face_it->second;
#ifdef DEBUG
    vcl_cout << "old face id=" << face->id() << vcl_endl;
#endif
    face->_sort_HEs_circular();
    face->_ifs_clear_vertices();
    bmsh3d_face_mc* f = mesh->_new_face();
    bmsh3d_halfedge* he = face->halfedge();
    bmsh3d_halfedge* HE = he;

    // clone the half edges
    do {
      bmsh3d_edge* E = HE->edge();
#ifdef DEBUG
      vcl_cout << E->id() << vcl_endl;
#endif
      bmsh3d_halfedge* new_he = new bmsh3d_halfedge(mesh->edgemap(E->id()), f);
      f->connect_E_to_end(new_he->edge());
      HE = HE->next();
    } while (HE != he);

    mesh->_add_face(f);
    // clone the inner faces
    if (face->size() > 0) {
      vcl_map<int, bmsh3d_halfedge*> inner_faces = face->get_mc_halfedges();
      vcl_map<int, bmsh3d_halfedge*>::iterator it = inner_faces.begin();
      while (it != inner_faces.end()) {
        vcl_vector<bmsh3d_edge*> incident_edges;
        face->get_mc_incident_edges(it->second, incident_edges);
        bmsh3d_face* inner_face = copy_inner_face(incident_edges, mesh);
        f->add_mc_halfedge((bmsh3d_halfedge *)inner_face->halfedge());
        it++;
      }
    }
    face_it++;
  }
  return mesh;
}

//: Break the mesh face into two faces at the specified place.
//  The break edge is given in V1 and V2,
//  where V1 is on E1 and V2 is on E2.
//
void mesh_break_face(bmsh3d_mesh_mc* M, bmsh3d_face_mc* F,
                     bmsh3d_edge* E1, bmsh3d_edge* E2,
                     bmsh3d_vertex* V1, bmsh3d_vertex* V2,
                     bmsh3d_face_mc* &F1, bmsh3d_face_mc* &F2)
{
  assert(E1 != E2);

  // get the incident edges on F
  // first make sure that the half edges are in order
  F->_sort_HEs_circular();
  F->_ifs_clear_vertices();

  unsigned E1_index = 0, E2_index = 0;
  vcl_vector<bmsh3d_edge*> inc_edges;
  F->get_incident_Es(inc_edges);

  vcl_vector<bmsh3d_halfedge*> inc_hedges;
  F->get_incident_HEs(inc_hedges);

  //Determine E1_index and E2_index.
  //Also makes E1 and E2 in the correct direction.
  for (unsigned i=0; i < inc_edges.size(); i++) {
    bmsh3d_halfedge* he = inc_hedges[i];
    bmsh3d_edge* e = he->edge();
    bmsh3d_halfedge* nhe = he->next();
    bmsh3d_edge* ne = nhe->edge();
    bmsh3d_vertex* v1 = ne->sV();
    bmsh3d_vertex* v2 = ne->eV();

    if (e == E1) {
      E1_index = i;
      //if (ne->is_V_incident(e->sV()))
      if (e->sV() == v1 || e->sV() == v2)
        e->switch_s_e_vertex();
    }
    else if (e == E2) {
      E2_index = i;
      //if (ne->is_V_incident(e->sV()))
      if (e->sV() == v1 || e->sV() == v2)
        e->switch_s_e_vertex();
    }
  }

  //disconnect F from all incident edges and delete face F
  M->remove_face(F);

  //disconnect E1 from all incident faces and delete edge E1
  //the incident faces of E1 is stored in incident_faces1.
  vcl_vector<bmsh3d_face*> incident_faces1;
  E1->disconnect_all_Fs(incident_faces1);
  bmsh3d_vertex* V1s = E1->sV();
  bmsh3d_vertex* V1e = E1->eV();
  M->remove_edge(E1);

  //disconnect E2 from all incident faces and delete edge E2
  //the incident faces of E1 is stored in incident_faces1.
  vcl_vector<bmsh3d_face*> incident_faces2;
  E2->disconnect_all_Fs(incident_faces2);
  bmsh3d_vertex* V2s = E2->sV();
  bmsh3d_vertex* V2e = E2->eV();
  M->remove_edge(E2);

  //create new edge on (V1s, V1) and (V1, V1e)
  bmsh3d_edge* newE1s = M->add_new_edge(V1s, V1);
  bmsh3d_edge* newE1e = M->add_new_edge(V1, V1e);

  //create new edge on (V2s, V2) and (V2, V2e)
  bmsh3d_edge* newE2s = M->add_new_edge(V2s, V2);
  bmsh3d_edge* newE2e = M->add_new_edge(V2, V2e);

  //create new edge E12 on (V1, V2)
  bmsh3d_edge* E12 = M->add_new_edge(V1, V2);

  //create new faces F1
  bmsh3d_face_mc* face1 = M->_new_face();
  bmsh3d_face_mc* face2 = M->_new_face();

  //connect all incident edges of F1, including E12
  vcl_cout << "face1\n"
           << E12->id() << "from " << E12->sV()->id() << " to" << E12->eV()->id() << vcl_endl;
  _connect_F_E_end(face1, E12);
  _connect_F_E_end(face1, newE1e);
  vcl_cout << newE1e->id() << "from " << newE1e->sV()->id() << " to" << newE1e->eV()->id() << vcl_endl;
  unsigned index=E1_index+1;
  if (index == inc_edges.size()) {
      index=0;
  }
  for (; index!= E2_index;) {
    _connect_F_E_end(face1, inc_edges[index]);
    vcl_cout << inc_edges[index]->id() << "from " << inc_edges[index]->sV()->id() << " to" << inc_edges[index]->eV()->id() << vcl_endl;
    if (++index == inc_edges.size())
      index=0;
  }
  _connect_F_E_end(face1, newE2s);
  vcl_cout << newE2s->id() << "from " << newE2s->sV()->id() << " to" << newE2s->eV()->id() << vcl_endl;
  M->_add_face(face1);
  bmsh3d_halfedge* nhe = E12->halfedge()->next();
  bmsh3d_edge* ne = (bmsh3d_edge*) nhe->edge();

  vcl_ostringstream oss;
  face1->_sort_HEs_circular();
  face1->_ifs_clear_vertices();
  face1->getInfo(oss);
#ifdef DEBUG
  vcl_cout << oss.str().c_str();
#endif


  //connect all incident edges of F2, including E12
  _connect_F_E_end(face2, E12);
  vcl_cout << "FACE2\n"
           << E12->id() << "from " << E12->sV()->id() << " to" << E12->eV()->id() << vcl_endl;
  _connect_F_E_end(face2, newE2e);
  vcl_cout << newE2e->id() << "from " << newE2e->sV()->id() << " to" << newE2e->eV()->id() << vcl_endl;
  index = E2_index+1;
  if (index == inc_edges.size())
      index=0;
  for (; index!= E1_index; ) {
    _connect_F_E_end(face2, inc_edges[index]);
    vcl_cout << inc_edges[index]->id() << "from " << inc_edges[index]->sV()->id() << " to" << inc_edges[index]->eV()->id() << vcl_endl;
    if (++index == inc_edges.size())
      index=0;
  }
  _connect_F_E_end(face2, newE1s);
  vcl_cout << newE1s->id() << "from " << newE1s->sV()->id() << " to" << newE1s->eV()->id() << vcl_endl;
  M->_add_face(face2);
  nhe = E12->halfedge()->next();
  ne = (bmsh3d_edge*) nhe->edge();
  //if (ne->is_V_incident(E12->sV()))
  //  E12->switch_s_e_vertex();
  face2->_sort_HEs_circular();
  face2->_ifs_clear_vertices();
  face2->getInfo(oss);
#ifdef DEBUG
  vcl_cout << oss.str().c_str();
#endif

  for (unsigned f=0; f<incident_faces1.size(); f++) {
    // add the new edges
    bmsh3d_face_mc* face = (bmsh3d_face_mc*) incident_faces1[f];
    _connect_F_E_end(face, newE1s);
    _connect_F_E_end(face, newE1e);
    face->_sort_HEs_circular();
    face->_ifs_clear_vertices();
#ifdef DEBUG
    face->getInfo(oss);
    vcl_cout << oss.str().c_str();
#endif
  }

  for (unsigned f=0; f<incident_faces2.size(); f++) {
    // add the new edges
    bmsh3d_face_mc* face = (bmsh3d_face_mc*) incident_faces2[f];
    _connect_F_E_end(face, newE2s);
    _connect_F_E_end(face, newE2e);
    face->_sort_HEs_circular();
    face->_ifs_clear_vertices();
    //face->getInfo(oss);
  }
  vcl_cout << oss.str().c_str();
  F1 = face1;
  F2 = face2;
}

void merge_mesh(bmsh3d_mesh_mc* M1, bmsh3d_mesh_mc* M2)
{
  unsigned v_num = M1->vertexmap().size();
  unsigned e_num = M1->edgemap().size();

  // merge the vertices
  vcl_map<int, bmsh3d_vertex* > vertices = M2->vertexmap();
  vcl_map<int, bmsh3d_vertex* >::iterator v_it = vertices.begin();
  while (v_it != vertices.end()) {
    bmsh3d_vertex* vertex = v_it->second;
    bmsh3d_vertex* v = new bmsh3d_vertex(vertex->id() + v_num);//M1->_new_vertex();
    v->set_pt(vertex->get_pt());
    M1->_add_vertex(v);
    v_it++;
  }

  // merge the edges
  vcl_map<int, bmsh3d_edge* > edgemap = M2->edgemap();
  vcl_map<int, bmsh3d_edge* >::iterator edge_it = edgemap.begin();
  while (edge_it != edgemap.end()) {
    // create new edges
    bmsh3d_edge* edge = edge_it->second;
    bmsh3d_edge* e = new bmsh3d_edge(M1->vertexmap(edge->sV()->id() + v_num),
      M1->vertexmap(edge->eV()->id() + v_num),
      edge->id() + e_num);
    M1->add_edge_incidence(e);
    edge_it++;
  }

  // merge the faces
  vcl_map<int, bmsh3d_face* > fmap = M2->facemap();
  vcl_map<int, bmsh3d_face* >::iterator face_it = fmap.begin();
  while (face_it != fmap.end()) {
    bmsh3d_face_mc* face = (bmsh3d_face_mc*) face_it->second;
    bmsh3d_face_mc* f = M1->_new_mc_face();
    bmsh3d_halfedge* he = face->halfedge();
    bmsh3d_halfedge* HE = he;
    do {
      bmsh3d_edge* edge = HE->edge();
      bmsh3d_halfedge* new_he = new bmsh3d_halfedge(M1->edgemap(edge->id()+e_num), f);
      f->connect_E_to_end(new_he->edge());
      HE = HE->next();
    } while (HE != he);

    M1->_add_face(f);

    if (face->size() > 0) {
      vcl_map<int, bmsh3d_halfedge*> inner_faces = face->get_mc_halfedges();
      vcl_map<int, bmsh3d_halfedge*>::iterator it = inner_faces.begin();
      while (it != inner_faces.end()) {
        //bmsh3d_face* inner_face = new bmsh3d_face();
        vcl_vector<bmsh3d_edge*> incident_edges;
        face->get_mc_incident_edges(it->second, incident_edges);
        bmsh3d_face* inner_face = copy_inner_face(incident_edges, M1);
        f->add_mc_halfedge((bmsh3d_halfedge *)inner_face->halfedge());
        it++;
      }
    }
#ifdef DEBUG
    vcl_cout << "new_face id =" << f->id() << vcl_endl;
#endif
    face_it++;
  }
}

bmsh3d_face* copy_inner_face(vcl_vector<bmsh3d_edge*> incident_edges,
                          bmsh3d_mesh_mc* mesh)
{
  bmsh3d_face* inner_face = new bmsh3d_face();

  bmsh3d_vertex *prev_v = 0, *v_first = 0, *v;
  for (unsigned i=0; i<incident_edges.size()-1; i++) {
    bmsh3d_edge* edge = incident_edges[i];
    bmsh3d_vertex* s = edge->sV();
    bmsh3d_vertex* e = edge->eV();
    if (i ==0) {
      prev_v = mesh->_new_vertex();
      prev_v->set_pt(s->get_pt());
      //mesh->_add_vertex(prev_v);
      v_first = prev_v;
    }

     v = mesh->_new_vertex();
     v->set_pt(e->get_pt());
     //mesh->_add_vertex(v);
#ifdef DEBUG
     vcl_cout << prev_v << " to " << v << vcl_endl;
#endif
     bmsh3d_edge* new_edge = new  bmsh3d_edge(prev_v, v, 100);
     bmsh3d_halfedge *he = new bmsh3d_halfedge(new_edge, inner_face);
     prev_v = v;
     inner_face->_connect_HE_to_end(he);
   }

  // add the last halfedge
#ifdef DEBUG
  vcl_cout << prev_v << " to " << v_first << vcl_endl;
#endif
  bmsh3d_edge* new_edge = new  bmsh3d_edge(prev_v, v_first, 100);
  bmsh3d_halfedge *he = new bmsh3d_halfedge(new_edge, inner_face);
  inner_face->_connect_HE_to_end(he);
  return inner_face;
}

void bmsh3d_mesh_mc::orient_face_normals()
{
  // find the face (topmost) with the biggest z value
  vcl_map<int, bmsh3d_face* > fmap = facemap();
  vcl_map<int, bmsh3d_face* >::iterator face_it = fmap.begin();
  double maxz=-1e26;
  int maxz_face_id=0;
  while (face_it != fmap.end()) {
    bmsh3d_face_mc* face = (bmsh3d_face_mc*) face_it->second;
    vgl_point_3d<double> center = face->compute_center_pt();
    if (center.z() > maxz) {
      maxz = center.z();
      maxz_face_id = face_it->first;
    }
    face_it++;
  }

  // force the plane to point towards the positive z axis
  bmsh3d_face_mc* face = (bmsh3d_face_mc*) this->facemap(maxz_face_id);
  face->_sort_HEs_circular();
  face->_ifs_clear_vertices();
  vgl_vector_3d<double> normal = face->compute_normal();
  normal /= normal.length();
  vgl_vector_3d<double> z(0, 0, 1);
  double a = angle(normal, z);

  if (a > vnl_math::pi/2.0) {
    face->_reverse_HE_chain();
  } // else (the normal is in correct position)
  face->set_visited(true);

  // visit all the other faces and fix their normal
  vcl_list<bmsh3d_face_mc*> face_stack;
  do {
#ifdef DEBUG
    vcl_cout << "Face ID=" << face->id() << vcl_endl;
#endif
    vcl_vector<bmsh3d_face*> inc_faces;

    vcl_vector<bmsh3d_edge*> inc_edges;
    face->get_incident_Es(inc_edges);
    for (unsigned i=0; i<inc_edges.size(); i++) {
      bmsh3d_edge* E = inc_edges[i];
      bmsh3d_halfedge* he = E->get_HE_of_F(face);
      vcl_vector<bmsh3d_face*> faces;
      E->get_incident_Fs(faces);
      for (unsigned f=0; f<faces.size(); f++) {
        bmsh3d_face_mc* inc_face = (bmsh3d_face_mc*) faces[f];
        if ((inc_face != face) && (!inc_face->is_visited(1))) {
          inc_face->_sort_HEs_circular();
          inc_face->_ifs_clear_vertices();
          bmsh3d_halfedge* inc_he = E->get_HE_of_F(inc_face);
          bmsh3d_halfedge* h1 = he->next();
          bmsh3d_halfedge* h2 = inc_he->next();
          if (h1->s_vertex() == h2->s_vertex()) {
            inc_face->_reverse_HE_chain();
#ifdef DEBUG
            vcl_cout << "changing face direction" << inc_face->id() << vcl_endl;
#endif
          }
          inc_face->set_visited(true);
          face_stack.push_back(inc_face);
        }
      }
    }
    if (face_stack.size() > 0) {
      face = (bmsh3d_face_mc*) face_stack.front();
      face_stack.pop_front();
    } else {
      face = 0;
    }
  } while (face != 0);

  // print out the normals;
  face_it = fmap.begin();
  while (face_it != fmap.end()) {
    bmsh3d_face_mc* face = (bmsh3d_face_mc*) face_it->second;
    vgl_vector_3d<double> normal = face->compute_normal();
    normal /= normal.length();
#ifdef DEBUG
    vcl_cout << "Face " << face->id() << " N=" << normal << vcl_endl;
#endif
    face_it++;
  }
}
