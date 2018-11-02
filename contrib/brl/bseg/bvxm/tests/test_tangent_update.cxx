#include <iostream>
#include <cstdlib>
#include <testlib/testlib_test.h>
#include <vul/vul_file.h>
#include <vpl/vpl.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_infinite_line_3d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/algo/vgl_intersection.h>
#include <vgl/vgl_closest_point.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_von_mises_tangent_processor.h>
#include <bvxm/grid/bvxm_voxel_slab.h>
#include <bvxm/grid/bvxm_voxel_slab_iterator.h>
#include <bvxm/bvxm_edge_ray_processor.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_math.h>

static double ran()
{
  return 2.0*double(std::rand()/(RAND_MAX+1.0)) - 1.0;
}

bool tangent_image(vpgl_perspective_camera<double> const* cam, vgl_infinite_line_3d<double> const& line_3d, double image_error_radius, vil_image_view<float>* tan_image)
{
  if (tan_image->nplanes()!=3)
    return false;
  tan_image->fill(0.0f);
  vgl_line_2d<double> l2d = (*cam)(line_3d);
  // perturb the line with noise
  // assume the line direction is estimated over three pixels
  l2d.normalize();
  double a = l2d.a(), b = l2d.b(), c = l2d.c();
  double noise_x = 0.5*image_error_radius*(ran()-ran());
  double noise_y = 0.5*image_error_radius*(ran()-ran());
  a += noise_x; b +=noise_y;
  c += image_error_radius*ran();
  double mg = std::sqrt(a*a + b*b);
  //renormalize the line
  a/= mg; b/= mg; c/= mg;
  // perturb the line with noise
  // assume the line direction is estimated over three pixels
  for (unsigned j=0; j<tan_image->nj(); ++j)
    for (unsigned i=0; i<tan_image->ni(); ++i)
    {
      //create a box for each pixel
      vgl_box_2d<double> bx;
      bx.add(vgl_point_2d<double>(i,j));
      bx.add(vgl_point_2d<double>(i+1,j+1));
      vgl_point_2d<double> p0, p1;
      //see if the line intersects the pixel domain
      if (vgl_intersection<double>(bx, l2d, p0, p1))
      {
        (*tan_image)(i,j,0) = static_cast<float>(a);
        (*tan_image)(i,j,1) = static_cast<float>(b);
        (*tan_image)(i,j,2) = static_cast<float>(c);
      }
      else {
        //can't happen with a normalized line
        (*tan_image)(i,j,0)=-2.0;
        (*tan_image)(i,j,1)=-2.0;
      }
    }
  return true;
}

static void test_tangent_update()
{
  //
  //============= test ray processor with perfect data ======================
  //
  std::string model_dir("test_tangent_world_dir");
  if (vul_file::is_directory(model_dir))
    vpl_rmdir(model_dir.c_str());  //use this instead of vul since some MS versions hang on on Y/N?
  else if (vul_file::exists(model_dir))
    vpl_unlink(model_dir.c_str());
  vul_file::make_directory(model_dir);


  vgl_point_3d<float> grid_corner(0.f,0.f,0.f);
  vgl_vector_3d<unsigned> grid_size(5,5,1);
  float vox_len = 1.0f;

  // create the world
  bvxm_voxel_world_sptr world = new bvxm_voxel_world();

  // the tangent processor
  bvxm_edge_ray_processor edge_proc(world);

  bvxm_world_params_sptr params = new bvxm_world_params;

  // needs an lvcs, TO DO, instantiate lvcs with right parameters
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs();
  params->set_params(model_dir,grid_corner,grid_size,vox_len,lvcs);

  world->set_params(params);
  world->clean_grids();
  const double sq3 = vnl_math::sqrt1_3;
  vgl_point_3d<double> p0(2.5,2.5,0.0), p1(2.5+sq3, 2.5+sq3, sq3);
  vgl_infinite_line_3d<double> l3d(p0, p1);
  vgl_point_3d<double> org(2.5, 2.5, 0);
  if (l3d.contains(org))
    std::cout << "Test Line intersects (2.5, 2.5, 0)\n";
  //create synthetic cameras
  vnl_matrix_fixed<double,3,3> temp;
  temp.fill(0.0);
  temp[0][0]=5.0;  temp[0][2]=5.0;
  temp[1][1]=5.0; temp[1][2]=5.0;
  temp[2][2]=1.0;
  vpgl_calibration_matrix<double> K(temp);
  auto* c0 = new vpgl_perspective_camera<double>();
  auto* c1 = new vpgl_perspective_camera<double>();
  auto* c2 = new vpgl_perspective_camera<double>();
  c0->set_calibration(K);  c1->set_calibration(K);   c2->set_calibration(K);
  c0->set_camera_center(vgl_point_3d<double>(2.5, 2.5, 5.0));
  c1->set_camera_center(vgl_point_3d<double>(0.0, 2.5, 5.0));
  c2->set_camera_center(vgl_point_3d<double>(2.5, 0.0, 5.0));
  c0->look_at(vgl_homg_point_3d<double>(2.5, 2.5,0.0),
              vgl_vector_3d<double>(0.0, 1.0, 0.0) );
  c1->look_at(vgl_homg_point_3d<double>(2.5, 2.5,0.0),
              vgl_vector_3d<double>(0.0, 1.0, 0.0) );
  c2->look_at(vgl_homg_point_3d<double>(2.5, 2.5,0.0),
              vgl_vector_3d<double>(0.0, 1.0, 0.0) );
  //print cameras
  std::cout << "**camera 0**\n" << c0->get_matrix() << '\n'
           << "**camera 1**\n" << c1->get_matrix() << '\n'
           << "**camera 2**\n" << c2->get_matrix() << '\n'
           << "camera 0 *center* " << c0->get_camera_center() << '\n'
           << "camera 1 *center* " << c1->get_camera_center() << '\n'
           << "camera 2 *center* " << c2->get_camera_center() << '\n';
  //create a synthetic image
  vil_image_view_base_sptr view_0 = new vil_image_view<float>(10,10,3);
  vil_image_view_base_sptr view_1 = new vil_image_view<float>(10,10,3);
  vil_image_view_base_sptr view_2 = new vil_image_view<float>(10,10,3);
  // fill images
  auto* v0 = static_cast<vil_image_view<float>*>(view_0.ptr());
  auto* v1 = static_cast<vil_image_view<float>*>(view_1.ptr());
  auto* v2 = static_cast<vil_image_view<float>*>(view_2.ptr());
  tangent_image(c0, l3d, 0.0, v0);
  tangent_image(c1, l3d, 0.0, v1);
  tangent_image(c2, l3d, 0.0, v2);
  vgl_homg_point_3d<double> c00(0,0,0), c10(5,0,0), c01(0,5,0), c11(5,5,0);
  std::cout << "voxel world corners in camera 0\n"
           << "(0,0,0)->" << vgl_point_2d<double>(c0->project(c00)) << '\n'
           << "(5,0,0)->" << vgl_point_2d<double>(c0->project(c10)) << '\n'
           << "(0,5,0)->" << vgl_point_2d<double>(c0->project(c01)) << '\n'
           << "(5,5,0)->" << vgl_point_2d<double>(c0->project(c11)) << '\n'
           << "voxel world corners in camera 1\n"
           << "(0,0,0)->" << vgl_point_2d<double>(c1->project(c00)) << '\n'
           << "(5,0,0)->" << vgl_point_2d<double>(c1->project(c10)) << '\n'
           << "(0,5,0)->" << vgl_point_2d<double>(c1->project(c01)) << '\n'
           << "(5,5,0)->" << vgl_point_2d<double>(c1->project(c11)) << '\n'
           << "voxel world corners in camera 2\n"
           << "(0,0,0)->" << vgl_point_2d<double>(c2->project(c00)) << '\n'
           << "(5,0,0)->" << vgl_point_2d<double>(c2->project(c10)) << '\n'
           << "(0,5,0)->" << vgl_point_2d<double>(c2->project(c01)) << '\n'
           << "(5,5,0)->" << vgl_point_2d<double>(c2->project(c11)) << '\n';

  // print tangent images
  std::cout << "view 0\n";
  for (unsigned j = 0; j<v0->nj(); j++) {
    std::cout.precision(1);
    for (unsigned i = 0; i<v0->ni(); i++)
      std::cout << '(' << (*v0)(i,j,0) << ' ' << (*v0)(i,j,1) << ' ' << (*v0)(i,j,2) << ')';
    std::cout << '\n';
  }
  std::cout << "view 1\n";
  for (unsigned j = 0; j<v1->nj(); j++) {
    std::cout.precision(1);
    for (unsigned i = 0; i<v1->ni(); i++)
      std::cout << '(' << (*v1)(i,j,0) << ' ' << (*v1)(i,j,1) << ' ' << (*v1)(i,j,2) << ')';
    std::cout << '\n';
  }
  std::cout << "view 2\n";
  for (unsigned j = 0; j<v2->nj(); j++) {
    std::cout.precision(1);
    for (unsigned i = 0; i<v2->ni(); i++)
      std::cout << '(' << (*v2)(i,j,0) << ' ' << (*v2)(i,j,1) << ' ' << (*v2)(i,j,2) << ')';
    std::cout << '\n';
  }

  //create metadata:
  bvxm_image_metadata obs0(view_0, c0);
  bvxm_image_metadata obs1(view_1, c1);
  bvxm_image_metadata obs2(view_2, c2);

  bool init_success = edge_proc.init_von_mises_edge_tangents(obs0, obs1);
  TEST("init", init_success, true);
  bool update_success = edge_proc.update_von_mises_edge_tangents(obs2);
  TEST("update", update_success, true);
  //test the resulting grid
  // 3-d tangent position grid distributions
  typedef bvxm_voxel_traits<TANGENT_POS>::voxel_datatype upos_dist_t;
  typedef bvxm_voxel_traits<TANGENT_POS>::obs_type upos_t;
  typedef bvxm_voxel_traits<TANGENT_DIR>::voxel_datatype udir_dist_t;
  typedef bvxm_voxel_traits<TANGENT_DIR>::obs_type udir_t;
  bvxm_voxel_grid_base_sptr tangent_pos_base =
    world->get_grid<TANGENT_POS>(0,0);
  auto *up_pos_dist_grid  =
    static_cast<bvxm_voxel_grid<upos_dist_t>*>(tangent_pos_base.ptr());
  //3-d tangent direction grid distributions
  bvxm_voxel_grid_base_sptr tangent_dir_base =
    world->get_grid<TANGENT_DIR>(0, 0);
  auto *up_dir_dist_grid  =
    static_cast<bvxm_voxel_grid<udir_dist_t>*>(tangent_dir_base.ptr());

  bvxm_voxel_grid<upos_dist_t>::iterator up_pos_dist_it = up_pos_dist_grid->begin();
  bvxm_voxel_grid<udir_dist_t>::iterator up_dir_dist_it = up_dir_dist_grid->begin();
  upos_dist_t up_pos_dist = (*up_pos_dist_it)(2,2);
  udir_dist_t up_dir_dist = (*up_dir_dist_it)(2,2);
  std::cout.precision(4);
  std::cout << up_pos_dist << '\n' << up_dir_dist << '\n';
  upos_t mup = up_pos_dist.mean();
  float var = up_pos_dist.var();
  float nobsp = up_pos_dist.num_observations;
  TEST_NEAR("test updated position distribution", mup[0] + mup[1] + var+ nobsp - 2.0f, 1.0205177+1.7676896,1e-3);
  udir_t mud = up_dir_dist.mean();
  float kappa = up_dir_dist.kappa();
  float nobsd = up_dir_dist.num_observations;
  TEST_NEAR("test updated direction distribution", mud[0] + mud[1] + mud[2]+ 1.0f/kappa,1.732050807+nobsd-2.0f,1e-6);

#if 0
  //
  //============= test ray processor with noisy data ======================
  //
  update_success = true;
  unsigned n_noise_full = 500;
  for (unsigned j = 0; j<n_noise_full; ++j) {
    edge_proc.init_von_mises_edge_tangents(obs0, obs1);
    for (unsigned i = 0; i<=j; ++i) {
      tangent_image(c2, l3d, .1, v2);
      update_success = edge_proc.update_von_mises_edge_tangents(obs2);
    }
#if 0
    std::cout << "grid contents after " << j << " noisy updates\n";
    for (unsigned r=0; r<5; ++r) {
      for (unsigned c=0; c<5; ++c) {
        up_pos_dist = (*up_pos_dist_it)(c,r);
        up_dir_dist = (*up_dir_dist_it)(c,r);
        std::cout << '(' << up_pos_dist.num_observations << ' '
                 << up_dir_dist.num_observations << ") ";
      }
      std::cout << '\n';
    }
#endif
    std::cout.precision(4);
    up_pos_dist = (*up_pos_dist_it)(2,2);
    up_dir_dist = (*up_dir_dist_it)(2,2);
    std::cout << "iteration: " << j << '\n' << up_pos_dist << '\n' << up_dir_dist << '\n';
  }
  mup = up_pos_dist.mean();
  var = up_pos_dist.var();
  nobsp = up_pos_dist.num_observations;
  //  TEST_NEAR("test updated position distribution", mup[0] + mup[1] + var+ nobsp - 2.0f, 1.0205177+1.7676896,1e-6);
  mud = up_dir_dist.mean();
  kappa = up_dir_dist.kappa();
  nobsd = up_dir_dist.num_observations;
  //  TEST_NEAR("test updated direction distribution", mud[0] + mud[1] + mud[2]+ 1.0f/kappa,1.732050807+nobsd-2.0f,1e-6);
#endif
  /// test with actual observed backprojected tangent planes
  std::list<vgl_plane_3d<double> > planes;
  planes.emplace_back(-0.5285, 0.458072, 0.714743, 43.7552);
  planes.emplace_back(-0.522202, 0.435534, 0.733222, 42.5723);
  planes.emplace_back(-0.501778, 0.444072, 0.742307, 42.0312);
  planes.emplace_back(0.548674, -0.432393, -0.715537, -43.5871);
  planes.emplace_back(0.588869, -0.441348, -0.677085, -45.619);
  planes.emplace_back(0.623675, -0.523734, -0.580286, -50.6641);
  planes.emplace_back(0.625087, -0.577003, -0.525675, -52.8468);
  planes.emplace_back(0.685963, -0.600205, -0.411351, -56.5028);
  planes.emplace_back(-0.698784, 0.653447, 0.291046, 59.2007);
  planes.emplace_back(-0.715996, 0.68723, 0.122737, 61.2933);
  planes.emplace_back(-0.705237, 0.70682, -0.0551943, 61.4763);
  planes.emplace_back(-0.688649, 0.673296, -0.269136, 59.1564);
  planes.emplace_back(0.649432, -0.601921, 0.464681, -54.1332);
  planes.emplace_back(-0.520232, 0.467336, 0.714811, 43.76);
  planes.emplace_back(-0.483641, 0.482089, 0.730535, 42.8933);
  planes.emplace_back(-0.510775, 0.434404, 0.741891, 42.0785);
  planes.emplace_back(0.509235, -0.442755, -0.738003, -42.2039);
  planes.emplace_back(0.598526, -0.394519, -0.697224, -44.0743);
  planes.emplace_back(0.57679, -0.454419, -0.678834, -45.5965);
  planes.emplace_back(0.657091, -0.508597, -0.556382, -51.446);
  planes.emplace_back(0.615269, -0.586592, -0.526645, -52.894);
  planes.emplace_back(0.695664, -0.600032, -0.394986, -56.9672);
  planes.emplace_back(-0.698014, 0.659298, 0.279469, 59.4179);
  planes.emplace_back(0.642563, -0.600573, 0.475841, -53.8482);
  planes.emplace_back(-0.485003, 0.478106, 0.732248, 42.6716);
  planes.emplace_back(0.540018, -0.418333, -0.730327, -42.4096);
  planes.emplace_back(0.539065, -0.446844, -0.713961, -43.6953);
  planes.emplace_back(-0.664266, 0.566153, 0.48808, 54.2134);
  planes.emplace_back(-0.7264, 0.68346, 0.0722881, 61.5753);
  planes.emplace_back(-0.712671, 0.691969, -0.115233, 61.1809);
  planes.emplace_back(-0.680231, 0.665795, -0.306598, 58.3956);
  planes.emplace_back(0.634285, -0.603643, 0.483009, -53.754);
  planes.emplace_back(-0.516749, 0.465635, 0.718439, 43.4822);
  planes.emplace_back(-0.532607, 0.414477, 0.737928, 42.1126);
  planes.emplace_back(0.517689, -0.439086, -0.734304, -42.4402);
  planes.emplace_back(0.549794, -0.441542, -0.709061, -43.9967);
  planes.emplace_back(0.590554, -0.457122, -0.665045, -46.1956);
  planes.emplace_back(0.595241, -0.503461, -0.626271, -48.4808);
  planes.emplace_back(0.637005, -0.532552, -0.557327, -51.628);
  planes.emplace_back(0.676165, -0.564935, -0.472915, -54.6711);
  planes.emplace_back(-0.69297, 0.614464, 0.377131, 57.4514);
  planes.emplace_back(0.69961, -0.669562, -0.249462, -59.8512);
  planes.emplace_back(-0.715962, 0.694683, 0.0693803, 61.5591);
  planes.emplace_back(-0.715381, 0.683568, -0.144792, 60.8299);
  planes.emplace_back(0.684953, -0.637682, 0.352421, -57.3216);
  planes.emplace_back(-0.52007, 0.456974, 0.721596, 43.2585);
  planes.emplace_back(0.528574, -0.431593, -0.730983, -42.6288);
  planes.emplace_back(0.570125, -0.429754, -0.700192, -44.404);
  planes.emplace_back(0.615801, -0.496418, -0.611848, -49.0148);
  planes.emplace_back(0.659619, -0.523962, -0.538856, -52.2182);
  planes.emplace_back(-0.656362, 0.582268, 0.479742, 54.4518);
  planes.emplace_back(0.700516, -0.616266, -0.359852, -57.8066);
  planes.emplace_back(-0.714899, 0.664226, 0.218455, 60.2885);
  planes.emplace_back(-0.717838, 0.694559, 0.04791, 61.6234);
  planes.emplace_back(0.710361, -0.685558, 0.159363, -60.8077);
  planes.emplace_back(-0.675597, 0.646403, -0.354587, 57.2442);
  planes.emplace_back(-0.497109, 0.486057, 0.71877, 43.4578);
  planes.emplace_back(-0.528637, 0.417225, 0.739234, 42.1204);
  planes.emplace_back(0.522817, -0.439771, -0.730249, -42.6491);
  planes.emplace_back(0.427178, -0.529172, -0.733141, -42.3359);
  planes.emplace_back(0.62753, -0.494254, -0.601597, -49.4515);
  planes.emplace_back(0.703954, -0.620522, -0.345546, -58.0937);
  planes.emplace_back(-0.727901, 0.685567, 0.0125787, 61.7201);
  planes.emplace_back(-0.70875, 0.681868, -0.180913, 60.5072);
  planes.emplace_back(0.677235, -0.623893, 0.390014, -56.4256);
  planes.emplace_back(-0.509639, 0.46449, 0.724236, 43.1061);
  planes.emplace_back(0.516222, -0.431992, -0.739525, -42.1699);
  planes.emplace_back(0.577871, -0.43265, -0.692011, -44.7954);
  planes.emplace_back(0.591776, -0.469869, -0.654999, -46.9072);
  planes.emplace_back(0.604973, -0.513093, -0.608887, -49.4073);
  planes.emplace_back(-0.620909, 0.611106, 0.490939, 54.0752);
  planes.emplace_back(0.724924, -0.665907, -0.176217, -60.8058);
  planes.emplace_back(-0.716708, 0.697287, 0.0109315, 61.6121);
  planes.emplace_back(0.667254, -0.632211, 0.393804, -56.283);
  planes.emplace_back(-0.536984, 0.421891, 0.730517, 42.5845);
  planes.emplace_back(0.54799, -0.423347, -0.721446, -43.135);
  planes.emplace_back(0.571107, -0.441157, -0.692255, -44.7929);
  planes.emplace_back(0.599161, -0.522051, -0.607017, -49.5332);
  planes.emplace_back(0.610538, -0.568233, -0.551684, -51.7647);
  planes.emplace_back(0.671252, -0.591074, -0.447272, -55.4253);
  planes.emplace_back(-0.712105, 0.679614, 0.176155, 60.9133);
  planes.emplace_back(-0.724748, 0.688592, -0.0241104, 61.6027);
  planes.emplace_back(-0.705201, 0.67126, -0.228256, 59.8412);
  planes.emplace_back(0.592515, -0.632831, 0.498449, -53.0998);
  vgl_infinite_line_3d<double> l3d_int = vgl_intersection(planes);
  vgl_vector_3d<double> dir = l3d_int.direction(), vsum(0,0,0);
  vgl_point_3d<double> pline = l3d_int.point();
  dir/=static_cast<double>(dir.length());
  unsigned cnt = 0;
  for (auto & plane : planes)
  {
    cnt++;
    vgl_vector_3d<double> normal = plane.normal();
      //the vector in the tangent plane closest to mu
    vgl_vector_3d<double> dirv = normalized(dir -dot_product(normal, dir)*normal);
    vsum += dirv;
    double dp = dot_product(dir, dirv);
    vgl_point_3d<double> cp = vgl_closest_point(plane, pline);
    vgl_vector_3d<double> dv = cp-pline;
    vgl_vector_3d<double> d0 = dv-dot_product(dir,dv)*dir;
    std::cout << std::acos(dp)*vnl_math::deg_per_rad << ' ' << vsum.length()/cnt
             << ' ' << d0.length() << '\n';
  }
  double final = vsum.length()/cnt;
  std::cout << "Final: " << final << '\n';
}

TESTMAIN( test_tangent_update );
