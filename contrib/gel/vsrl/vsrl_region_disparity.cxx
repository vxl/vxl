//
// vsrl_region_disparity.cxx
//
// Routines to calculate disparity based on regions
// rather than pixels.
//
// G.W. Brooksby
// 09/13/03, 09/17/03
// Written at ORD on two LONG layovers...

#include <vsrl/vsrl_region_disparity.h>
#include <vcl_cstdlib.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vsrl/vsrl_stereo_dense_matcher.h>
#include <sdet/sdet_region_proc_params.h>
#include <sdet/sdet_region_proc.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_intensity_face.h>
#include <vepl/vepl_gradient_mag.h>
#include <vepl/vepl_gaussian_convolution.h>
#include <vsrl/vsrl_results_dense_matcher.h>

extern "C"
{
#include <jseg/jseg.h>
}

vsrl_region_disparity::vsrl_region_disparity()
{
  l_img_ = NULL;
  r_img_ = NULL;
  d_img_ = NULL;
  this->init();
}

vsrl_region_disparity::vsrl_region_disparity(vil1_image* li, vil1_image* ri)
{
  l_img_ = li;
  r_img_ = ri;
  this->init();
}

vsrl_region_disparity::~vsrl_region_disparity()
{
  delete reg_disp_img_;
}

void* vsrl_region_disparity::init()
{
  vcl_cout << "vsrl_region_disparity::init()" << vcl_endl;
  if_regions_ = NULL;
  digi_regions_ = NULL;
  reg_disp_img_ = NULL;
  region_type_ = UNDEFINED;
  return 0;
}

void* vsrl_region_disparity::SetRegions(vcl_vector<vtol_intensity_face_sptr>* regs)
{
  vcl_cout << "vsrl_region_disparity::SetRegions(intensity faces)" << vcl_endl;
  if_regions_ = regs;
  region_type_ = INTENSITY_FACE;
  return 0;
}

void* vsrl_region_disparity::SetRegions(vcl_vector<vdgl_digital_region*>* regs)
{
  vcl_cout << "vsrl_region_disparity::SetRegions(digital regions)" << vcl_endl;
  digi_regions_ = regs;
  region_type_ = DIGITAL_REGION;
  return 0;
}

bool vsrl_region_disparity::Execute()
{
  bool result = false;
  switch (region_type_)
    {
    case INTENSITY_FACE:
      {
        vcl_cout << "vsrl_region_disparity::Execute: ";
        vcl_cout << "region type: INTENSITY_FACE" << vcl_endl;
        result = run_intensity_faces();
        break;
      }
    case DIGITAL_REGION:
      {
        vcl_cout << "vsrl_region_disparity::Execute: ";
        vcl_cout << "region type: DIGITAL_REGION" << vcl_endl;
        result = run_digital_regions();
        break;
      }
    case UNDEFINED:
    default:
      {
        vcl_cerr << "vsrl_region_disparity::Execute(): ";
        vcl_cerr << "Error: region type is UNDEFINED." << vcl_endl;
        result = false;
        break;
      }
    }
  return result;
}

bool vsrl_region_disparity::run_intensity_faces()
{
  // Remember, Intensity Faces contain digital regions.
  // Make sure intensity faces were established before proceeding.
  bool result = false;
  vcl_cerr << "vsrl_region_disparity::run_intesity_faces..." << vcl_endl;
  if (if_regions_ == NULL) {
    vcl_cerr << "vsrl_region_disparity::run_intesity_faces: Error:" << vcl_endl;
    vcl_cerr << "No intensity faces present." << vcl_endl;
    return false;
  }

  // overwrite anything that may exist.  If digi_regions_ doesn't start from
  // scratch, it may have garbage in it and it may produce garbage.
  if (digi_regions_ != NULL) {
    delete digi_regions_;
    digi_regions_ = new vcl_vector<vdgl_digital_region*>;
  }

  for (vcl_vector<vtol_intensity_face_sptr>::iterator fit = if_regions_->begin();
       fit != if_regions_->end(); fit++) {
    vdgl_digital_region* reg = (*fit)->cast_to_digital_region();
    digi_regions_->push_back(reg);
  }
  // Now we have a vector of digital regions, pass it off...

  this->run_digital_regions();

  return result;
}

bool vsrl_region_disparity::run_digital_regions()
{
  vcl_cerr << "vsrl_region_disparity::run_digital_regions..." << vcl_endl;
  // Error checking
  if ( digi_regions_ == NULL) {
    vcl_cerr << "vsrl_region_disparity::run_digital_regions: Error:" << vcl_endl;
    vcl_cerr << "No digital regions present." << vcl_endl;
    return false;
  }

  // Up-front prep.
  // create the new disparity image in memory...
  if ( reg_disp_img_ != NULL) delete reg_disp_img_; // Clear out the old...
  reg_disp_img_ = new vil1_memory_image_of<double>(d_img_->width(),d_img_->height());  // Prepare for the new...
  for (int c=0; c<reg_disp_img_->width(); c++) {
    for (int r=0; r<reg_disp_img_->height(); r++) {
      (*reg_disp_img_)(c,r)=0.0; // Start with a zero'd buffer
    }
  }
  
  // iterate through all the regions
  vcl_vector<vdgl_digital_region*>::iterator rit;
  for (rit = (*digi_regions_).begin(); rit != (*digi_regions_).end(); rit++) {
    // For each region, get the pixel coordinates
    double avg=0;
    int npix=0;
    float x,y;
    for ( (*rit)->reset(); (*rit)->next();) {
      x = (*rit)->X(); y = (*rit)->Y(); // get coordinates of this pixel
      // Get the disparity from the disparity image for the corresponding pixels.
      // Average the disparities across the entire region.
      avg += (int)(*d_img_)(x,y);
      npix++;
    }
    avg /= npix;  // Final division to get the average

    vcl_cout << "Region average disparity: " << avg << " Number of Pixels: " << npix << vcl_endl;

    // Now that we have the average for the region, insert
    // it into every pixel of that region in the disparity image
    for ( (*rit)->reset(); (*rit)->next();) {
      x = (*rit)->X(); y = (*rit)->Y(); // get coordinates of this pixel
      // Insert the average disparity into a new disparity image.
      (*reg_disp_img_)(x,y) = avg;
    }
  }
  return true;
}

