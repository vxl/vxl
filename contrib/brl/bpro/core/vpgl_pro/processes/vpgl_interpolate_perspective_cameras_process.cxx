#include <iostream>
#include <sstream>
#include <iomanip>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to get interpolated cameeras between two cameras

#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/vnl_quaternion.h>
#include <vul/vul_file_iterator.h>
#include <bpgl/bpgl_camera_utils.h>
#include <vul/vul_file.h>
#include <bpgl/algo/bpgl_interpolate.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//: Init function
bool vpgl_interpolate_perspective_cameras_process_cons(bprb_func_process& pro)
{
  //this process takes 3 inputs and has 0 outputs:

  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr"); // cam 0
  input_types.emplace_back("vpgl_camera_double_sptr"); // cam 1
  input_types.emplace_back("unsigned"); // # of cameras
  input_types.emplace_back("vcl_string"); // output folder for interpolated cameras


  bool ok = pro.set_input_types(input_types);


  if (!ok) return ok;

  std::vector<std::string> output_types;
  return pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_interpolate_perspective_cameras_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= pro.input_types().size()) {
    std::cout << "vpgl_interpolate_perspective_cameras_process: The number of inputs should be " << pro.input_types().size() << std::endl;
    return false;
  }
  int i=0;
  vpgl_camera_double_sptr cam0 = pro.get_input<vpgl_camera_double_sptr>(i++);
  vpgl_camera_double_sptr cam1 = pro.get_input<vpgl_camera_double_sptr>(i++);
  int ncams = pro.get_input<unsigned int>(i++);
  std::string outdir = pro.get_input<std::string>(i++);

  if (  ncams <=  0 || !vul_file::is_directory(outdir) )
  {
      std::cout<<"Incorrect arguments "<<std::endl;
      return false;
  }
  auto * pcam0 = dynamic_cast<vpgl_perspective_camera<double> *>(cam0.ptr()) ;
  auto * pcam1 = dynamic_cast<vpgl_perspective_camera<double> *>(cam1.ptr()) ;
   if( pcam0 && pcam1)
  {
      std::vector<vpgl_perspective_camera<double> >  cams;
      if(!bpgl_interpolate::interpolate(*pcam0, *pcam1, ncams , cams))
      {
          std::cout<<"Something crashed "<<std::endl;
          return false;
      }


      for(unsigned k = 0 ; k < cams.size(); k++)
      {
          std::ostringstream ss;
          ss << std::setw( 5 ) << std::setfill( '0' ) << k<<".txt";
          std::string filename = ss.str();
          std::ofstream ofile((outdir + "/" + filename).c_str());
          ofile<<cams[k];
          ofile.close();
      }
  }
  else
  {
      std::cout<<"Incorrect arguments "<<std::endl;
      return false;
  }
  return true;
}
