#include <iostream>
#include <sstream>
#include "bwm_observer_rat_cam.h"
//:
// \file
#include <bwm/bwm_observable_textured_mesh.h>
#include <bwm/bwm_observable_textured_mesh_sptr.h>
#include <bwm/bwm_texture_map_generator.h>
#include <bwm/bwm_world.h>
#include <bwm/reg/bwm_reg_utils.h>
#include <bwm/reg/bwm_reg_processor.h>
#include <bwm/algo/bwm_utils.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#if 0
#include <vgl/vgl_box_2d.h>
#endif
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_digital_curve_3d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vgui/vgui_message.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>

#include <bgui/bgui_vsol_soview2D.h>

#include <vpgl/algo/vpgl_backproject.h>

#include <vul/vul_file.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bmsh3d/bmsh3d_textured_mesh_mc.h>
#include <vpgl/vpgl_local_rational_camera.h>

vpgl_camera<double>* bwm_observer_rat_cam::read_camera(std::string cam_path,
                                                       bool& local)
{
  local = true;
  //vpgl_rational_camera<double> *
  //rational camera may be local, therefore we check if it's local first
  vpgl_camera<double>* cam  = read_local_rational_camera<double>(cam_path);

  if(!cam){
    std::cout << "Rational cmaera isn't RPB ... trying TXT\n";
    cam = read_local_rational_camera_from_txt<double>(cam_path);
  }
  if ( !cam ) {
    std::cout << "Rational camera isn't local... trying global" << std::endl;
    cam = read_rational_camera<double>(cam_path);
    if(!cam)
      cam = read_rational_camera_from_txt<double>(cam_path);
    local = false;
  }
  if (!cam) {
    bwm_utils::show_error("[" + cam_path + "] is not a valid rational camera path");
    return nullptr;
  }
  return cam;
}
bwm_observer_rat_cam::bwm_observer_rat_cam(bgui_image_tableau_sptr img,
                                           std::string& name,
                                           std::string& image_path,
                                           std::string& cam_path,
                                           bool display_image_path)
    : bwm_observer_cam(img)
{
  img->show_image_path(display_image_path);
  // LOAD IMAGE
  vgui_range_map_params_sptr params;
  vil_image_resource_sptr img_res = bwm_utils::load_image(image_path, params);

  if (!img_res) {
    bwm_utils::show_error("Image [" + image_path + "] is NOT found");
    return;
  }

  img->set_image_resource(img_res, params);
  img->set_file_name(image_path);

  // LOAD CAMERA
  // check if the camera path is not empty, if it is NITF, the camera
  // info is in the image, not a separate file
  if (cam_path.size() == 0)
  {
    bwm_utils::show_error("Camera tableaus need a valid camera path!");
    return;
  }
  set_camera_path(cam_path);
  bool local = false;
  camera_ = this->read_camera(cam_path, local);
  bwm_observer_mgr::instance()->add(this);
  set_tab_name(name);
  set_camera_path(cam_path);
}


vpgl_rational_camera<double> bwm_observer_rat_cam::camera()
{
  //NEED TO HAVE NAME FOR SAFE CASTING!!! or USE RTTI FIXME JLM
  return *static_cast<vpgl_rational_camera<double>* >(camera_);
}

void bwm_observer_rat_cam::camera_center(vgl_point_3d<double> &center)
{
#if 0 // Dan says this is not right
  center.set(camera_->offset(camera_->X_INDX),
             camera_->offset(camera_->Y_INDX),
             camera_->offset(camera_->Z_INDX));
#endif // 0
  vgl_vector_3d<double> v = camera_direction();
  center.set(v.x(), v.y(), v.z());
}

vgl_vector_3d<double> bwm_observer_rat_cam::camera_direction()
{
  vpgl_rational_camera<double>* rat_cam = static_cast<vpgl_rational_camera<double> *> (camera_);

  // Assume that the camera is a satellite camera, and center is at infinity.
  // Calculate the direction based on intersection points of two planes near the ground

  double pix_offset_x = rat_cam->offset(rat_cam->U_INDX);
  double pix_offset_y = rat_cam->offset(rat_cam->V_INDX);
  double z_offset = rat_cam->offset(rat_cam->Z_INDX);

  // use center point of image
  vgl_point_2d<double>  img_pt(pix_offset_x,pix_offset_y);

  double zval1 = z_offset, zval2 = z_offset + 200;

  // initial guess for backprojection - just use center point of image
  vgl_point_3d<double> guess(rat_cam->offset(rat_cam->X_INDX),
                             rat_cam->offset(rat_cam->Y_INDX),
                             rat_cam->offset(rat_cam->Z_INDX));
  vgl_vector_3d<double> plane_normal(0.0,0.0,1.0);
  vgl_point_3d<double> plane_point1(rat_cam->offset(rat_cam->X_INDX),
                                    rat_cam->offset(rat_cam->Y_INDX),zval1);
  vgl_point_3d<double> plane_point2(rat_cam->offset(rat_cam->X_INDX),
                                    rat_cam->offset(rat_cam->Y_INDX),zval2);

  vgl_plane_3d<double> plane1(plane_normal,plane_point1);
  vgl_plane_3d<double> plane2(plane_normal,plane_point2);

  vgl_point_3d<double> p1,p2;

  if (!vpgl_backproject::bproj_plane(*rat_cam, img_pt, plane1, plane_point1, p1)) {
    std::cerr << "Error: vpgl_backproject::broj_plane() failed.\n";
  }

  if (!vpgl_backproject::bproj_plane(*rat_cam, img_pt, plane2, plane_point2, p2)) {
    std::cerr << "Error: vpgl_backproject::broj_plane() failed.\n";
  }

  // convert p1 and p2 to lvcs
  double x,y,z;
  vpgl_lvcs lvcs;
  if (bwm_world::instance()->get_lvcs(lvcs)) {
    lvcs.global_to_local(p1.x(),p1.y(),p1.z(),vpgl_lvcs::wgs84,x,y,z,vpgl_lvcs::DEG,vpgl_lvcs::METERS);
    p1.set(x,y,z);
    lvcs.global_to_local(p2.x(),p2.y(),p2.z(),vpgl_lvcs::wgs84,x,y,z,vpgl_lvcs::DEG,vpgl_lvcs::METERS);
    p2.set(x,y,z);
  }

  // find vector defined by p1 and p2
  vgl_vector_3d<double> direction(p2.x() - p1.x(), p2.y() - p1.y(), p2.z() - p1.z());
  direction = direction / direction.length();
  return direction;
}

void bwm_observer_rat_cam::center_pos()
{
  vgui_dialog params ("Geographic Coordinates");
  double lat=41.830939, lon=-71.394178, elev=0;

  if (viewer_ == nullptr) {
    std::cerr << "viewer2D tableau is not found in the parents\n";
    return;
  }

  float zoom_factor = viewer_->zoom_factor;

  params.field ("Latitude..", lat);
  params.field ("Longitude..", lon);
  params.field ("Elevation..", elev);
  params.field ("Zoom Factor..", zoom_factor);
  if (!params.ask())
    return;

  // check the valid range..
  if ((lat < -180.0) || (lat > 180.0) || (lon < -180.0) || (lon > 180.0)) {
    vgui_dialog error ("Error");
    error.message ("Please specify valid geographic coordinates in the range [-180, 180]" );
    error.ask();
    return;
  }

  vgl_point_3d<double> world_pt(lon, lat, elev);
  vgl_point_2d<double> image_pt;

  // find the image point corresponds to this (lat, lon, elev);
  proj_point(world_pt, image_pt);

  float x, y; // viewport coordinates
  x = image_pt.x();
  y = image_pt.y();
#if 0
  vgui_projection_inspector p_insp;
  vgl_box_2d<double> bb(p_insp.x1, p_insp.x2, p_insp.y1, p_insp.y2);
  viewer_->token.offsetX = (bb.width()/2.0 - x);
  viewer_->token.offsetY = (bb.height()/2.0 - y);
#endif
  bwm_observer_img::move_to_point(x, y);
  this->add_circle(x, y, 2);
  viewer_->post_redraw();
}

bool bwm_observer_rat_cam::shift_camera(double dx, double dy)
{
  double u_off,v_off;
  vpgl_rational_camera<double>* rat_cam = static_cast<vpgl_rational_camera<double> *> (camera_);
  rat_cam->image_offset(u_off,v_off);
  rat_cam->set_image_offset(u_off + dx, v_off + dy);
  cam_adjusted_ = true;

#if 0 // take this to the camera tableau
  std::map<bwm_observable_sptr, std::vector<bgui_vsol_soview2D_polygon* > >::iterator objit;
  for (objit = objects_.begin(); objit != objects_.end(); objit++) {
    vgui_message msg;
    msg.from = objit->first;
    msg.data = new std::string("update");
    update(msg);
  }
#endif // 0
  return true;
}

bool bwm_observer_rat_cam::intersect_ray_and_plane(vgl_point_2d<double> img_point,
                                                   vgl_plane_3d<double> plane,
                                                   vgl_point_3d<double> &world_point)
{
  vpgl_rational_camera<double>* rat_cam = static_cast<vpgl_rational_camera<double> *> (camera_);
  vgl_point_2d<double>  p2d(img_point.x(),img_point.y());

  // initial guess for backprojection - just use center point of image
  vgl_point_3d<double> guess(rat_cam->offset(rat_cam->X_INDX),
                             rat_cam->offset(rat_cam->Y_INDX),
                             rat_cam->offset(rat_cam->Z_INDX));
  vgl_plane_3d<double> plane_nonhomg(plane);
  vgl_point_3d<double> p;

  if (!vpgl_backproject::bproj_plane(*rat_cam, p2d, plane_nonhomg, guess, p)) {
    std::cout << "vpgl_backproject::broj_plane() failed.\n";
    return false;
  }

  world_point.set(p.x(),p.y(),p.z());
  return true;
}


void bwm_observer_rat_cam::set_lvcs_at_selected_vertex()
{
  vsol_point_3d_sptr sv = selected_vertex();
  if (!sv)
    return;
  vpgl_lvcs lvcs(sv->y(),sv->x(),sv->z(), vpgl_lvcs::wgs84,vpgl_lvcs::DEG,vpgl_lvcs::METERS);
  bwm_world::instance()->set_lvcs(lvcs);
  std::cout << "defining lvcs with origin = <" << sv->x() << ", "<<
    sv->y() <<", "<< sv->z() << '>' <<std::endl;
}

//: Define a local vertical coordinate system by choosing a single point as the origin.
//  Elevation is set to the height of the current projection plane.
void bwm_observer_rat_cam::define_lvcs(float x1, float y1)
{
  vsol_point_2d_sptr img_point = new vsol_point_2d(x1,y1);
  std::vector<vsol_point_2d_sptr> points2d;
  std::vector<vsol_point_3d_sptr> points3d;
  // push img_point 3x to create degenerate polygon
  for (int i=0;i<3;i++) {
    points2d.push_back(img_point);
    points3d.push_back(new vsol_point_3d(0,0,0));
  }
  vsol_polygon_2d_sptr origin_poly2d = new vsol_polygon_2d(points2d);
  vsol_polygon_3d_sptr origin_poly3d = new vsol_polygon_3d(points3d);

  backproj_poly(origin_poly2d,origin_poly3d);

  // note constructor takes lat, long (as opposed to long, lat) so switch x and y
  vpgl_lvcs lvcs(origin_poly3d->vertex(0)->y(),
                 origin_poly3d->vertex(0)->x(),
                 origin_poly3d->vertex(0)->z(),
                 vpgl_lvcs::wgs84,vpgl_lvcs::DEG,vpgl_lvcs::METERS);
  std::cout << "defining lvcs with origin = <"
           << origin_poly3d->vertex(0)->x() << ", "
           << origin_poly3d->vertex(0)->y() << ", "
           << origin_poly3d->vertex(0)->z() << '>' << std::endl;
  bwm_world::instance()->set_lvcs(lvcs);
}

void bwm_observer_rat_cam::adjust_camera_offset(vsol_point_2d_sptr img_point)
{
#if 0 // make sure lvcs is defined
  if (!lvcs_) {
    std::cerr << "error: no lvcs defined!\n";
    return;
  }
#endif // 0

  vpgl_lvcs lvcs;
  if (bwm_world::instance()->get_lvcs(lvcs))
  {
    // get projection of lvcs origin
    double lat,lon,elev;
    lvcs.local_to_global(0, 0, 0,vpgl_lvcs::wgs84, lon ,lat ,elev,
                         vpgl_lvcs::DEG,vpgl_lvcs::METERS);
    vgl_point_3d<double> world_pt(lon,lat,elev);
    vgl_point_2d<double> image_pt;
    proj_point(world_pt,image_pt);

    // shift camera translation to line up points
    if (shift_camera(img_point->x() - image_pt.x(),
      img_point->y() - image_pt.y())) {
      std::cout << "shifted right camera offset by [" <<
        img_point->x() - image_pt.x() <<", " <<
        image_pt.y() - image_pt.y() <<"]\n";
      cam_adjusted_ = true;
    }
    else {
      std::cerr << " error shifting camera offset\n";
    }
  }
}

void bwm_observer_rat_cam::save_selected()
{
  vgui_dialog params("File Save");
  std::string ext, file, empty="";

  params.file ("Save...", ext, file);
  bool use_lvcs = false;
  params.checkbox("use lvcs",use_lvcs);

  if (!params.ask())
    return;

  if (file == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a filename (prefix)." );
    error.ask();
    return;
  }

  vpgl_lvcs lvcs;
  if (bwm_world::instance()->get_lvcs(lvcs)) {
    unsigned face_id;
    bwm_observable_sptr obj = selected_face(face_id);
    if (obj)
     obj->save(file.data(),&lvcs);
  }
}

#if 0
void bwm_observer_rat_cam::save_all()
{
  vgui_dialog params("File Save");
  std::string ext, list_name, empty="";
  bool use_lvcs = false;

  params.file ("Filename...", ext, list_name);
  params.checkbox("Use LVCS", use_lvcs);

  if (!params.ask())
    return;

  if (list_name == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a filename." );
    error.ask();
    return;
  }

  std::string directory_name = vul_file::dirname(list_name);

  std::ofstream list_out(list_name.data());
  if (!list_out.good()) {
    std::cerr << "error opening file "<< list_name <<'\n';
    return;
  }

  int mesh_idx = 0;
  std::map<bwm_observable_sptr, std::map<unsigned, bgui_vsol_soview2D_polygon* > >::iterator it = objects_.begin();
  while (it != objects_.end()) {
    std::ostringstream meshname;
    std::ostringstream fullpath;
    meshname << "obj" << mesh_idx <<".ply2";
    fullpath << directory_name << '/' << meshname.str();

    list_out << meshname.str() << std::endl;
    bwm_observable_sptr o = it->first;
    if (use_lvcs) {
      o->save(fullpath.str().data(),lvcs_);
    }
    else {
      o->save(fullpath.str().data());
    }
    it++;
    mesh_idx++;
  }
}
#endif // 0

#if 0
void bwm_observer_rat_cam::save_gml()
{
  if (!lvcs_) {
    std::cerr << "Error: lvcs not defined.\n";
    return;
  }

  vgui_dialog params("File Save");
  std::string ext, gml_filename, empty="";
  std::string model_name;

  params.field("model name", model_name);

  params.file("Save...", ext, gml_filename);
  if (!params.ask())
    return;

  if (gml_filename == "") {
    std::cerr << "Error: no filename selected.\n";
    return;
  }

  FILE* fp;
  if ((fp = std::fopen(gml_filename.c_str(), "w")) == NULL) {
    std::fprintf (stderr, "Can't open xml file %s to write.\n", gml_filename.c_str());
    return;
  }

  std::fprintf (fp, "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\n");
  std::fprintf (fp, "<CityModel xmlns=\"http://www.citygml.org/citygml/1/0/0\" xmlns:gml=\"http://www.opengis.net/gml\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.citygml.org/citygml/1/0/0 http://www.citygml.org/citygml/1/0/0/CityGML.xsd\">\n");
  std::fprintf (fp, "<gml:description>%s</gml:description>\n",model_name.c_str());
  std::fprintf (fp, "<gml:name>%s</gml:name>\n",model_name.c_str());

  std::map<bwm_observable_sptr, std::map<unsigned, bgui_vsol_soview2D_polygon* > >::iterator it;
  int obj_count = 0;
  for (it = objects_.begin(); it != objects_.end(); it++, obj_count++) {
    bwm_observable_sptr obj = it->first;
    if (obj->type_name().compare("bwm_observable_textured_mesh") == 0) {
      bwm_observable_textured_mesh* tm_obj = static_cast<bwm_observable_textured_mesh*>(obj.as_pointer());
      tm_obj->save_gml(fp, obj_count, lvcs_);
      std::fprintf (fp, "   </Building>");
      std::fprintf (fp, "  </cityObjectMember>");
    }
  }
  std::fprintf (fp, " </CityModel>");
  std::fclose (fp);
}
#endif // 0

#if 0
void bwm_observer_rat_cam::save_kml()
{
  vgui_dialog params("File Save");
  std::string ext, kml_filename, empty="";
  std::string model_name;
  double ground_height = 0.0;
  double x_offset = 0.0;
  double y_offset = 0.0;

  params.field("model name",model_name);
  params.field("ground height",ground_height);
  params.field("x offset",x_offset);
  params.field("y offset",y_offset);

  params.file("Save...",ext,kml_filename);
  if (!params.ask())
    return;

  if (kml_filename == "") {
    std::cerr << "Error: no filename selected.\n";
    return;
  }

  FILE* fp;
  if ((fp = std::fopen(kml_filename.c_str(), "w")) == NULL) {
    std::fprintf (stderr, "Can't open xml file %s to write.\n", kml_filename.c_str());
    return;
  }

  std::fprintf (fp, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
  std::fprintf (fp, "<kml xmlns=\"http://earth.google.com/kml/2.1\">\n");
  std::fprintf (fp, "<Document>\n");
  std::fprintf (fp, "  <name>%s</name>\n",vul_file::strip_directory(kml_filename.c_str()));
  std::fprintf (fp, "  <open>1</open>\n");
  std::fprintf (fp, "  <Placemark>\n");
  std::fprintf (fp, "    <name>%s</name>\n",model_name.c_str());
  std::fprintf (fp, "    <visibility>1</visibility>\n");

  std::fprintf (fp, "    <MultiGeometry>\n");

  std::map<bwm_observable_sptr, std::map<unsigned, bgui_vsol_soview2D_polygon* > >::iterator it;
  int obj_count = 0;
  for (it = objects_.begin(); it != objects_.end(); it++, obj_count++) {
    bwm_observable_sptr obj = it->first;
    if (obj->type_name().compare("bwm_observable_textured_mesh") == 0) {
      bwm_observable_textured_mesh* tm_obj = static_cast<bwm_observable_textured_mesh*>(obj.as_pointer());
      tm_obj->save_kml(fp, obj_count, lvcs_, ground_height, x_offset, y_offset);
    }
  }

  std::fprintf(fp, "    </MultiGeometry>\n");
  std::fprintf(fp, "  </Placemark>\n");
  std::fprintf(fp, "</Document>\n");
  std::fprintf(fp, "</kml>\n");

  std::fclose (fp);
}
#endif // 0

#if 0
void bwm_observer_rat_cam::save_x3d()
{
  if (lvcs_ != 0) {
    std::cerr << "Error: lvcs not defined.\n";
    return;
  }

  vgui_dialog params("File Save");
  std::string ext, x3d_filename, empty="";

  params.file("Save...",ext,x3d_filename);
  if (!params.ask())
    return;

  if (x3d_filename == "") {
    std::cerr << "Error: no filename selected.\n";
    return;
  }

  FILE* fp;
  if ((fp = std::fopen(x3d_filename.c_str(), "w")) == NULL) {
    std::fprintf (stderr, "Can't open x3d file %s to write.\n", x3d_filename.c_str());
    return;
  }

  std::fprintf(fp, "#VRML V2.0 utf8\n");
  std::fprintf(fp, "PROFILE Immersive\n\n");

  std::map<bwm_observable_sptr, std::map<unsigned, bgui_vsol_soview2D_polygon*> >::iterator it;
  int obj_count = 0;
  for (it = objects_.begin(); it != objects_.end(); it++, obj_count++) {
    bwm_observable_sptr obj = it->first;
    if (obj->type_name().compare("bwm_observable_textured_mesh") == 0) {
      bwm_observable_textured_mesh* tm_obj = static_cast<bwm_observable_textured_mesh*> (obj.as_pointer());
      tm_obj->save_x3d(fp, lvcs_);
      std::fprintf(fp, "      ]\n\n");
      std::fprintf(fp, "      solid TRUE\n");
      std::fprintf(fp, "      convex FALSE\n");
      std::fprintf(fp, "      creaseAngle 0\n");
      std::fprintf(fp, "    }\n");
      std::fprintf(fp, "  }\n");
      std::fprintf(fp, "}\n\n\n");
    }
  }

  std::fclose(fp);
  return;
}
#endif // 0

#if 0
void bwm_observer_rat_cam::save_kml_collada()
{
  if (!lvcs_) {
    std::cerr << "Error: lvcs not defined.\n";
    return;
  }
  double origin_lat = 0,origin_lon = 0, origin_elev = 0;
  lvcs_->get_origin(origin_lat,origin_lon,origin_elev);

  vgui_dialog params("File Save");
  std::string ext, kmz_dir, empty="";

  // guess at ground height = lowest vertex
  double minz = 1e6;
  std::map<bwm_observable_sptr, std::map<unsigned, bgui_vsol_soview2D_polygon* > >::iterator it;
 // std::map<int, dbmsh3d_vertex*>::iterator vit;
  for (it = objects_.begin(); it != objects_.end(); it++) {
    bwm_observable_sptr obj = it->first;
    obj->global_to_local(lvcs_, minz);
  }

  double ground_height = minz;
  double lat_offset = 0.0;
  double lon_offset = 0.0;
  std::string model_name;

  params.field("model name",model_name);
  params.field("ground height",ground_height);
  params.field("latitude offset",lat_offset);
  params.field("longitude offset",lon_offset);

  params.file("Save...",ext,kmz_dir);
  if (!params.ask())
    return;

  if (kmz_dir == "") {
    std::cerr << "Error: no filename selected.\n";
    return;
  }

  if (!vul_file::is_directory(kmz_dir)) {
    std::cerr << "Error: Select a directory name.\n";
    return;
  }

  std::ostringstream dae_fname;
  dae_fname << kmz_dir << "/models/mesh.dae";

  FILE* dae_fp;
  if ((dae_fp = std::fopen(dae_fname.str().data(), "w")) == NULL) {
    std::fprintf (stderr, "Can't open .dae file %s to write.\n", dae_fname.str().data());
    return;
  }

  std::vector<std::string> image_names;
  std::vector<std::string> image_fnames;
  std::vector<std::string> material_ids;
  std::vector<std::string> material_names;
  std::vector<std::string> effect_ids;
  std::vector<std::string> surface_ids;
  std::vector<std::string> image_sampler_ids;
  std::vector<std::string> geometry_ids;
  std::vector<std::string> geometry_position_ids;
  std::vector<std::string> geometry_position_array_ids;
  std::vector<std::string> geometry_uv_ids;
  std::vector<std::string> geometry_uv_array_ids;
  std::vector<std::string> geometry_vertex_ids;
  std::vector<std::string> mesh_ids;

  int nobjects = 0;
  unsigned min_faces = 3;

  for (it = objects_.begin(); it != objects_.end(); it++)
  {
    bwm_observable_sptr obj = it->first;
    if ( obj->num_faces() <  min_faces)
      // single mesh face is probably ground plane, which we do not want to render
      continue;

    // check if object is texture mapped
    if (obj->type_name().compare("bwm_observable_textured_mesh") == 0)
    {
      bwm_observable_textured_mesh* mesh = static_cast<bwm_observable_textured_mesh*>(obj.as_pointer());
      std::string image_fname = vul_file::strip_directory(mesh->tex_map_uri()); // assume all faces have same texmap img
      std::string image_name = vul_file::strip_extension(image_fname);

      std::ostringstream image_path;
      image_path << "../images/" << image_fname;

      std::ostringstream objname;
      objname << "object_"<<nobjects;

      std::ostringstream material_id;
      material_id << objname.str() <<"_materialID";

      std::ostringstream material_name;
      material_name << objname.str() <<"_material";

      std::ostringstream effect_id;
      effect_id << objname.str() << "_effect";

      std::ostringstream surface_id;
      surface_id << objname.str() << "_surface";

      std::ostringstream image_sampler_id;
      image_sampler_id << objname.str() << "_sampler";

      std::ostringstream geometry_id;
      geometry_id << objname.str() << "_geometry";

      std::ostringstream geometry_position_id;
      geometry_position_id << objname.str() << "_geometry_position";

      std::ostringstream geometry_position_array_id;
      geometry_position_array_id << objname.str() <<"_geometry_position_array";

      std::ostringstream geometry_uv_id;
      geometry_uv_id << objname.str() << "_geometry_uv";

      std::ostringstream geometry_uv_array_id;
      geometry_uv_array_id << objname.str() << "_geometry_uv_array";

      std::stringstream geometry_vertex_id;
      geometry_vertex_id << objname.str() << "_geometry_vertex";

      mesh_ids.push_back(objname.str());
      image_names.push_back(image_name);
      image_fnames.push_back(image_path.str());
      material_names.push_back(material_name.str());
      material_ids.push_back(material_id.str());
      effect_ids.push_back(effect_id.str());
      surface_ids.push_back(surface_id.str());
      image_sampler_ids.push_back(image_sampler_id.str());
      geometry_ids.push_back(geometry_id.str());
      geometry_position_ids.push_back(geometry_position_id.str());
      geometry_position_array_ids.push_back(geometry_position_array_id.str());
      geometry_uv_ids.push_back(geometry_uv_id.str());
      geometry_uv_array_ids.push_back(geometry_uv_array_id.str());
      geometry_vertex_ids.push_back(geometry_vertex_id.str());

      nobjects++;
    }
  }

  std::fprintf(dae_fp,"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
  std::fprintf(dae_fp,"<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\">\n");

  std::fprintf(dae_fp,"  <asset>\n");
  std::fprintf(dae_fp,"    <contributor>\n");
  std::fprintf(dae_fp,"      <authoring_tool> Brown University World Modeler </authoring_tool>\n");
  std::fprintf(dae_fp,"    </contributor>\n");
  std::fprintf(dae_fp,"    <unit name=\"meters\" meter=\"1\"/>\n");
  std::fprintf(dae_fp,"    <up_axis>Z_UP</up_axis>\n");
  std::fprintf(dae_fp,"  </asset>\n");

  std::fprintf(dae_fp,"  <library_images>\n");
  for (int i=0; i<nobjects; i++) {
    std::fprintf(dae_fp,"    <image id=\"%s\" name=\"%s\">\n",image_names[i].c_str(),image_names[i].c_str());
    std::fprintf(dae_fp,"      <init_from>%s</init_from>\n",image_fnames[i].c_str());
    std::fprintf(dae_fp,"    </image>\n");
  }
  std::fprintf(dae_fp,"  </library_images>\n");

  std::fprintf(dae_fp,"  <library_materials>\n");
  for (int i=0; i<nobjects; i++) {
    std::fprintf(dae_fp,"    <material id=\"%s\" name=\"%s\">\n",material_ids[i].c_str(),material_names[i].c_str());
    std::fprintf(dae_fp,"      <instance_effect url=\"#%s\"/>\n",effect_ids[i].c_str());
    std::fprintf(dae_fp,"    </material>\n");
  }
  std::fprintf(dae_fp,"  </library_materials>\n");

  std::fprintf(dae_fp,"  <library_effects>\n");
  for (int i=0; i<nobjects; i++)
  {
    std::fprintf(dae_fp,"    <effect id=\"%s\" name=\"%s\">\n",effect_ids[i].c_str(),effect_ids[i].c_str());
    std::fprintf(dae_fp,"      <profile_COMMON>\n");
    std::fprintf(dae_fp,"        <newparam sid=\"%s\">\n",surface_ids[i].c_str());
    std::fprintf(dae_fp,"          <surface type=\"2D\">\n");
    std::fprintf(dae_fp,"            <init_from>%s</init_from>\n",image_names[i].c_str());
    std::fprintf(dae_fp,"          </surface>\n");
    std::fprintf(dae_fp,"        </newparam>\n");
    std::fprintf(dae_fp,"        <newparam sid=\"%s\">\n",image_sampler_ids[i].c_str());
    std::fprintf(dae_fp,"          <sampler2D>\n");
    std::fprintf(dae_fp,"            <source>%s</source>\n",surface_ids[i].c_str());
    std::fprintf(dae_fp,"          </sampler2D>\n");
    std::fprintf(dae_fp,"        </newparam>\n");
    std::fprintf(dae_fp,"        <technique sid=\"COMMON\">\n");
    std::fprintf(dae_fp,"          <phong>\n");
    std::fprintf(dae_fp,"            <emission>\n");
    std::fprintf(dae_fp,"              <color>0.0 0.0 0.0 1</color>\n");
    std::fprintf(dae_fp,"            </emission>\n");
    std::fprintf(dae_fp,"            <ambient>\n");
    std::fprintf(dae_fp,"              <color>0.0 0.0 0.0 1</color>\n");
    std::fprintf(dae_fp,"            </ambient>\n");
    std::fprintf(dae_fp,"            <diffuse>\n");
    std::fprintf(dae_fp,"              <texture texture=\"%s\" texcoord=\"UVSET0\"/>\n",image_sampler_ids[i].c_str());
    std::fprintf(dae_fp,"            </diffuse>\n");
    std::fprintf(dae_fp,"            <specular>\n");
    std::fprintf(dae_fp,"              <color>0.33 0.33 0.33 1</color>\n");
    std::fprintf(dae_fp,"            </specular>\n");
    std::fprintf(dae_fp,"            <shininess>\n");
    std::fprintf(dae_fp,"              <float>20.0</float>\n");
    std::fprintf(dae_fp,"            </shininess>\n");
    std::fprintf(dae_fp,"            <reflectivity>\n");
    std::fprintf(dae_fp,"              <float>0.1</float>\n");
    std::fprintf(dae_fp,"            </reflectivity>\n");
    std::fprintf(dae_fp,"            <transparent>\n");
    std::fprintf(dae_fp,"              <color>1 1 1 1</color>\n");
    std::fprintf(dae_fp,"            </transparent>\n");
    std::fprintf(dae_fp,"            <transparency>\n");
    std::fprintf(dae_fp,"              <float>0.0</float>\n");
    std::fprintf(dae_fp,"            </transparency>\n");
    std::fprintf(dae_fp,"          </phong>\n");
    std::fprintf(dae_fp,"        </technique>\n");
    std::fprintf(dae_fp,"      </profile_COMMON>\n");
    std::fprintf(dae_fp,"    </effect>\n");
  }
  std::fprintf(dae_fp,"  </library_effects>\n");

  std::fprintf(dae_fp,"  <library_geometries>\n");
  int idx = 0;
  for (it = objects_.begin(); it != objects_.end(); it++)
  {
    bwm_observable_sptr obj = it->first;
    // assume object is texture mapped
    //dbmsh3d_textured_mesh_mc* mesh = (dbmsh3d_textured_mesh_mc*)obj->get_object();
    if (obj->num_faces() < min_faces) {
      // single mesh face is probably ground plane, which we do not want to render
      continue;
    }

    if (obj->type_name().compare("bwm_observable_textured_mesh") == 0) {
      bwm_observable_textured_mesh* tm_obj = static_cast<bwm_observable_textured_mesh*>(obj.as_pointer());
      tm_obj->save_kml_collada(dae_fp, lvcs_, geometry_ids[idx],
                               geometry_position_ids[idx],
                               geometry_position_array_ids[idx],
                               geometry_uv_ids[idx],
                               geometry_uv_array_ids[idx],
                               geometry_vertex_ids[idx],
                               material_names[idx]);
    }

    std::fprintf(dae_fp,"</p>\n");
    std::fprintf(dae_fp,"      </triangles>\n");
    std::fprintf(dae_fp,"    </mesh>\n");
    std::fprintf(dae_fp,"  </geometry>\n");
    idx++;
  }

  std::fprintf(dae_fp,"</library_geometries>\n");
  std::fprintf(dae_fp,"<library_nodes>\n");
  for (int i=0; i<nobjects; i++) {
    std::fprintf(dae_fp,"  <node id=\"Component_%d\" name=\"Component_%d\">\n",i,i);
    std::fprintf(dae_fp,"    <node id=\"%s\" name=\"%s\">\n",mesh_ids[i].c_str(),mesh_ids[i].c_str());
    std::fprintf(dae_fp,"      <instance_geometry url=\"#%s\">\n",geometry_ids[i].c_str());
    std::fprintf(dae_fp,"        <bind_material>\n");
    std::fprintf(dae_fp,"          <technique_common>\n");
    std::fprintf(dae_fp,"            <instance_material symbol=\"%s\" target=\"#%s\">\n",material_names[i].c_str(),material_ids[i].c_str());
    std::fprintf(dae_fp,"              <bind_vertex_input semantic=\"UVSET0\" input_semantic=\"TEXCOORD\" input_set=\"0\"/>\n");
    std::fprintf(dae_fp,"            </instance_material>\n");
    std::fprintf(dae_fp,"          </technique_common>\n");
    std::fprintf(dae_fp,"        </bind_material>\n");
    std::fprintf(dae_fp,"      </instance_geometry>\n");
    std::fprintf(dae_fp,"    </node>\n");
    std::fprintf(dae_fp,"  </node>\n");
  }
  std::fprintf(dae_fp,"</library_nodes>\n");

  std::fprintf(dae_fp,"<library_visual_scenes>\n");
  std::fprintf(dae_fp,"  <visual_scene id=\"WorldModelerScene\" name=\"WorldModelerScene\">\n");
  std::fprintf(dae_fp,"    <node id=\"Model\" name=\"Model\">\n");
  for (int i=0; i<nobjects; i++) {
    std::fprintf(dae_fp,"      <node id=\"Component_%d_1\" name=\"Component_%d_1\">\n",i,i);
    std::fprintf(dae_fp,"        <instance_node url=\"#Component_%d\"/>\n",i);
    std::fprintf(dae_fp,"      </node>\n");
  }
  std::fprintf(dae_fp,"    </node>\n");
  std::fprintf(dae_fp,"  </visual_scene>\n");
  std::fprintf(dae_fp,"</library_visual_scenes>\n");

  std::fprintf(dae_fp,"<scene>\n");
  std::fprintf(dae_fp,"  <instance_visual_scene url=\"#WorldModelerScene\"/>\n");
  std::fprintf(dae_fp,"</scene>\n");
  std::fprintf(dae_fp,"</COLLADA>\n");

  std::fclose(dae_fp);

  std::ostringstream textures_fname;
  textures_fname << kmz_dir << "/textures.txt";

  FILE* tex_fp;
  if ((tex_fp = std::fopen(textures_fname.str().data(), "w")) == NULL) {
    std::fprintf (stderr, "Can't open .dae file %s to write.\n", textures_fname.str().data());
    return;
  }

  for (int i=0; i<nobjects; i++) {
    std::fprintf(tex_fp,"<%s> <%s>\n",image_fnames[i].c_str(),image_fnames[i].c_str());
  }
  std::fclose(tex_fp);

  std::ostringstream kml_fname;
  kml_fname << kmz_dir << "/doc.kml";
  FILE* kml_fp;
  if ((kml_fp = std::fopen(kml_fname.str().data(), "w")) == NULL) {
    std::fprintf (stderr, "Can't open .kml file %s to write.\n", kml_fname.str().data());
    return;
  }

  std::fprintf(kml_fp,"<?xml version='1.0' encoding='UTF-8'?>\n");
  std::fprintf(kml_fp,"<kml xmlns='http://earth.google.com/kml/2.1'>\n");
  std::fprintf(kml_fp,"<Folder>\n");
  std::fprintf(kml_fp,"  <name>%s</name>\n",model_name.c_str());
  std::fprintf(kml_fp,"  <description><![CDATA[Created with <a href=\"http://www.lems.brown.edu\">Brown University World Modeler</a>]]></description>\n");
  std::fprintf(kml_fp,"  <DocumentSource>Brown University World Modeler</DocumentSource>\n");
  std::fprintf(kml_fp,"  <visibility>1</visibility>\n");
  std::fprintf(kml_fp,"  <LookAt>\n");
  std::fprintf(kml_fp,"    <heading>0</heading>\n");
  std::fprintf(kml_fp,"    <tilt>45</tilt>\n");
  std::fprintf(kml_fp,"    <longitude>%f</longitude>\n",origin_lon);
  std::fprintf(kml_fp,"    <latitude>%f</latitude>\n",origin_lat);
  std::fprintf(kml_fp,"    <range>200</range>\n");
  std::fprintf(kml_fp,"    <altitude>%f</altitude>\n",0.0f);
  std::fprintf(kml_fp,"  </LookAt>\n");
  std::fprintf(kml_fp,"  <Folder>\n");
  std::fprintf(kml_fp,"    <name>Tour</name>\n");
  std::fprintf(kml_fp,"    <Placemark>\n");
  std::fprintf(kml_fp,"      <name>Camera</name>\n");
  std::fprintf(kml_fp,"      <visibility>1</visibility>\n");
  std::fprintf(kml_fp,"    </Placemark>\n");
  std::fprintf(kml_fp,"  </Folder>\n");
  std::fprintf(kml_fp,"  <Placemark>\n");
  std::fprintf(kml_fp,"    <name>Model</name>\n");
  std::fprintf(kml_fp,"    <description><![CDATA[]]></description>\n");
  std::fprintf(kml_fp,"    <Style id='default'>\n");
  std::fprintf(kml_fp,"    </Style>\n");
  std::fprintf(kml_fp,"    <Model>\n");
  std::fprintf(kml_fp,"      <altitudeMode>relativeToGround</altitudeMode>\n");
  std::fprintf(kml_fp,"      <Location>\n");
  std::fprintf(kml_fp,"        <longitude>%f</longitude>\n",origin_lon + lon_offset);
  std::fprintf(kml_fp,"        <latitude>%f</latitude>\n",origin_lat + lat_offset);
  std::fprintf(kml_fp,"        <altitude>%f</altitude>\n",origin_elev - ground_height);
  std::fprintf(kml_fp,"      </Location>\n");
  std::fprintf(kml_fp,"      <Orientation>\n");
  std::fprintf(kml_fp,"        <heading>0</heading>\n");
  std::fprintf(kml_fp,"        <tilt>0</tilt>\n");
  std::fprintf(kml_fp,"        <roll>0</roll>\n");
  std::fprintf(kml_fp,"      </Orientation>\n");
  std::fprintf(kml_fp,"      <Scale>\n");
  std::fprintf(kml_fp,"        <x>1.0</x>\n");
  std::fprintf(kml_fp,"        <y>1.0</y>\n");
  std::fprintf(kml_fp,"        <z>1.0</z>\n");
  std::fprintf(kml_fp,"      </Scale>\n");
  std::fprintf(kml_fp,"      <Link>\n");
  std::fprintf(kml_fp,"        <href>models/mesh.dae</href>\n");
  std::fprintf(kml_fp,"      </Link>\n");
  std::fprintf(kml_fp,"    </Model>\n");
  std::fprintf(kml_fp,"  </Placemark>\n");
  std::fprintf(kml_fp,"</Folder>\n");
  std::fprintf(kml_fp,"</kml>\n");
  std::fclose(kml_fp);
}
#endif // 0

#if 0
void bwm_observer_rat_cam::generate_textures()
{
  if (!lvcs_) {
    std::cerr << "Error: must define LVCS coordinate system before generating tex coords.\n";
    return;
  }
  vgui_dialog params("Texture Map filename");
  std::string ext, tex_filename, empty = "";
  params.file("Texture map...",ext,tex_filename);

  if (!params.ask())
    return;

  bwm_texture_map_generator tex_generator;

  // each object independently for now.
  std::map<bwm_observable_sptr, std::map<unsigned, bgui_vsol_soview2D_polygon* > >::iterator it;
  int mesh_idx = 0;
  for (it = objects_.begin(); it != objects_.end(); it++, mesh_idx++) {
    bwm_observable_sptr o = it->first;
    if (o->type_name().compare("bwm_observable_textured_mesh") == 0) {
      std::ostringstream tex_fullpath;
      tex_fullpath << tex_filename << '.' << mesh_idx << ".jpg";
      bwm_observable_textured_mesh* obj = static_cast<bwm_observable_textured_mesh*> (o.as_pointer());
      tex_generator.generate_texture_map(obj, tex_fullpath.str(), *lvcs_);
    }
  }
  return;
}
#endif // 0

#if 0
void bwm_observer_rat_cam::save_lvcs()
{
  std::string filename = select_file();
  // just save origin for now
  std::ofstream os(filename.data());
  double lat,lon,elev;
  lvcs_->get_origin(lat,lon,elev);
  os.precision(12);
  os << lat << ' '<< lon << ' ' << elev << std::endl;

  return;
}

void bwm_observer_rat_cam::load_lvcs()
{
  std::string filename = select_file();
  // just load origin for now
  std::ifstream is(filename.data());
  double lat, lon, elev;
  is >> lat;
  is >> lon;
  is >> elev;

  lvcs_ = new vpgl_lvcs(lat,lon,elev,vpgl_lvcs::wgs84,vpgl_lvcs::DEG,vpgl_lvcs::METERS);
  std::cout << "loaded lvcs with origin "<<lat<<", "<<lon<<", "<<elev<<std::endl;

  return;
}
#endif // 0

#if 0
void bwm_observer_rat_cam::convert_file_to_lvcs()
{
  if (!lvcs_) {
    std::cerr << "error: lvcs is not defined!\n";
    return;
  }
  // expects simple text file with each line being of the form "lat lon z"
  std::string filename_in = select_file();
  std::string filename_out = filename_in + ".lvcs";
  std::ifstream is(filename_in.data());
  std::ofstream os(filename_out.data());

  double lat,lon,elev;
  double x,y,z;

  while (!is.eof()) {
    is >> lat;
    is >> lon;
    is >> elev;
    lvcs_->global_to_local(lon,lat,elev,vpgl_lvcs::wgs84,x,y,z,vpgl_lvcs::DEG,vpgl_lvcs::METERS);
    os << x <<' '<< y <<' '<<z<<std::endl;
  }

  return;
}
#endif // 0

std::string bwm_observer_rat_cam::select_file()
{
  vgui_dialog params ("File Open");
  std::string ext, file, empty="";

  params.file ("Open...", ext, file);
  if (!params.ask())
    return empty;

  if (file == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify an input file (prefix)." );
    error.ask();
    return empty;
  }
  return file;
}

std::ostream& bwm_observer_rat_cam::print_camera(std::ostream& s)
{
  vpgl_rational_camera<double>* rat_cam = static_cast<vpgl_rational_camera<double> *> (camera_);
  s << *rat_cam;
  return s;
}


// If the correspondence mode is world_to_image
// and there is at least one existing correspondence
// that has a world point defined then shift the current image
// to align its correspondence with the world point.
void bwm_observer_rat_cam::adjust_camera_to_world_pt()
{
#if 0
  std::vector<bwm_corr_sptr> corrs =
    bwm_observer_mgr::instance()->correspondences();
  if (!corrs.size())
  {
    std::cerr << "In bwm_observer_rat_cam::adjust_camera_to_world_pt() -"
             << " no correspondences to use for alignment\n";
   return;
  }

  //Find the world to image correspondence for this observer_cam
  bwm_corr_sptr corr;
  bwm_observer_cam* this_obs = (bwm_observer_cam*)this;
  for (std::vector<bwm_corr_sptr>::iterator cit = corrs.begin();
       cit != corrs.end(); ++cit)
    if ((*cit)->obs_in(this_obs))
      corr = *cit;

  // get the world point to use for alignment
  if (!corr||corr->mode())
  {
    std::cerr << "In bwm_observer_rat_cam::adjust_camera_to_world_pt() -"
             << " corr is null or correspondence is not world_to_image\n";
   return;
  }
#endif
  vgl_point_3d<double> pt_3d;
  if (!bwm_world::instance()->world_pt(pt_3d))
  {
    std::cerr << "In bwm_observer_rat_cam::adjust_camera_to_world_pt() -"
             << " no world point defined\n";
    return;
  }

  // get the corresponding 2-d point
  float x=0, y=0;
  this->corr_image_pt(x, y);

  // now all that has to be done is to project pt_3d and adjust the camera
  vpgl_rational_camera<double> rcam = this->camera();
  vgl_point_2d<double> proj_point_2d = rcam.project(pt_3d);

  double tx = x-proj_point_2d.x();
  double ty = y-proj_point_2d.y();
  this->shift_camera(tx, ty);
  std::cout << "Shifting camera[" << this->camera_path() <<  "]:\n("
           << tx << ' ' << ty << "):\n point_3d("
           << pt_3d.x() << ' ' << pt_3d.y()
           << ' ' << pt_3d.z() << ")\n";
  cam_adjusted_ = true;
  //send the objects in the world the fact that they need to redisplay
  std::vector<bwm_observable_sptr> objs = bwm_world::instance()->objects();
  for (std::vector<bwm_observable_sptr>::iterator oit = objs.begin();
       oit != objs.end(); ++oit)
    (*oit)->send_update();

  this->post_redraw();
}

// Just a temporary experiment to understand how edges behave across image
// resolution and sensor modality. Not to be used in production
void bwm_observer_rat_cam::project_edges_from_master()
{
  bwm_observer_cam* mobs = bwm_observer_mgr::BWM_MASTER_OBSERVER;
  if (!mobs)
  {
    std::cout << "In bwm_observer_rat_cam::project_edges_from_master() -"
             << " no master selected\n";
    return;
  }
  // First determine if the master has a selected box
  bgui_vsol_soview2D_polygon* box = nullptr;
  bool selected =
    mobs->get_selected_box(box);
  if (!selected)
  {
    std::cout << "In bwm_observer_rat_cam::project_edges_from_master() -"
             << " no box selected\n";
    return;
  }
  std::vector<vsol_digital_curve_2d_sptr> edges_2d = mobs->edges(box->get_id());
  if (!edges_2d.size())
  {
    std::cout << "In bwm_observer_rat_cam::project_edges_from_master() -"
             << " no edges in box\n";
    return;
  }

  vpgl_camera<double>* mcam  =   mobs->camera();
  if (!mcam)
  {
    std::cout << "In bwm_observer_rat_cam::project_edges_from_master() -"
             << " master camera null\n";
    return;
  }

  vpgl_rational_camera<double> master_cam =
    *static_cast<vpgl_rational_camera<double>* >(mcam);
  vgl_plane_3d<double> hpl= mobs->get_proj_plane();
  vgl_plane_3d<double> master_plane(hpl);
  vgl_point_3d<double> wpt;
  if (!bwm_world::instance()->world_pt(wpt))
  {
    std::cout << "In bwm_observer_rat_cam::project_edges_from_master() -"
             << " no world point to use as an initial guess\n";
    return;
  }
  std::vector<vsol_digital_curve_3d_sptr> edges_3d;
  if (! bwm_reg_utils::back_project_edges(edges_2d, master_cam, master_plane,
                                          wpt, edges_3d))
  {
    std::cout << "In bwm_observer_rat_cam::project_edges_from_master() -"
             << " back-projection of edges failed\n";
    return;
  }
  //now project forward onto this image
  if (!camera_)
  {
    std::cout << "In bwm_observer_rat_cam::project_edges_from_master() -"
             << " rational camera null\n";
    return;
  }

  vpgl_rational_camera<double> my_cam =
    *static_cast<vpgl_rational_camera<double>* >(camera_);
  std::vector<vsol_digital_curve_2d_sptr> transfered_curves;
  bwm_reg_utils::project_edges(edges_3d, my_cam,transfered_curves);

  vgui_style_sptr pstyle = vgui_style::new_style(0.1f, 0.8f, 0.1f, 1.0f, 3.0f);
  std::vector<vsol_digital_curve_2d_sptr>::iterator cit =
    transfered_curves.begin();
  for (; cit != transfered_curves.end(); ++cit)
    this->add_digital_curve(*cit, pstyle);
  this->post_redraw();
}

// A method to register a search image to the master image
// prototype only
void bwm_observer_rat_cam::register_search_to_master()
{
  bwm_observer_cam* eo_obs = bwm_observer_mgr::BWM_EO_OBSERVER;
  if (!eo_obs)
  {
    std::cout << "In bwm_observer_rat_cam::register_search_to_master() -"
             << " no eo observer selected\n";
    return;
  }
  vil_image_resource_sptr eo_image =
    eo_obs->image_tableau()->get_image_resource();
  if (!eo_image)
  {
    std::cout << "In bwm_observer_rat_cam::register_search_master() -"
             << " eo observer image null\n";
    return;
  }
  vpgl_camera<double>* eoc  =   eo_obs->camera();
  if (!eoc)
  {
    std::cout << "In bwm_observer_rat_cam::register_search_to_master() -"
             << " eo observer camera null\n";
    return;
  }
  vpgl_rational_camera<double> eo_cam =
    *static_cast<vpgl_rational_camera<double>* >(eoc);

  bwm_observer_cam* other_mode_obs = bwm_observer_mgr::BWM_OTHER_MODE_OBSERVER;
  if (!other_mode_obs)
  {
    std::cout << "In bwm_observer_rat_cam::register_search_to_master() -"
             << " no other observer selected\n";
    return;
  }
  vil_image_resource_sptr other_mode_image =
    other_mode_obs->image_tableau()->get_image_resource();
  if (!other_mode_image)
  {
    std::cout << "In bwm_observer_rat_cam::register_search_to_master() -"
             << " other_observer image null\n";
    return;
  }
  vpgl_camera<double>* other_mode_c  =  other_mode_obs->camera();
  if (!other_mode_c)
  {
    std::cout << "In bwm_observer_rat_cam::register_search_to_master() -"
             << " other_observer camera null\n";
    return;
  }

  vpgl_rational_camera<double> other_mode_cam =
    *static_cast<vpgl_rational_camera<double>* >(other_mode_c);


  vgl_point_3d<double> wpt;
  if (!bwm_world::instance()->world_pt(wpt))
  {
    std::cout << "In bwm_observer_rat_cam::register_search_to_master() -"
             << " no world point to use as an initial guess\n";
    return;
  }
  vgl_plane_3d<double> world_plane = bwm_world::instance()->world_plane();

  vpgl_rational_camera<double> search_cam =
    *static_cast<vpgl_rational_camera<double>* >(camera_);
  vil_image_resource_sptr search_image =
    this->image_tableau()->get_image_resource();

  bwm_reg_processor brp(eo_image, eo_cam, other_mode_image, other_mode_cam,
                        wpt, world_plane,  search_image, search_cam);

  vgui_dialog reg_params("Automatic Registration");
  unsigned min_curve_length = 10;
  static double radius = 20.0;
  static double perr = 20.0;
  static double dist_threshold = 1.0;
  static double angle_threshold = 0.5;
  static double min_probability = 0.998;
  static float model_noise_threshold = 2.0f;
  static float search_noise_threshold = 2.0f;
  static bool show_edges = true;
  reg_params.field ("World Point Neighborhood Radius (meters)", radius);
  reg_params.field ("Expected Projection Error (meters)", perr);
  reg_params.field ("Distance threshold (pixels)", dist_threshold);
  reg_params.field ("Angle threshold (radians)", angle_threshold);
  reg_params.field ("Minimum Cumulative Probability ", min_probability);
  reg_params.field ("Model Detector threshold ", model_noise_threshold);
  reg_params.field ("Search Detector threshold ", search_noise_threshold);
  reg_params.checkbox("Show Edges?", show_edges);
  if (!reg_params.ask())
    return;
  int tcol=0, trow=0;
  if (!brp.match(radius, perr, dist_threshold, angle_threshold,
                 min_curve_length,
                 min_probability,
                 model_noise_threshold,
                 search_noise_threshold,
                 tcol, trow))
  {
    std::cout << "In bwm_observer_rat_cam::register_search_to_master() -"
             << " registration failed\n";
    //   return;
  }

  if (show_edges) {
    std::vector<vsol_digital_curve_2d_sptr> search_edges = brp.search_curves();
    std::vector<vsol_digital_curve_2d_sptr> trans_model_edges
      = brp.trans_model_curves();
    this->display_reg_seg(search_edges, trans_model_edges);
  }
  //  this->shift_camera(tcol, trow);
}
