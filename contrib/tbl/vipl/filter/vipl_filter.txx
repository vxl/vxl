// This is tbl/vipl/filter/vipl_filter.txx
#ifndef vipl_filter_txx_
#define vipl_filter_txx_
//:
// \file

#include "vipl_filter.h"
#include <vcl_iostream.h>
#include <vcl_algorithm.h> // for vcl_max and vcl_min

#ifdef VCL_VC
#pragma warning( disable: 4390 )
#endif

template <class ImgIn,class ImgOut,class DataIn,class DataOut,int Arity,class PixelItr>
const VIPL_FILTER_STATE vipl_filter<ImgIn,ImgOut,DataIn,DataOut,Arity,PixelItr>::Not_Ready VCL_STATIC_CONST_INIT_INT_DEFN( 0 );
template <class ImgIn,class ImgOut,class DataIn,class DataOut,int Arity,class PixelItr>
const VIPL_FILTER_STATE vipl_filter<ImgIn,ImgOut,DataIn,DataOut,Arity,PixelItr>::Ready VCL_STATIC_CONST_INIT_INT_DEFN( 1 );
template <class ImgIn,class ImgOut,class DataIn,class DataOut,int Arity,class PixelItr>
const VIPL_FILTER_STATE vipl_filter<ImgIn,ImgOut,DataIn,DataOut,Arity,PixelItr>::Unchanged VCL_STATIC_CONST_INIT_INT_DEFN( 2 );
template <class ImgIn,class ImgOut,class DataIn,class DataOut,int Arity,class PixelItr>
const VIPL_FILTER_STATE vipl_filter<ImgIn,ImgOut,DataIn,DataOut,Arity,PixelItr>::Filter_Owned VCL_STATIC_CONST_INIT_INT_DEFN( 4 );

//: A workhorse constructor for this abstract class.
// If dst_image (by default) the output will be generated automatically when
// filtering is about to proceed.
// (Either way, the filter increments refcount when set and
// decrements the refcount of the output when it is destroyed.) Some filters
// support multiple inputs, if ninputs is >1 then this constructor expects
// src_img to be the first element pointer to the input (i.e. src_img+1 is
// the location of input image2). Note that the filter keeps pointers to the
// input (properly refcounted).
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int  Arity, class PixelItr>
  vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
  ::vipl_filter( ImgIn const* src_img ,
                 ImgOut* dst_img ,
                 int ninputs,
                 int img_border ,
                 DataOut fill_val )
  : hsimage_border_size(img_border),
    hsdef_fill_value( fill_val ),
    hsinput_state(Ready),
    hsfilter_state(Not_Ready),
    hsoutput_state(Not_Ready),
    hsnuminputs(ninputs),
    hsinf(vcl_vector<inimagept>(hsnuminputs)),
    hsoutf(dst_img),
    hssrc_section (0),
    hsinsecp (0),
    hsdst_section (0),
    hssecp (0),
    hsinROA (NULL),
    hsROA (NULL),
    hsis_input_driven(false)
{
#if 0
  for (int j = numinputs()-1; j>=0; --j)
    ref_inf()[j] = 0;
#endif
  if (dst_img) {
    if (UNCHANGED(output_state()))
      put_output_state(output_state() ^ Unchanged);
    if (FILTER_OWNED(output_state())) {
      put_output_state(output_state() ^ Filter_Owned);
#if 0
      if (READY(output_state())) FILTER_IMPTR_DEC_REFCOUNT(ref_outf());
#endif
    }
    if (NOT_READY(output_state()))
      put_output_state(output_state() | Ready);
#if 0
    FILTER_IMPTR_INC_REFCOUNT(dst_img);
#endif
  }
  for (int i=0; i< ninputs; i++) {
    ref_inf()[i] = src_img;
    if (src_img) src_img++; // if real, go to next one
#if 0
    if (ref_inf()[i]) FILTER_IMPTR_INC_REFCOUNT(((ImgIn*)ref_inf()[i]));
#endif
  }
}

//: A second workhorse constructor for this abstract class.
// If dst_img is null (by default), the output will be generated automatically
// when filtering is about to proceed. The filter decrements the refcount of the
// output when it is destroyed. Some filters support multiple inputs, if
// ninputs is >1 then this constructor uses non_consecutive input images (with
// their address in a c_vector, i.e. *(src_img+1) is the location of input
// image2). Note that the filter keeps pointers to the input (properly
// refcounted).
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
  ::vipl_filter( ImgIn const** src_img ,
                 ImgOut* dst_img ,
                 int ninputs,
                 int img_border ,
                 DataOut fill_val)
  : hsimage_border_size(img_border),
    hsdef_fill_value( fill_val ),
    hsinput_state(Ready),
    hsfilter_state(Not_Ready),
    hsoutput_state(Not_Ready),
    hsnuminputs(ninputs),
    hsinf(vcl_vector<inimagept>(hsnuminputs)),
    hsoutf(dst_img),
    hssrc_section (0),
    hsinsecp (0),
    hsdst_section (0),
    hssecp (0),
    hsinROA (NULL),
    hsROA (NULL),
    hsis_input_driven(false)
{
#if 0
  for (int j = numinputs()-1; j>=0; --j)
    ref_inf()[j] = 0;
#endif
  if (dst_img) {
    if (UNCHANGED(output_state()))
      put_output_state(output_state() ^ Unchanged);
    if (FILTER_OWNED(output_state())) {
      put_output_state(output_state() ^ Filter_Owned);
#if 0
      if (READY(output_state())) FILTER_IMPTR_DEC_REFCOUNT(ref_outf());
#endif
    }
    if (NOT_READY(output_state()))
      put_output_state(output_state() | Ready);
#if 0
    FILTER_IMPTR_INC_REFCOUNT(dst_img);
#endif
  }
  for (int i=0; i< ninputs; i++, src_img++) {
    if (src_img == 0)
      vcl_cerr << "filter ctor passed vector will null src_img pointers, ignored them watch out.\n";
    else
      ref_inf()[i] = *src_img;
#if 0
    if (ref_inf()[i]) FILTER_IMPTR_INC_REFCOUNT(((ImgIn*)ref_inf()[i]));
#endif
  }
}

template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
  ::~vipl_filter()
{
#ifdef DEBUG
  vcl_cout << "destructor for abstract class filter called " << this << vcl_endl;
#endif
#ifndef SMARTPTR
  if (ref_src_section()) FILTER_IMPTR_DEC_REFCOUNT(ref_src_section()); // dec_refcount or kill it
  if (ref_dst_section()) FILTER_IMPTR_DEC_REFCOUNT(ref_dst_section()); // dec_refcount or kill it
  if (ref_secp())        FILTER_IMPTR_DEC_REFCOUNT(ref_secp()); // dec_refcount or kill it
  if (ref_insecp())      FILTER_IMPTR_DEC_REFCOUNT(ref_insecp()); // dec_refcount or kill it
  if (ref_ROA())         FILTER_IMPTR_DEC_REFCOUNT(ref_ROA()); // dec_refcount or kill it
  if (ref_inROA())       FILTER_IMPTR_DEC_REFCOUNT(ref_inROA()); // dec_refcount or kill it
#if 0
  for (int i=0; i< numinputs(); i++) {
    if (ref_inf()[i]) { // no longer needed with smart pointers
      FILTER_IMPTR_DEC_REFCOUNT(
//    ((ImgIn*) (ref_inf()[i]))); //SGI CC doesn't like this...
      *((ImgIn**)(ref_inf())+i)); //FIXME, cast cause inf is const Im**
    }
  }
  //???  we did new (poor man's array) so we use delete????
  if (ref_inf()) {
    delete [] ref_inf();
    // free up new'd space
    ref_inf() = 0;
  }
  // delete the output and intermediate functions if they are filter-owned
#if 0
    if (FILTER_OWNED(hsoutput_state) && hsoutf)
#else
    if (ref_outf())
#endif
    {
      FILTER_IMPTR_DEC_REFCOUNT(ref_outf());
    }
#endif
#endif
}

template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
  ::vipl_filter()
  : hsimage_border_size(0),
    hsinput_state(Not_Ready),
    hsfilter_state(Not_Ready),
    hsoutput_state(Not_Ready),
    hsnuminputs(1),
    hsinf(vcl_vector<inimagept>(hsnuminputs)),
    hsoutf(0),
    hssrc_section(0),
    hsinsecp(0),
    hsdst_section(0),
    hssecp(0),
    hsinROA (NULL),
    hsROA (NULL),
    hsis_input_driven(false)
{
#if 0
  for (int i = 0; i < numinputs(); i++)
    ref_inf()[i] = 0;
#endif
}

template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                         ::vipl_filter(vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr > const& t)
  : vipl_filter_abs (t),
    hsimage_border_size(t.hsimage_border_size),
    hsdef_fill_value(t.hsdef_fill_value),
    hsinput_state(t.hsinput_state),
    hsfilter_state(t.hsfilter_state),
    hsoutput_state(t.hsoutput_state),
    hsnuminputs(t.hsnuminputs),
    hsinf(vcl_vector<inimagept>(hsnuminputs)),
    hsoutf(0),
    hssrc_section(t.hssrc_section),
    hsinsecp(t.hsinsecp),
    hsdst_section(t.hsdst_section),
    hssecp(t.hssecp),
    hsinROA (t.hsinROA),
    hsROA (t.hsROA),
    hsis_input_driven(t.hsis_input_driven)
// C++ low-level copy constructor
{
  // you can fill special ``copy constructor'' stuff here.
  // All dynamic/soft attributes are copied. Thus your
  //code should not change any soft attributes (if you
  //want to change it here is should be hard because it
  //is always changed!) So don't change things without
  //knowing their form.
  for (int i = 0; i < numinputs(); i++) {
    ref_inf()[i] = t.inf()[i];
#if 0
    if (ref_inf()[i]) FILTER_IMPTR_INC_REFCOUNT(((ImgIn*)ref_inf()[i]));
#endif
  }
#ifndef SMARTPTR
  if (ref_outf()) FILTER_IMPTR_INC_REFCOUNT(ref_outf());
  if (ref_src_section()) FILTER_IMPTR_INC_REFCOUNT(ref_src_section()); // share so inc_refcount
  if (ref_dst_section()) FILTER_IMPTR_INC_REFCOUNT(ref_dst_section()); // share so inc_refcount
  if (ref_secp()) FILTER_IMPTR_INC_REFCOUNT(ref_secp()); // share so inc_refcount
  if (ref_insecp()) FILTER_IMPTR_INC_REFCOUNT(ref_insecp()); // share so inc_refcount
  if (ref_ROA()) FILTER_IMPTR_INC_REFCOUNT(ref_ROA()); // share so inc_refcount
  if (ref_inROA()) FILTER_IMPTR_INC_REFCOUNT(ref_inROA()); // share so inc_refcount
#endif
  // FIXME should we do deep copy of filter-ownable parameters.???
  if (FILTER_OWNED(hsoutput_state))
    hsoutput_state ^= Filter_Owned;
}

//:
// Is the current apply section intersected with the ROA an empty region, if
// so we should not load it. If its empty there is no guarantee that the
// section_start and section_end will not overlap.
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  int vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                   ::is_section_within_ROA(int axis) const
{
  if (is_input_driven()) { // if we are input driven
    if (!inROA())
  // if we don't have an ROA we must be in it.
      return true;
    // should this consider insecp???????
    if (insecp()) { // ok have a valid section .
      for (int i =0; i < Arity; i++){
        if ((insecp()->curr_sec_start(axis) > inROA()->curr_sec_end(axis)) ||
            (insecp()->curr_sec_end(axis) < inROA()->curr_sec_start(axis)))
          return false;
      }
      return true;
  // if we make it this far, must be in
    }
  } else {
    // if we don't have an ROA we must be in it.
    if (!ROA())
      return true;
    // should this consider secp or insecp???????
    if (secp()) { // ok have a valid section .
      for (int i =0; i < Arity; i++) {
        if ((secp()->curr_sec_start(axis) > ROA()->curr_sec_end(axis)) ||
            (secp()->curr_sec_end(axis) < ROA()->curr_sec_start(axis)))
          return false;
      }
      return true;
  // if we make it this far, must be in
    }
  }
  // error if reaching this point:
  vcl_cerr << "Warning: called is_section_within_ROA but no valid sections defined. Returning 0\n";
  return 0;
}

//: What is the ``start coordinate'' for the current apply section.
// This always leaves a border around the section. (E.g. if there is no ROA this
// is actual section start + image_boarder_size;  remember section iteration
// overlaps).  If the current section is outside the ROA, the section_start
// and section_end may be equal.
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  int vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                     ::start(int axis) const
{
  if (is_input_driven()) return start_src(axis);
  else  return start_dst(axis);
}

//: What is the ``start'' coordinate for the current source apply section.
// This always leaves a border around the section. (E.g. if there is no ROA this
// is actual section start + image_boarder_size;  remember section iteration
// overlaps).  If the current section is outside the ROA, the section_start
// and section_end may be equal.
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  int vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                     ::start_src(int axis) const
{
  if (insecp()){ // ok have a valid section
    int ibs = image_border_size();
    int end = insecp()->curr_sec_end(axis);
    int st = insecp()->curr_sec_start(axis);
    if (st > end){ // swap hack in case people get it wrong....
      int temp = end; end = st; st = temp;
    }
    st += ibs;
    end -= ibs;
    if (inROA())
      st =vcl_min(end,vcl_max(st,inROA()->curr_sec_start(axis)+ibs));
#ifdef DEBUG
    vcl_cerr << "i_ [" << axis << "] st=" << st << " ibs=" << ibs << vcl_endl;
#endif
    return st;
  }

  // error if reaching this point:
  vcl_cerr << "Warning: called start_src but no valid sections defined. Returning 0\n";
  return 0;
}


//: What is the ``start'' coordinate for the current destination apply section.
// This always leaves a border around the section. (E.g. if there is no ROA this
// is actual section start + image_boarder_size;  remember section iteration
// overlaps).  If the current section is outside the ROA, the section_start
// and section_end may be equal.
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  int vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                     ::start_dst(int axis) const
{
  // should we consider secp or insecp????
  if (secp()){ // ok have a valid section
    int ibs = image_border_size();
    int end = secp()->curr_sec_end(axis);
    int st = secp()->curr_sec_start(axis);
    if (st > end){ // swap hack in case people get it wrong....
      int temp = end; end = st; st = temp;
    }
    st += ibs;
    end -= ibs;
    if (ROA())
      st = vcl_min(end,vcl_max(st,ROA()->curr_sec_start(axis)+ibs));
#ifdef DEBUG
    vcl_cerr << "o_ [" << axis << "] st=" << st << " ibs=" << ibs << vcl_endl;
#endif
    return st;
  }

  // error if reaching this point:
  vcl_cerr << "Warning: called start_dst but no valid sections defined. Returning 0\n";
  return 0;
}

template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  int vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                     ::start(int axis, int /*other_axis_value*/) const
{ return start(axis); }

//: What is the ``stopping'' coordinate for the current apply section.
// This always leaves a border around the section. (E.g. if there is no ROA this
// is actual section end - image_boarder_size;  remember section iteration
// overlaps).  If the current section is outside the ROA, the section_start
// and section_end may be equal.
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  int vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                     ::stop(int axis) const
{
  if (is_input_driven()) return stop_src(axis);
  else  return stop_dst(axis);
}


//: What is the ``stopping'' coordinate for the current apply section.
// This always leaves a border around the section. (E.g. if there is no ROA this
// is actual section end - image_boarder_size;  remember section iteration
// overlaps).  If the current section is outside the ROA, the section_start
// and section_end may be equal.
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  int vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                     ::stop_src(int axis) const
{
  if (insecp()){ // ok have a valid section
    int ibs = image_border_size();
    int end = insecp()->curr_sec_end(axis);
    int st = insecp()->curr_sec_start(axis);
    if (st > end){ // swap hack in case people get it wrong....
      int temp = end; end = st; st = temp;
    }
    end -= ibs;
#ifdef DEBUG
    vcl_cerr << "_i [" << axis << "] end=" << end << " ibs=" << ibs << vcl_endl;
#endif
    return end;
  }

  // error if reaching this point:
  vcl_cerr << "Warning: called stop_src but no valid sections defined. Returning 0\n";
  return 0;
}


//: What is the ``stopping'' coordinate for the current apply section.
// This always leaves a border around the section. (E.g. if there is no ROA this
// is actual section end - image_boarder_size;  remember section iteration
// overlaps).  If the current section is outside the ROA, the section_start
// and section_end may be equal.
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  int vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                     ::stop_dst(int axis) const
{
  // should we consider secp or insecp????
  if (secp()){ // ok have a valid section
    int ibs = image_border_size();
    int end = secp()->curr_sec_end(axis);
    int st = secp()->curr_sec_start(axis);
    if (st > end){ // swap hack in case people get it wrong....
      int temp = end; end = st; st = temp;
    }
    end -= ibs;
#ifdef DEBUG
    vcl_cerr << "_o [" << axis << "] end=" << end << " ibs=" << ibs << vcl_endl;
#endif
    return end;
  }

  // error if reaching this point:
  vcl_cerr << "Warning: called stop_dst but no valid sections defined. Returning 0\n";
  return 0;
}

template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  int vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                     ::stop(int axis, int /*other_axis_value*/) const
{ return stop(axis); }

// Put the given pointer into the array of input ``functions'' at the
// provided index. Decrements old objects refcount, Inc's newobjects
// refcount
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  bool vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                        ::put_in_data_ptr(ImgIn const* fpointer , int index)
{
  if (UNCHANGED(input_state()) || NOT_READY(input_state()))
    ref_input_state() = Ready;
  if ( 0 <= index && index < numinputs()) {
#if 0
    if (ref_inf()[index]) { // no longer needed for smart pointers
      // FILTER_IMPTR_DEC_REFCOUNT(((ImgIn*)ref_inf()[index])); //SGI CC doesn't like this...
      FILTER_IMPTR_DEC_REFCOUNT(*((ImgIn**)(ref_inf())+index)); // release old
    }
#endif
    ref_inf()[index] = fpointer;
#if 0
    if (fpointer) FILTER_IMPTR_INC_REFCOUNT(((ImgIn*)fpointer)); // mark new
#endif
    return true;
  }
  // error if reaching this point:
  vcl_cerr << "Warning: index out of range in put_in_data_ptr, ignored\n";
  return false;
}

// Return a pointer to the input ``functions'' at the provided
// index. Increments refcount before returning
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  ImgIn const* vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                          ::in_data_ptr(int index)
{
  if (index < 0 || index >= numinputs()) {
    vcl_cerr << "Warning: index " << index << " out of range, returning data at 0 instead\n";
    index = 0;
  }
  return inf()[index];
}

//: Return a ref to the input ``data object'' at the provided index.
// (dereferences the internal pointer)
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  const ImgIn& vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr>
                           ::in_data(int index) const
{
  if (0 <= index && index < numinputs()) {
    if (inf()[index])
      return *inf()[index];
    else {
      vcl_cerr << "Warning: input pointer is null returning image at index 0\n";
      return *inf()[0];
    }
  } //else
   vcl_cerr << "Warning: out of range is null, a new val, it will leak\n";
   return *inf()[0];
}

//: Put the given pointer into output data at the given index location.
// Decrements old putput refcount, increments newobjects refcount.
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  bool vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                        ::put_out_data_ptr(ImgOut* fpointer , int /*index*/)
{
  if (UNCHANGED(output_state()))    put_output_state(output_state() ^ Unchanged);
  if (FILTER_OWNED(output_state())) put_output_state(output_state() ^ Filter_Owned);
#if 0
  if (ref_outf())                   FILTER_IMPTR_DEC_REFCOUNT(ref_outf());
#endif
  if (NOT_READY(output_state()))    put_output_state(output_state() | Ready);
#if 0
  FILTER_IMPTR_INC_REFCOUNT(fpointer); // we will keep a pointer, inc it
#endif
  put_outf(fpointer);
  return true;
}

//: Get ptr to specified output data item given index location.
// Inc's refcount before returning ptr.
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  ImgOut* vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                     ::out_data_ptr(int /*index*/)
{
  if (READY(output_state()) || UNCHANGED(output_state()))
    put_output_state(output_state() ^ Unchanged);
  if (READY(output_state()) )
    return ref_outf();
  else {
    vcl_cerr << "Warning: Tried to reference a NOT READY output-data, returned 0\n";
    return 0;
  }
}

//: Get ref to specified output data item given index location.
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
   ImgOut& vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr>
                            ::out_data(int /*index*/) const
{
  if (READY(output_state()) )
    return *outf();
  else {
    vcl_cerr << "Warning: Tried to reference a NOT READY output-returning old input, may coredump\n";
    return *outf();
  }
}

//: This function gets called for every iteration of the filtering operation, before the actual filtering routine.
// Can be used for input normalization or such. Default op is noop.
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  bool vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                      ::preop()
{
  return true;
}

//: This function gets called after every iteration of the actual filtering routine.
// Can be used for post_processing normalization or cleaning up the edges. Default op is noop.
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  bool vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                      ::postop()
{
  return true;
}

//: This is the method that implements the basic form for the filtering operation.
// For each section, this method runs before section_applyop. Default at this
// level is noop. (lower level class redefine it to ``fill'' the image borders).
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  bool vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                      ::section_preop()
{
  return true;
}

//: For each section, this method runs after section_applyop.
// Default is noop
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  bool vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                     ::section_postop()
{
  return true;
}

//: This is the method that implements the filtering inside each section.
// You must supply this function.
// If a section is pointer safe, then this function is called to filter
// it. Default is just to call the non-pointer section_applyop
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  bool vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                      ::ptr_based_section_applyop()
{
  return section_applyop();
}

//:
// Before this function can run to completion, all arguments must be set via
// the respective ``put_*'' functions (or be supplied at
// construction time). Particularly important is the filter input. The
// programmer should take a look at the concrete child classes of filter to
// see what additional parameters they need before the actual filter
// operation can proceed.
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  bool vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                     ::filter()
{
  bool proceed_anyway = false;
  if (NOT_READY(input_state())) {
    // first make sure that the filter is ready to proceed
    ref_filter_state() = Not_Ready;
    vcl_cerr << "Warning: filtering without valid input\n";
    return false;
  } else if (UNCHANGED(input_state())) {
    ref_filter_state() |= Unchanged;
  } else
    ref_filter_state() |= Ready;
  if (NOT_READY(output_state()) || (FILTER_OWNED(output_state()) &&
                                    CHANGED(input_state()))) {
    // be conservative - if input has changed, then may need to regen the
    // output since we can not compare respective sizes .
#if 0
    if (outf()) FILTER_IMPTR_DEC_REFCOUNT(ref_outf());
#endif
    vcl_cerr << "Warning: Input changed after output set.  Sizes may not match...\n";
  }
  if ((check_params_1(proceed_anyway) &&
      READY(filter_state()) && CHANGED(filter_state())) || proceed_anyway) {
    return applyop();
  }
  return proceed_anyway;
}

template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  bool vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                       ::is_ready() const
{
  bool proceeding=false;
  return check_params_1(proceeding) && !proceeding;
}

//:
// For those filters that only need one input and output set (after
// construction), the following function allows one to treat the filter
// object more like a function calling
//     obj.process(inimg, outimg)
// will set the input and output then call filter()
// It does not require pointers but takes the address of its
// inputs, set the fields in the filter
// does the filter and un-sets the in/out fields in the filter.
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
bool vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                     ::process( ImgIn const& inimg, ImgOut& outimg)
{
  put_in_data_ptr(&inimg);
  put_out_data_ptr(&outimg);
  bool ret = filter(); // do the real work
  put_in_data_ptr(0);  // reset input
  put_out_data_ptr(0); // reset output
  return ret;
}

//: second process form passing imgs by ptr.
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
bool vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                     ::process( ImgIn const* inimg, ImgOut* outimg)
{
  put_in_data_ptr(inimg);
  put_out_data_ptr(outimg);
  bool ret = filter(); // do the real work
  put_in_data_ptr(0);  // reset input
  put_out_data_ptr(0); // reset output
  return ret;
}

//:
// Called by \usemethod {filter()} and checks additional parameters required
// before the filtering operation can proceed. Default is empty_func
// returning true, but subclasses can define as then need.  check_parms_1
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  bool vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                        ::check_params_1( bool& proceed_on_warn) const
{
  proceed_on_warn = true;
  // check input and output are OK.
  return !NOT_READY(output_state()) && !NOT_READY(input_state());
}

#ifdef USE_COMPOSE_WITH
//:
// Try to set the output of this filter to be the input of ``to'', and
// if possible make the filtering more efficient than just sequential
// calls. Currently unimplemented so far this function does
// nothing. Arg should be a nonconst ref because composition may
// change the filter!
template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr>
  bool vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr >
                      ::compose_with(vipl_filter_abs& to)
{
  vcl_cerr << "Warning: called unimplemented method compose_with\n";
  return false;
}
#endif

#endif // vipl_filter_txx_
