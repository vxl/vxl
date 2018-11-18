#include <iostream>
#include <cmath>
#include <sstream>
#include "brec_glitch.h"
//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date October 16, 2008

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_save.h>

vil_image_view<vxl_byte>
brec_glitch::square_glitch_mask_img(int c_size)
{
  //: find the neighborhood by preparing a map image
  int c_size_outer = c_size+2;
  while (c_size_outer*c_size_outer - c_size*c_size < c_size*c_size)
    c_size_outer+=2;

  vil_image_view<vxl_byte> map_img(c_size_outer, c_size_outer);
  map_img.fill(0);
  unsigned cnt = 0;
  int dif = (c_size_outer-c_size+1)/2;
  for (int i = dif; i < (int)(dif+c_size); i++)
    for (int j = dif; j < (int)(dif+c_size); j++) {
      map_img(i,j) = 100;
      cnt++;
    }

  unsigned cnt2 = 0;
  int subt = 1;
  while (cnt2 < cnt || subt <= dif) {
    int is = dif-subt;
    for (int i = is; i < c_size_outer-is; i++) {
      bool did_break = false;
      for (int j = is; j < c_size_outer-is; j++) {
        if (!map_img(i,j)) {
          map_img(i,j) = 255;
          cnt2++;
          if (cnt2 >= cnt) {
            did_break = true;
            break;
          }
        }
      }
      if (did_break)
        break;
    }
    subt++;
  }

#if 1
  std::stringstream ss;
  ss << c_size;
  std::string name = "./glitch_mask_"+ss.str()+"x"+ss.str()+".png";
  vil_save(map_img, name.c_str());
#endif

  return map_img;
}

//: given a size, generate a square center mask together with a surround neighborhood mask with the same number of pixels with the center
void
brec_glitch::square_glitch(int c_size, std::vector<std::pair<int, int> >& neighborhood_center, std::vector<std::pair<int, int> >& neighborhood_surround)
{
  neighborhood_center.clear();
  neighborhood_surround.clear();

  vil_image_view<vxl_byte> map_img = square_glitch_mask_img(c_size);
  int c_size_outer = (int)map_img.ni();

  for (int i = 0; i < c_size_outer; i++)
    for (int j = 0; j < c_size_outer; j++) {
      if (map_img(i,j) == 100)
        neighborhood_center.emplace_back(i-(c_size_outer/2), j-(c_size_outer/2));
    }

  for (int i = 0; i < (int)c_size_outer; i++)
    for (int j = 0; j < (int)c_size_outer; j++) {
      if (map_img(i,j) == 255)
        neighborhood_surround.emplace_back(i-(c_size_outer/2), j-(c_size_outer/2));
    }
}

void brec_glitch::extend_prob_to_square_region(int c_size, vil_image_view<float>& input_map, vil_image_view<float>& output_map)
{
  std::vector<std::pair<int, int> > neighborhood;
  std::vector<std::pair<int, int> > neighborhood_outer;
  square_glitch(c_size, neighborhood, neighborhood_outer);

  output_map.fill(0.0f);
  for (unsigned i = 0; i < input_map.ni(); i++) {
    for (unsigned j = 0; j < input_map.nj(); j++) {
      if (input_map(i,j) > 0) {
        for (auto & k : neighborhood) {
          int ii = i+k.first;
          int jj = j+k.second;
          if (ii > 0 && jj > 0 && ii < (int)output_map.ni() && jj < (int)output_map.nj())
            output_map(ii, jj) = input_map(i,j);
        }
      }
    }
  }
}
