//:
// \file
// \brief  First attempt at multi gpu render
// \author Andy Miller
// \date 13-Oct-2011
#include <boxm2/reg/ocl/boxm2_ocl_reg_mutual_info.h>
#include <boxm2/reg/ocl/boxm2_ocl_hierarchical_reg.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_lru_cache2.h>
#include <boxm2/io/boxm2_stream_scene_cache.h>

#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vul/vul_arg.h>
#include <vul/vul_timer.h>
#include <vcl_algorithm.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache2.h>

#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_powell.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

#include <vcl_iostream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

struct ltstr
{
  bool operator()(const vcl_pair<int,int> & s1, const vcl_pair<int,int> & s2) const
  {
    return  s1.first < s2.first
        || (s1.first == s2.first && s1.second < s2.second);
  }
};

//: function to convert params to Xform Matrix.
void convert_params_to_xform(vnl_vector<double>  x, double scale, vnl_matrix<double> & xform)
{
    xform.set_size(4,4);
    xform.fill(0.0);
    vgl_rotation_3d<double> r(vnl_vector_fixed<double,3>(x[3],x[4],x[5]));

    xform[0][0] = r.as_matrix()[0][0];  xform[0][1] = r.as_matrix()[0][1]; xform[0][2] = r.as_matrix()[0][2];
    xform[1][0] = r.as_matrix()[1][0];  xform[1][1] = r.as_matrix()[1][1]; xform[1][2] = r.as_matrix()[1][2];
    xform[2][0] = r.as_matrix()[2][0];  xform[2][1] = r.as_matrix()[2][1]; xform[2][2] = r.as_matrix()[2][2];

    xform[0][3] = x[0] ; 
    xform[1][3] = x[1] ;
    xform[2][3] = x[2] ;

    xform = xform * scale ;

    xform[3][3]  = 1;
}
int main(int argc,  char** argv)
{
  //init vgui (should choose/determine toolkit)
  vul_arg<vcl_string> sceneA_file("-sceneA", "sceneA filename", "");
  vul_arg<vcl_string> sceneB_file("-sceneB", "sceneB filename", "");
  vul_arg<vcl_string> xformAtoB_file("-xform", "xfrom filename", "");
  vul_arg<double> rotationangle("-rot", "rotation angle ( in radians )", 0.5);
  vul_arg<bool> coarse("-coarse", "Registration USing coarser model", 0);

  vul_arg_parse(argc, argv);
  
  vcl_cout<<"Is Coarse "<<coarse()<<vcl_endl;

  //create scene
  boxm2_scene_sptr sceneA = new boxm2_scene(sceneA_file());
  boxm2_scene_sptr sceneB = new boxm2_scene(sceneB_file());
  boxm2_lru_cache2::create(sceneA);
  boxm2_cache2_sptr cacheA =boxm2_cache2::instance();
  bocl_manager_child_sptr mgr =bocl_manager_child::instance();
  if (mgr->gpus_.size()==0)
    return false;

  bocl_device_sptr  device = mgr->cpus_[0];
  vcl_vector<vcl_string> data_types;
  vcl_vector<vcl_string> identifiers;
  data_types.push_back("alpha");
  identifiers.push_back("");
  boxm2_stream_scene_cache cacheB( sceneB, data_types,identifiers);
  boxm2_opencl_cache2_sptr opencl_cacheA = new boxm2_opencl_cache2(device);

  vnl_vector<double> x(6,0.0);      vnl_vector<double> var(6,0.0);
  double q0=0,q1=0,q2=0,q3=0;       vgl_rotation_3d<double> r;
  double scale = 1.0;               int numsamples = 20;
  

  vcl_cout<<"Covariance in X direction can be "<<2*sceneB->blocks().begin()->second.sub_block_dim_.x()<<vcl_endl;
  vcl_cout<<"Origin of Scene A is "<<sceneA->local_origin()<<vcl_endl;
  vnl_vector<double> sceneA_origin(4);
  sceneA_origin[0] = sceneA->local_origin().x();
  sceneA_origin[1] = sceneA->local_origin().y();
  sceneA_origin[2] = sceneA->local_origin().z();
  sceneA_origin[3] = 1.0;

  var[0] = 2*sceneB->blocks().begin()->second.sub_block_dim_.x()/scale;  // var[0] = 1.0/8;     //   BH  
  var[1] = 2*sceneB->blocks().begin()->second.sub_block_dim_.y()/scale;  // var[1] = 1.0/8;     //   BH
  var[2] = 2*sceneB->blocks().begin()->second.sub_block_dim_.z()/scale;  // var[2] = 1.0/8;     //   BH
  var[3] = rotationangle();                                              // var[3] = 0.3/8;     //   BH
  var[4] = rotationangle();                                              // var[4] = 0.3/8;     //   BH
  var[5] = rotationangle();                                              // var[5] = 0.3/8;     //   BH
  
  if(xformAtoB_file() != "" )
  {
    vcl_ifstream ifile( xformAtoB_file().c_str() ) ;
    if(!ifile)
    {
        vcl_cout<<"Error: Cannot open" <<xformAtoB_file()<<vcl_endl;
        return -1;  
    }
    ifile >> scale ;
    vnl_matrix<double> mat(4,4);
    ifile >> mat;

    vnl_vector<double> sceneA_xformed_center =  mat*sceneA_origin;
    vgl_box_3d<double> scene_box = sceneB->bounding_box();

    double xfurthest = vcl_fabs(scene_box.min_x()-sceneA_xformed_center[0]) > vcl_fabs(scene_box.max_x()-sceneA_xformed_center[0])? scene_box.min_x():scene_box.max_x();
    double yfurthest = vcl_fabs(scene_box.min_y()-sceneA_xformed_center[1]) > vcl_fabs(scene_box.max_y()-sceneA_xformed_center[1])? scene_box.min_y():scene_box.max_y();
    double zfurthest = vcl_fabs(scene_box.min_z()-sceneA_xformed_center[2]) > vcl_fabs(scene_box.max_z()-sceneA_xformed_center[2])? scene_box.min_z():scene_box.max_z();

    vgl_point_3d<double> ptfurthest(xfurthest,yfurthest,zfurthest);
    vgl_point_3d<double> pt_sceneA_xformed_center(sceneA->local_origin().x(), sceneA->local_origin().y(), sceneA->local_origin().z());
    double dist = (ptfurthest -  pt_sceneA_xformed_center).length();
    // compute furthest point

    double thetafurthest = 2*  vcl_asin(sceneB->blocks().begin()->second.sub_block_dim_.x()/2/dist);

    vcl_cout<<"Angle is "<<thetafurthest<<vcl_endl;
  
    mat = mat/scale;
    
    vnl_matrix<double> matr(3,3);     mat.extract(matr);
    vgl_rotation_3d<double> r1(matr);

    vcl_cout<<"Sub Block Dim "<<sceneB->blocks().begin()->second.sub_block_dim_.x()<<vcl_endl;

    // use 2 to multiple if 4096 samples are asked but use 1 as factor if 64 samples. 

    x[0] = mat[0][3];              var[0] = 2*sceneB->blocks().begin()->second.sub_block_dim_.x()/scale;  // var[0] = 1.0/8;     //   BH    
    x[1] = mat[1][3];              var[1] = 2*sceneB->blocks().begin()->second.sub_block_dim_.y()/scale;  // var[1] = 1.0/8;     //   BH
    x[2] = mat[2][3];              var[2] = 2*sceneB->blocks().begin()->second.sub_block_dim_.z()/scale;  // var[2] = 1.0/8;     //   BH
    x[3] = r1.as_rodrigues()[0];   var[3] = rotationangle();  // var[3] = 0.3/8;     //   BH
    x[4] = r1.as_rodrigues()[1];   var[4] = rotationangle();  // var[4] = 0.3/8;     //   BH
    x[5] = r1.as_rodrigues()[2];   var[5] = rotationangle();  // var[5] = 0.3/8;     //   BH

    r= r1;  
 
    vnl_matrix<double> xform;
    convert_params_to_xform(x+var,scale,xform);
    vcl_cout<<xform<<vcl_endl;


  }

  boxm2_ocl_hierarchical_reg func(opencl_cacheA,cacheB,device,5,coarse(),scale,numsamples );
  func.init(x, var);
  vul_timer t;
  func.exhaustive(0);
  
  vnl_vector<double> xfinal = func.max_sample();
  vnl_matrix<double> xform;
  convert_params_to_xform(xfinal,scale,xform);
  
  vcl_cout<<"Final Xform is "<<vcl_endl;
  vcl_cout<<xform<<vcl_endl;
  vcl_cout<<"Total time taken is "<<t.all()<<vcl_endl;



  return 0;
}