//:
// \file
// \brief Example of generating functions to manipulate views.
// \author Tim Cootes - Manchester

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>

//: Example function to return a transpose of the view
// This similar to the provided vil_transpose.
vil_image_view<vxl_byte> transpose_view(const vil_image_view<vxl_byte>& v)
{
  // Create view with i and j switched
  return vil_image_view<vxl_byte>(v.memory_chunk(),v.top_left_ptr(),
                                  v.nj(),v.ni(),v.nplanes(),
                                  v.jstep(),v.istep(),v.planestep());
}

int main()
{
  unsigned ni=9;
  unsigned nj=8;
  vil_image_view<vxl_byte> image(ni,nj);

  // Slow fill
  for (unsigned j=0;j<nj;++j)
    for (unsigned i=0;i<ni;++i)
      image(i,j) = vxl_byte(i+10*j);

  vcl_cout<<"Original image:\n";
  vil_print_all(vcl_cout,image);

  vcl_cout<<"\nCreate transposed view of plane\n";
  vil_image_view<vxl_byte> transpose = transpose_view(image);
  vil_print_all(vcl_cout,transpose);

  return 0;
}
