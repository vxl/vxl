// This is brl/bseg/sdet/sdet_watershed_region_proc.cxx
#include "sdet_watershed_region_proc.h"
//:
// \file
#include <vcl_algorithm.h>
#include <vnl/vnl_numeric_traits.h>
#include <vil1/vil1_memory_image_of.h>
#include <vsol/vsol_box_2d.h>
#include <brip/brip_vil1_float_ops.h>
#include <brip/brip_roi.h>
#include <brip/brip_watershed.h>
#include <bdgl/bdgl_region_algs.h>
#include <sdet/sdet_region.h>

//Gives a sort on region intensity in increasing order
static bool region_intensity_compare_increasing(sdet_region_sptr const r1,
                                                sdet_region_sptr const r2)
{
  return r2->Io() > r1->Io();
}

//Gives a sort on region intensity in decreasing order
static bool region_intensity_compare_decreasing(sdet_region_sptr const r1,
                                                sdet_region_sptr const r2)
{
  return r1->Io() > r2->Io();
}

//Gives a sort on region area in increasing order
static bool region_area_compare_increasing(sdet_region_sptr const r1,
                                           sdet_region_sptr const r2)
{
  return r2->Npix() > r1->Npix();
}

//Gives a sort on region area in decreasing order
static bool region_area_compare_decreasing(sdet_region_sptr const r1,
                                           sdet_region_sptr const r2)
{
  return r1->Npix() > r2->Npix();
}

//---------------------------------------------------------------
// Constructors
//  regions_ = temp2;

//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
//
sdet_watershed_region_proc::sdet_watershed_region_proc(sdet_watershed_region_proc_params& wrpp)
  : sdet_watershed_region_proc_params(wrpp)
{
  regions_valid_ = false;
  region_image_valid_ =false;
  boundaries_valid_ = false;
  min_label_ = 0;
  max_label_ = 0;
}

//:Default Destructor
sdet_watershed_region_proc::~sdet_watershed_region_proc()
{
  for (vcl_map<sdet_region_sptr, vcl_vector<sdet_region_sptr>* >::iterator
         rait = region_adjacency_.begin();
       rait != region_adjacency_.end(); rait++)
    delete (*rait).second;
}

//: if an roi is defined then create a chip image and define the bounds
void sdet_watershed_region_proc::clip_image()
{
  if (!image_||!roi_)
    return;
  vsol_box_2d_sptr box = roi_->region(0);
  if (! brip_vil1_float_ops::chip(image_, box, clip_))
    vcl_cout << "sdet_watershed_region_proc::chip_out_roi(.) - failed\n";
}

//-------------------------------------------------------------------------
//: Set the image to be processed
//
void sdet_watershed_region_proc::set_image(vil1_image const& image,
                                           vsol_box_2d_sptr const& box)
{
  if (!image)
  {
    vcl_cout <<"In sdet_watershed_region_proc::set_image(.) - null input\n";
    return;
  }

  image_ = brip_vil1_float_ops::convert_to_float(image);

  if (box)
  {
    roi_ = new brip_roi(image.width(), image.height());
    roi_->add_region(box);
    this->clip_image();
  }
  regions_valid_ = false;
  region_image_valid_ =false;
  boundaries_valid_ = false;
}


//----------------------------------------------------------------
//: scan the region label array to fill region pixel data
//
void sdet_watershed_region_proc::scan_region_data(vbl_array_2d<unsigned int> const & lab_array)
{
  int rows = lab_array.rows(), cols = lab_array.cols();
  int n_regions = max_label_ - min_label_ +1;
  //initialize the regions
  int imgc = 0, imgr = 0;
  for (int r = 0; r<rows; r++)
    for (int c = 0; c<cols; c++)
    {
      imgc = c; imgr = r;
      if (roi_)
      {
        imgc = roi_->ic(c);
        imgr = roi_->ir(r);
      }
      unsigned int lab = lab_array[r][c];
      if (lab<min_label_||lab>max_label_)
        continue;
      int index = lab-min_label_;
      //dangerous, might not be scaled properly (FIXME)
      unsigned short v =  (unsigned short) image_(imgc, imgr);
      regions_[index]->IncrementMeans(float(imgc), float(imgr), v);
    }
  //fill the region data
  for (int i = 0; i<n_regions; i++)
    regions_[i]->InitPixelArrays();

  for (int r = 0; r<rows; r++)
    for (int c = 0; c<cols; c++)
    {
      imgc = c; imgr = r;
      if (roi_)
      {
        imgc = roi_->ic(c);
        imgr = roi_->ir(r);
      }
      unsigned int lab = lab_array[r][c];
      if (lab<min_label_||lab>max_label_)
        continue;
      int index = lab-min_label_;
      //dangerous, might not be scaled properly (FIXME)
      unsigned short v = (unsigned short) image_(imgc, imgr);
      regions_[index]->InsertInPixelArrays(float(imgc), float(imgr), v);
      regions_[index]->ComputeIntensityStdev();
    }
}

bool sdet_watershed_region_proc::add_adjacency(sdet_region_sptr const& reg,
                                               sdet_region_sptr const& adj_reg)
{
  vcl_map<sdet_region_sptr, vcl_vector<sdet_region_sptr>* >::iterator rai;
  rai = region_adjacency_.find(reg);

  if (rai !=region_adjacency_.end())
  {
    vcl_vector<sdet_region_sptr> * vec = region_adjacency_[reg];

    for (unsigned int i =0 ; i < vec->size(); ++i)
      if ((*vec)[i] == adj_reg)
        return false; //adjacency relation already known

    vec->push_back(adj_reg);
  }
  else//make a new adjacent region array
  {
    vcl_vector<sdet_region_sptr>* adj_array = new vcl_vector<sdet_region_sptr>;
    adj_array->push_back(adj_reg);
    region_adjacency_[reg]=adj_array;
  }
  return true;
}

bool
sdet_watershed_region_proc::remove_adjacencies(sdet_region_sptr const& reg)
{
  vcl_map<sdet_region_sptr, vcl_vector<sdet_region_sptr>* >::iterator rai;
  rai = region_adjacency_.find(reg);
  if (rai ==region_adjacency_.end())
    return false;
  vcl_vector<sdet_region_sptr>* adjacencies = (*rai).second;
  region_adjacency_.erase(rai);
  delete adjacencies;
  return true;
}

bool sdet_watershed_region_proc::
adjacent_regions(sdet_region_sptr const& reg,
                 vcl_vector<sdet_region_sptr>& adj_regs)
{
  adj_regs.clear();
  vcl_map<sdet_region_sptr, vcl_vector<sdet_region_sptr>* >::iterator rai;
  rai = region_adjacency_.find(reg);

  if (rai !=region_adjacency_.end())
  {
    vcl_vector<sdet_region_sptr> * vec = region_adjacency_[reg];
    adj_regs = *vec;

    return true;
  }
  return false;
}

static bool found_in_regions(sdet_region_sptr const &r,
                             vcl_vector<sdet_region_sptr> const & regions)
{
  bool found = false;
  for (vcl_vector<sdet_region_sptr>::const_iterator rit = regions.begin();
       rit != regions.end()&& !found; rit++)
    if ((*rit).ptr() == r.ptr())
      found = true;
  return found;
}

static
void add_new_adjacent_regs(vcl_vector<sdet_region_sptr> const& new_adj_regs,
                           vcl_vector<sdet_region_sptr>& adj_regs)
{
  for (vcl_vector<sdet_region_sptr>::const_iterator rit = new_adj_regs.begin();
       rit != new_adj_regs.end(); rit++)
    if (!found_in_regions(*rit, adj_regs))
      adj_regs.push_back(*rit);
}

bool sdet_watershed_region_proc::merge_regions()
{
  //sort input on priority type
  switch (merge_priority_)
  {
   case DARK:
    vcl_sort(regions_.begin(),
             regions_.end(), region_intensity_compare_increasing);
    break;
   case LIGHT:
    vcl_sort(regions_.begin(),
             regions_.end(), region_intensity_compare_decreasing);
    break;
   case SMALL:
    vcl_sort(regions_.begin(),
             regions_.end(), region_area_compare_increasing);
    break;
   case BIG:
    vcl_sort(regions_.begin(),
             regions_.end(), region_area_compare_decreasing);
    break;
   default:
    ;      //don't sort at all
  }

  if (verbose_)
  {
    vcl_cout << "Regions before merge\n";
    this->print_region_info();
  }

  vcl_vector<sdet_region_sptr> new_regions;
  vcl_vector<sdet_region_sptr> merged_regions;
  for (vcl_vector<sdet_region_sptr>::iterator rit = regions_.begin();
       rit != regions_.end(); rit++)
  {
    sdet_region_sptr& r = *rit;
    if (found_in_regions(r, merged_regions))
      continue;
    sdet_region_sptr& rm = r; //merged region
    vcl_vector<sdet_region_sptr> adj_regs;
    if (!adjacent_regions(r, adj_regs))
      continue;
    bool merged = false;
    vcl_vector<sdet_region_sptr> temp_adj;
    for (vcl_vector<sdet_region_sptr>::iterator arit = adj_regs.begin();
         arit != adj_regs.end(); arit++)
    {
      sdet_region_sptr& ar = *arit;
      if (found_in_regions(ar, merged_regions))
        continue;
      if (r->Npix()<min_area_||ar->Npix()<min_area_)
        continue;
#ifdef DEBUG
      vcl_cout << "R[" << r->label() <<"]:vs:[" << ar->label() << "]\n";
#endif
      if (bdgl_region_algs::
          earth_mover_distance(r->cast_to_digital_region(),
                              ar->cast_to_digital_region()) > merge_tol_)
        {
          //will potentially be adjacent to the new region
          temp_adj.push_back(ar);
          continue;
        }
      //found two mergeable regions
      vdgl_digital_region_sptr temp;
      if (bdgl_region_algs::merge(rm->cast_to_digital_region(),
                                  ar->cast_to_digital_region(), temp))
      {
        rm = new sdet_region(*temp);
        merged = true;
        merged_regions.push_back(ar);
        //now we must add the regions that are adjacent to ar
        //since they are adjacent to the new region
        vcl_vector<sdet_region_sptr> new_adj_regs;
        if (adjacent_regions(ar, new_adj_regs))
          add_new_adjacent_regs(new_adj_regs, temp_adj);
      }
    }
    //declare a new region and update region adjacency map
    if (merged)
    {
      merged_regions.push_back(r);
      new_regions.push_back(rm);
      this->remove_adjacencies(r);
      rm->set_label(++max_label_);
#ifdef DEBUG
      vcl_cout << "New Region[" << rm->label() << "](" << rm->Npix()
               << ")(Xo: " << rm->Xo() << " Yo: " << rm ->Yo()
               << " Io: " << rm ->Io() <<')' << vcl_endl;
#endif
      for (vcl_vector<sdet_region_sptr>::iterator arit = temp_adj.begin();
           arit != temp_adj.end(); arit++)
        this->add_adjacency(rm, *arit);
    }
  }
  if (verbose_)
  {
    vcl_cout << "# merged regions " << merged_regions.size() << '\n'
             << "# new regions " << new_regions.size() << '\n';
  }
  //remove merged regions from the region_index
  vcl_vector<sdet_region_sptr> temp2;
  for (vcl_vector<sdet_region_sptr>::iterator rit = regions_.begin();
       rit != regions_.end(); rit++)
    if (!found_in_regions(*rit, merged_regions))
      temp2.push_back(*rit);
  //add new regions to region index
  for (vcl_vector<sdet_region_sptr>::iterator rit = new_regions.begin();
       rit != new_regions.end(); rit++)
    temp2.push_back(*rit);
  //replace the index
  regions_ = temp2;
  if (verbose_)
  {
    vcl_cout << "Regions after merge\n";
    this->print_region_info();
  }
  boundaries_valid_ = false;
  return true;
}

//--------------------------------------------------------------------------
//: extract a set of digital regions
bool sdet_watershed_region_proc::extract_regions()
{
  if (regions_valid_)
    return true;

  // Check the image
  if (!image_)
  {
    vcl_cout << "In sdet_watershed_region_proc::extract_regions() - no image\n";
    return false;
  }

  vcl_cout << "sdet_watershed_region_proc::extract_regions(): width = "
           << image_.width() << " height = " << image_.height() << vcl_endl;

  //Process the image to extract regions
  //for now we assume the image is unsigned char or unsigned short
  regions_.clear();

  brip_watershed watershed(wp_);
  if (roi_)
    watershed.set_image(clip_);
  else
    watershed.set_image(image_);
  if (!watershed.compute_regions())
    return false;
  overlay_image_ = watershed.overlay_image();
  vil1_memory_image_of<vil1_rgb<unsigned char> > color_image =
    brip_vil1_float_ops::convert_to_rgb(image_, 0, 255);
  if (roi_)
    overlay_image_ =
      brip_vil1_float_ops::insert_chip_in_image(color_image, overlay_image_, roi_);


  vbl_array_2d<unsigned int>& lab_array = watershed.region_label_array();
  min_label_ = watershed.min_region_label();
  max_label_ = watershed.max_region_label();
  int n_labs = max_label_ - min_label_ + 1;//number of labels

  //initialize the region array
  regions_.resize(n_labs);
  for (int n = 0; n<n_labs; n++)
  {
    regions_[n] = new sdet_region();
    regions_[n]->set_label(n+min_label_);
  }
  //initialize region adjacency
  vcl_vector<unsigned int> adj_regs;
  for (int n = 0; n<n_labs; n++)
  {
    unsigned int ln = min_label_ + n;
    sdet_region_sptr rn = regions_[n];
    if (watershed.adjacent_regions(ln, adj_regs))
      for (unsigned int i = 0; i<adj_regs.size(); ++i)
        this->add_adjacency(rn, regions_[adj_regs[i]-min_label_]);
  }

  //fill the region intensity data
  this->scan_region_data(lab_array);

  //sort the regions on intensity (not really needed but good for debugging)
  vcl_sort(regions_.begin(),
           regions_.end(), region_intensity_compare_increasing);
  regions_valid_ = true;
  boundaries_valid_ = false;
  return true;
}

//----------------------------------------------------------
//: Clear internal storage
//
void sdet_watershed_region_proc::clear()
{
  regions_.clear();
}

//--------------------------------------------------------------------------
//: Use a linear approximation to intensity to predict region data.
//  Return the residual error
vil1_image sdet_watershed_region_proc::get_residual_image()
{
  if (!image_||!regions_valid_)
  {
    vcl_cout << "In sdet_watershed_region_proc::get_residual_image() - no regions\n";
    return 0;
  }
  int xsize = image_.width(), ysize = image_.height();
  vil1_memory_image_of<unsigned char> res_image(xsize, ysize);
  res_image.fill(0);
  float min_res = (float)vnl_numeric_traits<unsigned short>::maxval;
  for (vcl_vector<sdet_region_sptr>::iterator rit = regions_.begin();
       rit != regions_.end(); rit++)
    for ((*rit)->reset(); (*rit)->next();)
    {
      float res = (*rit)->Ir();
      if (res<min_res)
        min_res = res;
    }

  for (vcl_vector<sdet_region_sptr>::iterator rit = regions_.begin();
       rit != regions_.end(); rit++)
    for ((*rit)->reset(); (*rit)->next();)
    {
      int x = int((*rit)->X()), y = int((*rit)->Y());
      float res = (*rit)->Ir();
      float is = res-min_res;//to ensure non-negative
      if (is>255)
        is = 255;//to ensure within char
      unsigned char pix = (unsigned char)is;
      res_image(x, y)=pix;
    }
  return res_image;
}

void sdet_watershed_region_proc::print_region_info()
{
  if (!regions_valid_)
    return;
  int index = 0;
  for (vcl_vector<sdet_region_sptr>::iterator rit = regions_.begin();
       rit != regions_.end(); rit++, index++)
  {
    int npix = (*rit)->Npix();
    if (npix<min_area_)
      continue;
    vcl_cout << "R[" << (*rit)->label() << "](Np:" << npix << " Io:"
             << (*rit)->Io() << " Xo:" << (*rit)->Xo()
             << " Yo:" << (*rit)->Yo() << "):sd"<< (*rit)->Io_sd() << vcl_endl;
  }
}

bool sdet_watershed_region_proc::compute_region_image()
{
  if (!regions_valid_||!image_)
    return false;
  //create the image assume for now everything is unsigned char
  int w = image_.width(), h = image_.height();
  region_image_.resize(w, h);
  region_image_.fill(255);
  for (vcl_vector<sdet_region_sptr>::iterator rit =  regions_.begin();
       rit != regions_.end(); rit++)
  {
    for ((*rit)->reset(); (*rit)->next();)
    {
      int c = (int)(*rit)->X(), r = (int)(*rit)->Y();
      if (c<0||r<0||c>w-1||r>h-1)
        continue;
      unsigned char val = (unsigned char)(*rit)->I();
      region_image_(c,r) = val;
    }
  }
  region_image_valid_ = true;
  return true;
}

vil1_image sdet_watershed_region_proc::region_image()
{
  vil1_image out;
  if (!region_image_valid_)
    if (!this->compute_region_image())
      return out;
  return region_image_;
}

void sdet_watershed_region_proc::compute_boundaries()
{
  for (vcl_vector<sdet_region_sptr>::iterator rit =  regions_.begin();
       rit != regions_.end(); rit++)
  {
    if ((*rit)->Npix()<min_area_)
      continue;
    if ((*rit)->boundary())
      boundaries_.push_back((*rit)->boundary());
  }
  boundaries_valid_ = true;
}

vcl_vector<vsol_polygon_2d_sptr> sdet_watershed_region_proc::boundaries()
{
  if (!boundaries_valid_)
    this->compute_boundaries();
  return boundaries_;
}
