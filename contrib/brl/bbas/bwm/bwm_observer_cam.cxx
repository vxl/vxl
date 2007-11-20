#include "bwm_observer_cam.h"
#include "bwm_observer_mgr.h"
#include "bwm_observable_mesh.h"
#include "algo/bwm_algo.h"
#include "algo/bwm_plane_fitting_lsf.h"
#include "bwm_tableau_mgr.h"

#include <vcl_iostream.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_closest_point.h> 
#include <vgl/vgl_distance.h>
#include <vgl/vgl_plane_3d.h>

#include <vil/vil_image_resource.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_3d.h>


//#include <vgui/vgui_soview2D.h>
#include <vgui/vgui_message.h>
#include <vgui/vgui_dialog.h>

//#define CAM_DEBUG   

void bwm_observer_cam::set_ground_plane(double x1, double y1, double x2, double y2)
{
  vgl_homg_point_3d<double> world_point1, world_point2;
  vgl_homg_plane_3d<double> xy_plane(0, 0, 1, 0);

  intersect_ray_and_plane(vgl_homg_point_2d<double> (x1, y1), xy_plane, world_point1);
  intersect_ray_and_plane(vgl_homg_point_2d<double> (x2, y2), xy_plane, world_point2);

  // define the third point in z direction which is the normal to the z=0 plane
  vgl_homg_point_3d<double> world_point3(world_point1.x(), world_point1.y(), 
    world_point1.z()+3.0, world_point1.w());

  proj_plane_ = vgl_homg_plane_3d<double> (world_point1, world_point2, world_point3);
}

void bwm_observer_cam::select_proj_plane()
{
  // first get the selected polygon
  vcl_vector<vgui_soview*> select_list = this->get_selected_soviews();

  if ((select_list.size() == 1) && 
    (select_list[0]->type_name().compare("bgui_vsol_soview2D_polygon") == 0)) {
    bgui_vsol_soview2D_polygon* polygon = static_cast<bgui_vsol_soview2D_polygon*> (select_list[0]);
    unsigned face_id;
    bwm_observable_sptr obs = find_object(polygon->get_id(), face_id);
    vgl_homg_plane_3d<double> plane = obs->get_plane(face_id);
    set_proj_plane(plane);
  } else
    vcl_cerr << "Select a face for projection plane" << vcl_endl;
}

void bwm_observer_cam::move_ground_plane( vgl_homg_plane_3d<double> master_plane,
                                          vsol_point_2d_sptr new_pt)
{
  // first get the selected vertex
  vcl_vector<vgui_soview*> select_list = this->get_selected_soviews();

  //vcl_cerr << "select_list.size == "<<select_list.size() << "  type_name == " << select_list[0]->type_name() << vcl_endl;
  if ((select_list.size() == 1) && 
    (select_list[0]->type_name().compare("bwm_soview2D_vertex") == 0)) {
    bwm_soview2D_vertex* circle = static_cast<bwm_soview2D_vertex*> (select_list[0]);

    //find the mesh this vertex belongs to
    unsigned int face_id;
    bwm_observable_sptr obs = find_object(circle->get_id(), face_id);

    float x, y;
    circle->get_centroid(&x, &y);
    vsol_point_2d_sptr old_pt = new vsol_point_2d(x,y);
    // create a new plane which is the translation of the proj_plane
    // by a dist,
    // Remember the definition of a plane, which is 
    // a*x + b*y + c*z + d = 0
    // where (a,b,c) is your normal, and d is negative distance to origin. 

    vcl_cerr << "old_pt = "<<old_pt->get_p() <<", new_pt = "<<new_pt->get_p() <<vcl_endl;

    // recalculating d based on the old d and the dist
    double d = master_plane.d();
    double a = master_plane.a();
    double b = master_plane.b();
    double c = master_plane.c();

    double orig_d = d;
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
    lm.diagnose_outcome(vcl_cout);
    
    vcl_ofstream fstream;
    vcl_cout << " minimization ended" << vcl_endl;
    vcl_cout << "X value after--> " << fx[0] << vcl_endl;
    vcl_cout << "Error=" << lm.get_end_error() << vcl_endl;
    vcl_cout << "num_iterations_" << lm.get_num_iterations()<< vcl_endl;


    // change the projection plane for the master observer and this one.
    // It may be also necessary to change all the observers projection plane, 
    // but I am not sure about it.. Will decide later. GT
    master_plane = lsf.plane();
    bwm_observer_mgr::BWM_MASTER_OBSERVER->set_proj_plane(master_plane);
    this->set_proj_plane(master_plane);
    vsol_point_3d_sptr new_pt3d;
    bwm_observer_mgr::BWM_MASTER_OBSERVER->backproj_point((new vsol_point_2d(master_img_pt)), new_pt3d, master_plane);

    // find the translation bewteen the beginning 3d point and ending, 
    // after moving the plane
    vgl_vector_3d<double> t(new_pt3d->x() - old_pt3d->x(),
      new_pt3d->y() - old_pt3d->y(),
      new_pt3d->z() - old_pt3d->z());
    bwm_observer_mgr::BWM_MASTER_OBSERVER->translate(t, obs);
    return;
  }
  vcl_cerr << "The original vertex to be moved is not selected!" << vcl_endl;
}

void bwm_observer_cam::proj_point(vgl_point_3d<double> world_pt, 
                                  vgl_point_2d<double> &image_pt)
{
  double u,v;
  camera_->project(world_pt.x(), world_pt.y(), world_pt.z(), u,v);
  image_pt.set(u,v);
  return;
}

void bwm_observer_cam::proj_poly(vsol_polygon_3d_sptr poly3d, 
                                  vsol_polygon_2d_sptr& poly2d)
{
  if(!poly3d)
    return;

  vcl_vector<vsol_point_2d_sptr> vertices;

  for (unsigned i=0; i<poly3d->size(); i++) {
    double u = 0,v = 0;
    //vcl_cout << "point " << *(poly3d->vertex(i)) << vcl_endl;
    camera_->project(poly3d->vertex(i)->x(), poly3d->vertex(i)->y(), poly3d->vertex(i)->z(),u,v); 
    vsol_point_2d_sptr p = new vsol_point_2d(u,v);
    vertices.push_back(p);
  }

  poly2d = new vsol_polygon_2d (vertices);
}

/*void bwm_observer_cam::proj_poly(vcl_vector<bmsh3d_vertex*> verts, 
                                 vcl_vector<vgl_point_2d<double> > &projections)
{
  for (unsigned v=0; v<verts.size(); v++) {
    bmsh3d_vertex* vert = (bmsh3d_vertex*)verts[v];
    double img_x = 0, img_y = 0;
    camera_->project(vert->pt().x(),vert->pt().y(),vert->pt().z(),img_x,img_y);
    vgl_point_2d<double> projection(img_x,img_y);
    projections.push_back(projection);
  }
}*/

void bwm_observer_cam::triangulate_meshes()
{
  vcl_map<bwm_observable_sptr, vcl_map<unsigned, bgui_vsol_soview2D_polygon*> >::iterator it;
  int obj_count = 0;
  for (it = objects_.begin(); it != objects_.end(); it++, obj_count++) {
    vcl_printf("triangulating mesh %d\n", obj_count);
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
  vcl_vector<vgui_soview*> select_list = this->get_selected_soviews();

  if (select_list.size() == 1) {
    if (select_list[0]->type_name().compare("bwm_soview2D_cross") == 0) {
    bwm_soview2D_cross* cross = static_cast<bwm_soview2D_cross*> (select_list[0]);
    vgl_point_2d<double> old_pt(cross->x, cross->y);
    update_corr_pt(new_pt->x(), new_pt->y());
    this->post_redraw();

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
  if(!sov) 
    {
      vcl_cerr << "In bwm_observer_cam::set_corr_to_vertex() -"
               << " no vertex selected\n";
        return;
    }
  bwm_observer_vgui::update_corr_pt(sov->x, sov->y);
  bwm_observer_img::post_redraw();
}

void bwm_observer_cam::world_pt_corr()
{
  // first get the selected cross
  vcl_vector<vgui_soview*> select_list = this->get_selected_soviews();
  if (select_list.size() == 1) {
    if (select_list[0]->type_name().compare("bwm_soview2D_vertex") == 0) {
      bwm_soview2D_vertex* vertex = static_cast<bwm_soview2D_vertex*> (select_list[0]);
      vsol_point_2d_sptr vertex_2d = new vsol_point_2d(vertex->x, vertex->y);
      vsol_point_3d_sptr vertex_3d;
      this->backproj_point(vertex_2d, vertex_3d);
      bwm_observer_mgr::instance()->save_world_pt_corr(vertex_3d->get_p());
    } else
      vcl_cerr << "Please select a vertex only (circle) " << vcl_endl;
  }
  vcl_cerr << "Please select only one object" << vcl_endl;
}


void bwm_observer_cam::scroll_to_point()
{
  static int ix = 0, iy = 0;
  vgui_dialog zoom("Zoom to Point");
  zoom.field ("xpos", ix);  
  zoom.field ("ypos", iy);  
  if (!zoom.ask())
    return;
  float x = static_cast<float>(ix), y = static_cast<float>(iy);
  this->move_to_point(x,y);
}

//********************** Protected Methods
//: returns the distance between a ray and a 3D polygon
bool bwm_observer_cam::find_intersection_points(vgl_homg_point_2d<double> const img_point1,
                                             vgl_homg_point_2d<double> const img_point2,
                                             vsol_polygon_3d_sptr poly3d,
                                             vgl_point_3d<double>& point1,
/* end points of the first polygon segment*/ vgl_point_3d<double>& l1, vgl_point_3d<double>& l2, 
                                             vgl_point_3d<double>& point2,
/* end points of the second polygon segment*/vgl_point_3d<double>& l3, vgl_point_3d<double>& l4) 
{
  
  vgl_homg_plane_3d<double> poly_plane = poly3d->plane();

  vgl_homg_point_3d<double> p1,p2;
  intersect_ray_and_plane(img_point1, poly_plane, p1);
  intersect_ray_and_plane(img_point2, poly_plane, p2);

  if (is_ideal(p1))
    vcl_cout << "p1 is ideal" << vcl_endl;
  if (is_ideal(p2))
    vcl_cout << "p2 is ideal" << vcl_endl;

  vgl_point_3d<double> non_homg_p1(p1);
  vgl_point_3d<double> non_homg_p2(p2);

  double *x_list, *y_list, *z_list;
  bwm_algo::get_vertices_xyz(poly3d, &x_list, &y_list, &z_list);
  
  double dist1 = vgl_distance_to_closed_polygon (x_list, y_list, z_list, poly3d->size(), 
    non_homg_p1.x(), non_homg_p1.y(), non_homg_p1.z());

  double dist2 = vgl_distance_to_closed_polygon (x_list, y_list, z_list, poly3d->size(), 
    non_homg_p2.x(), non_homg_p2.y(), non_homg_p2.z());

  // if it is close enough, find the intersection
  if ((dist1 < 2.0) && (dist2 < 2.0)) {
    double point1_x, point1_y, point1_z, 
      point2_x, point2_y, point2_z;

    int edge_index1 = vgl_closest_point_to_closed_polygon(point1_x, point1_y, point1_z, 
      x_list, y_list, z_list, poly3d->size(),
      non_homg_p1.x(), non_homg_p1.y(), non_homg_p1.z());
    
    int edge_index2 = vgl_closest_point_to_closed_polygon(point2_x, point2_y, point2_z, 
      x_list, y_list, z_list, poly3d->size(),
      non_homg_p2.x(), non_homg_p2.y(), non_homg_p2.z());

    if (edge_index1 == edge_index2) {
      vcl_cerr << "bwm_observer_cam::find_intersection_points() -- Both points are on the same edge!!!" << vcl_endl;
      return false;
    }

    l1 = vgl_point_3d<double> (x_list[edge_index1], y_list[edge_index1], z_list[edge_index1]);
    int next_index = edge_index1+1;
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

//: determines if a given line intersects with any faces of the object, if true
//  then returns the points in point1 and point2
bool bwm_observer_cam::intersect(bwm_observable_sptr obj, unsigned face_id, 
                                 float x1, float y1, float x2, float y2)
{
  vgl_homg_point_2d<double> image_point1(x1, y1);
  vgl_homg_point_2d<double> image_point2(x2, y2);

  if (obj) {
    vsol_polygon_3d_sptr face = obj->extract_face(face_id);
    if (face != 0) {
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

//: determines if a given line intersects any faces of the mesh, if intersects 
// then returns the points in point1 and point2
bool bwm_observer_cam::intersect(float x1, float y1, float x2, float y2)
{
  vcl_map<bwm_observable_sptr, vcl_map<unsigned, bgui_vsol_soview2D_polygon* > >::iterator itr = objects_.begin();
  vcl_vector<bwm_observable_sptr> intersecting_obs;
  vgl_homg_point_2d<double> image_point1(x1, y1);
  vgl_homg_point_2d<double> image_point2(x2, y2);
 
  // method of intersecting is different depending on what type of camera we are using
  // TEST THIS!!!!!!!!!!!!!!
  //if (vpgl_proj_camera<double> *pro_cam = dynamic_cast<vpgl_proj_camera<double>*>(camera_)) {

    // first, check the intersection with the bounding box
  for (; itr != objects_.end(); itr++) {
    vgl_box_3d<double> bb;
    bwm_observable_sptr obs =(*itr).first;
   // bmsh3d_mesh* mesh = new bmsh3d_mesh(*(obs->get_object()));
    //detect_bounding_box(mesh, bb);
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
  vgl_homg_point_3d<double> cam_center;
  camera_center(cam_center);
  /*if (vpgl_proj_camera<double> *pro_cam = dynamic_cast<vpgl_proj_camera<double>*>(camera_)) {
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
     vcl_cerr << "poly_cam_observer::intersect(): unsupported camera type!\n";
     return false;
   }*/
    // choose the polygon amongst the selected objects
    // select the one closest to the camera
    int closest_face_id = 0;
    vgl_point_3d<double> seg_l1, seg_l2, seg_l3, seg_l4, inters_point1, inters_point2;
    double dist = 1e06;
    bool found=false;
    for (unsigned i=0; i<intersecting_obs.size(); i++) {
      bwm_observable_sptr obs = intersecting_obs[i];
      vcl_map<int, vsol_polygon_3d_sptr> faces = obs->extract_faces();
      vcl_map<int, vsol_polygon_3d_sptr>::iterator iter = faces.begin();
   
      while (iter != faces.end()) {
        vgl_point_3d<double> l1, l2, l3, l4;
        vgl_point_3d<double> point1, point2;
        vsol_polygon_3d_sptr face = iter->second;
        if (this->find_intersection_points(image_point1, image_point2, face, point1, l1, l2, point2, l3, l4)) {
          vcl_cout << "found intersecting line:" << vcl_endl;
          found = true;
          double dist1 = vgl_distance(cam_center, vgl_homg_point_3d<double> (point1));
          double dist2 = vgl_distance(cam_center, vgl_homg_point_3d<double> (point2));
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
  vgl_homg_point_2d<double> img_point(img_x,img_y);
  if (obj) {
    vsol_polygon_3d_sptr face = obj->extract_face(face_id);
    if (face) {
      return (find_intersection_point(img_point,face,pt3d));
    }
  }
  return false;
}

bool bwm_observer_cam::find_intersection_point(vgl_homg_point_2d<double> img_point, 
                                                vsol_polygon_3d_sptr poly3d, 
                                                vgl_point_3d<double> &point3d)
{
  vgl_homg_plane_3d<double> poly_plane = poly3d->plane(); //find_plane(poly3d);
  vgl_homg_point_3d<double> p3d_homg;
  
  if(!intersect_ray_and_plane(img_point,poly_plane,p3d_homg))
    return false;

  if (is_ideal(p3d_homg)) {
    vcl_cout << "intersection point is ideal!" <<vcl_endl;
  }
  vgl_point_3d<double> p3d(p3d_homg);

  double *x_list, *y_list, *z_list;
  bwm_algo::get_vertices_xyz(poly3d, &x_list, &y_list, &z_list);
  
  double dist = vgl_distance_to_closed_polygon(x_list,y_list,z_list,poly3d->size(),p3d.x(),p3d.y(),p3d.z());

  //vcl_cout << "dist = "<<dist<<vcl_endl;
  if (dist)
    return false;
  
  return true;
}

/*void bwm_observer_cam::connect_inner_face(bwm_observable* obj, 
                                          unsigned face_id, 
                                          vsol_polygon_2d_sptr poly2d)
{
  if (obj) {
    vsol_polygon_3d_sptr poly = obj->extract_face(face_id);
    if (poly != 0) {
      vgl_homg_plane_3d<double> plane = poly->plane();
      vsol_polygon_3d_sptr poly3d;

      // back project the inner polygon to the plane of the outer polygon
      backproj_poly(poly2d, poly3d, plane);

      // attach the inner polygon to the outer one
      obj->attach_inner_face(face_id, poly3d);
    }
  }
}
*/
//
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
  vcl_vector<vgui_soview*> select_list = this->get_selected_soviews();

  // a polygon should be selected first
  if (select_list.size() == 2) {
    if (((select_list[0]->type_name().compare("bgui_vsol_soview2D_polygon") == 0) && (select_list[1]->type_name().compare("bwm_soview2D_vertex") == 0)) ||
    ((select_list[1]->type_name().compare("bgui_vsol_soview2D_polygon") == 0) && (select_list[0]->type_name().compare("bwm_soview2D_vertex") == 0)))
  {
    bgui_vsol_soview2D_polygon* poly;
    bwm_soview2D_vertex* circle;

    if (select_list[0]->type_name().compare("bgui_vsol_soview2D_polygon") == 0) {
      poly = static_cast<bgui_vsol_soview2D_polygon*> (select_list[0]);
      circle = static_cast<bwm_soview2D_vertex*> (select_list[1]);
    } else {
      poly = static_cast<bgui_vsol_soview2D_polygon*> (select_list[1]);
      circle = static_cast<bwm_soview2D_vertex*> (select_list[0]);
    }
    unsigned face_id;
    bwm_observable_sptr obs = this->find_object(poly->get_id(), face_id);

    float x, y;
    circle->get_centroid(&x, &y);
    vsol_point_2d_sptr picked_v = new vsol_point_2d(x,y);

    vsol_polygon_3d_sptr face = obs->extract_face(face_id);

    // find the backprojection of the selected vertex 
    vsol_point_3d_sptr v3d;
    backproj_point(picked_v, v3d, face->plane());
    
    // find the index of the selected vertex, by searching for the 
    // closest 3-d vertex to the backprojected 2-d vertex
    double min_dist = 1e23;
    unsigned index = -1;
    for (unsigned i=0; i<face->size(); i++) {
      double dist = (face->vertex(i)->get_p() - v3d->get_p()).length(); 
      if (dist < min_dist) {
        min_dist = dist;
        index = i;
      }
    }
    if (index == -1) {
      vcl_cerr << "Select a vertex on the face to be extruded" << vcl_endl;
      return;
    }

    // define a new plane, to backproject the extrusion point
    // get the next vertex index on the selected face
    unsigned next_index = (index == face->size()-1) ? 0 : index+1;
    // selected vertex
    vgl_homg_point_3d<double> p0(face->vertex(index)->get_p());
    // next vertex
    vgl_homg_point_3d<double> p1(face->vertex(next_index)->get_p());
    // a point above p0 along normal
    vgl_homg_point_3d<double> p2(face->vertex(index)->get_p().x() +
                                 face->normal().x(), 
                                 face->vertex(index)->get_p().y() +
                                 face->normal().y(),
                                 face->vertex(index)->get_p().z() +
                                 face->normal().z());
    vgl_homg_plane_3d<double> plane(p0, p1, p2);
#ifdef CAM_DEBUG    
    vcl_cout << "projection plane normal " << plane.normal() << '\n';
#endif
    vsol_point_3d_sptr pt3d;
    backproj_point(pt, pt3d, plane);
    if(!pt3d)
      {
        vcl_cout << "Target point did not project successfully "
                 <<"to building surface. Try again.\n";
        return;
      }
          
#ifdef CAM_DEBUG
    vcl_cout << "The back-projected point ("
             << pt3d->x() << ' ' << pt3d->y() << ' ' << pt3d->z() << ")\n";
#endif


    // the face normal vector
    vgl_vector_3d<double> n1 = face->normal();
#ifdef CAM_DEBUG   
    vcl_cout << "extrusion face normal " << n1 << '\n';
#endif
    // the vector pointing from the selected vertex to the 3-d target point
    vgl_vector_3d<double> n2 = pt3d->get_p() - v3d->get_p();
    double a = angle(n1, n2);
    double ninety_deg = vnl_math::pi/2.0;
    
#ifdef CAM_DEBUG   
    vcl_cout << "angle " << a*180/vnl_math::pi << '\n';
#endif

    //magnitude of the distance from the selected vertex to the target point
    double dist = (face->vertex(index)->get_p() - pt3d->get_p()).length();
    if (a > ninety_deg)
      dist *= -1;
    //OK but what if the target point does not lie exactly on the 
    //vector from the selected vertex to the ground. We should only
    //consider that the z coordinate of the selected point is important.
    //This method should be modified, perhaps define a "visible side"
    //construction plane. 

    obs->extrude(face_id, dist);
  } 
  } else 
    vcl_cerr << "Please select only one face to extrude!" << vcl_endl;

}

void bwm_observer_cam::divide_face(bwm_observable_sptr obs, unsigned face_id, 
                                   float x1, float y1, float x2, float y2)
{
  // first get the selected objects
 // vcl_vector<vgui_soview*> select_list = this->get_selected_soviews();

  // a polygon should be selected first
  //if ((select_list.size() == 1) && (select_list[0]->type_name().compare("bgui_vsol_soview2D_polygon") == 0)) {
  //  unsigned face_id;
 //   bwm_observable* obs = this->find_object(select_list[0]->get_id(), face_id);
    intersect(obs, face_id, x1, y1, x2, y2);
 // }
}

void bwm_observer_cam::backproj_point(vsol_point_2d_sptr p2d, vsol_point_3d_sptr& p3d)
{
  backproj_point(p2d, p3d, this->proj_plane_);
}

void bwm_observer_cam::backproj_point(vsol_point_2d_sptr p2d, 
                                      vsol_point_3d_sptr& p3d, 
                                      vgl_homg_plane_3d<double> proj_plane)
{
  vgl_homg_point_3d<double> world_point;

  vgl_homg_point_2d<double> image_point(p2d->x(), p2d->y());
  //vcl_cout << " Before projection---> x=" << p->x() << "  y=" << p->y() << vcl_endl;

  if(intersect_ray_and_plane(image_point,proj_plane,world_point))
    {
      double x = world_point.x()/world_point.w();
      double y = world_point.y()/world_point.w();
      double z = world_point.z()/world_point.w();  
      p3d = new vsol_point_3d (x, y, z);
    }
  else
    p3d = 0;
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
  vgl_homg_plane_3d<double> trans_plane(a, b, c, 1*trans_d);
  backproj_poly(poly2d, poly3d, trans_plane);
}

void bwm_observer_cam::backproj_poly(vsol_polygon_2d_sptr poly2d, 
                              vsol_polygon_3d_sptr& poly3d,
                              vgl_homg_plane_3d<double> proj_plane)
{

  vcl_vector<vsol_point_3d_sptr> projected_list;
  vgl_homg_point_3d<double> world_point;

  for(unsigned i=0; i<poly2d->size(); i++) {
    vsol_point_2d_sptr p = poly2d->vertex(i);
    vgl_homg_point_2d<double> image_point(p->x(), p->y());

    intersect_ray_and_plane(image_point,proj_plane,world_point);
    double x = world_point.x()/world_point.w();
    double y = world_point.y()/world_point.w();
    double z = world_point.z()/world_point.w();
    projected_list.push_back(new vsol_point_3d (x, y, z));
  }
  poly3d = new vsol_polygon_3d(projected_list);
}

bool bwm_observer_cam::intersect_ray_and_box(vgl_box_3d<double> box, 
                                           vgl_homg_point_2d<double> img_point,
                                           vgl_point_3d <double> &point)
{
  // test the intersection of the line with 6 planes of the box
  vcl_vector<vgl_homg_plane_3d<double> > planes;

  // make the box a thad bigger so that we accept closeby points
  box.expand_about_centroid (2);
  vgl_homg_point_3d<double> min_point( box.min_point());
  vgl_homg_point_3d<double> max_point( box.max_point());

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

  vcl_vector< vgl_point_3d <double> > intersection_points;

  for (unsigned i=0; i<planes.size(); i++) {
    vgl_homg_point_3d<double> p;
    intersect_ray_and_plane(img_point,planes[i],p);
    vgl_point_3d<double> ip(p);
    if (box.contains(ip))
      intersection_points.push_back(ip);  
  }
  
  if (intersection_points.size() > 0) 
    return true;
  else {
    vcl_cout << "Rays do not intersect the box" << vcl_endl;
    return false;
  }
}

void bwm_observer_cam::save()
{
  vgui_dialog params("File Save");
  vcl_string ext, file, empty="";

  params.file ("Save...", ext, file);  
  bool use_lvcs = false;
  //params.checkbox("use lvcs",use_lvcs);
  if (!params.ask())
    return;

  if (file == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a filename (prefix)." );
    error.ask();
    return;
  }

  vcl_map<bwm_observable_sptr, vcl_map<unsigned, bgui_vsol_soview2D_polygon* > >::iterator it = objects_.begin();
  while (it != objects_.end()) {
    bwm_observable_sptr o = it->first;
    o->save(file.data());

    it++;
  }
}
