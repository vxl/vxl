#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <testlib/testlib_test.h>
#include <vul/vul_file.h>
#include <vcl_string.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_interpolate.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <bwm/video/bwm_video_site_io.h>
#include <bwm/video/bwm_video_corr.h>
#include <vpgl/algo/vpgl_bundle_adjust.h>
#include <bwm/video/bwm_video_cam_istream.h>
#include <bwm/video/bwm_video_cam_ostream.h>

static void test_video_bundle_adjust()
{
  vcl_string corr_path = "";
#if 0
  corr_path = "c:/images/CapitolSite/capitol_32_corrs_camera.xml";
#endif
#if 1
  // corr_path = "c:/images/CapitolSite/capitol_32_corrs_interp_camera_wpts_refined.xml";
 corr_path = "c:/images/CapitolSite/capitol_32_corrs_full_computed_camera_wpts_refined_corrs.xml";
#endif
  bwm_video_site_io vio;
  if(!vio.open(corr_path))
    return;
  vcl_vector<bwm_video_corr_sptr> corrs = vio.corrs();

  if(!corrs.size())
    return;
  //format corrs as follows
  //  x_c0_w0 x_c0_w1 .. x_c0_wn-1 x_c1_w0 ...
  // iterate first over cameras and then over world points
  //the video corrs are world points with a list of camera image points
  //i.e. just the reverse. So it will be necessary to iterate a number
  //of times through the corr list


  //first get the start and end frame numbers
  unsigned min_frame = vnl_numeric_traits<unsigned>::maxval;
  unsigned max_frame = 0;
  for(vcl_vector<bwm_video_corr_sptr>::iterator cit = corrs.begin();
      cit != corrs.end(); ++cit)
    {
      unsigned minf = (*cit)->min_frame();
      unsigned maxf = (*cit)->max_frame();
      if(minf<min_frame)
        min_frame = minf;
      if(maxf>max_frame)
        max_frame = maxf;
    }
  unsigned ncameras = max_frame-min_frame +1;
  unsigned npoints = corrs.size();
  //next populate the image points and the mask array
  //the mask appears to be m x n ,
  // where m = number of cameras an n = number of world points (corrs)
  //
  vcl_vector<bool> init(npoints);
  vcl_vector<vcl_vector<bool> > mask(ncameras, init);
  vcl_vector<vgl_point_2d<double> > image_points(ncameras*npoints);
  for(unsigned w = 0; w<npoints; ++w)
    for(unsigned f = min_frame; f<=max_frame; ++f)
      {
        vgl_point_2d<double> pt;
        if(corrs[w]->match(f, pt))
          {
            image_points[f*npoints + w] = pt;
            mask[f][w] = true;
          }
        else
          mask[f][w] = false;
      }
  //next, find the number of rows that are completely masked and 
  //remove them creating a smaller problem
  vcl_vector<bool> empty_rows(ncameras, false);
  int filled_rows = 0;
  for(unsigned f = 0; f<ncameras; ++f){
    bool empty = true;
    for(unsigned w = 0; w<npoints; ++w)
      if(mask[f][w]) empty = false;
    if(empty)
      empty_rows[f] = true;
    else 
      ++filled_rows;
  }
  //create a new mask array and image point vector for the smaller problem
  vcl_vector<vcl_vector<bool> > cmask(filled_rows, init);
  vcl_vector<vgl_point_2d<double> > cimage_points(filled_rows*npoints);
  unsigned ff = 0;
  for(unsigned f = 0; f<ncameras; ++f)
	  if(!empty_rows[f]){
      for(unsigned w = 0; w<npoints; ++w){
        cmask[ff][w] = mask[f][w];
        cimage_points[ff*npoints + w] = image_points[f*npoints + w];
      }
	  ++ff;
	  }
  //set up the cameras
  vnl_double_3x3 M;
  M.fill(0.0);
  M[0][0] = 2200; M[0][2] = 640;
  M[1][1] = 2200; M[1][2] = 360;
  M[2][2]=1.0;
  vpgl_calibration_matrix<double> K(M);
  vnl_double_3 r(0,0,0.000001);
  vgl_rotation_3d<double> I(r); // small initial rotation
  vgl_homg_point_3d<double> center(0.0, 0.0, -1000.0);
  vpgl_perspective_camera<double> default_camera(K,center,I);
  vcl_vector<vpgl_perspective_camera<double> > cunknown_cameras(filled_rows,
                                                                default_camera);
  // initialize unknown world points
  vgl_point_3d<double> pun(0.0, 0.0, 0.0);
  vcl_vector<vgl_point_3d<double> > unknown_world(npoints,pun);

  // exectute the bundle adjustment
  bool success = vpgl_bundle_adjust::optimize(cunknown_cameras,
                                              unknown_world, cimage_points,
                                              cmask);
  //save the world points in the correspondences
  for(unsigned w = 0; w<npoints; ++w)
    {
      vgl_point_3d<double> pt = unknown_world[w];
      corrs[w]->set_world_pt(pt);
    }
  vcl_string test_path = "";
#if 0
  vcl_string test_path = "c:/images/CapitolSite/capitol_32_corrs_camera_wpts.xml";
#endif

#if 1
 test_path = "c:/images/CapitolSite/capitol_32_corrs_full_computed_camera_wpts_refined_corrs.xml";
#endif

  vio.x_write(test_path);

#if 0
  // fill in the missing rows by interpolation
  vcl_vector<vpgl_perspective_camera<double> > solved_cameras(ncameras);

  //first, fill in the known cameras
  unsigned ne = empty_rows.size();
  unsigned count = 0;
  for(unsigned fi = 0; fi<ne; ++fi)
    if(!empty_rows[fi])
      solved_cameras[fi]=cunknown_cameras[count++];
  //then interpolate the missing cameras
  count = 0;
  for(unsigned fi = 0; fi<ne; ++fi)
    {
      if(empty_rows[fi])//row is empty
        {
          // case 1 - i == 0
          if(fi==0)
            {
              // find the smallest row that is not empty
              bool found = false;
              for(unsigned j = 1; j<ne&&!found; ++j)
                if(!empty_rows[j])
                  {
                    found = true;
                    for(unsigned k = 0; k<j; ++k)
                      solved_cameras[k] = cunknown_cameras[0];
                    fi = j;
                  }
              if(!found){
                vcl_cerr << "Shouldn't happen\n";
                return;}
            }
          // case 2 - i == ne-1 (the last row)
          if(fi == ne-1)//last known camera
            solved_cameras[fi] = cunknown_cameras[filled_rows-1];
          
          // case 3 - the normal case 
          // typically there will be a run of missing cameras
          // also the cameras are filled in up to fi-1;
          // find the next known camera
          bool found = false;
          unsigned ki = fi+1;
          for(; ki<ne&&!found; ++ki)
            if(!empty_rows[ki])
              found = true;
          if(!found){
            vcl_cerr << "Shouldn't happen\n";
            return;}
          // get the cameras at fi-1 and next solved camera
          vpgl_perspective_camera<double> c0 = solved_cameras[fi-1];
          vpgl_perspective_camera<double> c1 = solved_cameras[ki-1];
          vcl_vector<vpgl_perspective_camera<double> > interp_cams;
          unsigned n_interp = ki-fi-1;
          if(!vpgl_interpolate::interpolate(c0, c1, n_interp, interp_cams))
            {
            vcl_cerr << "Interpolation failed\n";
            return;}
		  for(unsigned c = 0; c<interp_cams.size(); ++c)
			solved_cameras[fi+c] = interp_cams[c];
		  fi = ki-1;
        }
    }
  vcl_cout << "Solved Cameras \n";
  for(unsigned c = 0; c<ncameras; ++c){
    vgl_point_3d<double> p =solved_cameras[c].get_camera_center();
    vgl_rotation_3d<double> R = solved_cameras[c].get_rotation();
    vcl_cout << "C[" << c << "]("<< R << '|' << p.x() << ' ' << p.y() << ' '
             << p.z() <<  ")\n"; 
  }
#endif
  // create the camera output stream
    vcl_string dir = "cam_dir";
    vcl_cout << "Made camera stream directory "<< dir << '\n';
    vul_file::make_directory(dir.c_str());
    bwm_video_cam_ostream cam_ostr(dir);
    bool open = cam_ostr.is_open();
    if(!open) return;
    for(unsigned c = 0; c<ncameras; ++c)
      open = open && cam_ostr.write_camera(&cunknown_cameras[c]);
    if(!open) vcl_cout << "output camera stream failed\n";
  //	vpl_unlink(dir.c_str());
}

TESTMAIN(test_video_bundle_adjust);
