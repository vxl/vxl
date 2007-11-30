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
  this->set_foreground(1.0, 1.0, 0.0);
  //float wx, wy;
  //vgui_projection_inspector pi;
  //pi.image_to_window_coordinates(x, y, wx, wy);
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

void bwm_observer_vgui::handle_update(vgui_message const& msg,
                                      bwm_observable_sptr observable)
{
  const vcl_string* str = static_cast<const vcl_string*> (msg.data);

  vcl_map<unsigned, bgui_vsol_soview2D_polygon* > poly_list;
  if (str->compare("delete") == 0) {
    vcl_map<unsigned, bgui_vsol_soview2D_polygon* > p = objects_[observable];
    vcl_map<unsigned, vcl_vector<bwm_soview2D_vertex*> > ov = object_verts_[observable];
    objects_.erase(observable);
    object_verts_.erase(observable);
    for (unsigned i=0; i<p.size(); i++)  {
      if (p[i]) {
        this->remove(p[i]);
        for (unsigned j = 0; j<ov[i].size(); j++)
          this->remove(ov[i][j]);
      }
    }
  } else {
    vcl_map<int, vsol_polygon_3d_sptr> faces = observable->extract_faces();
    vcl_map<int, vsol_polygon_3d_sptr>::iterator iter = faces.begin();
    vcl_map<unsigned, vcl_vector<bwm_soview2D_vertex*> >poly_verts;

    while (iter != faces.end()) {
      // project the new object with the given camera
      int face_id = iter->first;
      vsol_polygon_3d_sptr obj = iter->second;
      vsol_polygon_2d_sptr poly_2d;
      proj_poly(obj, poly_2d);
      unsigned nverts = poly_2d->size();
      float *x, *y;
      bwm_algo::get_vertices_xy(poly_2d, &x, &y);

      this->set_foreground(1,1,0);
      bgui_vsol_soview2D_polygon* polygon = this->add_vsol_polygon_2d(poly_2d);
      poly_list[face_id] = polygon;

      proj_poly(obj, poly_2d);
      vcl_vector<bwm_soview2D_vertex*> verts;
      this->set_foreground(0,1,0);
      //JLM Changed vertex size
      for (unsigned i = 0; i<nverts; ++i) {
        bwm_soview2D_vertex* sopt = new bwm_soview2D_vertex(x[i], y[i], 2.0f, polygon, i);
        this->add(sopt);
        verts.push_back(sopt);
      }

      poly_verts[face_id] = verts;

      // get the inner faces connected to this face
      vcl_map<int, vsol_polygon_3d_sptr> inner_faces = observable->extract_inner_faces(face_id);
      vcl_map<int, vsol_polygon_3d_sptr>::iterator inner_iter= inner_faces.begin();
      while (inner_iter != inner_faces.end()) {
        vsol_polygon_3d_sptr poly = inner_iter->second;
        vsol_polygon_2d_sptr poly_2d;
        proj_poly(poly, poly_2d);
        float *x, *y;
        bwm_algo::get_vertices_xy(poly_2d, &x, &y);
        bgui_vsol_soview2D_polygon* polygon = this->add_vsol_polygon_2d(poly_2d);
        poly_list[face_id] = polygon;
        inner_iter++;
      }
      iter++;
    }

    if (str->compare("new") == 0) {
      //this->set_foreground(0.0, 0.5, 0.5);
      objects_[observable] = poly_list;
      object_verts_[observable] = poly_verts;
    } else if ((str->compare("update") == 0) || (str->compare("move") == 0)){
      vcl_map<unsigned, bgui_vsol_soview2D_polygon* > p = objects_[observable];
      vcl_map<unsigned, vcl_vector<bwm_soview2D_vertex* > > ov = object_verts_[observable];

      vcl_map<unsigned, bgui_vsol_soview2D_polygon* >::iterator it =  p.begin();
      while (it != p.end()) {
        // remove the polygon
        this->remove(it->second);

        // remove the vertices
        vcl_vector<bwm_soview2D_vertex* > vertices = ov[it->first];
        for (unsigned j = 0; j < vertices.size(); j++)
          this->remove(vertices[j]);

        it++;
      }
      objects_[observable] = poly_list;
      object_verts_[observable] = poly_verts;
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

  // now try the circles, if not found in polygons
  vcl_map<bwm_observable_sptr,
    vcl_map<unsigned, vcl_vector<bwm_soview2D_vertex* > > >::iterator v_iter = object_verts_.begin();

  while (v_iter != object_verts_.end()) {
    vcl_map<unsigned, vcl_vector<bwm_soview2D_vertex* > > v  = v_iter->second;
    vcl_map<unsigned, vcl_vector<bwm_soview2D_vertex* > >::iterator obs = v.begin();
    while (obs != v.end()) {
      vcl_vector<bwm_soview2D_vertex* > vertices = obs->second;
      for (unsigned i=0; i < vertices.size(); i++) {
        if (vertices[i]->get_id() == soview2D_id) {
          face_id = obs->first;
          return v_iter->first;
        }
      }
      obs++;
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
    object_verts_[found_obj][found_poly_index];
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

#if 0 // commented out
void bwm_observer_vgui::hist_plot()
{
  bwm_image_processor::hist_plot(img_);
}
#endif // 0

#if 0 // commented out
void bwm_observer_vgui::intensity_profile(float start_col, float start_row,
                                          float end_col, float end_row)
{
  bwm_image_processor::intensity_profile(img_, start_col, start_row, end_col, end_row);
}
#endif // 0

#if 0 // commented out
void bwm_observer_vgui::range_map()
{
  bwm_image_processor::range_map(img_);
}
#endif // 0
