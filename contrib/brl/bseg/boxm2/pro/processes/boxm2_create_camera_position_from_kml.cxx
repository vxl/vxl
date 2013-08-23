// This is brl/bseg/boxm2/pro/processes/boxm2_create_stream_cache_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process that reads the path ground coordiantes (wgs84) from kml file for creating camera ground position relative to scene coordination.
//         Note that the path in google kml only contains latitude and longtitude and therefore the output txt file contains only x and y ground position
// \author Yi Dong
// \date September 10, 2012
//
// \verybatim
//  Modifications
//   <none yet>
// \endverbatim
#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <boxm2/boxm2_scene.h>
#include <bkml/bkml_parser.h>
#include <bpgl/bpgl_camera_utils.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_line_2d.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <vcl_iomanip.h>

namespace boxm2_create_camera_from_kml_path_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;
}

bool boxm2_create_camera_from_kml_path_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_create_camera_from_kml_path_process_globals;
  //process takes 4 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";      // kml filename where camera path is stored
  input_types_[1] = "vpgl_lvcs_sptr";  // scene local coordinates
  input_types_[2] = "unsigned";        // number of camera position along each path segment
  input_types_[3] = "vcl_string";      // output txt file which contains camera coordinates on the ground
  // process has 0 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_create_camera_from_kml_path_process(bprb_func_process& pro)
{
  using namespace boxm2_create_camera_from_kml_path_process_globals;

  if(pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  vcl_string cam_path = pro.get_input<vcl_string>(i++);
  vpgl_lvcs_sptr lvcs = pro.get_input<vpgl_lvcs_sptr>(i++);
     unsigned cam_num = pro.get_input<unsigned>(i++);
  vcl_string out_path = pro.get_input<vcl_string>(i++);

  // read the path from kml file
  bkml_parser* parser = new bkml_parser();
  vcl_FILE* kmlFile = vcl_fopen(cam_path.c_str(), "r");
  if(!kmlFile){
    vcl_cerr << cam_path.c_str() << " error on opening the input kml file\n";
    delete parser;
    return false;
  }
  if(!parser->parseFile(kmlFile)){
    vcl_cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
         << parser->XML_GetCurrentLineNumber() << '\n';
    delete parser;
    return false;
  }
  if(parser->linecord_[0].size()<2){
    vcl_cerr << "error: input kml has NO path\n";
    delete parser;
    return false;
  }
  // transfer from global wgs84 to local lvcs
  vcl_vector<vgl_point_2d<double> > vp;
  
  for(unsigned i=0; i<(unsigned)parser->linecord_[0].size(); i++){
    double local_x, local_y, local_z;
    vcl_cout << " geo_coord = " << parser->linecord_[0][i] << vcl_endl;
    lvcs->global_to_local(parser->linecord_[0][i].x(),parser->linecord_[0][i].y(), parser->linecord_[0][i].z(),
      vpgl_lvcs::wgs84, local_x, local_y, local_z);
    vp.push_back(vgl_point_2d<double>(local_x, local_y));
  }

  // calculate the camera position along the path
  vcl_vector<vgl_point_2d<double> > cam_pos;
  for(unsigned int i=0; i<((unsigned int)vp.size()-1);i++){
    vgl_point_2d<double> startp = vp[i];
    vgl_point_2d<double> endp = vp[i+1];
    vgl_line_2d<double> line(startp, endp);
    double length = (endp - startp).length()/cam_num;
    vgl_vector_2d<double> vs2e = line.direction();
    for(unsigned int ci=0; ci<cam_num; ci++)
      cam_pos.push_back(startp + ci * length * vs2e);
  }
  // add the last point along the path
  cam_pos.push_back(vp[vp.size()-1]); 

  // write it into the txt file
  vcl_ofstream ofs(out_path.c_str());
  for(unsigned int i=0; i<(unsigned int)cam_pos.size(); i++){
    ofs << vcl_setprecision(10) << vcl_setw(15) << cam_pos[i].x() << "     " << cam_pos[i].y() << vcl_endl;
  }

  ofs.close();
  delete parser;
  return true;
}