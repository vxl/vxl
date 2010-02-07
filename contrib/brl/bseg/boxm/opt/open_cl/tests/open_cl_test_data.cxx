#include "open_cl_test_data.h"
#include <vcl_cmath.h>
#include <boxm/opt/open_cl/boxm_ray_trace_manager.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
//UNUSED: #include <vgl/vgl_point_3d.h>

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
