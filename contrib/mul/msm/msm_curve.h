#ifndef msm_curve_h_
#define msm_curve_h_
//:
// \file
// \brief List of points making a curve - for defining boundaries
// \author Tim Cootes

#include <iostream>
#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_fwd.h>

//: List of points making a curve - for defining boundaries
//  A curve is a sequence of point indices indicating how
//  to joint the dots to form a boundary, for instance for
//  display or in an Active Shape Model.
//
//  If closed, then last point assumed to be connected to the
//  first.
class msm_curve
{
 private:
  //: Name of this curve or boundary
  std::string name_;

  //: True if this is an open curve (else closed curve)
  bool open_;

  //: List of point indices representing curve
  std::vector<unsigned> index_;
 public:
  // Default Constructor
  msm_curve();

  //: Define as range of indices [lo,hi]
  msm_curve(unsigned lo, unsigned hi,
            bool open=true, std::string name="");

  // Destructor
  ~msm_curve() = default;

  //: Number of points defining the curve
  std::size_t size() const { return index_.size(); }

  void set(const std::vector<unsigned>& index,
           bool open=true, std::string name="");

  //: Define as range of indices [lo,hi]
  void set(unsigned lo, unsigned hi,
           bool open=true, std::string name="");

  //: Name of this curve or boundary
  const std::string& name() const { return name_; }

  //: True if this is an open curve (else closed curve)
  bool open() const { return open_; }

  //: Indicate if this is an open curve
  void set_open(bool b) { open_=b; }

  //: List of point indices representing curve
  std::vector<unsigned>& index() { return index_; }

  //: List of point indices representing curve
  const std::vector<unsigned>& index() const { return index_; }

  unsigned operator[](unsigned i) const
  { assert(i<index_.size()); return index_[i]; }

  //: Return the largest index value
  unsigned max_index() const;

  //: Adds offset to index of every point
  //  Useful when concatenating models
  void add_index_offset(int offset);

  //: Parse parameters in stream
  //  Expects
  // \verbatim
  // { name: Chin open: true indices: { 0 1 2 3 4 5 6 } }
  // or equivalently
  // { name: Chin open: true indices: { 0 : 6 } }
  // \endverbatim
  void config_from_stream(std::istream&);

  //: Print class to os
  void print_summary(std::ostream& os) const;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs);

  //: Equality test
  bool operator==(const msm_curve& curve) const;
};


//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const msm_curve& c);


//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, msm_curve& c);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const msm_curve& c);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_curve& c);

//: Container for a set of curves
class msm_curves : public std::vector<msm_curve>
{
 public:
  //: Default constructor
  msm_curves();

  //: Construct as a single curve
  msm_curves(unsigned lo, unsigned hi,
             bool open=true, std::string name="");

  //: Return index of first curve with given name, or -1
  int which_curve(const std::string& name) const;

  //: Return the largest index value in any curve
  unsigned max_index() const;

  //: Parse parameters in stream
  //  Expects
  // \verbatim
  // {
  //   curve: { name: Chin open: true indices: { 0 1 2 3 4 5 6 } }
  //   curve: { name: Nose open: false indices: { 11 : 15 } }
  // }
  // \endverbatim
  void config_from_stream(std::istream&);

  //: Either read from file or parse parameters in string.
  // If curve_data starts with { then parse using config_from_stream,
  // else assume it is a filename and load.
  // If curves_data="-" then empty curves
  void parse_or_load(const std::string& curves_data);

  //: Save to text file
  // Writes in format:
  // \verbatim
  // curves: {
  //   curve: { name: Chin open: true indices: { 0 1 2 3 4 5 6 } }
  //   curve: { name: Nose open: false indices: { 11 : 15 } }
  // }
  // \endverbatim
  bool write_text_file(const std::string& path);

  //: Read from text file
  bool read_text_file(const std::string& path);
};

//: Stream output operator
std::ostream& operator<<(std::ostream& os,const msm_curves& c);

//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const msm_curves& c);


//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, msm_curves& c);


#endif // msm_curve_h_
