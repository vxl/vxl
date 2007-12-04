#include "bwm_observable_mesh.h"
#include "bwm_world.h"
//:
// \file
#include "bwm_def.h"
#include "algo/bwm_algo.h"

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vnl/vnl_math.h>

#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/algo/vgl_fit_plane_3d.h>

#include <vsol/vsol_point_3d.h>

#include <bmsh3d/bmsh3d_textured_mesh_mc.h>
#include <bmsh3d/vis/bmsh3d_vis_backpt.h>
#include <bmsh3d/vis/bmsh3d_vis_mesh.h>
#include <bmsh3d/algo/bmsh3d_fileio.h>
#include <bmsh3d/algo/bmsh3d_mesh_triangulate.h>

bwm_observable_mesh::bwm_observable_mesh() 
: object_(0) 
{
  //bwm_world::instance()->add(this);
}

bwm_observable_mesh::bwm_observable_mesh(BWM_MESH_TYPES type)  
: object_(0), mesh_type_(type) 
{
  //bwm_world::instance()->add(this);
}
  
bwm_observable_mesh::bwm_observable_mesh(bmsh3d_mesh_mc* object)  
: object_(object) 
{
  //bwm_world::instance()->add(this);
}

bwm_observable_mesh::bwm_observable_mesh(vsol_polygon_3d_sptr poly)
{
  // create a single face mesh
  object_ = new bmsh3d_mesh_mc;
  bmsh3d_face_mc* face = create_face(poly);
  object_->_add_face (face);
  object_->orient_face_normals();
  // add yourself to The World
  //bwm_world::instance()->add(this);
  notify_observers("new");
}

bwm_observable_mesh::~bwm_observable_mesh()
{
#if 0
  notify_observers("delete");
  vcl_vector<vgui_observer*> observers;
  get_observers(observers);
  for (unsigned i=0; i<observers.size(); i++) {
    detach(observers[i]);
  }
#endif // 0
  remove();
}

void bwm_observable_mesh::remove()
{
  if (object_) {
    notify_observers("delete");
  }

  vcl_vector<vgui_observer*> observers;
  get_observers(observers);
  for (unsigned i=0; i<observers.size(); i++) {
    detach(observers[i]);
  }
  // remove the object from world
  //bwm_world::instance()->remove(this);
}

void bwm_observable_mesh::notify_observers(vcl_string message_type)
{
  vgui_message msg;
  msg.from = this;
  msg.data = new vcl_string(message_type);
  this->notify(msg);
}

vgl_box_3d<double> bwm_observable_mesh::bounding_box()
{
  vgl_box_3d<double> bb;
  detect_bounding_box(object_, bb);
  return bb;
}

BWM_FACE_LABEL bwm_observable_mesh::get_face_label(unsigned face_id)
{
  vcl_map<unsigned, BWM_FACE_LABEL>::iterator it = labels_.find(face_id);
  if (it != labels_.end())
    return it->second;
  return None;
}

void bwm_observable_mesh::translate(vgl_vector_3d<double> T)
{
  vcl_map<int, bmsh3d_vertex*> v_map = object_->vertexmap();
  vcl_map<int, bmsh3d_vertex*>::iterator iter = v_map.begin();
  while (iter != v_map.end()) {
    bmsh3d_vertex* v = (bmsh3d_vertex*) iter->second;
    v->set_pt (vgl_point_3d<double> (v->get_pt().x() + T.x() ,
            v->get_pt().y() + T.y(),
            v->get_pt().z() + T.z()));
    vcl_cout << v->get_pt() << vcl_endl;
    iter++;
  }
  notify_observers("move");
}

bwm_observable_sptr bwm_observable_mesh::transform(vgl_h_matrix_3d<double> T_)
{
  bmsh3d_mesh_mc* M_copy = object_->clone();
  vcl_map<int, bmsh3d_vertex*> vertices = M_copy->vertexmap();
  for (unsigned i=0; i<vertices.size(); i++) {
    bmsh3d_vertex* v = vertices[i];
    vgl_point_3d<double> p = v->get_pt();
    vgl_point_3d<double> tp = T_(vgl_homg_point_3d<double>(p));
    v->set_pt (tp);
  }
  return new bwm_observable_mesh(M_copy);
}

void bwm_observable_mesh::extrude(int face_id)
{
  this->extrude(face_id, 0.01);
#if 0 // commented out
  if (object_ != 0) {
    bmsh3d_face_mc* face = (bmsh3d_face_mc*)object_->facemap(face_id);
    if (face) {
      if (object_->facemap().size() == 1) {
        bmsh3d_face_mc* face = (bmsh3d_face_mc*) object_->facemap(face_id);
        vcl_vector<bmsh3d_vertex*> vertices;
        vsol_polygon_3d_sptr poly = this->extract_face(face, vertices);
        vcl_map<int, vsol_polygon_3d_sptr> inner_faces = this->extract_inner_faces(face);
        delete object_;
        object_ = new bmsh3d_mesh_mc();
        // the first polygon is always the outer one
        create_mesh_HE(poly, 0.01, inner_faces);
      } else {
        current_extr_face = extrude_face(object_, face);
      }
      notify_observers("update");
    } else
      current_extr_face = 0;
   }
  vcl_cout << "FACES====>" << vcl_endl;
  this->print_faces();
#endif // 0
}

void bwm_observable_mesh::extrude(int face_id, double dist)
{
  if (object_ != 0) {
    bmsh3d_face_mc* face = (bmsh3d_face_mc*)object_->facemap(face_id);
    if (face) {
      if (object_->facemap().size() == 1) {
        bmsh3d_face_mc* face = (bmsh3d_face_mc*) object_->facemap(face_id);
        vcl_vector<bmsh3d_vertex*> vertices;
        vsol_polygon_3d_sptr poly = this->extract_face(face, vertices);
        vcl_map<int, vsol_polygon_3d_sptr> inner_faces = this->extract_inner_faces(face);
        delete object_;
        object_ = new bmsh3d_mesh_mc();
        // the first polygon is always the outer one
        create_mesh_HE(poly, dist, inner_faces);
      } else {
        current_extr_face = extrude_face(object_, face);
        move_extr_face(dist);
      }
      notify_observers("update");
    } else
      current_extr_face = 0;
   }
  //vcl_cout << "FACES====>" << vcl_endl;
  //this->print_faces();
}

void bwm_observable_mesh::set_object(bmsh3d_mesh_mc* obj)
{
  vcl_string msg = "";
  if (object_ == 0) 
    msg = "new";
  else {
    msg = "update";
    delete object_;
  }

  object_ = obj;
  //object_->orient_face_normals();
  notify_observers(msg);
 }

void bwm_observable_mesh::set_object(vsol_polygon_3d_sptr poly, double z)
{
  vcl_string msg = "";
  if (object_ == 0) 
    msg = "new";
  else {
    msg = "update";
    delete object_;
  }

  object_ = new bmsh3d_mesh_mc;
  vcl_map<int, vsol_polygon_3d_sptr> inner_faces;
  create_mesh_HE(poly, z, inner_faces);
  //object_->orient_face_normals();
  notify_observers(msg);
}

void bwm_observable_mesh::set_object(vsol_polygon_3d_sptr poly)
{
  if (!poly || poly->size()==0)
    return;

  vcl_string msg = "";
  if (object_ == 0) 
    msg = "new";
  else {
    msg = "update";
    delete object_;
  }
  object_ = new bmsh3d_mesh_mc;
  bmsh3d_face_mc* face = create_face(poly);
  object_->_add_face (face);
  //object_->orient_face_normals();
  notify_observers(msg);
}

void bwm_observable_mesh::replace(bmsh3d_mesh_mc* obj)
{
  vcl_string msg="";
  if (object_) {
    delete object_;
    msg = "update";
  } else
    msg = "new";
  object_ = obj;
  //object_->orient_face_normals();
  notify_observers(msg);
}

//: Returns a list of polygons.
//  If there are inner faces, there are more than one polygon,
//  otherwise it is always one.
vsol_polygon_3d_sptr bwm_observable_mesh::extract_face(bmsh3d_face_mc* face,
                                             vcl_vector<bmsh3d_vertex*> &vertices)
{
  vcl_vector<vsol_point_3d_sptr> v_list;
  bmsh3d_halfedge* cur_he = (bmsh3d_halfedge*) face->halfedge();

  //open
//   bwm_algo::move_points_to_plane(face);

  int i=0;
  do {
    bmsh3d_halfedge* next_he = (bmsh3d_halfedge*) cur_he->next();
    bmsh3d_vertex* vertex = (bmsh3d_vertex*) Es_sharing_V  (cur_he->edge(), next_he->edge());

    vertices.push_back(vertex);
    //vcl_cout << "vertex " << vertex->id() << " between " <<
    //  cur_he->edge()->id() << " and " << next_he->edge()->id() << vcl_endl;
    vgl_point_3d<double> p = vertex->get_pt();
    v_list.push_back(new vsol_point_3d (p.x(), p.y(), p.z()));
    cur_he = (bmsh3d_halfedge*) cur_he->next();
  } while (cur_he != face->halfedge());

  vsol_polygon_3d_sptr poly3d = new vsol_polygon_3d(v_list);
  return poly3d;
}

vcl_map<int, vsol_polygon_3d_sptr> 
bwm_observable_mesh::extract_inner_faces(bmsh3d_face_mc* face)
{
  // now, add the inner polygons
  vcl_map<int, bmsh3d_halfedge*> set_he = face->get_mc_halfedges();
  vcl_map<int, vsol_polygon_3d_sptr> polygons;
  vcl_map<int, bmsh3d_halfedge*>::iterator it = set_he.begin();
  while (it != set_he.end()) {
    bmsh3d_halfedge* he = it->second;
    bmsh3d_halfedge* HE = he;
    vcl_vector<vsol_point_3d_sptr> v_list;

    do {
      bmsh3d_halfedge* next_he = (bmsh3d_halfedge*) HE->next();
      bmsh3d_vertex* vertex = (bmsh3d_vertex*) Es_sharing_V  (HE->edge(), next_he->edge());
      //vertices.push_back(vertex);
      vgl_point_3d<double> p = vertex->get_pt();
      v_list.push_back(new vsol_point_3d (p.x(), p.y(), p.z()));
      HE = next_he;
    } while (HE != he);

    vsol_polygon_3d_sptr poly3d = new vsol_polygon_3d(v_list);
    polygons[it->first] = poly3d;
    it++;
  }
  return polygons;
}

vcl_map<int, vsol_polygon_3d_sptr> 
bwm_observable_mesh::extract_inner_faces(int face_id)
{
  bmsh3d_face_mc* face = (bmsh3d_face_mc*)object_->facemap(face_id);
  vcl_map<int, vsol_polygon_3d_sptr> polys;
  if (face) {
    polys = extract_inner_faces(face);
  }
  return polys;
}

vsol_polygon_3d_sptr bwm_observable_mesh::extract_face(unsigned i)
{
  bmsh3d_face_mc* face = (bmsh3d_face_mc*) object_->facemap(i);
  vcl_vector<bmsh3d_vertex*> vertices;
  vsol_polygon_3d_sptr poly = extract_face(face, vertices);
  return poly;
}

vcl_map<int, vsol_polygon_3d_sptr> bwm_observable_mesh::extract_faces()
{
  vcl_map<int, bmsh3d_face*>::iterator it = object_->facemap().begin();
  vcl_map<int, vsol_polygon_3d_sptr> faces;

  for (; it != object_->facemap().end(); it++) {
    bmsh3d_face_mc* face = (bmsh3d_face_mc*) (*it).second;
    //vcl_cout << "face " << face->id() << vcl_endl;
    vcl_vector<bmsh3d_vertex*> vertices;
    vsol_polygon_3d_sptr poly = this->extract_face(face, vertices);
    faces[face->id()] = poly;
  }
  return faces;
}

void bwm_observable_mesh::create_interior()
{
  if (object_) {
    bmsh3d_mesh_mc* interior = object_->clone();
    double l = object_->edgemap(0)->length();
    shrink_mesh(interior, 0);//l/10.);
    //merge_mesh(object_, interior);
    object_=interior;
    this->notify_observers("update");
  }
}

void bwm_observable_mesh::move(vsol_polygon_3d_sptr poly)
{
  if (!object_) {
    vcl_cerr << "mesh object id null" << vcl_endl;
    return; 
  }

  if (object_->facemap().size() == 1) {
    vcl_map<int, bmsh3d_face*>::iterator it = object_->facemap().begin();
    bmsh3d_face_mc* face = (bmsh3d_face_mc*) (*it).second;
    vcl_vector<bmsh3d_vertex*> vertices;
    vsol_polygon_3d_sptr old_poly = extract_face(face, vertices);

    for (unsigned i=0; i<poly->size(); i++) {
      bmsh3d_vertex* v = (bmsh3d_vertex*) vertices[i];
      vsol_point_3d_sptr p = poly->vertex(i);
      v->set_pt (vgl_point_3d<double> (p->x(), p->y(), p->z()));
    }
    notify_observers("move");
  }
}

void bwm_observable_mesh::move_normal_dir(double dist)
{
  if (object_->facemap().size() == 1) {
    vcl_map<int, bmsh3d_face*>::iterator it = object_->facemap().begin();
    bmsh3d_face_mc* face = (bmsh3d_face_mc*) (*it).second;
    //vgl_vector_3d<double> normal = face->compute_normal();
    //normal /= normal.length();

    vcl_vector<bmsh3d_vertex*> vertices;
    vsol_polygon_3d_sptr poly = extract_face(face, vertices);
    vgl_vector_3d<double> normal = poly->normal();
    //normal /= normal.length();
    for (unsigned i=0; i<poly->size(); i++) {
      bmsh3d_vertex* v = (bmsh3d_vertex*) vertices[i];
      vsol_point_3d_sptr p = poly->vertex(i);
      v->set_pt (vgl_point_3d<double> (v->get_pt().x() + dist*normal.x() ,
            v->get_pt().y() + dist*normal.y(),
            v->get_pt().z() + dist*normal.z()));
    }
    notify_observers("move");
  }
}

void bwm_observable_mesh::move_extr_face(double z)
{
  if (current_extr_face) {
    vcl_vector<bmsh3d_vertex*> vertices;
    vsol_polygon_3d_sptr polygon = extract_face(current_extr_face, vertices);
    // at this point, the halfedges already sorted in extract_face

    for (unsigned i=0; i<vertices.size(); i++) {
      bmsh3d_vertex* v = vertices[i];
      //vgl_vector_3d<double> normal = current_extr_face->compute_normal(edge, v);
      vsol_point_3d_sptr p = polygon->vertex(i);
      //vgl_vector_3d<double> normal = polygon->normal_at_point(p);

      /*v->set_pt (vgl_point_3d<double> (v->get_pt().x() + dist*normal.x() ,
            v->get_pt().y() + dist*normal.y(),
            v->get_pt().z() + dist*normal.z()));*/
      v->set_pt (vgl_point_3d<double> (v->get_pt().x(), v->get_pt().y(), z));
      //just use the z value as the new height

      //vcl_cout << "new v=" << v->get_pt() << vcl_endl;
    }
    // FIX THIS , uncomment
    //bwm_algo::move_points_to_plane(current_extr_face);
    notify_observers("update");
  }
}

void bwm_observable_mesh::divide_face(unsigned face_id,
                                 vgl_point_3d<double> l1, vgl_point_3d<double> l2,
                                 vgl_point_3d<double> p1,
                                 vgl_point_3d<double> l3, vgl_point_3d<double> l4,
                                 vgl_point_3d<double> p2)
{
  bmsh3d_face_mc* face = (bmsh3d_face_mc*) object_->facemap(face_id);
  vcl_vector<bmsh3d_halfedge *> halfedges;
  if (face == 0) {
    print_faces();
    vcl_cerr << "Face " << face_id << " is not found in the facemap\n";
  }
  face->get_incident_HEs(halfedges);
  vcl_vector<bmsh3d_vertex *> vertices;
  extract_face(face, vertices);
  // create 2 new vertices
  bmsh3d_vertex* v1 = (bmsh3d_vertex*) object_->_new_vertex ();
  v1->get_pt().set(p1.x(), p1.y(), p1.z());
  object_->_add_vertex(v1);

  bmsh3d_vertex* v2 = (bmsh3d_vertex*) object_->_new_vertex ();
  v2->get_pt().set(p2.x(), p2.y(), p2.z());
  object_->_add_vertex(v2);

  // find the halfedges corresponding to edge segments
  bmsh3d_edge* edge1=0;
  bmsh3d_edge* edge2=0;

  //vcl_cout << "p1=" << p1 << " p2=" << p2 << vcl_endl
  //         << "l1=" << l1 << vcl_endl
  //         << "l2=" << l2 << vcl_endl
  //         << "l3=" << l3 << vcl_endl
  //         << "l4=" << l4 << vcl_endl;
  for (unsigned i=0; i<halfedges.size(); i++) {
    bmsh3d_halfedge* he = (bmsh3d_halfedge*) halfedges[i];
    bmsh3d_edge* edge = he->edge();
    bmsh3d_vertex* s = (bmsh3d_vertex*) edge->sV();
    bmsh3d_vertex* e = (bmsh3d_vertex*) edge->eV();
    vgl_homg_point_3d<double> sp(s->get_pt());
    vgl_homg_point_3d<double> ep(e->get_pt());


    vgl_homg_line_3d_2_points<double> line(sp, ep);
    //vcl_cout << "edge" << edge->id() << " s=" << s->get_pt() << "e =" << e->get_pt() << vcl_endl;

   double d1 = vgl_distance(vgl_homg_point_3d<double>(l1), line);
   double d2 = vgl_distance(vgl_homg_point_3d<double>(l2), line);
   double d3 = vgl_distance(vgl_homg_point_3d<double>(l3), line);
   double d4 = vgl_distance(vgl_homg_point_3d<double>(l4), line);

   // we are checking if the points l1, l2, l3 or l4 are on the line
   if (d1 < 0.1 && d2 < 0.1)
     edge1 = edge;
   if (d3 < 0.1 && d4 < 0.1)
     edge2 = edge;

#if 0 // commented out
    if (collinear(line, vgl_homg_point_3d<double>(l1)) &&
      collinear(line, vgl_homg_point_3d<double>(l2)))
        edge1 = edge;

    if (collinear(line, vgl_homg_point_3d<double>(l3)) &&
      collinear(line, vgl_homg_point_3d<double>(l4)))
        edge2 = edge;
#endif // 0
  }

  if (edge1 == 0 || edge2 == 0) {
    vcl_cerr << "bwm_observable_mesh::divide_face -- edges are not found in polygon\n";
    return;
  }

  bmsh3d_face_mc *f1, *f2;
  mesh_break_face(object_, face, edge1, edge2, v1, v2, f1, f2);
  int num_faces = object_->face_id_counter();
  object_->orient_face_normals();
  notify_observers("update");
}

//: Creates an inner face
bmsh3d_face* bwm_observable_mesh::create_inner_face(vsol_polygon_3d_sptr polygon)
{
  polygon = bwm_algo::move_points_to_plane(polygon);
  unsigned n = polygon->size();
  bmsh3d_face* inner_face = new bmsh3d_face();
  // create vertices
  bmsh3d_vertex* prev_v = 0;
  bmsh3d_vertex* v_first;
  for (unsigned i=0; i<n; i++) {
    double x = polygon->vertex(i)->x();
    double y = polygon->vertex(i)->y();
    double z = polygon->vertex(i)->z();
    bmsh3d_vertex* v = (bmsh3d_vertex*) object_->_new_vertex ();
    v->get_pt().set(x, y, z);
    object_->_add_vertex(v); //??? do we add this vertex to the mesh
    if (prev_v != 0) {
      //object_->add_new_edge (v, prev_v);
      bmsh3d_edge* edge = new  bmsh3d_edge(prev_v, v, 100);
      bmsh3d_halfedge *he = new bmsh3d_halfedge (edge, inner_face);
      inner_face->_connect_HE_to_end(he);
    } else {
      v_first = v;
    }
    prev_v = v;
  }
  // add an edge between vertices (0, n-1)
  bmsh3d_edge* edge = new  bmsh3d_edge(v_first, prev_v, 100);
  bmsh3d_halfedge *he = new bmsh3d_halfedge (edge, inner_face);
  inner_face->_connect_HE_to_end(he);

  return inner_face;
}

//: Creates a polygon from the given vertex list and adds it to the mesh
 bmsh3d_face_mc* bwm_observable_mesh::create_face(vsol_polygon_3d_sptr polygon)
{
  polygon = bwm_algo::move_points_to_plane(polygon);
  unsigned n = polygon->size();

  // create vertices
  bmsh3d_vertex* prev_v = 0;
  for (unsigned i=0; i<n; i++) {
    double x = polygon->vertex(i)->x();
    double y = polygon->vertex(i)->y();
    double z = polygon->vertex(i)->z();
    bmsh3d_vertex* v = (bmsh3d_vertex*) object_->_new_vertex ();
    v->get_pt().set(x, y, z);
    object_->_add_vertex(v);
    if (prev_v != 0)
      object_->add_new_edge (v, prev_v);
    prev_v = v;
  }
  // add an edge between vertices (0, n-1)
  object_->add_new_edge ((bmsh3d_vertex*) object_->vertexmap(0), (bmsh3d_vertex*) object_->vertexmap(n-1));

  // create a polygon, there is only one face
  bmsh3d_face_mc* face = object_->_new_mc_face ();
  //object_->_add_face (face);
  for (unsigned i=0; i<n; i++) {
    bmsh3d_vertex* vertex = (bmsh3d_vertex*) object_->vertexmap(i);
    _connect_F_E_end (face, object_->edgemap(i));
  }
  face->_sort_HEs_circular();
  return face;
}

// given a 3d point on the mesh, it finds the face that the point is on and returns the face index
// of that face. If the point is not close enough to the mesh, it returns -1.
int bwm_observable_mesh::find_closest_face(vgl_point_3d<double> point)
{
  int index = -1;
  double dmin = 1e26;
  vcl_cout << "dmin=" << dmin << vcl_endl;
  vcl_map<int, bmsh3d_face*>::iterator it = object_->facemap().begin();
  vcl_map<int, vsol_polygon_3d_sptr> faces;

  for (; it != object_->facemap().end(); it++) {
    bmsh3d_face_mc* face = (bmsh3d_face_mc*) (*it).second;
    vgl_homg_plane_3d<double> plane = get_plane(face->id());
    double d = vgl_distance(vgl_homg_point_3d<double> (point), plane);
    if (d < dmin) {
      dmin = d;
      index = face->id();
    }
    vcl_cout << face->id() << " dmin=" << dmin << vcl_endl;
  }
  return index;
}

void bwm_observable_mesh::create_mesh_HE(vsol_polygon_3d_sptr polygon,
                                    double dist,
                                    vcl_map<int, vsol_polygon_3d_sptr> inner_faces)
{
  polygon = bwm_algo::move_points_to_plane(polygon);

  unsigned n = polygon->size();

  vcl_vector<bmsh3d_vertex* > v_list(2*n);
  unsigned next_index, next_index_ext;

  // first create the vertices
  for (unsigned i=0; i<n; i++) {
    bmsh3d_vertex* v = (bmsh3d_vertex*) object_->_new_vertex ();
    v->set_pt (vgl_point_3d<double> (polygon->vertex(i)->x(),
      polygon->vertex(i)->y(), polygon->vertex(i)->z()));
    object_->_add_vertex (v);
    v_list[i] = v;
  }

  // create the extruded vertices
  for (unsigned i=0; i<n; i++) {
    bmsh3d_vertex* v = (bmsh3d_vertex*) object_->_new_vertex ();
#if 0 //JLM
    vsol_point_3d p(v->get_pt());
    vsol_point_3d_sptr p = polygon->vertex(i);
    vgl_vector_3d<double> normal = polygon->normal_at_point(p);
#endif
    vgl_vector_3d<double> normal = polygon->normal();
    double fact = dist;
    /*v->set_pt (vgl_point_3d<double> (polygon->vertex(i)->x() + fact*normal.x() ,
      polygon->vertex(i)->y() + fact*normal.y(),
      polygon->vertex(i)->z() + fact*normal.z()));*/
    v->set_pt (vgl_point_3d<double> (polygon->vertex(i)->x(),polygon->vertex(i)->y(),dist));
    object_->_add_vertex (v);
    v_list[n+i] = v;
  }

  // create the edges of parallel faces
  vcl_vector<bmsh3d_edge* > e_list(2*n);
  for (unsigned i=0; i<2*n; i++) {
    next_index = i + 1;
    if (next_index == n)
      next_index = 0;
    else if (next_index == 2*n)
      next_index = n;
    bmsh3d_edge* e = object_->add_new_edge (v_list[i], v_list[next_index]);
    e_list[i] = e;
  }

  // create the first face for z=0
  bmsh3d_face_mc* f0 = object_->_new_mc_face ();
  object_->_add_face (f0);
  for (unsigned i=0; i<n; i++) {
     _connect_F_E_end(f0, e_list[i]);
  }

  // re-aatach the inner faces
  vcl_map<int, vsol_polygon_3d_sptr>::iterator iter = inner_faces.begin();
  while (iter != inner_faces.end()) {
    attach_inner_face(f0->id(), iter->second);
    iter++;
  }

  // create the second face for z=ext_value
  bmsh3d_face_mc* f1 = object_->_new_mc_face ();
  object_->_add_face (f1);
  for (unsigned i=n; i<2*n; i++) {
    _connect_F_E_end (f1, e_list[i]);
  }
  current_extr_face = f1;

  // create the in between edges and faces
  vcl_vector<bmsh3d_edge* > e_btw_list(n);
  for (unsigned i=0; i<n; i++) {
    bmsh3d_edge* e = object_->add_new_edge (v_list[i], v_list[n+i]);
    e_btw_list[i] = e;
  }

  for (unsigned i=0; i<n; i++) {
    // create 2 new edges
    if (i == n-1) {
      next_index = 0;
      next_index_ext = n;
    } else {
      next_index = i+1;
      next_index_ext = n+i+1;
    }
    bmsh3d_face_mc* f = object_->_new_mc_face ();
    object_->_add_face (f);
    _connect_F_E_end (f, e_list[i]);
    _connect_F_E_end (f, e_btw_list[next_index]);
    _connect_F_E_end (f, e_list[n+i]);
    _connect_F_E_end (f, e_btw_list[i]);
  }

  print_faces();
}

void bwm_observable_mesh::attach_inner_face(unsigned face_id, vsol_polygon_3d_sptr poly)
{
  bmsh3d_face* inner_face = create_inner_face(poly);
  bmsh3d_face_mc* outer_face = (bmsh3d_face_mc*) object_->facemap(face_id);
  if (outer_face != 0) {
    bmsh3d_halfedge* he = (bmsh3d_halfedge*) inner_face->halfedge();
    outer_face->add_mc_halfedge(he);
    notify_observers("update");
  } else {
    vcl_cerr << "bwm_observable_mesh::attach_inner_face() -- outer face id is not valid\n";
  }
}

bmsh3d_face_mc* bwm_observable_mesh::extrude_face(bmsh3d_mesh_mc* M, bmsh3d_face_mc* F)
{
  F->_sort_HEs_circular();
  vgl_point_3d<double> center = F->compute_center_pt();
  bmsh3d_face_mc* cur_face = F;
#if 0 // commented out
  if (M->facemap().size() > 1) {
    vcl_vector<bmsh3d_edge*> inc_edges;
    F->get_incident_edges (inc_edges);
    bmsh3d_edge* first_edge = inc_edges[0];
    vgl_vector_3d<double> face_normal = cur_face->compute_normal(center, first_edge, first_edge->s_vertex());
    face_normal /= face_normal.length();
    vcl_vector<bmsh3d_face*> incident_faces;
    for (unsigned i=0; i<inc_edges.size(); i++) {
      bmsh3d_edge* edge = inc_edges[i];
      vcl_vector<bmsh3d_face*> faces;
      edge->get_incident_faces(faces);

      for (unsigned j=0; j<faces.size(); j++) {
        bmsh3d_face_mc* pair_face = (bmsh3d_face_mc*) faces[j];
        if (pair_face!=F) {
          pair_face->_sort_halfedges_circular();
          vgl_point_3d<double> pair_center = pair_face->compute_center_pt();
          vgl_vector_3d<double> n = pair_face->compute_normal(pair_center, edge, edge->s_vertex());
          n /= n.length();
          double a = angle(face_normal, n);
          double ninety_deg = vnl_math::pi/2.0;
          incident_faces.push_back(pair_face);
          // if both faces are on the same plane, they are planar
          //if ((a == 0) || (a == vnl_math::pi)) {
          //  incident_faces.push_back(pair_face);
          //}// else if ((a <= ninety_deg-0.05) || (a >= ninety_deg+0.05)) {
           // incident_faces.push_back(pair_face);
          //}
        }
      }
    }

    for (unsigned i=0; i<incident_faces.size(); i++) {
      bmsh3d_face_mc* inc_face = (bmsh3d_face_mc*) incident_faces[i];
      // check with all the edges, if the incident face share this edge,
      //trying to find the edge between the current face and the given face
      vcl_vector<bmsh3d_edge*> inc_edges;
      bmsh3d_edge* edge;
      cur_face->get_incident_edges (inc_edges);
      for (unsigned j=0; j<inc_edges.size(); j++) {
        if (inc_edges[j]->is_face_incident(inc_face)){
          edge = inc_edges[j];
          break;
        }
      }

      if (edge == 0) {
        vcl_cerr << "ERROR: incident face is not found in extrude_face()\n";
        return 0;
      }

      bmsh3d_halfedge* he = edge->incident_halfedge(cur_face);
      bmsh3d_vertex* v0 = (bmsh3d_vertex*) M->_new_vertex ();
      bmsh3d_vertex* s = (bmsh3d_vertex*) edge->s_vertex();
      bmsh3d_vertex* e = (bmsh3d_vertex*) edge->e_vertex();
      vgl_point_3d<double> p1 = s->get_pt();
      v0->get_pt().set(p1.x(), p1.y(), p1.z());
      bmsh3d_vertex* v1 = (bmsh3d_vertex*) M->_new_vertex ();
      vgl_point_3d<double> p2 = e->get_pt();
      v1->get_pt().set(p2.x(), p2.y(), p2.z());
      bmsh3d_face_mc *f1, *f2;
      bmsh3d_edge* next =  cur_face->find_other_edge(e, edge);
      bmsh3d_edge* prev = cur_face->find_other_edge(s, edge);
      mesh_break_face(M, cur_face, prev, next, v0, v1,  f1, f2);
      if (f1->containing_vertex(s))
        cur_face = f2;
      else
        cur_face = f1;
      this->print_faces();
    }
  // there is only one face, so we will extrude iT anyway
  } else {
    vcl_vector<bmsh3d_edge*> inc_edges;
    cur_face->get_incident_edges (inc_edges);
    for (unsigned j=0; j<inc_edges.size(); j++) {
      bmsh3d_edge* edge = inc_edges[j];
      bmsh3d_halfedge* he = edge->halfedge();
      bmsh3d_vertex* v0 = (bmsh3d_vertex*) M->_new_vertex ();
      bmsh3d_vertex* s = (bmsh3d_vertex*) edge->s_vertex();
      bmsh3d_vertex* e = (bmsh3d_vertex*) edge->e_vertex();
      vgl_point_3d<double> p1 = s->get_pt();
      v0->get_pt().set(p1.x(), p1.y(), p1.z());
      bmsh3d_vertex* v1 = (bmsh3d_vertex*) M->_new_vertex ();
      vgl_point_3d<double> p2 = e->get_pt();
      v1->get_pt().set(p2.x(), p2.y(), p2.z());
      bmsh3d_face_mc *f1, *f2;
      bmsh3d_edge* next =  cur_face->find_other_edge(e, edge);
      bmsh3d_edge* prev = cur_face->find_other_edge(s, edge);
      mesh_break_face(M, cur_face, prev, next, v0, v1, f1, f2);
      if (f1->containing_vertex(s))
        cur_face = f2;
      else
        cur_face = f1;
    }
#endif // 0
  vcl_vector<bmsh3d_vertex*> v_list;
  vcl_vector<bmsh3d_edge*> e_vert_list;
  bmsh3d_halfedge* he = (bmsh3d_halfedge*) cur_face->halfedge();
  bmsh3d_vertex* s = he->s_vertex();
  bmsh3d_vertex* v0 = (bmsh3d_vertex*) M->_new_vertex ();
  vgl_point_3d<double> p1 = s->get_pt();
  v0->get_pt().set(p1.x(), p1.y(), p1.z());
  M->_add_vertex (v0);
  bmsh3d_edge* edge = M->_new_edge(s,v0);
  e_vert_list.push_back(edge);
  v_list.push_back(v0);
#if 0
  bmsh3d_vertex* v1 = (bmsh3d_vertex*) M->_new_vertex ();
  vgl_point_3d<double> p2 = e->get_pt();
  v1->get_pt().set(p2.x(), p2.y(), p2.z());
  v_list.push_back(v1);
#endif // 0
  bmsh3d_halfedge* next = he->next();
  while (next != he) {
    bmsh3d_vertex* v = next->s_vertex();
    bmsh3d_vertex* v1 = (bmsh3d_vertex*) M->_new_vertex ();
    vgl_point_3d<double> p2 = v->get_pt();
    v1->get_pt().set(p2.x(), p2.y(), p2.z());
    v_list.push_back(v1);
    M->_add_vertex (v1);
    bmsh3d_edge* edge = M->_new_edge(v,v1);
    e_vert_list.push_back(edge);
    next = next->next();
  }

  // create th new top face
  vcl_vector<bmsh3d_edge* > e_hor_list;
  bmsh3d_face_mc* new_face = M->_new_mc_face();
  M->_add_face (new_face);
  for (unsigned i=0; i < v_list.size(); i++) {
    int next_index = i + 1;
    if (next_index == v_list.size())
      next_index = 0;
    bmsh3d_edge* e = M->add_new_edge (v_list[i], v_list[next_index]);
    e_hor_list.push_back(e);
    _connect_F_E_end (new_face, e);
  }

  // create the in between faces
  he = cur_face->halfedge();
  bmsh3d_edge* e = he->edge();
  next = he;
  int i = 0, next_i = 1;
  do {
    bmsh3d_face_mc* face = M->_new_mc_face();
    M->_add_face (face);
    bmsh3d_edge* e = next->edge();
    if (i == e_vert_list.size()-1)
      next_i=0;
    else
      next_i = i+1;
    _connect_F_E_end (face, e_vert_list[i]);
    _connect_F_E_end (face, e_hor_list[i]);
    _connect_F_E_end (face, e_vert_list[next_i]);
    _connect_F_E_end (face, e);
    next = next->next();
    i++;
  } while (next != he);

  //}
  new_face->_sort_HEs_circular();
  vcl_ostringstream oss;
  new_face->getInfo(oss);

  vcl_cout << oss.str().c_str();
  return new_face;
}

vgl_homg_plane_3d<double> bwm_observable_mesh::get_plane(unsigned face_id)
{
  bmsh3d_face* face = object_->facemap(face_id);

  vcl_vector<vsol_point_3d_sptr> points;
  vgl_fit_plane_3d<double> fitter;
  for (unsigned i=0; i<face->vertices().size(); i++) {
    bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(i);
    vgl_homg_point_3d<double> hp(v->get_pt().x(), v->get_pt().y(), v->get_pt().z());
    fitter.add_point(hp);
  }

  vgl_homg_plane_3d<double> plane;
#if 0
  if (face->vertices().size()==0)
    return plane;
#endif
  if (fitter.fit(0.0001)) {
  plane = fitter.get_plane();
  } else {
    vcl_cout << "NO FITTING" << vcl_endl;
  }

  return plane;
}

void bwm_observable_mesh::print_faces()
{
  vcl_ostringstream oss;
  vcl_map<int, bmsh3d_face*>::iterator it = object_->facemap().begin();

  for (; it != object_->facemap().end(); it++) {
    bmsh3d_face_mc* face = (bmsh3d_face_mc*) (*it).second;
    //face->_sort_halfedges_circular();
    face->getInfo(oss);
  }
  vcl_cout << oss.str().c_str();
}

void bwm_observable_mesh::move_points_to_plane(bmsh3d_face_mc* face)
{
 bmsh3d_face* temp = object_->facemap(face->id());
  if (temp->vertices().size()==0)
    return;
  vgl_homg_plane_3d<double> plane = get_plane(face->id());

  // find the closest point on the plane and replace it for each point
  vcl_vector<vsol_point_3d_sptr> points;
  for (unsigned i=0; i<face->vertices().size(); i++) {
    bmsh3d_vertex* v = (bmsh3d_vertex*) face->vertices(i);
    vgl_homg_point_3d<double> hp(v->get_pt().x(), v->get_pt().y(), v->get_pt().z());
    vgl_homg_point_3d<double> p = vgl_closest_point(plane, hp);
    v->set_pt (vgl_point_3d<double> (p.x()/p.w(), p.y()/p.w(), p.z()/p.w()));
  }
}

void bwm_observable_mesh::shrink_mesh(bmsh3d_mesh_mc* mesh, double r)
{
  mesh->orient_face_normals();

  vcl_map<int, bmsh3d_vertex* > vertices = mesh->vertexmap();
  vcl_vector<vgl_point_3d<double> > new_vertices;
  vcl_map<int, bmsh3d_vertex* >::iterator v_it = vertices.begin();
  while (v_it != vertices.end()) {
    bmsh3d_vertex* vertex = (bmsh3d_vertex*) v_it->second;
    vgl_homg_point_3d<double> p(vertex->get_pt());
    vcl_cout << "old vertex->" << p << vcl_endl;
    // find the faces incident to this vertex
    vcl_set<bmsh3d_face*> inc_faces;
    vertex->get_incident_Fs(inc_faces);
    if (inc_faces.size() < 3) {
      vcl_cerr << "The number of planes < 3!!!!!!!!!!!\n";
    }

    vcl_set<bmsh3d_face*>::iterator it = inc_faces.begin();

    while (it != inc_faces.end()) {
      bmsh3d_face_mc* face1 = (bmsh3d_face_mc*) *it;
      vgl_vector_3d<double> n1 = face1->compute_normal();
      n1 /= n1.length();
      it++;

      bmsh3d_face* face2 = (bmsh3d_face_mc*) *it;
      vgl_vector_3d<double> n2 = face2->compute_normal();
      n2 /= n2.length();
      it++;

      bmsh3d_face* face3 = (bmsh3d_face_mc*) *it;
      vgl_vector_3d<double> n3 = face3->compute_normal();
      n3 /= n3.length();
      it++;
      vgl_point_3d<double> v = bwm_algo::fit_sphere_to_corner(p, -1*n1, p, -1*n2, p , -1*n3, r);
      vcl_cout << "New vertex->" << v << vcl_endl;
      new_vertices.push_back(v);
    }
    v_it++;
  }

  //update the vertex values
  //assert(vertices.size() == new_vertices.size());
  v_it = vertices.begin();
  unsigned i=0;
  while (v_it != vertices.end()) {
    bmsh3d_vertex* v1 = (bmsh3d_vertex*) v_it->second;
    v1->set_pt(new_vertices[i++]);
    v_it++;
  }
}

void bwm_observable_mesh::triangulate()
{
  //object_->build_IFS_mesh();
  bmsh3d_mesh* tri_mesh = generate_tri_mesh(object_);
  tri_mesh->IFS_to_MHE();
  bmsh3d_mesh_mc* tri_mesh_mc = new bmsh3d_mesh_mc(tri_mesh);
  delete object_;
  object_ = tri_mesh_mc;
  object_->orient_face_normals();
  //delete tri_mesh;
  notify_observers("update");
}

bwm_observable_sptr bwm_observable_mesh::global_to_local(bgeo_lvcs* lvcs, double& min_z)
{
  if (lvcs) {
    bmsh3d_mesh_mc* mesh = object_->clone();
    vcl_map<int, bmsh3d_vertex*>::iterator vit;
    for (vit = mesh->vertexmap().begin(); vit!= mesh->vertexmap().end(); vit++) {
      bmsh3d_vertex* v = (bmsh3d_vertex*)vit->second;
      double x,y,z;
      lvcs->global_to_local(v->pt().x(), v->pt().y(), v->pt().z(),
        bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
      if (z < min_z) {
        min_z = z;
      }
    }
    return new bwm_observable_mesh(mesh);
  }
  return 0;
}

SoSeparator* bwm_observable_mesh::convert_coin3d(bool b_shape_hints,
                                                 float transp,
                                                 int colorcode)
{
  if (object_->is_MHE() == false)
    object_->IFS_to_MHE();
  object_->orient_face_normals();
  object_->build_face_IFS ();
  SoSeparator* mesh = draw_M (object_, b_shape_hints, transp, colorcode);
  mesh->ref();
  return mesh;
}

void bwm_observable_mesh::load_from(vcl_string filename)
{

  vcl_string msg = "new";
  if (object_) {
    delete object_;
    msg = "update";
  }

  object_ = new bmsh3d_mesh_mc();
  if (!bmsh3d_load_ply(object_,filename.data())) {
    vcl_cerr << "Error loading mesh from " << filename << vcl_endl;
    return;
  }

  // build half-edge structure
  object_->IFS_to_MHE();
  object_->orient_face_normals();
  notify_observers(msg);
}

void bwm_observable_mesh::save(const char* filename, bgeo_lvcs* lvcs)
{
  object_->build_IFS_mesh();
  bmsh3d_mesh* mesh2 = object_->clone();

  if (!lvcs){
    vcl_cerr << "error: lvcs == null\n";
    return;
  }

  vcl_map<int, bmsh3d_vertex*>::iterator it = mesh2->vertexmap().begin();
  for (; it != mesh2->vertexmap().end(); it++) {
    bmsh3d_vertex* V = (bmsh3d_vertex*) (*it).second;
    double x=0,y=0,z=0;
    lvcs->global_to_local(V->pt().x(),V->pt().y(),V->pt().z(),bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
    vgl_point_3d<double> new_pt(x,y,z);
    vcl_cout << "converted global <"<<V->pt().x() <<", "<< V->pt().y()
             <<", "<< V->pt().z() <<"> to <" <<x<< ", "<<y<<" ,"<<z<<"> "<<vcl_endl;
    V->set_pt(new_pt);
  }

  mesh2->build_IFS_mesh();
  bmsh3d_save_ply2(mesh2, filename);
  return;
}

void bwm_observable_mesh::save(const char* filename)
{
  //object_->build_IFS_mesh();
  bmsh3d_mesh* mesh2 = object_->clone();

  mesh2->build_IFS_mesh();
  vcl_string comment;
  if (this->mesh_type_ == BWM_MESH_FEATURE)
    comment = BWM_MESH_FEATURE_STR;
  else if (this->mesh_type_ == BWM_MESH_IMAGE_PROCESSING)
    comment = BWM_MESH_IMAGE_PROCESSING_STR;
  else if (this->mesh_type_ == BWM_MESH_TERRAIN)
    comment = BWM_MESH_TERRAIN_STR;
  else {
    comment = "";
    vcl_cerr << "Mesh type is invalid\n";
  }

  bmsh3d_save_ply(mesh2, filename, true, comment);
  return;
}
