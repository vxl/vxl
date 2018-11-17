#include <sstream>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "betr_event_trigger.h"
#include "betr_geo_object_3d.h"
#include "betr_gridded_geo_polygon_3d.h"
#include "vsol_mesh_3d.h"
#include <vsol/vsol_region_3d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_3d.h>
#include <bmsh3d/bmsh3d_mesh.h>
#include <bmsh3d/bmsh3d_mesh_mc.h>
#include <bmsh3d/algo/bmsh3d_fileio.h>
#include <vpgl/vpgl_lvcs.h>
#include <vsol/vsol_spatial_object_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include "betr_edgel_change_detection.h"
#include "betr_edgel_reference_cd.h"
#include "betr_pixelwise_change_detection.h"
#include "betr_algorithm.h"
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <bil/bil_convert_to_grey.h>
unsigned betr_event_trigger::process_counter_ = 0;

void betr_event_trigger::set_ref_image(const vil_image_resource_sptr& ref_imgr, bool apply_mask, bool keep_data){
  vil_image_resource_sptr temp;
  bil_convert_resource_to_grey cnv;
  cnv(ref_imgr, temp, apply_mask);
  if (!keep_data) {
    ref_rescs_.clear();
  }
  ref_rescs_.push_back(temp);
}
void betr_event_trigger::set_ref_images(std::vector<vil_image_resource_sptr> const& ref_rescs, bool apply_mask){
  ref_rescs_.clear();
  bil_convert_resource_to_grey cnv;
  for(const auto & ref_resc : ref_rescs){
    vil_image_resource_sptr temp;
    cnv(ref_resc, temp, apply_mask);
    ref_rescs_.push_back(temp);
  }
}
void betr_event_trigger::set_evt_image(const vil_image_resource_sptr& evt_imgr, bool apply_mask){
  bil_convert_resource_to_grey cnv;
  cnv(evt_imgr, evt_imgr_, apply_mask);
}
vpgl_camera_double_sptr betr_event_trigger::cast_camera(vpgl_camera_double_sptr const& camera){
  vpgl_camera_double_sptr ret_cam;
  if(!camera){
    std::cout <<"Fatal - reference camera" << std::endl;
    return ret_cam;
  }
  // if camera is already local do nothing
  if(camera->type_name() == "vpgl_local_rational_camera"){
    return camera;
  }else  if(camera->type_name() == "vpgl_rational_camera"){
    auto* rat_cam_ptr = dynamic_cast<vpgl_rational_camera<double>*>(camera.ptr());
    if(!rat_cam_ptr){
      std::cout << "Fatal - can't convert camera to rational_camera" << std::endl;
      return ret_cam;
    }
    auto* lcam_ptr = new vpgl_local_rational_camera<double>(lvcs_, *rat_cam_ptr);
    ret_cam  = lcam_ptr;
  }else{
    std::cout << "Fatal - camera not global or local rational_camera" << std::endl;
    return ret_cam;
  }
  return ret_cam;
}
void betr_event_trigger::set_ref_camera(vpgl_camera_double_sptr const& camera, bool keep_data){
  if (!keep_data) {
    ref_cameras_.clear();
  }
  vpgl_camera_double_sptr temp = cast_camera(camera);
  if(temp)
    ref_cameras_.push_back(temp);
}
void betr_event_trigger::set_ref_cameras(std::vector<vpgl_camera_double_sptr> const& cameras){
  ref_cameras_.clear();
  for(const auto & camera : cameras){
    vpgl_camera_double_sptr temp = cast_camera(camera);
    if(temp)
      ref_cameras_.push_back(temp);
    else{
      std::cout << "Can't set ref cameras - null cast" << std::endl;
      return;
    }
  }
}
void betr_event_trigger::set_evt_camera(vpgl_camera_double_sptr const& camera){
  if(!camera){
    std::cout <<"Fatal - Null reference camera" << std::endl;
    return;
  }
  // if camera is already local do nothing
  if(camera->type_name() == "vpgl_local_rational_camera"){
    std::cout << " setting local camera " << std::endl;
    evt_camera_ = camera;
    return;
  }else  if(camera->type_name() == "vpgl_rational_camera"){
    std::cout << " converting to local camera " << std::endl;
    auto* rat_cam_ptr = dynamic_cast<vpgl_rational_camera<double>*>(camera.ptr());
    if(!rat_cam_ptr){
      std::cout << "Fatal - can't convert camera to rational_camera" << std::endl;
      return;
    }
    std::cout << "LVCS " << lvcs_ << std::endl;
    auto* lcam_ptr = new vpgl_local_rational_camera<double>(lvcs_, *rat_cam_ptr);
    evt_camera_  = lcam_ptr;
  }else{
      std::cout << "Fatal - camera not global or local rational_camera" << std::endl;
      return;
  }
}

void betr_event_trigger::register_algorithms(){
  betr_algorithm_sptr alg0 = new betr_edgel_change_detection();
  algorithms_[alg0->name()] = alg0;
  betr_algorithm_sptr alg1 = new betr_edgel_reference_cd();
  algorithms_[alg1->name()] = alg1;
  betr_algorithm_sptr alg2 = new betr_pixelwise_change_detection();
  algorithms_[alg2->name()] = alg2;
}
void betr_event_trigger::update_local_bounding_box(){
  if(global_bbox_.is_empty())
    return;
  if(!lvcs_valid_){
    std::cout << "lvcs for trigger region not defined - fatal\n";
    return;
  }
  if(!local_bbox_)
    local_bbox_ = new vsol_box_3d();
  // global geographic coordinates (lon, lat, elev)
  vgl_point_3d<double> min_pt = global_bbox_.min_point();
  vgl_point_3d<double> max_pt = global_bbox_.max_point();
  // convert to local Cartesian coordinates
  double lx=0.0, ly=0.0, lz=0.0;
  lvcs_.global_to_local(min_pt.x(), min_pt.y(), min_pt.z(), vpgl_lvcs::wgs84,
                        lx, ly, lz);
  local_bbox_->add_point(lx, ly, lz);

  lvcs_.global_to_local(max_pt.x(), max_pt.y(), max_pt.z(), vpgl_lvcs::wgs84,
                        lx, ly, lz);
  local_bbox_->add_point(lx, ly, lz);
}
bool betr_event_trigger::add_geo_object(std::string const& name, double lon, double lat ,
                                        double elev, std::string const& geom_path, bool is_ref_obj){
  //load the mesh from the geometry file
  bmsh3d_mesh* mesh = new bmsh3d_mesh_mc();
  bool good = bmsh3d_load_ply(mesh, geom_path.c_str());
  if(!good){
    std::cout << "invalid bmesh3d ply file - " << geom_path << std::endl;
    return false;
  }
  //convert to a vsol_spatial_object_3d
  auto* vmesh = new vsol_mesh_3d();
  auto* mesh_mc = dynamic_cast< bmsh3d_mesh_mc*>(mesh);
  vmesh->set_mesh(mesh_mc);
  vsol_spatial_object_3d_sptr so = vmesh;
  //define the global origin and store in the map
  vpgl_lvcs lvcs(lat, lon, elev, vpgl_lvcs::wgs84, vpgl_lvcs::DEG);
  betr_geo_object_3d_sptr geo_obj = new betr_geo_object_3d(so, lvcs);
  this->add_geo_object(name, geo_obj, is_ref_obj);
  return true;
}
bool betr_event_trigger::add_gridded_event_poly(std::string const& name, double lon, double lat ,
                                                double elev, std::string const& geom_path,
                                                double grid_spacing){
  //load the mesh from the geometry file
  bmsh3d_mesh* mesh = new bmsh3d_mesh_mc();
  bool good = bmsh3d_load_ply(mesh, geom_path.c_str());
  if(!good){
    std::cout << "invalid bmesh3d ply file - " << geom_path << std::endl;
    return false;
  }
  //convert to a vsol_mesh_3d
  auto* vmesh = new vsol_mesh_3d();
  auto* mesh_mc = dynamic_cast< bmsh3d_mesh_mc*>(mesh);
  vmesh->set_mesh(mesh_mc);
  if(vmesh->num_faces() != 1){
   std::cout << "Mesh is not a single polygon - can't be gridded" << geom_path << std::endl;
    return false;
  }
  // convert mesh to a vsol_polygon_3d
  std::vector<vsol_point_3d_sptr> verts = vmesh->vertices();
  auto* poly = new vsol_polygon_3d(verts);
  vsol_spatial_object_3d_sptr so = poly;
  // define global origin
  vpgl_lvcs lvcs(lat, lon, elev, vpgl_lvcs::wgs84, vpgl_lvcs::DEG);

  // the grid polys are generated in the constructor below
  betr_gridded_geo_polygon_3d geo_obj(so, lvcs, grid_spacing);

  // add an event geo object for each grid polygon
  const std::vector<vsol_polygon_3d_sptr>& polys = geo_obj.grid_polys();
  unsigned grid_count = 0;
  for(const auto & poly : polys){
    // create the event object
    std::stringstream ss;
    ss<< name << '_'<< grid_count++;//define a unique name extension
    std::string grid_name = ss.str();
    vsol_spatial_object_3d_sptr pso = new vsol_polygon_3d(*poly);
    betr_geo_object_3d_sptr go = new betr_geo_object_3d(pso, lvcs);
    //
    //added as an event object (not reference)
    //                                    |
    //                                    v
    this->add_geo_object(grid_name, go, false);
  }
  return true;
}
//The object in general will have a different lvcs from the event trigger origin
//but assume that X and Y translations are just given in the tangent plane
void betr_event_trigger::add_geo_object(std::string const& obj_name, betr_geo_object_3d_sptr const& geo_object, bool is_ref_obj){
  betr_geo_box_3d box = geo_object->bounding_box();
  global_bbox_.add(box);
  this->update_local_bounding_box();
  //assume local transform is identity and lvcs is in degrees and meters
  vpgl_lvcs obj_lvcs = geo_object->lvcs();
  double obj_lat = 0.0, obj_lon = 0.0, obj_elev =0.0;
  obj_lvcs.get_origin(obj_lat, obj_lon, obj_elev);
  double trig_lat = 0.0, trig_lon = 0.0, trig_elev =0.0;
  lvcs_.get_origin(trig_lat, trig_lon, trig_elev);
  double tx, ty;
  lvcs_.angle_diff_to_cartesian_vector(trig_lon, trig_lat, obj_lon, obj_lat, tx, ty);
  double tz = obj_elev - trig_elev;
  vgl_vector_3d<double> transl(tx, ty, tz);
  if(is_ref_obj)
    ref_trigger_objects_[obj_name]=geo_object;
  else
    evt_trigger_objects_[obj_name] = geo_object;
  local_trans_[obj_name]=transl;
}
bool betr_event_trigger::project_object(const vpgl_camera_double_sptr& cam, std::string const& obj_name, vsol_polygon_2d_sptr& poly_2d){
  // determine if object is a reference or event object
  std::map<std::string, betr_geo_object_3d_sptr>::iterator oit;
  bool is_ref_obj = false;
  oit = ref_trigger_objects_.find(obj_name);
  is_ref_obj = oit != ref_trigger_objects_.end();

  betr_geo_object_3d_sptr obj;
  if(is_ref_obj){
    obj = ref_trigger_objects_[obj_name];
    if(!obj){
      std::cout << "no ref geo object with name " << obj_name << '\n';
      return false;
    }
  }else{
    obj = evt_trigger_objects_[obj_name];
    if(!obj){
      std::cout << "no evt geo object with name " << obj_name << '\n';
      return false;
    }
  }
  vgl_vector_3d<double> transl = local_trans_[obj_name];
  vsol_spatial_object_3d_sptr so_ptr = obj->obj();
  if(!so_ptr){
   std::cout << "null spatial object on  " << obj_name << '\n';
    return false;
  }
  vsol_region_3d* reg_ptr = nullptr;
  vsol_volume_3d* vol_ptr = nullptr;
  if( ( reg_ptr = so_ptr->cast_to_region() ) )
    {
      vsol_polygon_3d* poly_3d = reg_ptr->cast_to_polygon();
      if(!poly_3d){
        std::cout << "only handle polygonal regions for now " << obj_name << " is not a vsol_polygon_3d\n";
        return false;
      }
      poly_2d = project_poly(cam, poly_3d, transl);
      return true;
    }else if( ( vol_ptr = so_ptr->cast_to_volume() ) ){
      vsol_mesh_3d* mesh_3d = vol_ptr->cast_to_mesh();
      if(!mesh_3d){
        std::cout << "only handle vsol_mesh_3d for now " << obj_name << " is not a vsol_mesh_3d\n";
        return false;
      }
      std::vector<vsol_point_3d_sptr> verts = mesh_3d->vertices();

      std::vector<vgl_point_2d<double> > pts_2d;
      for(auto & vert : verts){
        double u = 0, v = 0;
        cam->project(vert->x()+transl.x(),
                     vert->y()+transl.y(),
                     vert->z()+transl.z(), u, v);
        pts_2d.emplace_back(u, v);
      }
      std::vector<vsol_point_2d_sptr> sverts;
      if(mesh_3d->num_faces()>1)
        {
          vgl_convex_hull_2d<double> conv_hull(pts_2d);
          vgl_polygon<double> h = conv_hull.hull();
          std::vector<vgl_point_2d<double> > hverts = h[0];
          for(auto & hvert : hverts)
            sverts.push_back(new vsol_point_2d(hvert));
        }else
        for(auto vit : pts_2d)
          sverts.push_back(new vsol_point_2d(vit));
      poly_2d = new vsol_polygon_2d(sverts);
      return true;
  }
 return false;
}
vsol_polygon_2d_sptr  betr_event_trigger::project_poly(vpgl_camera_double_sptr const& camera,
                                                       const vsol_polygon_3d_sptr& poly_3d,
                                                       vgl_vector_3d<double> const& transl){
 std::vector<vsol_point_2d_sptr> vertices;
  for (unsigned i=0; i<poly_3d->size(); i++) {
    double u = 0,v = 0;
#ifdef DEBUG
    std::cout << "3d point " << *(poly_3d->vertex(i)) << std::endl;
#endif
    vgl_point_3d<double> p(poly_3d->vertex(i)->x()+transl.x(),
                           poly_3d->vertex(i)->y()+transl.y(),
                           poly_3d->vertex(i)->z()+transl.z());
    p += transl;
    camera->project(p.x(), p.y(), p.z(), u,v);
    vsol_point_2d_sptr vp = new vsol_point_2d(u,v);
    vertices.push_back(vp);
  }
  vsol_polygon_2d_sptr poly_2d = new vsol_polygon_2d (vertices);
  return poly_2d;
}

bool betr_event_trigger::process(std::string alg_name, double& prob_change,std::string const& params_json){
  std::vector<double> scores;
  bool good = this->process(std::move(alg_name), scores, params_json);
  if(good&&scores.size()>=1){
    prob_change = scores[0];
    return good;
  }
  prob_change = -1.0;
  return false;
}
bool betr_event_trigger::process(std::string alg_name, std::vector<double>& prob_change,std::string const& params_json){
  std::vector<vil_image_resource_sptr> rescs;
  std::vector<vgl_point_2d<unsigned> > offsets;
  bool good = this->process(std::move(alg_name), prob_change, rescs, offsets, params_json);
  return good;
}
bool betr_event_trigger::process(std::string alg_name, double& prob_change,
                                 vil_image_resource_sptr change_img, vgl_point_2d<unsigned> offset,
                                 std::string const& params_json){
  std::vector<double> scores;
  std::vector<vil_image_resource_sptr> rescs;
  std::vector<vgl_point_2d<unsigned> > offsets;
  bool good = this->process(std::move(alg_name), scores, rescs, offsets, params_json);
  if(good&&scores.size()>=1){
    prob_change = scores[0];
    change_img = rescs[0];
    offset = offsets[0];
    return good;
  }
  prob_change = -1.0;
  return false;
}
bool betr_event_trigger::process(std::string alg_name, std::vector<double>& prob_change,
                                 std::vector<vil_image_resource_sptr>& change_images,
                                 std::vector<vgl_point_2d<unsigned> >& offsets, std::string const& params_json){
  std::vector<std::string> event_region_names;
  return process(std::move(alg_name), prob_change, event_region_names, change_images, offsets, params_json);
}

bool betr_event_trigger::process(const std::string& alg_name, std::vector<double>& prob_change,
                                 std::vector<std::string>& event_region_names,
                                 std::vector<vil_image_resource_sptr>& change_images,
                                 std::vector<vgl_point_2d<unsigned> >& offsets, std::string const& params_json){

  betr_algorithm_sptr alg = algorithms_[alg_name];
  if(!alg){
    std::cout <<"algorithm " << alg_name << " does not exist" << std::endl;
    return false;
  }
  if(!ref_rescs_.size() || !evt_imgr_ ){
    std::cout << "reference image(s) or event image not set";
      return false;
  }
  // for now only one ref object and one or more event objects
  if(evt_trigger_objects_.size() < 1 || ref_trigger_objects_.size() != 1 ){
    std::cout << "for now only one ref object and one or more evt object"<< std::endl;
    return false;
  }
  // extract algorithm parameters
  betr_params_sptr params = alg->params();
  bool success = read_params_json(params_json, params);
  if(!success){
    std::cout << "Bad parse of json parameter string " << params_json << std::endl;
    return false;
  }

  std::cout << "Number of Reference Images:  " << ref_rescs_.size() << std::endl;
  std::cout << "Number of Reference Cameras: " << ref_cameras_.size() << std::endl;
  if(verbose_){
    std::cout << "Reference Image: " << ref_path_ << std::endl;
    std::cout << "Event Image: " << evt_path_ << std::endl;
  }
  prob_change.clear();
  auto rit = ref_trigger_objects_.begin();
  std::string ref_obj_name = rit->first;
  // project the reference object into both reference and event images
  vsol_polygon_2d_sptr evt_ref_poly, ref_ref_poly;
  std::vector<vsol_polygon_2d_sptr> ref_ref_polys;
  for(auto & ref_camera : ref_cameras_){
    if(!this->project_object(ref_camera, ref_obj_name, ref_ref_poly))
      return false;
      ref_ref_polys.push_back(ref_ref_poly);
    }
  alg->set_verbose(verbose_);
  if(!this->project_object(evt_camera_, ref_obj_name, evt_ref_poly)){
      return false;
  }
  // iterate through the event objects and project them onto both reference and event images
  for(auto & evt_trigger_object : evt_trigger_objects_){
    // clear the algorithm data, since only one event region at a time is processed
    alg->clear();
    // reset the image resources

    if (alg->requires_multiple_ref_images()){
      alg->set_reference_images(ref_rescs_);
     }else{
      alg->set_reference_image(ref_rescs_[0]);
     }
    alg->set_event_image(evt_imgr_);
    std::string evt_obj_name = evt_trigger_object.first;
    std::stringstream ss;
    ss<< betr_event_trigger::process_counter_;
    alg->set_identifier(evt_obj_name + "_" + ss.str());
    std::cout << "Processing event object " << evt_obj_name << std::endl;
    // project the event objects into both reference and event images
    std::vector<vsol_polygon_2d_sptr> ref_evt_polys;
    vsol_polygon_2d_sptr  evt_evt_poly;
    for(auto & ref_camera : ref_cameras_){
      vsol_polygon_2d_sptr ref_evt_poly;
      if(!this->project_object(ref_camera, evt_obj_name, ref_evt_poly))
      return false;
      ref_evt_polys.push_back(ref_evt_poly);
  }
    if(alg->requires_multiple_ref_images()){
      alg->set_proj_ref_ref_objects(ref_ref_polys);//same for all event objects
      alg->set_proj_ref_evt_objects(ref_evt_polys);
    }else{
      alg->set_proj_ref_ref_object(ref_ref_polys[0]);//same for all event objects
      alg->set_proj_ref_evt_object(ref_evt_polys[0]);
    }
    if(!this->project_object(evt_camera_, evt_obj_name, evt_evt_poly))
      return false;
    alg->set_proj_evt_ref_object(evt_ref_poly);//same for all event objects
    alg->set_proj_evt_evt_object(evt_evt_poly);
    // all the data has been assigned, so the algorithm can process
    if(!alg->process()){
      event_region_names.push_back(evt_obj_name);
      prob_change.push_back(-1.0);
      std::cout << "Algorithm failed " << std::endl;
      return false;
    }
    event_region_names.push_back(evt_obj_name);
    prob_change.push_back(alg->prob_change());
    unsigned ioff, joff;
    vil_image_resource_sptr resc = alg->change_image(ioff, joff);
    vgl_point_2d<unsigned> offset(ioff, joff);
    change_images.push_back(resc);
    offsets.push_back(offset);
    std::cout << "Size of offsets is " << offsets.size() << "\n";
  }
  process_counter_++;
  return true;
}

std::vector<std::string> betr_event_trigger::algorithms() const{
  std::vector<std::string> ret;
  auto ait = algorithms_.begin();
  for(;ait!=algorithms_.end(); ++ait)
    ret.push_back(ait->first);
  return ret;
}

bool betr_event_trigger::save_projected_polys(std::string const& path, std::vector<vsol_polygon_2d_sptr> const& polys){
  vsl_b_ofstream ostr(path);
  if(!ostr){
    std::cout << "couldn't open binary stream for " << path << '\n';
    return false;
  }
  // convert to spatial object 2d
  std::vector<vsol_spatial_object_2d_sptr> sos;
  for(const auto & poly : polys){
    vsol_spatial_object_2d_sptr ply = dynamic_cast<vsol_spatial_object_2d*>(poly.ptr());
    sos.push_back(ply);
  }
  vsl_b_write(ostr, sos);
  ostr.close();
  return true;
}

/// to support process database
//: Binary write boxm2_event_trigger to stream
void vsl_b_write(vsl_b_ostream& /*os*/, betr_event_trigger const& /*bit_event_trigger*/) {}
//: Binary write betr_event_trigger pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, betr_event_trigger* const& /*ph*/) {}
//: Binary write betr_event_trigger smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, betr_event_trigger_sptr&) {}
//: Binary write betr_event_trigger smart pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, betr_event_trigger_sptr const&) {}

//: Binary load boxm event_trigger from stream.
void vsl_b_read(vsl_b_istream& /*is*/, betr_event_trigger& /*bit_event_trigger*/) {}
//: Binary load boxm event_trigger pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, betr_event_trigger* /*ph*/) {}
//: Binary load boxm event_trigger smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, betr_event_trigger_sptr&) {}
//: Binary load boxm event_trigger smart pointer from stream.
void vsl_b_read(vsl_b_istream& /*is*/, betr_event_trigger_sptr const&) {}
