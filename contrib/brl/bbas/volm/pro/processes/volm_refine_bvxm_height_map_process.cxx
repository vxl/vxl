// This is brl/bbas/volm/pro/processes/volm_refine_bvxm_height_map_process.cxx
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vil/vil_image_view.h>
#include <vul/vul_file.h>
#include <vil/algo/vil_region_finder.h>

// for debug
#if 0
#include <vil/vil_save.h>
#endif

namespace volm_refine_bvxm_height_map_process_globals
{
  const unsigned int n_inputs_  = 3;
  const unsigned int n_outputs_ = 1;
  float neighbor_min_height(vcl_vector<unsigned> const& ri, vcl_vector<unsigned> const& rj, vil_image_view<float> const& in_img)
  {
    // create a neighbor list
    static int const nbrs8_delta[8][2] = { { 1, 0}, { 1,-1}, { 0,-1}, {-1,-1},
                                         {-1, 0}, {-1, 1}, { 0, 1}, { 1, 1} };
    unsigned num_nbrs = 8;
    float min_h = 10000.0f;
    // loop over all pixel to obtain min height (TO DO: speed up to avoid searching pixels that are inside the region)
    for (unsigned k = 0; k < ri.size(); k++) {
      unsigned i = ri[k];  unsigned j = rj[k];
      for (unsigned c = 0; c < num_nbrs; c++) {
        unsigned nbr_i = (unsigned)( (signed)i + nbrs8_delta[c][0] );
        unsigned nbr_j = (unsigned)( (signed)j + nbrs8_delta[c][1] );
        if (nbr_i < in_img.ni() && nbr_j < in_img.nj())
          if ( in_img(nbr_i, nbr_j) < min_h )
            min_h = in_img(nbr_i, nbr_j);
      }
    }
    return min_h;
  }
}

//: simple algorithm to refine the height map generate from bvxm_scene
bool volm_refine_bvxm_height_map_process_cons(bprb_func_process& pro)
{
  using namespace volm_refine_bvxm_height_map_process_globals;
  // inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";          // original height map image
  input_types_[1] = "float";               // predominant height for sky mask
  input_types_[2] = "float";               // predominant height for ground mask

  // output
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_refine_bvxm_height_map_process(bprb_func_process& pro)
{
  using namespace volm_refine_bvxm_height_map_process_globals;
  // input check
  if (!pro.verify_inputs()) {
    vcl_cout << pro.name() << ": invalid inputs" << vcl_endl;
    return false;
  }
  // get input
  unsigned i = 0;
  vil_image_view_base_sptr i_img_res = pro.get_input<vil_image_view_base_sptr>(i++);
  float sky_h = pro.get_input<float>(i++);
  float grd_h = pro.get_input<float>(i++);

  vil_image_view<float>* in_img = dynamic_cast<vil_image_view<float>*>(i_img_res.ptr());
  if (!in_img) {
    vcl_cout << pro.name() << ": The image pixel format: " << i_img_res->pixel_format() << " is not supported" << vcl_endl;
    return false;
  }
  unsigned ni = in_img->ni();
  unsigned nj = in_img->nj();

  // generate sky mask
  vil_image_view<vxl_byte> sky_mask = vil_image_view<vxl_byte>(ni, nj);
  sky_mask.fill(0);
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++)
      if ( (*in_img)(i,j) > sky_h )
        sky_mask(i,j) = 1;

  // generate ground mask
  vil_image_view<vxl_byte> grd_mask = vil_image_view<vxl_byte>(ni, nj);
  grd_mask.fill(0);
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++)
      if ( (*in_img)(i,j) < grd_h )
        grd_mask(i,j) = 1;

  // create an output image
  vil_image_view<float>* out_img = new vil_image_view<float>(ni, nj);
  out_img->deep_copy(*in_img);

#if 0
  // output for debug
  vcl_string sky_fname = "d:/work/find/phase_1b/satellite_modeling/wr2/scene_11278_small/sky_mask.tif";
  vcl_string grd_fname = "d:/work/find/phase_1b/satellite_modeling/wr2/scene_11278_small/grd_mask.tif";
  vil_save(sky_mask, sky_fname.c_str());
  vil_save(grd_mask, grd_fname.c_str());
#endif

  // refine sky/grd mask
  vil_region_finder<vxl_byte> sky_region_finder(sky_mask, vil_region_finder_8_conn);
  vil_region_finder<vxl_byte> grd_region_finder(grd_mask, vil_region_finder_8_conn);
  vcl_map<unsigned, vcl_pair<float, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > > > sky_regions;
  vcl_map<unsigned, vcl_pair<float, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > > > grd_regions;
  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      // refined sky mask
      if (sky_mask(i,j) != 0) {
        vcl_vector<unsigned> ri;  vcl_vector<unsigned> rj;
        sky_region_finder.same_int_region(i, j, ri, rj);
        if (ri.empty())
          continue;
        // search perimeter around the super pixel to obtain minimum neighbor height
        float min_h = neighbor_min_height(ri, rj, *in_img);

        unsigned key = (i+j)*(i+j+1)/2 + j;
        vcl_pair<float, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >tmp_pair(min_h, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> >(ri, rj));
        sky_regions.insert(vcl_pair<unsigned, vcl_pair<float, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > > >(key, tmp_pair));
        //vcl_cout << " for super pixel (" << i << " x " << j << "), sky mask has height value: " << min_h << vcl_endl;
      }
      if (grd_mask(i,j) != 0) {
        vcl_vector<unsigned> ri;  vcl_vector<unsigned> rj;
        grd_region_finder.same_int_region(i, j, ri, rj);
        if (ri.empty())
          continue;
        float min_h = neighbor_min_height(ri, rj, *in_img);
        unsigned key = (i+j)*(i+j+1)/2 + j;
        vcl_pair<float, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > >tmp_pair(min_h, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> >(ri, rj));
        grd_regions.insert(vcl_pair<unsigned, vcl_pair<float, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > > >(key, tmp_pair));
        //vcl_cout << " for super pixel (" << i << " x " << j << "), grd mask has height value: " << min_h << vcl_endl;
      }
    }
  }
  
  // modify the output images with mask values
  vcl_map<unsigned, vcl_pair<float, vcl_pair<vcl_vector<unsigned>, vcl_vector<unsigned> > > >::iterator mit;
  for (mit = sky_regions.begin();  mit != sky_regions.end(); ++mit) {
    vcl_vector<unsigned> ri;  vcl_vector<unsigned> rj;
    float min_h = mit->second.first;
    ri = mit->second.second.first;  rj = mit->second.second.second;
    for (unsigned k = 0; k < ri.size(); k++)
      (*out_img)(ri[k],rj[k]) = min_h;
  }

  for (mit = grd_regions.begin();  mit != grd_regions.end(); ++mit) {
    vcl_vector<unsigned> ri;  vcl_vector<unsigned> rj;
    float min_h = mit->second.first;
    ri = mit->second.second.first;  rj = mit->second.second.second;
    for (unsigned k = 0; k < ri.size(); k++)
      (*out_img)(ri[k],rj[k]) = min_h;
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img);

  return true;
}