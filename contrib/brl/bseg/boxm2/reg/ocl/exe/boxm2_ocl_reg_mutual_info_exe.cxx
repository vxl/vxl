//:
// \file
// \brief  First attempt at multi gpu render
// \author Andy Miller
// \date 13-Oct-2011
#include <boxm2/reg/ocl/boxm2_ocl_reg_mutual_info.h>
#include <boxm2/reg/ocl/boxm2_ocl_monte_carlo_reg.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/io/boxm2_stream_scene_cache.h>

#include <vgl/vgl_vector_3d.h>
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
  bool operator()(const vcl_pair<int,int> & s1, const vcl_pair<int,int> & s2) const
  {
    return  s1.first < s2.first
        || (s1.first == s2.first && s1.second < s2.second);
  }
};

int main(int argc,  char** argv)
{
  //init vgui (should choose/determine toolkit)
  vul_arg<vcl_string> sceneA_file("-sceneA", "sceneA filename", "");
  vul_arg<vcl_string> sceneB_file("-sceneB", "sceneB filename", "");
  vul_arg<vcl_string> xformAtoB_file("-xform", "xfrom filename", "");
  vul_arg_parse(argc, argv);
  //create scene
  boxm2_scene_sptr sceneA = new boxm2_scene(sceneA_file());
  boxm2_scene_sptr sceneB = new boxm2_scene(sceneB_file());
  boxm2_lru_cache::create(sceneA);
  boxm2_cache_sptr cacheA =boxm2_cache::instance();
  bocl_manager_child_sptr mgr =bocl_manager_child::instance();
  if (mgr->gpus_.size()==0)
    return false;
  bocl_device_sptr  device = mgr->cpus_[0];
  vcl_vector<vcl_string> data_types;
  vcl_vector<vcl_string> identifiers;
  data_types.push_back("alpha");
  identifiers.push_back("");
  boxm2_stream_scene_cache cacheB( sceneB, data_types,identifiers);
  boxm2_opencl_cache_sptr opencl_cacheA = new boxm2_opencl_cache(sceneA,device);

  vnl_vector<double> x(6,0.0);      vnl_vector<double> var(6,0.0);
  double q0=0,q1=0,q2=0,q3=0;       vgl_rotation_3d<double> r;
  double scale = 1.0;
  int numsamples = 100;
  if(xformAtoB_file() != "" )
  {
    vcl_ifstream ifile( xformAtoB_file().c_str() ) ;
    if(!ifile)
    {
        vcl_cout<<"Error: Cannot open" <<xformAtoB_file()<<vcl_endl;
        return -1;
    }
    //: scale 
    //ifile>>scale;
    //ifile>>q0>>q1>>q2>>q3;
    //ifile>>x[0]>>x[1]>>x[2];
    //vgl_rotation_3d<double> r1(vnl_quaternion<double>(q0,q1,q2,q3));

    ifile >> scale ;
    vnl_matrix<double> mat(4,4);
    ifile >> mat;

    mat = mat/scale;
    
    vnl_matrix<double> matr(3,3);     mat.extract(matr);
    vgl_rotation_3d<double> r1(matr);

    x[0] = mat[0][3];               var[0] = 150;            
    x[1] = mat[1][3];               var[1] = 150;
    x[2] = mat[2][3];               var[2] = 150;
    x[3] = r1.as_rodrigues()[0];    var[3] = 0.0;
    x[4] = r1.as_rodrigues()[1];    var[4] = 0.0;
    x[5] = r1.as_rodrigues()[2];    var[5] = 0.0;
    r= r1;  
 
  }
 
  vcl_cout<<"R "<< r <<vcl_endl;
  boxm2_ocl_monte_carlo_reg func(opencl_cacheA,cacheB,device,5,scale,numsamples);
  vcl_cout<<"Coimputing Minfo for the Original Position"<<vcl_endl;
  vcl_cout<<"M-INFO is " <<func.mutual_info(x)<<vcl_endl;

  func.init(x, var);
  vul_timer t;
  //func.likelihood(1);
  func.run_annealing();
  vcl_cout<<"Total time taken is "<<t.all()<<vcl_endl;
#if 0
  vnl_powell powell(&func);
  powell.set_linmin_xtol(0.1);
  //powell.set_x_tolerance(1);
  //powell.set_max_function_evals(10);
  vul_timer t ;
  t.mark();
  powell.minimize(x);

  vcl_cout<<"Time Taken is "<<t.all()<<vcl_endl;
  vgl_rotation_3d<double> rf(x[3],x[4],x[5]);

  vcl_ofstream ofile("outxform.txt");
  for(unsigned k = 0; k < 3; k++)
  {
      for(unsigned m = 0; m < 3; m++)
      {
          ofile << scale*rf.as_matrix()[k][m]<<" ";
      }
      ofile<<scale * x[k]<<vcl_endl;
  }
  ofile<<0<<" "<<0<<" "<<0<<" "<<1<<vcl_endl;

  vcl_cout<<" B to A\n"
          <<"Translation is ("<<x[0]<<','<<x[1]<<','<<x[2]<<")\n"
          <<"Rotation is "<<x[3]<<","<<x[4]<<","<<x[5]<<'\n'
          <<"Initial Mutual Info "<<func.mutual_info(vgl_rotation_3d<double>(),vgl_vector_3d<double>())<<'\n'
          <<"Final Mutual Info "<<func.mutual_info(rf,vgl_vector_3d<double>(x[0],x[1],x[2]))<<vcl_endl;
#endif

#if 0
  int numsamples  = 10;
  float var1_range  = 0.5;
  float var2_range  = 0.5;

  vil_image_view<float> mi(numsamples+1,numsamples+1);
  mi.fill(0.0);
  //vnl_vector<double> x(6,0.0);

  float var1_inc = 2* var1_range / ( (float) numsamples );
  float var2_inc = 2* var2_range / ( (float) numsamples );
  for (unsigned i = 0 ; i <=numsamples; i++)
  {
    for (unsigned j = 0 ; j <=numsamples; j++)
    {
      if(i == 0 && j == 0)
      {
          double tx =  x[0] + (-0.05+(double)i * 0.01);
          double ty =  x[1] + (-0.05+(double)j * 0.01);
          double tz = x[2];//+ (-50+(double)i * 10);
          //x[1] = -var2_range + (float)j * var2_inc;

          mi(i,j) = func.mutual_info(r,vgl_vector_3d<double>(tx,ty,tz));
          vcl_cout<<mi(i,j)<<" ";
          vcl_ofstream ofile("outxform.txt");

          for(unsigned k = 0; k < 3; k++)
          {
              for(unsigned m = 0; m < 3; m++)
                  ofile << scale*r.as_matrix()[k][m]<<" ";

              if(k == 0 )  ofile<<scale * tx<<vcl_endl;
              if(k == 1 )  ofile<<scale * ty<<vcl_endl;
              if(k == 2 )  ofile<<scale * tz<<vcl_endl;
          }

          ofile<<0<<" "<<0<<" "<<0<<" "<<1<<vcl_endl;
          ofile.close();
      }
    }
    vcl_cout<<vcl_endl;
  }
  vil_save(mi,"e:/data/xy.tiff");
#endif
  return 0;
}
