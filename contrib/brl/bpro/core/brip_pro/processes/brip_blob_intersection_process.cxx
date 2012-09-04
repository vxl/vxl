// This is brl/bpro/core/brip_pro/processes/brip_blob_intersection_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <boxm2/util/boxm2_detect_change_blobs.h>
#include <bil/algo/bil_blob_finder.h>
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_chord.h>
#include <bbas_pro/bbas_1d_array_float.h>
#include <vcl_algorithm.h>
#include <vcl_sstream.h>

namespace brip_blob_intersection_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 3;
}

//: Constructor
bool brip_blob_intersection_process_cons(bprb_func_process& pro)
{
  using namespace brip_blob_intersection_process_globals;

  // this process takes 2 inputs:
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");  // modeled blob map
  input_types.push_back("vil_image_view_base_sptr");  // ground truth map
  // this process takes 3 outputs:
  vcl_vector<vcl_string> output_types;
  output_types.push_back("int");  // num true positives
  output_types.push_back("int");  // num false positives
  output_types.push_back("int");  // num blobs in GT

  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool brip_blob_intersection_process(bprb_func_process& pro)
{
  using namespace brip_blob_intersection_process_globals;

  // Sanity check
  if (pro.n_inputs() < n_inputs_) {
    vcl_cerr << "brip_blob_intersection_process: The number of inputs should be 2\n";
    return false;
  }

  // get the inputs
  vil_image_view_base_sptr blob_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vil_image_view_base_sptr gt_sptr   = pro.get_input<vil_image_view_base_sptr>(1);

  // check bounds to make sure they match
  if (blob_sptr->ni() != gt_sptr->ni() || blob_sptr->nj() != gt_sptr->nj()) {
    vcl_cout<<"brip_blob_intersection_process:: detection map doesn't match ground truth map"<<vcl_endl;
    return false;
  }

  // cast to usable image views
  vil_image_view<vxl_byte> * gt_uchar = dynamic_cast<vil_image_view<vxl_byte> *>(gt_sptr.ptr());
  if ( !gt_uchar ) {
    vcl_cout<<"brip_blob_intersection_process:: gt map is not an unsigned char map"<<vcl_endl;
    return false;
  }
  vil_image_view<unsigned char> * blob_uchar =dynamic_cast<vil_image_view<unsigned char> *>(blob_sptr.ptr());
  if (!blob_uchar) {
    vcl_cout<<"brip_blob_intersection_process:: blob map is not an unsigned char map"<<vcl_endl;
    return false;
  }

  // create boolean gtmap
  vil_image_view<bool>  gt_map(gt_uchar->ni(), gt_uchar->nj()),
                        blob_map(gt_uchar->ni(), gt_uchar->nj());
  for (unsigned int i=0; i<gt_uchar->ni(); ++i)
    for (unsigned int j=0; j<gt_uchar->nj(); ++j) {
      gt_map(i,j)   = (*gt_uchar)(i,j) == 0 ? false : true;
      blob_map(i,j) = (*blob_uchar)(i,j) == 0 ? false : true;
    }

  // blob detect each image
  bil_blob_finder gt_finder(gt_map);
  bil_blob_finder mp_finder(blob_map);
  typedef vcl_vector<vil_chord> blob_t;
  vcl_vector<blob_t> gt_blobs, mp_blobs;

  // blob region is just a vector of vil_chords (rows in image)
  vcl_vector<vil_chord> region;
  while (gt_finder.next_4con_region(region))
    gt_blobs.push_back(region);
  while (mp_finder.next_4con_region(region))
    mp_blobs.push_back(region);

  // ---- cross check blobs ---------
  int numTP=0;
  bool* trueBlobs = new bool[mp_blobs.size()];
  vcl_fill(trueBlobs, trueBlobs+mp_blobs.size(), false);
  for (unsigned int i=0; i<gt_blobs.size(); ++i) {
    bool gt_blob_found = false;
    for (unsigned int j=0; j<mp_blobs.size(); ++j) {
      blob_t gt_blob = gt_blobs[i];
      blob_t mp_blob = mp_blobs[j];

      // determine if gt_blob[i] intersects with mp_blob[j]
      bool intersects = false;
      blob_t::iterator gt_iter;
      for (gt_iter=gt_blob.begin(); gt_iter!=gt_blob.end(); ++gt_iter) {
        for (unsigned gt_i=gt_iter->ilo; gt_i<gt_iter->ihi; ++gt_i) {

          // gt pixel = (gt_i, gt_iter->j)
          blob_t::iterator mp_iter;
          for (mp_iter=mp_blob.begin(); mp_iter!=mp_blob.end(); ++mp_iter) {
            for (unsigned mp_i=mp_iter->ilo; mp_i<mp_iter->ihi; ++mp_i) {
              // mp pixel = (mp_i, mp_iter->j)
              // if pixels match, they intersect
              if (mp_i==gt_i && mp_iter->j==gt_iter->j) {
                intersects = true;
                break;
              }
            }
          }
          if (intersects)
            break;
        }
        if (intersects)
          break;
      }

      // count total number of model blobs that intersect
      if (intersects)
        trueBlobs[j] = true;

      // determine true positive, false positive
      if (intersects && !gt_blob_found) {
        numTP++;
        gt_blob_found = true;
        break;
      }
    }
  }

  // count non intersecting blobs
  int falseBlobs = 0;
  for (unsigned int i=0; i<mp_blobs.size(); ++i)
    if (!trueBlobs[i]) falseBlobs++;

  // set outputs
  if (pro.n_outputs() < n_outputs_) {
    vcl_cerr << "brip_blob_intersection_process: The number of outputs should be "<<n_outputs_<<'\n';
    return false;
  }
  pro.set_output_val<int>(0, numTP);
  pro.set_output_val<int>(1, falseBlobs);
  pro.set_output_val<int>(2, gt_blobs.size());
  return true;
}

