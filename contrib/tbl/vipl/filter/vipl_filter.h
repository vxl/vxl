// This is tbl/vipl/filter/vipl_filter.h
#ifndef vipl_filter_h_
#define vipl_filter_h_
//:
// \file
// Here is how the get/set macros are used.
// First note there are 4 types of access:
// for each of get and set we can do it with or without bounds/cache checking.
// These are respectively defined in 4 macros:
// GET_PIXEL SET_PIXEL FGET_PIXEL FSET_PIXEL (fast get...)
// Arguments for GET_PIXEL and FGET_PIXEL are (x,y)  (i.e. column,row) and they return the value.
// For SET_PIXEL FSET_PIXEL, the syntax is SET_PIXEL(x,y,value).
// Note that SET_PIXEL is expected to return value!
// We support operator() being the access, or if the used defines USE_NAMED_ACCESSORS, the named accessors.
// For the named accessors the user can #define the names to use.
// (And if they really want to, they can change the #defines for GET_PIXEL SET_PIXEL...
//  but be careful as existing code uses the args in the given order.)

#include "vipl_filter_abs.h"
#include <vipl/filter/vipl_trivial_pixeliter.h>

template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr = vipl_trivial_pixeliter >
 class vipl_filter ;

#include <vipl/section/vipl_section_descriptor.h>
#include <vipl/section/vipl_section_container.h>

#ifdef USE_NAMED_ACCESSORS // cannot have both set,
#undef USE_OPERATOR_ACCESSORS // to be safe if we have named we undefine operator()
#else
#ifndef USE_OPERATOR_ACCESSORS // if neither is set
#define USE_NAMED_ACCESSORS    // use names by default
#endif
#endif

#ifndef GET_NAME
#define GET_NAME get_pixel
#endif
#ifndef SET_NAME
#define SET_NAME set_pixel
#endif
#ifndef FGET_NAME
#define FGET_NAME fget_pixel
#endif
#ifndef FSET_NAME
#define FSET_NAME fset_pixel
#endif
#ifdef USE_OPERATOR_ACCESSORS
// if not we use operator accessors
#ifndef GET_PIXEL
#define GET_PIXEL(img,x,y)  (img)(x,y)
#endif
#ifndef SET_PIXEL
#define SET_PIXEL(img,x,y,expr)  (img)(x,y) = expr
#endif
#ifndef FGET_PIXEL
#define FGET_PIXEL(img,x,y)  (img)(x,y)
#endif
#ifndef FSET_PIXEL
#define FSET_PIXEL(img,x,y,expr)  (img)(x,y) = expr
#endif
#else // USE_NAMED_ACCESSORS
#ifndef GET_PIXEL
#define GET_PIXEL(img,x,y)  (img). GET_NAME (x,y)
#endif
#ifndef SET_PIXEL
#define SET_PIXEL(img,x,y,expr)  (img). SET_NAME (expr, x,y)
#endif
#ifndef FGET_PIXEL
#define FGET_PIXEL(img,x,y)  (img). FGET_NAME (x,y)
#endif
#ifndef FSET_PIXEL
#define FSET_PIXEL(img,x,y,expr)  (img). FSET_NAME (expr, x,y)
#endif
#endif // end use NAMED ACCESSORS

#ifndef CONVERT_TO_OUT
#define CONVERT_TO_OUT(v) ((DataOut) (v))
#endif
// the include file below defines the type VIPL_FILTER_STATE
#include "vipl_filter_helper.h" // for adding members by macros....

class vipl_trivial_pixeliter;
extern const void * DAhelp(vipl_trivial_pixeliter const*,int level=0);

template < class ImgIn, class ImgOut, class DataIn, class DataOut, int Arity, class PixelItr >
class vipl_filter          : public vipl_filter_abs
{
  // declare some static consts....
  static constexpr VIPL_FILTER_STATE Not_Ready = 0;
  static constexpr VIPL_FILTER_STATE Ready = 1;
  static constexpr VIPL_FILTER_STATE Unchanged = 2;
  static constexpr VIPL_FILTER_STATE Filter_Owned = 4;

 public: typedef typename PixelItr::Titerator Titerator;
 public: typedef typename PixelItr::Yiterator Yiterator;
 public: typedef typename PixelItr::Xiterator Xiterator;
 public: typedef typename PixelItr::Ziterator Ziterator;
 public: typedef vipl_filter_abs parent;
 public: typedef ImgIn const* inimagept; // make it easy to use.
 public: typedef ImgOut* outimagept; // make it easy to use.
 public: typedef vipl_filter thisclass; // needed for macro use

  //declare variable to hold "border" size for images.
  // May change to array in future (with argless accessor defaulting to largest)
 private: int hsimage_border_size;
 public: int image_border_size() const { return hsimage_border_size; }
 public: int & ref_image_border_size() { return hsimage_border_size; }
  // when too close to border what should we use as "fill" value.  It's up to
  // the filter implementer to "use" this value in their implementation so it
  // might get ignored...
 private: DataOut hsdef_fill_value;
 public: DataOut def_fill_value() const { return hsdef_fill_value; }
 public: DataOut & ref_def_fill_value() { return hsdef_fill_value; }
  // we track "state" for input, output and overall filter using these
  // ints. g++ had problems with nested enums as types outside of class
 private: VIPL_FILTER_STATE  hsinput_state;
 public: VIPL_FILTER_STATE input_state() const { return hsinput_state; }
 public: VIPL_FILTER_STATE & ref_input_state() { return hsinput_state; }
 private: VIPL_FILTER_STATE  hsfilter_state;
 public: VIPL_FILTER_STATE filter_state() const { return hsfilter_state; }
 public: VIPL_FILTER_STATE & ref_filter_state() { return hsfilter_state; }
 private: VIPL_FILTER_STATE  hsoutput_state;
 public: VIPL_FILTER_STATE output_state() const { return hsoutput_state; }
 public: VIPL_FILTER_STATE & ref_output_state() { return hsoutput_state; }
 public: void put_output_state(VIPL_FILTER_STATE const t) { hsoutput_state = t; }
  // how many input images are there.
 private: int hsnuminputs;
 public: int numinputs() const { return hsnuminputs; }
 public: int & ref_numinputs() { return hsnuminputs; }
  // how many output images are there. currently only 1 is allowed but this will change...
 private: int hsnumoutputs;
 public: int numoutputs() const { return hsnumoutputs; }
 public: int & ref_numoutputs() { return hsnumoutputs; }
  // raw "C" like array of pointers to input images
 private: std::vector<inimagept> hsinf;
 public: std::vector<inimagept> inf() const { return hsinf; }
 public: std::vector<inimagept> & ref_inf() { return hsinf; }
  // should be raw "C" like array of pointers to output images but
  // right now it is a direct pointer to the output image
 private: outimagept hsoutf;
 public: outimagept outf() const { return hsoutf; }
 public: outimagept & ref_outf() { return hsoutf; }
 public: void put_outf(outimagept const& t) { hsoutf = t; }
  // input section container.  Macros cannot handle commas so use typedef.
  // Assume one container works for all input images (implies multi-image filters cannot be ptr-safe.. FIXME)
  typedef vipl_section_container< DataIn >* in_section_type;
 private:   in_section_type hssrc_section;
 public: in_section_type src_section() const { return hssrc_section; }
 public: in_section_type & ref_src_section() { return hssrc_section; }
  // section descriptor for "current" input section
  typedef vipl_section_descriptor< DataIn >* in_descriptor_type;
 private:  in_descriptor_type hsinsecp;
 public: in_descriptor_type insecp() const { return hsinsecp; }
 public: in_descriptor_type & ref_insecp() { return hsinsecp; }
 public: void put_insecp(in_descriptor_type const t) { hsinsecp = t; }
  // output section container.  Macros cannot handle commas so use typedef
  typedef vipl_section_container< DataOut >* out_section_type;
 private:  out_section_type hsdst_section;
 public: out_section_type dst_section() const { return hsdst_section; }
 public: out_section_type & ref_dst_section() { return hsdst_section; }
  // section descriptor for "current" output section
  typedef vipl_section_descriptor< DataOut>* out_descriptor_type;
 private:  out_descriptor_type hssecp;
 public: out_descriptor_type secp() const { return hssecp; }
 public: out_descriptor_type & ref_secp() { return hssecp; }
 public: void put_secp(out_descriptor_type t) { hssecp = t; }
  // section descriptor for input ROA
  typedef vipl_section_descriptor< DataIn >* in_ROA_descriptor_type;
 private:  in_ROA_descriptor_type hsinROA;
 public: in_ROA_descriptor_type inROA() const { return hsinROA; }
 public: in_ROA_descriptor_type & ref_inROA() { return hsinROA; }
  // section descriptor for output ROA
  typedef vipl_section_descriptor< DataOut >* out_ROA_descriptor_type;
 private:  out_ROA_descriptor_type hsROA;
 public: out_descriptor_type ROA() const { return hsROA; }
 public: out_descriptor_type & ref_ROA() { return hsROA; }
  // if false (default) the ROA is taken from output image
  // if true, it is taken from the input image.
 private:  bool hsis_input_driven;
 public: bool is_input_driven() const { return hsis_input_driven; }
 public: bool & ref_is_input_driven() { return hsis_input_driven; }
 public: void put_is_input_driven(bool b=true) { hsis_input_driven=b; }

 public:
  //:
  // A workhorse constructor for this abstract class. If dst_image
  // (by default) the output will be generated automatically when
  // filtering is about to proceed. (Either way, the filter
  // increments refcount when set and decrements the refcount of
  // the output when it is destroyed.) Some filters support
  // multiple inputs, if ninputs is >1 then this constructor expects
  // src_img to be the first element pointer to the input
  // (i.e. src_img+1 is the location of input image2). Note that
  // the filter keeps pointers to the input (properly refcounted).
  vipl_filter(ImgIn const* src_img,
              ImgOut* dst_img=0,
              int ninputs=1,
              int img_border=0 ,
              DataOut fill_val=0 ) ;
  //:
  // A second workhorse constructor for this abstract class. If
  // dst_img is null (by default), the output will be generated
  // automatically when filtering is about to proceed. The filter
  // decrements the refcount of the output when it is
  // destroyed. Some filters support multiple inputs, if ninputs is
  // >1 then this constructor uses non_consecutive input images (with
  // their address in a c_vector, i.e. *(src_img+1) is the location
  // of input image2). Note that the filter keeps pointers to the
  // input (properly refcounted).
  vipl_filter(ImgIn const** src_img,
              ImgOut* dst_img=0,
              int ninputs=1,
              int img_border=0 ,
              DataOut fill_val=0 );
  virtual ~vipl_filter();
  vipl_filter();
  vipl_filter(vipl_filter< ImgIn, ImgOut, DataIn, DataOut, Arity, PixelItr > const&);

  // begin method list for class filter

  //:
  // The main operation of the class, filters input images to
  // produce output image.
  // Before this function can run to completion, all arguments must
  // be set via the respective ``put_*'' functions (or
  // be supplied at construction time). Particularly important is
  // the filter input. The programmer should take a look at the
  // concrete child classes of filter to see what additional
  // parameters they need before the actual filter operation can
  // proceed.
  virtual bool filter();

  //:
  // For those filters that only need one input and output set (after
  // construction), the following function allows one to treat the filter
  // object more like a function calling
  // obj.process(inimg, outimg)
  // will set the input and output then call filter()
  // It does not require pointers but takes the address of its
  // inputs, set the fields in the filter
  // does the filter and un-sets the in/out fields in the filter.
  bool process(ImgIn const& inimg, ImgOut& outimg);
  // second form passing pointers...
  bool process(ImgIn const* inimg, ImgOut* outimg);

  //: The ``start'' coordinate for the current apply section.
  // This always leaves a border around the
  // section. (E.g. if there is no ROA this is actual section start
  // + image_boarder_size; remember section iteration overlaps). If
  // the current section is outside the ROA, the section_start and
  // section_end may be equal.
  int start(int axis) const;
  int start(int axis, int other_axis_value) const;

  //: The ``stopping'' coordinate for the current apply section.
  // This always leaves a border around the
  // section. (E.g. if there is no ROA this is actual section end -
  // image_boarder_size; remember section iteration overlaps). If
  // the current section is outside the ROA, the section_start and
  // section_end may be equal.
  int stop(int axis) const;
  int stop(int axis, int other_axis_value) const;

  //: The ``start'' coordinate for the current source apply section.
  // This always leaves a border around the
  // section. (E.g. if there is no ROA this is actual section start
  // + image_boarder_size; remember section iteration overlaps). If
  // the current section is outside the ROA, the section_start and
  // section_end may be equal.
  int start_src(int axis) const;

  //: The ``stopping'' coordinate for the current source apply section.
  // This always leaves a border around the
  // section. (E.g. if there is no ROA this is actual section end -
  // image_boarder_size; remember section iteration overlaps). If
  // the current section is outside the ROA, the section_start and
  // section_end may be equal.
  int stop_src(int axis) const;

  //: The ``start'' coordinate for the current destination apply section.
  // This always leaves a border around the
  // section. (E.g. if there is no ROA this is actual section start
  // + image_boarder_size; remember section iteration overlaps). If
  // the current section is outside the ROA, the section_start and
  // section_end may be equal.
  int start_dst(int axis) const;

  //: The ``stopping'' coordinate for the current destination apply section.
  // This always leaves a border around the
  // section. (E.g. if there is no ROA this is actual section end -
  // image_boarder_size; remember section iteration overlaps). If
  // the current section is outside the ROA, the section_start and
  // section_end may be equal.
  int stop_dst(int axis) const;


  //:
  // Put the given pointer into an input "image" at the provided
  // index. Decrements old objects refcount, increments
  // newobjects refcount
  bool put_in_data_ptr(ImgIn const* fpointer, int index=0);

  //:
  // Return a smart pointer to the input ``image'' at the
  // provided index. Increments refcount before returning
  inimagept in_data_ptr( int index=0);

  //:
  // Return a ref to the input ``data object'' at the provided
  // index (dereferences the internal pointer).
  const ImgIn& in_data( int index=0) const ;

  //:
  // Put the given pointer into output data at the given index
  // location Decrements old putput refcount, Inc's newobjects
  // refcount
  bool put_out_data_ptr(ImgOut* fpointer, int /*index*/=0);

  //:
  // Get ptr to specified output data item given index
  // location. Inc's refcount before returning ptr
  virtual outimagept out_data_ptr(int index=0);

  //:
  // Get ref to specified output data item given index location
  virtual ImgOut& out_data(int index=0) const;

 protected:

  //:
  // This is the function that gets called for every iteration of
  // the filtering operation, before the actual filtering
  // routine. Can be used for normalization or such. Default op is
  // noop
  virtual bool preop();

  //:
  // This is the function that gets called after every iteration of
  // the actual filtering routine. Can be used for post_processing
  // normalization or cleaning up the edges. Default op is noop
  virtual bool postop();

  //:
  // This is the method that implements the basic form for the
  // filtering operation.
  virtual bool applyop() = 0;

  //:
  // For each section, this method runs before
  // section_applyop. Default at this level is no_op. (lower level
  // class redefines it to ``fill'' the image borders).
  virtual bool section_preop();

  //:
  // For each section, this method runs after
  // section_applyop. Default is no_op
  virtual bool section_postop();

  //:
  // This is the method that implements the filtering inside each
  // section. You must supply this function.
  // pure virtual function
  virtual bool section_applyop() = 0;

  //:
  // If a section is pointer safe, then this function is called to
  // filter it. default is just to call section_applyop
  virtual bool ptr_based_section_applyop();

  //:
  // Called by filter(). checks for input/output
  // being set.  User can make it check for additional
  // parameters required before the filtering operation can
  // proceed.  Allows filter to "proceed" on warnings so not public
  virtual bool check_params_1(bool& proceed_on_warn) const ;

 public:

  //:
  // returns if the filter is "ready" to run, i.e. all needed
  // parameters are "set".  Default just calls check_parms_1()
  virtual bool is_ready() const;

  //:
  // Is the current apply section intersected with the ROA an empty
  // region, if so we should not load it. If it's empty there is no
  // guarantee that the section_start and section_end will not overlap.
  int is_section_within_ROA( int axis) const;

#ifdef USE_COMPOSE_WITH
  //:
  // Try to set the output of this filter to be the input of
  // ``to'', and if possible make the filtering more efficient than
  // just sequential calls. Currently unimplemented so far this
  // function does nothing. Arg should be a nonconst ref because
  // composition may change the filter!.
  bool compose_with( vipl_filter_abs& to);
#endif

}; // end of class definition

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_filter.hxx"
#endif

#endif // file guard
