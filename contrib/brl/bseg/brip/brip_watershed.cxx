#include <iostream>
#include <cmath>
#include "brip_watershed.h"
//:
// \file
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_numeric_traits.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>
#include <vil1/vil1_rgb.h>
#include "brip_vil1_float_ops.h"

//Define 8-connected neighbors
static int n_col[8]={-1, 0, 1,-1,1,-1,0,1};
static int n_row[8]={-1,-1,-1, 0,0, 1,1,1};

brip_watershed::brip_watershed(brip_watershed_params const& bwp)
  : brip_watershed_params(bwp)
{
  max_region_label_ = 1;
}

brip_watershed::~brip_watershed()
{
  for (auto & mit : region_adjacency_)
    delete mit.second;
}

//: Print the region label array
void brip_watershed::print_region_array()
{
  std::cout << "\n\n";
  int rows = region_label_array_.rows(), cols = region_label_array_.cols();
  for (int r = 0; r<rows; r++)
  {
    for (int c = 0; c<cols; c++)
      std::cout << region_label_array_[r][c] << ' ';
    std::cout << '\n';
  }
  std::cout << std::endl << std::endl;
}

void brip_watershed::print_neighborhood(int col, int row, unsigned int lab)
{
  int rows = region_label_array_.rows(), cols = region_label_array_.cols();
  int k = 0, m = 0;
  for (int n = 0; n<8; n++, k++, m++)
  {
    int rn = row + n_row[n];
    int cn = col + n_col[n];
    if (rn<0||rn>=rows||cn<0||cn>=cols)
    {
      std::cout << "Neighborhood out of range\n";
      return;
    }
    if (k>2)
    {
      std::cout << '\n';
      k = 0;
    }
    if (m == 4)
    {
      std::cout << lab << ' ' << region_label_array_[rn][cn];
      k++;
    }
    else
      std::cout << region_label_array_[rn][cn] << ' ';
  }
  std::cout << std::endl;
}

void brip_watershed::print_adjacency_map()
{
  for (auto & mit : region_adjacency_)
  {
    unsigned int reg = mit.first;
    std::vector<unsigned int>* adj_regs = mit.second;
    std::cout << "R[" << reg << "]:(";
    if (adj_regs)
      for (unsigned int & adj_reg : *adj_regs)
        std::cout << adj_reg << ' ';
    std::cout << ")\n";
  }
}

void brip_watershed::set_image(vil1_memory_image_of<float> const& image)
{
  image_ = image;
  int w = image_.width(), h = image_.height();
  vil1_memory_image_of<float> Ix(w, h), Iy(w, h);
  vil1_memory_image_of<float> gauss =
    brip_vil1_float_ops::gaussian(image_, sigma_);
  brip_vil1_float_ops::gradient_3x3(gauss, Ix, Iy);
  gradient_mag_image_.resize(w,h);
  for (int r = 0; r<h; r++)
  {
    for (int c = 0; c<w; c++)
    {
      float gx = Ix(c,r), gy = Iy(c,r);
      gradient_mag_image_(c,r) = std::sqrt(gx*gx+gy*gy);
   // std::cout << gradient_mag_image_(c,r) << ' ';
    }
 // std::cout << std::endl;
  }
  region_label_array_.resize(h,w);
  region_label_array_.fill(UNLABELED);
}

//==============================================================
//: Find points of local minima in gradient magnitude.
//  Label these points as seeds.
bool brip_watershed::compute_seeds()
{
  int w = gradient_mag_image_.width(), h = gradient_mag_image_.height();
  max_region_label_ = 1;
  for (int r = 2; r<h-2; r++)
    for (int c = 2;c<w-2;c++)
    {
      float min_grad = vnl_numeric_traits<float>::maxval;
      float max_grad = -min_grad;
      int min_i = 0, min_j=0;
      for (int i = -1;i<=1; i++)
        for (int j = -1;j<=1; j++)
        {
          float g = gradient_mag_image_(c+j, r+i);
          max_grad = std::max(max_grad, g);
          if (g<min_grad)
          {
            min_grad = g;
            min_i = i;
            min_j = j;
          }
        }
      float diff = max_grad-min_grad;
      //a seed is defined when the minimum is at the center of
      //local 3x3 neighborhood and gradient diff is greater than
      //a threshold.
      if (!min_i&&!min_j&&diff>thresh_)
      {
        max_region_label_++;
        region_label_array_[r][c] = max_region_label_;
        if (verbose_)
        {
          std::cout << "\nS(c:" << c << " r:" << r << ')' << std::endl;
          this->print_neighborhood(c, r, max_region_label_);
        }
      }
    }
  return max_region_label_ != 0;
}

//==============================================================
//: Initialize the priority queue from the seeds give each region a new label
bool brip_watershed::initialize_queue()
{
  int w = gradient_mag_image_.width(), h = gradient_mag_image_.height();
  for (int r = 2; r<h-2; r++)
    for (int c = 2;c<w-2;c++)
      for (int n = 0;n<8; n++)
      {
        int cn = c+n_col[n], rn = r+n_row[n];
        unsigned int lab = region_label_array_[rn][cn];
        if (lab>BOUNDARY)
          //add a new region pixel to the priority queue
        {
          vgl_point_2d<int> location(c,r);
          vgl_point_2d<int> nearest(cn,rn);
          float cost = gradient_mag_image_(c, r);
          brip_region_pixel_sptr pix =
            new brip_region_pixel(location, nearest, cost, 0, lab);
          priority_queue_.push(pix);
        }
      }
  //  std::cout << *(priority_queue_.top()) << std::endl;
  return priority_queue_.size()>0;
}

//==============================================================
//: Process the priority queue and grow regions
bool brip_watershed::grow_regions()
{
  int n_boundary_pix=0, n_region_pix = 0;
  int rs = region_label_array_.rows(), cs = region_label_array_.cols();
  while (priority_queue_.size() != 0)
  {
    //get the lowest cost pixel
    brip_region_pixel_sptr pix = priority_queue_.top();
    if (!pix)
      return false;
    priority_queue_.pop();

    unsigned int lab = pix->label_;
    if (lab <= BOUNDARY)
      continue;
    vgl_point_2d<int> location = pix->location_;
    vgl_point_2d<int> nearest =  pix->nearest_;
    if (verbose_)
    {
      std::cout << "\nN(c:" << location.x() << " r:" << location.y() << ")\n";
      this->print_neighborhood(location.x(), location.y(), lab);
    }
    for (int n = 0; n<8; n++)
    {
      //location of neighboring pixel
      int rn = location.y()+n_row[n];
      int cn = location.x()+n_col[n];
      if (rn<1||cn<1||rn>rs-2||cn>cs-2)
        continue;
      unsigned int n_lab = region_label_array_[rn][cn];

      //the neighbor is labeled but not my label then I become
      //a boundary point
      if (n_lab>BOUNDARY&&n_lab!=lab)
      {
        //note that the regions are adjacent
        this->add_adjacency(lab, n_lab);
        this->add_adjacency(n_lab, lab);
        lab = BOUNDARY;
        n_boundary_pix++;
        break;
      }
    }
    pix->label_=lab;
    region_label_array_[location.y()][location.x()]=lab;
    //If we didn't detect a boundary then add new region pixels
    if (lab>BOUNDARY)
      for (int n = 0; n<8; n++)
      {
        //location of neighboring pixel
        int rn = location.y()+n_row[n];
        int cn = location.x()+n_col[n];
        if (rn<1||cn<1||rn>rs-2||cn>cs-2)
          continue;
        unsigned int n_lab = region_label_array_[rn][cn];
        if (n_lab != UNLABELED)
          continue;
        //attributes of unlabeled neighboring pixel
        vgl_point_2d<int> n_location(cn, rn);
        float cost = gradient_mag_image_(cn, rn);
        brip_region_pixel_sptr pix =
          new brip_region_pixel(n_location, nearest, cost, 0, lab);
        priority_queue_.push(pix);
        //mark the label array
        region_label_array_[rn][cn] = lab;
        n_region_pix++;
      }
    if (verbose_)
    {
      std::cout << '\n';
      this->print_neighborhood(location.x(), location.y(), lab);
    }
  }
  if (verbose_)
    std::cout << "Found " << n_boundary_pix << " boundary pixels and "
             << n_region_pix << " region pixels\n";
  return n_region_pix>0||n_boundary_pix>0;
}

//:add a region adjacency relation.  Returns false if relation is already known.
bool brip_watershed::add_adjacency(const unsigned int reg,
                                   const unsigned int adj_reg)
{
  std::map<unsigned int, std::vector<unsigned int>* >::iterator adi;
  adi = region_adjacency_.find(reg);

  if (adi !=region_adjacency_.end())
  {
    std::vector<unsigned int> * vec = region_adjacency_[reg];

    for (unsigned int i : *vec)
      if (i == adj_reg)
        return false; //adjacency relation already known

    vec->push_back(adj_reg);
  }
  else//make a new adjacent region array
  {
    auto* adj_array = new std::vector<unsigned int>;
    adj_array->push_back(adj_reg);
    region_adjacency_[reg]=adj_array;
  }
  return true;
}

bool brip_watershed::adjacent_regions(const unsigned int reg,
                                      std::vector<unsigned int>& adj_regs)
{
  adj_regs.clear();
  std::map<unsigned int, std::vector<unsigned int>* >::iterator adi;
  adi = region_adjacency_.find(reg);

  if (adi !=region_adjacency_.end())
  {
    adj_regs = *(region_adjacency_[reg]);
    return true;
  }
  else
    return false;
}

bool brip_watershed::compute_regions()
{
  if (!compute_seeds())
  {
    std::cout << "In brip_watershed::compute_regions() - no seeds\n";
    return false;
  }

  if (!initialize_queue())
  {
    std::cout << "In brip_watershed::compute_regions() - queue initialization failed\n";
    return false;
  }
  if (!grow_regions())
  {
    std::cout << "In brip_watershed::grow_regions() - failed\n";
    return false;
  }

  return true;
}

//compute a color image with the original monochrome image and green
//region boundary overlay
vil1_image brip_watershed::overlay_image()
{
  // convert the float image
  vil1_memory_image_of<unsigned char> cimage =
    brip_vil1_float_ops::convert_to_byte(image_);
  //create a rgb image
  int w = cimage.width(), h = cimage.height();
  vil1_memory_image_of<vil1_rgb<unsigned char> > overlay(w,h);
  for (int row = 0; row<h; row++)
    for (int col = 0; col<w; col++)
      if (region_label_array_[row][col]>BOUNDARY)
      {
        overlay(col,row).r = cimage(col,row);
        overlay(col,row).g = cimage(col,row);
        overlay(col,row).b = cimage(col,row);
      }
      else
      {
        overlay(col,row).r = 0;
        overlay(col,row).g = 255;
        overlay(col,row).b = 0;
      }
  return overlay;
}
