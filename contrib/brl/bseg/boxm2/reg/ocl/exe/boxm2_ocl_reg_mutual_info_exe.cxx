// PUBLIC RELEASE APPROVAL FROM AFRL
// Case Number: RY-14-0126
// PA Approval Number: 88ABW-2014-1143
// :
// \file
// \brief  Register two boxm2 worlds using muual information.
// \author Vishal JAin
// \date Apr-21-2014
#include <boxm2/reg/ocl/boxm2_ocl_reg_mutual_info.h>
#include <boxm2/reg/ocl/boxm2_ocl_hierarchical_reg.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_lru_cache.h>

#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vul/vul_arg.h>
#include <vul/vul_timer.h>
#include <vcl_algorithm.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_powell.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

#include <vcl_iostream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

struct ltstr
  {
  bool operator()(const vcl_pair<int, int> & s1, const vcl_pair<int, int> & s2) const
  {
    return s1.first < s2.first
           || (s1.first == s2.first && s1.second < s2.second);
  }

  };

// : function to convert params to Xform Matrix.
void convert_params_to_xform(vnl_vector<double>  x,  vnl_matrix<double> & xform)
{
  xform.set_size(4, 4);
  xform.fill(0.0);
  vgl_rotation_3d<double> r(vnl_vector_fixed<double, 3>(x[3], x[4], x[5]) );

  xform[0][0] = r.as_matrix()[0][0];  xform[0][1] = r.as_matrix()[0][1]; xform[0][2] = r.as_matrix()[0][2];
  xform[1][0] = r.as_matrix()[1][0];  xform[1][1] = r.as_matrix()[1][1]; xform[1][2] = r.as_matrix()[1][2];
  xform[2][0] = r.as_matrix()[2][0];  xform[2][1] = r.as_matrix()[2][1]; xform[2][2] = r.as_matrix()[2][2];

  xform[0][3] = x[0];
  xform[1][3] = x[1];
  xform[2][3] = x[2];

  xform = xform * x[6];

  xform[3][3]  = 1;
}

int main(int argc,  char* * argv)
{
  // init vgui (should choose/determine toolkit)
  vul_arg<vcl_string> sceneA_file("-sceneA", "sceneA filename", "");
  vul_arg<vcl_string> sceneB_file("-sceneB", "sceneB filename", "");
  vul_arg<vcl_string> xformAtoB_file("-xform", "xfrom filename", "");
  vul_arg<double>     rotationangle("-rot", "rotation angle ( in radians )", 0.1);
  vul_arg<vcl_string> oxform("-oxform", "xform filename", "");
  vul_arg<float>      radius("-radius", "radius * sub_block_dim of scene B", 3);
  vul_arg<float>      iscale("-iscale", "init scale", 1);
  vul_arg<float>      rscale("-rscale", "radius of scale", 0.00);
  vul_arg_parse(argc, argv);
  vul_timer t;

  if( !vul_file::exists(sceneA_file() ) || !vul_file::exists(sceneB_file() ) )
    {
    vcl_cout << "One or both of the secene files do not exist" << vcl_endl;
    return -1;
    }
  // create scene
  boxm2_scene_sptr sceneA = new boxm2_scene(sceneA_file() );
  boxm2_scene_sptr sceneB = new boxm2_scene(sceneB_file() );

  boxm2_lru_cache::create(sceneA);
  boxm2_cache_sptr     cache = boxm2_cache::instance();
  bocl_manager_child & mgr = bocl_manager_child::instance();
  if( mgr.gpus_.size() == 0 )
    {
    return false;
    }

  bocl_device_sptr        device = mgr.gpus_[1];
  boxm2_opencl_cache_sptr opencl_cache = new boxm2_opencl_cache(device);

  vnl_vector<double> x(7, 0.0);
  vnl_vector<double> var(7, 0.0);
  x[6] = iscale();
  if( xformAtoB_file() != "" )
    {
    vcl_ifstream ifile( xformAtoB_file().c_str() );
    if( !ifile )
      {
      vcl_cout << "Error: Cannot open" << xformAtoB_file() << vcl_endl;
      return -1;
      }
    double scale = 1.0;
    ifile >> scale;
    vnl_matrix<double> mat(4, 4);
    ifile >> mat;
    ifile.close();
    mat = mat / scale;
    vnl_matrix<double>      matr(3, 3);     mat.extract(matr);
    vgl_rotation_3d<double> r1(matr);
    x[0] = mat[0][3];
    x[1] = mat[1][3];
    x[2] = mat[2][3];
    x[3] = r1.as_rodrigues()[0];
    x[4] = r1.as_rodrigues()[1];
    x[5] = r1.as_rodrigues()[2];
    x[6] = scale;
    }
  var[0] = radius() * sceneA->blocks().begin()->second.sub_block_dim_.x();
  var[1] = radius() * sceneA->blocks().begin()->second.sub_block_dim_.y();
  var[2] = radius() * sceneA->blocks().begin()->second.sub_block_dim_.z();
  var[3] = rotationangle() * 0.0;
  var[4] = rotationangle();
  var[5] = rotationangle();
  var[6] = rscale();

  bool do_vary_scale = false;

  if( rscale() > 1e-10 )
    {
    do_vary_scale = true;
    }

  boxm2_ocl_hierarchical_reg func(opencl_cache, sceneA, sceneB, device, 5, do_vary_scale );
  func.init(x, var);
  func.exhaustive();
  vnl_vector<double> xfinal = func.max_sample();
  vnl_matrix<double> xform;
  convert_params_to_xform(xfinal, xform);
  vcl_cout << "Final Xform is " << vcl_endl;
  vcl_cout << xform << vcl_endl;
  vcl_ofstream ofile(oxform().c_str() );
  if( ofile )
    {
    ofile << xfinal[6] << "\n";
    ofile << xform;
    ofile.close();
    }
  else
    {
    vcl_cout << "Incorrect output file " << vcl_endl;
    }

  vcl_cout << "Total time taken is " << t.all() << vcl_endl;
  return 0;
}
