#include <iostream>
#include <fstream>
#include <sstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "boxm2_vecf_labeled_point.h"
//
// there are two anchor file formats at present:
// x, y, z, label
// ...
// i.e. 3-d point and label for all entries
// and one where the point is a scalar value
// to obtain uniformity scalar values are encoded as a 3-d point also
//
bool boxm2_vecf_labeled_point::read_points(std::string const& path,   std::map<std::string, std::vector<vgl_point_3d<double> > >& labeled_pts){
  std::ifstream istr(path.c_str());
  if(!istr.is_open()){
    std::cout << "point file " << path << " not found\n";
    return false;
  }
  // this loop finds the number of commas in a
  // line from the file. Thus determining
  // the second file format's  lines with scalar values

  while(true){
    if(istr.eof())
      break;
    char buf[100];
    istr.getline(buf,100);
    std::string buf_str;
    bool done = false;
    unsigned comma_count = 0;
    for(unsigned i =0; i<100&&!done; ++i){
      char c = buf[i];
      if(c == '\0'||c == '\n'){
        done = true;
        continue;
      }else{
        buf_str.push_back(c);
        if(c == ',')
          comma_count++;
      }
    }
    // Now that the number of commas is
    // known, the actual file parsing
    // phase can exectute
    std::stringstream isstr(buf_str);
    double x, y, z;
    unsigned char c;
    std::string lab;
    // the standard form with a 3-d point followed by a label
    if(comma_count == 3){
      isstr >> x >> c;
      if(c!=',')
        return false;
      isstr >> y >> c;
      if(c!=',')
        return false;
      isstr >> z >> c;
      if(c!=',')
        return false;
      isstr >> lab;
      if(lab=="") continue;
      // add the labeled point to the map
      vgl_point_3d<double> p(x,y,z);
      labeled_pts[lab].push_back(p);
    }else if(comma_count == 1){// the scalar case
      isstr >> x >> c >> lab;
      if(c!=','||lab == "")
        return false;
      vgl_point_3d<double> p(x,x,x);
      labeled_pts[lab].push_back(p);
    }else if(buf_str != ""){
      std::cout << "Bad file format line " << buf_str << '\n';
      return false;
    }
  }
  return true;
}
