//:
// \file
// \brief A process that creates a boxm scene from a .ply file
// \author Isabel Restrepo
// \date 2-Jan-2012

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <boxm/boxm_scene.h>
#include <rply.h>   //.ply parser
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boct/util/boct_construct_tree.h>
#define DEBUG_PLY

class boxm_vgl_point_3d_coord_compare
{
 public:
  boxm_vgl_point_3d_coord_compare() = default;
  ~boxm_vgl_point_3d_coord_compare() = default;

  bool operator() ( vgl_point_3d<int> const& pa, vgl_point_3d<int> pb ) const
  {
    if      ( pa.x() != pb.x() ) return pa.x() < pb.x();
    else if ( pa.y() != pb.y() ) return pa.y() < pb.y();
    else                         return pa.z() < pb.z();
  }
};


class boxm_apperace_fileio_parsed_ply_
{
 public:
  double p[3];  //holds location
  unsigned char color; //holds color
  double grey_offset; //offset the floating point (0,1) color value
  //accumulated data in the shape of leaf cells
  std::map<vgl_point_3d<int>, std::vector<boct_tree_cell<short, float> >, boxm_vgl_point_3d_coord_compare > data;
  boxm_scene<boct_tree<short, float> > *scene;
};

bool boxm_load_color_ply(const std::string &ply_file,  boxm_scene<boct_tree<short, float> > *scene,
                         std::map<vgl_point_3d<int>,
                         std::vector<boct_tree_cell<short, float> >,boxm_vgl_point_3d_coord_compare > &data,
                         const float &grey_offset);

//: Call-back function for a "vertex" element
int boxm_plyio_vertex_cb_(p_ply_argument argument);

//: global variables
namespace boxm_create_scene_from_ply_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}


//: sets input and output types
bool boxm_create_scene_from_ply_process_cons(bprb_func_process& pro)
{
  using namespace boxm_create_scene_from_ply_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; //path to .ply file
  input_types_[1] = "boxm_scene_base_sptr"; //the scene to fill up
  input_types_[2] = "float"; //offset for appearance values

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: the process
bool boxm_create_scene_from_ply_process(bprb_func_process& pro)
{
  using namespace boxm_create_scene_from_ply_process_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << ": the input number should be " << n_inputs_
             << " but instead it is " << pro.n_inputs() << std::endl;
    return false;
  }

  //get inputs:
  std::string ply_file = pro.get_input<std::string>(0);
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(1);
  auto grey_offset = pro.get_input<float>(2);

  // Note initial implementation is for fixed types, but this can be changed if more cases are needed
  // cast input scene
  auto *scene = dynamic_cast<boxm_scene<boct_tree<short, float> >* > (scene_base.as_pointer());

  //check input's validity
  if (!scene_base.ptr()) {
    std::cout <<  " :-- Scene is not valid!\n";
    return false;
  }

  // Map to hold the index of block and the cell containing appearance (0,1)
  std::map<vgl_point_3d<int>, std::vector<boct_tree_cell<short, float> >, boxm_vgl_point_3d_coord_compare > data;

  boxm_block_iterator<boct_tree<short, float> > block_iter = scene->iterator();

  for (block_iter.begin(); !block_iter.end(); ++block_iter)
  {
    data[block_iter.index()]= std::vector<boct_tree_cell<short, float> >();
  }

  if (!boxm_load_color_ply(ply_file, scene, data, grey_offset)){
    pro.set_output_val<boxm_scene_base_sptr>(0, nullptr);
    return false;
  }


  //construct trees
  for (block_iter.begin(); !block_iter.end(); ++block_iter)
  {
    std::cout << "In block: " << block_iter.index() << "Number of leaves: " <<data[block_iter.index()].size() << std::endl;
    boct_tree_cell<short, float>* root = boct_construct_tree(data[block_iter.index()], scene->max_level(), 0.0f );
    auto* tree = new boct_tree<short,float>(root,  scene->max_level());
    vgl_box_3d<double> tree_bbox= scene->get_block_bbox(block_iter.index());
    tree->set_bbox(tree_bbox);

    scene->load_block(block_iter.index());
    (*block_iter)->init_tree(tree);
    scene->write_active_block();
    data[block_iter.index()].clear();
  }

  pro.set_output_val<boxm_scene_base_sptr>(0, scene);

  return true;
}


// ============================== PLY ==============================


bool boxm_load_color_ply(const std::string &ply_file,  boxm_scene<boct_tree<short, float> > *scene,
                         std::map<vgl_point_3d<int>,
                         std::vector<boct_tree_cell<short, float> >,boxm_vgl_point_3d_coord_compare > &data,
                         const float &grey_offset)
{
  long nvertices;

  boxm_apperace_fileio_parsed_ply_ parsed_ply;
  parsed_ply.grey_offset = grey_offset;
  parsed_ply.data = data;
  parsed_ply.scene = scene;


  // OPEN file
#ifdef DEBUG_PLY
  std::cerr << " loading " << ply_file << " :\n";
  std::cerr << " gray_offset " << parsed_ply.grey_offset << grey_offset << " :\n";

#endif

  p_ply ply = ply_open(ply_file.c_str(), nullptr, 0, nullptr);
  if (!ply){
    std::cerr << "Couldn't open ply file: " << ply_file << '\n';
    return false;
  }

  // Read HEADER
  if (!ply_read_header(ply)) return false;

  // vertex
  nvertices =
  ply_set_read_cb(ply, "vertex", "x",
                  boxm_plyio_vertex_cb_, (void*) (&parsed_ply), 0);
  ply_set_read_cb(ply, "vertex", "y",
                  boxm_plyio_vertex_cb_, (void*) (&parsed_ply), 1);
  ply_set_read_cb(ply, "vertex", "z",
                  boxm_plyio_vertex_cb_, (void*) (&parsed_ply), 2);
  ply_set_read_cb(ply, "vertex", "diffuse_red",
                  boxm_plyio_vertex_cb_, (void*) (&parsed_ply), 3);

#ifdef DEBUG_PLY
  std::cerr << nvertices << " points\n";
#endif

  // Read DATA
  if (!ply_read(ply)) return false;

  // CLOSE file
  ply_close(ply);

  data=parsed_ply.data;

#ifdef DEBUG_PLY
  std::cerr << "  done.\n";
#endif

  return true;
}


//: Call-back function for a "vertex" element
int boxm_plyio_vertex_cb_(p_ply_argument argument)
{
  long index;
  void* temp;
  ply_get_argument_user_data(argument, &temp, &index);

  auto* parsed_ply =  (boxm_apperace_fileio_parsed_ply_*) temp;
  //std::cout << "color: " <<  parsed_ply->grey_offset << std::endl;

  switch (index)
  {
    case 0: // "x" coordinate
      parsed_ply->p[0] = ply_get_argument_value(argument);
      break;
    case 1: // "y" coordinate
      parsed_ply->p[1] = ply_get_argument_value(argument);
      break;
    case 2: // "z" coordinate
      parsed_ply->p[2] = ply_get_argument_value(argument);
      break;
    case 3:
    {
      parsed_ply->color = (unsigned char)ply_get_argument_value(argument);
      //insert data
      vgl_point_3d<int> block_index(-1,-1,-1);
      if (parsed_ply->scene->get_block_index(vgl_point_3d<double>(parsed_ply->p), block_index))
      {
        //convert point into local(tree coordinates)
        vgl_box_3d<double> global_bbox= parsed_ply->scene->get_block_bbox(block_index);
        vgl_point_3d<double> norm_p((parsed_ply->p[0]-global_bbox.min_x())/global_bbox.width(),
                                    (parsed_ply->p[1]-global_bbox.min_y())/global_bbox.height(),
                                    (parsed_ply->p[2]-global_bbox.min_z())/global_bbox.depth());
        // convert point to location code.
        boct_loc_code<short> loc_code(norm_p, parsed_ply->scene->max_level()-1);
        boct_tree_cell<short, float> leaf_cell(loc_code);
        float expected_color = float(parsed_ply->color)/255.0f + float(parsed_ply->grey_offset);
        if (expected_color > 2.0f || expected_color < 0.0f)
          std::cout << "color: " <<  parsed_ply->grey_offset << std::endl;
        leaf_cell.set_data(expected_color);
        parsed_ply->data[block_index].push_back(leaf_cell);
      }
      break;
    }
    default:
      assert(!"This should not happen: index out of range");
      break;
  }
  return 1;
}
