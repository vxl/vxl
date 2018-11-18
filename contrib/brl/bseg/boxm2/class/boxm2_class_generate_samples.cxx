#include <iostream>
#include <algorithm>
#include <cstddef>
#include "boxm2_class_generate_samples.h"
#include <bvgl/bvgl_labelme_parser.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <boxm2/boxm2_util.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

boxm2_class_generate_samples::boxm2_class_generate_samples(std::string xml_label,
                                                           const std::string& eoPath,
                                                           const std::string& irPath,
                                                           std::size_t K)
{
  //parse file
  bvgl_labelme_parser parser(xml_label);

  std::cout<<"XML: "<<xml_label<<" eo: "<<eoPath<<" ir: "<<irPath<<std::endl;

  //grab image
  vil_image_view_base_sptr eo = boxm2_util::prepare_input_image(eoPath, false);
  vil_image_view_base_sptr ir = boxm2_util::prepare_input_image(irPath);
  vil_image_view<vil_rgba<vxl_byte> >& eoImg =
      *dynamic_cast<vil_image_view<vil_rgba<vxl_byte> >* >(eo.ptr());
  vil_image_view<float>& irImg = *static_cast<vil_image_view<float>* >(ir.ptr());
  assert( eoImg && irImg );
  assert( eoImg.ni() == irImg.ni() && eoImg.nj() == irImg.nj() );

  //grab pointer to image data
  std::size_t ni = eoImg.ni(),
             nj = eoImg.nj();

  //randomly choose K pixels and class
  int* buffer = new int[ni*nj];
  for (unsigned int i=0; i<ni*nj; ++i) buffer[i] = i;
  boxm2_util::random_permutation(buffer, ni*nj);

  //limit K to make sense
  K = std::min(K, ni*nj);
  for (unsigned int k=0; k<K; ++k) {
    //get image coordinates
    int i = buffer[k] % ni;
    int j = (buffer[k] - i)/ni;
    vgl_point_2d<int> pt(i,j);
    points_.push_back(pt);

    //push back pixel class
    classes_.push_back(pixel_class(i,j, parser.polygons(), parser.obj_names()));

    //store intensity
    vil_rgba<vxl_byte>& pixel = eoImg(i,j);
    r_.push_back( pixel.R()/255.0f );
    g_.push_back( pixel.G()/255.0f );
    b_.push_back( pixel.B()/255.0f );
    intensities_.push_back( irImg(i,j) );
  }

#if 0
  //write classes onto test image
  vil_image_view<vxl_byte> out(ni,nj);
  out.fill(0);
  for (int i=0; i<points_.size(); ++i)
  {
    if (classes_[i] == "water")
      out(points_[i].x(), points_[i].y()) = (vxl_byte) 255;
  }
  std::cout<<"Saving for image "<<image_path<<std::endl;
  vil_save(out, "test.png");
#endif
}

// Helper
std::string
pixel_class(int i, int j, std::vector<vgl_polygon<double> >& polygons, std::vector<std::string>& classes)
{
  for (unsigned int c=0; c<polygons.size(); ++c)
    if (polygons[c].contains((double) i, (double)j))
      return classes[c];
  return "noclass";
}
