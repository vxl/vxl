//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/16/08
//
//

#include <rec/bvxm_glitch.h>
#include <vcl_cmath.h>
#include <vil/vil_save.h>

vil_image_view<vxl_byte>
bvxm_glitch::square_glitch_mask_img(int c_size)
{
  //: find the neighborhood by preparing a map image
  int c_size_outer = c_size+2;
  while (c_size_outer*c_size_outer - c_size*c_size < c_size*c_size)
    c_size_outer+=2;

  vil_image_view<vxl_byte> map_img(c_size_outer, c_size_outer);
  map_img.fill(0);
  unsigned cnt = 0;
  int dif = (int)vcl_floor((c_size_outer-c_size)/2.0f+0.5f);
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
  vcl_stringstream ss;
  ss << c_size;
  vcl_string name = "./glitch_mask_"+ss.str()+"x"+ss.str()+".png";
  vil_save(map_img, name.c_str());
#endif

  return map_img;
}

//: given a size, generate a square center mask together with a surround neighborhood mask with the same number of pixels with the center
void
bvxm_glitch::square_glitch(int c_size, vcl_vector<vcl_pair<int, int> >& neighborhood_center, vcl_vector<vcl_pair<int, int> >& neighborhood_surround)
{
  neighborhood_center.clear();
  neighborhood_surround.clear();

  vil_image_view<vxl_byte> map_img = square_glitch_mask_img(c_size);
  int c_size_outer = (int)map_img.ni();

  for (int i = 0; i < c_size_outer; i++)
    for (int j = 0; j < c_size_outer; j++) {
      if (map_img(i,j) == 100)
        neighborhood_center.push_back(vcl_pair<int, int>(i-(c_size_outer/2), j-(c_size_outer/2)));
    }

  for (int i = 0; i < (int)c_size_outer; i++)
    for (int j = 0; j < (int)c_size_outer; j++) {
      if (map_img(i,j) == 255)
        neighborhood_surround.push_back(vcl_pair<int, int>(i-(c_size_outer/2), j-(c_size_outer/2)));
    }
}

void bvxm_glitch::extend_prob_to_square_region(int c_size, vil_image_view<float>& input_map, vil_image_view<float>& output_map)
{
  vcl_vector<vcl_pair<int, int> > neighborhood;
  vcl_vector<vcl_pair<int, int> > neighborhood_outer;
  square_glitch(c_size, neighborhood, neighborhood_outer);

  output_map.fill(0.0f);
  for (unsigned i = 0; i < input_map.ni(); i++) {
    for (unsigned j = 0; j < input_map.nj(); j++) {
      if (input_map(i,j) > 0) {
        for (unsigned k = 0; k < neighborhood.size(); k++) {
          int ii = i+neighborhood[k].first;
          int jj = j+neighborhood[k].second;
          if (ii > 0 && jj > 0 && ii < (int)output_map.ni() && jj < (int)output_map.nj())
            output_map(ii, jj) = input_map(i,j);
        }
      }
    }
  }
}

#if 0
//: output_img needs to have 3 planes
void
bvxm_glitch::generate_output_img(vcl_vector<bvxm_part_instance_sptr>& extracted_parts,
                                 vil_image_view<vxl_byte>& input_img,
                                 vil_image_view<vxl_byte>& output_img)
{
  unsigned ni = input_img.ni();
  unsigned nj = input_img.nj();
  output_img.fill(0);

  vil_image_view<float> map(ni, nj);
  generate_output_map(extracted_parts, map);
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++) {
      output_img(i,j,0) = input_img(i,j);
      output_img(i,j,1) = input_img(i,j);
      output_img(i,j,2) = (vxl_byte)(map(i,j)*255);
    }
}
#endif // 0


