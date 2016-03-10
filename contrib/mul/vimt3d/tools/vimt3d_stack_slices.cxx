//:
// \file
// \brief Tool to load in a set of 2D images and produce a 3D image
// \author Tim Cootes

#include <vul/vul_arg.h>
#include <iostream>
#include <vcl_compiler.h>
#include <iostream>
#include <fstream>
#include <vimt3d/vimt3d_save.h>
#include <vimt3d/vimt3d_image_3d_of.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil3d/vil3d_slice.h>
#include <vimt3d/vimt3d_add_all_loaders.h>

void print_usage()
{
  std::cout<<"vimt3d_stack_slices: Tool to load in a set of 2D images and produce a 3D image."<<std::endl;

  vul_arg_display_usage_and_exit();
}


int main(int argc, char** argv)
{
  vul_arg<std::string> image_path("-i","File containing list of 2D images");
  vul_arg<std::string> output_path("-o","Output path","image.v3i");
  vul_arg<double> wx("-wx","Voxel width along x",1.0);
  vul_arg<double> wy("-wy","Voxel width along y",1.0);
  vul_arg<double> wz("-wz","Voxel width along z",1.0);

  vul_arg_parse(argc,argv);
  if (image_path()=="")
  {
    print_usage();
    return 0;
  }

  vimt3d_add_all_loaders();

  // Attempt to read image image list from named file
  std::ifstream ifs(image_path().c_str());
  std::vector<std::string> names;
  std::string name;
  while (!ifs.eof())
  {
    ifs>>name>>std::ws;
    if (name.size()!=0) names.push_back(name);
  }

  if (names.size()==0)
  {
    std::cerr<<"No images listed in "<<image_path()<<std::endl;
    return 1;
  }

  std::cout<<"Loading in "<<names.size()<<" 2D slices."<<std::endl;

  // Load in the first image
  vil_image_view<float> image = vil_load(names[0].c_str());
  if (image.size()==0)
  {
    std::cerr<<"Unable to read image from "<<names[0]<<std::endl;
    return 2;
  }

  std::cout<<"First slice: "<<image<<std::endl;

  unsigned ni=image.ni(),nj=image.nj(),np=image.nplanes();
  unsigned nk=names.size();

  vimt3d_image_3d_of<float> image3d;
  image3d.image().set_size(ni,nj,nk,np);

  vil_image_view<float> slice = vil3d_slice_ij(image3d.image(),0);
  slice.deep_copy(image);

  for (unsigned k=1;k<nk;++k)
  {
    image = vil_load(names[k].c_str());
    if (image.ni()!=ni || image.nj()!=nj || image.nplanes()!=np)
    {
      std::cerr<<"Image "<<names[k]<<" does not match size of first."<<std::endl;
      return 3;
    }

    vil3d_slice_ij(image3d.image(),k).deep_copy(image);
  }

  vimt3d_transform_3d w2i;
  w2i.set_zoom_only(1.0/wx(),1.0/wy(),1.0/wz(),0,0,0);

  image3d.set_world2im(w2i);

  bool use_mm=true;
  if (!vimt3d_save(output_path(),image3d,use_mm))
  {
    std::cerr<<"Failed to save image to "<<output_path()<<std::endl;
    return 4;
  }
  std::cout<<"Saved to "<<output_path()<<std::endl;
}
