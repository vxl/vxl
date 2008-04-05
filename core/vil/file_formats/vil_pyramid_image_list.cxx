#include "vil_pyramid_image_list.h"
//:
// \file
#include <vcl_algorithm.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vcl_sstream.h>
#include <vil/vil_stream_fstream.h>
#include <vil/vil_image_list.h>
#include <vil/vil_blocked_image_facade.h>
#include <vil/vil_cached_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_load.h>
#include <vil/vil_copy.h>

//:Load a pyramid image.  The path should correspond to a directory.
//If not, return a null resource.
vil_pyramid_image_resource_sptr
vil_pyramid_image_list_format::make_input_pyramid_image(char const* directory)
{
  vil_image_list il(directory);
  vcl_vector<vil_image_resource_sptr> rescs = il.resources();
  unsigned nr = rescs.size();
  if (nr < 2)
    return 0;
  vil_pyramid_image_list* pil = new vil_pyramid_image_list(rescs);
  pil->set_directory(directory);
  return pil;
}

vil_pyramid_image_resource_sptr
vil_pyramid_image_list_format::make_pyramid_output_image(char const* file)
{
  if (!vil_image_list::vil_is_directory(file))
    return 0;
  return new vil_pyramid_image_list(file);
}

static bool copy_base_resc(vil_image_resource_sptr const& base_image,
                           vcl_string full_filename,
                           char const* file_format,
                           vil_blocked_image_resource_sptr& copy)
{
  { //scope for closing resource
  //Create a new blocked base image resource
  vcl_cout << "Copying base resource\n";
  vil_blocked_image_resource_sptr brsc = blocked_image_resource(base_image);
  if (!brsc||brsc->size_block_i()%2!=0||brsc->size_block_i()%2!=0)
    brsc = new vil_blocked_image_facade(base_image);
  vil_blocked_image_resource_sptr out_resc =
    vil_new_blocked_image_resource(full_filename.c_str(),
                                   brsc->ni(), brsc->nj(),
                                   brsc->nplanes(),
                                   brsc->pixel_format(),
                                   brsc->size_block_i(),
                                   brsc->size_block_j(),
                                   file_format);
  if (!out_resc)
    return false;
  for (unsigned j = 0; j<brsc->n_block_j(); ++j)
    for (unsigned i = 0; i<brsc->n_block_i(); ++i)
    {
      vil_image_view_base_sptr blk = brsc->get_block(i,j);
      if (!blk)
        return 0;
      if (!out_resc->put_block(i, j, *blk))
        return 0;
    }
  }//end scope for out resource
  //
  //reopen the resource for reading.
  vil_image_resource_sptr temp = vil_load_image_resource(full_filename.c_str());
  copy = blocked_image_resource(temp);
  if (!copy)
    return false;
  return true;
}

static vcl_string level_filename(vcl_string& directory, vcl_string& filename,
                                 float level)
{
  vcl_string slash;

#ifdef VCL_WIN32
  slash =  "\\";
#else
  slash = "/";
#endif
  vcl_stringstream cs;
  cs << level;
  return directory + slash + filename + "_" + cs.str();
}

//: Construct pyramid image files in the directory
//  Each level has the same scale ratio (0.5) to the preceeding level.
//  Level 0 is the original base image.
vil_pyramid_image_resource_sptr vil_pyramid_image_list_format::
    make_pyramid_image_from_base(char const* directory,
                                 vil_image_resource_sptr const& base_image,
                                 unsigned nlevels,
                                 bool copy_base,
                                 char const* level_file_format,
                                 char const* filename
                                )
{
  if (!vil_image_list::vil_is_directory(directory))
    return 0;
  vcl_string d = directory;
  vcl_string fn = filename;
  vcl_string full_filename = level_filename(d,fn, 0.0f) + '.'+ level_file_format;
  vil_blocked_image_resource_sptr blk_base;
  if (copy_base)
  {
    if (!copy_base_resc(base_image, full_filename,
                        level_file_format, blk_base))
      return 0;
  }
  else
  {
    blk_base =
      blocked_image_resource(base_image);
    if (!blk_base)
      return 0;
  }
  //Create the other pyramid levels
  { //program scope to close resource files
    vil_image_resource_sptr image = blk_base.ptr();
    for (unsigned L = 1; L<nlevels; ++L)
    {
      vcl_cout << "Decimating Level " << L << vcl_endl;
      full_filename = level_filename(d, fn, float(L)) + '.'+ level_file_format;
      image = vil_pyramid_image_resource::decimate(image,full_filename.c_str());
    }
  } //end program scope to close resource files
  vil_image_list il(directory);
  vcl_vector<vil_image_resource_sptr> rescs = il.resources();
  return new vil_pyramid_image_list(rescs);
}

///==============  start vil_pyramid_image_list  =========================
//comparison of level scales
static bool level_compare(pyramid_level* const l1, pyramid_level* const l2)
{
  assert(l1&&l2);
  return l1->image_->ni() > l2->image_->ni();
}


vil_pyramid_image_list::vil_pyramid_image_list() : directory_("")
{}

vil_pyramid_image_list::vil_pyramid_image_list(char const* directory) : directory_(directory)
{}

vil_pyramid_image_list::vil_pyramid_image_list(vcl_vector<vil_image_resource_sptr> const& images) : directory_("")
{
  for (vcl_vector<vil_image_resource_sptr>::const_iterator rit = images.begin();
       rit != images.end(); ++rit)
  {
    //if the resource is blocked use a cached access
    vil_blocked_image_resource_sptr brsc = blocked_image_resource(*rit);
    if (!brsc)
      brsc = new vil_blocked_image_facade(*rit);
    vil_cached_image_resource* cimr = new vil_cached_image_resource(brsc, 100);
    vil_image_resource_sptr ir = (vil_image_resource*)cimr;
    pyramid_level* level = new pyramid_level(ir);
    levels_.push_back(level);
  }
  //sort on image width
  vcl_sort(levels_.begin(), levels_.end(), level_compare);
  this->normalize_scales();
}

vil_pyramid_image_list::~vil_pyramid_image_list()
{
  unsigned nlevels = levels_.size();
  for (unsigned i = 0; i<nlevels; ++i)
    delete levels_[i];
}

//: Assumes that the image in level 0 is the largest
void vil_pyramid_image_list::normalize_scales()
{
  unsigned nlevels = levels_.size();
  if (nlevels==0)
    return;
  levels_[0]->scale_ = 1.0f;
  if (nlevels==1)
    return;
  float ni0 = static_cast<float>(levels_[0]->image_->ni());
  for (unsigned i = 1; i<nlevels; ++i)
    levels_[i]->scale_ = static_cast<float>(levels_[i]->image_->ni())/ni0;
}

bool vil_pyramid_image_list::is_same_size(vil_image_resource_sptr const& image)
{
  unsigned ni = image->ni(), nj = image->nj();
  for (unsigned L = 0; L<this->nlevels(); ++L)
    if (levels_[L]->image_->ni()==ni&&levels_[L]->image_->nj()==nj)
      return true;
  return false;
}

bool
vil_pyramid_image_list::add_resource(vil_image_resource_sptr const& image)
{
  if (this->is_same_size(image))
    return false;

  pyramid_level* level = new pyramid_level(image);
  levels_.push_back(level);

  //is this the first image added?
  if (levels_.size() == 1)
    return true;
  //sort the pyramid
  vcl_sort(levels_.begin(), levels_.end(), level_compare);
  //normalize the scales
  this->normalize_scales();
  return true;
}

//: Find an appropriate filename extension for the image.
// If the size of the image lies between existing scales then use
// the fractional amount in the name
float
vil_pyramid_image_list::find_next_level(vil_image_resource_sptr const& image)
{
  unsigned nlevels = this->nlevels();
  if (nlevels==0)
    return 0.0f;
  float base_ni = static_cast<float>(levels_[0]->image_->ni());
  return static_cast<float>(image->ni())/base_ni;
}

//: This method copies the resource into the pyramid.
// Use add_resource if the existing resource is to be just inserted into the level stack.
bool vil_pyramid_image_list::put_resource(vil_image_resource_sptr const& image)
{
  if (this->is_same_size(image))
    return false;
  float level = this->find_next_level(image);
  vcl_string copy_name = "copyR";
  vcl_string file = level_filename(directory_,copy_name, level);
  vcl_string ffmt = "pgm";
  if (image->file_format())
    ffmt = image->file_format();
  file = file +'.'+ ffmt;
  unsigned sbi = 0, sbj = 0;
  vil_blocked_image_resource_sptr bir = blocked_image_resource(image);
  if (bir)
  { sbi = bir->size_block_i(); sbj = bir->size_block_j(); }
  vil_image_resource_sptr copy;
  if (sbi==0||sbj==0)
  {
#ifdef VIL_USE_FSTREAM64
    vil_stream_fstream64* os = new vil_stream_fstream64(file.c_str(), "w");
#else //VIL_USE_FSTREAM64
    vil_stream_fstream* os = new vil_stream_fstream(file.c_str(), "w");
#endif //VIL_USE_FSTREAM64
    copy = vil_new_image_resource(os, image->ni(), image->nj(),
                                  image->nplanes(), image->pixel_format(),
                                  ffmt.c_str());
  }
  else
    copy = vil_new_blocked_image_resource(file.c_str(),
                                          image->ni(), image->nj(),
                                          image->nplanes(),
                                          image->pixel_format(),
                                          sbi, sbj,
                                          ffmt.c_str()).ptr();
  if (!vil_copy_deep(image, copy))
    return false;
  return this->add_resource(copy);
}

//:find the level closest to the specified scale
pyramid_level* vil_pyramid_image_list::closest(const float scale) const
{
  unsigned nlevels = levels_.size();
  if (nlevels == 0)
    return 0;

  if (nlevels == 1)
    return levels_[0];
  float mind = 1.0e08f;//huge scale;
  unsigned lmin = 0;
  for (unsigned i = 0; i<nlevels; ++i)
  {
    float ds = vcl_fabs(scale - levels_[i]->scale_);
    if (ds<mind)
    {
      mind = ds;
      lmin = i;
    }
  }
  pyramid_level* pl = levels_[lmin];
  if (pl)
    pl->cur_level_ = lmin;
  return pl;
}

vil_image_view_base_sptr
vil_pyramid_image_list::get_copy_view(unsigned i0, unsigned n_i,
                                      unsigned j0, unsigned n_j,
                                      unsigned level) const
{
  if (level>=this->nlevels())
  {
    vcl_cerr << "pyramid_image_list::get_copy_view(.) level = "
             << level << " max level = "
             << this->nlevels() -1 << '\n';
    return 0;
  }
  pyramid_level* pl = levels_[level];
  float actual_scale = pl->scale_;

  float fi0 = actual_scale*i0, fni = actual_scale*n_i, fj0 = actual_scale*j0, fnj = actual_scale*n_j;
  //transform image coordinates by actual scale
  unsigned si0 = static_cast<unsigned>(fi0);
  unsigned sni = static_cast<unsigned>(fni);
  if (sni == 0) sni = 1;//can't have less than one pixel
  unsigned sj0 = static_cast<unsigned>(fj0);
  unsigned snj = static_cast<unsigned>(fnj);
  if (snj == 0) snj = 1;//can't have less than one pixel
  vil_image_view_base_sptr v = pl->image_->get_copy_view(si0, sni, sj0, snj);
  if (!v)
  {
    vcl_cerr << "pyramid_image_list::get_copy_view(.) level = "
             << level << "(i0,j0):("
             << i0 << ' ' << j0 << ") (ni, nj):("
             << n_i << ' ' << n_j << ")\n"
             << "Get copy view from level image failed\n";
    return 0;
  }
  return v;
}

//:return a view with image scale that is closest to scale.
vil_image_view_base_sptr
vil_pyramid_image_list::get_copy_view(unsigned i0, unsigned n_i,
                                      unsigned j0, unsigned n_j,
                                      const float scale,
                                      float& actual_scale) const
{
  //find the resource that is closest to the specified scale
  pyramid_level* pl = this->closest(scale);
  if (!pl)
  {
    actual_scale = 0;
    return 0;
  }
  actual_scale = pl->scale_;
  unsigned level = pl->cur_level_;
  return this->get_copy_view(i0, n_i, j0, n_j, level);
}

