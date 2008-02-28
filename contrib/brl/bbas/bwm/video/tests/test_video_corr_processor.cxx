#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <testlib/testlib_test.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_interpolate.h>
#include <vnl/vnl_double_3x3.h>
#include <bwm/video/bwm_video_corr.h>
#include <bwm/video/bwm_video_corr_processor.h>

static void test_video_corr_processor()
{
  vgl_point_2d<double> p0[32]=
  {
    vgl_point_2d<double>(447.818939,519.803162),
    vgl_point_2d<double>(489.128021,426.969147),
    vgl_point_2d<double>(902.101501,501.610199),
    vgl_point_2d<double>(871.030823,408.823853),
    vgl_point_2d<double>(1111.207642,195.616898),
    vgl_point_2d<double>(1155.191162,255.666275),
    vgl_point_2d<double>(471.077576,162.970963),
    vgl_point_2d<double>(329.395050,55.820988),
    vgl_point_2d<double>(247.825806,122.180397),
    vgl_point_2d<double>(149.576172,165.165329),
    vgl_point_2d<double>(431.884399,262.910187),
    vgl_point_2d<double>(1008.051514,638.074585),
    vgl_point_2d<double>(1153.102539,464.335297),
    vgl_point_2d<double>(1042.628540,411.755707),
    vgl_point_2d<double>(802.537231,78.058441),
    vgl_point_2d<double>(790.458801,178.810211),
    vgl_point_2d<double>(1195.638184,552.541931),
    vgl_point_2d<double>(588.208984,225.705322),
    vgl_point_2d<double>(686.217224,230.859421),
    vgl_point_2d<double>(786.409302,299.519714),
    vgl_point_2d<double>(558.763062,154.499741),
    vgl_point_2d<double>(1117.828125,46.877785),
    vgl_point_2d<double>(1000.712708,91.822479),
    vgl_point_2d<double>(1161.266846,131.635925),
    vgl_point_2d<double>(1237.810181,99.633217),
    vgl_point_2d<double>(1022.436218,225.683136),
    vgl_point_2d<double>(1111.637939,283.855988),
    vgl_point_2d<double>(932.763000,413.853302),
    vgl_point_2d<double>(867.870483,368.896820),
    vgl_point_2d<double>(600.515869,182.515900),
    vgl_point_2d<double>(147.986938,421.438141),
    vgl_point_2d<double>(1108.481934,159.562622)
  };

  vgl_point_2d<double> p5[32]=
  {
    vgl_point_2d<double>(464.136719,431.885468),
    vgl_point_2d<double>(881.031555,503.014862),
    vgl_point_2d<double>(848.146484,410.638214),
    vgl_point_2d<double>(1081.841431,194.431717),
    vgl_point_2d<double>(1126.746704,253.690964),
    vgl_point_2d<double>(441.052887,166.658203),
    vgl_point_2d<double>(298.580231,59.179012),
    vgl_point_2d<double>(216.879303,125.867638),
    vgl_point_2d<double>(118.761360,170.081665),
    vgl_point_2d<double>(404.903137,267.124207),
    vgl_point_2d<double>(986.893738,638.776917),
    vgl_point_2d<double>(1127.906372,463.369598),
    vgl_point_2d<double>(1019.451538,411.463043),
    vgl_point_2d<double>(776.485107,79.924004),
    vgl_point_2d<double>(766.360046,181.312271),
    vgl_point_2d<double>(1172.197876,550.786133),
    vgl_point_2d<double>(562.398315,230.182678),
    vgl_point_2d<double>(661.108887,233.756531),
    vgl_point_2d<double>(762.530029,301.392609),
    vgl_point_2d<double>(531.020935,157.748016),
    vgl_point_2d<double>(1085.959839,45.955975),
    vgl_point_2d<double>(970.336853,92.788185),
    vgl_point_2d<double>(1130.847046,130.319061),
    vgl_point_2d<double>(1207.258667,97.262848),
    vgl_point_2d<double>(993.596741,225.683136),
    vgl_point_2d<double>(1083.544678,282.539124),
    vgl_point_2d<double>(909.849426,415.082397),
    vgl_point_2d<double>(844.459473,370.301483),
    vgl_point_2d<double>(573.469604,185.695923),
    vgl_point_2d<double>(121.737282,428.988220),
    vgl_point_2d<double>(1078.574341,158.509125)
  };

  vgl_point_2d<double> p10[32]=
  {
    vgl_point_2d<double>(402.167358,535.956787),
    vgl_point_2d<double>(439.086853,440.898712),
    vgl_point_2d<double>(859.142212,509.921112),
    vgl_point_2d<double>(824.325745,416.783600),
    vgl_point_2d<double>(1051.553345,198.118958),
    vgl_point_2d<double>(1097.248779,257.641571),
    vgl_point_2d<double>(410.940399,174.998383),
    vgl_point_2d<double>(266.185181,67.870369),
    vgl_point_2d<double>(185.669434,136.402618),
    vgl_point_2d<double>(88.034332,181.933517),
    vgl_point_2d<double>(376.926941,277.015381),
    vgl_point_2d<double>(965.209290,643.781006),
    vgl_point_2d<double>(1102.271240,466.705688),
    vgl_point_2d<double>(995.689270,416.203796),
    vgl_point_2d<double>(750.586609,86.683952),
    vgl_point_2d<double>(740.812683,187.764938),
    vgl_point_2d<double>(1148.406494,553.419861),
    vgl_point_2d<double>(534.612305,237.688858),
    vgl_point_2d<double>(636.351685,241.218811),
    vgl_point_2d<double>(738.241028,308.181793),
    vgl_point_2d<double>(502.927673,165.649261),
    vgl_point_2d<double>(1054.486694,49.248154),
    vgl_point_2d<double>(938.380798,97.002182),
    vgl_point_2d<double>(1098.978760,133.216171),
    vgl_point_2d<double>(1175.653809,100.950089),
    vgl_point_2d<double>(964.625549,229.897125),
    vgl_point_2d<double>(1055.187988,286.753113),
    vgl_point_2d<double>(886.672485,421.315582),
    vgl_point_2d<double>(820.404541,376.446869),
    vgl_point_2d<double>(546.342041,193.421570),
    vgl_point_2d<double>(94.697502,440.664490),
    vgl_point_2d<double>(1047.847290,161.786682)
  };

  //make set of correspondences
  vcl_vector<bwm_video_corr_sptr> corrs(32, new bwm_video_corr());
  for (unsigned i = 0; i<32; ++i)
  {
    corrs[i]->add(0, p0[i]);
    corrs[i]->add(5, p5[i]);
    corrs[i]->add(10, p10[i]);
  }

  bwm_video_corr_processor cp;
  cp.set_verbose(true);
  cp.set_correspondences(corrs);
  //set up the calibration matrix
  vnl_double_3x3 M;
  M.fill(0.0);
  M[0][0] = 2200; M[0][2] = 640;
  M[1][1] = 2200; M[1][2] = 360;
  M[2][2]=1.0;
  vpgl_calibration_matrix<double> K(M);
  // estimated distance is 330 meters
  bool good = cp.initialize_world_pts_and_cameras(K, 330);
  vcl_vector<vpgl_perspective_camera<double> > cameras = cp.cameras();
  vcl_vector<bwm_video_corr_sptr> mcorrs = cp.correspondences();
  bool valid = mcorrs[0]->world_pt_valid();
  vgl_point_3d<double> p3d = mcorrs[0]->world_pt();
  vgl_point_2d<double> p2d = cameras[2].project(p3d);
  double x2 = 0.6*p0[0].x()+0.4*p5[0].x();
  double y2 = 0.6*p0[0].y()+0.4*p5[0].y();
  double d = vcl_fabs(p2d.x()-x2)+vcl_fabs(p2d.y()-y2);
  TEST_NEAR("test initialize cameras and world points", d, 0, 0.1);
}

TESTMAIN(test_video_corr_processor);
