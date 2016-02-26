#include <boxm2/vecf/boxm2_vecf_estimate_camera_from_canthus.h>

bool boxm2_vecf_estimate_camera_from_canthus::estimate_camera(vgl_vector_2d<double> t){
  vcl_map<vcl_string,vgl_point_2d<double> >::iterator it;
  vcl_vector<vcl_string> field_array {"image_shape","left_iris_part_0","left_iris_part_1","right_iris_part_0","right_iris_part_1","nose_ridge_part_0","left_medial_canthus","right_medial_canthus","left_lateral_canthus","right_lateral_canthus"};
  unsigned count =0;
  for (unsigned i = 0 ; i <field_array.size();i++){
    it = this->dlib_part_map_.find(field_array[i]);
    unsigned count = 0;
    if (it == dlib_part_map_.end()){
      vcl_cout<<" Missing dlib part " << field_array[i] << vcl_endl;
      count++;
    }
  }

  if (count != 0)
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
#define len(a,b) vcl_sqrt( sq(a.y() - b.y() ) + sq( a.x() - b.x() ) )
  double left_iris_radius = len(l_i0,l_i1)/2.0;
  double right_iris_radius = len(r_i0,r_i1)/2.0;
  vcl_cout<<" iris radii: "<<left_iris_radius<<" "<<right_iris_radius<<vcl_endl;
  double max_iris_radius = left_iris_radius > right_iris_radius ? left_iris_radius : right_iris_radius;
  scale_ =  iris_nominal_ / max_iris_radius;
  double  s =  1./ scale_;
  phi_ = atan( (llc.y() - rlc.y()) / (llc.x() - rlc.x()));
  double l_cc = len(llc,rlc);
  vcl_cout<<"l_cc "<< l_cc<<" , "<<vcl_sqrt( sq( l_cc /( this->canthus_line_length_ * s) ) - sq( sin(phi_) ) ) <<vcl_endl;
  psi_ = acos( 1./cos(phi_) * vcl_sqrt( sq( l_cc /( this->canthus_line_length_ * s) ) - sq( sin(phi_) ) ) ) ;
  vgl_point_2d<double> mid        = vgl_point_2d<double>((llc.x()+rlc.x()) / 2 , (llc.y()+rlc.y()) / 2);
  vgl_point_2d<double> nose_ridge = nr_0;

  if (nose_ridge.x() < mid.x() )
    psi_ *= -1; //left o

  // set the canthus mid point to map in the origin of the frontal plane
  vcl_cout<<w/2<<" "<<h/2<<vcl_endl;
  vcl_cout<<mid.x()<<" "<<mid.y()<<vcl_endl;
  double t_x = w/2 - s * cos(psi_) * cos(phi_) * mid.x() + s * cos(psi_) * sin(phi_) * mid.y() - z0_ * sin(psi_);
  double t_y = h/2 - s * sin(phi_) * mid.x()             - s * cos(phi_) * mid.y();
  t_.set(t_x,t_y);
  //t_ = t;

  double arr[] = { s  * cos(psi_) * cos(phi_), -s  * cos(psi_) * sin(phi_), t_.x() + z0_ * sin(psi_),
                   s  * sin(phi_),              s  * cos(phi_)            , t_.y(),
                   0             ,              0                         , 1};

  vcl_cout<<"roll : "<<phi_<< " yaw: "<<psi_<<" scale "<<scale_<<vcl_endl;
  H_.set(arr); // homography from raw image plane to frontal plane (0,0,z0_)
#undef sq
#undef len
  return true;

}

bool boxm2_vecf_estimate_camera_from_canthus::parse_files(vcl_string& left_dlib_path,vcl_string& right_dlib_path, vcl_string& alfw_path){

  vcl_ifstream rfile(right_dlib_path);
  vcl_ifstream lfile(left_dlib_path);
  vcl_ifstream alfw_file(alfw_path);


  if(!lfile){
    vcl_cout<<" Could not open "<<left_dlib_path <<vcl_endl;
    return -1;
  }

  if(!rfile){
    vcl_cout<<" Could not open "<<right_dlib_path<<vcl_endl;
    return -1;
  }

  if(!alfw_file){
    vcl_cout<<" Could not open "<<alfw_file<<vcl_endl;
    return -1;
  }

  vcl_vector< vgl_point_2d<double> > pts_l;
  while(lfile){
    double x,y;
    lfile >> x;
    lfile >> y;
    pts_l.push_back(vgl_point_2d<double>(x ,y));
  }
  if (pts_l.size()!= this->n_dlib_orbit_parts_){
    vcl_cout<< " file has "<<pts_l.size()<<" orbit parts instead of the required "<<this->n_dlib_orbit_parts_<<vcl_endl;
    return false;
  }
  this->add_dlib_part(pts_l[6], "left_iris_part_0");
  this->add_dlib_part(pts_l[7], "left_iris_part_1");
  this->add_dlib_part(pts_l[8], "left_lateral_canthus");
  this->add_dlib_part(pts_l[9], "left_medial_canthus");


  vcl_vector< vgl_point_2d<double> > pts_r;
  while(rfile){
    double x,y;
    rfile >> x;
    rfile >> y;
    pts_r.push_back(vgl_point_2d<double>(x ,y));
  }
  if (pts_r.size()!= this->n_dlib_orbit_parts_){
    vcl_cout<< " file has "<<pts_r.size()<<" orbit parts instead of the required "<<this->n_dlib_orbit_parts_<<vcl_endl;
    return false;
  }
  this->add_dlib_part(pts_r[6], "right_iris_part_0");
  this->add_dlib_part(pts_r[7], "right_iris_part_1");
  this->add_dlib_part(pts_r[8], "right_lateral_canthus");
  this->add_dlib_part(pts_r[9], "right_medial_canthus");

  vcl_vector< vgl_point_2d<double> > pts_alfw;
  while(alfw_file){
    double x,y;
    alfw_file >> x;
    alfw_file >> y;
    pts_alfw.push_back(vgl_point_2d<double>(x ,y));
  }
  if (pts_alfw.size()!= this->n_dlib_alfw_landmarks_){
    vcl_cout<< " file has "<<pts_alfw.size()<<" aflw parts instead of the required "<<this->n_dlib_alfw_landmarks_ <<vcl_endl;
    return false;
  }
  this->add_dlib_part(pts_alfw[6], "nose_ridge_part_0");

  return true;
}
