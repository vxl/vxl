//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "SequenceFileName.h"
#endif
//
// Class: SequenceFileName
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 26 Jan 99
// Modifications:
//   990126 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "SequenceFileName.h"

#include <vcl/vcl_cstdlib.h> // atoi()
#include <vcl/vcl_iostream.h>
#include <vbl/vbl_reg_exp.h>
#include <vbl/vbl_file.h>
#include <vbl/vbl_sprintf.h>
#include <vbl/vbl_printf.h>

SequenceFileName::SequenceFileName(char const* s, char const* read_or_write)
{
  int start_frame = 0;
  int step = 1;
  end_ = -1;
  
  // First match any trailing ",n[:n]:[n]" (can use ; or ,)
  vcl_string filename = s;
  vbl_reg_exp re("[,;]([0-9]+)(:[0-9]+)?:([0-9]+)?$");
  if (re.find(s)) {
    vcl_string match_start = re.match(1);
    vcl_string match_step = re.match(2);
    vcl_string match_end = re.match(3);
    
    int last = re.start(0);
    filename[last] = 0;

    vbl_printf(cerr, "SequenceFileName: %s [%s:%s:%s]  -> ",
	   filename.c_str(),
	   match_start.c_str(), match_step.c_str()+1, match_end.c_str());


    if (match_start.length() > 0)
      start_frame = atoi(match_start.c_str());

    if (match_step.length() > 0)
      step = atoi(match_step.c_str()+1);
    
    if (match_end.length() > 0)
      end_ = atoi(match_end.c_str());

    vbl_printf(cerr, "[%d:%d:%d]\n", start_frame, step, end_);
    
  } else if (strchr(s, ',')) {
    vbl_printf(cerr, "SequenceFileName: Warning: \"%s\" contains a comma, but didn't match my regexp.\n", s);
  }
  
  init(filename.c_str(), start_frame, step, read_or_write);
}

SequenceFileName::SequenceFileName(char const* s, int start_frame, int step, char const* read_or_write)
{
  end_ = -1;
  init(s, start_frame, step, read_or_write);
}

void SequenceFileName::init(char const* s, int start_frame, int step, char const* /*read_or_write*/)
{
  if (end_ == -1)
    end_ = 9999999;

  fmt_ = s;
  ext_ = "";
  start_frame_ = start_frame;
  step_ = step;

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
	cerr << "SequenceFileName: ** Need directory " << dir << endl;
	abort();
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
  vbl_reg_exp re("\\.([a-zA-Z_0-9]+)$");
  if (re.find(fmt_.c_str())) {
    cerr << "SequenceFileName: Found extension [" << re.match(1) << "]\n";
    int pointpos = re.start(0);
    ext_ = fmt_.substr(pointpos);
    fmt_.erase(pointpos, vcl_string::npos);
  }
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
