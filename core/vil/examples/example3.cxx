//:
// \file
// \brief Example of setting up view of external data.
// \author Tim Cootes - Manchester

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>

int main()
{
  // Create some data in memory
  vcl_vector<vxl_byte> vec_data(64);
  for (unsigned i=0;i<vec_data.size();++i) vec_data[i] = i;

  vcl_cout<<"Creating a view of some data in memory\n";
  vil_image_view<vxl_byte> image(&vec_data[0],8,8,1,1,8,1);

  // equivalent to image.set_to_memory(&vec_data[0],8,8,1,1,8,1);

  vcl_cout<<"View as image:\n";
  vil_print_all(vcl_cout,image);

  vcl_cout<<"\nNote that if the raw data gets destroyed\n"
            "then the view could be left pointing at nothing!\n"
            "You have been warned.\n";

  return 0;
}
