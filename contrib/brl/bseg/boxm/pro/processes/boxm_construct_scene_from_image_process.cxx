// This is brl/bseg/boxm/pro/processes/boxm_construct_scene_from_image_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Process to construct an ideal scene for testing
//
// \author J.L. Mundy
// \date January 23, 2010
// \verbatim
//  Modifications
// \endverbatim

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#include <boxm/sample/boxm_sample.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_box_3d.h>

namespace boxm_construct_scene_from_image_process_globals
{
  // The image area should be a multiple of area unit, e.g. 64.
  // This granularity is useful for parallel implementation
  template <class T>
  void tree_from_image(vil_image_view<float> const& image,
                       unsigned area_unit,
                       unsigned& base_ni, unsigned& base_nj,
                       boct_tree<short, T >*& tree)
  {
    tree = nullptr;
    unsigned ni = image.ni(), nj = image.nj();
    // find a square arrangement consistent with area unit
    auto gsize = static_cast<double>(area_unit);
    auto gwidth = static_cast<unsigned>(std::sqrt(gsize));
    unsigned gheight = area_unit/gwidth;
    if (gwidth*gheight!=gsize)
      return;//maybe fixup later to handle all situations
    unsigned mni = (ni/gwidth)*gwidth, mnj = (nj/gheight)*gheight;
    double min_dim = mni;
    if (mnj<min_dim)
      min_dim = mnj;
    double dlev = std::log(min_dim)/std::log(2.0);
    auto n_levels = static_cast<unsigned>(dlev+1.0);

    //note that tree is being constructed in local coordinates
    float rni = 1.0f/static_cast<float>(mni), rnj =1.0f/static_cast<float>(mnj);
    base_ni = mni; base_nj = mnj;
    std::vector<boct_loc_code<short> > leaf_codes;
    std::vector<float> image_int;
    float upper_v=float(mnj-1)*rnj;
    for (unsigned j=0; j<mnj; ++j)
      for (unsigned i=0; i<mni; ++i)
      {
        float u = static_cast<float>(i)*rni;
        float v = upper_v-static_cast<float>(j)*rnj;
        vgl_point_3d<double> p(u, v, 0.0f);
        boct_loc_code<short> loc(p, n_levels-1);
        leaf_codes.push_back(loc);
        image_int.push_back(image(i,j));
      }
    //construct leaves
    std::vector<boct_tree_cell<short, T > > leaves;
    for (auto & leaf_code : leaf_codes) {
      boct_tree_cell<short, T > leaf(leaf_code);
      leaves.push_back(leaf);
    }

    //construct tree from leaves
    auto *init_tree =
      new boct_tree<short, T >(n_levels, 0);
    boct_tree_cell<short, T > *root =
      init_tree->construct_tree(leaves, init_tree->number_levels());

    // construct full tree with leaves and root
    //set the global bounding box, assuming a box with depth ni
    vgl_box_3d<double> box;
    vgl_point_3d<double> p0(0,0,0);
    vgl_point_3d<double> p1(mni, mnj, mni);
    box.add(p0); box.add(p1);
    tree = new boct_tree<short, T >(root, init_tree->number_levels());
    tree->set_bbox(box);
    delete init_tree;

    // fill the leaves with data
    std::vector<boct_tree_cell<short, T >* > tleaves;
    tleaves = tree->leaf_cells();
    std::size_t i = 0;
    auto lit =
      tleaves.begin();
    for (; lit!= tleaves.end(); ++lit, ++i)
    {
      boct_loc_code<short> code = (*lit)->get_code();
      bool found = false;
      unsigned found_k = 0;
      for (unsigned k = 0; k<leaf_codes.size()&&!found; ++k)
        if (code == leaf_codes[k]) {
          found = true;
          found_k = k;
        }
      if (auto * cell_grey_ptr = reinterpret_cast<boct_tree_cell<short, boxm_sample<BOXM_APM_SIMPLE_GREY> >* >(*lit))
      {
        if (found) {
          boxm_sample<BOXM_APM_SIMPLE_GREY> data(1.0f,boxm_sample<BOXM_APM_SIMPLE_GREY>::apm_datatype(image_int[found_k],0.0008f));
          cell_grey_ptr->set_data(data);
        }
        else {
          boxm_sample<BOXM_APM_SIMPLE_GREY> data(0.0f,boxm_sample<BOXM_APM_SIMPLE_GREY>::apm_datatype(0.0f,0.0008f));
          cell_grey_ptr->set_data(data);
        }
      }
      else if (auto * cell_mog_grey_ptr = reinterpret_cast<boct_tree_cell<short, boxm_sample<BOXM_APM_MOG_GREY> >* >(*lit))
      {
        if (found) {
          boxm_sample<BOXM_APM_MOG_GREY> data(1.0f);
          typedef boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_processor aproc;
          aproc::update(data.appearance(),image_int[found_k],1.0);
          cell_mog_grey_ptr->set_data(data);
        }
        else {
          boxm_sample<BOXM_APM_MOG_GREY> data(0.0f);
          cell_mog_grey_ptr->set_data(data);
        }
      }
      else if (auto * cell_float_ptr
               =reinterpret_cast<boct_tree_cell<short, float >* >(*lit))
      {
        if (found) {
          float data(1.0f);
          cell_float_ptr->set_data(data);
        }
      }
    }
  }
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 1;
}

bool boxm_construct_scene_from_image_process_cons(bprb_func_process& pro)
{
  using namespace boxm_construct_scene_from_image_process_globals;

  //input[0]: The image view
  //input[1]: appearance model type
  //input[2]: scene directory
  //input[3]: scene xml path
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "vcl_string";
  input_types_[3] = "vcl_string";

  //output[0]: The scene
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]=  "boxm_scene_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_construct_scene_from_image_process(bprb_func_process& pro)
{
  using namespace boxm_construct_scene_from_image_process_globals;

  if ( !pro.verify_inputs()){
    std::cerr << pro.name() << "boxm_construct_scene_from_image_process: invalid inputs\n";
    return false;
  }

  //get inputs:
  unsigned i = 0;
  vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(i++);
  std::string app_type =  pro.get_input<std::string>(i++);
  std::string scene_dir =  pro.get_input<std::string>(i++);
  std::string xml_path =  pro.get_input<std::string>(i++);
  if (image == nullptr) {
    std::cerr << "boxm_construct_scene_from_image_process: null image value, cannot run\n";
    return false;
  }

  vil_image_view<float> img = *vil_convert_cast(float(), image);
  if (image->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
    vil_math_scale_values(img,1.0/255.0);

  unsigned base_ni =0, base_nj=0;
  std::string block_prefix = "ideal";
  boxm_scene_base_sptr scene_base = nullptr;
  if (app_type == "simple_grey")
  {
    typedef boxm_sample<BOXM_APM_SIMPLE_GREY> data_type;
    boct_tree<short, data_type>* tree = nullptr;
    tree_from_image<data_type>(img, 16, base_ni, base_nj,tree);
    if (tree == nullptr) return false;
    boxm_block<boct_tree<short, data_type> >* blk =
      new boxm_block<boct_tree<short, data_type> >(tree->bounding_box(), tree);
    vgl_box_3d<double> bb = blk->bounding_box();
    vgl_point_3d<double> origin(0,0,0);
    vgl_vector_3d<double> block_dim(bb.width(),bb.height(),bb.depth());
    vgl_vector_3d<unsigned> world_dim(1,1,1);
    auto* scene
 = new boxm_scene<boct_tree<short, data_type> >(origin, block_dim, world_dim);
    scene->set_appearance_model(BOXM_APM_SIMPLE_GREY);
    scene->set_block(vgl_point_3d<int>(0,0,0), blk);
    scene->set_path(scene_dir, block_prefix);
    scene->write_scene(xml_path);
    scene->write_active_block();
    scene_base = scene;
  }
  pro.set_output_val<boxm_scene_base_sptr>(0, scene_base);
  return true;
}
