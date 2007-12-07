#include "bwm_observer_vgui.h"
//:
// \file
#include "bwm_observer_mgr.h"
#include "bwm_world.h"
#include "algo/bwm_algo.h"
#include "algo/bwm_image_processor.h"

#include "bwm_tableau_mgr.h"

#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_vector_3d.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_3d.h>

#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_message.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_range_map_params.h>

#include <bgui/bgui_image_utils.h>
#include <bgui/bgui_image_tableau.h>
#include <bgui/bgui_vsol_soview2D.h>

#define SHRINK_MESH 1

bwm_observer_vgui::bwm_observer_vgui(bgui_image_tableau_sptr const& img)
    : bwm_observer_img(img), moving_face_(0) 
{
  corr_.second = 0;
  mesh_style_= vgui_style::new_style();
  mesh_style_->rgba[0] = 0.75f; 
  mesh_style_->rgba[1] = 0.75f; 
  mesh_style_->rgba[2] = 0.0f; 
  mesh_style_->line_width = 3.0;

  vertex_style_ = vgui_style::new_style();
  vertex_style_->rgba[0] = 0.0f; 
  vertex_style_->rgba[1] = 1.0f; 
  vertex_style_->rgba[2] = 0.0f; 
  vertex_style_->line_width = 2.0;
}

bool bwm_observer_vgui::handle(const vgui_event& e)
{
  // handle mouse clicks for correspondences
  vgui_projection_inspector pi;
  if (e.type == vgui_BUTTON_DOWN &&
      e.button == vgui_LEFT &&
      e.modifier == vgui_SHIFT &&
      bwm_observer_mgr::instance()->in_corr_picking())
  {
      float x,y;
      pi.window_to_image_coordinates(e.wx, e.wy, x, y);
      this->set_corr(x, y);
      return true;
  }

  return base::handle(e);
}

void bwm_observer_vgui::add_cross(float x, float y, float r)
{
  bwm_soview2D_cross* cross = new bwm_soview2D_cross(x, y, r);
  corr_.second = cross;
  this->add(cross);
  this->post_redraw();
}

void bwm_observer_vgui::corr_image_pt(float& x, float& y)
{
  vgl_point_2d<double> pt = corr_.first;
  x = pt.x(); y = pt.y();
}

void bwm_observer_vgui::show_vertices(bool show)
{
  // do nothing if the status is the same
  if (show != show_vertices_) {
    show_vertices_ = show;
    if (show) {
      // put the vertices on the tableau
      vcl_map<bwm_observable_sptr, vcl_vector<vsol_point_2d_sptr> >::iterator it = object_verts_xy_.begin();
      vcl_vector<bwm_soview2D_vertex*> new_vertex_list;
      while (it != object_verts_xy_.end()) {
        bwm_observable_sptr obs = it->first;

        vcl_map<unsigned, bgui_vsol_soview2D_polygon* > faces = objects_[obs];
        // get the first face of the object
        if (faces.size() == 0) return ;
        vcl_map<unsigned, bgui_vsol_soview2D_polygon* >::iterator face_it = faces.begin();
        bgui_vsol_soview2D_polygon* poly = face_it->second;

        vcl_vector<vsol_point_2d_sptr> vertices = it->second;
        for (unsigned i=0; i<vertices.size(); i++) {
          bwm_soview2D_vertex* v = new bwm_soview2D_vertex(vertices[i]->x(), vertices[i]->y(), 1.0, poly, i);
          this->add(v);
          v->set_style(vertex_style_);
          new_vertex_list.push_back(v);
        }
        object_verts_[obs] = new_vertex_list;
        it++;
      }
      
    } else {
      // remove the vertices from the tableau
      vcl_map<bwm_observable_sptr, vcl_vector<bwm_soview2D_vertex* > >::iterator it = object_verts_.begin();
      while (it != object_verts_.end()) {
        vcl_vector<bwm_soview2D_vertex* > vertices = it->second;
        for (unsigned i=0; i<vertices.size(); i++) {
          bwm_soview2D_vertex* v = vertices[i];
          this->remove(v);
        }
        it++;
      }
    }
    this->post_redraw();
  }
}

void bwm_observer_vgui::draw_mesh(bwm_observable_sptr observable, 
                                  vcl_map<unsigned, bgui_vsol_soview2D_polygon* > &poly_list,
                                  vcl_vector<bwm_soview2D_vertex*> &vertx_list,
                                  vcl_vector<vsol_point_2d_sptr> &vertx_xy_list)

{
  if (observable) {
    vcl_map<int, vsol_polygon_3d_sptr> faces = observable->extract_faces();
    vcl_vector<vsol_point_3d_sptr> vertices = observable->extract_vertices();

    vcl_map<int, vsol_polygon_3d_sptr>::iterator iter = faces.begin();
    while (iter != faces.end()) {
      // project the new object with the given camera
      int face_id = iter->first;
      vsol_polygon_3d_sptr obj = iter->second;
      vsol_polygon_2d_sptr poly_2d;
      proj_poly(obj, poly_2d);
      if (faces.size() > 1)
        poly_2d = shrink_face(poly_2d);

      bgui_vsol_soview2D_polygon* polygon = this->add_vsol_polygon_2d(poly_2d, mesh_style_);
      poly_list[face_id] = polygon;

      proj_poly(obj, poly_2d);

      // get the inner faces connected to this face
      vcl_map<int, vsol_polygon_3d_sptr> inner_faces = observable->extract_inner_faces(face_id);
      vcl_map<int, vsol_polygon_3d_sptr>::iterator inner_iter= inner_faces.begin();
      while (inner_iter != inner_faces.end()) {
        vsol_polygon_3d_sptr poly = inner_iter->second;
        vsol_polygon_2d_sptr poly_2d;
        proj_poly(poly, poly_2d);

        if ((inner_faces.size() > 1) && (SHRINK_MESH))
          poly_2d = shrink_face(poly_2d);
        bgui_vsol_soview2D_polygon* polygon = this->add_vsol_polygon_2d(poly_2d);
        poly_list[face_id] = polygon;
        inner_iter++;
      }
      iter++;
    }

    // add the vertices of the mesh
    for(unsigned i=0; i<vertices.size(); i++) {
      vsol_point_3d_sptr v = vertices[i];
      vgl_point_2d<double> v2d;
      proj_point(v->get_p(), v2d);
      vertx_xy_list.push_back(new vsol_point_2d(v2d.x(), v2d.y()));
      
      if (show_vertices_) {
        if (poly_list.size() == 0) 
          return ;

        vcl_map<unsigned, bgui_vsol_soview2D_polygon* >::iterator face_it = poly_list.begin();
        bgui_vsol_soview2D_polygon* poly = face_it->second;

        bwm_soview2D_vertex* sopt = new bwm_soview2D_vertex(v2d.x(), v2d.y(), 1.0f, poly, i);
        this->add(sopt);
        sopt->set_style(vertex_style_);
        vertx_list.push_back(sopt);
      }
    }
  }
}

void bwm_observer_vgui::add_new_obj(bwm_observable_sptr observable)
{  
  vcl_map<unsigned, bgui_vsol_soview2D_polygon* > poly_list;
  vcl_vector<bwm_soview2D_vertex*> vertx_list;
  vcl_vector<vsol_point_2d_sptr> vertx_list_xy;
  draw_mesh(observable, poly_list, vertx_list, vertx_list_xy);

  if (poly_list.size() > 0) {
    objects_[observable] = poly_list;
    object_verts_[observable] = vertx_list;
    object_verts_xy_[observable] = vertx_list_xy;
  }
}
void bwm_observer_vgui::handle_update(vgui_message const& msg,
                                      bwm_observable_sptr observable)
{
  const vcl_string* str = static_cast<const vcl_string*> (msg.data);

  vcl_map<unsigned, bgui_vsol_soview2D_polygon* > poly_list;
  if (str->compare("delete") == 0) {
    vcl_map<unsigned, bgui_vsol_soview2D_polygon* > p = objects_[observable];
    vcl_vector<bwm_soview2D_vertex*> ov = object_verts_[observable];
    //vcl_vector<vsol_point_2d_sptr> xy = object_verts_xy_[observable];
    //xy.clear();

    objects_.erase(observable);
    object_verts_.erase(observable);
    object_verts_xy_.erase(observable);

    // delete the polygons from the tableau
    for (unsigned i=0; i<p.size(); i++)  {
      if (p[i]) {
        this->remove(p[i]);
        //delete p[i];
      }
    }

    //delete the vertices from the tableau
    for (unsigned i=0; i<ov.size(); i++)  {
      if (ov[i]) {
        this->remove(ov[i]);
        //delete ov[i];
      }
    }
  } else {
        
    vcl_map<unsigned, bgui_vsol_soview2D_polygon* > poly_list;
    vcl_vector<bwm_soview2D_vertex*> poly_verts;
    vcl_vector<vsol_point_2d_sptr> poly_verts_xy;

    draw_mesh(observable, poly_list, poly_verts, poly_verts_xy);

    if (str->compare("new") == 0) {
      objects_[observable] = poly_list;
      object_verts_[observable] = poly_verts;
      object_verts_xy_[observable] = poly_verts_xy;
    } else if ((str->compare("update") == 0) || (str->compare("move") == 0)){
      vcl_map<unsigned, bgui_vsol_soview2D_polygon* > p = objects_[observable];
      vcl_vector<bwm_soview2D_vertex* > ov = object_verts_[observable];
      vcl_map<unsigned, bgui_vsol_soview2D_polygon* >::iterator it =  p.begin();
      while (it != p.end()) {
        // remove the polygon
        this->remove(it->second);
        //delete (it->second);
        it++;
      }

      // remove the vertices
      for (unsigned i=0; i<ov.size(); i++) {
          this->remove(ov[i]);
          //delete(ov[i]);
      }
      objects_[observable] = poly_list;
      object_verts_[observable] = poly_verts;
      object_verts_xy_[observable] = poly_verts_xy;
    }
  }

  this->post_overlay_redraw();
}

//: Updates the soview objects on the pane.
// This is activated by the observer itself, not the observables.
// The change is due to the movement of projection plane, and
// the need for re-projection of observables.
void bwm_observer_vgui::update_all()
{
  vcl_map<bwm_observable_sptr,
    vcl_map<unsigned, bgui_vsol_soview2D_polygon* > >::iterator iter = objects_.begin();

  while (iter != objects_.end()) {
    bwm_observable_sptr obs = iter->first;
    vgui_message msg;
    msg.data = new vcl_string("update");
    handle_update(msg, obs);
    iter++;
  }
}

//: Select a polygon before you call this method.
// If it is a multiface object, it deletes the object where the selected polygon belongs to
void bwm_observer_vgui::delete_object()
{
  // first get the selected polygon
  vcl_vector<vgui_soview*> select_list = this->get_selected_soviews();

  if ((select_list.size() == 1) &&
    (select_list[0]->type_name().compare("bgui_vsol_soview2D_polygon") == 0)) {
      unsigned face_id;
      bwm_observable_sptr obj = find_object(select_list[0]->get_id(), face_id);
      if (obj) {
        bwm_world::instance()->remove(obj);
        obj->remove();
      }
  }
}

//: Deletes the whole set of objects created so far
void bwm_observer_vgui::delete_all()
{
  vcl_map<bwm_observable_sptr,
    vcl_map<unsigned, bgui_vsol_soview2D_polygon* > >::iterator iter;

  while (objects_.size() > 0) {
    iter = objects_.begin();
    bwm_observable_sptr obj = iter->first;
    if (obj) {
      bwm_world::instance()->remove(obj);
      obj->remove();
    }

  }
  objects_.clear();
}


void bwm_observer_vgui::set_corr(float x, float y)
{
  corr_.first = vgl_homg_point_2d<double> (x, y);

  // delete the previous correlation point if valid
  if (corr_.second) {
    this->remove(corr_.second);
  }

  // draw a cross at that point
  add_cross(x, y, 2.0);
}

void bwm_observer_vgui::remove_corr_pt()
{
  if (corr_.second) {
    this->remove(corr_.second);
    corr_.second = 0;
  }
}


//: finds the observable and the face id given by the the vgui soview2d id
bwm_observable_sptr bwm_observer_vgui::find_object(unsigned soview2D_id, unsigned &face_id)
{
  vcl_map<bwm_observable_sptr,
    vcl_map<unsigned, bgui_vsol_soview2D_polygon* > >::iterator iter = objects_.begin();

  while (iter != objects_.end()) {
    vcl_map<unsigned, bgui_vsol_soview2D_polygon*> v  = iter->second;
    vcl_map<unsigned, bgui_vsol_soview2D_polygon*>::iterator obs = v.begin();
    while (obs != v.end()) {
      if (obs->second->get_id() == soview2D_id) {
        face_id = obs->first;
        return iter->first;
      }
      obs++;
    }
    iter++;
  }

  // now try the vertices, if not found in polygons
  vcl_map<bwm_observable_sptr,vcl_vector<bwm_soview2D_vertex* > >::iterator v_iter = object_verts_.begin();

  while (v_iter != object_verts_.end()) {
    vcl_vector<bwm_soview2D_vertex* > v  = v_iter->second;
    //vcl_map<unsigned, vcl_vector<bwm_soview2D_vertex* > >::iterator obs = v.begin();
    //while (obs != v.end()) {
    for (unsigned i=0; i<v.size(); i++) {
      if (v[i]->get_id() == soview2D_id) {
        //face_id = obs->first;
        return v_iter->first;
      }
    }
    v_iter++;
  }
  return 0;
}

//: Translates the 3D objects by vector T, only one if specified, all otherwise
void bwm_observer_vgui::translate(vgl_vector_3d<double> T,
                                  bwm_observable_sptr object)
{
  vcl_map<bwm_observable_sptr,
    vcl_map<unsigned, bgui_vsol_soview2D_polygon* > >::iterator iter = objects_.begin();

  while (iter != objects_.end()) {
    bwm_observable_sptr obs = iter->first;
    if (obs==0) continue;
    if (object==0){
      obs->translate(T);
      continue;
    }
    else if (obs == object){
      obs->translate(T);
    return;
    }
    iter++;
  }
}

void bwm_observer_vgui::connect_inner_face(vsol_polygon_2d_sptr poly2d)
{
  // first get the selected objects
  vcl_vector<vgui_soview*> select_list = this->get_selected_soviews();

  // a polygon should be selected first
  if ((select_list.size() == 1) && (select_list[0]->type_name().compare("bgui_vsol_soview2D_polygon") == 0)) {
    unsigned face_id;
    bwm_observable_sptr obs = this->find_object(select_list[0]->get_id(), face_id);
    if (obs) {
      vsol_polygon_3d_sptr poly3d;
      backproj_poly(poly2d, poly3d);
      obs->attach_inner_face(face_id, poly3d);
    }
  }
}

void bwm_observer_vgui::create_interior()
{
  // first get the selected objects
  vcl_vector<vgui_soview*> select_list = this->get_selected_soviews();

  // a polygon should be selected first
  if ((select_list.size() == 1) && (select_list[0]->type_name().compare("bgui_vsol_soview2D_polygon") == 0)) {
    unsigned face_id;
    bwm_observable_sptr obs = this->find_object(select_list[0]->get_id(), face_id);
    if (obs)
      obs->create_interior();
  }
}

bwm_observable_sptr bwm_observer_vgui::selected_face(unsigned& face_id)
{
  // first get the selected objects
  vcl_vector<vgui_soview*> select_list = this->get_selected_soviews();

  // a polygon should be selected first
  if ((select_list.size() == 1) &&
    (select_list[0]->type_name().compare("bgui_vsol_soview2D_polygon") == 0)) {
    bgui_vsol_soview2D_polygon* polygon = static_cast<bgui_vsol_soview2D_polygon*> (select_list[0]);
    bwm_observable_sptr obs = find_object(polygon->get_id(), face_id);
    return obs;
  }
  return 0;
}

unsigned bwm_observer_vgui::get_selected_3d_vertex_index(unsigned poly_id)
{
  bwm_observable_sptr found_obj = 0;
  unsigned found_poly_index = 0;
  for (vcl_map<bwm_observable_sptr, vcl_map<unsigned, bgui_vsol_soview2D_polygon* > >::iterator
    oit = objects_.begin();
    oit != objects_.end(); ++oit) {
      unsigned pindex = 0;
      vcl_map<unsigned, bgui_vsol_soview2D_polygon* > polys = oit->second;
      for (vcl_map<unsigned, bgui_vsol_soview2D_polygon* >::iterator pit = polys.begin();
           pit != polys.end(); ++pit, ++pindex)
        if (pit->second && pit->second->get_id() == poly_id) {
          found_obj = oit->first;
          found_poly_index = pindex;
        }
  }

  if (!found_obj)
    return 0;

  vcl_vector<bwm_soview2D_vertex* > verts =
    object_verts_[found_obj];
  unsigned found_vert_index = 0;
  for (vcl_vector<bwm_soview2D_vertex* >::iterator vit = verts.begin();
       vit != verts.end(); ++vit, found_vert_index++)
    if (this->is_selected((*vit)->get_id()))
      return found_vert_index;
  return 0;
}

vsol_point_3d_sptr bwm_observer_vgui::selected_vertex()
{
  //Now find out which polygon is selected (if any)
  vcl_vector<vgui_soview*> select_list = this->get_selected_soviews();
  vsol_point_3d_sptr selected_vertex;

  //Check if there is at least one selected polygon
  if (select_list.size() > 0) {
    unsigned list_index = 0;
    bool found = false;
    for (vcl_vector<vgui_soview*>::iterator sit = select_list.begin();
      sit != select_list.end(); ++sit,++list_index)
      if ((*sit)->type_name()!= "bgui_vsol_soview2D_polygon")
          continue;
      else {
        found = true;
        break;
      }

    if (!found)
    {
      return (vsol_point_3d*)0;
    }

    unsigned id = select_list[list_index]->get_id();
    //Here we get the first polygon, no matter how many are selected

    bgui_vsol_soview2D_polygon* polygon = static_cast<bgui_vsol_soview2D_polygon *> (select_list[list_index]);

    if (!polygon) {
      vcl_cout << "Is a face selected?\n";
      return 0;
    }

    //deselect the polygon since we have it

    unsigned face_id;
    //Get the 3d polygon which has been projected onto the construction plane
    bwm_observable_sptr obs = find_object(id, face_id);

    if (!obs) {
      vcl_cout << "Is a face selected?\n";
      return 0;
    }
    vsol_polygon_3d_sptr poly3d = obs->extract_face(face_id);
    unsigned vindex =  get_selected_3d_vertex_index(id);
    if (vindex<poly3d->size())
      selected_vertex = poly3d->vertex(vindex);
    else
      selected_vertex = (vsol_point_3d*)0;
  }
  else
    selected_vertex = (vsol_point_3d*)0;

  return selected_vertex;
}

void bwm_observer_vgui::print_selected_vertex()
{
  vsol_point_3d_sptr sv = selected_vertex();
  if (!sv)
    return;
  vcl_cout.precision(10);
  vcl_cout << "Lat: " << sv->y() << ' '
           << "Lon: " << sv->x() << ' '
           << "Elv: " << sv->z() << '\n';
}

void bwm_observer_vgui::label_roof()
{
  unsigned face_id;
  bwm_observable_sptr obs = selected_face(face_id);
  obs->label_roof(face_id);
}

void bwm_observer_vgui::label_wall()
{
  unsigned face_id;
  bwm_observable_sptr obs = selected_face(face_id);
  obs->label_wall(face_id);
}

//: makes the polygon a little smaller to prevent the face edges overlapping
vsol_polygon_2d_sptr bwm_observer_vgui::shrink_face(vsol_polygon_2d_sptr poly)
{
  double ratio = 0.95;
  vcl_vector<vsol_point_2d_sptr> new_points;
  vgl_point_2d<double> center = poly->centroid()->get_p();
  for (unsigned i=0; i<poly->size(); i++) {
    vsol_point_2d_sptr p = poly->vertex(i);
    vgl_point_2d<double> old_v = p->get_p();
    double x = old_v.x();
    double y = old_v.y();
    vgl_point_2d<double> new_v((x - center.x())*ratio+center.x(), (y - center.y())*ratio+center.y());
    new_points.push_back(new vsol_point_2d(new_v));
  }
  vsol_polygon_2d_sptr new_polygon = new vsol_polygon_2d(new_points);
  return new_polygon;
}
