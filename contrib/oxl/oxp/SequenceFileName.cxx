#ifdef __GNUC__
#pragma implementation
#endif

#include "SequenceFileName.h"

#include <vcl_cstdlib.h> // atoi()
#include <vcl_iostream.h>
#include <vbl/vbl_reg_exp.h>
#include <vbl/vbl_file.h>
#include <vbl/vbl_sprintf.h>
#include <vbl/vbl_printf.h>

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
  if (range.start != -1) start_frame = range.start;
  if (range.step != -1) step = range.step;
  end_ = range.end;
  
  if (end_ == -1)
    end_ = 9999999;

  fmt_ = range.filename;
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
    size_t i = fmt_.rfind('/');
    // If no /, assume that the file is in CWD which exists
    if (i != vcl_string::npos) {
      vcl_string dir = fmt_.substr(0, i);
      if (!vbl_file::exists(dir.c_str())) {
	cerr << "SequenceFileName: ** Image directory [" << dir << "] does not exist" << endl;
	return;
	// cerr << "SequenceFileName: ** Making directory " << dir << endl;
	// if (!vbl_file::make_directory(dir.c_str())) {
	//   cerr << "SequenceFileName: ** Could not mkdir " << dir << endl;
	//   abort();
	// }
      } else 
	if (!vbl_file::is_directory(dir.c_str()))
	  cerr << "SequenceFileName: WARNING: Inferred subdir [" << dir << "]"
	       << " exists and is not already a directory\n";
    }
  }

  // If no extension, we'll need a default
  {
    vbl_reg_exp re("\\.([a-zA-Z_0-9]+)$");
    if (re.find(fmt_.c_str())) {
      cerr << "SequenceFileName: Found extension [" << re.match(1) << "]\n";
      int pointpos = re.start(0);
      ext_ = fmt_.substr(pointpos);
      fmt_.erase(pointpos, vcl_string::npos);
    }
  }

  ok_ = true;
}

vcl_string SequenceFileName::name(int frame)
{
  vbl_sprintf buf((fmt_ + ext_).c_str(), frame * step_ + start_frame_);
  return (char const*) buf;
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
  vbl_sprintf buf((fmt + extension).c_str(), frame);
  cerr << "SequenceFileName: Checking [" << buf << "]\n";
  return (vbl_file::size(buf.c_str()) > 0 || vbl_file::size((vcl_string(buf) + ".gz").c_str()) > 0);
}

ostream& SequenceFileName::print(ostream& s) const
{
  return vbl_printf(s, "[%s %d:%d:%d]", (fmt_ + ext_).c_str(), start_frame_, step_, end_);
}
