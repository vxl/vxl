// This is brl/bpro/core/vil_pro/processes/vil_fill_holes_in_regions_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vil/vil_image_view.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_blob.h>
#include <vil/algo/vil_binary_closing.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>

//: Constructor
bool vil_fill_holes_in_regions_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the filename
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");

  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");  // label image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vil_fill_holes_in_regions_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    vcl_cout << "vil_fill_holes_in_regions_process: The number of inputs should be 1" << vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr in_img_ptr = pro.get_input<vil_image_view_base_sptr>(i++);

  if (vil_image_view<unsigned char> *view=dynamic_cast<vil_image_view<unsigned char>* > (in_img_ptr.ptr()))
  {
    unsigned int ni = view->ni();
    unsigned int nj = view->nj();


    for (unsigned k = 0; k< ni; k++)
    {
      if (k % 10 == 0)
        vcl_cout<<k<<vcl_endl;
      for (unsigned l = 0; l< nj; l++)
      {
        if ((*view)(k,l) == 0)
        {
          unsigned char labels[256]={0};

          for (unsigned int s = k; (int)s >= vcl_max((int)k-10,0) ; s-- )
            if ( (*view)(s,l) > 0 )
            {
              labels[(int)((*view)(s,l))]++;
              break;
            }
          for (unsigned int s = k; (int)s < vcl_min((int)k+10,(int)ni) ; s++ )
          if ( (*view)(s,l) > 0 )
          {
            labels[(int)((*view)(s,l))]++;
            break;
          }
          unsigned int lmax = (unsigned int)vcl_max((int)l-10,(int)0);
          for (unsigned int s = l; s > lmax  ; s-- )
          if ( (*view)(k,s) > 0 )
          {
            labels[(int)((*view)(k,s))]++;
            break;
          }
          for (unsigned int s = l; (int)s < vcl_min((int)l+10,(int)nj) ; s++ )
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

  vcl_cerr<<"Error! Require a Byte image\n";
  return false;
}
