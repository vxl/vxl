#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_cstdlib.h>
#include <vcl_algorithm.h>
#include <vcl_vector.h>
#include <vcl_numeric.h>
#include <vul/vul_file.h>
#include <vbl/vbl_array_3d.txx>
#include <vbl/vbl_triple.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_load.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_copy.h>
#include <vil3d/file_formats/vil3d_gen_synthetic.h>
#include <vimt3d/vimt3d_add_all_loaders.h>

void usage(char * progname)
{
  vcl_cout << "Usage: " << progname << "output image1 image2 - image3 image4 -- image5 ...\n"
    "\n"
    "Each new image will be concatenated to the right of the previous.\n"
    "\"-\" means start a new row\n"
    "\"--\" means start a new slice\n";
    "Output will have the pixel sizes, number of planes, etc. of image1.\n";
  vcl_exit(2);
}

void parse_cmdline(int argc, const char **argv, vbl_array_3d<vcl_string>& fnames)
{
  const char **arg = argv;

  unsigned max_i=0, max_j=0, max_k=0;
  unsigned i=0, j=0, k=0;
  for (unsigned a = 0; a!=argc; ++arg, ++a)
  {
    if (vcl_string("--") == *arg)
    {
      ++k;
      i=0;
      j=0;
    }
    else if (vcl_string("-") == *arg)
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

  for (unsigned a = 0; a!=argc; ++arg, ++a)
  {
    if (vcl_string("--") == *arg)
    {
      ++k;
      i=0;
      j=0;
    }
    else if (vcl_string("-") == *arg)
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



vil3d_image_resource_sptr fname_to_resource(const vcl_string& fname)
{
  if (fname.empty()) return 0;

  vil3d_image_resource_sptr im = vil3d_load_image_resource(fname.c_str());

  if (!im)
  {
    vcl_cerr << "ERROR: unable to load image \"" << fname << '\"' << vcl_endl;
    vcl_exit(4);
  }

  return im;
}



void calc_image_sizes(
  const vbl_array_3d<vil3d_image_resource_sptr> &images, 
  vcl_vector<unsigned> & sizes_i,
  vcl_vector<unsigned> & sizes_j,
  vcl_vector<unsigned> & sizes_k)
{

  // Get max size of each slice of images in each direction.

  vcl_vector<unsigned>
    max_size_k(images.get_row3_count(), 0u),
    max_size_j(images.get_row2_count(), 0u),
    max_size_i(images.get_row1_count(), 0u);

  for (int k = 0; k < images.get_row3_count(); ++k)
    for (int j = 0; j < images.get_row2_count(); ++j)
      for (int i = 0; i < images.get_row1_count(); ++i)
        if (images(i,j,k))
        {
          max_size_k[k]  = vcl_max(max_size_k[k], images(i,j,k)->nk());
          max_size_j[j]  = vcl_max(max_size_j[j], images(i,j,k)->nj());
          max_size_i[i]  = vcl_max(max_size_i[i], images(i,j,k)->ni());
        }

  // Sum these max sizes to find the postion of each input image, in the output image.

  sizes_k.resize(images.get_row3_count()+1);
  sizes_j.resize(images.get_row2_count()+1);
  sizes_i.resize(images.get_row1_count()+1);
  sizes_k.front() = sizes_j.front() = sizes_i.front() = 0;
  vcl_partial_sum(max_size_k.begin(), max_size_k.end(), sizes_k.begin()+1);
  vcl_partial_sum(max_size_j.begin(), max_size_j.end(), sizes_j.begin()+1);
  vcl_partial_sum(max_size_i.begin(), max_size_i.end(), sizes_i.begin()+1);

}

int main(int argc, char*argv[])
{
  if (argc == 2 || vcl_string("-?") == argv[1] ||
    vcl_string("--help") == argv[1] || vcl_string("-h") == argv[1])
    usage(argv[0]);

  const char * filename = argv[1];

  if (vul_file::exists(filename))
  {
    vcl_cerr << "ERROR: Output file \"" << filename << "\" already exists." << vcl_endl;
    vcl_exit(5);
  }

  vbl_array_3d<vcl_string> fnames;

  parse_cmdline(argc-2, const_cast<const char **>(argv+2), fnames);


  for (int k = 0; k < fnames.get_row3_count(); ++k)
  {
    for (int j = 0; j < fnames.get_row2_count(); ++j)
    {
      for (int i = 0; i < fnames.get_row1_count(); ++i)
        vcl_cout << '\"' << fnames(i,j,k) << "\" ";
      vcl_cout << vcl_endl;
    }
    vcl_cout << "END_SLICE" << vcl_endl;
  }

  vbl_array_3d<vil3d_image_resource_sptr> im_resources(
    fnames.get_row1_count(), fnames.get_row2_count(), 
    fnames.get_row3_count());

  vimt3d_add_all_loaders();
  vil3d_file_format::add_format(new vil3d_gen_synthetic_format);

  vcl_transform(fnames.begin(), fnames.end(),
    im_resources.begin(), fname_to_resource);

  if (!im_resources(0,0,0))
  {
    vcl_cerr << "ERROR: The first (front-top-left) input image has not been correctly specified.";
    vcl_exit(6);
  }

  vcl_vector<unsigned> sizes_i;
  vcl_vector<unsigned> sizes_j;
  vcl_vector<unsigned> sizes_k;
  calc_image_sizes(im_resources, sizes_i, sizes_j, sizes_k);
 
  vil3d_image_resource_sptr output = 
    vil3d_new_image_resource (filename, sizes_i.back(), sizes_j.back(), sizes_k.back(),
    im_resources(0,0,0)->nplanes(), im_resources(0,0,0)->pixel_format());

  if (!output)
  {
    vcl_cerr << "ERROR: Unable to create output file \"" <<  filename << '\"' << vcl_endl;
    vcl_exit(7);
  }

  vil3d_gen_synthetic_pixel_value pv;
  pv.double_value=0.0;

  vil3d_image_resource_sptr blank = 
    new vil3d_gen_synthetic_image( sizes_i.back(), sizes_j.back(), sizes_k.back(),
      im_resources(0,0,0)->pixel_format(), pv);

  bool rv = vil3d_copy_deep(blank, output);
  assert(rv);

  for (int k = 0; k < im_resources.get_row3_count(); ++k)
    for (int j = 0; j < im_resources.get_row2_count(); ++j)
      for (int i = 0; i < im_resources.get_row1_count(); ++i)
        if (im_resources(i,j,k))
        {
          vil3d_image_view_base_sptr in = im_resources(i,j,k)->get_view();
          if (!output->put_view(*in, sizes_i[i], sizes_j[j], sizes_k[k]))
          {
            vcl_cerr << "ERROR: Unable to copy image \"" << fnames(i,j,k) <<
              "\" into output image \"" << filename << '\"' << vcl_endl;
            vcl_exit(8);
          }
        }

  return 0;
}
