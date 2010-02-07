#ifndef open_cl_test_data_h_
#define open_cl_test_data_h_

#include <boxm/boxm_mog_grey_processor.h>
#include <boxm/boxm_simple_grey_processor.h>

#include <boct/boct_tree.h>
#include <vbl/vbl_array_2d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_iostream.h>

class open_cl_test_data
{
 public:
  template <class T>
  static boct_tree<short, T >* tree();
  template <class T>
  static void save_tree(vcl_string const& tree_path);

  static void test_rays(vbl_array_2d<vnl_vector_fixed<float, 3> >& ray_origin,
                        vbl_array_2d<vnl_vector_fixed<float, 3> >& ray_dir);

  template <class T>
  static  void tree_and_rays_from_image(vcl_string const& image_path,
                                        unsigned group_size,
                                        boct_tree<short, T >*& tree,
                                        vbl_array_2d<vnl_vector_fixed<float, 3> >& ray_origin,
                                        vbl_array_2d<vnl_vector_fixed<float, 3> >& ray_dir);

  static void save_expected_image(vcl_string const& image_path,
                                  unsigned cols, unsigned rows,
                                  float* expected_img);
};

template <class T>
boct_tree<short,T > * open_cl_test_data::tree()
{
  vcl_vector<boct_tree_cell<short, T > > leaves;
  vcl_vector<vgl_point_3d<double> > pts;
  pts.push_back(vgl_point_3d<double>(0,0,0));
  pts.push_back(vgl_point_3d<double>(0.51,0,0));
  pts.push_back(vgl_point_3d<double>(0,0.51,0));
  pts.push_back(vgl_point_3d<double>(0.51,0.51,0));
  pts.push_back(vgl_point_3d<double>(0,0,0.51));
  pts.push_back(vgl_point_3d<double>(0.51,0,0.51));
  pts.push_back(vgl_point_3d<double>(0.0,0.51,0.51));
  pts.push_back(vgl_point_3d<double>(0.51,0.51,0.51));
  vcl_vector<boct_loc_code<short> > leaf_codes;
  unsigned n_levels = 3;
  for (unsigned i = 0; i<8; ++i) {
    boct_loc_code<short> loc(pts[i], n_levels-1);
    vcl_cout << "code[" << i << "] = " << loc << '\n';
    leaf_codes.push_back(loc);
  }
  for (unsigned i = 0; i<8; ++i) {
    boct_tree_cell<short, T > leaf(leaf_codes[i]);
    leaves.push_back(leaf);
  }
  boct_tree<short, T > *init_tree =
    new boct_tree<short, T >(n_levels, 0);

  boct_tree_cell<short, T > *root =
    init_tree->construct_tree(leaves, init_tree->number_levels());

  boct_tree<short, T > * ret_tree =
    new boct_tree<short, T >(root, init_tree->number_levels());
  delete init_tree;
  vcl_vector<boct_tree_cell<short, T >* > tleaves;
  tleaves = ret_tree->leaf_cells();
  vcl_size_t i = 0;
  typename vcl_vector<boct_tree_cell<short, T >* >::iterator lit = tleaves.begin();
  for (; lit!= tleaves.end(); ++lit, ++i)
  {
    T v((float)1.0*i);
    (*lit)->set_data(v);
  }

  return ret_tree;
}

template <class T>
void open_cl_test_data::
tree_and_rays_from_image(vcl_string const& image_path,
                         unsigned group_size,
                         boct_tree<short, T >*& tree,
                         vbl_array_2d<vnl_vector_fixed<float, 3> >& ray_origin,
                         vbl_array_2d<vnl_vector_fixed<float, 3> >& ray_dir)
{
  tree = 0;
  vil_image_resource_sptr img =
    vil_load_image_resource(image_path.c_str());
  if (!img|| img->nplanes()!=1||img->pixel_format()!=VIL_PIXEL_FORMAT_BYTE)
    return;
  vil_image_view<unsigned char> image = img->get_view();
  unsigned ni = image.ni(), nj = image.nj();
  // find a square arrangement consistent with group size
  double gsize = static_cast<double>(group_size);
  unsigned gwidth = static_cast<unsigned>(vcl_sqrt(gsize));
  unsigned gheight = group_size/gwidth;
  if (gwidth*gheight!=gsize)
    return;//maybe fixup later to handle all situations
  unsigned mni = (ni/gwidth)*gwidth, mnj = (nj/gheight)*gheight;
  double min_dim = mni;
  if (mnj<min_dim)
    min_dim = mnj;
  double dlev = vcl_log(min_dim)/vcl_log(2.0);
  unsigned n_levels = static_cast<unsigned>(dlev+1.0);

  float rni = 1.0f/static_cast<float>(mni), rnj =1.0f/static_cast<float>(mnj);
  ray_origin.resize(mnj, mni);
  ray_dir.resize(mnj, mni);
  ray_dir.fill(vnl_vector_fixed<float,3>(0.0f, 0.0f, -1.0f));
  vcl_vector<boct_loc_code<short> > leaf_codes;
  vcl_vector<float> image_int;
  float upper_v=(mnj-1)*rnj;
  for (unsigned j=0; j<mnj; ++j)
    for (unsigned i=0; i<mni; ++i)
    {
      float u = static_cast<float>(i)*rni;
      float v = upper_v-static_cast<float>(j)*rnj;
      ray_origin[j][i][0]=u+rni/2; ray_origin[j][i][1]=v+rnj/2;
      ray_origin[j][i][2]=10.0f;
      vgl_point_3d<double> p(u, v, 0.0f);
      boct_loc_code<short> loc(p, n_levels-1);
      leaf_codes.push_back(loc);
      image_int.push_back(static_cast<float>(image(i,j))/255.0f);
    }
  //construct leaves
  vcl_vector<boct_tree_cell<short, T > > leaves;
  for (unsigned i = 0; i<leaf_codes.size(); ++i) {
    boct_tree_cell<short, T > leaf(leaf_codes[i]);
    leaves.push_back(leaf);
  }

  //construct tree from leaves
  boct_tree<short, T > *init_tree =
    new boct_tree<short, T >(n_levels, 0);

  boct_tree_cell<short, T > *root =
    init_tree->construct_tree(leaves, init_tree->number_levels());

  // construct full tree with leaves and root
  tree = new boct_tree<short, T >(root, init_tree->number_levels());
  delete init_tree;

  // fill the leaves with data
  vcl_vector<boct_tree_cell<short, T >* > tleaves;
  tleaves = tree->leaf_cells();
  vcl_size_t i = 0;
  typename vcl_vector<boct_tree_cell<short, T >* >::iterator lit = tleaves.begin();

  for (; lit!= tleaves.end(); ++lit, ++i)
  {
    boct_loc_code<short> code = (*lit)->get_code();
    bool found = false;
    unsigned found_k = 0;
    for (unsigned k = 0; k<leaf_codes.size()&&!found; ++k)
    {
      if (code == leaf_codes[k]) {
        found = true;
        found_k = k;
      }
      if (boct_tree_cell<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > * cell_grey_ptr
          = reinterpret_cast<boct_tree_cell<short, boxm_sample<BOXM_APM_SIMPLE_GREY> >* >(*lit))
      {
        if (found) {
          boxm_sample<BOXM_APM_SIMPLE_GREY> data(1.0f,boxm_sample<BOXM_APM_SIMPLE_GREY>::apm_datatype(image_int[found_k],0.0008));
          cell_grey_ptr->set_data(data);
        }
        else {
          boxm_sample<BOXM_APM_SIMPLE_GREY> data(0.0f,boxm_sample<BOXM_APM_SIMPLE_GREY>::apm_datatype(0.0,0.0008));
          cell_grey_ptr->set_data(data);
        }
      }
      else if (boct_tree_cell<short, boxm_sample<BOXM_APM_MOG_GREY> > * cell_mog_grey_ptr
               = reinterpret_cast<boct_tree_cell<short, boxm_sample<BOXM_APM_MOG_GREY> >* >(*lit))
      {
        if (found) {
          boxm_sample<BOXM_APM_MOG_GREY> data(1.0f);
          boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_processor::update(data.appearance(),image_int[found_k],1.0);
          cell_mog_grey_ptr->set_data(data);
        }
        else {
          boxm_sample<BOXM_APM_MOG_GREY> data(0.0f);
          cell_mog_grey_ptr->set_data(data);
        }
      }
      else if (boct_tree_cell<short, float > * cell_float_ptr
               = reinterpret_cast<boct_tree_cell<short, float >* >(*lit))
      {
        if (found) {
          float data(1.0f);
          cell_float_ptr->set_data(data);
        }
      }
    }
  }
}


template <class T>
void open_cl_test_data::save_tree(vcl_string const& tree_path)
{
  boxm_ray_trace_manager<T>* ray_mgr = typename boxm_ray_trace_manager<T>::instance();
  boct_tree<short,T >* tree = typename open_cl_test_data::tree<T>();
  ray_mgr->set_tree(tree);
  ray_mgr->write_tree(tree_path);
  ray_mgr->set_tree(0);
  delete tree;
}

#endif // open_cl_test_data_h_
