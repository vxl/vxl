//:
// \file
// \brief Example of generating functions to manipulate views.
// \author Tim Cootes - Manchester

#include <vcl_iostream.h>
#include <vxl_config.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_image_view_functions.h>

//: Example function to return a transpose of the view
vil2_image_view<vxl_byte> transpose_view(const vil2_image_view<vxl_byte>& v)
{
  // Create view with i and j switched
  return vil2_image_view<vxl_byte>(v.memory_chunk(),v.top_left_ptr(),
                                    v.nj(),v.ni(),v.nplanes(),
                                    v.jstep(),v.istep(),v.planestep());
}

int main(int argc, char** argv)
{
  unsigned ni=9;
  unsigned nj=8;
  vil2_image_view<vxl_byte> image(ni,nj);

  // Slow fill
  for (unsigned j=0;j<nj;++j)
    for (unsigned i=0;i<ni;++i)
      image(i,j) = vxl_byte(i+10*j);

  vcl_cout<<"Original image:"<<vcl_endl;
  vil2_print_all(vcl_cout,image);

  vcl_cout<<vcl_endl;
  vcl_cout<<"Create transposed view of plane"<<vcl_endl;
  vil2_image_view<vxl_byte> transpose = transpose_view(image);
  vil2_print_all(vcl_cout,transpose);

  return 0;
}
