#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to get interpolated cameeras between two cameras

#include <vcl_iostream.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/vnl_quaternion.h>
#include <vul/vul_file_iterator.h>
#include <bpgl/bpgl_camera_utils.h>
#include <vul/vul_file.h>
#include <bpgl/algo/bpgl_interpolate.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
//: Init function
bool vpgl_interpolate_perspective_cameras_process_cons(bprb_func_process& pro)
{
  //this process takes 3 inputs and has 0 outputs:

  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr"); // cam 0
  input_types.push_back("vpgl_camera_double_sptr"); // cam 1
  input_types.push_back("unsigned"); // # of cameras
  input_types.push_back("vcl_string"); // output folder for interpolated cameras

  
  bool ok = pro.set_input_types(input_types);


  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  return pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_interpolate_perspective_cameras_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= pro.input_types().size()) {
    vcl_cout << "vpgl_interpolate_perspective_cameras_process: The number of inputs should be " << pro.input_types().size() << vcl_endl;
    return false;
  }
  int i=0;
  vpgl_camera_double_sptr cam0 = pro.get_input<vpgl_camera_double_sptr>(i++);
  vpgl_camera_double_sptr cam1 = pro.get_input<vpgl_camera_double_sptr>(i++);
  int ncams = pro.get_input<unsigned int>(i++);
  vcl_string outdir = pro.get_input<vcl_string>(i++);

  if (  ncams <=  0 || !vul_file::is_directory(outdir) )
  {
      vcl_cout<<"Incorrect arguments "<<vcl_endl;
      return false;
  }
  vpgl_perspective_camera<double> * pcam0 = dynamic_cast<vpgl_perspective_camera<double> *>(cam0.ptr()) ;
  vpgl_perspective_camera<double> * pcam1 = dynamic_cast<vpgl_perspective_camera<double> *>(cam1.ptr()) ;
   if( pcam0 && pcam1) 
  {
      vcl_vector<vpgl_perspective_camera<double> >  cams;
      if(!bpgl_interpolate::interpolate(*pcam0, *pcam1, ncams , cams))
      {
          vcl_cout<<"Something crashed "<<vcl_endl;
          return false;
      }

      
      for(unsigned k = 0 ; k < cams.size(); k++)
      {
          vcl_ostringstream ss;
          ss << vcl_setw( 5 ) << vcl_setfill( '0' ) << k<<".txt";
          vcl_string filename = ss.str();
          vcl_ofstream ofile((outdir + "/" + filename).c_str());
          ofile<<cams[k];
          ofile.close();
      }
  }
  else
  {
      vcl_cout<<"Incorrect arguments "<<vcl_endl;
      return false;
  }
  return true;
}

