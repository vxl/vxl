#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <bbas/volm/volm_camera_space.h>

static void test_camera_space()
{

  double head_mid=0.0, head_radius=180.0, head_inc=45.0;
  double tilt_mid=90.0, tilt_radius=20.0, tilt_inc=10.0;
  double roll_mid=0.0,  roll_radius=3.0,  roll_inc=3.0;
  double top_fov_vals[] = {3.0,  4.0, 5.0, 12.0, 17.0, 18.0,19.0, 20.0, 24.0};
  vcl_vector<double> fovs(top_fov_vals, top_fov_vals + 9);
  double altitude = 1.6;
  unsigned ni = 1280, nj = 760;
  volm_camera_space csp(fovs, altitude, ni, nj,
			head_mid, head_radius, head_inc,
			tilt_mid, tilt_radius, tilt_inc,
			roll_mid, roll_radius, roll_inc);
  for(camera_space_iterator cit = csp.begin(0.0); cit != csp.end(); ++cit){
    cam_angles ca = cit->camera_angles();
    ca.print();
    unsigned indx = cit->cam_index();
    unsigned roll_index,  fov_index,  head_index, tilt_index;
    cit->cam_indices(roll_index,  fov_index,  head_index, tilt_index);
    unsigned de_roll_index,  de_fov_index,  de_head_index, de_tilt_index;
    cit->cam_indices(indx,de_roll_index,  de_fov_index,  
		     de_head_index, de_tilt_index);
    unsigned de_indx = cit->cam_index(roll_index,  fov_index,  
				      head_index, tilt_index);
    
    vcl_cout << "(index " << indx << ' ' << de_indx << ")\n";
    vcl_cout << "(" << roll_index << ' ' << fov_index << ' '
	     << head_index << ' ' << tilt_index << ")\n";
    vcl_cout << "(" << de_roll_index << ' ' << de_fov_index << ' '
	     << de_head_index << ' ' << de_tilt_index << ")\n";
  }

}


TESTMAIN(test_camera_space);
