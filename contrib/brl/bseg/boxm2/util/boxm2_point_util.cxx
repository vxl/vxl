#include "boxm2_point_util.h"
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/algo/vgl_fit_plane_3d.h>
#include <vgl/vgl_distance.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_quaternion.h>
#include <vcl_cstdlib.h> // for std::rand()
#include <vcl_cassert.h>

bool boxm2_point_util::fit_plane_ransac(vcl_vector<vgl_homg_point_3d<double> > & points, vgl_homg_plane_3d<double>  & plane)
{
  unsigned int nchoose=3;
  unsigned int nsize=points.size();
  unsigned int max_its = 1000;
  double err=10.0;
  double inlier_dist = 0.1;
  vcl_vector<int> best_inliers;
  for (unsigned i=0;i<max_its;++i)
  {
    vcl_cout << '.' << vcl_flush;
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

bool boxm2_point_util::axis_align_scene(vcl_vector<bwm_video_corr_sptr> & corrs,
                                        vcl_vector<vpgl_perspective_camera<double> > & cams)
{
  vcl_vector<vgl_homg_point_3d<double> > points;
  for (unsigned i=0;i<corrs.size();++i)
  {
    vgl_homg_point_3d<double> homg_world_pt(corrs[i]->world_pt());
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
  if (!fit_plane.fit(1e6, &vcl_cerr))
    return false;
  vgl_homg_plane_3d<double> plane=fit_plane.get_plane();
#endif

  //get plane rotation
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

  //vgl_fit_plane_3d<double> fit_plane1(xformed_points);
  //if (!fit_plane1.fit(1e6, &vcl_cerr))
      //return false;

  //vgl_homg_plane_3d<double> plane1=fit_plane1.get_plane();

  return true;
}

vnl_double_3 boxm2_point_util::stddev( vcl_vector<vgl_point_3d<double> > const& v)
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
void boxm2_point_util::calc_projection_error(vcl_vector<vpgl_perspective_camera<double> >& cams,
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
        vcl_cout<<"Bad camera "<<view_number<<" is behind world point"<<vcl_endl;
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

void boxm2_point_util::report_error(vcl_map<unsigned,double>&   view_error_map,
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
//#ifdef DEBUG
    unsigned cam=ve_itr->first;
    double   err=ve_itr->second;
    unsigned cnt=view_count_map[cam];
    vcl_cout<<"   error for camera_"<<cam<<": "<<err/cnt<<vcl_endl;
//#endif
    double terror = ve_itr->second/view_count_map[ve_itr->first];
    if (terror > filter_thresh) {
      vcl_cout<<"Bad camera "<<ve_itr->first<<" has error "<<terror<<vcl_endl;
      bad_cams.insert(ve_itr->first);
    }
    else {
      error  += ve_itr->second;
      counts += view_count_map[ve_itr->first];
    }
  }
  vcl_cout<<"Filtered Avg Projection Error "<<error/(float)counts<<vcl_endl;
}
