//:
// \file
// \brief Example of setting up view of external data.
// \author Tim Cootes - Manchester

#include <vil2/vil2_image_view.h>
#include <vil/vil_byte.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>

int main(int argc, char** argv)
{
  // Create some data in memory
  vcl_vector<vil_byte> vec_data(64);
  for (int i=0;i<vec_data.size();++i) vec_data[i] = i;

  vcl_cout<<"Creating a view of some data in memory"<<vcl_endl;
  vil2_image_view<vil_byte> image(&vec_data[0],8,8,1,1,8,1);

  // equivalent to image.set_to_memory(&vec_data[0],8,8,1,1,8,1);

  vcl_cout<<"View as image:"<<vcl_endl;
  image.print_all(vcl_cout);

  vcl_cout<<vcl_endl;
  vcl_cout<<"Note that if the raw data gets destroyed "<<vcl_endl;
  vcl_cout<<"then the view could be left pointing at nothing!"<<vcl_endl;
  vcl_cout<<"You have been warned."<<vcl_endl;

  return 0;
}
