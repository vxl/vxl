//--*-c++-*--
// <begin copyright notice>
// <end copyright notice>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>
#include <gevd/gevd_bufferxy.h>
#include <vil/vil_image.h>
#include <gevd/gevd_pixel.h>
#include <gevd/gevd_float_operators.h>
//#include <SpatialBasics/tolerance.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_digital_region.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_face.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_face_sptr.h>
#include <vtol/vtol_two_chain.h>
#include <gevd/gevd_detector.h>
#include <vdgl/vdgl_intensity_face.h>
//#include <vdgl_poly_intensity_face.h>
#include <gevd/gevd_clean_edgels.h>
//#include <CAD_Detection/lung_roi_proc.h>
#include <gevd/gevd_edgel_regions.h>
#include <gevd/gevd_clean_edgels.h>
#include <gevd/gevd_region_proc.h>




#define ushortPixel(buf,x,y)  (*((unsigned short*)buf->GetElementAddr(x,y)))
//---------------------------------------------------------------
// -- Constructors 
//
//----------------------------------------------------------------

//: -- constructor from a parameter block (the only way)
//    
//    
region_proc::region_proc()
{
  _debug=false;
  //_debug_data = 0;
  //_roi_proc = 0;
  _buf = 0;
}

//:Default Destructor
region_proc::~region_proc()
{
  delete _buf;
}

//---------------------------------------------------------
//:  -- Extract the region of interest from _image as a BufferXY.
//
gevd_bufferxy* region_proc::get_image_buffer(vil_image& image)
{
  if(!image)
    return NULL;

  return new gevd_bufferxy(image);
}

//---------------------------------------------------------
//:  -- Convert buf to a floating point buffer
//
gevd_bufferxy*  region_proc::get_float_buffer(gevd_bufferxy* buf)
{
  if(!buf)
    return NULL;
  gevd_bufferxy* fbuf =  new gevd_bufferxy(buf->GetSizeX(),
                                 buf->GetSizeY(),
                                 bits_per_float);
  gevd_float_operators::BufferToFloat(*buf, *fbuf);
  return fbuf;
}


//---------------------------------------------------------
//  -- convert a float buffer back to 16 bits/pixel
//
gevd_bufferxy* region_proc::put_float_buffer(gevd_bufferxy* fbuf)
{
  if(!fbuf)
    return NULL;

  gevd_bufferxy* pbuf = new gevd_bufferxy(fbuf->GetSizeX(), fbuf->GetSizeY(), 16);
  gevd_float_operators::FloatToBuffer(*fbuf, *pbuf);
  return pbuf;
}
//-------------------------------------------------------------------------
// -- Set the image to be processed
//
void region_proc::set_image(vil_image& image)
{
  if(!image)
    {
      vcl_cout <<"In region_proc::set_image(.) - null input" << vcl_endl;
      return;
    }
  _regions_valid = false;
  _image = image;
  if(_buf)
    {
      delete _buf;
      _buf = NULL;
    }
  //Expand by _expand_scale
  if(expand_scale_==1)
    {
      _buf = this->get_image_buffer(_image);
      return;
    }
  if(expand_scale_==2)
    {
      gevd_bufferxy* temp = this->get_image_buffer(_image);
      if(!temp)
        {
          vcl_cout <<"In region_proc::set_image(.) - couldn't get gevd_bufferxy from"
               <<" the image" << vcl_endl;
          return;
        }
      gevd_bufferxy* fbuf = this->get_float_buffer(temp);
      gevd_bufferxy* expand = NULL;
      gevd_float_operators::ExpandBy2(*fbuf, expand, burt_adelson_factor_);
      gevd_float_operators::TruncateToPositive(*expand);
      _buf = this->put_float_buffer(expand);
      delete temp;
      delete fbuf;
      delete expand;
      return;
    }
  if(expand_scale_==0.5)
    {
      gevd_bufferxy* temp = this->get_image_buffer(_image);
      if(!temp)
        {
          vcl_cout <<"In region_proc::set_image(.) - couldn't get gevd_bufferxy from"
               <<" the image" << vcl_endl;
          return;
        }
      gevd_bufferxy* fbuf = this->get_float_buffer(temp);
      gevd_bufferxy* shrink = NULL;
      gevd_float_operators::ShrinkBy2(*fbuf, shrink, burt_adelson_factor_);
      gevd_float_operators::TruncateToPositive(*shrink);
      _buf = this->put_float_buffer(shrink);
      delete temp;
      delete fbuf;
      delete shrink;
      return;
    }
  vcl_cout <<"In region_proc::set_image(.) - invalid expand scale factor "
       << vcl_endl;
}

//--------------------------------------------------------------------------
// -- extract a set of vdgl_poly_intensity_face(s)
void region_proc::extract_regions()
{
  if(_regions_valid)
    return;

  // Check the image
  if(!_buf)
    {
      vcl_cout << "In region_proc::extract_regions() - no image" << vcl_endl;
      return;
    }

  vcl_cout << "region_proc::extract_regions(): sizeX = "
       << _buf->GetSizeX() << " sizeY = " << _buf->GetSizeY() << vcl_endl;

  //Process the image to extract regions
  _regions.clear();

  // -tpk- need to pass along the scaled image rather than the orignal
  gevd_detector detector ( _image);

  //could move this all back into detector_params
  detector.junctionFactor = 1.0;
  detector.contourFactor = 2.0;
  detector.minLength = 4;
  detector.junctionp = 1;
  detector.smooth = 2.0;
  detector.borderp = 1;

  detector.DoContour();
  gevd_detector det(dp_);

  vcl_vector<vtol_edge_2d_sptr> * edgels, clean_edgels, broken_edgels;
  edgels = detector.GetEdges();

  if(!edgels->size())
    {
      vcl_cout << "In region_proc::extract_regions()- No Edgels were computed"
           << vcl_endl;
      return;
    }
  /* 
  vcl_vector<vtol_edge_2d_sptr>::iterator eit; = edgels.begin();
  for(eit = edgels->begin(); eit != edgels->end(); eit++)
    {
      vcl_cout << "Edgel output from DoContour:";
      (*eit)->describe(vcl_cout,2);
    }
  */

  //det.DoBreakCorners(edgels, broken_edgels);

  gevd_clean_edgels cl;
  cl.DoCleanEdgelChains(*edgels, clean_edgels);
  if(!clean_edgels.size())
    {
      vcl_cout << "In region_proc::extract_regions()- All edges removed by clean"
           << vcl_endl;
      return;
    }
  gevd_edgel_regions er(_debug);
  er.set_magnification(expand_scale_);
  //if(_verbose)
  //  er.SetVerbose();

  vcl_vector<vdgl_intensity_face_sptr> faces;
  //float xo = _roi_proc->get_xo(), yo = _roi_proc->get_yo();
  er.compute_edgel_regions(_buf, clean_edgels, faces);
  /*
  // Transform the extracted region boundaries if necessary
  float s = float(_expand_scale);
  if(s)
    {
      float si = 1/s;
      //We use a TwoChain to provide a superior to the set of Faces
      //so that the standard ::TaggedTransform can be used to 
      //transform IntensityFace(s) segmented at an expanded scale.
      TwoChain_ref tc = new TwoChain(faces.length());
      for(vcl_vector<IntensityFace*>::iterator fit = faces.begin();
          fit != faces.end(); fit++)
        tc->add_face((Face*)(*fit), '1');
      //Coordiantes are defined at the center of a pixel
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
  */
  //Copy the faces to a vector
  vcl_vector<vdgl_intensity_face_sptr>::iterator fit;
  for( fit = faces.begin(); fit != faces.end(); fit++)
    {
      //joe mod
      //vdgl_intensity_face_ref ifr = (*fit);
      //      if(!_roi_proc->inside_roi_mask(ifr))
      //if(!_roi_proc->inside_process_rois(ifr))
        continue;
      //end joe mod
      vdgl_intensity_face_sptr intf = (*fit);
      //vcl_vector<OneChain*>* chains = intf->OneChains();
      vdgl_digital_region * dr = intf->cast_to_digital_region();
      //vdgl_poly_intensity_face_ref rf = new vdgl_poly_intensity_face(chains, *dr);
      _regions.push_back(dr);
      //delete chains;
    }
  _regions_valid = true;
}
//----------------------------------------------------------
// -- Clear internal storage
//
void region_proc::clear()
{
  _regions.clear();
}


