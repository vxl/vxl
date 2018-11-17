#include "vsol_mesh_3d.h"
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vgl/algo/vgl_fit_plane_3d.h>
#include <vnl/algo/vnl_svd.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_box_3d.h>
#include <vsol/vsol_line_3d.h>
#include <vsol/vsol_point_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_intersection.h>


static vsol_polygon_3d_sptr move_poly_points_to_plane(const vsol_polygon_3d_sptr& polygon)
{
  vgl_fit_plane_3d<double> fitter;
  for (unsigned i=0; i<polygon->size(); i++) {
    fitter.add_point(polygon->vertex(i)->x(),
                     polygon->vertex(i)->y(),
                     polygon->vertex(i)->z());
  }

  vgl_homg_plane_3d<double> plane;

  if (fitter.fit(0.1, &std::cerr)) {
  //fitter.fit();

  plane = fitter.get_plane();
  }
  else {
    std::cout << "NO FITTING" << std::endl;
    return nullptr;
  }


  // find the closest point on the plane and replace it for each point
  std::vector<vsol_point_3d_sptr> points;
  for (unsigned i=0; i<polygon->size(); i++) {
    vgl_homg_point_3d<double> hp(polygon->vertex(i)->x(),
                                 polygon->vertex(i)->y(),
                                 polygon->vertex(i)->z());
    vgl_homg_point_3d<double> p = vgl_closest_point(plane, hp);
    points.push_back(new vsol_point_3d(p.x()/p.w(), p.y()/p.w(), p.z()/p.w()));
  }
  vsol_polygon_3d_sptr new_polygon = new vsol_polygon_3d(points);
  return new_polygon;
}

static vsol_polygon_3d_sptr move_points_to_plane(std::vector<vsol_point_3d_sptr> points)
{
  vgl_fit_plane_3d<double> fitter;
  std::cout << "fitting----------------" << std::endl;
  for (auto & point : points) {
    fitter.add_point(point->x(),
                     point->y(),
                     point->z());
    std::cout << *point << std::endl;
  }

  vgl_homg_plane_3d<double> plane;

  if (fitter.fit(1.0, &std::cerr)) {
  // fitter.fit();
  plane = fitter.get_plane();
  }
  else {
    std::cout << "NO FITTING" << std::endl;
    return nullptr;
  }


  // find the closest point on the plane and replace it for each point
  std::vector<vsol_point_3d_sptr> new_points;
  for (auto & point : points) {
    vgl_homg_point_3d<double> hp(point->x(), point->y(), point->z());
    vgl_homg_point_3d<double> p = vgl_closest_point(plane, hp);
    new_points.push_back(new vsol_point_3d(p.x()/p.w(), p.y()/p.w(), p.z()/p.w()));
  }
  vsol_polygon_3d_sptr polygon = new vsol_polygon_3d(new_points);
  return polygon;
}

//: Finds the center of the sphere with radius r that is tangent to the given three planes
static vgl_point_3d<double> fit_sphere_to_corner(vgl_point_3d<double> P1, vgl_vector_3d<double> N1,
                                                 vgl_point_3d<double> P2, vgl_vector_3d<double> N2,
                                                 vgl_point_3d<double> P3, vgl_vector_3d<double> N3,
                                                 double r)
{
  vgl_vector_3d<double> v1 = vgl_vector_3d<double> (P1.x(), P1.y(), P1.z()) + (r*N1);
  vgl_point_3d<double> p1(v1.x(), v1.y(), v1.z());
  vgl_homg_plane_3d<double> plane1(N1, vgl_homg_point_3d<double>(p1));

  vgl_vector_3d<double> v2 = vgl_vector_3d<double> (P2.x(), P2.y(), P2.z()) + (r*N2);
  vgl_point_3d<double> p2(v2.x(), v2.y(), v2.z());
  vgl_homg_plane_3d<double> plane2(N2, vgl_homg_point_3d<double>(p2));

  vgl_vector_3d<double> v3 = vgl_vector_3d<double> (P3.x(), P3.y(), P3.z()) + (r*N3);
  vgl_point_3d<double> p3(v3.x(), v3.y(), v3.z());
  vgl_homg_plane_3d<double> plane3(N3, vgl_homg_point_3d<double>(p3));

  vgl_homg_point_3d<double> Q = vgl_homg_operators_3d<double>::intersection(plane1, plane2, plane3);

  return (vgl_point_3d<double> (Q.x()/Q.w(), Q.y()/Q.w(), Q.z()/Q.w()));
}

// construct from box
vsol_mesh_3d::vsol_mesh_3d(const vsol_box_3d_sptr& box){
// get the bottom polygon
  double xmin = box->get_min_x(), xmax = box->get_max_x();
  double ymin = box->get_min_y(), ymax = box->get_max_y();
  double zmin = box->get_min_z(), zmax = box->get_max_z();
  std::vector<vsol_point_3d_sptr> verts;
  verts.push_back(new vsol_point_3d(xmin, ymin, zmin));
  verts.push_back(new vsol_point_3d(xmax, ymin, zmin));
  verts.push_back(new vsol_point_3d(xmin, ymax, zmin));
  verts.push_back(new vsol_point_3d(xmax, ymax, zmin));
  vsol_polygon_3d_sptr poly = new vsol_polygon_3d(verts);
  // create the mesh
  mesh_ = new bmsh3d_mesh_mc();
  bmsh3d_face_mc* face = this->create_face(poly);
  mesh_->_add_face (face);
  mesh_->orient_face_normals();
  // extrude the bottom till top
  this->extrude(0, zmax-zmin);
}
void vsol_mesh_3d::create_mesh_HE(const vsol_polygon_3d_sptr& polygon,
                                  double dist,
                                  std::map<int, vsol_polygon_3d_sptr> inner_faces)
{
  //polygon = bwm_algo::move_points_to_plane(polygon);

  unsigned n = polygon->size();

  std::vector<bmsh3d_vertex* > v_list(2*n);
  unsigned int next_index;

  // first create the vertices
  for (unsigned i=0; i<n; i++) {
    auto* v = (bmsh3d_vertex*) mesh_->_new_vertex ();
    v->set_pt (vgl_point_3d<double> (polygon->vertex(i)->x(),
                                     polygon->vertex(i)->y(), polygon->vertex(i)->z()));
    mesh_->_add_vertex (v);
    v_list[i] = v;
  }

  // create the extruded vertices
  for (unsigned i=0; i<n; i++) {
    auto* v = (bmsh3d_vertex*) mesh_->_new_vertex ();
    v->set_pt (vgl_point_3d<double> (polygon->vertex(i)->x(),
                                     polygon->vertex(i)->y(),
                                     polygon->vertex(i)->z()+ dist));
    mesh_->_add_vertex (v);
    v_list[n+i] = v;
  }

  // create the edges of parallel faces
  std::vector<bmsh3d_edge* > e_list(2*n);
  for (unsigned i=0; i<2*n; i++) {
    next_index = i + 1;
    if (next_index == n)
      next_index = 0;
    else if (next_index == 2*n)
      next_index = n;
    bmsh3d_edge* e = mesh_->add_new_edge (v_list[i], v_list[next_index]);
    e_list[i] = e;
  }

  // create the first face for z=0
  bmsh3d_face_mc* f0 = mesh_->_new_mc_face ();
  mesh_->_add_face (f0);
  for (unsigned i=0; i<n; i++) {
    _connect_F_E_end(f0, e_list[i]);
  }

  // re-attach the inner faces
  auto iter = inner_faces.begin();
  while (iter != inner_faces.end()) {
    attach_inner_face(f0->id(), iter->second);
    iter++;
  }

  // create the second face for z=ext_value
  bmsh3d_face_mc* f1 = mesh_->_new_mc_face ();
  mesh_->_add_face (f1);
  for (unsigned i=n; i<2*n; i++) {
    _connect_F_E_end (f1, e_list[i]);
  }
  current_extr_face_ = f1;

  // create the in between edges and faces
  std::vector<bmsh3d_edge* > e_btw_list(n);
  for (unsigned i=0; i<n; i++) {
    bmsh3d_edge* e = mesh_->add_new_edge (v_list[i], v_list[n+i]);
    e_btw_list[i] = e;
  }

  for (unsigned i=0; i<n; i++) {
    // create 2 new edges
    if (i == n-1) {
      next_index = 0;
      // next_index_ext = n;
    }
    else {
      next_index = i+1;
      // next_index_ext = n+i+1;
    }
    bmsh3d_face_mc* f = mesh_->_new_mc_face ();
    mesh_->_add_face (f);
    _connect_F_E_end (f, e_list[i]);
    _connect_F_E_end (f, e_btw_list[next_index]);
    _connect_F_E_end (f, e_list[n+i]);
    _connect_F_E_end (f, e_btw_list[i]);
  }

  print_faces();
}
std::vector<vsol_point_3d_sptr> vsol_mesh_3d::vertices() const{
  std::vector<vsol_point_3d_sptr> ret;
  int n = this->num_vertices();
  std::map<int, bmsh3d_vertex*>& vmap = mesh_->vertexmap();
  for(int i = 0; i<n; ++i){
    bmsh3d_vertex* mv = vmap[i];
    vsol_point_3d_sptr v = new vsol_point_3d(mv->pt());
    ret.push_back(v);
  }
  return ret;
}
void vsol_mesh_3d::attach_inner_face(unsigned face_id, const vsol_polygon_3d_sptr& poly)
{
  bmsh3d_face* inner_face = create_inner_face(poly);
  auto* outer_face = (bmsh3d_face_mc*) mesh_->facemap(face_id);
  if (outer_face != nullptr) {
    auto* he = (bmsh3d_halfedge*) inner_face->halfedge();
    outer_face->add_mc_halfedge(he);
  }
  else {
    std::cerr << "vsol_mesh_3d::attach_inner_face() -- outer face id is not valid\n";
  }
}
bmsh3d_face_mc* vsol_mesh_3d::extrude_face(bmsh3d_mesh_mc* M, bmsh3d_face_mc* F)
{
  F->_sort_HEs_circular();
  bmsh3d_face_mc* cur_face = F;
#if 0 // commented out
  vgl_point_3d<double> center = F->compute_center_pt();
  if (M->facemap().size() > 1)
  {
    std::vector<bmsh3d_edge*> inc_edges;
    F->get_incident_edges (inc_edges);
    bmsh3d_edge* first_edge = inc_edges[0];
    vgl_vector_3d<double> face_normal = cur_face->compute_normal(center, first_edge, first_edge->s_vertex());
    face_normal /= face_normal.length();
    std::vector<bmsh3d_face*> incident_faces;
    for (unsigned i=0; i<inc_edges.size(); i++) {
      bmsh3d_edge* edge = inc_edges[i];
      std::vector<bmsh3d_face*> faces;
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
#if 0
          // if both faces are on the same plane, they are planar
          if ((a == 0) || (a == vnl_math::pi))
            incident_faces.push_back(pair_face);
          else if ((a <= ninety_deg-0.05) || (a >= ninety_deg+0.05))
            incident_faces.push_back(pair_face);
#endif // inner "#if 0"
        }
      }
    }

    for (unsigned i=0; i<incident_faces.size(); i++) {
      bmsh3d_face_mc* inc_face = (bmsh3d_face_mc*) incident_faces[i];
      // check with all the edges, if the incident face share this edge,
      //trying to find the edge between the current face and the given face
      std::vector<bmsh3d_edge*> inc_edges;
      bmsh3d_edge* edge;
      cur_face->get_incident_edges (inc_edges);
      for (unsigned j=0; j<inc_edges.size(); j++) {
        if (inc_edges[j]->is_face_incident(inc_face)) {
          edge = inc_edges[j];
          break;
        }
      }

      if (edge == 0) {
        std::cerr << "ERROR: incident face is not found in extrude_face()\n";
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
  }
  else
  {
    std::vector<bmsh3d_edge*> inc_edges;
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
  }
#endif // 0
  std::vector<bmsh3d_vertex*> v_list;
  std::vector<bmsh3d_edge*> e_vert_list;
  auto* he = (bmsh3d_halfedge*) cur_face->halfedge();
  bmsh3d_vertex* s = he->s_vertex();
  auto* v0 = (bmsh3d_vertex*) M->_new_vertex ();
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
    auto* v1 = (bmsh3d_vertex*) M->_new_vertex ();
    vgl_point_3d<double> p2 = v->get_pt();
    v1->get_pt().set(p2.x(), p2.y(), p2.z());
    v_list.push_back(v1);
    M->_add_vertex (v1);
    bmsh3d_edge* edge = M->_new_edge(v,v1);
    e_vert_list.push_back(edge);
    next = next->next();
  }

  // create th new top face
  std::vector<bmsh3d_edge* > e_hor_list;
  bmsh3d_face_mc* new_face = M->_new_mc_face();
  M->_add_face (new_face);
  for (unsigned i=0; i < v_list.size(); i++) {
    unsigned int next_index = i + 1;
    if (next_index == v_list.size())
      next_index = 0;
    bmsh3d_edge* e = M->add_new_edge (v_list[i], v_list[next_index]);
    e_hor_list.push_back(e);
    _connect_F_E_end (new_face, e);
  }

  // create the in between faces
  he = cur_face->halfedge();
  next = he;
  unsigned int i = 0, next_i = 1;
  do {
    bmsh3d_face_mc* face = M->_new_mc_face();
    M->_add_face (face);
    bmsh3d_edge* e = next->edge();
    if (i+1 == e_vert_list.size())
      next_i=0;
    else
      next_i = i+1;
    _connect_F_E_end (face, e_vert_list[i]);
    _connect_F_E_end (face, e_hor_list[i]);
    _connect_F_E_end (face, e_vert_list[next_i]);
    _connect_F_E_end (face, e);
    next = next->next();
    ++i;
  } while (next != he);

  new_face->_sort_HEs_circular();
  std::ostringstream oss;
  new_face->getInfo(oss);

  std::cout << oss.str().c_str();
  return new_face;
}

void vsol_mesh_3d::move_extr_face(double z)
{
  if (current_extr_face_) {
    std::vector<bmsh3d_vertex*> vertices;
    vsol_polygon_3d_sptr polygon = extract_face(current_extr_face_, vertices);
    // at this point, the halfedges already sorted in extract_face

    for (unsigned i=0; i<vertices.size(); i++) {
      bmsh3d_vertex* v = vertices[i];
      //vgl_vector_3d<double> normal = current_extr_face->compute_normal(edge, v);
      vsol_point_3d_sptr p = polygon->vertex(i);
      //vgl_vector_3d<double> normal = polygon->normal_at_point(p);
      v->set_pt (vgl_point_3d<double> (v->get_pt().x(), v->get_pt().y(), v->get_pt().z()+z));
      //just use the z value as the new height

      //std::cout << "new v=" << v->get_pt() << std::endl;
    }
  }
}

vgl_plane_3d<double> vsol_mesh_3d::get_plane(unsigned face_id)
{
  bmsh3d_face* face = mesh_->facemap(face_id);
  std::vector<bmsh3d_vertex*> vertices;
  face->get_ordered_Vs(vertices);

  std::vector<vsol_point_3d_sptr> points;
  vgl_fit_plane_3d<double> fitter;
  for (auto & vertice : vertices) {
    auto* v = (bmsh3d_vertex*) vertice;
    vgl_homg_point_3d<double> hp(v->get_pt().x(), v->get_pt().y(), v->get_pt().z());
    fitter.add_point(hp);
  }

  vgl_plane_3d<double> plane;
#if 0
  if (face->vertices().size()==0)
    return plane;
#endif
  if (fitter.fit(0.0001, &std::cerr)) {
    plane = fitter.get_plane();
  }
  else {
    std::cout << "NO FITTING" << std::endl;
  }

  return plane;
}

void vsol_mesh_3d::print_faces()
{
  std::ostringstream oss;
  auto it = mesh_->facemap().begin();

  for (; it != mesh_->facemap().end(); it++) {
    auto* face = (bmsh3d_face_mc*) (*it).second;
    //face->_sort_halfedges_circular();
    face->getInfo(oss);
  }
  std::cout << oss.str().c_str();
}

void vsol_mesh_3d::move_points_to_plane(bmsh3d_face_mc* face)
{
  bmsh3d_face* temp = mesh_->facemap(face->id());
  if (temp->vertices().size()==0)
    return;
  vgl_plane_3d<double> plane = get_plane(face->id());

  // find the closest point on the plane and replace it for each point
  std::vector<vsol_point_3d_sptr> points;
  for (unsigned i=0; i<face->vertices().size(); i++) {
    auto* v = (bmsh3d_vertex*) face->vertices(i);
    vgl_point_3d<double> hp(v->get_pt().x(), v->get_pt().y(), v->get_pt().z());
    vgl_point_3d<double> p = vgl_closest_point(plane, hp);
    v->set_pt (vgl_point_3d<double> (p.x(), p.y(), p.z()));
  }
}

void vsol_mesh_3d::shrink_mesh(bmsh3d_mesh_mc* mesh, double r)
{
  mesh->orient_face_normals();

  std::map<int, bmsh3d_vertex* > vertices = mesh->vertexmap();
  std::vector<vgl_point_3d<double> > new_vertices;
  auto v_it = vertices.begin();
  while (v_it != vertices.end()) {
    auto* vertex = (bmsh3d_vertex*) v_it->second;
    vgl_point_3d<double> p(vertex->get_pt());
    std::cout << "old vertex->" << p << std::endl;
    // find the faces incident to this vertex
    std::set<bmsh3d_face*> inc_faces;
    vertex->get_incident_Fs(inc_faces);
    if (inc_faces.size() < 3) {
      std::cerr << "The number of planes < 3!!!!!!!!!!!\n";
    }

    auto it = inc_faces.begin();

    while (it != inc_faces.end()) {
      auto* face1 = (bmsh3d_face_mc*) *it;
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
      vgl_point_3d<double> v = fit_sphere_to_corner(p, -1*n1, p, -1*n2, p , -1*n3, r);
      std::cout << "New vertex->" << v << std::endl;
      new_vertices.push_back(v);
    }
    v_it++;
  }

  //update the vertex values
#ifdef DEBUG
  assert(vertices.size() == new_vertices.size());
#endif
  v_it = vertices.begin();
  unsigned i=0;
  while (v_it != vertices.end()) {
    auto* v1 = (bmsh3d_vertex*) v_it->second;
    v1->set_pt(new_vertices[i++]);
    v_it++;
  }
}
void vsol_mesh_3d::compute_bounding_box() const{
  vgl_box_3d<double> bb;
  detect_bounding_box(mesh_, bb);
  vgl_point_3d<double> minpt = bb.min_point();
  vgl_point_3d<double> maxpt = bb.max_point();
  set_bounding_box(minpt.x(), minpt.y(), minpt.z());
  add_to_bounding_box(maxpt.x(), maxpt.y(), maxpt.z());
}

bool vsol_mesh_3d::single_face_with_vertices(unsigned face_id,
                                             vsol_polygon_3d_sptr& poly,
                                             std::vector<bmsh3d_vertex*>& verts)
{
  auto* face = (bmsh3d_face_mc*)mesh_->facemap(face_id);
  if (face && (mesh_->facemap().size() == 1)) {
    auto* face = (bmsh3d_face_mc*) mesh_->facemap(face_id);
    poly = this->extract_face(face, verts);
    return true;
  }
  else
    return false;
}
void vsol_mesh_3d::set_mesh(bmsh3d_mesh_mc* obj)
{
  if (mesh_ != nullptr){
    delete mesh_;
  }
  mesh_ = obj;
}

void vsol_mesh_3d::set_mesh(const vsol_polygon_3d_sptr& poly, double z)
{
  if (mesh_ != nullptr)
    delete mesh_;

  mesh_ = new bmsh3d_mesh_mc;
  std::map<int, vsol_polygon_3d_sptr> inner_faces;
  create_mesh_HE(poly, z, inner_faces);
}

void vsol_mesh_3d::set_mesh(const vsol_polygon_3d_sptr& poly)
{
  if (!poly || poly->size()==0)
    return;

  if (mesh_ != nullptr)
    delete mesh_;

  mesh_ = new bmsh3d_mesh_mc;
  bmsh3d_face_mc* face = create_face(poly);
  mesh_->_add_face (face);
}

//: Creates a polygon from the given vertex list and adds it to the mesh
bmsh3d_face_mc* vsol_mesh_3d::create_face(const vsol_polygon_3d_sptr& polygon)
{
  //  polygon = bwm_algo::move_points_to_plane(polygon); //later
  unsigned n = polygon->size();
  // create vertices
  bmsh3d_vertex* prev_v = nullptr;
  for (unsigned i=0; i<n; i++) {
    double x = polygon->vertex(i)->x();
    double y = polygon->vertex(i)->y();
    double z = polygon->vertex(i)->z();
    auto* v = (bmsh3d_vertex*) mesh_->_new_vertex ();
    v->get_pt().set(x, y, z);
    mesh_->_add_vertex(v);
    if (prev_v != nullptr)
      mesh_->add_new_edge (v, prev_v);
    prev_v = v;
  }
  // add an edge between vertices (0, n-1)
  mesh_->add_new_edge ((bmsh3d_vertex*) mesh_->vertexmap(0), (bmsh3d_vertex*) mesh_->vertexmap(n-1));

  // create a polygon, there is only one face
  bmsh3d_face_mc* face = mesh_->_new_mc_face ();

  for (unsigned i=0; i<n; i++) {
    // from bmsh_mesh_3d (public function)
    _connect_F_E_end (face, mesh_->edgemap(i));
  }
  face->_sort_HEs_circular();
  return face;
}

//  If there are inner faces, there are more than one polygon,
//  otherwise it is always one.
vsol_polygon_3d_sptr vsol_mesh_3d::extract_face(bmsh3d_face_mc* face,
                                                std::vector<bmsh3d_vertex*> &vertices)
{
  std::vector<vsol_point_3d_sptr> v_list;
  auto* cur_he = (bmsh3d_halfedge*) face->halfedge();
  do {
    auto* next_he = (bmsh3d_halfedge*) cur_he->next();
    auto* vertex = (bmsh3d_vertex*) Es_sharing_V  (cur_he->edge(), next_he->edge());

    vertices.push_back(vertex);
    //std::cout << "vertex " << vertex->id() << " between "
    //         << cur_he->edge()->id() << " and "
    //         << next_he->edge()->id() << std::endl;
    vgl_point_3d<double> p = vertex->get_pt();
    v_list.push_back(new vsol_point_3d (p.x(), p.y(), p.z()));
    cur_he = (bmsh3d_halfedge*) cur_he->next();
  }
  while (cur_he != face->halfedge());

  vsol_polygon_3d_sptr poly3d = new vsol_polygon_3d(v_list);
  return poly3d;
}

std::map<int, vsol_polygon_3d_sptr>
vsol_mesh_3d::extract_inner_faces(bmsh3d_face_mc* face)
{
  // now, add the inner polygons
  std::map<int, bmsh3d_halfedge*> set_he = face->get_mc_halfedges();
  std::map<int, vsol_polygon_3d_sptr> polygons;
  auto it = set_he.begin();
  while (it != set_he.end())
  {
    bmsh3d_halfedge* he = it->second;
    bmsh3d_halfedge* HE = he;
    std::vector<vsol_point_3d_sptr> v_list;

    do {
      auto* next_he = (bmsh3d_halfedge*) HE->next();
      auto* vertex = (bmsh3d_vertex*) Es_sharing_V  (HE->edge(), next_he->edge());
      //vertices.push_back(vertex);
      vgl_point_3d<double> p = vertex->get_pt();
      v_list.push_back(new vsol_point_3d (p.x(), p.y(), p.z()));
      HE = next_he;
    }
    while (HE != he);

    vsol_polygon_3d_sptr poly3d = new vsol_polygon_3d(v_list);
    polygons[it->first] = poly3d;
    it++;
  }
  return polygons;
}
//: Creates an inner face
bmsh3d_face* vsol_mesh_3d::create_inner_face(vsol_polygon_3d_sptr polygon)
{
  polygon = move_poly_points_to_plane(polygon);
  unsigned n = polygon->size();
  auto* inner_face = new bmsh3d_face();
  // create vertices
  bmsh3d_vertex* prev_v = nullptr;
  bmsh3d_vertex* v_first;
  for (unsigned i=0; i<n; i++) {
    double x = polygon->vertex(i)->x();
    double y = polygon->vertex(i)->y();
    double z = polygon->vertex(i)->z();
    auto* v = (bmsh3d_vertex*) mesh_->_new_vertex ();
    v->get_pt().set(x, y, z);
    mesh_->_add_vertex(v); //??? do we add this vertex to the mesh
    if (prev_v != nullptr) {
      auto* edge = new  bmsh3d_edge(prev_v, v, 100);
      auto *he = new bmsh3d_halfedge (edge, inner_face);
      inner_face->_connect_HE_to_end(he);
    }
    else {
      v_first = v;
    }
    prev_v = v;
  }
  // add an edge between vertices (0, n-1)
  auto* edge = new  bmsh3d_edge(v_first, prev_v, 100);
  auto *he = new bmsh3d_halfedge (edge, inner_face);
  inner_face->_connect_HE_to_end(he);

  return inner_face;
}

std::map<int, vsol_polygon_3d_sptr>
vsol_mesh_3d::extract_inner_faces(int face_id)
{
  auto* face = (bmsh3d_face_mc*)mesh_->facemap(face_id);
  std::map<int, vsol_polygon_3d_sptr> polys;
  if (face) {
    polys = extract_inner_faces(face);
  }
  return polys;
}

vsol_polygon_3d_sptr vsol_mesh_3d::extract_face(unsigned i)
{
  auto* face = (bmsh3d_face_mc*) mesh_->facemap(i);
  std::vector<bmsh3d_vertex*> vertices;
  vsol_polygon_3d_sptr poly = extract_face(face, vertices);
  return poly;
}

vsol_polygon_3d_sptr vsol_mesh_3d::extract_bottom_face()
{
  auto it = mesh_->facemap().begin();
  std::map<int, vsol_polygon_3d_sptr> faces;
  double min_z=1e6;
  vsol_polygon_3d_sptr bottom;
  for (; it != mesh_->facemap().end(); it++) {
    auto* face = (bmsh3d_face_mc*) (*it).second;
    std::vector<bmsh3d_vertex*> vertices;
    vsol_polygon_3d_sptr poly = this->extract_face(face, vertices);
    vsol_box_3d_sptr bb = poly->get_bounding_box();
    // want to find face with least max z
    if (min_z > bb->get_max_z()) {
      min_z = bb->get_max_z();
      bottom = poly;
    }
  }
  return bottom;
}
vsol_polygon_3d_sptr vsol_mesh_3d::extract_top_face()
{
  auto it = mesh_->facemap().begin();
  std::map<int, vsol_polygon_3d_sptr> faces;
  double max_z=-1.0e6;
  vsol_polygon_3d_sptr top;
  for (; it != mesh_->facemap().end(); it++) {
    auto* face = (bmsh3d_face_mc*) (*it).second;
    std::vector<bmsh3d_vertex*> vertices;
    vsol_polygon_3d_sptr poly = this->extract_face(face, vertices);
    vsol_box_3d_sptr bb = poly->get_bounding_box();
    // want to find face with largest min z
    if (max_z < bb->get_min_z()) {
      max_z = bb->get_min_z();
      top = poly;
    }
  }
  return top;
}
std::map<int, vsol_polygon_3d_sptr> vsol_mesh_3d::extract_faces()
{
  auto it = mesh_->facemap().begin();
  std::map<int, vsol_polygon_3d_sptr> faces;

  for (; it != mesh_->facemap().end(); it++) {
    auto* face = (bmsh3d_face_mc*) (*it).second;
    //std::cout << "face " << face->id() << std::endl;
    std::vector<bmsh3d_vertex*> vertices;
    vsol_polygon_3d_sptr poly = this->extract_face(face, vertices);
    faces[face->id()] = poly;
  }
  return faces;
}

std::map<int, vsol_line_3d_sptr> vsol_mesh_3d::extract_edges()
{
  auto it = mesh_->edgemap().begin();
  std::map<int, vsol_line_3d_sptr> edges;

  for (; it != mesh_->edgemap().end(); it++) {
    auto* edge = (bmsh3d_edge*) (*it).second;
    bmsh3d_vertex* V0 = edge->vertices(0);
    bmsh3d_vertex* V1 = edge->vertices(1);
    vsol_point_3d_sptr v0 = new vsol_point_3d(V0->pt().x(),V0->pt().y(),V0->pt().z());
    vsol_point_3d_sptr v1 = new vsol_point_3d(V1->pt().x(),V1->pt().y(),V1->pt().z());
    vsol_line_3d_sptr e = new vsol_line_3d(v0, v1);
    edges[edge->id()] = e;
  }
  return edges;
}

std::vector<vsol_point_3d_sptr> vsol_mesh_3d::extract_vertices()
{
  std::vector<vsol_point_3d_sptr> vertices;
  if (mesh_) {
    auto it = mesh_->vertexmap().begin();
    for (; it != mesh_->vertexmap().end(); it++) {
      auto* V = (bmsh3d_vertex*) it->second;
      vsol_point_3d_sptr pt = new vsol_point_3d(V->pt().x(),V->pt().y(),V->pt().z());
      vertices.push_back(pt);
    }
  }
  return vertices;
}

void vsol_mesh_3d::create_interior()
{
  if (mesh_) {
    bmsh3d_mesh_mc* interior = mesh_->clone();
    //double l = mesh_->edgemap(0)->length();
    shrink_mesh(interior, 0);//l/10.);
    //merge_mesh(mesh_, interior);
    mesh_=interior;
  }
}

void vsol_mesh_3d::extrude(int face_id, double dist)
{
  if (mesh_ != nullptr) {
    auto* face = (bmsh3d_face_mc*)mesh_->facemap(face_id);
    if (face) {
      if (mesh_->facemap().size() == 1) {
        auto* face = (bmsh3d_face_mc*) mesh_->facemap(face_id);
        std::vector<bmsh3d_vertex*> vertices;
        vsol_polygon_3d_sptr poly = this->extract_face(face, vertices);
        std::map<int, vsol_polygon_3d_sptr> inner_faces = this->extract_inner_faces(face);
        delete mesh_;
        mesh_ = new bmsh3d_mesh_mc();
        // the first polygon is always the outer one
        create_mesh_HE(poly, dist, inner_faces);
      }
      else {
        current_extr_face_ = extrude_face(mesh_, face);
        move_extr_face(dist);
      }
    }
    else
      current_extr_face_ = nullptr;
  }
}
void vsol_mesh_3d::divide_face(unsigned face_id,
                                      vgl_point_3d<double> l1, vgl_point_3d<double> l2,
                                      vgl_point_3d<double> p1,
                                      vgl_point_3d<double> l3, vgl_point_3d<double> l4,
                                      vgl_point_3d<double> p2)
{
  auto* face = (bmsh3d_face_mc*) mesh_->facemap(face_id);
  std::vector<bmsh3d_halfedge *> halfedges;
  if (face == nullptr) {
    print_faces();
    std::cerr << "Face " << face_id << " is not found in the facemap\n";
  }
  face->get_incident_HEs(halfedges);
  std::vector<bmsh3d_vertex *> vertices;
  extract_face(face, vertices);
  // create 2 new vertices
  auto* v1 = (bmsh3d_vertex*) mesh_->_new_vertex ();
  v1->get_pt().set(p1.x(), p1.y(), p1.z());
  mesh_->_add_vertex(v1);

  auto* v2 = (bmsh3d_vertex*) mesh_->_new_vertex ();
  v2->get_pt().set(p2.x(), p2.y(), p2.z());
  mesh_->_add_vertex(v2);

  // find the halfedges corresponding to edge segments
  bmsh3d_edge* edge1=nullptr;
  bmsh3d_edge* edge2=nullptr;

#ifdef DEBUG
  std::cout << "p1=" << p1 << " p2=" << p2 << '\n'
           << "l1=" << l1 << '\n'
           << "l2=" << l2 << '\n'
           << "l3=" << l3 << '\n'
           << "l4=" << l4 << std::endl;
#endif
  double min_d1=1e23, min_d2=1e23;
  unsigned min_index1=0, min_index2=0;
  for (unsigned i=0; i<halfedges.size(); i++) {
    auto* he = (bmsh3d_halfedge*) halfedges[i];
    bmsh3d_edge* edge = he->edge();
    auto* s = (bmsh3d_vertex*) edge->sV();
    auto* e = (bmsh3d_vertex*) edge->eV();
    vgl_point_3d<double> sp(s->get_pt());
    vgl_point_3d<double> ep(e->get_pt());

    vgl_line_3d_2_points<double> line(sp, ep);
    //std::cout << "edge" << edge->id() << " s=" << s->get_pt() << "e =" << e->get_pt() << std::endl;

    double d1 = vgl_distance(vgl_point_3d<double>(l1), line);
    double d2 = vgl_distance(vgl_point_3d<double>(l2), line);
    double d3 = vgl_distance(vgl_point_3d<double>(l3), line);
    double d4 = vgl_distance(vgl_point_3d<double>(l4), line);

    // we are checking if the points l1, l2, l3 or l4 are on the line
    if ((d1+d2) < min_d1) {
      min_d1 = d1+d2;
      min_index1 = i;
    }

    if ((d3+d4) < min_d2) {
      min_d2 = d3+d4;
      min_index2 = i;
    }

#if 0 // commented out
    if (collinear(line, vgl_point_3d<double>(l1)) &&
        collinear(line, vgl_point_3d<double>(l2)))
      edge1 = edge;

    if (collinear(line, vgl_point_3d<double>(l3)) &&
        collinear(line, vgl_point_3d<double>(l4)))
      edge2 = edge;
#endif // 0
  }
  auto* he1 = (bmsh3d_halfedge*) halfedges[min_index1];
  edge1 = he1->edge();

  auto* he2 = (bmsh3d_halfedge*) halfedges[min_index2];
  edge2 = he2->edge();

  if (edge1 == nullptr || edge2 == nullptr) {
    std::cerr << "vsol_mesh_3d::divide_face -- edges are not found in polygon\n";
    return;
  }

  bmsh3d_face_mc *f1, *f2;
  mesh_break_face(mesh_, face, edge1, edge2, v1, v2, f1, f2);
  //int num_faces = mesh_->face_id_counter();
  mesh_->orient_face_normals();
}
// implement me!
bool vsol_mesh_3d::in(vsol_point_3d_sptr const&  /*p*/) const{
  return false;
}
