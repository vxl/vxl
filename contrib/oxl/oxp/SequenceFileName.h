// This is oxl/oxp/SequenceFileName.h
#ifndef SequenceFileName_h_
#define SequenceFileName_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 26 Jan 99
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include <vcl_iosfwd.h>

class SequenceFileName
{
 public:

  // Initialize and infer start and step from any ",1:2:100" in the filename
  SequenceFileName(char const* s, char const* read_or_write = "r");
  SequenceFileName(char const* s, int start_frame, int step, char const* read_or_write = "r");

  void set_default_extension(char const* ext);
  vcl_string name(int frame);
  vcl_ostream& print(vcl_ostream& s) const;

  //: Set start_frame, end_frame by scanning the directory given by the pattern, and finding limits.
  void probe_limits();

  int real_index(int index) const { return start_frame_ + index * step_; }

  int get_start_frame() const { return start_frame_; }
  int get_step() const { return step_; }
  int get_end() const { return end_; }

  int n() const { return n_; }

  void set_end(int e) {
    end_ = e;
    n_ = (end_ - start_frame_) / step_ + 1;
  }

 public:

  vcl_string fmt_;
  bool ok_;
  vcl_string ext_;
  int start_frame_;
  int step_;
  int end_;
  int n_;

 protected:
  void init(char const* s, int start_frame, int step, char const* read_or_write = "r");
  static bool exists(const vcl_string& fmt, const char* extension, int real_frame_index);
};

inline vcl_ostream& operator<<(vcl_ostream& s, const SequenceFileName& p) { return p.print(s); }

#endif // SequenceFileName_h_
