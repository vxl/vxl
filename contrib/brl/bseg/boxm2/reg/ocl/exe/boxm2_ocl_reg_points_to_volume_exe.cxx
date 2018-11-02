//PUBLIC RELEASE APPROVAL FROM AFRL
//Case Number: RY-14-0126
//PA Approval Number: 88ABW-2014-1143
//:
// \file
// \brief
// \author Vishal Jain
// \date 13-Nov-2013
#include <iostream>
#include <algorithm>
#include <boxm2/reg/ocl/boxm2_ocl_reg_mutual_info.h>
#include <boxm2/reg/ocl/boxm2_ocl_hierarchical_points_to_volume_reg.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_lru_cache.h>


#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vul/vul_arg.h>
#include <vul/vul_timer.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bocl/bocl_manager.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_powell.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

#include <vil/vil_image_view.h>
#include <vil/vil_save.h>


struct ltstr
{
  bool operator()(const std::pair<int,int> & s1, const std::pair<int,int> & s2) const
  {
    return  s1.first < s2.first
        || (s1.first == s2.first && s1.second < s2.second);
  }
};

//: function to convert params to Xform Matrix.
void convert_params_to_xform(vnl_vector<double>  x,  vnl_matrix<double> & xform, bool reverse = false)
{
    xform.set_size(4,4);
    xform.fill(0.0);
    vgl_rotation_3d<double> r(vnl_vector_fixed<double,3>(x[3],x[4],x[5]));
    vnl_vector<double>  t(3);
    t[0]= x[0];t[1]=x[1];t[2]=x[2];
    double scale = x[6];
    if(reverse)
    {
        r = r.inverse();
        t = -scale*r.as_matrix()*t;
        scale = 1/scale ;
    }
    xform[0][0] = r.as_matrix()[0][0];  xform[0][1] = r.as_matrix()[0][1]; xform[0][2] = r.as_matrix()[0][2];
    xform[1][0] = r.as_matrix()[1][0];  xform[1][1] = r.as_matrix()[1][1]; xform[1][2] = r.as_matrix()[1][2];
    xform[2][0] = r.as_matrix()[2][0];  xform[2][1] = r.as_matrix()[2][1]; xform[2][2] = r.as_matrix()[2][2];

    xform[0][3] = t[0] ;
    xform[1][3] = t[1] ;
    xform[2][3] = t[2] ;

    xform = xform * scale ;

    xform[3][3]  = 1;

}
int main(int argc,  char** argv)
{
  //init vgui (should choose/determine toolkit)
  vul_arg<std::string> sceneA_file("-plyA", "points A (xyz) filename", "");
  vul_arg<std::string> sceneB_file("-sceneB", "sceneB filename", "");
  vul_arg<std::string> xformAtoB_file("-xform", "xfrom filename", "");
  vul_arg<double> rotationangle("-rot", "rotation angle ( in radians )", 0.1);
  vul_arg<std::string> oxform("-oxform", "xform filename", "");
  vul_arg<float> radius("-radius", "radius * sub_block_dim of scene B",3);
  vul_arg<float> iscale("-iscale", "init scale",1);
  vul_arg<float> rscale("-rscale", "radius of scale",0.00);
  vul_arg<bool> reverse("-reverse", "On B--> A , Off A--> B",false);
  vul_arg_parse(argc, argv);
  vul_timer t;

  if (!vul_file::exists(sceneB_file()) )
  {
      std::cout<<"scene files do not exist"<<std::endl;
      return -1;
  }
  //create scene
  std::vector<vgl_point_3d<double> > pts;
  if(vul_file::extension(sceneA_file()) == ".xyz")
  {
      std::ifstream ifile(sceneA_file().c_str());
      std::string line;
      while(std::getline(ifile,line))
      {
          std::istringstream iss(line);
          vnl_vector<double> x;
          iss>>x;
          pts.emplace_back(x[0],x[1],x[2] );
      }
      ifile.close();
  }
  else{
      std::cout<<"Point Cloud is empty"<<std::endl;
      return 0;
  }
  std::cout<<"Points Loaded "<<pts.size()<<std::endl;
  auto * vpts = new float[pts.size()*3];
  for(unsigned i= 0;i < pts.size(); i++)
  {
      vpts[i*3+0] = pts[i].x(); vpts[i*3+1] = pts[i].y(); vpts[i*3+2] = pts[i].z();
  }
  boxm2_scene_sptr sceneB = new boxm2_scene(sceneB_file());
  boxm2_lru_cache::create(sceneB);
  boxm2_cache_sptr cache =boxm2_cache::instance();
  bocl_manager_child &mgr =bocl_manager_child::instance();
  if (mgr.gpus_.size()==0)
    return false;

  bocl_device_sptr  device = mgr.gpus_[1];
  boxm2_opencl_cache_sptr opencl_cache = new boxm2_opencl_cache(device);

  vnl_vector<double> x(7,0.0);
  vnl_vector<double> var(7,0.0);
  x[6] = iscale();
  if(xformAtoB_file() != "" )
  {
      std::ifstream ifile( xformAtoB_file().c_str() ) ;
      if(!ifile)
      {
          std::cout<<"Error: Cannot open" <<xformAtoB_file()<<std::endl;
          return -1;
      }
      double scale = 1.0;
      ifile >> scale ;
      vnl_matrix<double> mat(4,4);
      ifile >> mat;
      ifile.close();
      mat = mat/scale;
      vnl_matrix<double> matr(3,3);
      mat.extract(matr);
      vnl_vector<double> t(3,0.0);
      t[0] = mat[0][3]; t[1] = mat[1][3]; t[2] = mat[2][3];

      if(reverse())
      {
          matr = matr.transpose();
          t = -scale*matr*t;
          scale = 1/scale;
      }
      vgl_rotation_3d<double> r1(matr);
      x[0] = t[0];         x[3] = r1.as_rodrigues()[0];
      x[1] = t[1];         x[4] = r1.as_rodrigues()[1];
      x[2] = t[2];         x[5] = r1.as_rodrigues()[2];
      x[6] = scale;
  }
  var[0] = radius()*sceneB->blocks().begin()->second.sub_block_dim_.x()/x[6];
  var[1] = radius()*sceneB->blocks().begin()->second.sub_block_dim_.y()/x[6];
  var[2] = radius()*sceneB->blocks().begin()->second.sub_block_dim_.z()/x[6];
  var[3] = rotationangle();
  var[4] = rotationangle();
  var[5] = rotationangle();
  var[6] = rscale();

  bool do_vary_scale = false;
  if(rscale() > 1e-10)
      do_vary_scale = true ;

  boxm2_ocl_hierarchical_points_to_volume_reg func(opencl_cache,vpts,sceneB,pts.size(),device,do_vary_scale );
  func.init(x, var);
  func.exhaustive();
  vnl_vector<double> xfinal = func.max_sample();
  vnl_matrix<double> xform;
  convert_params_to_xform(xfinal,xform,reverse());
  std::cout<<"Final Xform is "<<std::endl;
  std::cout<<xform<<std::endl;
  std::ofstream ofile(oxform().c_str());
  if(ofile)
  {
      ofile<<xfinal[6]<<"\n";
      ofile<<xform;
      ofile.close();
  }
  else
      std::cout<<"Incorrect output file "<<std::endl;

  std::cout<<"Total time taken is "<<t.all()<<std::endl;
  return 0;
}
