#include "bvpl_kernel_base.h"
#include <vcl_map.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_algorithm.h>

bool bvpl_kernel_base::warp_nearest_neighbor()
{
  vcl_vector<vcl_pair<vgl_point_3d<int>, bvpl_kernel_dispatch> >::iterator kernel_it =this->kernel_.begin();
  vcl_vector<vcl_pair<vgl_point_3d<int>, bvpl_kernel_dispatch> > new_map;
  vcl_map< char, unsigned int> new_symbols_map;
  
  
  //fill new_symbols_map to keep a new count for sympols
  #if 0
  vcl_map< char, unsigned int>::iterator syms_it = symbols_map_.begin();
  
  for(; syms_it != symbols_map_.end(); ++syms_it)
   new_symbols_map.insert(vcl_make_pair( syms_it->first, 0)); 
  #endif
  
  for(; kernel_it != kernel_.end(); ++kernel_it)
  {
    //Rotate, mantain floating point values
    vnl_vector_fixed<double,3> new_coord = R_*vnl_vector_fixed<double,3>(double((*kernel_it).first.x()),
                                                         double((*kernel_it).first.y()),
                                                         double((*kernel_it).first.z()));
    
    // cast to the nearest integer value
    int x0 = (int)vcl_floor(new_coord[0]+0.5f);
    int y0 = (int)vcl_floor(new_coord[1]+0.5f);
    int z0 = (int)vcl_floor(new_coord[2]+0.5f);
    
     new_map.push_back(vcl_make_pair(vgl_point_3d<int>(x0,y0,z0), (kernel_it->second)));
    
     // As is is implemented now, if many point to round a sigle integer, then that integer is used multiple times
     // this may be a good solution, and avoids the problem of getting unequal number of symbols
    
  }
 
  //TO DO: check the number of symbols is consistent
  //vcl_map< vgl_point_3d<int>, char >::iterator new_map_it = new_map.begin();
  
    
  //swap maps
  kernel_.swap(new_map);
  
}

//:Converts the kernel_map to a bvpl_kernel_iterator
#if 0
bvpl_kernel_iterator bvpl_kernel_base::iterator()
{
  //the iterator
  vcl_vector<vcl_pair<vgl_point_3d<int>, bvpl_kernel_dispatch> > kernel_vals;
  vcl_vector<vcl_pair<vgl_point_3d<int>, bvpl_kernel_dispatch> >::iterator k_it= kernel_.begin();
  
  for(; k_it!=kernel_.end(); ++k_it)
  {
    kernel_vals.push_back(vcl_make_pair<vgl_point_3d<int>, bvpl_kernel_dispatch>
                          (vgl_point_3d<int>(k_it->first[0],k_it->first[1],k_it->first[2]), bvpl_kernel_dispatch(k_it->second)));
  }
  return kernel_vals;
  
}
#endif



