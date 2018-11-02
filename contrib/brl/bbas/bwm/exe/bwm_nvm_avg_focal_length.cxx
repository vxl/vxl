#include <iostream>
#include <fstream>
#include <string>
#include <bwm/bwm_observer_cam.h>
#include <bwm/bwm_site_mgr.h>
#include <bwm/video/bwm_video_cam_ostream.h>
#include <bwm/video/bwm_video_corr_sptr.h>
#include <bwm/video/bwm_video_corr.h>
#include <bwm/video/bwm_video_site_io.h>

#include <vul/vul_arg.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool NVMreplaceByAvgFocalLength(std::ifstream& in1,
                                std::ifstream& in2,
                                std::ofstream& out)
{
  int rotation_parameter_num = 4;
  std::string token;
  // bool format_r9t = false; // unused
  if (in1.peek() == 'N')
  {
    in1 >> token; //file header
    if (std::strstr(token.c_str(), "R9T"))
    {
      rotation_parameter_num = 9;    //rotation as 3x3 matrix
      // format_r9t = true;
    }
  }
  int ncam = 0;
  // read # of cameras
  in1 >> ncam;  if (ncam <= 1) return false;

  float count = 0;
  float sum_focals = 0.0;
  for (int i = 0; i < ncam; ++i)
  {
    double f, q[9], c[3], d[2];
    in1 >> token >> f ;

    if (f> 0.0)
    {
      sum_focals += f;
      count = count + 1.0f;
    }
    for (int j = 0; j < rotation_parameter_num; ++j) in1 >> q[j];
    in1 >> c[0] >> c[1] >> c[2] >> d[0] >> d[1];
  }

  float avg_focal_length = 0.0;
  if (count > 0.0 )
      avg_focal_length = sum_focals/count ;

  in1.close();
  //======================================================
  if (in2.peek() == 'N')
  {
    in2 >> token; //file header
    if (std::strstr(token.c_str(), "R9T"))
    {
      rotation_parameter_num = 9;    //rotation as 3x3 matrix
      // format_r9t = true;
    }
  }
  ncam = 0;
  // read # of cameras
  in2 >> ncam;  if (ncam <= 1) return false;

  out<<token<<'\n'
     <<ncam<<'\n';

  //read the camera parameters

  for (int i = 0; i < ncam; ++i)
  {
    double f, q[9], c[3], d[2];
    in2 >> token >> f ;

    for (int j = 0; j < rotation_parameter_num; ++j) in2 >> q[j];
    in2 >> c[0] >> c[1] >> c[2] >> d[0] >> d[1];

    out << token << '\t' << avg_focal_length << '\t';
    for (int j = 0; j < rotation_parameter_num; ++j) out << q[j]<<'\t';
    out << c[0] <<'\t'<< c[1] <<'\t'<< c[2] <<'\t'<< d[0] <<'\t'<< d[1]<<'\n';
  }

  //////////////////////////////////////
  int npoint = 0;
  in2 >> npoint;   if (npoint <= 0) return false;

  out << npoint << '\n';

  //read image projections and 3D points.
  for (int i = 0; i < npoint; ++i)
  {
    float pt[3]; int cc[3], npj;
    in2  >> pt[0] >> pt[1] >> pt[2]
         >> cc[0] >> cc[1] >> cc[2] >> npj;
    out << pt[0] <<'\t'<< pt[1] <<'\t'<< pt[2]
        <<'\t'<< cc[0] <<'\t'<< cc[1] <<'\t'<< cc[2] <<'\t'<< npj<<'\t';

    for (int j = 0; j < npj; ++j)
    {
      int cidx, fidx; float imx, imy;
      in2 >> cidx >> fidx >> imx >> imy;
      out <<'\t'<< cidx <<'\t'<< fidx <<'\t'<< imx <<'\t'<< imy;
    }
    out<<'\n';
  }
  ////////////////////////////////////////////////////////////////////////////
  return true;
}

// An executable that read bundler file and convert it into video site.
int main(int argc, char** argv)
{
  //Get Inputs

  vul_arg<std::string> infile   ("-isfm", "Input file of bundler",  "");
  vul_arg<std::string> outfile   ("-osfm", "Output file of bundler",  "");

  vul_arg_parse(argc, argv);

  // open the bundler file
  std::ifstream ifile1( infile().c_str() );
  if (!ifile1)
  {
    std::cout<<"Error Opening NVM input file"<<std::endl;
    return -1;
  }
  std::ifstream ifile2( infile().c_str() );
  if (!ifile2)
  {
    std::cout<<"Error Opening NVM input file"<<std::endl;
    return -1;
  }
  std::ofstream ofile( outfile().c_str() );
  if (!ofile)
  {
    std::cout<<"Error Opening NVM output file"<<std::endl;
    return -1;
  }

  NVMreplaceByAvgFocalLength(ifile1,ifile2,ofile);

  return 0;
}
