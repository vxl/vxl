// This is brl/bpro/core/brad_pro/processes/brad_get_image_footprint_process.cxx
//:
// \file
//     get the footprint (array of 4 corners) of the satellite image
//
#include <bprb/bprb_func_process.h>
#include <brad/brad_image_metadata.h>

#include "vgl/vgl_polygon.h"
#include <bpro/core/bbas_pro/bbas_1d_array_unsigned.h>
#include <bpro/core/bbas_pro/bbas_1d_array_double.h>

namespace brad_get_image_footprint_process_globals
{
  const unsigned n_inputs_  = 1;
  const unsigned n_outputs_ = 3;
}

bool brad_get_image_footprint_process_cons(bprb_func_process& pro)
{
  using namespace brad_get_image_footprint_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "brad_image_metadata_sptr"; // image metadata

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "unsigned"; // number of sheets in polygon
  output_types_[1] = "bbas_1d_array_unsigned_sptr"; // number of vertices in each polygon sheet
  output_types_[2] = "bbas_1d_array_double_sptr";    // polygon vertices

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool brad_get_image_footprint_process(bprb_func_process& pro)
{
  using namespace brad_get_image_footprint_process_globals;

  if (!pro.verify_inputs()) {
    std::cout << pro.name() << ": WRONG inputs!!!" << std::endl;
    return false;
  }

  //get the input(s)
  brad_image_metadata_sptr meta = pro.get_input<brad_image_metadata_sptr>(0);

  // vectorize footprint for output
  vgl_polygon<double> footprint = meta->footprint_;
  unsigned int nsheets = footprint.num_sheets();
  std::vector<unsigned> nverts;
  std::vector<double> verts;

  for (unsigned int s = 0; s < nsheets; ++s) {
    nverts.push_back(footprint[s].size());
    for (unsigned int p = 0; p < footprint[s].size(); ++p) {
      verts.push_back(footprint[s][p].x());
      verts.push_back(footprint[s][p].y());
    }
  }

  // fill out bbas_1d arrays
  bbas_1d_array_unsigned_sptr poly_nverts = new bbas_1d_array_unsigned(nverts.size());
  unsigned nv = 0;
  for(std::vector<unsigned>::iterator nit = nverts.begin(); nit != nverts.end(); ++nit, ++nv)
    poly_nverts->data_array[nv]=*nit;

  bbas_1d_array_double_sptr poly_verts = new bbas_1d_array_double(verts.size());
  unsigned iv = 0;
  for(std::vector<double>::iterator vit = verts.begin(); vit != verts.end(); ++vit, ++iv)
    poly_verts->data_array[iv]=*vit;

  // generate output
  unsigned i = 0;
  pro.set_output_val<unsigned int>(i++, nsheets);
  pro.set_output_val<bbas_1d_array_unsigned_sptr>(i++, poly_nverts);
  pro.set_output_val<bbas_1d_array_double_sptr>(i++, poly_verts);

  return true;
}

