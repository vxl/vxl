#include <iostream>
#include <cstdlib>
#include "boxm2_point_util.h"
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/algo/vgl_fit_plane_3d.h>
#include <vgl/vgl_distance.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_quaternion.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

bool boxm2_point_util::fit_plane_ransac(std::vector<vgl_homg_point_3d<double> > & points, vgl_homg_plane_3d<double>  & plane)
{
  unsigned int nchoose=3;
  unsigned int nsize=points.size();
  unsigned int max_its = 1000;
  double err=10.0;
  double inlier_dist = 0.1;
  std::vector<int> best_inliers;
  for (unsigned i=0;i<max_its;++i)
  {
    std::cout << '.' << std::flush;
    std::vector<vgl_homg_point_3d<double> > subset;
    std::vector<int> inliers;
    for (unsigned j=0;j<nchoose;++j)
      subset.push_back(points[std::rand()%nsize]);
    std::cout<<subset.size();std::cout.flush();
    vgl_fit_plane_3d<double> fit_plane(subset);
    if (fit_plane.fit(err, &std::cerr))
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

  for (int best_inlier : best_inliers)
  {
    fit_plane_inliers.add_point(points[best_inlier]);
  }
  std::cout<<"Inliers "<<best_inliers.size()<<std::endl;
  if (fit_plane_inliers.fit(23.0, &std::cerr))
  {
    plane=fit_plane_inliers.get_plane();
    return true;
  }
  else
    return false;
}

bool boxm2_point_util::axis_align_scene(std::vector<bwm_video_corr_sptr> & corrs,
                                        std::vector<vpgl_perspective_camera<double> > & cams)
{
  std::vector<vgl_homg_point_3d<double> > points;
  for (auto & corr : corrs)
  {
    vgl_homg_point_3d<double> homg_world_pt(corr->world_pt());
    points.push_back(homg_world_pt);
  }

#if 0
  double max_dist = 0.0f;
  for (int i=0; i<points.size(); ++i)
    for (int j=0; j<points.size(); ++j)
      if ( vgl_distance(points[i], points[j]) > max_dist)
        max_dist = vgl_distance(points[i], points[j]);
#endif

#if 0//Either fit using ransac, or just minimum squared error
  //fit plane ransac
  vgl_homg_plane_3d<double> plane;
  fit_plane_ransac(points, plane);
#else
  // fit the plane
  vgl_fit_plane_3d<double> fit_plane(points);
  if (!fit_plane.fit(1e6, &std::cerr))
    return false;
  vgl_homg_plane_3d<double> plane=fit_plane.get_plane();
#endif

  //get plane rotation
  vgl_rotation_3d<double> rot_scene(plane.normal(),vgl_vector_3d<double>(0,0,1));

  double sumx=0,sumy=0,sumz=0;
  for (auto & corr : corrs)
  {
    vgl_homg_point_3d<double> p(corr->world_pt());
    vgl_homg_point_3d<double> pnew=rot_scene*p;
    if (!pnew.ideal())
    {
      vgl_point_3d<double> pnew_nonhomg(pnew.x()/pnew.w(),pnew.y()/pnew.w(),pnew.z()/pnew.w());
      sumx+=pnew_nonhomg.x();sumy+=pnew_nonhomg.y();sumz+=pnew_nonhomg.z();
      corr->set_world_pt(pnew_nonhomg);
    }
  }
  vgl_point_3d<double> center(sumx/corrs.size(),sumy/corrs.size(),sumz/corrs.size());
  vnl_vector_fixed<double,3> tr(center.x(),center.y(),center.z());
  std::vector<vgl_homg_point_3d<double> > xformed_points;
  for (auto & corr : corrs)
  {
    vgl_point_3d<double> p(corr->world_pt());
    corr->set_world_pt(vgl_point_3d<double>(p.x()-center.x(),p.y()-center.y(),p.z()-center.z()));
    xformed_points.emplace_back(p.x()-center.x(),p.y()-center.y(),p.z()-center.z());
  }
  std::vector<vpgl_perspective_camera<double> > new_cams;
  unsigned int up=0;
  for (unsigned i=0;i<cams.size();++i)
  {
    // new rotation
    vgl_rotation_3d<double> rot_cami=cams[i].get_rotation()*rot_scene.inverse();

    // new translation
    vnl_vector_fixed<double,3> newtranslation=rot_cami.as_matrix()*tr;
    vgl_vector_3d<double> translation_vec(newtranslation[0],newtranslation[1],newtranslation[2]);
    vgl_vector_3d<double> tr_cami=cams[i].get_translation()+translation_vec;


    new_cams.emplace_back(cams[i].get_calibration(),rot_cami,tr_cami);
    if (new_cams[i].get_camera_center().z()>0)
      ++up;
  }

  if (2*up<cams.size())// flip the world
  {
    vnl_quaternion<double> q(vnl_math::pi,0,0);
    vgl_rotation_3d<double> rotx_pi(q);
    for (auto & corr : corrs)
    {
      vgl_homg_point_3d<double> p(corr->world_pt());
      vgl_homg_point_3d<double> pnew=rotx_pi*p;
      if (!pnew.ideal())
      {
        vgl_point_3d<double> pnew_nonhomg(pnew.x()/pnew.w(),pnew.y()/pnew.w(),pnew.z()/pnew.w());
        corr->set_world_pt(pnew_nonhomg);
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

  //vgl_fit_plane_3d<double> fit_plane1(xformed_points);
  //if (!fit_plane1.fit(1e6, &std::cerr))
      //return false;

  //vgl_homg_plane_3d<double> plane1=fit_plane1.get_plane();

  return true;
}

vnl_double_3 boxm2_point_util::stddev( std::vector<vgl_point_3d<double> > const& v)
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
    stddev[i] = std::sqrt(stddev[i]/(n-1));

  return stddev;
}


//------------------------------------------------------------------------
// Calc projection error
// read the correspondence and 3-d points
//------------------------------------------------------------------------
void boxm2_point_util::calc_projection_error(std::vector<vpgl_perspective_camera<double> >& cams,
                                             std::set<int>&                    bad_cams,
                                             std::vector<bwm_video_corr_sptr>& corrs,
                                             std::map<unsigned,double>&        view_error_map,
                                             std::map<unsigned,unsigned>&      view_count_map )
{
  double err=0;
  double cnt=0;
  for (const auto& corr : corrs)
  {
    vgl_homg_point_3d<double> wpt(corr->world_pt());

    //grab number of views that see this point
    std::map<unsigned, vgl_point_2d<double> >& matches = corr->matches();
    std::map<unsigned, vgl_point_2d<double> >::iterator iter;
    for (iter = matches.begin(); iter != matches.end(); ++iter)
    {
      unsigned view_number = iter->first;
      vgl_point_2d<double> xy = iter->second;
      double img_x = xy.x(), img_y = xy.y();

      //calc error for this point
      if (cams[view_number].is_behind_camera(wpt)) {
        std::cout<<"Bad camera "<<view_number<<" is behind world point"<<std::endl;
        bad_cams.insert(view_number);
      }
      else
      {
        //project x,y,z onto image plane, calc rms error
        double u=0,v=0;
        cams[view_number].project(wpt.x(), wpt.y(), wpt.z(),u,v);
        double rms=std::sqrt((u-img_x)*(u-img_x)+(v-img_y)*(v-img_y));
        err+=rms;++cnt;

        //store view error and counts in a map
        auto ve_itr = view_error_map.find(view_number);
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

void boxm2_point_util::report_error(std::map<unsigned,double>&   view_error_map,
                                    std::map<unsigned,unsigned>& view_count_map,
                                    std::set<int>&               bad_cams,
                                    float                       filter_thresh)
{
  std::cout<<"Projection error per camera:"<<std::endl;
  float error  = 0.0;
  float counts = 0.0;
  auto ve_itr = view_error_map.begin(),
                                     ve_end = view_error_map.end();
  for (;ve_itr!=ve_end;++ve_itr) {
//#ifdef DEBUG
    unsigned cam=ve_itr->first;
    double   err=ve_itr->second;
    unsigned cnt=view_count_map[cam];
    std::cout<<"   error for camera_"<<cam<<": "<<err/cnt<<std::endl;
//#endif
    double terror = ve_itr->second/view_count_map[ve_itr->first];
    if (terror > filter_thresh) {
      std::cout<<"Bad camera "<<ve_itr->first<<" has error "<<terror<<std::endl;
      bad_cams.insert(ve_itr->first);
    }
    else {
      error  += ve_itr->second;
      counts += view_count_map[ve_itr->first];
    }
  }
  std::cout<<"Filtered Avg Projection Error "<<error/(float)counts<<std::endl;
}
