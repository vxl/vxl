#include "open_cl_test_data.h"

#include <boxm/ocl/boxm_ray_trace_manager.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <vcl_cmath.h>
#include <vil/vil_save.h>

void open_cl_test_data::
test_rays(vbl_array_2d<vnl_vector_fixed<float, 3> >& ray_origin,
          vbl_array_2d<vnl_vector_fixed<float, 3> >& ray_dir)
{
  ray_origin = vbl_array_2d<vnl_vector_fixed<float, 3> > (4, 4);
  ray_dir = vbl_array_2d<vnl_vector_fixed<float, 3> > (4, 4);
  for (unsigned r = 0; r<4; r++)
    for (unsigned c = 0; c<4; c++)
    {
      ray_origin[r][c][0] = 0.125f + 0.25f*c;
      ray_origin[r][c][1] = 0.125f + 0.25f*r;
      ray_origin[r][c][2] = -10.0f;
      ray_dir[r][c][0]= 0.0f;
      ray_dir[r][c][1]= 0.0f;
      ray_dir[r][c][2]= -1.0f;
    }
}

void open_cl_test_data::save_expected_image(vcl_string const& image_path,
                                            unsigned cols, unsigned rows,
                                            float* expected_img)
{
  if (!expected_img)
    return;
  vil_image_view<float> out(cols, rows);
  unsigned expt_ptr = 0;
  for (unsigned j = 0; j<rows; ++j)
    for (unsigned i = 0; i<cols; ++i) {
      out(i,j) = expected_img[expt_ptr];
      expt_ptr += 4;
    }
  vil_save(out, image_path.c_str());
}

template <>
void set_data(boct_tree<short,float >* tree)
{
  vcl_vector<boct_tree_cell<short, float >* > tleaves;
  tleaves = tree->leaf_cells();
  vcl_size_t i = 0;
  vcl_vector<boct_tree_cell<short, float >* >::iterator lit = tleaves.begin();
  for (; lit!= tleaves.end(); ++lit, ++i)
  {
    if (i==21 || i==41 || i==35) {
      float v((float)2.0);
      (*lit)->set_data(v);
    }
    else {
      float v((float)0.1);
      (*lit)->set_data(v);
    }
  }
}

template <>
void set_data(boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> >* tree)
{
  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type_sf1;
  typedef bsta_num_obs<bsta_mixture_fixed<gauss_type_sf1, 3> > mix_type;
  vcl_vector<boct_tree_cell<short, boxm_sample<BOXM_APM_MOG_GREY> >* > tleaves;
  tleaves = tree->leaf_cells();
  vcl_size_t i = 0;
  vcl_vector<boct_tree_cell<short, boxm_sample<BOXM_APM_MOG_GREY> >* >::iterator lit = tleaves.begin();
  gauss_type_sf1 g;
  g.set_mean(0.533333f); g.set_var(0.01f); g.num_observations = 1;
  mix_type mix;
  mix.insert(g, 1.0f); mix.num_observations = 1;
  boxm_sample<BOXM_APM_MOG_GREY> samp;
  samp.set_appearance(mix);
  samp.alpha = 1.38629f;
  for (; lit!= tleaves.end(); ++lit, ++i)
  {
    (*lit)->set_data(samp);
  }
}

