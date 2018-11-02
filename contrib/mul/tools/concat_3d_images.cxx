#include <iostream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <numeric>
#include <functional>
#include <cstring>
#include <cassert>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vbl/vbl_array_3d.hxx>
#include <vil3d/vil3d_property.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_load.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_copy.h>
#include <vil3d/file_formats/vil3d_gen_synthetic.h>
#include <vimt3d/vimt3d_add_all_loaders.h>

void usage(char * progname)
{
  std::cout << "Usage:\n"
           << progname << " [-m margin_width] [-b voxel_value] output image1 image2 - image3 image4 -- image5 ...\n"
           << '\n'
           << "Option: -m number: Set the margin between images - defaults to 0\n"
           << "Option: -b number: Set background voxel value - defaults to 0\n"
           << "Each new image will be concatenated to the right of the previous.\n"
           << "\"-\" means start a new row\n"
           << "\"--\" means start a new slice\n"
           << "Output will have the pixel sizes, number of planes, etc. of image1.\n";
  std::exit(2);
}

void parse_cmdline(int argc, const char **argv, vbl_array_3d<std::string>& fnames)
{
  const char **arg = argv;

  unsigned max_i=0, max_j=0, max_k=0;
  unsigned i=0, j=0, k=0;
  for (int a = 0; a!=argc; ++arg, ++a)
  {
    if (std::string("--") == *arg)
    {
      ++k;
      i=0;
      j=0;
    }
    else if (std::string("-") == *arg)
    {
      ++j;
      i=0;
    }
    else
    {
      ++i;
      if (i==1 && j==0) j=1;
      if (j==1 && k==0) k=1;
    }
    if (i > max_i) max_i = i;
    if (j > max_j) max_j = j;
    if (k > max_k) max_k = k;
  }

  fnames.resize(max_i, max_j, max_k);

  arg = argv;
  i = j = k = 0;

  for (int a = 0; a!=argc; ++arg, ++a)
  {
    if (std::string("--") == *arg)
    {
      ++k;
      i=0;
      j=0;
    }
    else if (std::string("-") == *arg)
    {
      ++j;
      i=0;
    }
    else
    {
      fnames(i,j,k) = *arg;
      ++i;
    }
  }
}


vil3d_image_resource_sptr fname_to_resource(const std::string& fname)
{
  if (fname.empty()) return nullptr;

  vil3d_image_resource_sptr im = vil3d_load_image_resource(fname.c_str());

  if (!im)
  {
    std::cerr << "ERROR: unable to load image \"" << fname << "\"\n";
    std::exit(4);
  }

  return im;
}


void calc_image_sizes(
  const vbl_array_3d<vil3d_image_resource_sptr> &images,
  unsigned margin,
  std::vector<unsigned> & sizes_i,
  std::vector<unsigned> & sizes_j,
  std::vector<unsigned> & sizes_k)
{
  // Get max size of each slice of images in each direction.

  std::vector<unsigned>
    max_size_k(images.get_row3_count(), 0u),
    max_size_j(images.get_row2_count(), 0u),
    max_size_i(images.get_row1_count(), 0u);

  for (unsigned int k = 0; k < images.get_row3_count(); ++k)
    for (unsigned int j = 0; j < images.get_row2_count(); ++j)
      for (unsigned int i = 0; i < images.get_row1_count(); ++i)
        if (images(i,j,k))
        {
          max_size_k[k]  = std::max(max_size_k[k], images(i,j,k)->nk());
          max_size_j[j]  = std::max(max_size_j[j], images(i,j,k)->nj());
          max_size_i[i]  = std::max(max_size_i[i], images(i,j,k)->ni());
        }

  // Add margins.
  std::transform(max_size_k.begin(), max_size_k.end(), max_size_k.begin(),
                std::bind2nd(std::plus<unsigned>(),margin));
  std::transform(max_size_j.begin(), max_size_j.end(), max_size_j.begin(),
                std::bind2nd(std::plus<unsigned>(),margin));
  std::transform(max_size_i.begin(), max_size_i.end(), max_size_i.begin(),
                std::bind2nd(std::plus<unsigned>(),margin));

  // Sum these max sizes to find the position of each input image, in the output image.

  sizes_k.resize(images.get_row3_count()+1);
  sizes_j.resize(images.get_row2_count()+1);
  sizes_i.resize(images.get_row1_count()+1);
  sizes_k.front() = sizes_j.front() = sizes_i.front() = 0;
  std::partial_sum(max_size_k.begin(), max_size_k.end(), sizes_k.begin()+1);
  std::partial_sum(max_size_j.begin(), max_size_j.end(), sizes_j.begin()+1);
  std::partial_sum(max_size_i.begin(), max_size_i.end(), sizes_i.begin()+1);

  //Remove margin at far end of concatenated volume.
  sizes_k.back() -= margin;
  sizes_j.back() -= margin;
  sizes_i.back() -= margin;
}

int main(int argc, char*argv[])
{
  double background = 0.0;
  unsigned margin = 0;
  char* progname = argv[0];

  while (argc >= 2 && *argv[1] == '-')
  {
    if (std::string("-b") == argv[1])
    {
      if (argc == 2) usage(progname);
      background = std::atof(argv[2]);
      argv += 2;
      argc -= 2;
    }

    if (std::string("-m") == argv[1])
    {
      if (argc == 2) usage(progname);
      margin = std::atoi(argv[2]);
      argv += 2;
      argc -= 2;
    }

    if (std::string("-?") == argv[1] ||
        std::string("--help") == argv[1] || std::string("-h") == argv[1])
      usage(progname);
  }


  if (argc < 3) usage(progname);

  const char * filename = argv[1];

  if (vul_file::exists(filename))
  {
    std::cerr << "ERROR: Output file \"" << filename << "\" already exists.\n";
    std::exit(5);
  }

  vbl_array_3d<std::string> fnames;


  parse_cmdline(argc-2, const_cast<const char **>(argv+2), fnames);


  for (unsigned int k = 0; k < fnames.get_row3_count(); ++k)
  {
    for (unsigned int j = 0; j < fnames.get_row2_count(); ++j)
    {
      for (unsigned int i = 0; i < fnames.get_row1_count(); ++i)
        std::cout << '\"' << fnames(i,j,k) << "\" ";
      std::cout << std::endl;
    }
    std::cout << "END_SLICE" << std::endl;
  }

  vbl_array_3d<vil3d_image_resource_sptr> im_resources(
    fnames.get_row1_count(), fnames.get_row2_count(),
    fnames.get_row3_count());

  vimt3d_add_all_loaders();
  vil3d_file_format::add_format(new vil3d_gen_synthetic_format);

  std::transform(fnames.begin(), fnames.end(),
                im_resources.begin(), fname_to_resource);

  if (!im_resources(0,0,0))
  {
    std::cerr << "ERROR: The first (front-top-left) input image has not been correctly specified.";
    std::exit(6);
  }

  std::vector<unsigned> sizes_i;
  std::vector<unsigned> sizes_j;
  std::vector<unsigned> sizes_k;
  calc_image_sizes(im_resources, margin, sizes_i, sizes_j, sizes_k);

  vil3d_image_resource_sptr output =
    vil3d_new_image_resource(filename,
                             sizes_i.back(), sizes_j.back(), sizes_k.back(),
                             im_resources(0,0,0)->nplanes(),
                             im_resources(0,0,0)->pixel_format());

  if (!output)
  {
    std::cerr << "ERROR: Unable to create output file \"" <<  filename << "\"\n";
    std::exit(7);
  }

  vil3d_gen_synthetic_pixel_value pv;
  switch (im_resources(0,0,0)->pixel_format())
  {
  case VIL_PIXEL_FORMAT_DOUBLE:
    pv.double_value=background;
    break;
  case VIL_PIXEL_FORMAT_FLOAT:
    pv.float_value=(float)background;
    break;
  case VIL_PIXEL_FORMAT_INT_32:
    pv.int_32_value=(vxl_int_32)background;
    break;
  case VIL_PIXEL_FORMAT_UINT_32:
    pv.uint_32_value=(vxl_uint_32)background;
    break;
  case VIL_PIXEL_FORMAT_INT_16:
    pv.int_16_value=(vxl_int_16)background;
    break;
  case VIL_PIXEL_FORMAT_UINT_16:
    pv.uint_16_value=(vxl_uint_16)background;
    break;
  case VIL_PIXEL_FORMAT_BYTE:
    pv.byte_value=(vxl_byte)background;
    break;
  case VIL_PIXEL_FORMAT_SBYTE:
    pv.sbyte_value=(vxl_sbyte)background;
    break;
  default:
    std::memset(&pv, 0, sizeof(pv));
    break;
  }

  vil3d_image_resource_sptr blank =
    new vil3d_gen_synthetic_image(sizes_i.back(), sizes_j.back(), sizes_k.back(),
                                  im_resources(0,0,0)->pixel_format(), pv);

#ifndef NDEBUG
  bool rv =
#endif
    vil3d_copy_deep(blank, output);
  assert(rv);

  for (unsigned int k = 0; k < im_resources.get_row3_count(); ++k)
    for (unsigned int j = 0; j < im_resources.get_row2_count(); ++j)
      for (unsigned int i = 0; i < im_resources.get_row1_count(); ++i)
        if (im_resources(i,j,k))
        {
          vil3d_image_view_base_sptr in = im_resources(i,j,k)->get_view();
          if (!output->put_view(*in, sizes_i[i], sizes_j[j], sizes_k[k]))
          {
            std::cerr << "ERROR: Unable to copy image \"" << fnames(i,j,k)
                     << "\" into output image \"" << filename << "\"\n";
            std::exit(8);
          }
        }

  float voxel_size[3];
  im_resources(0,0,0)->get_property(vil3d_property_voxel_size, voxel_size);
  output->set_voxel_size_mm(voxel_size[0], voxel_size[1], voxel_size[2]);
  return 0;
}
