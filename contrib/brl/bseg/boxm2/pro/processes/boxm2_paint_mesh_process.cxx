// This is brl/bseg/boxm2/pro/processes/boxm2_paint_mesh_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for assigning colors to the vertices of a given mesh. There is also a threshold prob_t on probability of points to output.
//         Points with lower probability than prob_t are assigned a color of (0,0,0).
//
// \author Ali Osman Ulusoy
// \date Mar 02, 2012

#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_cache.h>

#include <bvrml/bvrml_write.h>
#include "boxm2/cpp/algo/boxm2_export_vis_wrl_function.h"

#include <bmsh3d/algo/bmsh3d_fileio.h>

#include <rply.h>

namespace boxm2_paint_mesh_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 0;
}

bool boxm2_paint_mesh_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_paint_mesh_process_globals;

  //process takes 4 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vcl_string"; //input mesh filename
  input_types_[3] = "vcl_string"; //output mesh filename
  input_types_[4] = "float"; //prob threshold

  brdb_value_sptr prob_t = new brdb_value_t<float>(0.0);
  pro.set_input(4, prob_t);

  // process has no outputs
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_paint_mesh_process(bprb_func_process& pro)
{
  using namespace boxm2_paint_mesh_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  vcl_string input_mesh_filename = pro.get_input<vcl_string>(i++);
  vcl_string output_mesh_filename = pro.get_input<vcl_string>(i++);
  float prob_t = pro.get_input<float>(i++);

  bool foundDataType = false;

  vcl_string data_type;
  vcl_vector<vcl_string> apps = scene->appearances();
  int appTypeSize = 0; // just to avoid compiler warning about using potentially uninitialised value
  for (unsigned int i=0; i<apps.size(); ++i) {
      if ( apps[i] == boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix() )
      {
          data_type = apps[i];
          foundDataType = true;
          appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix());
      }
  }

  if (!foundDataType) {
      vcl_cout<<"BOXM2_PAINT_MESH_PROCESS ERROR: scene doesn't have appearance..."<<vcl_endl;
      return false;
  }

  //read incoming mesh
  bmsh3d_mesh input_mesh;
  bmsh3d_load_ply (&input_mesh, input_mesh_filename.c_str());
  input_mesh.assign_IFS_vertex_vid_by_vertex_order();

  //print input mesh summary
  input_mesh.print_summary(vcl_cout);


  //write outgoing mesh header
  p_ply oply = ply_create(output_mesh_filename.c_str(), PLY_ASCII, NULL, 0, NULL);


  // HEADER SECTION
  // vertex
  ply_add_element(oply, "vertex", input_mesh.vertexmap().size());
  ply_add_scalar_property(oply, "x", PLY_FLOAT); //PLY_FLOAT
  ply_add_scalar_property(oply, "y", PLY_FLOAT); //PLY_FLOAT
  ply_add_scalar_property(oply, "z", PLY_FLOAT); //PLY_FLOAT
  ply_add_scalar_property(oply, "diffuse_red", PLY_UCHAR); //PLY_UCHAR
  ply_add_scalar_property(oply, "diffuse_green", PLY_UCHAR); //PLY_UCHAR
  ply_add_scalar_property(oply, "diffuse_blue", PLY_UCHAR); //PLY_UCHAR

  // face
  ply_add_element(oply, "face", input_mesh.facemap().size());
  ply_add_list_property(oply, "vertex_indices", PLY_UCHAR, PLY_INT);


  // end header
  ply_write_header(oply);


  vcl_cout << "Start iterating over pts..." << vcl_endl;
  //zip thru points
  float prob;
  vnl_vector_fixed<float,3> intensity;
  for(unsigned  i = 0; i < input_mesh.num_vertices(); i++) {
    const vgl_point_3d<double> pt = input_mesh.vertexmap(i)->pt();
    boxm2_util::query_point(scene,cache,pt, prob, intensity);

    ply_write(oply, pt.x());
    ply_write(oply, pt.y());
    ply_write(oply, pt.z());

    if(prob >= prob_t) {
      ply_write(oply, (unsigned char)(intensity[0]*255.0f) );
      ply_write(oply, (unsigned char)(intensity[1]*255.0f) );
      ply_write(oply, (unsigned char)(intensity[2]*255.0f) );
    }
    else {
      ply_write(oply, 0 );
      ply_write(oply, 0 );
      ply_write(oply, 0 );
    }

  }
  vcl_cout << "Done iterating over pts..." << vcl_endl;

  // faces
  vcl_map<int, bmsh3d_face*>::iterator fit = input_mesh.facemap().begin();
  for (; fit != input_mesh.facemap().end(); fit++)
  {
    bmsh3d_face* f = (*fit).second;

    // a. write the number of vertices of the face
    ply_write(oply,f->vertices().size());

    // b. write id's of the vertices
    for (unsigned int j=0; j< f->vertices().size(); ++j)
    {
      bmsh3d_vertex* v = f->vertices(j);
      assert(v);
      ply_write(oply, v->vid());
    }
  }


  // CLOSE PLY FILE
  ply_close(oply);


  return true;
}

