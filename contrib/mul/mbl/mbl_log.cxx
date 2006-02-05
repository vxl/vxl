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
#include <vcl_fstream.h>

// Got the a streambuf example from vul_redirector and hacked around with it.
// It passes on all stuff direct to the real_streambuf, whilst calling
// extra functions on the output object to print message headers and footers.


int mbl_log_streambuf::sync()
{
  logger_->output_.start_entry();
  vcl_ptrdiff_t n = pptr() - pbase();

  if (n && logger_->output_.real_streambuf().sputn(pbase(), n) != n)
  {
    logger_->output_.stop_entry();
    return EOF;
  }
  if (logger_->output_.real_streambuf().pubsync())
  {
    logger_->output_.stop_entry();
    return EOF;
  }

  pbump(-n);  // Reset pptr().
  logger_->output_.stop_entry();
  return 0;
}

int mbl_log_streambuf::overflow(int ch)
{
  logger_->output_.start_entry();
  long n = pptr() - pbase();

  if (n && logger_->output_.real_streambuf().sputn(pbase(), n) != n)
    return EOF;

  if (ch != EOF)
  {
    char cbuf = ch;
    if (logger_->output_.real_streambuf().sputn(&cbuf, 1) != 1)
      return EOF;
  }
  pbump(-n);  // Reset pptr().
  return 0;
}

vcl_streamsize mbl_log_streambuf::xsputn( const char *ptr, vcl_streamsize nchar)
{
  logger_->output_.start_entry();

  // Output anything already in buffer
  long n = pptr() - pbase();
  if (n && logger_->output_.real_streambuf().sputn(pbase(), n) != n)
    return EOF;
  pbump(-n);  // Reset pptr().

  return logger_->output_.real_streambuf().sputn(ptr, nchar);
}

//: Default constructor only available to root's default logger.
mbl_logger::mbl_logger():
  level_(WARN),
  output_(&vcl_cerr, ""),
  streambuf_(this),
  logstream_(&streambuf_),
  mt_logstream_(&logstream_)
{
  // This will have to change to support proper hierarchical control over categories.
  logstream_.tie(output_.real_stream_);
  // Don't insert default root logger - this would cause infinite recursion.
}

mbl_logger::mbl_logger(const vcl_string & id):
  level_(mbl_logger::root().default_logger.level()),
  output_(mbl_logger::root().default_logger.output_),
  streambuf_(this),
  logstream_(&streambuf_),
  mt_logstream_(&logstream_)
{
  // This will have to change to support proper hierarchical control over categories.
  output_.id_ = id;
  logstream_.tie(output_.real_stream_);
  root().all_loggers_.insert(this);
}

void mbl_logger::reinitialise()
{
  // This will have to change to support proper hierarchical control over categories.
  level_ = mbl_logger::root().default_logger.level();
  output_ = mbl_logger::root().default_logger.output_;
}

void mbl_logger::set(int level, const mbl_log_output& output)
{
  level_ = level;
  output_ = output;
  logstream_.tie(output_.real_stream_);
}


vcl_ostream &mbl_logger::log(int level, const char * srcfile, int srcline)
{
  if (level_ < level)
    return root().null_stream_;
  output_.set_next_event_info(level, srcfile, srcline);
  return logstream_;
}

void mbl_logger::mtstart(int level, const char * srcfile, int srcline)
{
  if (level_ < level)
  {
    mt_logstream_ = &root().null_stream_;
    return;
  }
  mt_logstream_ = &logstream_;
  output_.set_mt_next_event_info(level, srcfile, srcline);
}

void mbl_logger::mtstop()
{
  logstream_.flush();
  output_.mt_stop_entry();
}


mbl_logger_root &mbl_logger::root()
{
  static vcl_auto_ptr<mbl_logger_root> root_;

  if (!root_.get())
    root_ = vcl_auto_ptr<mbl_logger_root>(new mbl_logger_root());
  return *root_;
}


//:Load a default configuration file
// Current Format is
//\verbatim
//LEVEL
//\end verbatim
// where LEVEL is an integer - setting the logging level.
// see mbl_logger:levels for useful values.
void mbl_logger_root::load_log_config_file()
{
  vcl_ifstream config("mbl_log.properties");
  if (!config.is_open())
    config.open("~/mbl_log.properties");
  if (!config.is_open())
    config.open("~/.mbl_log.properties");
  if (!config.is_open())
    config.open("~/.mbl_log.properties");
  if (!config.is_open())
    config.open("C:\\mbl_log.properties");

  if (config.is_open())
  {
    int level;
    config >> level;
    default_logger.set(level, default_logger.output_);
  }

  update_all_loggers();
}

// Make sure all known loggers reinitialise themselves.
void mbl_logger_root::update_all_loggers()
{
  for (vcl_set<mbl_logger *>::iterator it=all_loggers_.begin(),
       end=all_loggers_.end(); it!=end; ++it)
    (*it)->reinitialise();
}

void mbl_log_output::set_next_event_info(int level, const char *srcfile, int srcline)
{
  if (has_started_) return;
  next_level_ = level;
  next_srcfile_ = srcfile;
  next_srcline_ = srcline;
  manual_termination_=false;
}

void mbl_log_output::set_mt_next_event_info(int level, const char *srcfile, int srcline)
{
  next_level_ = level;
  next_srcfile_ = srcfile;
  next_srcline_ = srcline;
  manual_termination_=true;
  has_started_=true;
  print_header();
}

void mbl_log_output::print_header()
{
  switch (next_level_)
  {
  case mbl_logger::EMERG:
    (*real_stream_)<< "EMERG: ";
    break;
  case mbl_logger::ALERT:
    (*real_stream_)<< "ALERT: ";
    break;
  case mbl_logger::CRIT:
    (*real_stream_)<< "CRIT: ";
    break;
  case mbl_logger::ERR:
    (*real_stream_)<< "ERR: ";
    break;
  case mbl_logger::WARN:
    (*real_stream_)<< "WARN: ";
    break;
  case mbl_logger::NOTICE:
    (*real_stream_)<< "NOTICE: ";
    break;
  case mbl_logger::INFO:
    (*real_stream_)<< "INFO: ";
    break;
  case mbl_logger::DEBUG:
    (*real_stream_)<< "DEBUG: ";
    break;
  default:
    (*real_stream_)<< "LOG" << next_level_ << ' ';
    break;
  }
  (*real_stream_)<< id_ << ' ';
}

//: If it hasn't already been started, this prints out the beginning of a log entry.
void mbl_log_output::start_entry()
{
  if (has_started_) return;
  print_header();
  has_started_ = true;

  //reset level indicator as a subtle indicator of log system error.
  next_level_=1000;
}

//: If it hasn't already been stopped, this prints out the end of a log entry.
void mbl_log_output::stop_entry()
{
  if (!manual_termination_)
    has_started_ = false;
}

//: If it hasn't already been stopped, this prints out the end of a log entry.
void mbl_log_output::mt_stop_entry()
{
  has_started_ = false;
}

