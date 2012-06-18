// This is brl/bseg/boxm2/pro/processes/boxm2_add_aux_info_to_ply_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process to ad auxiliary information (vis, prob, nmag) the vertices in a given ply file.
//
// \author Isabel Restrepo
// \date April 30, 2012

#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_cache.h>

#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>
#include <boct/boct_bit_tree.h>

#include <vcl_fstream.h>
#include <vcl_cassert.h>

#include <rply.h>

namespace boxm2_add_aux_info_to_ply_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;

  //helper class to read in bb from file
  class ply_points_reader
  {
   public:
    vcl_vector<vgl_point_3d<double> > points;
    double p[3];
  };

  //: Call-back function for a "vertex" element
  int plyio_vertex_cb(p_ply_argument argument)
  {
    long index;
    void* temp;
    ply_get_argument_user_data(argument, &temp, &index);

    ply_points_reader* parsed_ply =  (ply_points_reader*) temp;

    switch (index)
    {
      case 0: // "x" coordinate
        parsed_ply->p[0] = (double)ply_get_argument_value(argument);
        break;
      case 1: // "y" coordinate
        parsed_ply->p[1] =(double)ply_get_argument_value(argument);
        break;
      case 2: // "z" coordinate
        parsed_ply->p[2]= (double)ply_get_argument_value(argument);
        // Insert point
        parsed_ply->points.push_back(vgl_point_3d<double>(parsed_ply->p));
        break;
      default:
        assert(!"This should not happen: index out of range");
    }
    return 1;
  }

  //: The PLY reader of PCL is rather strict, so lets load the cloud on our own
  bool read_points_from_ply(const vcl_string &filename, vcl_vector<vgl_point_3d<double> > &points)
  {
    ply_points_reader parsed_ply;
    parsed_ply.points = points;

    p_ply ply = ply_open(filename.c_str(), NULL, 0, NULL);
    if (!ply) {
      vcl_cout << "File " << filename << " doesn't exist.";
      return false;
    }
    if (!ply_read_header(ply)){
      vcl_cout << "File " << filename << " doesn't have header.";
      return false;
    }

    // vertex
    int nvertices = ply_set_read_cb(ply, "vertex", "x", plyio_vertex_cb, (void*) (&parsed_ply), 0);
    ply_set_read_cb(ply, "vertex", "y", plyio_vertex_cb, (void*) (&parsed_ply), 1);
    ply_set_read_cb(ply, "vertex", "z", plyio_vertex_cb, (void*) (&parsed_ply), 2);

    vcl_cout << "Parsed: " << nvertices << "points\n";

    // Read DATA
    ply_read(ply);

    // CLOSE file
    ply_close(ply);

    points=parsed_ply.points;

    return true;
  }
}

bool boxm2_add_aux_info_to_ply_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_add_aux_info_to_ply_process_globals;

  //process takes 4 or 5 inputs and no outputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "boxm2_cache_sptr";
  input_types_[2] = "vcl_string"; //input PLY filename
  input_types_[3] = "vcl_string"; //output PLY filename

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}

bool boxm2_add_aux_info_to_ply_process(bprb_func_process& pro)
{
  using namespace boxm2_add_aux_info_to_ply_process_globals;

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

  vcl_string data_type;
  vcl_vector<vcl_string> apps = scene->appearances();

  //read incoming ply -- only points are read. Normals, prob, vis and color are read from the scene
  vcl_vector<vgl_point_3d<double> > points;
  read_points_from_ply(input_mesh_filename, points);

  //write outgoing mesh header
  p_ply oply = ply_create(output_mesh_filename.c_str(), PLY_ASCII, NULL, 0, NULL);

  // HEADER SECTION
  // vertex
  ply_add_element(oply, "vertex", points.size());
  ply_add_scalar_property(oply, "x", PLY_FLOAT); //PLY_FLOAT
  ply_add_scalar_property(oply, "y", PLY_FLOAT); //PLY_FLOAT
  ply_add_scalar_property(oply, "z", PLY_FLOAT); //PLY_FLOAT
  ply_add_scalar_property(oply, "nx", PLY_FLOAT); //PLY_FLOAT
  ply_add_scalar_property(oply, "ny", PLY_FLOAT); //PLY_FLOAT
  ply_add_scalar_property(oply, "nz", PLY_FLOAT); //PLY_FLOAT
  ply_add_scalar_property(oply, "prob", PLY_FLOAT); //PLY_FLOAT
  ply_add_scalar_property(oply, "vis", PLY_FLOAT); //PLY_FLOAT
  ply_add_scalar_property(oply, "nmag", PLY_FLOAT); //PLY_FLOAT
  ply_add_scalar_property(oply, "diffuse_red", PLY_UCHAR); //PLY_UCHAR
  ply_add_scalar_property(oply, "diffuse_green", PLY_UCHAR); //PLY_UCHAR
  ply_add_scalar_property(oply, "diffuse_blue", PLY_UCHAR); //PLY_UCHAR
  ply_add_scalar_property(oply, "tree_depth", PLY_UCHAR); //PLY_UCHAR

  // end header
  ply_write_header(oply);

  vcl_cout << "Start iterating over pts..." << vcl_endl;

  //get data sizes
  vcl_size_t alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
//vcl_size_t pointTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_POINT>::prefix()); // UNUSED!! -- fixme
  vcl_size_t normalTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL>::prefix());
  vcl_size_t visTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_VIS_SCORE>::prefix());
  int mogSize = (int) boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());

  //iterate through the points
  float prob;
  vnl_vector_fixed<float,3> intensity;
  vgl_point_3d<double> local;
  boxm2_block_id id;
  for (unsigned  i = 0; i < points.size(); i++) {

    const vgl_point_3d<double> pt = points[i];

    if (!scene->contains(pt, id, local)) {
      vcl_cout << "ERROR: point: " << pt << " isn't in scene. Exiting...." << vcl_endl;
      return false;
    }

    int index_x=(int)vcl_floor(local.x());
    int index_y=(int)vcl_floor(local.y());
    int index_z=(int)vcl_floor(local.z());

    boxm2_block * blk=cache->get_block(id);
    boxm2_block_metadata mdata = scene->get_block_metadata_const(id);
    vnl_vector_fixed<unsigned char,16> treebits=blk->trees()(index_x,index_y,index_z);
    boct_bit_tree tree(treebits.data_block(),mdata.max_level_);
    int bit_index=tree.traverse(local);

    int depth=tree.depth_at(bit_index);
    if (!tree.is_leaf(bit_index)) {
      vcl_cout << "ERROR: point: " << pt << " isn't a leaf cell...." << vcl_endl;
      //return false;
    }

    //get the base data
    int data_offset=tree.get_data_index(bit_index,false);
    boxm2_data_base * alpha_base = cache->get_data_base(id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    int data_buff_length = (int) (alpha_base->buffer_length()/alphaTypeSize);

//  boxm2_data_base * points = cache->get_data_base(id,boxm2_data_traits<BOXM2_POINT>::prefix(), data_buff_length * pointTypeSize); // UNUSED!! -- fixme
    boxm2_data_base * normals = cache->get_data_base(id,boxm2_data_traits<BOXM2_NORMAL>::prefix(), data_buff_length * normalTypeSize);
    boxm2_data_base * vis = cache->get_data_base(id,boxm2_data_traits<BOXM2_VIS_SCORE>::prefix(), data_buff_length * visTypeSize);
    boxm2_data_base * mog = cache->get_data_base(id,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix(), data_buff_length * mogSize);

    //get the actual data
    boxm2_data_traits<BOXM2_ALPHA>::datatype * alpha_data = (boxm2_data_traits<BOXM2_ALPHA>::datatype*) alpha_base->data_buffer();
//  boxm2_data_traits<BOXM2_POINT>::datatype * points_data = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer(); // UNUSED!! -- fixme
    boxm2_data_traits<BOXM2_NORMAL>::datatype * normals_data = (boxm2_data_traits<BOXM2_NORMAL>::datatype*) normals->data_buffer();
    boxm2_data_traits<BOXM2_VIS_SCORE>::datatype * vis_data = (boxm2_data_traits<BOXM2_VIS_SCORE>::datatype*) vis->data_buffer();
    boxm2_data_traits<BOXM2_MOG3_GREY>::datatype * mog_data = (boxm2_data_traits<BOXM2_MOG3_GREY>::datatype*) mog->data_buffer();

    float alpha=alpha_data[data_offset];
    double side_len = 1.0 / (double) (1 << depth);
    //store cell probability
    prob = 1.0f - (float)vcl_exp(-alpha * side_len * mdata.sub_block_dim_.x());
    unsigned char intensity = (unsigned char)(boxm2_mog3_grey_processor::expected_color(mog_data[data_offset])*255.0f);

    ply_write(oply, pt.x());
    ply_write(oply, pt.y());
    ply_write(oply, pt.z());
    ply_write(oply, normals_data[data_offset][0]);
    ply_write(oply, normals_data[data_offset][1]);
    ply_write(oply, normals_data[data_offset][2]);
    ply_write(oply, prob);
    ply_write(oply, vis_data[data_offset]);
    ply_write(oply, normals_data[data_offset][3]);
    ply_write(oply, (unsigned char)(intensity) );
    ply_write(oply, (unsigned char)(intensity) );
    ply_write(oply, (unsigned char)(intensity));
    ply_write(oply, (unsigned char) depth);
  }
  vcl_cout << "Done iterating over pts..." << vcl_endl;

   // CLOSE PLY FILE
  ply_close(oply);

  return true;
}
