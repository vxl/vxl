// This is mul/vil3d/file_formats/vil3d_analyze_format.h
#ifndef vil3d_analyze_format_h_
#define vil3d_analyze_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Reader/Writer for analyze format images.
// \author Tim Cootes - Manchester
// (Based on old code whos provenance has been lost in the mists of time...)

#include <vil3d/vil3d_file_format.h>
#include <vil/vil_stream.h>

//: Structure containing anaylse header file information.
class vil3d_analyze_header {
public :

    // obligatory
  class Key {

      public :

    int sizeof_hdr;
    char data_type[10];
    char db_name[18];
    int extents;
    short int session_error;
    char regular;
    char hkey_un0;

    Key() { reset(); }
    ~Key() {}

    void reset();
  };

    // obligatory
  class Dimensions {

      public :

    short int dim[8];
    short int unused8;
    short int unused9;
    short int unused10;
    short int unused11;
    short int unused12;
    short int unused13;
    short int unused14;
    short int datatype;
    short int bitpix;
    short int dim_un0;
    float pixdim[8];
    float funused8;
    float funused9;
    float funused10;
    float funused11;
    float funused12;
    float funused13;
    float compressed;
    float verified;
    int glmax;
    int glmin;

    Dimensions() { reset(); }
    ~Dimensions() {}

    void reset();
  };

    // optional
  class History {

      public :

    char descrip[80];
    char aux_file[24];
    char orient;
    char originator[10];
    char generated[10];
    char scannum[10];
    char patient_id[10];
    char exp_date[10];
    char exp_time[10];
    char hist_un0[3];
    int views;
    int vols_added;
    int start_field;
    int field_skip;
    int omax;
    int omin;
    int smax;
    int smin;

    History() { reset(); }
    ~History() {}

    void reset();
  };

  Key        key;
  Dimensions dim;
  History    history;

private:
  bool swap_bytes_; // True if bytes need to be swapped

public:
  vil3d_analyze_header() {}
  ~vil3d_analyze_header() {}

  void reset();

  //: Define format of pixels
  enum vil_pixel_format pixel_format() const;

  //: Define format of pixels
  void set_pixel_format(enum vil_pixel_format format);

  //: Define number of pixels in each dimension
  void set_image_size(unsigned ni, unsigned nj, unsigned nk);

  short int ni() const { return dim.dim[1]; }
  short int nj() const { return dim.dim[2]; }
  short int nk() const { return dim.dim[3]; }

  //: Define width of voxels in each dimension
  void vil3d_analyze_header::set_voxel_size(float si, float sj, float sk);

  //: Read in header from given file
  bool read_file(const vcl_string& path);

  //: Write header to given file
  bool write_file(const vcl_string& path) const;

  void swapBytes(char *data, int size);
  bool needSwap() { return swap_bytes_; }

  //: Print out some parts of header
  void print_summary(vcl_ostream& os) const;
};

//: Print out some parts of header
vcl_ostream& operator<<(vcl_ostream& os, const vil3d_analyze_header&);

//: Reader/Writer for analyze format images.
class vil3d_analyze_format : public vil3d_file_format
{
 public:
  vil3d_analyze_format();
  //: The destructor must be virtual so that the memory chunk is destroyed.
  virtual ~vil3d_analyze_format();

  virtual vil3d_image_resource_sptr make_input_image(const char *) const;

  //: Make a "generic_image" on which put_section may be applied.
  // The file may be opened immediately for writing so that a header can be written.
  virtual vil3d_image_resource_sptr make_output_image(const char* filename,
                                                      unsigned ni,
                                                      unsigned nj,
                                                      unsigned nk,
                                                      unsigned nplanes,
                                                      enum vil_pixel_format) const;


  //: default filename tag for this image.
  virtual const char * tag() const {return "analyze";}
};

//: Object which acts as an interface to an analyze format file
// You can't create one of these yourself.
// Use vil3d_analyze_format instead.
class vil3d_analyze_image: public vil3d_image_resource
{
  //: Basename of file (not including .hdr/.img)
  vcl_string base_path_;

  //: Header information
  vil3d_analyze_header header_;

  //: image dimensions
  unsigned dim1_, dim2_, dim3_;

  //: number of planes
  unsigned nplanes_;

  //: Physical Voxel dimensions ( in mm )
  float vox_width1_, vox_width2_, vox_width3_;

public:
  //: Create object with given header and base_path
  //  Doesn't actually load/save anything until get_copy_view()
  //  or put_view() called.
  vil3d_analyze_image(vil3d_analyze_header& header,
                      const vcl_string& base_path);

  virtual ~vil3d_analyze_image();

  //: Dimensions:  nplanes x ni x nj x nk.
  // This concept is treated as a synonym to components.
  virtual unsigned nplanes() const;
  //: Dimensions:  nplanes x ni x nj x nk.
  // The number of pixels in each row.
  virtual unsigned ni() const;
  //: Dimensions:  nplanes x ni x nj x nk.
  // The number of pixels in each column.
  virtual unsigned nj() const;
  //: Dimensions:  nplanes x ni x nj x nk.
  // The number of slices per image.
  virtual unsigned nk() const;

  //: Basename of file (not including .hdr/.img)
  const vcl_string& base_path() const { return base_path_; }

  //: Header information
  const vil3d_analyze_header& header() { return header_; }

  //: Pixel Format.
  virtual enum vil_pixel_format pixel_format() const;


  //: Create a read/write view of a copy of this data.
  // This function will always return a
  // multi-plane scalar-pixel view of the data.
  // \return 0 if unable to get view of correct size, or if resource is write-only.
  virtual vil3d_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                   unsigned j0, unsigned nj,
                                                   unsigned k0, unsigned nk) const;

  //: Put the data in this view back into the image source.
  // The view must be of scalar components. Assign your
  // view to a scalar-component view if this is not the case.
  // \return false if failed, because e.g. resource is read-only,
  // format of view is not correct (if it is a compound pixel type, try
  // assigning it to a multi-plane scalar pixel view.)
  virtual bool put_view(const vil3d_image_view_base& im,
                        unsigned i0, unsigned j0, unsigned k0);

  //: Return a string describing the file format.
  // Only file images have a format, others return 0
  virtual char const* file_format() const { return "analyze"; }

  //: Extra property information
  // This will just return the property of the first slice in the list.
  virtual bool get_property(char const* label, void* property_value = 0) const;
};

#endif
