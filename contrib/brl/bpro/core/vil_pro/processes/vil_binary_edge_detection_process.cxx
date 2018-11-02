// This is brl/bpro/core/vil_pro/processes/vil_binary_edge_detection_process.cxx
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
//   Process to return edge pixels of a binary image using vil_blob

#include <vil/vil_image_view.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_blob.h>
#include <vil/algo/vil_binary_closing.h>
#include <vil/vil_save.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

namespace vil_binary_edge_detection_process_globals
{
  unsigned n_inputs_  = 4;
  unsigned n_outputs_ = 1;
}

//: constructor
bool vil_binary_edge_detection_process_cons(bprb_func_process& pro)
{
  using namespace vil_binary_edge_detection_process_globals;
  // process takes 3 inputs
  std::vector<std::string> input_types(n_inputs_);
  input_types[0] = "vil_image_view_base_sptr";          // input image
  input_types[1] = "unsigned";                          // max size of the blob in pixels
  input_types[2] = "unsigned";                          // min size of the blob in pixels
  input_types[3] = "unsigned";                          // threshold id to transfer from byte image to boolean image
  // process takes 1 output
  std::vector<std::string> output_types(n_outputs_);
  output_types[0] = "vil_image_view_base_sptr";         // edge image

  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

//: execute the process
bool vil_binary_edge_detection_process(bprb_func_process& pro)
{
  // sanity check
  if (!pro.verify_inputs())
    return false;
  // get input
  unsigned in_i = 0;
  vil_image_view_base_sptr img_ptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  auto max_size = pro.get_input<unsigned>(in_i++);
  auto min_size = pro.get_input<unsigned>(in_i++);
  auto threshold = pro.get_input<unsigned>(in_i++);
  auto thres_id = (unsigned char)threshold;

  auto* view = dynamic_cast<vil_image_view<bool>*>(img_ptr.ptr());
  if (!view) {
    vil_image_view<bool> temp(img_ptr->ni(), img_ptr->nj(), img_ptr->nplanes());
    temp.fill(false);
    auto* view_temp = dynamic_cast<vil_image_view<vxl_byte>*>(img_ptr.ptr());
    if (!view_temp) {
      std::cerr << pro.name() << " input image pixel " << img_ptr->pixel_format() << " is not supported!\n";
      return false;
    }
    // transfer byte image
    for (unsigned i = 0; i < img_ptr->ni(); i++)
      for (unsigned j = 0; j < img_ptr->nj(); j++)
        for (unsigned k = 0; k < img_ptr->nplanes(); k++)
          if ( (*view_temp)(i,j,k) == thres_id)
            temp(i,j,k) = true;
    view = new vil_image_view<bool>(temp);
  }

  // Closing the holes or gaps
  vil_structuring_element selem;
  selem.set_to_disk(1.0);
  vil_image_view<bool> view_closed(view->ni(),view->nj());
  vil_binary_closing(*view,view_closed,selem);

  // Find the blob
  vil_image_view<unsigned> blob_labels;
  vil_image_view<unsigned> edge_labels;
  std::vector<vil_blob_pixel_list> edge_pixel_list;
  vil_blob_labels(*view, vil_blob_8_conn, blob_labels);
  vil_blob_labels_to_edge_labels(blob_labels, vil_blob_8_conn, edge_labels);
  vil_blob_labels_to_pixel_lists(edge_labels, edge_pixel_list);

  // remove the edges that do not satisfy the desired size
  std::vector<unsigned> valid_edge_labels;
  for (auto & vit : edge_pixel_list) {
    if ( vit.size() > max_size || vit.size() < min_size )
      continue;
    unsigned label = edge_labels(vit.begin()->first, vit.begin()->second);
    valid_edge_labels.push_back(label);
  }


  vil_image_view<vxl_byte> view_blobs(view->ni(),view->nj());
  view_blobs.fill(0);
  for (unsigned i = 0; i < edge_labels.ni(); i++)
    for (unsigned j = 0; j < edge_labels.nj(); j++)
      for (unsigned k = 0; k < edge_labels.nplanes(); k++)
        if ( std::find(valid_edge_labels.begin(), valid_edge_labels.end(), edge_labels(i,j,k)) != valid_edge_labels.end())
          view_blobs(i,j,k) = 255;

  // output
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vxl_byte>(view_blobs));

  return true;
}
