// This is brl/bpro/core/vil_pro/processes/vil_image_range_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Compute range (min/max) of an image

#include <vil/vil_math.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vcl_iostream.h>

template <class T>
static void image_value_range(const vil_image_view<T>& view, T& min_value, T& max_value,
                              unsigned& min_i, unsigned& min_j, unsigned& min_p, unsigned& max_i, unsigned& max_j, unsigned& max_p)
{
  if (view.size() == 0)
  {
    min_value = 0;
    max_value = 0;
    min_i = 0;  min_j = 0;  min_p = 0;
    max_i = 0;  max_j = 0;  max_p = 0;
    return;
  }

  min_value = *(view.top_left_ptr());
  max_value = min_value;
  min_i = 0;  min_j = 0;  min_p = 0;
  max_i = 0;  max_j = 0;  max_p = 0;

  unsigned ni = view.ni();
  unsigned nj = view.nj();
  unsigned np = view.nplanes();

  for (unsigned p=0; p<np; ++p)
    for (unsigned j=0; j<nj; ++j)
      for (unsigned i=0; i<ni; ++i)
      {
        const T pixel = view(i,j,p);
        if (pixel<min_value)
        {
          min_value = pixel;  min_i = i;  min_j = j;  min_p = p;
        }
        if (pixel>max_value)
        {
          max_value = pixel;  max_i = i;  max_j = j;  max_p = p;
        }
      }
  return;
}

//: Constructor
bool vil_image_range_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the image
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");
  vcl_vector<vcl_string> output_types;
  output_types.push_back("float");  // min
  output_types.push_back("float");  // max
  output_types.push_back("unsigned");  // min pixel value position i
  output_types.push_back("unsigned");  // min pixel value position j
  output_types.push_back("unsigned");  // min pixel value position p
  output_types.push_back("unsigned");  // max pixel value position i
  output_types.push_back("unsigned");  // max pixel value position j
  output_types.push_back("unsigned");  // max pixel value position p
  return pro.set_input_types(input_types)
     &&  pro.set_output_types(output_types);
}

//: Execute the process
bool vil_image_range_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    vcl_cout << "vil_image_range_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the input
  unsigned i=0;
  vil_image_view_base_sptr img_ptr_a = pro.get_input<vil_image_view_base_sptr>(i++);

  // for now assume input is a float image
  vil_image_view<float> view_a = *(vil_convert_cast(float(), img_ptr_a));

  float min, max;
  unsigned min_i, min_j, min_p, max_i, max_j, max_p;
  vcl_cout << "before" << vcl_flush << vcl_endl;
  image_value_range(view_a, min, max, min_i, min_j, min_p, max_i, max_j, max_p);
  vcl_cout << "after min: " << min << " at [" << min_i << ',' << min_j << ',' << min_p << "], "
           << " max: " << max << " at [" << max_i << ',' << max_j << ',' << max_p << ']' << vcl_flush << vcl_endl;
  //vil_math_value_range(view_a, min, max); 
  pro.set_output_val<float>(0, min);
  pro.set_output_val<float>(1, max);
  pro.set_output_val<unsigned>(2, min_i);
  pro.set_output_val<unsigned>(3, min_j);
  pro.set_output_val<unsigned>(4, min_p);
  pro.set_output_val<unsigned>(5, max_i);
  pro.set_output_val<unsigned>(6, max_j);
  pro.set_output_val<unsigned>(7, max_p);

  return true;
}

