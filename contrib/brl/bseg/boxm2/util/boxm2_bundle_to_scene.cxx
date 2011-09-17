#include "boxm2_bundle_to_scene.h"
//:
// \file
#include <vpgl/algo/vpgl_camera_bounds.h>
#include <vgl/algo/vgl_orient_box_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/algo/vnl_cholesky.h>
#include <vnl/algo/vnl_svd_fixed.h>
#include <vcl_cassert.h>

//: Main boxm2_bundle_to_scene function
//  Takes in bundle.out file and image directory that created img_dir
void boxm2_util_bundle_to_scene(vcl_string bundle_file,
                                vcl_string img_dir,
                                vcl_map<vcl_string, vpgl_perspective_camera<double>* >& cams,
                                vgl_box_3d<double>& bbox,
                                double& resolution)
{
  boxm2_bundle_to_scene b2s(bundle_file, img_dir);
  cams        = b2s.get_cams();
  bbox        = b2s.get_bbox();
  resolution  = b2s.get_resolution();
}

// reads bundler file and populates list of cameras, and a scene bounding box
boxm2_bundle_to_scene::boxm2_bundle_to_scene(vcl_string bundle_file, vcl_string img_dir)
{
  img_dir_ = img_dir;
  bundle_file_ = bundle_file;

  // open the bundler file
  vcl_ifstream bfile( bundle_file.c_str() );
  if (!bfile) {
    vcl_cout<<"boxm2_bundle_to_scene::Error Opening Bundler output file"<<vcl_endl;
    return;
  }
  // verify image dir
  if (!vul_file::is_directory(img_dir.c_str()))
  {
    vcl_cout<<"boxm2_bundle_to_scene::Image directory does not exist"<<vcl_endl;
    return;
  }
  //image list istream
  vidl_image_list_istream imgstream(img_dir_+"/*");
  if (!imgstream.is_open()) {
    vcl_cout<<"Invalid image stream"<<vcl_endl;
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
  this->calc_projection_error(cams_, bad_cams_, corrs_, view_error_map_, view_count_map_);

  //--------------------------------------------------------------------------
  // make sure the scene is axis aligned
  ////--------------------------------------------------------------------------
  if (!axis_align_scene(corrs_,cams_))
    return;

  //------------------------------------------------------------------------
  // Filter out the cams with very high error
  //------------------------------------------------------------------------
  report_error(view_error_map_, view_count_map_, bad_cams_, 1.5f);

  //------------------------------------------------------------------------
  // Save camera and corresponding image file
  //------------------------------------------------------------------------
  for (unsigned i = 0; i < num_cams; ++i) {
    if ( !bad_cams_.count(i) ) {
      imgstream.seek_frame(i);
      vcl_string path = imgstream.current_path();
      CamType* cam = new CamType(cams_[i]);
      final_cams_[path] = cam;
      //vcl_cout<<"Final cam: "<<path<<vcl_endl;
    }
  }//end camera write

  //------------------------------------------------------------------------
  // Save calc bounding box
  //------------------------------------------------------------------------
  vcl_vector<vgl_point_3d<double> > pts_3d;
  vgl_box_3d<double> bounding_box;
  for (unsigned i=0; i<corrs_.size(); ++i)
  {
    bounding_box.add(corrs_[i]->world_pt());
    pts_3d.push_back(corrs_[i]->world_pt());
  }

  // Dimensions of the World
  vcl_cout<<"Full Point Bounding Box "<<bounding_box<<vcl_endl;
  vgl_point_3d<double> c = centre(pts_3d);
  vcl_cout<<"Center of Gravity "<<centre(pts_3d)<<vcl_endl;
  vnl_vector_fixed<double,3> sigma = stddev(pts_3d);
  vcl_cout<<"Point stddev "<< sigma <<vcl_endl;

  //--------------------------------------------------------------------------
  // Define dimensions to be used for a boxm scene
  // Note: x-y dimensions are kind of a good approximation
  // the z-dimension however suffers because most points tend to be on the ground and the average miss represents points off the gound
  //--------------------------------------------------------------------------
  double minx=-3.0f*sigma[0], miny=-3.0f*sigma[1], minz=-1.0f*sigma[2];
  double maxx=3.0f*sigma[0], maxy=3.0f*sigma[1], maxz=5.0f*sigma[2];
  bbox_ = vgl_box_3d<double>(minx, miny, minz, maxx, maxy,maxz);

  //--------------------------------------------------------------------------
  // Determining the resolution of the cells
  //--------------------------------------------------------------------------
  int good_cam = 0;
  while ( bad_cams_.count(good_cam) > 0 ) good_cam++;
  vcl_cout<<"Determining resolution of cells with cam: "<< good_cam << vcl_endl;

  vgl_ray_3d<double> cone_axis;
  double cone_half_angle, solid_angle;
  vpgl_camera_bounds::pixel_solid_angle(cams_[good_cam], ni/4, nj/4,cone_axis,cone_half_angle,solid_angle);
  vgl_point_3d<double> cc = cams_[good_cam].camera_center();
  resolution_ = 2*(cc-centre(pts_3d)).length()*cone_half_angle;
  vcl_cout<<"Resolution     "<<resolution_<<vcl_endl;
}
//------------------------------------------------------------------------
// reading the num cams and num points from bundler
//------------------------------------------------------------------------
bool boxm2_bundle_to_scene::read_nums(vcl_ifstream& bfile, unsigned& num_cams, unsigned& num_pts)
{
  //read bundler file header
  char buffer[1024];
  bfile.getline(buffer,1024);
  if (bfile.eof()) {
    vcl_cout<<"File Missing data"<<vcl_endl;
    return false;
  }
  // reading number of cameras and number of 3-d pts
  bfile>>num_cams>>num_pts;
  return true;
}

//------------------------------------------------------------------------
// reading the cameras from bundler
//------------------------------------------------------------------------
bool boxm2_bundle_to_scene::read_cameras(vcl_ifstream& bfile, unsigned num_cams, vgl_point_2d<double> ppoint)
{
  // read the cams from bundler and write it to a directory
  vcl_vector<vnl_matrix_fixed<double,3,3> > Rs;
  vcl_vector<vnl_vector_fixed<double,3> > Ts;

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
bool boxm2_bundle_to_scene::read_points(vcl_ifstream& bfile, unsigned num_pts, vgl_point_2d<double> ppoint)
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

bool boxm2_bundle_to_scene::fit_plane_ransac(vcl_vector<vgl_homg_point_3d<double> > & points, vgl_homg_plane_3d<double>  & plane)
{
  unsigned int nchoose=3;
  unsigned int nsize=points.size();
  unsigned int max_its = 500;
  double err=10.0;
  double inlier_dist = 0.01;
  vcl_vector<int> best_inliers;
  for (unsigned i=0;i<max_its;++i)
  {
    vcl_cout << '.';
    vcl_vector<vgl_homg_point_3d<double> > subset;
    vcl_vector<int> inliers;
    for (unsigned j=0;j<nchoose;++j)
      subset.push_back(points[vcl_rand()%nsize]);
    vcl_cout<<subset.size();vcl_cout.flush();
    vgl_fit_plane_3d<double> fit_plane(subset);
    if (fit_plane.fit(err, &vcl_cerr))
    {
      vgl_homg_plane_3d<double> plane=fit_plane.get_plane();
      for (unsigned j=0;j<nsize;++j)
      {
        double dist=vgl_distance<double>(points[j],plane);
        if (dist*dist<inlier_dist*inlier_dist)
        {
          inliers.push_back(j);
        }
      }
      if (inliers.size()>best_inliers.size())
        best_inliers=inliers;
    }
  }
  vgl_fit_plane_3d<double> fit_plane_inliers;

  for (unsigned i=0;i<best_inliers.size();++i)
  {
    fit_plane_inliers.add_point(points[best_inliers[i]]);
  }
  vcl_cout<<"Inliers "<<best_inliers.size()<<vcl_endl;
  if (fit_plane_inliers.fit(23.0, &vcl_cerr))
  {
    plane=fit_plane_inliers.get_plane();
    return true;
  }
  else
    return false;
}

bool boxm2_bundle_to_scene::axis_align_scene(vcl_vector<bwm_video_corr_sptr> & corrs,
                                             vcl_vector<vpgl_perspective_camera<double> > & cams)
{
  vcl_vector<vgl_homg_point_3d<double> > points;
  for (unsigned i=0;i<corrs.size();++i)
  {
    vgl_homg_point_3d<double> homg_world_pt(corrs[i]->world_pt());
    points.push_back(homg_world_pt);
  }

  // fit the plane
  vgl_fit_plane_3d<double> fit_plane(points);
  if (!fit_plane.fit(1e6, &vcl_cerr))
    return false;

  vgl_homg_plane_3d<double> plane=fit_plane.get_plane();
  vgl_rotation_3d<double> rot_scene(plane.normal(),vgl_vector_3d<double>(0,0,1));

  double sumx=0,sumy=0,sumz=0;
  for (unsigned i=0;i<corrs.size();++i)
  {
    vgl_homg_point_3d<double> p(corrs[i]->world_pt());
    vgl_homg_point_3d<double> pnew=rot_scene*p;
    if (!pnew.ideal())
    {
      vgl_point_3d<double> pnew_nonhomg(pnew.x()/pnew.w(),pnew.y()/pnew.w(),pnew.z()/pnew.w());
      sumx+=pnew_nonhomg.x();sumy+=pnew_nonhomg.y();sumz+=pnew_nonhomg.z();
      corrs[i]->set_world_pt(pnew_nonhomg);
    }
  }
  vgl_point_3d<double> center(sumx/corrs.size(),sumy/corrs.size(),sumz/corrs.size());
  vnl_vector_fixed<double,3> tr(center.x(),center.y(),center.z());
  vcl_vector<vgl_homg_point_3d<double> > xformed_points;
  for (unsigned i=0;i<corrs.size();++i)
  {
    vgl_point_3d<double> p(corrs[i]->world_pt());
    corrs[i]->set_world_pt(vgl_point_3d<double>(p.x()-center.x(),p.y()-center.y(),p.z()-center.z()));
    xformed_points.push_back(vgl_homg_point_3d<double>(p.x()-center.x(),p.y()-center.y(),p.z()-center.z()));
  }
  vcl_vector<vpgl_perspective_camera<double> > new_cams;
  unsigned int up=0;
  for (unsigned i=0;i<cams.size();++i)
  {
    // new rotation
    vgl_rotation_3d<double> rot_cami=cams[i].get_rotation()*rot_scene.inverse();

    // new translation
    vnl_vector_fixed<double,3> newtranslation=rot_cami.as_matrix()*tr;
    vgl_vector_3d<double> translation_vec(newtranslation[0],newtranslation[1],newtranslation[2]);
    vgl_vector_3d<double> tr_cami=cams[i].get_translation()+translation_vec;

    new_cams.push_back(vpgl_perspective_camera<double>(cams[i].get_calibration(),rot_cami,tr_cami));
    if (new_cams[i].get_camera_center().z()>0)
      ++up;
  }

  if (2*up<cams.size())// flip the world
  {
    vnl_quaternion<double> q(vnl_math::pi,0,0);
    vgl_rotation_3d<double> rotx_pi(q);
    for (unsigned i=0;i<corrs.size();++i)
    {
      vgl_homg_point_3d<double> p(corrs[i]->world_pt());
      vgl_homg_point_3d<double> pnew=rotx_pi*p;
      if (!pnew.ideal())
      {
        vgl_point_3d<double> pnew_nonhomg(pnew.x()/pnew.w(),pnew.y()/pnew.w(),pnew.z()/pnew.w());
        corrs[i]->set_world_pt(pnew_nonhomg);
      }
    }
    for (unsigned i=0;i<cams.size();++i)
    {
      // new rotation
      vgl_rotation_3d<double> rot_cami=new_cams[i].get_rotation()*rotx_pi.inverse();
      new_cams[i]=vpgl_perspective_camera<double>(new_cams[i].get_calibration(),rot_cami,new_cams[i].get_translation());
    }
  }
  for (unsigned i=0;i<cams.size();++i)
    cams[i]=new_cams[i];

  vgl_fit_plane_3d<double> fit_plane1(xformed_points);
  if (!fit_plane1.fit(1e6, &vcl_cerr))
      return false;

  vgl_homg_plane_3d<double> plane1=fit_plane1.get_plane();

  return true;
}

vnl_double_3 boxm2_bundle_to_scene::stddev( vcl_vector<vgl_point_3d<double> > const& v)
{
  unsigned n = v.size();
  assert(n>0);
  vnl_vector_fixed<double,3> m(0.0f), stddev(0.0f);

  for (unsigned i = 0; i < n; ++i)
  {
    m[0]+=v[i].x();
    m[1]+=v[i].y();
    m[2]+=v[i].z();
  }
  for (unsigned i = 0; i < 3; ++i)
    m[i]/=n;

  for (unsigned i = 0; i < n; ++i)
  {
    stddev[0] += (v[i].x()-m[0])*(v[i].x()-m[0]);
    stddev[1] += (v[i].y()-m[1])*(v[i].y()-m[1]);
    stddev[2] += (v[i].z()-m[2])*(v[i].z()-m[2]);
  }

  for (unsigned i = 0; i < 3; ++i)
    stddev[i] = vcl_sqrt(stddev[i]/(n-1));

  return stddev;
}


//------------------------------------------------------------------------
// Calc projection error
// read the correspondence and 3-d points
//------------------------------------------------------------------------
void boxm2_bundle_to_scene::calc_projection_error( vcl_vector<vpgl_perspective_camera<double> >& cams,
                                                   vcl_set<int>&                    bad_cams,
                                                   vcl_vector<bwm_video_corr_sptr>& corrs,
                                                   vcl_map<unsigned,double>&        view_error_map,
                                                   vcl_map<unsigned,unsigned>&      view_count_map )
{
  double err=0;
  double cnt=0;
  for (unsigned i=0;i<corrs.size();++i)
  {
    bwm_video_corr_sptr corr = corrs[i];
    vgl_homg_point_3d<double> wpt(corr->world_pt());

    //grab number of views that see this point
    vcl_map<unsigned, vgl_point_2d<double> >& matches = corr->matches();
    vcl_map<unsigned, vgl_point_2d<double> >::iterator iter;
    for (iter = matches.begin(); iter != matches.end(); ++iter)
    {
      unsigned view_number = iter->first;
      vgl_point_2d<double> xy = iter->second;
      double img_x = xy.x(), img_y = xy.y();

      //calc error for this point
      if (cams[view_number].is_behind_camera(wpt)) {
        bad_cams.insert(view_number);
      }
      else
      {
        //project x,y,z onto image plane, calc rms error
        double u=0,v=0;
        cams[view_number].project(wpt.x(), wpt.y(), wpt.z(),u,v);
        double rms=vcl_sqrt((u-img_x)*(u-img_x)+(v-img_y)*(v-img_y));
        err+=rms;++cnt;

        //store view error and counts in a map
        vcl_map<unsigned,double>::iterator ve_itr = view_error_map.find(view_number);
        if (ve_itr == view_error_map.end())
        {
          view_error_map[view_number]=rms;
          view_count_map[view_number]=1;
        }
        else
        {
          view_error_map[view_number]+=rms;
          view_count_map[view_number]+=1;
        }
      }
    }
  }
}

void boxm2_bundle_to_scene::report_error(vcl_map<unsigned,double>&   view_error_map,
                                         vcl_map<unsigned,unsigned>& view_count_map,
                                         vcl_set<int>&               bad_cams,
                                         float                       filter_thresh)
{
  vcl_cout<<"Projection error per camera:"<<vcl_endl;
  float error  = 0.0;
  float counts = 0.0;
  vcl_map<unsigned,double>::iterator ve_itr = view_error_map.begin(),
                                     ve_end = view_error_map.end();
  for (;ve_itr!=ve_end;++ve_itr) {
    unsigned cam=ve_itr->first;
    unsigned cnt=view_count_map[cam];
#ifdef DEBUG
    double   err=ve_itr->second;
    vcl_cout<<"   error for camera_"<<cam<<": "<<err/cnt<<vcl_endl;
#endif
    if (ve_itr->second/view_count_map[ve_itr->first] > filter_thresh) {
      bad_cams.insert(ve_itr->first);
    }
    else {
      error  += ve_itr->second;
      counts += view_count_map[ve_itr->first];
    }
  }
  vcl_cout<<"Filtered Avg Projection Error "<<error/counts<<vcl_endl;
}

