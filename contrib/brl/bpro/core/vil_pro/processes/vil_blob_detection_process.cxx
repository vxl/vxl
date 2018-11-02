// This is brl/bpro/core/vil_pro/processes/vil_blob_detection_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vil/vil_image_view.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_blob.h>
#include <vil/algo/vil_binary_closing.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_random.h>


//: Constructor
bool vil_blob_detection_process_cons(bprb_func_process& pro)
{
  //this process takes 3 inputs
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("unsigned"); // max size of the blob in pixels
  input_types.emplace_back("unsigned"); // min size of the blob in pixels

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // label image
  output_types.emplace_back("vil_image_view_base_sptr");  // randomly colored output image
  output_types.emplace_back("unsigned");  // also return the number of blobs

  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vil_blob_detection_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 3) {
    std::cout << "vil_blob_detection_process: The number of inputs should be 3" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr img_ptr_a = pro.get_input<vil_image_view_base_sptr>(i++);
  auto min_size  = pro.get_input<unsigned>(i++);
  auto max_size  = pro.get_input<unsigned>(i++);

  if (auto *view=dynamic_cast<vil_image_view<bool>* > (img_ptr_a.ptr()))
  {
    // Closing the holes or gaps
    vil_structuring_element selem;
    selem.set_to_disk(1.0);
    vil_image_view<bool> view_closed(view->ni(),view->nj());
    vil_binary_closing(*view,view_closed,selem);

    // Find the blobs
    std::vector<vil_blob_region> blob_regions;
    {
      vil_image_view<unsigned> blob_labels;
      vil_blob_labels(*view, vil_blob_4_conn, blob_labels);//view_closed);
      vil_blob_labels_to_regions(blob_labels, blob_regions);
    }
    vil_image_view<bool> view_blobs(view->ni(),view->nj());

    view_blobs.fill(false);

    auto* colored_blobs = new vil_image_view<vil_rgb<vxl_byte> >(view_blobs.ni(),view_blobs.nj());
    colored_blobs->fill(vil_rgb<vxl_byte>(0,0,0));
    vnl_random rng;
    unsigned cnt_blobs = 0;

    // Threshold the blobs.
    for (std::vector<vil_blob_region>::const_iterator it= blob_regions.begin(),
         end=blob_regions.end(); it!=end; ++it)
    {
      std::size_t sizecount= vil_area(*it);
      if (sizecount>min_size && sizecount<max_size) {
        vil_rgb<vxl_byte> random_color = vil_rgb<vxl_byte>((char)rng.lrand32(0,255), (char)rng.lrand32(0,255), (char)rng.lrand32(0,255));
        for (auto chords_it : *it)
          for (unsigned i=chords_it.ilo; i<=chords_it.ihi; i++) {
            view_blobs(i,chords_it.j)=true;
            (*colored_blobs)(i,chords_it.j) = random_color;
          }
        cnt_blobs++;
      }
    }

    pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<bool>(view_blobs));
    pro.set_output_val<vil_image_view_base_sptr>(1, colored_blobs);
    pro.set_output_val<unsigned>(2, cnt_blobs);
    return true;
  }

  std::cerr<<"Error! Require a boolean image\n";
  return false;
}
