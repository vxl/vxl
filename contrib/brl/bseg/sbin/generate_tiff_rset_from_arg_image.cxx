#include <string>
#include <iostream>
#include <cstdlib>
#include <vpl/vpl.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_pyramid_image_resource.h>
static bool convert_to_greyscale(vil_image_resource_sptr const& imgr, vil_image_resource_sptr& resc){
  vil_pixel_format fmt = imgr->pixel_format();
  unsigned ni = imgr->ni(), nj = imgr->nj();
  unsigned np = imgr->nplanes();
  auto npd = static_cast<double>(np);
  for(unsigned j = 0; j<nj; ++j){
    if(fmt == VIL_PIXEL_FORMAT_BYTE){
      vil_image_view<unsigned char> img = imgr->get_view(0, ni, j, 1);
      vil_image_view<unsigned char> out(ni,1);
      for(unsigned i = 0; i<ni; ++i){
        double sum = 0.0;
        for(unsigned p = 0; p<np; ++p)
          sum += img(i, 0, p);
        sum /= npd;
        out(i,0)=static_cast<unsigned char>(sum);
      }
      resc->put_view(out, 0, j);
    }else if(fmt == VIL_PIXEL_FORMAT_UINT_16){
      vil_image_view<unsigned short> img = imgr->get_view(0, ni, j, 1);
      vil_image_view<unsigned short> out(ni,1);
      for(unsigned i = 0; i<ni; ++i){
        double sum = 0.0;
        for(unsigned p = 0; p<np; ++p)
          sum += img(i, 0, p);
        sum /= npd;
        out(i,0)=static_cast<unsigned short>(sum);
      }
      resc->put_view(out, 0, j);
    }else if( fmt == VIL_PIXEL_FORMAT_RGBA_UINT_16){
      // has a mask so set data to zero when mask is zero
      if(np != 4){
        std::cout << "RGBA doesn't have 4 planes\n";
        return false;
      }
      vil_image_view<unsigned short> img = imgr->get_view(0, ni, j, 1);
      vil_image_view<unsigned short> out(ni,1);
      for(unsigned i = 0; i<ni; ++i){
        double sum = 0.0;
        for(unsigned p = 0; p<3; ++p)
          sum += img(i, 0, p);
        sum /= npd;
        if(img(i,0,3))
          out(i,0)=static_cast<unsigned short>(sum);
        else
          out(i,0)= static_cast<unsigned short>(0);
      }
      resc->put_view(out, 0, j);
    }else{
      std::cout << "Unknown pixel format\n";
      return false;
    }
  }
  return true;
}

//assumes that the base image is in the dir and the dir is
//named for the image without the extension.
static bool generate_rset(std::string const& base_image_path,
                          std::string const& pyramids_dir,
                          const unsigned nlevels, const unsigned greyscale, std::string& grey_outname)
{
  std::string slash;
  //generate the temporary dir
#ifdef _WIN32
  slash =  "\\";
#else
  slash = "/";
#endif
  // load the base image
  vil_image_resource_sptr base_image
    = vil_load_image_resource(base_image_path.c_str());
  if (!base_image)
    return false;
  if(!vul_file::is_directory(pyramids_dir))
        if(!vul_file::make_directory(pyramids_dir)){
    std::cout << "Can't make pyramids containing directory "<< pyramids_dir << '\n';
    return false;
  }
  std::string base_image_name = vul_file::strip_directory(base_image_path);
  base_image_name = vul_file::strip_extension(base_image_name);
  std::string pyramid_dir = pyramids_dir + slash + base_image_name;
  if(!vul_file::is_directory(pyramid_dir))
    if(!vul_file::make_directory(pyramid_dir)){
    std::cout << "Can't make pyramid directory "<< pyramid_dir << '\n';
    return false;
  }
  grey_outname = pyramid_dir + slash + base_image_name + "_temp.tif";
  if(greyscale){
    vil_pixel_format fmt = base_image->pixel_format();
  if(fmt == VIL_PIXEL_FORMAT_RGBA_UINT_16)
    fmt = VIL_PIXEL_FORMAT_UINT_16;
  vil_image_resource_sptr resc =  vil_new_image_resource(grey_outname.c_str(), base_image->ni(), base_image->nj(), 1, fmt, "tiff");
  if(!resc)
    return false;
  if(!convert_to_greyscale(base_image, resc))
    {
      std::cout << "Failed to convert "<< base_image_path << " to greyscale\n";
      return -1;
    }
  }
  vil_pyramid_image_resource_sptr pir;
  if(greyscale){//reload to access data in resc
    vil_image_resource_sptr resc = vil_load_image_resource(grey_outname.c_str());
    vil_blocked_image_resource_sptr blk_resc = vil_new_blocked_image_facade(resc, 256, 256).ptr();
    vil_image_resource_sptr blk_ptr = dynamic_cast<vil_image_resource*>(blk_resc.ptr());
    pir = vil_new_pyramid_image_list_from_base(pyramid_dir.c_str(), blk_ptr, nlevels,
                                                true, "tiff", base_image_name.c_str());
  }else{
    vil_blocked_image_resource_sptr blk_base = vil_new_blocked_image_facade(base_image, 256, 256);
    vil_image_resource_sptr blk_base_ptr = dynamic_cast<vil_image_resource*>(blk_base.ptr());
    pir = vil_new_pyramid_image_list_from_base(pyramid_dir.c_str(), blk_base_ptr, nlevels,
                                               true, "tiff", base_image_name.c_str());
  }
  return (bool)pir;
}

int main(int argc,char * argv[])
{
    if (argc<4)
    {
      std::cout<<"Usage : generate_rset.exe base_image_path pyramids_dir nlevels greyscale(0||1)\n";
      return -1;
    }
    else
    {
      std::string base_image_path(argv[1]);
      std::string pyramids_dir(argv[2]);
      unsigned nlevels = std::atoi(argv[3]);
      unsigned greyscale = 0;
      if(argc==5)
        greyscale = std::atoi(argv[4]);
      std::cout << base_image_path << std::endl
               <<  pyramids_dir << std::endl
                << nlevels << std::endl << greyscale << std::endl;
      if (nlevels<2)
      {
        std::cout << "Must have at least 2 levels\n";
        return 0;
      }
      std::string grey_outname;
      if (!generate_rset(base_image_path, pyramids_dir, nlevels, greyscale, grey_outname))
    {
        std::cout << "Generate R Set failed\n";
        return -1;
      }
      if(greyscale)
        vpl_unlink(grey_outname.c_str());
      return 0;
    }
}
