// This is brl/bbas/volm/volm_pro/processes/volm_upsample_dem_projected_img_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief special process to up-sample the image generated by projecting ASTER DEM to a given satellite viewpoint
//
// \author Yi Dong
// \date April 24, 2014
//  Modifications
//   <none yet>
// \endverbatim
//
#include <vcl_iostream.h>
#include <vil/vil_image_view.h>
#include <rsdl/rsdl_bins_2d.h>
#include <vnl/vnl_float_4.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_algorithm.h>
#include <vcl_cmath.h>
#include <vul/vul_timer.h>


//: global variable and functions
namespace volm_upsample_dem_projected_img_process_globals
{
  const unsigned n_inputs_  = 4;
  const unsigned n_outputs_ = 1;

  typedef vnl_vector_fixed<double,2> pt_type;
}
//: constructor
bool volm_upsample_dem_projected_img_process_cons(bprb_func_process& pro)
{
  using namespace volm_upsample_dem_projected_img_process_globals;
  // process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  // input image
  input_types_[1] = "unsigned";                  // number of nearest neighbors considered (default is 4)
  input_types_[2] = "unsigned";                  // bin size along image column
  input_types_[3] = "unsigned";                  // bin size along image row
  // process takes 1 output
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // output image
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}
//: execute the process
bool volm_upsample_dem_projected_img_process(bprb_func_process& pro)
{
  using namespace volm_upsample_dem_projected_img_process_globals;
  // sanity check
  if (!pro.verify_inputs()) {
    vcl_cout << pro.name() << ": Invalid inputs!" << vcl_endl;
    return false;
  }
  // get the input
  vil_image_view_base_sptr in_img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vil_image_view<float>* in_img = dynamic_cast<vil_image_view<float>*>(in_img_sptr.ptr());
  if (!in_img) {
    vcl_cout << pro.name() << ": The input image pixel format, " << in_img_sptr->pixel_format() << " is not supported!" << vcl_endl;
    return false;
  }
  unsigned num_neighbors = pro.get_input<unsigned>(1);
  unsigned bin_size_0 = pro.get_input<unsigned>(2);
  unsigned bin_size_1 = pro.get_input<unsigned>(3);

  unsigned ni = in_img->ni();
  unsigned nj = in_img->nj();
  vil_image_view<float>* out_img = new vil_image_view<float>(ni, nj, 1);
  out_img->deep_copy(*in_img);

  // look for the valid pixels (note that in order to calculate distance between pixels, double precision is considered for img coords)
  vcl_vector<pt_type > pixels;
  vcl_vector<float> values;
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++)
      if ((*in_img)(i,j) > 0.0f) {
        pixels.push_back(vnl_vector_fixed<double, 2>((double)i, (double)j));
        values.push_back((*in_img)(i,j));
      }

  // construct the rsdl_bin_2d to speed up pixel retrieval
  pt_type min_pt, max_pt, bin_sizes;
  min_pt[0] = 0.0;  max_pt[0] = (double)ni;
  min_pt[1] = 0.0;  max_pt[1] = (double)nj;
  // set the bin size to be 60 (roughly 60 meters)
  bin_sizes[0] = bin_size_0; bin_sizes[1] = bin_size_1;
  rsdl_bins_2d<double, float> bins(min_pt, max_pt, bin_sizes);
  double dist_tol = 0.0001;
  bins.set_distance_tolerance(dist_tol);

  // add all valid pixels into bin
  for (unsigned i = 0; i < pixels.size(); i++)
    bins.add_point(pixels[i], values[i]);

  // get point values
  for (unsigned i = 0; i < pixels.size(); i++) {
    float stored_value;
    bins.get_value(pixels[i], stored_value);
    //vcl_cout << "pixel [" << pixels[i][0] << ',' << pixels[i][1] << "], value = " << values[i] << " (diff = " << stored_value - values[i] << vcl_endl;
  }
  vcl_cout << "Construct rsdl bin with bin size " << bin_sizes[0] << 'x' << bin_sizes[1] << vcl_endl;
  vcl_cout << "Start to interpolate the [" << ni << 'x' << nj << "] image given " << pixels.size() << " valid pixels and " << num_neighbors << " nearest neighbors";

  vul_timer up_time;
  // for each pixel obtain its 4 nearest neighbors
  for (unsigned i = 0; i < ni; i++) {
    if (i%20 == 0)
      vcl_cout << '.' << i << vcl_flush;
    /*if (i > 500 || i < 200)
      continue;*/
    for (unsigned j = 0; j < nj; j++) {
      //if (i > 500 || i < 200)
      //  continue;
      if (vcl_find(pixels.begin(), pixels.end(), pt_type((double)i,(double)j)) != pixels.end())
        continue;
      pt_type query_pt((double)i,(double)j);
      vcl_vector<pt_type> neigh_points;
      vcl_vector<float>   neigh_values;
      bins.n_nearest(query_pt, num_neighbors, neigh_points, neigh_values);
      //vcl_cout << " for pixel: [" << i << "," << j << "], neighbors are: ";
      //for (unsigned n_idx = 0; n_idx < neigh_points.size(); n_idx++)
      //  vcl_cout << "[" << neigh_points[n_idx][0] << ", " << neigh_points[n_idx][1] << ", " << neigh_values[n_idx] << "], ";
      //vcl_cout << vcl_endl;

      // using distance to normalize the pixel value
      //vnl_float_4 dist(0,0,0,0);
      float value_all = 0.0f, dist_all = 0.0f;
      for (unsigned n_idx = 0; n_idx < 4; n_idx++) {
        float dist = (float)vcl_sqrt( (neigh_points[n_idx][0]-i)*(neigh_points[n_idx][0]-i) + (neigh_points[n_idx][1]-j)*(neigh_points[n_idx][1]-j) );
        dist_all  += dist;
        value_all += neigh_values[n_idx]*dist;
      }
      //float dist_all = dist[0] + dist[1] + dist[2] + dist[3];
      (*out_img)(i,j) = value_all / dist_all;
    }
  }
  vcl_cout << "\nupsampling [" << ni << 'x' << nj << "] costs " << up_time.all()/1000.0 << " seconds" << vcl_endl;
  //output
  pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(out_img));

  return true;
}
