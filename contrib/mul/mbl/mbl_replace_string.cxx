
//Function to build both integral images
#include <mbl/mbl_replace_string.h>



//: replace string
void mbl_replace_string( 
                          vcl_string& full_str,
                            const vcl_string& find_str,
                              const vcl_string& replace_str,
                                int num_times)
{
 
  // check full_str not empty
  //if (full_str.empty())
  //{
  //  full_str = replace_str;
  //  return;
  //}
  
  
  for (int i = 0; i<num_times; i++)
  {
    int loc = full_str.find( find_str,0);
    if (loc >= 0) 
    {
      full_str.replace( loc, find_str.length(), replace_str );
    }
    else 
    {
      return;
    }
  }
  
}



