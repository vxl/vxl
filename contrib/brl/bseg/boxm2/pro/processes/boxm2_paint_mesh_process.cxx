// This is brl/bseg/boxm2/pro/processes/boxm2_paint_mesh_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for assigning colors to the vertices of a given mesh.
// There is also a threshold prob_t on probability of points to output.
// Points with lower probability than prob_t are assigned a color of (0,0,0).
//
// \author Ali Osman Ulusoy
// \date Mar 02, 2012

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_cache.h>

#include <bvrml/bvrml_write.h>
#include <boxm2/cpp/algo/boxm2_gauss_rgb_processor.h>

#include <bmsh3d/algo/bmsh3d_fileio.h>
#include <boct/boct_bit_tree.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <rply.h>

namespace boxm2_paint_mesh_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_paint_mesh_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_paint_mesh_process_globals;

  //process takes 4 or 5 inputs and no outputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vcl_string"; //input mesh filename
  input_types_[3] = "vcl_string"; //output mesh filename
  input_types_[4] = "float"; //prob threshold

  brdb_value_sptr prob_t = new brdb_value_t<float>(0.0);
  pro.set_input(4, prob_t);

  std::vector<std::string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}

bool boxm2_paint_mesh_process(bprb_func_process& pro)
{
  using namespace boxm2_paint_mesh_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_cache_sptr cache = pro.get_input<boxm2_cache_sptr>(i++);
  std::string input_mesh_filename = pro.get_input<std::string>(i++);
  std::string output_mesh_filename = pro.get_input<std::string>(i++);
  auto prob_t = pro.get_input<float>(i++);

  bool foundDataType = false;

  std::string data_type;
  std::vector<std::string> apps = scene->appearances();
  for (const auto & app : apps) {
      if ( app == boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix() )
      {
          data_type = app;
          foundDataType = true;
          // boxm2_data_info::datasize(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix());
      }
  }

  if (!foundDataType) {
      std::cout<<"BOXM2_PAINT_MESH_PROCESS ERROR: scene doesn't have GAUSS_RGB appearance..."<<std::endl;
      return false;
  }

  //read incoming mesh
  bmsh3d_mesh input_mesh;
  bmsh3d_load_ply (&input_mesh, input_mesh_filename.c_str());
  input_mesh.assign_IFS_vertex_vid_by_vertex_order();

  //print input mesh summary
  input_mesh.print_summary(std::cout);

  //write outgoing mesh header
  p_ply oply = ply_create(output_mesh_filename.c_str(), PLY_ASCII, nullptr, 0, nullptr);

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

  std::cout << "Start iterating over pts..." << std::endl;
  //zip thru points
  float prob;
  vnl_vector_fixed<float,3> intensity;
  vgl_point_3d<double> local;
  boxm2_block_id id;
  for (unsigned  i = 0; i < input_mesh.num_vertices(); i++) {
    const vgl_point_3d<double> pt = input_mesh.vertexmap(i)->pt();

    if (!scene->contains(pt, id, local)) {
      std::cout << "ERROR: point: " << pt << " isn't in scene. Exiting...." << std::endl;
      return false;
    }

    int index_x=(int)std::floor(local.x());
    int index_y=(int)std::floor(local.y());
    int index_z=(int)std::floor(local.z());
    boxm2_block * blk=cache->get_block(scene,id);
    boxm2_block_metadata mdata = scene->get_block_metadata_const(id);
    vnl_vector_fixed<unsigned char,16> treebits=blk->trees()(index_x,index_y,index_z);
    boct_bit_tree tree(treebits.data_block(),mdata.max_level_);
    int bit_index=tree.traverse(local);

    int depth=tree.depth_at(bit_index);

    int data_offset=tree.get_data_index(bit_index,false);
    boxm2_data_base *  alpha_base = cache->get_data_base(scene,id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    boxm2_data<BOXM2_ALPHA> *alpha_data=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),alpha_base->buffer_length(),alpha_base->block_id());

    float alpha=(alpha_data->data())[data_offset];
    double side_len = 1.0 / (double) (1 << depth);
    //store cell probability
    prob = 1.0f - (float)std::exp(-alpha * side_len * mdata.sub_block_dim_.x());

    boxm2_data_base *  int_base = cache->get_data_base(scene,id, data_type);
    if (data_type.find(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) != std::string::npos) {
      boxm2_data<BOXM2_GAUSS_RGB> *int_data = new boxm2_data<BOXM2_GAUSS_RGB>(int_base->data_buffer(),int_base->buffer_length(),int_base->block_id());
      intensity = boxm2_gauss_rgb_processor::expected_color( (int_data->data())[data_offset]);
    }

    ply_write(oply, pt.x());
    ply_write(oply, pt.y());
    ply_write(oply, pt.z());

    if (prob >= prob_t) {
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
  std::cout << "Done iterating over pts..." << std::endl;

  // faces
  auto fit = input_mesh.facemap().begin();
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
