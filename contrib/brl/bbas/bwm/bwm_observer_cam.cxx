#include <ios>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "bwm_observer_cam.h"
//:
// \file
#include <bwm/bwm_observer_mgr.h>
#include <bwm/bwm_observable_mesh.h>
#include <bwm/bwm_observable_point.h>
#include <bwm/bwm_observable_point_sptr.h>
#include <bwm/bwm_observable_mesh_circular.h>
#include <bwm/algo/bwm_algo.h>
#include <bwm/bwm_plane_fitting_lsf.h>
#include <bwm/algo/bwm_image_processor.h>
#include <bwm/algo/bwm_delaunay_tri.h>
#include <bwm/bwm_tableau_mgr.h>
#include <bwm/bwm_world.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vgui/vgui.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/xio/vgl_xio_point_3d.h>
#include <vgl/xio/vgl_xio_vector_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vpgl/algo/vpgl_ray.h>
#include <vpgl/algo/vpgl_backproject.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_line_3d_sptr.h>
#include <vsol/vsol_line_3d.h>
#include <vsol/vsol_line_2d.h>

#include <vgui/vgui_dialog.h>
#include <vgui/vgui_dialog_extensions.h>
#include <vgui/vgui_projection_inspector.h>
#include <bgui/bgui_vsol_soview2D.h>


#if 0
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_scene_parser.h>
#endif
#include <vsl/vsl_basic_xml_element.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <bpgl/bpgl_camera_utils.h>
bool bwm_observer_cam::geo_position(double u, double v,
                                    double& x, double& y, double& z)
{
  x = 0; y=0; z=0;
  if (!bwm_world::instance()->world_pt_valid())
    return false;
  if (!camera_)
    return false;
  vgl_plane_3d<double> plane = bwm_world::instance()->world_plane();
  vgl_point_2d<double> pt(u, v);
  vgl_point_3d<double> init_world_pt = bwm_world::instance()->world_pt();
  vgl_point_3d<double> world_pt;
  bool good = vpgl_backproject::bproj_plane(camera_, pt, plane, init_world_pt,
                                            world_pt);
  if (!good)
    return false;
  x = world_pt.x();   y = world_pt.y();   z = world_pt.z();
  return true;
}

bool bwm_observer_cam::handle(const vgui_event &e)
{
  // handle movements for meshes
  vgui_projection_inspector pi;
  float x, y;
  pi.window_to_image_coordinates(e.wx, e.wy, x, y);

  if (e.type == vgui_BUTTON_DOWN && e.button == vgui_MIDDLE &&
      e.modifier == vgui_SHIFT)
  {
    if (in_jog_mode_)
    {
      in_jog_mode_ = false;
      return true;
    }

    vgui_text_tableau_sptr tt = img_tab_->text_tab();
    if (tt)
      tt->clear();

    // first get the selected polygon
    std::vector<vgui_soview*> select_list = this->get_selected_soviews();
    if (select_list.size() == 1)
    {
      if (select_list[0]->type_name().compare("bgui_vsol_soview2D_polygon") == 0) {
        unsigned face_id;
        bwm_observable_sptr obj = find_object(select_list[0]->get_id(), face_id);
        if (obj) {
          if (obj->num_faces() == 1) {
            // it is OK to move faces but not meshes with more than one face
            moving_p_ = (bgui_vsol_soview2D*) select_list[0];
            moving_face_ = obj;
            vsol_polygon_3d_sptr poly3d = obj->extract_face(face_id);
            if (poly3d) {
              vgl_homg_plane_3d<double> plane = poly3d->plane();
              moving_face_plane_ = plane;
            }
            start_x_ = x;
            start_y_ = y;
            moving_polygon_ = true;
            moving_vertex_ = false;
          }
          this->deselect();
          return true;
        }
      }
      else if (select_list[0]->type_name().compare("bwm_soview2D_vertex") == 0) {
        start_x_ = x;
        start_y_ = y;
        bwm_soview2D_vertex* v = (bwm_soview2D_vertex*) select_list[0];
        bgui_vsol_soview2D* poly = v->obj();
        unsigned face_id;
        bwm_observable_sptr obj = find_object(poly->get_id(), face_id);
        if (obj) {
          if (obj->num_faces() == 1) {
            moving_v_ = v;
            // since there is only one face we can choose the only one
            moving_p_ = poly;
            moving_face_ = obj;
            vsol_polygon_3d_sptr poly3d = obj->extract_face(face_id);
            if (poly3d) {
              vgl_homg_plane_3d<double> plane = poly3d->plane();
              moving_face_plane_ = plane;
            }
            moving_vertex_ = true;
            moving_polygon_ = false;
          }
          this->deselect();
          return true;
        }
      }
    }
  }
  else if (e.type == vgui_MOTION && e.button == vgui_MIDDLE &&
           e.modifier == vgui_SHIFT && !moving_vertex_ && moving_face_)
  {
    float x_diff = x-start_x_;
    float y_diff = y-start_y_;
    moving_p_->translate(x_diff, y_diff);

    // move all the vertices of the face
    std::vector<bwm_soview2D_vertex* > vertices = object_verts_[moving_face_];
    for (unsigned i=0; i<vertices.size(); i++) {
      bwm_soview2D_vertex* v = vertices[i];
      v->translate(x_diff, y_diff);
    }

    start_x_ = x;
    start_y_ = y;
    post_redraw();
    return true;
  }
  else if (e.type == vgui_MOTION && e.button == vgui_MIDDLE &&
           e.modifier == vgui_SHIFT && moving_vertex_ && moving_face_)
  {
    float x_diff = x-start_x_;
    float y_diff = y-start_y_;
    // find the polygon including this vertex
    if (moving_p_->type_name().compare(POLYGON_TYPE) == 0) {
      bgui_vsol_soview2D_polygon* polygon = (bgui_vsol_soview2D_polygon*) moving_p_;
      unsigned i = find_index_of_v(moving_v_, (bgui_vsol_soview2D_polygon*)moving_p_);
      if (i == (unsigned int)(-1))
        return true;
      moving_v_->translate(x_diff, y_diff);
      polygon->sptr()->vertex(i)->set_x(polygon->sptr()->vertex(i)->x() + x_diff );
      polygon->sptr()->vertex(i)->set_y(polygon->sptr()->vertex(i)->y() + y_diff );
    }
    else {
      std::cerr << moving_p_->type_name() << " is NOT movable!!!!\n";
    }

    start_x_ = x;
    start_y_ = y;
    post_redraw();
    return true;
  }
  else if (e.type == vgui_BUTTON_UP && e.button == vgui_MIDDLE && !moving_vertex_ && moving_face_)
  {
    if (moving_p_->type_name().compare("bgui_vsol_soview2D_polygon") == 0) {
      bgui_vsol_soview2D_polygon* polygon = (bgui_vsol_soview2D_polygon*) moving_p_;
      vsol_polygon_2d_sptr poly2d = polygon->sptr();
      vsol_polygon_3d_sptr poly3d;
      this->backproj_poly(poly2d, poly3d, moving_face_plane_);
      moving_face_->set_object(poly3d);
    }
    this->deselect();
    moving_face_ = nullptr;
    moving_vertex_ = false;
    moving_polygon_ = false;
    return true;
  }
  else if (e.type == vgui_BUTTON_UP && e.button == vgui_MIDDLE &&
           moving_vertex_ && moving_face_)
  {
    unsigned i = find_index_of_v(moving_v_, (bgui_vsol_soview2D_polygon*)moving_p_);
    if (i == (unsigned int)(-1))
      return true;
    if (moving_p_->type_name().compare(POLYGON_TYPE) == 0) {
      bgui_vsol_soview2D_polygon* polygon = (bgui_vsol_soview2D_polygon*) moving_p_;
      bwm_soview2D_vertex* vertex = moving_v_;
      float x, y;
      vertex->get_centroid(&x,&y);
      polygon->sptr()->vertex(i)->set_x(x);
      polygon->sptr()->vertex(i)->set_y(y);
      vsol_polygon_2d_sptr poly2d = polygon->sptr();
      vsol_polygon_3d_sptr poly3d;
      this->backproj_poly(poly2d, poly3d, moving_face_plane_);
      moving_face_->set_object(poly3d);
    }
    this->deselect();
    moving_face_ = nullptr;
    moving_vertex_ = false;
    moving_polygon_ = false;
    return true;
  }
  // extrude mode, goes only in z direction (up and down)
  if (extrude_mode_ && e.type==vgui_KEY_PRESS && e.key == vgui_CURSOR_UP)
  {
    if (e.modifier == vgui_SHIFT)
      extrude_obj_->move_extr_face(1.0);
    else if (e.modifier == vgui_CTRL)
      extrude_obj_->move_extr_face(0.1);
    else
      extrude_obj_->move_extr_face(0.01);
    in_jog_mode_ = true;
    return true;
  }
  if (extrude_mode_ && e.type==vgui_KEY_PRESS && e.key == vgui_CURSOR_DOWN)
  {
    if (e.modifier == vgui_SHIFT)
      extrude_obj_->move_extr_face(-1.0);
    else if (e.modifier == vgui_CTRL)
      extrude_obj_->move_extr_face(-0.1);
    else
      extrude_obj_->move_extr_face(-0.01);
    in_jog_mode_ = true;
    return true;
  }

  // stops the extrude mode
  if (extrude_mode_ && e.type==vgui_KEY_PRESS && e.key == vgui_END)
    extrude_mode_ = false;
  // moving along the optical axis
  if (e.type==vgui_KEY_PRESS && e.key == vgui_CURSOR_UP)
  {
    if (this == bwm_observer_mgr::BWM_MASTER_OBSERVER) {
      if (e.modifier == vgui_SHIFT)
        this->translate_along_optical_cone(1.0);
      else if (e.modifier == vgui_CTRL)
        this->translate_along_optical_cone(0.1);
      else
        this->translate_along_optical_cone(0.01);
    }
    in_jog_mode_ = true;
    return true;
  }
  if (e.type==vgui_KEY_PRESS && e.key == vgui_CURSOR_DOWN)
  {
    if (this == bwm_observer_mgr::BWM_MASTER_OBSERVER) {
      if (e.modifier == vgui_SHIFT)
        this->translate_along_optical_cone(-1.0);
      else if (e.modifier == vgui_CTRL)
        this->translate_along_optical_cone(-0.1);
      else
        this->translate_along_optical_cone(-0.01);
    }
    in_jog_mode_ = true;
    return true;
  }
  // extrude mode, goes only in z direction (up and down)
  if (shadow_mode_ && e.type==vgui_KEY_PRESS && e.modifier == vgui_CTRL)
  {
    if (e.key == 'w' )
      sun_elev_angle_+=0.01;
    else if (e.key == 's')
      sun_elev_angle_-=0.01;
    else if (e.key == 'a')
      sun_azim_angle_-=0.01;
    else if (e.key == 'd')
      sun_azim_angle_+=0.01;

    //sun_azim_angle_=vnl_math::angle_0_to_2pi(sun_azim_angle_);
    //sun_elev_angle_=sun_elev_angle_<vnl_math::pi_over_2 ? vnl_math::pi_over_2 : sun_elev_angle_;
    //sun_elev_angle_=sun_elev_angle_>vnl_math::pi        ? vnl_math::pi : sun_elev_angle_;
    this->project_shadow();
    return true;
  }
  // stops the extrude mode
  if (shadow_mode_ && e.type==vgui_KEY_PRESS && e.key == vgui_END)
    shadow_mode_ = false;

  // changing radius
  if (e.type==vgui_KEY_PRESS && e.key == vgui_CURSOR_LEFT && e.modifier == vgui_SHIFT) {
    std::vector<vgui_soview*> select_list = this->get_selected_soviews();
    if (select_list.size() == 1)
    {
      if (select_list[0]->type_name().compare("bgui_vsol_soview2D_polygon") == 0) {
        unsigned face_id;
        bwm_observable_sptr obj = find_object(select_list[0]->get_id(), face_id);
        if (obj) {
          if (obj->type_name().compare("bwm_observable_mesh_circular") == 0) {
            bwm_observable_mesh_circular* circle = static_cast<bwm_observable_mesh_circular*> (obj.as_pointer());
            circle->decrease_radius(0.1);
          }
        }
      }
    }
    in_jog_mode_ = true;
    return true;
  }

  // changing radius
  if (e.type==vgui_KEY_PRESS && e.key == vgui_CURSOR_RIGHT && e.modifier == vgui_SHIFT) {
    std::vector<vgui_soview*> select_list = this->get_selected_soviews();
    if (select_list.size() == 1)
    {
      if (select_list[0]->type_name().compare("bgui_vsol_soview2D_polygon") == 0) {
        unsigned face_id;
        bwm_observable_sptr obj = find_object(select_list[0]->get_id(), face_id);
        if (obj) {
          if (obj->type_name().compare("bwm_observable_mesh_circular") == 0) {
            bwm_observable_mesh_circular* circle = static_cast<bwm_observable_mesh_circular*> (obj.as_pointer());
            circle->increase_radius(0.1);
          }
        }
      }
    }
    in_jog_mode_ = true;
    return true;
  }
  if (e.type == vgui_MOTION &&
      e.button != vgui_MIDDLE &&
      show_geo_position_&&
      !bwm_observer_img::vgui_status_locked())
  {
    double X, Y, Z;
    if (this->geo_position(x, y, X, Y, Z))
      vgui::out << std::fixed << std::setprecision(6)<< "Lat(deg): "
                << Y << " Lon(deg): "
                << X << std::setprecision(2) << " Ele(m): "
                << Z << " (WGS84)" << std::endl;
  }
  return base::handle(e);
}


void bwm_observer_cam::set_ground_plane(double x1, double y1, double x2, double y2)
{
  vgl_point_3d<double> world_point1, world_point2;
  vgl_plane_3d<double> xy_plane(0, 0, 1, 0);

  if (!intersect_ray_and_plane(vgl_point_2d<double> (x1, y1), xy_plane, world_point1)||intersect_ray_and_plane(vgl_point_2d<double> (x2, y2), xy_plane, world_point2)) {
    std::cout << "bwm_observer_cam::set_ground_plane(.) -"
             << " intersect ray and plane failed\n";
    return;
  }
  // define the third point in z direction which is the normal to the z=0 plane
  vgl_point_3d<double> world_point3(world_point1.x(),
                                    world_point1.y(),
                                    world_point1.z()+3.0);

  proj_plane_ = vgl_plane_3d<double> (world_point1, world_point2, world_point3);
}

void bwm_observer_cam::select_proj_plane()
{
  // first get the selected polygon
  std::vector<vgui_soview*> select_list = this->get_selected_soviews();

  if ((select_list.size() == 1) &&
      (select_list[0]->type_name().compare("bgui_vsol_soview2D_polygon") == 0)) {
    bgui_vsol_soview2D_polygon* polygon = static_cast<bgui_vsol_soview2D_polygon*> (select_list[0]);
    unsigned face_id;
    bwm_observable_sptr obs = find_object(polygon->get_id(), face_id);
    vgl_plane_3d<double> plane = obs->get_plane(face_id);
    set_proj_plane(plane);
  }
  else
    std::cerr << "Select a face for projection plane\n";
}

void bwm_observer_cam::move_ground_plane( vgl_plane_3d<double> master_plane,
                                          vsol_point_2d_sptr new_pt)
{
  // first get the selected vertex
  std::vector<vgui_soview*> select_list = this->get_selected_soviews();

  if ((select_list.size() == 1) &&
      (select_list[0]->type_name().compare("bwm_soview2D_vertex") == 0)) {
    bwm_soview2D_vertex* circle = static_cast<bwm_soview2D_vertex*> (select_list[0]);

    //find the mesh this vertex belongs to
    unsigned int face_id;
    bwm_observable_sptr obs = find_object(circle->get_id(), face_id);

    make_object_selectable(obs, false);
    float x, y;
    circle->get_centroid(&x, &y);
    vsol_point_2d_sptr old_pt = new vsol_point_2d(x,y);
    // create a new plane which is the translation of the proj_plane
    // by a dist,
    // Remember the definition of a plane, which is
    // a*x + b*y + c*z + d = 0
    // where (a,b,c) is your normal, and d is negative distance to origin.

    std::cerr << "old_pt = "<<old_pt->get_p() <<", new_pt = "<<new_pt->get_p() <<'\n';

    // recalculating d based on the old d and the dist
    double d = master_plane.d();
    double a = master_plane.a();
    double b = master_plane.b();
    double c = master_plane.c();

    vsol_point_3d_sptr old_pt3d;
    backproj_point(old_pt, old_pt3d);
    vgl_point_2d<double> master_img_pt;

    bwm_observer_mgr::BWM_MASTER_OBSERVER->proj_point(old_pt3d->get_p(), master_img_pt);

    bwm_plane_fitting_lsf lsf(a, b, c, d, (new vsol_point_2d(master_img_pt)), new_pt, bwm_observer_mgr::BWM_MASTER_OBSERVER, this);
    vnl_levenberg_marquardt lm(lsf);
    vnl_vector<double> fx(1);
    fx[0] = d;
    lm.set_x_tolerance(0.01);//lm.get_x_tolerance()*1e4);
    lm.set_g_tolerance(0.01);//(lm.get_g_tolerance()*1e4);
    lm.set_epsilon_function(0.01);
    lm.set_trace(true);
    lm.minimize_without_gradient(fx);
    lm.diagnose_outcome(std::cout);

    std::cout << " minimization ended\n"
             << "X value after--> " << fx[0]
             << "\nError=" << lm.get_end_error()
             << "\nnum_iterations_" << lm.get_num_iterations() << std::endl;

    // change the projection plane for the master observer and this one.
    // It may be also necessary to change all the observers projection plane,
    // but I am not sure about it.. Will decide later. GT
    master_plane = lsf.plane();
    bwm_observer_mgr::BWM_MASTER_OBSERVER->set_proj_plane(master_plane);
    this->set_proj_plane(master_plane);
    vsol_point_3d_sptr new_pt3d;
    bwm_observer_mgr::BWM_MASTER_OBSERVER->backproj_point((new vsol_point_2d(master_img_pt)), new_pt3d, master_plane);

    // find the translation between the beginning 3d point and ending,
    // after moving the plane
    vgl_vector_3d<double> t(new_pt3d->x() - old_pt3d->x(),
                            new_pt3d->y() - old_pt3d->y(),
                            new_pt3d->z() - old_pt3d->z());
    bwm_observer_mgr::BWM_MASTER_OBSERVER->translate(t, obs);
    return;
  }
  std::cerr << "The original vertex to be moved is not selected!\n";
}

#if 0 //replaced by translate along optical cone
//Translate along *this* observer's optical axis
void bwm_observer_cam::translate_along_optical_axis(double da)
{
  std::vector<vgui_soview*> select_list = this->get_selected_soviews();

  if (select_list.size() == 1 &&
      select_list[0]->type_name().compare("bgui_vsol_soview2D_polygon") == 0)
    {
      bgui_vsol_soview2D_polygon* poly =
        static_cast<bgui_vsol_soview2D_polygon*> (select_list[0]);

      //find the mesh this polygon belongs to
      unsigned int face_id;
      bwm_observable_sptr obs = find_object(poly->get_id(), face_id);
      if (!obs)
      {
        std::cerr << "In bwm_observer_cam::translate_along_optical_axis - "
                 << "nothing selected to move\n";
        return;
      }
      std::vector<vsol_point_3d_sptr> verts = obs->extract_vertices();
      if (!verts.size())
      {
        std::cerr << "In bwm_observer_cam::translate_along_optical_axis - "
                 << "object has no vertices\n";
        return;
      }
      vsol_point_3d_sptr p3d = verts[0];
      vgl_point_3d<double> pg3d= p3d->get_p();
      //get the direction of a ray
      vgl_vector_3d<double> ray_dir;
      if (!vpgl_ray::ray(camera_, pg3d, ray_dir))
      {
        std::cerr << "In bwm_observer_cam::translate_along_optical_axis - "
                 << "ray direction computation failed\n";
        return;
      }
      ray_dir *= da;
      this->translate(ray_dir, obs);
      this->select_object(obs);
      return;
    }
  std::cerr << "In bwm_observer_cam::translate_along_optical_axis - "
           << "not exactly one selected vertex\n";
}
#endif

// expand or contract the object boundary so that the projection in the
// image is invariant to perspective scale change
void bwm_observer_cam::translate_along_optical_cone(double da)
{
  std::vector<vgui_soview*> select_list = this->get_selected_soviews();
#if 0
  std::cout << "#selected before move = " << select_list.size() << '\n';
#endif
  if (select_list.size() == 1 &&
      select_list[0]->type_name().compare("bgui_vsol_soview2D_polygon") == 0)
  {
    bgui_vsol_soview2D_polygon* poly =
      static_cast<bgui_vsol_soview2D_polygon*> (select_list[0]);

    //find the mesh this polygon belongs to
    unsigned int face_id;
    bwm_observable_sptr obs = find_object(poly->get_id(), face_id);
    if (!obs)
    {
      std::cerr << "In bwm_observer_cam::translate_along_optical_axis - "
               << "nothing selected to move\n";
      return;
    }
    // cone method only available on observable_mesh, not in general
    bwm_observable_mesh* omsh = nullptr;
    if (obs->type_name()=="bwm_observable_mesh")
      omsh = static_cast<bwm_observable_mesh*>(obs.ptr());
    else return;

    if (!omsh->move_poly_in_optical_cone(camera_,face_id,da))
      return;
#if 0
    select_list = this->get_selected_soviews();
    std::cout << "#selected after move = " << select_list.size() << '\n';
#endif
    //keep the object selected
    this->select_object(obs);
#if 0
    select_list = this->get_selected_soviews();
    std::cout << "#selected after select = " << select_list.size() << '\n';
#endif
    return;
  }
}

void bwm_observer_cam::proj_point(vgl_point_3d<double> world_pt,
                                  vgl_point_2d<double> &image_pt)
{
  double u,v;
  camera_->project(world_pt.x(), world_pt.y(), world_pt.z(), u,v);
  image_pt.set(u,v);
  return;
}

void bwm_observer_cam::proj_line(vsol_line_3d_sptr line_3d,
                                 vsol_line_2d_sptr &line_2d)
{
  double u0,v0,u1,v1;
  vsol_point_3d_sptr p0 = line_3d->p0();
  vsol_point_3d_sptr p1 = line_3d->p1();
  camera_->project(p0->x(), p0->y(), p0->z(), u0, v0);
  camera_->project(p1->x(), p1->y(), p1->z(), u1, v1);
  line_2d = new vsol_line_2d(vgl_point_2d<double>(u0,v0), vgl_point_2d<double>(u1,v1));
  return;
}

void bwm_observer_cam::proj_poly(vsol_polygon_3d_sptr poly3d,
                                 vsol_polygon_2d_sptr& poly2d)
{
  if (!poly3d)
    return;

  std::vector<vsol_point_2d_sptr> vertices;
  for (unsigned i=0; i<poly3d->size(); i++) {
    double u = 0,v = 0;
#ifdef DEBUG
    std::cout << "3d point " << *(poly3d->vertex(i)) << std::endl;
#endif
    camera_->project(poly3d->vertex(i)->x(), poly3d->vertex(i)->y(), poly3d->vertex(i)->z(),u,v);
    vsol_point_2d_sptr p = new vsol_point_2d(u,v);
    vertices.push_back(p);
  }
  poly2d = new vsol_polygon_2d (vertices);
}

void bwm_observer_cam::create_circular_polygon(std::vector< vsol_point_2d_sptr > ps_list,
                                               vsol_polygon_3d_sptr &circle3d,
                                               int num_sect, double &r, vgl_point_2d<double> &c)
{
  // need at least 3 points to create a circle
  assert (ps_list.size() >= 3);

  // project the points into 3D
  vsol_polygon_2d_sptr poly2d = new vsol_polygon_2d(ps_list);
  vsol_polygon_3d_sptr poly3d;
  vgl_plane_3d<double> proj_plane(0,0,1,0);
  backproj_poly(poly2d, poly3d, proj_plane);

  std::list< vgl_point_2d<double> > points;
  for (unsigned i=0; i<poly3d->size(); i++) {
    vsol_point_3d_sptr v = poly3d->vertex(i);
    points.push_back(vgl_point_2d<double>(v->x(), v->y())); //z's are 0
  }

  if (!bwm_algo::fit_circle(points, r, c)) {
    std::cerr << "There is an error computing the circle\n";
    return;
  }

  num_sect = vnl_math::twopi*r/(r/4.0);

  // create points on the circle
  std::vector< vsol_point_3d_sptr > circle_pts;
  double rho= vnl_math::twopi / num_sect;
  double x, y;
  double cx = c.x(), cy = c.y();
  for (int i=0; i<num_sect; i++) {
    x = cx + r*std::sin(rho*i);
    y = cy + r*std::cos(rho*i);
    circle_pts.push_back(new vsol_point_3d(x,y,0));
  }

  circle3d = new vsol_polygon_3d(circle_pts);
  vsol_polygon_2d_sptr circle2d;
  proj_poly(circle3d, circle2d);
  backproj_poly(circle2d, circle3d);
}

void bwm_observer_cam::triangulate_meshes()
{
  std::map<bwm_observable_sptr, std::map<unsigned, bgui_vsol_soview2D*> >::iterator it;
  int obj_count = 0;
  for (it = objects_.begin(); it != objects_.end(); it++, obj_count++) {
    std::cout << "triangulating mesh " << obj_count << std::endl;
    bwm_observable_sptr obj = it->first;
    if (obj->type_name().compare("bwm_observable_mesh") == 0) {
      bwm_observable_mesh_sptr mesh = static_cast<bwm_observable_mesh*> (obj.as_pointer());
      mesh->triangulate();
    }
  }
}

void bwm_observer_cam::move_corr_point(vsol_point_2d_sptr new_pt)
{
  // first get the selected cross
  std::vector<vgui_soview*> select_list = this->get_selected_soviews();

  if (select_list.size() == 1) {
    if (select_list[0]->type_name().compare("bwm_soview2D_cross") == 0) {
      bwm_soview2D_cross* cross = static_cast<bwm_soview2D_cross*> (select_list[0]);
      vgl_point_2d<double> old_pt(cross->x, cross->y);
      //Change both the corr position and cross display
      bwm_observer_vgui::set_corr(new_pt->x(), new_pt->y());
      // remove the old one
      this->remove(cross);
      // notify the observer mgr about the change
      bwm_observer_mgr::instance()->update_corr(this, old_pt, new_pt->get_p());
    }
  }
}

void bwm_observer_cam::set_corr_to_vertex()
{
  // get the currently selected vertex
  bwm_soview2D_vertex* sov =
    (bwm_soview2D_vertex*)bwm_observer_img::get_selected_object("bwm_soview2D_vertex");
  // extract the location
  if (!sov)
  {
    std::cerr << "In bwm_observer_cam::set_corr_to_vertex() -"
             << " no vertex selected\n";
    return;
  }
  bwm_observer_vgui::set_corr(sov->x, sov->y);
  bwm_observer_img::post_redraw();
}

void bwm_observer_cam::world_pt_corr()
{
  // first get the selected cross
  std::vector<vgui_soview*> select_list = this->get_selected_soviews();
  if (select_list.size() == 1) {
    if (select_list[0]->type_name().compare("bwm_soview2D_vertex") == 0) {
      bwm_soview2D_vertex* vertex = static_cast<bwm_soview2D_vertex*> (select_list[0]);
      vsol_point_2d_sptr vertex_2d = new vsol_point_2d(vertex->x, vertex->y);
      vsol_point_3d_sptr vertex_3d;
      this->backproj_point(vertex_2d, vertex_3d);
      bwm_world::instance()->set_world_pt(vertex_3d->get_p());
    }
    else
      std::cerr << "Please select a vertex only (circle)\n";
  }
  std::cerr << "Please select only one object\n";
}

void bwm_observer_cam::scroll_to_point(double lx, double ly, double lz)
{
  vgl_point_3d<double> world_pt(lx,ly,lz);
  vgl_point_2d<double> image_pt;
  proj_point(world_pt,image_pt);
  float x = static_cast<float>(image_pt.x()), y = static_cast<float>(image_pt.y());
  this->move_to_point(x,y);
}

// ********************** Protected Methods

//: returns the distance between a ray and a 3D polygon
bool bwm_observer_cam::find_intersection_points(vgl_point_2d<double> const img_point1,
                                                vgl_point_2d<double> const img_point2,
                                                vsol_polygon_3d_sptr poly3d,
                                                vgl_point_3d<double>& point1,
                                                /* end points of the first polygon segment*/ vgl_point_3d<double>& l1, vgl_point_3d<double>& l2,
                                                vgl_point_3d<double>& point2,
                                                /* end points of the second polygon segment*/ vgl_point_3d<double>& l3, vgl_point_3d<double>& l4)
{
  vgl_plane_3d<double> poly_plane = poly3d->plane();

  vgl_point_3d<double> p1,p2;
  if (!intersect_ray_and_plane(img_point1, poly_plane, p1)||
      !intersect_ray_and_plane(img_point2, poly_plane, p2))
    return false;

  if (is_ideal(p1))
    std::cout << "p1 is ideal" << std::endl;
  if (is_ideal(p2))
    std::cout << "p2 is ideal" << std::endl;

  vgl_point_3d<double> non_homg_p1(p1);
  vgl_point_3d<double> non_homg_p2(p2);

  double *x_list, *y_list, *z_list;
  bwm_algo::get_vertices_xyz(poly3d, &x_list, &y_list, &z_list);

  double dist1 = vgl_distance_to_closed_polygon (x_list, y_list, z_list, poly3d->size(),
                                                 non_homg_p1.x(), non_homg_p1.y(), non_homg_p1.z());

  double dist2 = vgl_distance_to_closed_polygon (x_list, y_list, z_list, poly3d->size(),
                                                 non_homg_p2.x(), non_homg_p2.y(), non_homg_p2.z());

  // if it is close enough, find the intersection
  if ((dist1 < 2.0) && (dist2 < 2.0))
  {
    double point1_x, point1_y, point1_z,
      point2_x, point2_y, point2_z;
    unsigned
      int edge_index1 = vgl_closest_point_to_closed_polygon(point1_x, point1_y, point1_z,
                                                            x_list, y_list, z_list, poly3d->size(),
                                                            non_homg_p1.x(), non_homg_p1.y(), non_homg_p1.z());
    unsigned
      int edge_index2 = vgl_closest_point_to_closed_polygon(point2_x, point2_y, point2_z,
                                                            x_list, y_list, z_list, poly3d->size(),
                                                            non_homg_p2.x(), non_homg_p2.y(), non_homg_p2.z());

    if (edge_index1 == edge_index2) {
      std::cerr << "bwm_observer_cam::find_intersection_points() -- Both points are on the same edge!!!\n";
      return false;
    }

    l1 = vgl_point_3d<double> (x_list[edge_index1], y_list[edge_index1], z_list[edge_index1]);
    unsigned int next_index = edge_index1+1;
    if (next_index == poly3d->size())
      next_index = 0;
    l2 = vgl_point_3d<double> (x_list[next_index], y_list[next_index], z_list[next_index]);
    l3 = vgl_point_3d<double> (x_list[edge_index2], y_list[edge_index2], z_list[edge_index2]);
    next_index = edge_index2+1;
    if (edge_index2+1 == poly3d->size())
      next_index = 0;
    l4 = vgl_point_3d<double> (x_list[next_index], y_list[next_index], z_list[next_index]);
    point1 = vgl_point_3d<double>(point1_x, point1_y, point1_z);
    point2 = vgl_point_3d<double>(point2_x, point2_y, point2_z);
    return true;
  }
  return false;
}

//: Determines if a given line intersects with any faces of the object.
//  If true then returns the points in point1 and point2
bool bwm_observer_cam::intersect(bwm_observable_sptr obj, unsigned face_id,
                                 float x1, float y1, float x2, float y2)
{
  vgl_point_2d<double> image_point1(x1, y1);
  vgl_point_2d<double> image_point2(x2, y2);

  if (obj) {
    vsol_polygon_3d_sptr face = obj->extract_face(face_id);
    if (face != nullptr) {
      vgl_point_3d<double> l1, l2, l3, l4;
      vgl_point_3d<double> point1, point2;
      if (find_intersection_points(image_point1, image_point2, face, point1, l1, l2, point2, l3, l4)) {
        obj->divide_face(face_id, l1, l2, point1, l3, l4, point2);
        return true;
      }
    }
  }
  return false;
}

//: Determines if a given line intersects any faces of the mesh.
//  If intersects then returns the points in point1 and point2
bool bwm_observer_cam::intersect(float x1, float y1, float x2, float y2)
{
  std::map<bwm_observable_sptr, std::map<unsigned, bgui_vsol_soview2D* > >::iterator itr = objects_.begin();
  std::vector<bwm_observable_sptr> intersecting_obs;
  vgl_point_2d<double> image_point1(x1, y1);
  vgl_point_2d<double> image_point2(x2, y2);

  // method of intersecting is different depending on what type of camera we are using
  // TEST THIS!!!!!!!!!!!!!!
  //if (vpgl_proj_camera<double> *pro_cam = dynamic_cast<vpgl_proj_camera<double>*>(camera_))
  //{
  // first, check the intersection with the bounding box
  for (; itr != objects_.end(); itr++) {
    vgl_box_3d<double> bb;
    bwm_observable_sptr obs =(*itr).first;
    // bmsh3d_mesh* mesh = new bmsh3d_mesh(*(obs->get_object()));
    // detect_bounding_box(mesh, bb);
    bb = obs->bounding_box();
    vgl_point_3d<double> point1, point2;
    if (intersect_ray_and_box(bb, image_point1, point1) &&
        intersect_ray_and_box(bb,image_point2, point2)) {
      intersecting_obs.push_back(obs);
    }
  }
  //}

  // choose the polygon amongst the selected objects
  // select the one closest to the camera
  vgl_point_3d<double> cam_center;
  camera_center(cam_center);
#if 0 // commented out
  if (vpgl_proj_camera<double> *pro_cam = dynamic_cast<vpgl_proj_camera<double>*>(camera_)) {
    cam_center = pro_cam->camera_center();
  }
  else if (vpgl_rational_camera<double> *rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(camera_)) {
    // THIS IS JUST A PLACE HOLDER TILL WE FIGURE OUT SOMETHING BETTER...

    cam_center.set(rat_cam->offset(rat_cam->X_INDX),
                   rat_cam->offset(rat_cam->Y_INDX),
                   rat_cam->offset(rat_cam->Z_INDX));
  }
  else {
    //unsupported camera type
    std::cerr << "poly_cam_observer::intersect(): unsupported camera type!\n";
    return false;
  }
#endif // 0
  // choose the polygon amongst the selected objects
  // select the one closest to the camera
  int closest_face_id = 0;
  vgl_point_3d<double> seg_l1, seg_l2, seg_l3, seg_l4, inters_point1, inters_point2;
  double dist = 1e06;
  bool found=false;
  for (unsigned i=0; i<intersecting_obs.size(); i++) {
    bwm_observable_sptr obs = intersecting_obs[i];
    std::map<int, vsol_polygon_3d_sptr> faces = obs->extract_faces();
    std::map<int, vsol_polygon_3d_sptr>::iterator iter = faces.begin();

    while (iter != faces.end())
    {
      vgl_point_3d<double> l1, l2, l3, l4;
      vgl_point_3d<double> point1, point2;
      vsol_polygon_3d_sptr face = iter->second;
      if (this->find_intersection_points(image_point1, image_point2, face, point1, l1, l2, point2, l3, l4)) {
        std::cout << "found intersecting line:" << std::endl;
        found = true;
        double dist1 = vgl_distance(cam_center, vgl_point_3d<double> (point1));
        double dist2 = vgl_distance(cam_center, vgl_point_3d<double> (point2));
        double avg_dist = (dist1 + dist2)/2;
        if (avg_dist < dist) {
          dist = avg_dist;
          closest_face_id = iter->first;
          seg_l1 = l1;
          seg_l2 = l2;
          seg_l3 = l3;
          seg_l4 = l4;
          inters_point1 = point1;
          inters_point2 = point2;
        }
      }
      iter++;
    }
    if (found) {
      obs->divide_face(closest_face_id, seg_l1, seg_l2, inters_point1,
                       seg_l3, seg_l4, inters_point2);
      return true;
    }
  }
  return false;
}

//: return the 3d intersection point between the ray generated by a single image point and a face of the object
bool bwm_observer_cam::intersect(bwm_observable_sptr obj,
                                 float img_x, float img_y,
                                 unsigned face_id, vgl_point_3d<double> &pt3d)
{
  vgl_point_2d<double> img_point(img_x,img_y);
  if (obj) {
    vsol_polygon_3d_sptr face = obj->extract_face(face_id);
    if (face) {
      return find_intersection_point(img_point,face,pt3d);
    }
  }
  return false;
}

bool bwm_observer_cam::find_intersection_point(vgl_point_2d<double> img_point,
                                               vsol_polygon_3d_sptr poly3d,
                                               vgl_point_3d<double> &point3d)
{
  vgl_plane_3d<double> poly_plane = poly3d->plane(); //find_plane(poly3d);
  vgl_point_3d<double> p3d_homg;

  if (!intersect_ray_and_plane(img_point,poly_plane,p3d_homg)) {
    std::cout << "in bwm_observer_cam::find_intersection_point(.) -"
             << " intersect ray and plane failed\n";
    return false;
  }

  if (is_ideal(p3d_homg)) {
    std::cout << "intersection point is ideal!" <<std::endl;
  }
  vgl_point_3d<double> p3d(p3d_homg);

  double *x_list, *y_list, *z_list;
  bwm_algo::get_vertices_xyz(poly3d, &x_list, &y_list, &z_list);

  double dist = vgl_distance_to_closed_polygon(x_list,y_list,z_list,poly3d->size(),p3d.x(),p3d.y(),p3d.z());

#ifdef DEBUG
  std::cout << "dist = "<<dist<<std::endl;
#endif
  return !dist;
}

#if 0 // commented out
void bwm_observer_cam::connect_inner_face(bwm_observable* obj,
                                          unsigned face_id,
                                          vsol_polygon_2d_sptr poly2d)
{
  if (obj) {
    vsol_polygon_3d_sptr poly = obj->extract_face(face_id);
    if (poly != 0) {
      vgl_plane_3d<double> plane = poly->plane();
      vsol_polygon_3d_sptr poly3d;

      // back project the inner polygon to the plane of the outer polygon
      backproj_poly(poly2d, poly3d, plane);

      // attach the inner polygon to the outer one
      obj->attach_inner_face(face_id, poly3d);
    }
  }
}
#endif // 0

// The input point, pt, is supplied by the caller. The routine
// expects that the user has selected a face and a vertex on the face.
// A plane is constructed that is perpendicular to the face to be extruded,
// and passing through one of the edges of the face. The 3-d location of
// the picked point is determined by projecting it onto the perpendicular
// plane. The face is extruded to this position along the normal of
// the selected face.
//
void bwm_observer_cam::extrude_face(vsol_point_2d_sptr pt)
{
  // first get the selected objects
  //std::vector<vgui_soview*> select_list = this->get_selected_soviews();
  bgui_vsol_soview2D_polygon* poly = (bgui_vsol_soview2D_polygon*) this->get_selected_object(POLYGON_TYPE);
  if (!poly) {
    std::cerr << "Please select a face (only one) to extrude!\n";
    return;
  }

  // get selected vertex pair
  std::vector<vgui_soview2D*> vertices = this->get_selected_objects(VERTEX_TYPE);
  if (vertices.size()  != 2) {
    std::cerr << "Please select 2 vertices on the face to extrude!\n";
    return;
  }

  bwm_soview2D_vertex *v1, *v2;
  v1 = static_cast<bwm_soview2D_vertex*> (vertices[0]);
  v2 = static_cast<bwm_soview2D_vertex*> (vertices[1]);

  // make sure that these vertices belong to the face
  //if ((v1->obj() == poly) && (v2->obj() == poly))
  unsigned face_id;
  bwm_observable_sptr obs = this->find_object(poly->get_id(), face_id);

  float x1, y1, x2, y2;
  v1->get_centroid(&x1, &y1);
  v2->get_centroid(&x2, &y2);
  vsol_point_2d_sptr picked_v1 = new vsol_point_2d(x1,y1);
  vsol_point_2d_sptr picked_v2 = new vsol_point_2d(x2,y2);
  vsol_polygon_3d_sptr face = obs->extract_face(face_id);
  vsol_polygon_2d_sptr face2d;

  // find the backprojection of the selected vertex
  proj_poly(face, face2d);

  // find the index of the selected vertices, by searching for the
  // closest 3-d vertex to the backprojected 2-d vertex
  double min_dist1 = 1e23, min_dist2 = 1e23;
  unsigned index1 = (unsigned int)(-1), index2 = (unsigned int)(-1);
  std::cout << "\n-- Selected v1=" << picked_v1->get_p()
           << "\n-- Selected v2=" << picked_v2->get_p() << std::endl;
  for (unsigned i=0; i<face2d->size(); i++)
  {
    vgl_point_2d<double> pt = face2d->vertex(i)->get_p();

    double dist1 = (pt - picked_v1->get_p()).length();
    double dist2 = (pt - picked_v2->get_p()).length();
    std::cout << i << "-- Vertex" << pt << " dist=" << dist1 << '\n'
             << i << "-- Vertex" << pt << " dist=" << dist2 << std::endl;
    if (dist1 < min_dist1) {
      min_dist1 = dist1;
      index1 = i;
    }
    if (dist2 < min_dist2) {
      min_dist2 = dist2;
      index2 = i;
    }
  }
  if ((index1 == (unsigned int)(-1)) || (index2 == (unsigned int)(-1))) {
    std::cerr << "The vertices (one or both) cannot be found on the face\n";
    return;
  }
#if 0
  // see if the vertices are adjacent
  unsigned index = (index1 < index2) ? index1 : index2;
  unsigned next_index = (index == face->size()-1) ? 0 : index+1;
#endif
  if (index1 == index2) {
    std::cerr << "the edge vertices are the same vertex, something terribly wrong!\n";
    return;
  }
  else if (index1 < index2) {
    unsigned next_index = (index1 == 0) ? face->size()-1 : index1+1;
    if (next_index != index2) {
      std::cerr << "backprojected vertices are not adjacent\n";
      return;
    }
  }
  else {
    unsigned next_index = (index2 == 0) ? face->size()-1 : index2+1;
    if (next_index != index1) {
      std::cerr << "backprojected vertices are not adjacent\n";
      return;
    }
  }

  // selected vertices
  vgl_point_3d<double> p0(face->vertex(index1)->get_p());
  vgl_point_3d<double> p1(face->vertex(index2)->get_p());

  // a point above p0 along normal
  std::cout << "Face Normal-->" << face->normal() << std::endl;
#if 0
  vgl_point_3d<double> p2(face->vertex(index1)->get_p().x() +
                          face->normal().x(),
                          face->vertex(index1)->get_p().y() +
                          face->normal().y(),
                          face->vertex(index1)->get_p().z() +
                          face->normal().z());
#endif
  vgl_point_3d<double> p2(face->vertex(index1)->get_p().x(),
                          face->vertex(index1)->get_p().y(),
                          face->vertex(index1)->get_p().z()+1);

  std::cout << "p0-->" << p0 << '\n'
           << "p1-->" << p1 << '\n'
           << "p2-->" << p2 << std::endl;
  vgl_plane_3d<double> plane(p0, p2, p1);
#ifdef CAM_DEBUG
  std::cout << "projection plane normal " << plane.normal() << '\n';
#endif
  vsol_point_3d_sptr pt3d;
  backproj_point(pt, pt3d, plane);
  if (!pt3d)
  {
    std::cout << "Target point did not project successfully "
             <<"to building surface. Try again.\n";
    return;
  }

#ifdef CAM_DEBUG
  std::cout << "The back-projected point ("
           << pt3d->x() << ' ' << pt3d->y() << ' ' << pt3d->z() << ")\n";
#endif

  // use the z value of the backprojected point

#if 0
  // the face normal vector
  vgl_vector_3d<double> n1 = face->normal();
#ifdef CAM_DEBUG
  std::cout << "extrusion face normal " << n1 << '\n';
#endif
  // the vector pointing from the selected vertex to the 3-d target point
  vgl_vector_3d<double> n2 = pt3d->get_p() - v3d->get_p();
  double a = angle(n1, n2);
  double ninety_deg = vnl_math::pi/2.0;

#ifdef CAM_DEBUG
  std::cout << "angle " << a*vnl_math::deg_per_rad << '\n';
#endif

  //magnitude of the distance from the selected vertex to the target point
  double dist = (face->vertex(index)->get_p() - pt3d->get_p()).length();
  if (a > ninety_deg)
    dist *= -1;
#endif // 0

  //OK but what if the target point does not lie exactly on the
  //vector from the selected vertex to the ground. We should only
  //consider that the z coordinate of the selected point is important.
  //This method should be modified, perhaps define a "visible side"
  //construction plane.

  obs->extrude(face_id, pt3d->z());
}

// The input point, pt, is supplied by the caller. The routine
// expects that the user has selected a face and a vertex on the face.
// A plane is constructed that is perpendicular to the face to be extruded,
// and passing through one of the edges of the face. The 3-d location of
// the picked point is determined by projecting it onto the perpendicular
// plane. The face is extruded to this position along the normal of
// the selected face.
//
void bwm_observer_cam::extrude_face()
{
  // first get the selected objects
  //std::vector<vgui_soview*> select_list = this->get_selected_soviews();
  bgui_vsol_soview2D_polygon* poly = (bgui_vsol_soview2D_polygon*) this->get_selected_object(POLYGON_TYPE);
  if (!poly) {
    std::cerr << "Please select a face (only one) to extrude!\n";
    return;
  }

  // make sure that the polygon is in the z=d plane, do NOT extrude otherwise
  unsigned face_id;
  bwm_observable_sptr obs = this->find_object(poly->get_id(), face_id);
  if (!obs)
    return;

  vsol_polygon_3d_sptr face = obs->extract_face(face_id);
  vgl_vector_3d<double> up(0,0,1);
  vgl_vector_3d<double> down(0,0,-1);
  vgl_vector_3d<double> normal = face->normal();
  double angle_u = angle(normal, up);
  double angle_d = angle(normal, down);
  // check if the normal is either up or down
  if ((angle_u < 0.01) || (angle_d < 0.01)) {
    extrude_mode_ = true;
    extrude_obj_ = obs;
    obs->extrude(face_id);
  }
  else {
    this->deselect();
    std::cout << "The face is not extrudable, it should be parallel to the ground!" << std::endl;
  }
}

void bwm_observer_cam::divide_face(bwm_observable_sptr obs, unsigned face_id,
                                   float x1, float y1, float x2, float y2)
{
  intersect(obs, face_id, x1, y1, x2, y2);
}

void bwm_observer_cam::backproj_point(vsol_point_2d_sptr p2d, vsol_point_3d_sptr& p3d)
{
  backproj_point(p2d, p3d, this->proj_plane_);
}

void bwm_observer_cam::backproj_point(vsol_point_2d_sptr p2d,
                                      vsol_point_3d_sptr& p3d,
                                      vgl_plane_3d<double> proj_plane)
{
  vgl_point_3d<double> world_point;

  vgl_point_2d<double> image_point(p2d->x(), p2d->y());
#ifdef DEBUG
  std::cout << " Before projection---> x=" << p->x() << "  y=" << p->y() << std::endl;
#endif

  if (intersect_ray_and_plane(image_point,proj_plane,world_point))
  {
    double x = world_point.x();
    double y = world_point.y();
    double z = world_point.z();
    p3d = new vsol_point_3d (x, y, z);
  }
  else
    p3d = nullptr;
}

void bwm_observer_cam::backproj_poly(vsol_polygon_2d_sptr poly2d,
                                     vsol_polygon_3d_sptr& poly3d,
                                     double dist)
{
  // create a new plane which is the translation of the proj_plane
  // by a dist,
  // Remember the definition of a plane, which is
  // a*x + b*y + c*z + d = 0
  // where (a,b,c) is your normal, and d is negative distance to origin.

  // recalculating d based on the old d and the dist
  double d = proj_plane_.d();
  double a = proj_plane_.a();
  double b = proj_plane_.b();
  double c = proj_plane_.c();

  double trans_d = d - dist;
  vgl_plane_3d<double> trans_plane(a, b, c, 1*trans_d);
  backproj_poly(poly2d, poly3d, trans_plane);
}

bool bwm_observer_cam::backproj_poly(vsol_polygon_2d_sptr poly2d,
                                     vsol_polygon_3d_sptr& poly3d,
                                     vgl_plane_3d<double> proj_plane)
{
  std::vector<vsol_point_3d_sptr> projected_list;
  vgl_point_3d<double> world_point;

  for (unsigned i=0; i<poly2d->size(); i++) {
    vsol_point_2d_sptr p = poly2d->vertex(i);
    vgl_point_2d<double> image_point(p->x(), p->y());

    if (!this->intersect_ray_and_plane(image_point,proj_plane,world_point)) {
      std::cout << "Intersection failed in backproj_poly\n";
      return false;
    }
    double x = world_point.x();
    double y = world_point.y();
    double z = world_point.z();
    projected_list.push_back(new vsol_point_3d (x, y, z));
  }
  poly3d = new vsol_polygon_3d(projected_list);
  return true;
}

bool bwm_observer_cam::intersect_ray_and_box(vgl_box_3d<double> box,
                                             vgl_point_2d<double> img_point,
                                             vgl_point_3d <double> &point)
{
  // test the intersection of the line with 6 planes of the box
  std::vector<vgl_plane_3d<double> > planes;

  // make the box a thad bigger so that we accept closeby points
  box.expand_about_centroid (2);
  vgl_point_3d<double> min_point( box.min_point());
  vgl_point_3d<double> max_point( box.max_point());

  // create planes from plane parameteres a, b, c, d
  vgl_plane_3d<double> plane1(1, 0, 0, min_point.x());
  vgl_plane_3d<double> plane2(1, 0, 0, max_point.x());
  vgl_plane_3d<double> plane3(0, 1, 0, min_point.y());
  vgl_plane_3d<double> plane4(0, 1, 0, max_point.y());
  vgl_plane_3d<double> plane5(0, 0, 1, min_point.z());
  vgl_plane_3d<double> plane6(0, 0, 1, max_point.z());

  planes.push_back(plane1);
  planes.push_back(plane2);
  planes.push_back(plane3);
  planes.push_back(plane4);
  planes.push_back(plane5);
  planes.push_back(plane6);

  std::vector< vgl_point_3d <double> > intersection_points;

  for (unsigned i=0; i<planes.size(); i++) {
    vgl_point_3d<double> p;
    if (intersect_ray_and_plane(img_point,planes[i],p)) {
      std::cout << "ray-plane intersection failed\n" << std::endl;
      return false;
    }
    vgl_point_3d<double> ip(p);
    if (box.contains(ip))
      intersection_points.push_back(ip);
  }

  if (intersection_points.size() > 0)
    return true;
  else {
    std::cout << "Rays do not intersect the box\n" << std::endl;
    return false;
  }
}

void bwm_observer_cam::save(std::string path)
{
  // first get the selected objects
  std::vector<vgui_soview*> select_list = this->get_selected_soviews();

  // a polygon should be selected first
  if (select_list.size() == 1) {
    if (select_list[0]->type_name().compare("bgui_vsol_soview2D_polygon") == 0) {
      bgui_vsol_soview2D_polygon* poly;
      poly = static_cast<bgui_vsol_soview2D_polygon*> (select_list[0]);

      unsigned face_id;
      bwm_observable_sptr obs = this->find_object(poly->get_id(), face_id);
      if (obs)
        obs->save(path.c_str(), nullptr); // LVCS?? gamze
    }
  }
}

void bwm_observer_cam::save()
{
  vgui_dialog params("File Save");
  std::string ext, file, empty="";

  params.file ("Save...", ext, file);
  //bool use_lvcs = false;
  //params.checkbox("use lvcs",use_lvcs);
  if (!params.ask())
    return;

  if (file == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a filename (prefix)." );
    error.ask();
    return;
  }

  save(file);
}

void bwm_observer_cam::save_all()
{
  vgui_dialog params("File Save");
  std::string ext, file, empty="";

  params.file ("Save...", ext, file);
  //bool use_lvcs = false;
  //params.checkbox("use lvcs",use_lvcs);
  if (!params.ask())
    return;

  if (file == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a filename (prefix)." );
    error.ask();
    return;
  }

  save_all(file);
}

void bwm_observer_cam::save_all(std::string path)
{
  std::map<bwm_observable_sptr, std::map<unsigned, bgui_vsol_soview2D* > >::iterator it = objects_.begin();
  unsigned i = 0;
  while (it != objects_.end()) {
    bwm_observable_sptr o = it->first;

    std::stringstream strm;
    strm << std::fixed << i;
    std::string str(strm.str());

    std::string obj_path = path + "_o" + str;
    o->save(obj_path.c_str());
    it++;
  }
}

unsigned bwm_observer_cam::find_index_of_v(bwm_soview2D_vertex* vertex,
                                           bgui_vsol_soview2D_polygon* polygon)
{
  vsol_polygon_2d_sptr poly = polygon->sptr();
  double min_dist = 1e23;
  unsigned index = (unsigned int)(-1);
  for (unsigned i=0; i<poly->size(); i++) {
    float x, y;
    vertex->get_centroid(&x, &y);
    vgl_point_2d<double> v(x,y);
    double dist = (v - poly->vertex(i)->get_p()).length();

    if (dist < min_dist) {
      min_dist = dist;
      index = i;
    }
  }
  return index;
}

void bwm_observer_cam::scan_regions()
{
  // get the selected objects
  std::vector<vgui_soview*> select_list = this->get_selected_soviews();
  std::vector<vgl_polygon<double> > polygons;
  for (unsigned s=0; s<select_list.size();s++)
  {
    if (select_list[s]->type_name().compare("bgui_vsol_soview2D_polygon") == 0)
    {
      bgui_vsol_soview2D_polygon* poly;
      poly = static_cast<bgui_vsol_soview2D_polygon*> (select_list[s]);
      unsigned face_id;
      vgl_polygon<double> polygon(1);
      bwm_observable_sptr obs = this->find_object(poly->get_id(), face_id);
      if (obs) {
        std::vector<bwm_soview2D_vertex*> vertices = object_verts_[obs];
        if (obs->num_faces() == 1) {
          for (unsigned i=0; i<vertices.size(); i++) {
            float x, y;
            vertices[i]->get_centroid(&x, &y);
            polygon.push_back(x,y);
          }
        }
        else {
          std::vector<vgl_point_2d<double> > points;
          for (unsigned i=0; i<vertices.size(); i++) {
            float x, y;
            vertices[i]->get_centroid(&x, &y);
            vgl_point_2d<double> p(x,y);
            points.push_back(p);
          }
          vgl_convex_hull_2d<double> convex_hull(points);
          polygon = convex_hull.hull();
        }
        polygons.push_back(polygon);
      }
    }
  }
  bwm_image_processor::scan_regions(img_tab_, polygons);
}

void bwm_observer_cam::project_shadow()
{
  // get the selected objects
  shadow_mode_=true;
  std::vector<vgui_soview*> select_list = this->get_selected_soviews();
  std::vector<vgl_polygon<double> > polygons;
  for (unsigned i=0;i<shadow_line_segs_.size();i++)
    this->remove(shadow_line_segs_[i]);
  shadow_line_segs_.clear();
  for (unsigned s=0; s<select_list.size();s++)
  {
    if (select_list[s]->type_name().compare("bgui_vsol_soview2D_line_seg") == 0)
    {
      bgui_vsol_soview2D_line_seg* lineseg;
      lineseg = static_cast<bgui_vsol_soview2D_line_seg*> (select_list[s]);
      vsol_line_2d_sptr line_sptr=lineseg->sptr();

      double x0=line_sptr->p0()->x();
      double y0=line_sptr->p0()->y();

      double x1=line_sptr->p1()->x();
      double y1=line_sptr->p1()->y();

      double dist_min1=1e10; int index_min1=-1;
      double dist_min2=1e10; int index_min2=-1;
      unsigned edge_id;
      bwm_observable_sptr obs = this->find_object(lineseg->get_id(), edge_id);

      std::vector<vsol_point_3d_sptr> mesh_pts=obs->extract_vertices();
      for (unsigned i=0;i<mesh_pts.size();i++)
      {
        double u,v;
        camera_->project(mesh_pts[i]->x(),
                         mesh_pts[i]->y(),
                         mesh_pts[i]->z(),u,v);
        double dist1=(x0-u)*(x0-u)+(y0-v)*(y0-v);
        double dist2=(x1-u)*(x1-u)+(y1-v)*(y1-v);
        if (dist_min1>dist1)
        {
          dist_min1=dist1;
          index_min1=i;
        }
        if (dist_min2>dist2)
        {
          dist_min2=dist2;
          index_min2=i;
        }
      }
      int min_z_index=mesh_pts[index_min1]->z()<mesh_pts[index_min2]->z()?index_min1:index_min2;
      double len=mesh_pts[index_min1]->distance(mesh_pts[index_min2]);
      vsol_point_3d_sptr base_point= mesh_pts[min_z_index];
      vsol_point_3d_sptr pnew=new vsol_point_3d(base_point->x()-len*std::tan(sun_elev_angle_)*std::cos(sun_azim_angle_),
                                                base_point->y()-len*std::tan(sun_elev_angle_)*std::sin(sun_azim_angle_),
                                                base_point->z());

      double u0,v0,u1,v1;
      camera_->project(base_point->x(),
                       base_point->y(),
                       base_point->z(),u0,v0);
      camera_->project(pnew->x(),pnew->y(),pnew->z(),u1,v1);


      vsol_line_2d_sptr shadow_line= new vsol_line_2d(new vsol_point_2d(u0,v0),
                                                      new vsol_point_2d(u1,v1));

      shadow_line_segs_.push_back(this->add_vsol_line_2d(shadow_line));
    }
  }
  std::cout<<"(theta,phi) = ("<<sun_elev_angle_<<','<<sun_azim_angle_<<')'<<std::endl;
  this->post_redraw();
}

void bwm_observer_cam::make_object_selectable(bwm_observable_sptr obj, bool status)
{
  // find the soview objects belong to this object
  std::map<unsigned, bgui_vsol_soview2D* > objs = objects_[obj];
  std::vector<bwm_soview2D_vertex* > vertices = object_verts_[obj];

  std::map<unsigned, bgui_vsol_soview2D* >::iterator it = objs.begin();
  while (it != objs.end()) {
    it->second->set_selectable(status);
    it++;
  }

  for (unsigned i=0; i<vertices.size(); i++) {
    bwm_soview2D_vertex* v = vertices[i];
    v->set_selectable(status);
  }
}

//: if true, makes all the objects selectable, otherwise unselectable
void bwm_observer_cam::set_selection(bool status)
{
  std::map<bwm_observable_sptr, std::map<unsigned, bgui_vsol_soview2D* > >::iterator it = objects_.begin();
  while (it != objects_.end()) {
    make_object_selectable(it->first, status);
    it++;
  }
}

void bwm_observer_cam::show_geo_position()
{
  show_geo_position_ = !show_geo_position_;
  if (!show_geo_position_) {
    img_tab_->lock_linenum(false);
    bwm_observer_img::set_vgui_status_on(false);
    bwm_observer_img::lock_vgui_status(false);
    return;
  }
  img_tab_->lock_linenum(true);
  bwm_observer_img::lock_vgui_status(false);
  bwm_observer_img::set_vgui_status_on(true);
}


void  bwm_observer_cam::position_vertex(bool show_as_geo)
{
  std::vector<vgui_soview2D*> vertices = this->get_selected_objects(VERTEX_TYPE);
  if (vertices.size()  != 1) {
    std::cerr << "Please select exactly 1 vertex for vertex_position!\n";
    return;
  }
  bwm_soview2D_vertex* vert = static_cast<bwm_soview2D_vertex*>(vertices[0]);
  double u = vert->x, v = vert->y;
  std::map<unsigned, vsol_point_3d_sptr>::iterator vit;
  vit = vertex_3d_map_.find(vert->get_id());
  if (vit==vertex_3d_map_.end())
  {
    std::cerr << "Can't find selected vertex for geo_position!\n";
    return;
  }
  vsol_point_3d_sptr p3d = (*vit).second;
  double lx,ly,lz;
  bool lvcs_valid = bwm_world::instance()->lvcs_valid();
  if (lvcs_valid) {
    vpgl_lvcs lvcs;
    bwm_world::instance()->get_lvcs(lvcs);
    // convert point to local
    lvcs.global_to_local(p3d->x(), p3d->y(), p3d->z(), vpgl_lvcs::wgs84,lx, ly, lz);
  }
  else {
    lx = p3d->x(); ly = p3d->y(); lz = p3d->z();
  }
  vgui_text_tableau_sptr tt = img_tab_->text_tab();
  if (!tt) return;
  tt->clear();
  tt->set_colour(1.0, 1.0, 0.0);
  std::stringstream str;
  if (show_as_geo&&lvcs_valid)
    str << std::fixed << std::setprecision(6)<< '(' << p3d->y()
        << ' '  << p3d->x() << std::setprecision(2)<< ' ' << p3d->z() << ')' << std::ends;
  else
    str << std::fixed << std::setprecision(6)<< '(' << lx
        << ' '  << ly << std::setprecision(2)<< ' ' << lz << ')' << std::ends;
  float x = static_cast<float>(u+2.0);
  float y = static_cast<float>(v-2.0);
  tt->add(x, y, str.str());
  img_tab_->post_redraw();
}


//: Creates a terrain from a given boundary and the building bottoms and correspondence points
void bwm_observer_cam::create_terrain()
{
  //vsol_polygon_3d_sptr p3d;
  //vgl_plane_3d<double> plane = bwm_world::instance()->world_plane();
  //backproj_poly(boundary, p3d, plane);

  // add boundary points
  std::vector<vgl_point_2d<double> > points;
  std::vector<vgl_point_3d<double> > points_3d;

  // get the terrain boundary from observer manager
  bwm_observer_mgr::instance()->find_terrain_points(points_3d);
  for (unsigned i=0; i<points_3d.size(); i++) {
    vgl_point_3d<double> v = points_3d[i];
    points.push_back(vgl_point_2d<double>(v.x(), v.y()));
    //points_3d.push_back(vgl_point_3d<double>(points[i].x(), points[i].y(), p3d->vertex(i)->z()));
  }

  // add the building bases
  std::vector<bwm_observable_sptr> objects = bwm_world::instance()->objects();
  for (unsigned i=0; i<objects.size(); i++) {
    vsol_polygon_3d_sptr b = objects[i]->extract_bottom_face();
    for (unsigned j=0; j<b->size(); j++) {
      vsol_point_3d_sptr v = b->vertex(j);
      points.push_back(vgl_point_2d<double>(v->x(), v->y()));
      points_3d.push_back(v->get_p());
    }
  }

  // add the correspondence points
  std::vector<vgl_point_2d<double> >
    corr_points = bwm_observer_mgr::instance()->get_corr_points(this);
  for (unsigned i=0; i<corr_points.size(); i++) {
    vsol_point_3d_sptr p3d;
    vsol_point_2d_sptr p2d = new vsol_point_2d(corr_points[i]);
    backproj_point(p2d, p3d);
    points_3d.push_back(p3d->get_p());
  }

  // triangulate
  bwm_delaunay_tri tri;
  std::vector<vgl_point_3d<int> > v;
  int ntri;
  tri.triangulate(points_3d, v, ntri);
  std::cout << "Number of Triangles:" << ntri << std::endl;
  for (int i=0; i<ntri; i++) {
    std::cout << i << "==>" << v[i] << std::endl;
  }

  bwm_observable_mesh_sptr terrain = new bwm_observable_mesh(bwm_observable_mesh::BWM_MESH_TERRAIN);
  bwm_world::instance()->add(terrain);
  bwm_observer_mgr::instance()->attach(terrain);
  terrain->create_mesh_surface(points_3d, v);
}

void bwm_observer_cam::deselect()
{
  this->deselect_all();
  for (unsigned i=0; i<selected_soviews_.size(); i++) {
    selected_soviews_[i]->set_style(mesh_style_);
  }
  selected_soviews_.clear();
}

void bwm_observer_cam::project_meshes(std::vector<std::string> paths,
                                      vpgl_camera<double>* cam,
                                      std::vector<vgl_polygon<double> > &poly_2d_list)
{
  for (unsigned i=0; i<paths.size(); i++) {
    bwm_observable_mesh_sptr mesh = new bwm_observable_mesh();
    mesh->load_from(paths[i]);
    if (!mesh)
      std::cout << "There is a problem loading " << paths[i] << std::endl;
    else {
      std::map<int, vsol_polygon_3d_sptr > faces = mesh->extract_faces();
      for (unsigned f=0; f<faces.size(); f++) {
        vsol_polygon_3d_sptr face = faces[f];
        vgl_polygon<double> poly2d(1);
        for (unsigned i=0; i<face->size(); i++) {
          double u = 0,v = 0;
          cam->project(face->vertex(i)->x(), face->vertex(i)->y(), face->vertex(i)->z(),u,v);
          poly2d.push_back(u,v);
        }
        poly_2d_list.push_back(poly2d);
      }
    }
  }
}

void bwm_project_meshes(std::vector<std::string> paths,
                        vpgl_camera<double>* cam,
                        std::vector<vgl_polygon<double> > &poly_2d_list)
{
  for (unsigned i=0; i<paths.size(); i++) {
    bwm_observable_mesh_sptr mesh = new bwm_observable_mesh();
    mesh->load_from(paths[i]);
    if (!mesh)
      std::cout << "There is a problem loading " << paths[i] << std::endl;
    else {
      std::map<int, vsol_polygon_3d_sptr > faces = mesh->extract_faces();
      for (unsigned f=0; f<faces.size(); f++) {
        vsol_polygon_3d_sptr face = faces[f];
        vgl_polygon<double> poly2d(1);
        for (unsigned i=0; i<face->size(); i++) {
          double u = 0,v = 0;
          cam->project(face->vertex(i)->x(), face->vertex(i)->y(), face->vertex(i)->z(),u,v);
          poly2d.push_back(u,v);
        }
        poly_2d_list.push_back(poly2d);
      }
    }
  }
}

#if 0
void bwm_observer_cam::create_boxm_scene()
{
  vgui_soview2D* obj =  bwm_observer_img::get_selected_object(POLYGON_TYPE);
  if (obj) {
    // find the mesh containing this polygon?????
    unsigned face_id;
    bwm_observable_sptr mesh = find_object(obj->get_id(), face_id);
    vgl_box_3d<double> bb = mesh->bounding_box();
    vgl_point_3d<double> min = bb.min_point();
    double x_dim = bb.width();
    double y_dim = bb.height();
    double z_dim = bb.depth();

    double lx=min.x(), ly=min.y(), lz=min.z();
    vpgl_lvcs lvcs(ly, lx, lz,vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
    double local_x, local_y, local_z;
    lvcs.global_to_local(lx+x_dim,ly+y_dim,lz+z_dim,vpgl_lvcs::wgs84,local_x, local_y, local_z);

    // create the dialog
    vgui_dialog_extensions dialog("Enter the World Parameters");
    std::string ext, file, empty="";
    dialog.file ("File Path", ext, file);

    // appearence model
    std::vector<std::string> app_models;
    for (int i=0; i < int(BOXM_APM_UNKNOWN); i++)
      app_models.push_back(boxm_apm_types::app_model_strings[i]);

    dialog.line_break();
    int app_model_num;
    dialog.choice("Appearence Model",app_models,app_model_num);
    dialog.line_break();

    // multi-bin?
    int multi_bin=0;
    dialog.choice("Multiple Bins", "false","true",multi_bin);
    dialog.line_break();

    // LVCS
    dialog.message("LVCS from the box ");
    dialog.field("lon:", lx);
    dialog.field("lat:", ly);
    dialog.field("elev:", lz);
    dialog.line_break();

    dialog.message("Dimensions of the World ");
    dialog.field("X:", local_x);
    dialog.field("Y:", local_y);
    dialog.field("Z:", local_z);
    dialog.line_break();

    // Local origin
    double x=0, y=0, z=0;
    dialog.message("Local Origin ");
    dialog.field("X:", x);
    dialog.field("Y:", y);
    dialog.field("Z:", z);
    dialog.line_break();

    //number of blocks
    double bx=1, by=1, bz=1;
    dialog.message("Number of Blocks ");
    dialog.field("X:", bx);
    dialog.field("Y:", by);
    dialog.field("Z:", bz);
    dialog.line_break();

    // scene path
    std::string path;
    dialog.field("Scene Path:", path);

    // block prefix
    std::string prefix;
    dialog.field("Block Prefix:", prefix);

    int tree_max=1, tree_init=1;
    dialog.message("Tree Levels ");
    dialog.field("Max:", tree_max);
    dialog.field("Init:", tree_init);
    dialog.line_break();

    int load_all_blocks=0;
    dialog.choice("Load All Blocks", "false","true",load_all_blocks);

    int save_internal_nodes=0;
    dialog.choice("Save Internal Nodes", "false","true",save_internal_nodes);

    int save_platform_independent=0;
    dialog.choice("save_platform_independent", "false","true",save_platform_independent);

    if (!dialog.ask())
      return;

    ////////// Write the XML file
    std::ofstream os(file.c_str());
    vsl_basic_xml_element scene_elm("boxm_scene");
    scene_elm.x_write_open(os);

    vsl_basic_xml_element app_model("appearence_model");
    app_model.add_attribute("type", app_models[app_model_num]);
    app_model.x_write(os);

    vsl_basic_xml_element bin("multi_bin");
    bin.add_attribute("value", multi_bin);
    bin.x_write(os);

    vsl_basic_xml_element save_nodes("save_internal_nodes");
    save_nodes.add_attribute("value", save_internal_nodes);
    save_nodes.x_write(os);

    vsl_basic_xml_element sp("save_platform_independent");
    sp.add_attribute("value", save_platform_independent);
    sp.x_write(os);

    vsl_basic_xml_element load_blocks("load_all_blocks");
    load_blocks.add_attribute("value", load_all_blocks);
    load_blocks.x_write(os);

    lvcs.x_write(os, "lvcs");

    // convert the world dimensions to local coordinate system (meters)
    x_write(os, vgl_point_3d<double>(x,y,z), "local_origin");

    // compute the block dimensions from the world dimensions
    double xx=local_x/bx, yy=local_y/by, zz=local_z/bz;
    x_write(os, vgl_vector_3d<double>(xx,yy,zz), "block_dimensions"); // dimensions of matrix of blocks

    vsl_basic_xml_element blocks("blocks");
    blocks.add_attribute("x_dimension", bx);
    blocks.add_attribute("y_dimension", by);
    blocks.add_attribute("z_dimension", bz);
    blocks.x_write(os);

    vsl_basic_xml_element paths("scene_paths");
    paths.add_attribute("path", path);
    paths.add_attribute("block_prefix", prefix);
    paths.x_write(os);

    vsl_basic_xml_element levels("octree_level");
    levels.add_attribute("max", tree_max);
    levels.add_attribute("max", tree_init);
    levels.x_write(os);
    scene_elm.x_write_close(os);
    os.close();
  }
}

void bwm_observer_cam::load_boxm_scene()
{
  vgui_dialog params("Load Scene File");
  std::string ext, file, empty="";

  params.file ("Load...", ext, file);
  if (!params.ask())
    return;

  if (file == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a filename (prefix)." );
    error.ask();
    return;
  }

  boxm_scene_parser parser;
  boxm_scene_base scene_base;
  scene_base.load_scene(file, parser);
  //parse_config(parser);
  vgl_point_3d<double> world_min,world_max;
  vgl_vector_3d<unsigned> dims = parser.block_nums();
  vgl_point_3d<double> loc_origin =  parser.origin();
  vgl_vector_3d<double> block_dim = parser.block_dim();
  double x_size = dims.x()*block_dim.x();
  double y_size = dims.y()*block_dim.y();
  double z_size = dims.z()*block_dim.z();

  vpgl_lvcs lvcs;
  if (parser.lvcs(lvcs)) {
    // set lvcs
    bwm_world::instance()->set_lvcs(lvcs);
    std::cout << "defining lvcs with origin = <" << lvcs << std::endl;

    // create the world

    // find the origin in global coordinates
    double lon, lat, elev;
    lvcs.local_to_global(loc_origin.x(), loc_origin.y(), loc_origin.z(), vpgl_lvcs::wgs84, lon, lat, elev);
    world_min.set(lon,lat,elev);

    //find the max point of world in local coordinates
    double w_max_x = loc_origin.x() + x_size;
    double w_max_y = loc_origin.y() + y_size;
    double w_max_z = loc_origin.z() + z_size;
    lvcs.local_to_global(w_max_x, w_max_y, w_max_z, vpgl_lvcs::wgs84, lon, lat, elev);
    world_max.set(lon,lat,elev);
  }
  else {
    world_min = loc_origin;
    world_max.set(loc_origin.x()+x_size, loc_origin.y()+y_size,
                  loc_origin.z()+z_size);
  }
  //create the observable mesh
  vgl_box_3d<double> world(world_min, world_max);
  bwm_observable_mesh_sptr mesh = new bwm_observable_mesh(world);
  bwm_world::instance()->add(mesh);
  bwm_observer_mgr::instance()->attach(mesh);
}
#endif

// ==================   camera calibration methods ===============
void bwm_observer_cam::set_horizon()
{
  std::vector<vgui_soview*> select_list = this->get_selected_soviews();
  if (select_list.size() == 1)
  {
    if (select_list[0]->type_name().compare("bgui_vsol_soview2D_polyline") == 0) {
      bgui_vsol_soview2D_polyline* pline_ptr = static_cast<bgui_vsol_soview2D_polyline*>(select_list[0]);
      vsol_polyline_2d_sptr pline = pline_ptr->sptr();
      if (pline->size()!=2) {
        std::cout << "polyline must have exactly 2 vertices to be a horizon line\n";
        return;
      }
      vsol_point_2d_sptr p0 = pline->p0(), p1 = pline->p1();
      horizon_ = new vsol_line_2d(p0, p1);
      std::cout << "got the horizon\n";
    }
  }
}

void bwm_observer_cam::calibrate_cam_from_horizon()
{
  if (!horizon_) {
    std::cout << "Null horizon - can't calibrate the camera\n";
    return;
  }
  // get a, b, c of horizon
  vgl_line_segment_2d<double> vgl_line = horizon_->vgl_seg_2d();
  double a = vgl_line.a(), b = vgl_line.b(), c = vgl_line.c();
  // get the principal point
  double ppu = static_cast<double>(img_tab_->width())/2.0;
  double ppv = static_cast<double>(img_tab_->height())/2.0;
  vpgl_perspective_camera<double> cam =
    bpgl_camera_utils::camera_from_horizon(focal_length_,ppu, ppv,
                                           cam_height_, a, b, c);
  std::cout << "camera from horizon\n" << cam << '\n';
  if (camera_)
    delete camera_;
  camera_ = new vpgl_perspective_camera<double>(cam);
}

void bwm_observer_cam::toggle_cam_horizon()
{
  if (!camera_||(camera_->type_name() != "vpgl_perspective_camera"))
    return;
  if (horizon_soview_) {
    // in vgui_easy2D
    remove(horizon_soview_);
    horizon_soview_ = nullptr;
    post_redraw();
    return;
  }
  vpgl_perspective_camera<double>* cam =
    static_cast<vpgl_perspective_camera<double>*>(camera_);
  vgl_line_2d<double> horizon = bpgl_camera_utils::horizon(*cam);
  horizon_soview_=//in vgui_easy2D
    this->add_infinite_line(horizon.a(), horizon.b(), horizon.c());
  post_redraw();
}

//==========================  depth map methods =================
void bwm_observer_cam::set_ground_plane()
{
  // get the selected polygon

  std::vector<vgui_soview2D*> polys = get_selected_objects(POLYGON_TYPE);
  if (polys.size()!=1) {
    std::cout << "Not a single polygon selected - select a single polygon\n";
    return;
  }
  bgui_vsol_soview2D_polygon* p = reinterpret_cast<bgui_vsol_soview2D_polygon*>(polys[0]);
  scene_.set_ground_plane( p->sptr() );
}

void bwm_observer_cam::add_ground_plane(unsigned order, unsigned nlcd_id, std::string name)
{
  // get the selected polygon
  std::vector<vgui_soview2D*> polys = get_selected_objects(POLYGON_TYPE);
  if (polys.size()!=1) {
    std::cout << "Not a single polygon selected - select a single polygon\n";
    return;
  }
  bgui_vsol_soview2D_polygon* p = reinterpret_cast<bgui_vsol_soview2D_polygon*>(polys[0]);
  scene_.add_ground(p->sptr(), 0.0, 0.0, order, name, nlcd_id);
}

void bwm_observer_cam::set_sky()
{
  std::vector<vgui_soview2D*> polys = get_selected_objects(POLYGON_TYPE);
  if (polys.size()!=1) {
    std::cout << "Not a single polygon selected - select a single polygon\n";
    return;
  }
  bgui_vsol_soview2D_polygon* p = reinterpret_cast<bgui_vsol_soview2D_polygon*>(polys[0]);

  scene_.set_sky( p->sptr());
}

void bwm_observer_cam::add_sky(unsigned order, std::string name)
{
  std::vector<vgui_soview2D*> polys = get_selected_objects(POLYGON_TYPE);
  if (polys.size()!=1) {
    std::cout << "Not a single polygon selected - select a single polygon\n";
    return;
  }
  bgui_vsol_soview2D_polygon* p = reinterpret_cast<bgui_vsol_soview2D_polygon*>(polys[0]);
  scene_.add_sky(p->sptr(), order, name);
}

void bwm_observer_cam::add_vertical_depth_region(double min_depth,
                                                 double max_depth,
                                                 std::string name) {
  std::vector<vgui_soview2D*> polys = get_selected_objects(POLYGON_TYPE);
  if (polys.size()!=1) {
    std::cout << "Not a single polygon selected - select a single polygon\n";
    return;
  }
  bgui_vsol_soview2D_polygon* p = reinterpret_cast<bgui_vsol_soview2D_polygon*>(polys[0]);
  scene_.add_ortho_perp_region(p->sptr(), min_depth, max_depth, name);
}

void bwm_observer_cam::add_region(std::string name,
                                  double min_depth,
                                  double max_depth,
                                  unsigned order,
                                  unsigned orient,
                                  unsigned land_id,
                                  double height)
{
  std::vector<vgui_soview2D*> polys = get_selected_objects(POLYGON_TYPE);
  if (polys.size() != 1) {
    std::cout << "Not a single polygon selected - select a single polygon\n";
    return;
  }
  bgui_vsol_soview2D_polygon* p = reinterpret_cast<bgui_vsol_soview2D_polygon*>(polys[0]);
  vgl_vector_3d<double> np; // surface normal
  if (orient == 0)
    np.set(0.0, 0.0, 1.0);
  else
    np.set(1.0, 1.0, 0.0);

  switch (orient)
  {
   case 0:
    scene_.add_region(p->sptr(), np, min_depth, max_depth, name, depth_map_region::HORIZONTAL, order,land_id, height);
    break;
   case 1:
    scene_.add_region(p->sptr(), np, min_depth, max_depth, name, depth_map_region::FRONT_PARALLEL, order,land_id, height);
    break;
   case 2:
    scene_.add_region(p->sptr(), np, min_depth, max_depth, name, depth_map_region::SLANTED_RIGHT, order,land_id, height);
    break;
   case 3:
    scene_.add_region(p->sptr(), np, min_depth, max_depth, name, depth_map_region::SLANTED_LEFT, order,land_id, height);
    break;
   case 4:
    scene_.add_region(p->sptr(), np, min_depth, max_depth, name, depth_map_region::POROUS, order,land_id, height);
    break;
   case 5:
    scene_.add_region(p->sptr(), np, min_depth, max_depth, name, depth_map_region::NON_PLANAR, order,land_id, height);
    break;
   default:
    scene_.add_region(p->sptr(), np, min_depth, max_depth, name, depth_map_region::INFINT, order,land_id, height);
  }
}

std::vector<depth_map_region_sptr> bwm_observer_cam::scene_regions()
{
  // this scene_regions returns all possible depth_map_region previously define
  std::vector<depth_map_region_sptr> regions;
  std::vector<depth_map_region_sptr> sky_regions = scene_.sky();
  for (std::vector<depth_map_region_sptr>::iterator rit = sky_regions.begin(); rit != sky_regions.end(); ++rit)
    regions.push_back((*rit));

  std::vector<depth_map_region_sptr> grd_regions = scene_.ground_plane();
  for (std::vector<depth_map_region_sptr>::iterator rit = grd_regions.begin(); rit != grd_regions.end(); ++rit)
    regions.push_back((*rit));
  std::vector<depth_map_region_sptr> obj_regions = scene_.scene_regions();
  for (std::vector<depth_map_region_sptr>::iterator rit = obj_regions.begin(); rit != obj_regions.end(); ++rit)
    regions.push_back((*rit));
  return regions;
}

void bwm_observer_cam::set_ground_plane_max_depth()
{
  if (scene_.ground_plane().size()) {
    double depth = scene_.ground_plane()[0]->max_depth();
    if (depth != -1.0)
      scene_.set_ground_plane_max_depth(depth);
  }
}


void bwm_observer_cam::set_image_path(std::string const& ipath)
{
  scene_.set_image_path(ipath);
}

void bwm_observer_cam::save_depth_map_scene(std::string const& path)
{
  vsl_b_ofstream os(path.c_str());
  if (!os) {
    std::cout << "invalid binary stream for path " << path << std::endl;
    return;
  }
  scene_.b_write(os);
}

void bwm_observer_cam::display_depth_map_scene()
{
  for (unsigned i = 0; i < scene_.ground_plane().size(); i++) {
    depth_map_region_sptr gp = scene_.ground_plane()[i];
    if (gp) {
      bwm_observer_img::create_polygon(gp->region_2d());
      vsol_point_2d_sptr c = gp->centroid_2d();
      if (c) {
        float x = static_cast<float>(c->x());
        float y = static_cast<float>(c->y());
        img_tab_->text_tab()->add(x, y, gp->name());
      }
    }
  }
  for (unsigned i = 0; i < scene_.sky().size(); i++) {
    depth_map_region_sptr sky = scene_.sky()[i];
    if (sky) {
      bwm_observer_img::create_polygon(sky->region_2d());
      vsol_point_2d_sptr c = sky->centroid_2d();
      if (c) {
        float x = static_cast<float>(c->x());
        float y = static_cast<float>(c->y());
        std::stringstream tmp; tmp << i;
        img_tab_->text_tab()->add(x, y, "sky_" + tmp.str());
      }
    }
  }
  std::vector<depth_map_region_sptr> regions = scene_.scene_regions();
  for (std::vector<depth_map_region_sptr>::iterator rit = regions.begin();
       rit != regions.end(); ++rit) {
    bwm_observer_img::create_polygon((*rit)->region_2d());
    vsol_point_2d_sptr c = (*rit)->centroid_2d();
    if (c) {
      float x = static_cast<float>(c->x());
      float y = static_cast<float>(c->y());
      img_tab_->text_tab()->add(x, y, (*rit)->name());
    }
    post_redraw();
  }
}

void bwm_observer_cam::save_weight_params(std::string const& path)
{
  std::ofstream ofs(path.c_str());
  ofs << "Note: 1. for all objects, the summation of weight in the last column should be equal to 1"
      << " (average = " << 1/float(!scene_.sky().empty() + !scene_.ground_plane().empty() + scene_.scene_regions().size()) << ")\n"
      << "      2. for any object, the summation of all weights from different attributes should be equal to 1\n\n"
      << "name                      type      orientation      land_class      min_distance      relative_order       obj_weight\n";
  ofs.setf(std::ios::left);
  ofs.precision(4);
  ofs.setf(std::ios::showpoint);
  for (std::vector<volm_weight>::iterator vit = weights_.begin();  vit != weights_.end(); ++vit) {
    ofs << vit->w_name_ << "   ";
    if (vit->w_typ_ == "sky")
      ofs << "sky" << "   0.000   0.0000   1.0000   0.0000   " << vit->w_obj_;
    else if (vit->w_typ_ == "ground_plane")
      ofs << "ground_plane" << "   "
          << vit->w_ori_ << "   "
          << vit->w_lnd_ << "   "
          << vit->w_dst_ << "   "
          << "0.0   "
          << vit->w_obj_;
    else
      ofs << "object" << "   "
          << vit->w_ori_ << "   "
          << vit->w_lnd_ << "   "
          << vit->w_dst_ << "   "
          << vit->w_ord_ << "   "
          << vit->w_obj_;
    if (vit != (weights_.end()-1))  ofs << '\n';
  }
  ofs.close();
}
