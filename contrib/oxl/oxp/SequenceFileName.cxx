// This is oxl/oxp/SequenceFileName.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "SequenceFileName.h"

#include <vcl_cstddef.h> // for vcl_size_t()
#include <vcl_iostream.h>
#include <vul/vul_reg_exp.h>
#include <vul/vul_file.h>
#include <vul/vul_sprintf.h>

#include <oxp/oxp_parse_seqname.h>

SequenceFileName::SequenceFileName(char const* s, char const* read_or_write)
{
  init(s, 0, 1, read_or_write);
}

SequenceFileName::SequenceFileName(char const* s, int start_frame, int step, char const* read_or_write)
{
  init(s, start_frame, step, read_or_write);
}

void SequenceFileName::init(char const* s, int start_frame, int step, char const* /*read_or_write*/)
{
  // First match any trailing ",n[:n]:[n]" (can use ; or ,)
  oxp_parse_seqname range(s);
  if (range.start_ != -1) start_frame = range.start_;
  if (range.step_ != -1) step = range.step_;
  end_ = range.end_;

  if (end_ == -1)
    end_ = 9999999;

  fmt_ = range.filename_;
  ext_ = "";
  start_frame_ = start_frame;
  step_ = step;
  ok_ = false;

  set_end(end_);

  if (fmt_.find('%') == vcl_string::npos) {
    // No %, add one
    fmt_ += ".%03d";
  }

  {
    // Make any subdirectory if not an input_file
    vcl_size_t i = fmt_.rfind('/');
    // If no /, assume that the file is in CWD which exists
    if (i != vcl_string::npos) {
      vcl_string dir = fmt_.substr(0, i);
      if (!vul_file::exists(dir.c_str())) {
        vcl_cerr << "SequenceFileName: ** Image directory [" << dir << "] does not exist\n";
        return;
        // vcl_cerr << "SequenceFileName: ** Making directory " << dir << vcl_endl;
        // if (!vul_file::make_directory(dir.c_str())) {
        //   vcl_cerr << "SequenceFileName: ** Could not mkdir " << dir << vcl_endl;
        //   abort();
        // }
      } else
        if (!vul_file::is_directory(dir.c_str()))
          vcl_cerr << "SequenceFileName: WARNING: Inferred subdir [" << dir << ']'
                   << " exists and is not already a directory\n";
    }
  }

  // If no extension, we'll need a default
  {
    vul_reg_exp re("\\.([a-zA-Z_0-9]+)$");
    if (re.find(fmt_.c_str())) {
      vcl_cerr << "SequenceFileName: Found extension [" << re.match(1) << "]\n";
      int pointpos = re.start(0);
      ext_ = fmt_.substr(pointpos);
      fmt_.erase(pointpos, vcl_string::npos);
    }
  }

  ok_ = true;
}

vcl_string SequenceFileName::name(int frame)
{
  return vul_sprintf((fmt_ + ext_).c_str(), frame * step_ + start_frame_);
}

void SequenceFileName::set_default_extension(char const* extension)
{
  if (ext_.length() == 0) {
    ext_ = ".";
    ext_ += extension;
  }
}

bool SequenceFileName::exists(const vcl_string& fmt, const char* extension, int frame)
{
  vul_sprintf buf((fmt + extension).c_str(), frame);
  vcl_cerr << "SequenceFileName: Checking [" << buf << "]\n";
  return (vul_file::size(buf.c_str()) > 0 || vul_file::size((vcl_string(buf) + ".gz").c_str()) > 0);
}

vcl_ostream& SequenceFileName::print(vcl_ostream& s) const
{
  return s << '[' << fmt_ << ext_ << ' ' << start_frame_ << ':' << step_ << ':' << end_ << ']';
}
