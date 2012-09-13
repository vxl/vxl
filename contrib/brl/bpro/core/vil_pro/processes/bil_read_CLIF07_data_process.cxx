// This is brl/bpro/core/vil_pro/processes/bil_read_CLIF07_data_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>

#include <bil/bil_raw_image_istream.h>
#include <vcl_sstream.h>
#include <vcl_cstdio.h>

//: Constructor
bool bil_read_CLIF07_data_process_cons(bprb_func_process& pro)
{
  //process takes 1 input
  vcl_vector<vcl_string> input_types_(3);
  input_types_[0] = "vcl_string"; //raw file dir
  input_types_[1] = "vcl_string"; //out dir
   input_types_[2] = "int"; //camer number
  // process has 1 output:
  vcl_vector<vcl_string>  output_types_(0);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: Execute the process
bool bil_read_CLIF07_data_process(bprb_func_process& pro)
{
  int numRows = 2672, numCols = 4016;
  vcl_string in_dir = pro.get_input<vcl_string> (0);
  vcl_string out_dir = pro.get_input<vcl_string> (1);
  int  camera_number = pro.get_input<int> (2);
  unsigned startFrame = 100006, endFrame = 100500;

  bool flipRowCol = true; // for all FPAs
  bool rotate180 = camera_number%2 ?true:false; // for even FPAs

  vcl_vector<vcl_string > imgNames;
  for (unsigned int n = startFrame; n < endFrame; ++n) {
    char  filename[1024];
    vcl_sprintf(filename, "%s/00000%d-%06d.raw",in_dir.c_str(),camera_number,n);
    //vcl_stringstream ss;
    // ss << in_dir << "/00000"<<camera_number<<'-'<<vcl_setfill( '0' ) << vcl_setw(6) << n << ".raw";
    imgNames.push_back( vcl_string(filename));
  }

  vil_image_view<unsigned char> img( numCols, numRows );
  int f = startFrame;
  for (unsigned int n = 0; n < imgNames.size(); n++, ++f) {
    vcl_cerr << imgNames[n] << '\n';

    vcl_ifstream ifs( imgNames[n].c_str(), vcl_ios::binary );
    // ifs.seekg (0, vcl_ios::end);
    // int length = ifs.tellg(); // unused!
    ifs.seekg (0, vcl_ios::beg);
    if ( ifs.bad() ) vcl_cerr << "BAD FILE\n";

    vil_memory_chunk_sptr mem_chunk = new vil_memory_chunk(numRows*numCols,VIL_PIXEL_FORMAT_BYTE);
    ifs.read( (char*) mem_chunk->data(), numRows*numCols );
    vil_image_view<unsigned char> img(mem_chunk, (unsigned char*) mem_chunk->data(), numCols, numRows, 1, 1, numCols, numCols*numRows);

    char  filename[1024];
    vcl_sprintf(filename, "%s/00000%d-%06d.png",out_dir.c_str(),camera_number,startFrame+n);

    //vcl_stringstream ss;
    //ss << out_dir <<"/00000"<<camera_number<<'-'<<vcl_setfill( '0' ) << vcl_setw(6) << n << ".png";
    if ( rotate180 ){
      vil_image_view<unsigned char> img2( numCols, numRows );
      for ( int i = 0; i < numCols; i++ )
        for ( int j = 0; j < numRows; j++ )
          img2(i,j) = img(numCols-1-i,numRows-1-j);
      img = img2;
    }
    vcl_cout<<"output ";vcl_cout.flush();

    if (flipRowCol) {
      vil_image_view<unsigned char> img2( numRows, numCols );
      for ( int i = 0; i < numCols; i++ )
        for ( int j = 0; j < numRows; j++ )
          img2(j,i) = img(i,j);
      vil_save( img2, filename);
    }
    else
      vil_save( img, filename );
  }
  return true;
}
