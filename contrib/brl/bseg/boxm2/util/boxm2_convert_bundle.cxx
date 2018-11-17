#include "boxm2_convert_bundle.h"
#include "boxm2_point_util.h"
//:
// \file
#include <cassert>
#include <utility>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vidl/vidl_image_list_istream.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vsph/vsph_camera_bounds.h>
#include <vul/vul_file.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Main boxm2_convert_bundle function
//  Takes in bundle.out file and image directory that created img_dir
void boxm2_util_convert_bundle (std::string bundle_file,
                                std::string img_dir,
                                std::map<std::string, vpgl_perspective_camera<double>* >& cams,
                                vgl_box_3d<double>& bbox,
                                double& resolution)
{
  boxm2_convert_bundle b2s(std::move(bundle_file), std::move(img_dir));
  cams        = b2s.get_cams();
  bbox        = b2s.get_bbox();
  resolution  = b2s.get_resolution();
}

// reads bundler file and populates list of cameras, and a scene bounding box
boxm2_convert_bundle::boxm2_convert_bundle(const std::string& bundle_file, const std::string& img_dir)
{
  img_dir_ = img_dir;
  bundle_file_ = bundle_file;

  // open the bundler file
  std::ifstream bfile( bundle_file.c_str() );
  if (!bfile) {
    std::cout<<"boxm2_convert_bundle::Error Opening Bundler output file"<<std::endl;
    return;
  }
  // verify image dir
  if (!vul_file::is_directory(img_dir.c_str()))
  {
    std::cout<<"boxm2_convert_bundle::Image directory does not exist"<<std::endl;
    return;
  }
  //image list istream
  vidl_image_list_istream imgstream(img_dir_+"/*");
  if (!imgstream.is_open()) {
    std::cout<<"Invalid image stream"<<std::endl;
    return;
  }
  // get image size/principal point
  unsigned ni=imgstream.width();
  unsigned nj=imgstream.height();
  vgl_point_2d<double> ppoint((double)ni/2,(double)nj/2);

  //read cameras/points from bundler file
  unsigned num_cams=0, num_pts=0;
  this->read_nums(bfile, num_cams, num_pts);
  this->read_cameras(bfile, num_cams, ppoint);
  this->read_points(bfile, num_pts, ppoint);
  boxm2_point_util::calc_projection_error(cams_, bad_cams_, corrs_, view_error_map_, view_count_map_);

  //--------------------------------------------------------------------------
  // make sure the scene is axis aligned
  ////--------------------------------------------------------------------------
  if (!boxm2_point_util::axis_align_scene(corrs_,cams_))
    return;

  //------------------------------------------------------------------------
  // Filter out the cams with very high error
  //------------------------------------------------------------------------
  boxm2_point_util::report_error(view_error_map_, view_count_map_, bad_cams_, 2.5f);

  //------------------------------------------------------------------------
  // Save camera and corresponding image file
  //------------------------------------------------------------------------
  for (unsigned i = 0; i < num_cams; ++i) {
    if ( !bad_cams_.count(i) ) {
      imgstream.seek_frame(i);
      std::string path = imgstream.current_path();
      auto* cam = new CamType(cams_[i]);
      final_cams_[path] = cam;
      //std::cout<<"Final cam: "<<path<<std::endl;
    }
  }//end camera write

  //------------------------------------------------------------------------
  // Save calc bounding box
  //------------------------------------------------------------------------
  std::vector<vgl_point_3d<double> > pts_3d;
  vgl_box_3d<double> bounding_box;
  for (auto & corr : corrs_)
  {
    bounding_box.add(corr->world_pt());
    pts_3d.push_back(corr->world_pt());
  }

  // Dimensions of the World
  std::cout<<"Full Point Bounding Box "<<bounding_box<<std::endl;
  vgl_point_3d<double> c = centre(pts_3d);
  std::cout<<"Center of Gravity "<<c<<std::endl;
  vnl_double_3 sigma = boxm2_point_util::stddev(pts_3d);
  std::cout<<"Point stddev "<< sigma <<std::endl;

  //--------------------------------------------------------------------------
  // Define dimensions to be used for a boxm scene
  // Note: x-y dimensions are kind of a good approximation
  // the z-dimension however suffers because most points tend to be on the ground and the average miss represents points off the gound
  //--------------------------------------------------------------------------
  double minx=-3.0f*sigma[0], miny=-3.0f*sigma[1], minz=-1.0f*sigma[2];
  double maxx=3.0f*sigma[0], maxy=3.0f*sigma[1], maxz=6.0f*sigma[2];
  bbox_ = vgl_box_3d<double>(minx, miny, minz, maxx, maxy,maxz);

  //--------------------------------------------------------------------------
  // Determining the resolution of the cells
  //--------------------------------------------------------------------------
  int good_cam = 0;
  while ( bad_cams_.count(good_cam) > 0 ) good_cam++;
  std::cout<<"Determining resolution of cells with cam: "<< good_cam << std::endl;

  vgl_ray_3d<double> cone_axis;
  double cone_half_angle, solid_angle;
  vsph_camera_bounds::pixel_solid_angle(cams_[good_cam], ni/4, nj/4,cone_axis,cone_half_angle,solid_angle);
  vgl_point_3d<double> cc = cams_[good_cam].camera_center();
  resolution_ = 2*(cc-centre(pts_3d)).length()*cone_half_angle;
  std::cout<<"Resolution     "<<resolution_<<std::endl;
}
//------------------------------------------------------------------------
// reading the num cams and num points from bundler
//------------------------------------------------------------------------
bool boxm2_convert_bundle::read_nums(std::ifstream& bfile, unsigned& num_cams, unsigned& num_pts)
{
  //read bundler file header
  char buffer[1024];
  bfile.getline(buffer,1024);
  if (bfile.eof()) {
    std::cout<<"File Missing data"<<std::endl;
    return false;
  }
  // reading number of cameras and number of 3-d pts
  bfile>>num_cams>>num_pts;
  return true;
}

//------------------------------------------------------------------------
// reading the cameras from bundler
//------------------------------------------------------------------------
bool boxm2_convert_bundle::read_cameras(std::ifstream& bfile, unsigned num_cams, vgl_point_2d<double> ppoint)
{
  // read the cams from bundler and write it to a directory
  std::vector<vnl_matrix_fixed<double,3,3> > Rs;
  std::vector<vnl_vector_fixed<double,3> > Ts;

  double f,k1,k2;
  for (unsigned i=0;i<num_cams;++i)
  {
    vnl_matrix_fixed<double,3,3> R;
    vnl_vector_fixed<double,3> T;

    //grab cam info from file
    bfile>>f>>k1>>k2;
    bfile>>R>>T;

    // negating to convert bundlers camera facing towards -ve z to positive
    R(2,0) = -R(2,0); R(2,1) = -R(2,1); R(2,2) = -R(2,2);
    R(1,0) =  R(1,0); R(1,1) =  R(1,1); R(1,2) =  R(1,2);
    T[1]= T[1];
    T[2]= -T[2];
    vnl_vector_fixed<double,3> CC = - R.transpose()*T;
    vgl_point_3d<double> cc(CC[0],CC[1],CC[2]);
    vgl_rotation_3d<double> rot(R);
    Rs.push_back(R);
    Ts.push_back(T);
    if (f>0.0)
    {
      vpgl_calibration_matrix<double> K(f,ppoint) ;
      vpgl_perspective_camera<double> cam(K,cc,rot);
      cams_.push_back(cam);
    }
    else
    {
      vpgl_perspective_camera<double> cam;
      cams_.push_back(cam);
      bad_cams_.insert(i);
    }
  }
  return true;
}

//------------------------------------------------------------------------
// Read points into vector of bwm_video_corr_sptrs
//------------------------------------------------------------------------
bool boxm2_convert_bundle::read_points(std::ifstream& bfile, unsigned num_pts, vgl_point_2d<double> ppoint)
{
  for (unsigned i=0;i<num_pts;++i)
  {
    bwm_video_corr_sptr corr = new bwm_video_corr();
    double x,y,z;

    // read the 3-d point
    bfile>>x>>y>>z;
    corr->set_world_pt(vgl_point_3d<double>(x,y,z));
    vgl_homg_point_3d<double> homg_world_pt(corr->world_pt());

    // read the intenstiy but don't do anything with it right now.
    int r,g,b;
    bfile>>r>>g>>b;
    corr->set_intensity( vgl_vector_3d<int>(r,g,b) );

    //grab number of views that see this point
    unsigned num_views;
    bfile>>num_views;
    for (unsigned j=0;j<num_views;++j)
    {
      //grab the view number
      unsigned view_number, key_number;
      double img_x,img_y;
      bfile>>view_number>>key_number>>img_x>>img_y;
      img_x = img_x+ppoint.x();
      img_y = img_y+ppoint.y();

      //track correlations
      corr->add(view_number,vgl_point_2d<double>(img_x,img_y));
    }
    corrs_.push_back(corr);
  }
  return true;
}
