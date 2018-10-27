#include <boxm2/vecf/boxm2_vecf_estimate_camera_from_canthus.h>

bool boxm2_vecf_estimate_camera_from_canthus::estimate_camera(vgl_vector_2d<double>  /*t*/){
  std::map<std::string,vgl_point_2d<double> >::iterator it;
  std::string fields [] =  {"image_shape","left_iris_part_0","left_iris_part_1","right_iris_part_0","right_iris_part_1","nose_ridge_part_0","left_medial_canthus","right_medial_canthus","left_lateral_canthus","right_lateral_canthus"};
  std::vector<std::string> field_array;
  field_array.assign(fields,fields + sizeof(fields)/sizeof(fields[0]));
  unsigned count_missing = 0;
  for (const auto & i : field_array){
    it = this->dlib_part_map_.find(i);
    if (it == dlib_part_map_.end()){
      std::cout<<" Missing dlib part " << i << std::endl;
      count_missing++;
    }
  }

  if (count_missing != 0)
    return false;
  //all data is accounted for, normalize data;
  unsigned h = dlib_part_map_["image_shape"].y(); //image height
  unsigned w = dlib_part_map_["image_shape"].x(); //image width
  for (unsigned i = 1 ; i <field_array.size();i++){ // skip image shape
    double x = dlib_part_map_[field_array[i]].x();
    double y = dlib_part_map_[field_array[i]].y();
    dlib_part_map_[field_array[i]] =vgl_point_2d<double>(x, h - y);
  }

  vgl_point_2d<double> llc = dlib_part_map_["left_lateral_canthus"];
  vgl_point_2d<double> lmc = dlib_part_map_["left_medial_canthus"];

  vgl_point_2d<double> rlc = dlib_part_map_["right_lateral_canthus"];
  vgl_point_2d<double> rmc = dlib_part_map_["right_medial_canthus"];

  vgl_point_2d<double> l_i0 = dlib_part_map_["left_iris_part_0"];
  vgl_point_2d<double> l_i1 = dlib_part_map_["left_iris_part_1"];

  vgl_point_2d<double> r_i0 = dlib_part_map_["right_iris_part_0"];
  vgl_point_2d<double> r_i1 = dlib_part_map_["right_iris_part_1"];

  vgl_point_2d<double> nr_0 = dlib_part_map_["nose_ridge_part_0"];

#define sq(a) (a) * (a)
#define len(a,b) std::sqrt( sq(a.y() - b.y() ) + sq( a.x() - b.x() ) )
  double left_iris_radius = len(l_i0,l_i1)/2.0;
  double right_iris_radius = len(r_i0,r_i1)/2.0;
  std::cout<<" iris radii: "<<left_iris_radius<<" "<<right_iris_radius<<std::endl;
  double max_iris_radius = left_iris_radius > right_iris_radius ? left_iris_radius : right_iris_radius;
  scale_ =  iris_nominal_ / max_iris_radius;
  double  s =  1./ scale_;
  phi_ = atan( (llc.y() - rlc.y()) / (llc.x() - rlc.x()));
  double l_cc = len(llc,rlc);
  std::cout<<"l_cc "<< l_cc<<" , "<<std::sqrt( sq( l_cc /( this->canthus_line_length_ * s) ) - sq( sin(phi_) ) ) <<std::endl;
  double cos_psi = ( 1./cos(phi_) * std::sqrt( sq( l_cc /( this->canthus_line_length_ * s) ) - sq( sin(phi_) ) ) );
    psi_ = acos(cos_psi) ;
    std::cout<<" cos of phi and psi is "<< cos(phi_)<<" "<<cos_psi<<std::endl;
  vgl_point_2d<double> mid        = vgl_point_2d<double>((llc.x()+rlc.x()) / 2 , (llc.y()+rlc.y()) / 2);
  vgl_point_2d<double> nose_ridge = nr_0;

  if (nose_ridge.x() < mid.x() )
    psi_ *= -1; //left o

  // set the canthus mid point to map in the origin of the frontal plane
  std::cout<<w/2<<" "<<h/2<<std::endl;
  std::cout<<mid.x()<<" "<<mid.y()<<std::endl;
  double t_x = w/2 - s * cos(psi_) * cos(phi_) * mid.x() + s * cos(psi_) * sin(phi_) * mid.y() - z0_ * sin(psi_);
  double t_y = h/2 - s * sin(phi_) * mid.x()             - s * cos(phi_) * mid.y();
  t_.set(t_x,t_y);
  //t_ = t;

  double arr[] = { s  * cos(psi_) * cos(phi_), -s  * cos(psi_) * sin(phi_), t_.x() + z0_ * sin(psi_),
                   s  * sin(phi_),              s  * cos(phi_)            , t_.y(),
                   0             ,              0                         , 1};

  std::cout<<"roll : "<<phi_<< " yaw: "<<psi_<<" scale "<<scale_<<std::endl;
  H_.set(arr); // homography from raw image plane to frontal plane (0,0,z0_)
#undef sq
#undef len
  return true;

}

bool boxm2_vecf_estimate_camera_from_canthus::parse_files(std::string& left_dlib_path,std::string& right_dlib_path, std::string& alfw_path){

  std::ifstream rfile(right_dlib_path.c_str());
  std::ifstream lfile(left_dlib_path.c_str());
  std::ifstream alfw_file(alfw_path.c_str());


  if(!lfile){
    std::cout<<" Could not open "<<left_dlib_path <<std::endl;
    return -1;
  }

  if(!rfile){
    std::cout<<" Could not open "<<right_dlib_path<<std::endl;
    return -1;
  }

  if(!alfw_file){
    std::cout<<" Could not open "<<alfw_path<<std::endl;
    return -1;
  }

  std::vector< vgl_point_2d<double> > pts_l;
  while(lfile){
    double x,y;
    lfile >> x;
    lfile >> y;
    pts_l.emplace_back(x ,y);
  }
  if (pts_l.size()!= this->n_dlib_orbit_parts_){
    std::cout<< " file has "<<pts_l.size()<<" orbit parts instead of the required "<<this->n_dlib_orbit_parts_<<std::endl;
    return false;
  }
  this->add_dlib_part(pts_l[6], "left_iris_part_0");
  this->add_dlib_part(pts_l[7], "left_iris_part_1");
  this->add_dlib_part(pts_l[8], "left_lateral_canthus");
  this->add_dlib_part(pts_l[9], "left_medial_canthus");


  std::vector< vgl_point_2d<double> > pts_r;
  while(rfile){
    double x,y;
    rfile >> x;
    rfile >> y;
    pts_r.emplace_back(x ,y);
  }
  if (pts_r.size()!= this->n_dlib_orbit_parts_){
    std::cout<< " file has "<<pts_r.size()<<" orbit parts instead of the required "<<this->n_dlib_orbit_parts_<<std::endl;
    return false;
  }
  this->add_dlib_part(pts_r[6], "right_iris_part_0");
  this->add_dlib_part(pts_r[7], "right_iris_part_1");
  this->add_dlib_part(pts_r[8], "right_lateral_canthus");
  this->add_dlib_part(pts_r[9], "right_medial_canthus");

  std::vector< vgl_point_2d<double> > pts_alfw;
  while(alfw_file){
    double x,y;
    alfw_file >> x;
    alfw_file >> y;
    pts_alfw.emplace_back(x ,y);
  }
  if (pts_alfw.size()!= this->n_dlib_alfw_landmarks_){
    std::cout<< " file has "<<pts_alfw.size()<<" aflw parts instead of the required "<<this->n_dlib_alfw_landmarks_ <<std::endl;
    return false;
  }
  this->add_dlib_part(pts_alfw[6], "nose_ridge_part_0");

  return true;
}
