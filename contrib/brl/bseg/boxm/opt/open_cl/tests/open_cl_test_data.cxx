#include <vcl_cmath.h>
#include <boxm/opt/open_cl/boxm_ray_trace_manager.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <vgl/vgl_point_3d.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include "open_cl_test_data.h"

boct_tree<short, vnl_vector_fixed<float, 2> > *
open_cl_test_data::tree()
{
  vcl_vector<boct_tree_cell<short, vnl_vector_fixed<float, 2> > > leaves;
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
  for(unsigned i = 0; i<8; ++i){
    boct_loc_code<short> loc(pts[i], n_levels-1);
    vcl_cout << "code[" << i << "] = " << loc << '\n';
    leaf_codes.push_back(loc);
  }
  for(unsigned i = 0; i<8; ++i){
    boct_tree_cell<short, vnl_vector_fixed<float, 2> > leaf(leaf_codes[i]);
    leaves.push_back(leaf);
  }
  boct_tree<short, vnl_vector_fixed<float, 2> > *init_tree =
    new boct_tree<short, vnl_vector_fixed<float, 2> >(n_levels, 0);

  boct_tree_cell<short, vnl_vector_fixed<float, 2> > *root =
    init_tree->construct_tree(leaves, init_tree->number_levels());

  boct_tree<short, vnl_vector_fixed<float, 2> > * ret_tree = 
    new boct_tree<short, vnl_vector_fixed<float, 2> >(root, init_tree->number_levels());
  delete init_tree;
  vcl_vector<boct_tree_cell<short, vnl_vector_fixed<float, 2> >* > tleaves;
  tleaves = ret_tree->leaf_cells();
  size_t i = 0;
  vcl_vector<boct_tree_cell<short, vnl_vector_fixed<float, 2> >* >::iterator lit = tleaves.begin();
  for(; lit!= tleaves.end(); ++lit, ++i)
    {
      vnl_vector_fixed<float, 2> v;
      v[0]=(float)1.0*i; v[1]=(float)10.0*i;
      (*lit)->set_data(v);
    }

  return ret_tree;
}
void open_cl_test_data::save_tree(vcl_string const& tree_path)
{
  boxm_ray_trace_manager* ray_mgr = boxm_ray_trace_manager::instance();
  boct_tree<short, vnl_vector_fixed<float, 2> >* tree = open_cl_test_data::tree();
  ray_mgr->set_tree(tree);
  ray_mgr->write_tree(tree_path);
  ray_mgr->set_tree(0);
  delete tree;
}

void open_cl_test_data::
test_rays(vbl_array_2d<vnl_vector_fixed<float, 3> >& ray_origin,
               vbl_array_2d<vnl_vector_fixed<float, 3> >& ray_dir)
{
  ray_origin = vbl_array_2d<vnl_vector_fixed<float, 3> > (4, 4);
  ray_dir = vbl_array_2d<vnl_vector_fixed<float, 3> > (4, 4);
  for(unsigned r = 0; r<4; r++)
    for(unsigned c = 0; c<4; c++)
      {
        ray_origin[r][c][0] = 0.125f + 0.25f*c;
        ray_origin[r][c][1] = 0.125f + 0.25f*r;
        ray_origin[r][c][2] = -10.0f;
        ray_dir[r][c][0]= 0.0f;
        ray_dir[r][c][1]= 0.0f;
        ray_dir[r][c][2]= -1.0f;
      }
}

void open_cl_test_data::
tree_and_rays_from_image(vcl_string const& image_path,
                         unsigned group_size,
                         boct_tree<short, vnl_vector_fixed<float, 2> >*& tree,
                         vbl_array_2d<vnl_vector_fixed<float, 3> >& ray_origin,
                         vbl_array_2d<vnl_vector_fixed<float, 3> >& ray_dir)
{
  tree = 0;
  vil_image_resource_sptr img = 
    vil_load_image_resource(image_path.c_str());
  if(!img|| img->nplanes()!=1||img->pixel_format()!=VIL_PIXEL_FORMAT_BYTE)
    return;
  vil_image_view<unsigned char> image = img->get_view();
  unsigned ni = image.ni(), nj = image.nj();
  // find a square arrangement consistent with group size
  double gsize = static_cast<double>(group_size);
  unsigned gwidth = static_cast<unsigned>(vcl_sqrt(gsize));
  unsigned gheight = group_size/gwidth; 
  if(gwidth*gheight!=gsize)
    return;//maybe fixup later to handle all situations
  unsigned mni = (ni/gwidth)*gwidth, mnj = (nj/gheight)*gheight;
  double min_dim = mni;
  if(mnj<min_dim)
    min_dim = mnj;
  double dlev = vcl_log(min_dim)/vcl_log(2.0);
  unsigned n_levels = static_cast<unsigned>(dlev+0.5);

  float rni = 1.0f/static_cast<float>(mni), rnj =1.0f/static_cast<float>(mnj);
  ray_origin.resize(mnj, mni);
  ray_dir.resize(mnj, mni);
  ray_dir.fill(vnl_vector_fixed<float,3>(0.0f, 0.0f, -1.0f));
  vcl_vector<boct_loc_code<short> > leaf_codes;
  vcl_vector<float> image_int;
  for(unsigned j=0; j<mnj; ++j)
    for(unsigned i=0; i<mni; ++i)
      {
        float u = static_cast<float>(i)*rni;
        float v = 1.0f-static_cast<float>(j)*rnj;
        ray_origin[j][i][0]=u; ray_origin[j][i][1]=v; 
        ray_origin[j][i][2]=10.0f;
        vgl_point_3d<double> p(u, v, 0.0f);
        boct_loc_code<short> loc(p, n_levels-1);
        leaf_codes.push_back(loc);
        image_int.push_back(static_cast<float>(image(i,j)));
      }
  //construct leaves
  vcl_vector<boct_tree_cell<short, vnl_vector_fixed<float, 2> > > leaves;
  for(unsigned i = 0; i<leaf_codes.size(); ++i){
    boct_tree_cell<short, vnl_vector_fixed<float, 2> > leaf(leaf_codes[i]);
    leaves.push_back(leaf);
  }

  //construct tree from leaves
  boct_tree<short, vnl_vector_fixed<float, 2> > *init_tree =
    new boct_tree<short, vnl_vector_fixed<float, 2> >(n_levels, 0);

  boct_tree_cell<short, vnl_vector_fixed<float, 2> > *root =
    init_tree->construct_tree(leaves, init_tree->number_levels());

  // construct full tree with leaves and root
  tree = new boct_tree<short, vnl_vector_fixed<float, 2> >(root, init_tree->number_levels());
  delete init_tree;

  // fill the leaves with data
  vcl_vector<boct_tree_cell<short, vnl_vector_fixed<float, 2> >* > tleaves;
  tleaves = tree->leaf_cells();
  size_t i = 0;
  vcl_vector<boct_tree_cell<short, vnl_vector_fixed<float, 2> >* >::iterator lit = tleaves.begin();
  for(; lit!= tleaves.end(); ++lit, ++i)
    {
      boct_loc_code<short> code = (*lit)->get_code();
      bool found = false;
      unsigned found_k = 0;
      for(unsigned k = 0; k<leaf_codes.size()&&!found; ++k)
        if(code == leaf_codes[k]){
          found = true;
          found_k = k;
        }
      vnl_vector_fixed<float, 2> v;
      if(found){
		  v[0]=1.0f; v[1]=image_int[found_k];
      }else{v[0]=0.0f; v[1]=0.0f;}
      (*lit)->set_data(v);
    }
}
void open_cl_test_data::save_expected_image(vcl_string const& image_path,
                                            unsigned cols, unsigned rows,
                                            float* expected_img)
{
  if(!expected_img)
    return;
  vil_image_view<float> out(cols, rows);
  unsigned expt_ptr = 0;
  for(unsigned j = 0; j<rows; ++j)
    for(unsigned i = 0; i<cols; ++i){
      out(i,j) = expected_img[expt_ptr];
      expt_ptr += 4;
    }
  vil_save(out, image_path.c_str());
}
