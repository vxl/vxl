// This is tbl/vipl/filter/vipl_filter_2d.txx
#ifndef vipl_filter_2d_txx_
#define vipl_filter_2d_txx_

#include "vipl_filter_2d.h"
#include <vcl_compiler.h>

// Main constructor for this abstract class. If dst_image
// (by default) the output will be generated automatically when
// filtering is about to proceed. (Either way, the filter
// decrements the refcount of the output when it is destroyed.)
// Some filters support multiple inputs, if ninputs is >1 then
// this constructor expects src_img to be the first element
// pointer to the input (i.e. src_img+1 is the location of input
// image2). Note that the filter keeps pointers to the input
// (properly refcounted). Actually the main filter constructor
// does all the work
template < class ImgIn,class ImgOut,class DataIn,class DataOut, class PixelItr >
  vipl_filter_2d< ImgIn,ImgOut,DataIn,DataOut,PixelItr >
  ::vipl_filter_2d(
                const ImgIn* src_img ,
                ImgOut* dst_img ,
                int ninputs,
                int img_border ,
                DataOut fill_val
                )
    : vipl_filter<ImgIn, ImgOut, DataIn, DataOut, 2 VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>
      (src_img, dst_img, ninputs, img_border, fill_val)
{}

// A second the workhorse constructor for this class. If dst_img is null (by
// default), the output will be generated automatically when filtering is
// about to proceed. The filter decrements the refcount of the output when it
// is destroyed. Some filters support multiple inputs, if ninputs is >1 then
// this constructor uses non_consecutive input images (with their address in a
// c_vector, i.e. *(src_img+1) is the location of input image2). Note that
// the filter keeps pointers to the input (properly refcounted). Actually the
// main filter constructor does all the work
template < class ImgIn,class ImgOut,class DataIn,class DataOut, class PixelItr >
  vipl_filter_2d< ImgIn,ImgOut,DataIn,DataOut,PixelItr > ::vipl_filter_2d(
                const ImgIn** src_img ,
                ImgOut* dst_img ,
                int ninputs,
                int img_border ,
                DataOut fill_val
                )
     : vipl_filter<ImgIn, ImgOut, DataIn, DataOut, 2 VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)>
       (src_img, dst_img, ninputs, img_border, fill_val)
{}

template < class ImgIn,class ImgOut,class DataIn,class DataOut, class PixelItr >
  vipl_filter_2d< ImgIn,ImgOut,DataIn,DataOut,PixelItr >
  ::vipl_filter_2d(const vipl_filter_2d< ImgIn,ImgOut,DataIn,DataOut,PixelItr > &t)
  :
  vipl_filter< ImgIn, ImgOut, DataIn, DataOut, 2 VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> (t)
// C++ special low-level copy constructor
{}

// This is the method that walks over the sections calling
// section_applyop. Now that we know the dim we can write the loop...
template < class ImgIn,class ImgOut,class DataIn,class DataOut, class PixelItr >
  bool vipl_filter_2d< ImgIn,ImgOut,DataIn,DataOut,PixelItr > ::applyop()
{
  typedef typename vipl_section_container< DataIn  >::iterator iter_in;
  typedef typename vipl_section_container< DataOut >::iterator iter_out;

  // assuming that the coordinate space of input, intermediate and output are
  // "locked" by sectioning
  if (!ref_outf()) {
    vcl_cerr << "Warning: empty output image in vipl_filter_2d::applyop, returning without processing\n";
    return false;
  }
  // the name of the section container generator.
  // do not generate a new one if there is one already.
  if (!ref_dst_section()) {
    ref_dst_section() = vipl_filterable_section_container_generator(*ref_outf(),(DataOut*) 0);
  }
  if (!ref_dst_section()) {
    vcl_cerr << "Warning: empty dst section in vipl_filter_2d::applyop, returning without processing\n";
    return false;
  }
  if (!ref_src_section()) {
    ref_src_section() = vipl_filterable_section_container_generator(*ref_inf()[0], (DataIn*) 0);
  }
  if (!ref_src_section()) {
    vcl_cerr << "Warning: empty src section in vipl_filter_2d::applyop, presuming output driving but cannot be ptr safe\n";
  }
  preop(); // virtual function call
  ref_dst_section()->ref_overlap()[0] = image_border_size();
  ref_dst_section()->ref_overlap()[1] = image_border_size();
  if (ref_src_section()) {
    ref_src_section()->ref_overlap()[0] = image_border_size();
    ref_src_section()->ref_overlap()[1] = image_border_size();
  }
  iter_out enddstitr, dstitr;
  iter_in endsrcitr, srcitr;
  if (ref_src_section()) {
    endsrcitr = (*ref_src_section()).end();
    srcitr = (*ref_src_section()).begin();
  }
  if (ref_dst_section()) {
    enddstitr = (*ref_dst_section()).end();
    dstitr = (*ref_dst_section()).begin();
  }
  if (is_input_driven()) {
    iter_in enditr = (*ref_src_section()).end();
    for (iter_in it = (*ref_src_section()).begin(); it != enditr; ++it) {
      if (dstitr ==enddstitr) {
        vcl_cerr << "Warning: In vipl_filter_2d, output iter ran out of items before input.  resetting to beginning\n";
        dstitr = (*ref_dst_section()).begin();
      }
#ifdef VCL_VC60
      // this awkward construction is to get around a VC60 compiler bug
      vipl_section_descriptor<DataOut>& secDesc = *dstitr;
      put_secp( new vipl_section_descriptor<DataOut> (secDesc) );
#else
      put_secp( new vipl_section_descriptor<DataOut> (*dstitr));
#endif
      ref_dst_section()->ref_overlap()[0] = image_border_size();
      ref_dst_section()->ref_overlap()[1] = image_border_size();
      if (ref_src_section()) {
#ifdef VCL_VC60
        // this awkward construction is to get around a VC60 compiler bug
        vipl_section_descriptor<DataIn>& inSecDesc = *it;
        put_insecp( new vipl_section_descriptor<DataIn> (inSecDesc));
#else
        put_insecp( new vipl_section_descriptor<DataIn> (*it));
#endif
        ref_src_section()->ref_overlap()[0] = image_border_size();
        ref_src_section()->ref_overlap()[1] = image_border_size();
      }
      section_preop(); // virtual function call
      section_applyop(); // virtual function call
      section_postop(); // virtual function call
      if (ref_secp()) {
        FILTER_IMPTR_DEC_REFCOUNT(ref_secp()); // ??really what we want or a kludge??
        ++dstitr;
      }
      if (ref_insecp()) {
        FILTER_IMPTR_DEC_REFCOUNT(ref_insecp());
      }
    }
  }
  else {
    iter_out enditr = (*ref_dst_section()).end();
    for (iter_out it = (*ref_dst_section()).begin(); it != enditr; ++it) {
      if (srcitr == endsrcitr) {
        vcl_cerr << "Warning: In vipl_filter_2d, input iter ran out of items before output.  resetting to beginning\n";
        srcitr = (*ref_src_section()).begin();
      }
#ifdef VCL_VC60
      // this awkward construction is to get around a VC60 compiler bug
      vipl_section_descriptor<DataOut>& secDesc2 = *it;
      put_secp( new vipl_section_descriptor<DataOut> (secDesc2));
#else
      put_secp( new vipl_section_descriptor<DataOut> (*it));
#endif
      ref_dst_section()->ref_overlap()[0] = image_border_size();
      ref_dst_section()->ref_overlap()[1] = image_border_size();
      if (ref_src_section()) {
#ifdef VCL_VC60
        // this awkward construction is to get around a VC60 compiler bug
        vipl_section_descriptor<DataIn>& inSecDesc2 = *srcitr;
        put_insecp( new vipl_section_descriptor<DataIn> (inSecDesc2));
#else
        put_insecp( new vipl_section_descriptor<DataIn> (*srcitr));
#endif
        ref_src_section()->ref_overlap()[0] = image_border_size();
        ref_src_section()->ref_overlap()[1] = image_border_size();
      }
      section_preop(); // virtual function call
      section_applyop(); // virtual function call
      section_postop(); // virtual function call
      if (ref_secp()) {
        FILTER_IMPTR_DEC_REFCOUNT(ref_secp()); // ??really what we want or a kludge??
      }
      if (ref_insecp()) {
        FILTER_IMPTR_DEC_REFCOUNT(ref_insecp());
        ++srcitr;
      }
    }
  }
  postop(); // virtual function call
  return true;
}

#endif // vipl_filter_2d_txx_
