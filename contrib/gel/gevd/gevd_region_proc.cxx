// This is gel/gevd/gevd_region_proc.cxx
#include "gevd_region_proc.h"
//:
// \file
// See gevd_region_proc.h for documentation
#include <gevd/gevd_float_operators.h>
#include <gevd/gevd_detector.h>
#include <gevd/gevd_clean_edgels.h>
#include <gevd/gevd_edgel_regions.h>
#include <vtol/vtol_intensity_face.h>

//---------------------------------------------------------------
// Constructors
//
//----------------------------------------------------------------

//: constructor from a parameter block (the only way)
//
gevd_region_proc::gevd_region_proc()
{
  debug_=false;
  //debug_data_ = 0;
  //roi_proc_ = 0;
  buf_ = 0;
}

//:Default Destructor
gevd_region_proc::~gevd_region_proc()
{
  delete buf_;
}

//---------------------------------------------------------
//: Extract the region of interest from image_ as a BufferXY.
//
gevd_bufferxy* gevd_region_proc::get_image_buffer(vil1_image& image)
{
  if (!image)
    return NULL;
  if (image.planes() != 1)
    return NULL;

  int wd = image.width();
  int ht = image.height();
  int sz = image.components() * image.bits_per_component(); // bits per pixel
  gevd_bufferxy* buf = new gevd_bufferxy(wd, ht, sz);
  image.get_section(buf->GetBuffer(), 0, 0, wd, ht);
  return buf;
}

//---------------------------------------------------------
//: Convert buf to a floating point buffer
//
gevd_bufferxy*  gevd_region_proc::get_float_buffer(gevd_bufferxy* buf)
{
  if (!buf)
    return NULL;
  gevd_bufferxy* fbuf =  new gevd_bufferxy(buf->GetSizeX(),
                                           buf->GetSizeY(),
                                           8*sizeof(float));
  gevd_float_operators::BufferToFloat(*buf, *fbuf);
  return fbuf;
}


//---------------------------------------------------------
// convert a float buffer back to 16 bits/pixel
//
gevd_bufferxy* gevd_region_proc::put_float_buffer(gevd_bufferxy* fbuf)
{
  if (!fbuf)
    return NULL;

  gevd_bufferxy* pbuf = new gevd_bufferxy(fbuf->GetSizeX(), fbuf->GetSizeY(), 16);
  gevd_float_operators::FloatToBuffer(*fbuf, *pbuf);
  return pbuf;
}
//-------------------------------------------------------------------------
//: Set the image to be processed
//
void gevd_region_proc::set_image(vil1_image& image)
{
  if (!image)
    {
      vcl_cout <<"In gevd_region_proc::set_image(.) - null input\n";
      return;
    }
  regions_valid_ = false;
  image_ = image;
  if (buf_)
    {
      delete buf_;
      buf_ = NULL;
    }
  //Expand by expand_scale_
  if (expand_scale_==1.0f)
    {
      buf_ = this->get_image_buffer(image_);
      return;
    }
  else if (expand_scale_==2.0f)
    {
      gevd_bufferxy* temp = this->get_image_buffer(image_);
      if (!temp)
        {
          vcl_cout <<"In gevd_region_proc::set_image(.) "
                   <<"- couldn't get gevd_bufferxy from the image\n";
          return;
        }
      gevd_bufferxy* fbuf = this->get_float_buffer(temp);
      gevd_bufferxy* expand = NULL;
      gevd_float_operators::ExpandBy2(*fbuf, expand, burt_adelson_factor_);
      gevd_float_operators::TruncateToPositive(*expand);
      buf_ = this->put_float_buffer(expand);
      delete temp;
      delete fbuf;
      delete expand;
      return;
    }
  else if (expand_scale_==0.5f)
    {
      gevd_bufferxy* temp = this->get_image_buffer(image_);
      if (!temp)
        {
          vcl_cout <<"In gevd_region_proc::set_image(.) "
                   <<"- couldn't get gevd_bufferxy from the image\n";
          return;
        }
      gevd_bufferxy* fbuf = this->get_float_buffer(temp);
      gevd_bufferxy* shrink = NULL;
      gevd_float_operators::ShrinkBy2(*fbuf, shrink, burt_adelson_factor_);
      gevd_float_operators::TruncateToPositive(*shrink);
      buf_ = this->put_float_buffer(shrink);
      delete temp;
      delete fbuf;
      delete shrink;
      return;
    }
  vcl_cout <<"In gevd_region_proc::set_image(.) - invalid expand scale factor\n";
}

//--------------------------------------------------------------------------
//: extract a set of vdgl_poly_intensity_face(s)
void gevd_region_proc::extract_regions()
{
  if (regions_valid_)
    return;

  // Check the image
  if (!buf_)
    {
      vcl_cout << "In gevd_region_proc::extract_regions() - no image\n";
      return;
    }

  vcl_cout << "gevd_region_proc::extract_regions(): sizeX = "
           << buf_->GetSizeX() << " sizeY = " << buf_->GetSizeY() << vcl_endl;

  //Process the image to extract regions
  regions_.clear();

  // -tpk- need to pass along the scaled image rather than the orignal
  gevd_detector detector ( image_);

  //could move this all back into detector_params
  detector.junctionFactor = 1.0;
  detector.contourFactor = 2.0;
  detector.minLength = 4;
  detector.junctionp = 1;
  detector.smooth = 2.0;
  detector.borderp = 1;

  detector.DoContour();

  vcl_vector<vtol_edge_2d_sptr> * edgels = detector.GetEdges();

  if (!edgels->size())
    {
      vcl_cout << "In gevd_region_proc::extract_regions()- No Edgels were computed\n";
      return;
    }
#if 0 // commented out
  vcl_vector<vtol_edge_2d_sptr>::iterator eit; = edgels.begin();
  for (eit = edgels->begin(); eit != edgels->end(); eit++)
    {
      vcl_cout << "Edgel output from DoContour:";
      (*eit)->describe(vcl_cout,2);
    }

  gevd_detector det(dp_);
  vcl_vector<vtol_edge_2d_sptr> broken_edgels;
  det.DoBreakCorners(edgels, broken_edgels);
#endif

  gevd_clean_edgels cl;
  vcl_vector<vtol_edge_2d_sptr> clean_edgels;
  cl.DoCleanEdgelChains(*edgels, clean_edgels);
  if (!clean_edgels.size())
    {
      vcl_cout << "In gevd_region_proc::extract_regions()- All edges removed by clean\n";
      return;
    }
  gevd_edgel_regions er(debug_);
  er.set_magnification(expand_scale_);
  //if (verbose_)
  //  er.SetVerbose();

  vcl_vector<vtol_intensity_face_sptr> faces;
  //float xo = roi_proc_->get_xo(), yo = roi_proc_->get_yo();
  er.compute_edgel_regions(buf_, clean_edgels, faces);
#if 0 // commented out
  // Transform the extracted region boundaries if necessary
  if (expand_scale_!=0.0f)
    {
      float si = 1.0f/expand_scale_;
      //We use a TwoChain to provide a superior to the set of Faces
      //so that the standard ::TaggedTransform can be used to
      //transform IntensityFace(expand_scale_) segmented at an expanded scale.
      TwoChain_ref tc = new TwoChain(faces.length());
      for (vcl_vector<IntensityFace*>::iterator fit = faces.begin();
          fit != faces.end(); fit++)
        tc->add_face((Face*)(*fit), '1');
      //Coordinates are defined at the center of a pixel
      CoolTransform minus=CoolTransform::translation(-0.5, -0.5, 0.0);
      CoolTransform plus=CoolTransform::translation(0.5+xo, 0.5+yo, 0.0);
      CoolTransform scale = CoolTransform::stretching(si, si, 1.0);
      CoolTransform t = CoolTransform::identity();
      //Concatenate the transforms
      t *= minus;
      t *= scale;
      t *= plus;
      tc->TaggedTransform(t);
    }
#endif
  //Copy the faces to a vector
  vcl_vector<vtol_intensity_face_sptr>::iterator fit;
  for ( fit = faces.begin(); fit != faces.end(); fit++)
    {
      //joe mod
      //vtol_intensity_face_ref ifr = (*fit);
      //      if (!roi_proc_->inside_roi_mask(ifr))
      //if (!roi_proc_->inside_process_rois(ifr))
        continue; // hence this for loop is void! - PVr
      //vtol_intensity_face_sptr intf = (*fit);
      //vcl_vector<OneChain*>* chains = intf->OneChains();
      //vdgl_digital_region * dr = intf->cast_to_digital_region();
      //vdgl_poly_intensity_face_ref rf = new vdgl_poly_intensity_face(chains, *dr);
      //regions_.push_back(dr);
      //delete chains;
      //end joe mod
    }
  regions_valid_ = true;
}
//----------------------------------------------------------
//: Clear internal storage
//
void gevd_region_proc::clear()
{
  regions_.clear();
}
