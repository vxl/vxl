// This is brl/bpro/core/vil_pro/processes/vil_fill_holes_in_regions_process.cxx
#include <iostream>
#include <algorithm>
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

//: Constructor
bool vil_fill_holes_in_regions_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the filename
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");

  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // label image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vil_fill_holes_in_regions_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "vil_fill_holes_in_regions_process: The number of inputs should be 1" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr in_img_ptr = pro.get_input<vil_image_view_base_sptr>(i++);

  if (auto *view=dynamic_cast<vil_image_view<unsigned char>* > (in_img_ptr.ptr()))
  {
    unsigned int ni = view->ni();
    unsigned int nj = view->nj();


    for (unsigned k = 0; k< ni; k++)
    {
      if (k % 10 == 0)
        std::cout<<k<<std::endl;
      for (unsigned l = 0; l< nj; l++)
      {
        if ((*view)(k,l) == 0)
        {
          unsigned char labels[256]={0};

          for (unsigned int s = k; (int)s >= std::max((int)k-10,0) ; s-- )
            if ( (*view)(s,l) > 0 )
            {
              labels[(int)((*view)(s,l))]++;
              break;
            }
          for (unsigned int s = k; (int)s < std::min((int)k+10,(int)ni) ; s++ )
          if ( (*view)(s,l) > 0 )
          {
            labels[(int)((*view)(s,l))]++;
            break;
          }
          unsigned int lmax = (unsigned int)std::max((int)l-10,(int)0);
          for (unsigned int s = l; s > lmax  ; s-- )
          if ( (*view)(k,s) > 0 )
          {
            labels[(int)((*view)(k,s))]++;
            break;
          }
          for (unsigned int s = l; (int)s < std::min((int)l+10,(int)nj) ; s++ )
          if ( (*view)(k,s) > 0 )
          {
            labels[(int)((*view)(k,s))]++;
            break;
          }

          for (unsigned int s = 0; s <256; s++)
          {
            if (labels[s] >= 3)
            {
              (*view)(k,l) = s;
              break;
            }
          }
        }
      }
    }
    pro.set_output_val<vil_image_view_base_sptr>(0, view);
    return true;
  }

  std::cerr<<"Error! Require a Byte image\n";
  return false;
}
