//:
// \file
// \author Ian Scott
// \date 16-Jan-2005
// \brief Flexible Status and Error Logging.
// These classes are patterned after the log4j logging library,
// although without all of the sophistication.

// n.b. We have not copied any code from log4j (or other logging libraries)
// - just the ideas.

#include "mbl_log.h"
#include <vcl_cstddef.h>



int mbl_log_streambuf::sync ()
{
  logger_->output_.start_entry();
  vcl_ptrdiff_t n = pptr() - pbase();

  if (n && logger_->output_.real_streambuf().sputn(pbase (), n) != n)
  {
    logger_->output_.stop_entry();
    return EOF;
  }
  pbump (-n);  // Reset pptr().
  logger_->output_.stop_entry();
  return 0;
}

int mbl_log_streambuf::overflow (int ch)
{
  logger_->output_.start_entry();
  long n = pptr() - pbase();
  
  if (n && logger_->output_.real_streambuf().sputn(pbase (), n) != n)
    return EOF;

  if (ch != EOF)
  {
    char cbuf = ch;
    if (logger_->output_.real_streambuf().sputn(&cbuf, 1) != 1)
      return EOF;
  }
  pbump (-n);  // Reset pptr().
  return 0;
}

vcl_streamsize mbl_log_streambuf::xsputn ( const char *ptr, vcl_streamsize nchar)
{
  logger_->output_.start_entry();
  
  // Output anything already in buffer
  long n = pptr() - pbase();
  if (n && logger_->output_.real_streambuf().sputn(pbase(), n) != n)
    return EOF;
  pbump (-n);  // Reset pptr().

  return logger_->output_.real_streambuf().sputn(ptr, nchar);
}



mbl_logger::mbl_logger(const vcl_string & id):
level_(mbl_logger::root().default_logger.level()),
  streambuf_(this),
  logstream_(&streambuf_),
  output_(mbl_logger::root().default_logger.output_)
{
  output_.id_ = id;
}
  
void mbl_logger::set(int level, const mbl_log_output& output)
{
  level_ = level;
  output_ = output;
}

mbl_logger_root &mbl_logger::root()
{
  if (!root_.get())
    root_ = vcl_auto_ptr<mbl_logger_root>(new mbl_logger_root());
  return *root_;
}

//: If it hasn't already been started, this prints out the beginning of a log entry.
void mbl_log_output::start_entry()
{
  if (has_started_) return;
  (*real_stream_)<< id_ << ' ';
  has_started_ = true;
}
//: If it hasn't already been stopped, this prints out the end of a log entry.
void mbl_log_output::stop_entry()
{
  has_started_ = false;
}

vcl_auto_ptr<VCL_DISAPPEARING_TYPENAME mbl_logger_root >  mbl_logger::root_ = 
  vcl_auto_ptr<VCL_DISAPPEARING_TYPENAME mbl_logger_root >(0);
