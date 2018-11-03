//: \file
//  \author Ian Scott
//  \date 16 Dec 2009
//  \brief Program to split a 3d image into blocks.

#include <iostream>
#include <exception>
#include <vul/vul_arg.h>
#include <vul/vul_reg_exp.h>
#include <vul/vul_sprintf.h>
#include <vnl/vnl_math.h>
#include <mbl/mbl_log.h>
#include <vimt3d/vimt3d_add_all_loaders.h>
#include <vimt3d/vimt3d_load.h>
#include <vimt3d/vimt3d_save.h>
#include <vimt3d/vimt3d_vil3d_v3i.h>
#include <vil3d/vil3d_save.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_crop.h>
#include <vul/vul_string.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=========================================================================
// Static function to create a static logger when first required
//=========================================================================
static mbl_logger& logger()
{
  static mbl_logger l("mul.tools.image3d_split");
  return l;
}


//========================================================================
// Actual main function
//========================================================================
int main2(int argc, char*argv[])
{
  const bool use_millimeters=false;

  // Parse the program arguments
  vul_arg_base::set_help_precis("Divide a volume image in smaller pieces.");
  vul_arg<std::string> img_src(nullptr, "input image filename");
  vul_arg<std::string> img_dst(nullptr, "output image file-stem");
  vul_arg<std::string> max_voxels_arg(nullptr, "Max number of voxels in an output image. (Suffix k=1000, kb=1024, M=1e6, G=1e9 etc.)");
  vul_arg<std::string> replace_str("-I", "Replace this string in the filestem with the split numbering. Default puts it at end.");
  vul_arg<std::string> output_format_arg("-f", "Format of output images (e.g. v3i, dcm, gipl.) Guesses from filestem by default.");
  vul_arg<bool> split_z("-z", "Split along z-plane boundaries, default is rough cubes.");
  vul_arg<bool> label_seq("-s", "Number output images sequentially, default is according to x,y,z pos");
//  vul_arg<bool> ("-t", "Try to keep all images roughly same size - Default keeps all bu last ones identically sized.");
  vul_arg_parse(argc, argv);

  auto max_voxels = (unsigned long) vul_string_atof_withsuffix(max_voxels_arg());
  if (max_voxels==0) vul_arg_base::display_usage_and_exit("Bad maximum number of voxels");


  // Determine the output filetype
  std::string output_format;
  if (!output_format_arg.set())
  {
    vul_reg_exp re("([^A-Za-z0-9])$");
    std::string filename_test;
    if (re.find(img_dst()))
      filename_test = img_dst().substr(0, re.start()); // strip trailing "._, "etc;
    else
      filename_test = img_dst();
    output_format = vil3d_save_guess_file_format(filename_test.c_str());
  }
  else
    output_format = output_format_arg();
  MBL_LOG(INFO, logger(), "Output format: " << output_format);


  // Load the image.

  vimt3d_add_all_loaders();

  vil3d_image_resource_sptr ir = vil3d_load_image_resource(img_src().c_str());
  if (!ir)
  {
    std::cerr << "ERROR: Failed to load input image resource\n";
    return 1;
  }

  MBL_LOG(INFO, logger(), "Loaded input image_resource: " << ir->ni() << " x " <<
          ir->nj() << " x " << ir->nk() << " x " << ir->nplanes());

  vimt3d_transform_3d w2i = vimt3d_load_transform(ir, use_millimeters);

  // Figure out the block arrangement.

  unsigned long ni = ir->ni();
  unsigned long nj = ir->nj();
  unsigned long nk = ir->nk();

  unsigned blockwidth_i, blockwidth_j, blockwidth_k;
  unsigned blockcount_i, blockcount_j, blockcount_k;

  if (split_z())
  {
    if (max_voxels < ni*nj)
    {
      std::cerr << "ERROR: More voxels in single plane than " << max_voxels << '\n';
      return 3;
    }

    blockwidth_i = ni;
    blockwidth_j = nj;
    blockcount_i = 1;
    blockcount_j = 1;
    blockwidth_k = std::min(nk,  max_voxels / (ni*nj));
    blockcount_k = (nk+blockwidth_k-1) / blockwidth_k;
  }
  else
  {
    blockwidth_k = std::min<unsigned long>(nk,
                                          vnl_math::rnd(std::cbrt(static_cast<double>(max_voxels))) );
    blockcount_k = (nk+blockwidth_k-1) / blockwidth_k;
    blockwidth_j = std::min<unsigned long>(nj,
                                          vnl_math::rnd(std::sqrt(static_cast<double>(max_voxels/blockwidth_k))));
    blockcount_j = (nj+blockwidth_j-1) / blockwidth_j;
    blockwidth_i = std::min<unsigned long>(ni,  max_voxels / (blockwidth_k*blockwidth_j));
    blockcount_i = (ni+blockwidth_i-1) / blockwidth_i;
  }

  MBL_LOG(INFO, logger(), "block sizes: " << blockwidth_i << ',' << blockwidth_i <<
          ',' << blockwidth_k << " block counts: " << blockcount_i << ',' << blockcount_i <<
          ',' << blockcount_k);


  // Figure out output filename pattern.

  std::string filename_pattern;

  if (label_seq())
    filename_pattern = vul_sprintf("%%0%dd",
                                   vnl_math::floor( std::log10( static_cast<double>(
                                     blockcount_i*blockcount_j*blockcount_k )))+1 );
  else
    filename_pattern = vul_sprintf("%%0%dd%%0%dd%%0%dd",
                                   vnl_math::floor(std::log10(static_cast<double>(blockcount_i)))+1,
                                   vnl_math::floor(std::log10(static_cast<double>(blockcount_j)))+1,
                                   vnl_math::floor(std::log10(static_cast<double>(blockcount_k)))+1 );

  if (replace_str.set())
  {
    std::string::size_type n = img_dst().find(replace_str());
    if (n==std::string::npos)
    {
      std::cerr << "ERROR: Could not find replace string \"" << replace_str()
               << "\" in \"" << img_dst() << "\"\n";
      return 3;
    }
    filename_pattern = img_dst().substr(0, n) + filename_pattern +
      img_dst().substr(n+replace_str().size());
  }
  else
    filename_pattern = img_dst() + filename_pattern;


  unsigned k0=0;
  for (unsigned k=0; k<blockcount_k; ++k)
  {
    unsigned klen = std::min<unsigned long>(nk, k0+blockwidth_k)-k0; // Handle truncated last block.
    unsigned j0=0;
    for (unsigned j=0; j<blockcount_j; ++j)
    {
      unsigned jlen = std::min<unsigned long>(nj, j0+blockwidth_j)-j0;
      unsigned i0=0;
      for (unsigned i=0; i<blockcount_i; ++i)
      {
        unsigned ilen = std::min<unsigned long>(ni, i0+blockwidth_i)-i0;

        vil3d_image_view_base_sptr block = ir->get_copy_view(i0, ilen, j0, jlen, k0, klen);

        std::string filename;
        if (label_seq())
          filename = vul_sprintf(filename_pattern.c_str(),
                                 i + j*blockcount_i + k*blockcount_i*blockcount_j );
        else
          filename = vul_sprintf(filename_pattern.c_str(), i, j, k);

        vil3d_image_resource_sptr ir2 = vil3d_new_image_resource(
          filename.c_str(), ilen, jlen, klen, block->nplanes(),
          block->pixel_format(), output_format.c_str());
        if (!ir2)
        {
          std::cerr << "ERROR: Failed to create output image resource\n";
          return 2;
        }
        vimt3d_transform_3d trans;
        trans.set_translation(-double(i0), -double(j0), -double(k0));
        vimt3d_save_transform(ir2, trans*w2i, use_millimeters);

        auto* v3i_ir2 = dynamic_cast<vimt3d_vil3d_v3i_image *>(ir2.as_pointer());
        if (v3i_ir2)
          v3i_ir2->set_world2im(trans*w2i);
        else
          vimt3d_save_transform(ir2, trans*w2i, true);

        ir2->put_view(*block);

        i0 += ilen;
      }
      j0 += jlen;
    }
    k0 += klen;
  }

  return 0;
}


//========================================================================
// Exception-handling wrapper around main function
//========================================================================
int main(int argc, char*argv[])
{
  // Initialize the logger
  mbl_logger::root().load_log_config_file();

  try
  {
    main2(argc, argv);
  }
  catch (std::exception& e)
  {
    std::cout << "caught exception " << e.what() << std::endl;
    return 3;
  }
  catch (...)
  {
    std::cout << "caught unknown exception" << std::endl;
    return 3;
  }


  return 0;
}
