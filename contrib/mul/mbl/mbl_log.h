#ifndef mbl_log_h
#define mbl_log_h

//:
// \file
// \author Ian Scott
// \date 16-Jan-2005
// \brief Flexible Status and Error Logging.
// These classes are patterned after the log4j logging library,
// although without all of the sophistication. (We have not copied any
// code from log4j (or other logging libraries
// - just the ideas.)
//
// This code hasn't been finished yet and will likely be subject to
// significant API changes.
// TODO: Categories, Passing more info to output, output format selection, config file.


#include <vcl_iostream.h>
#include <vcl_streambuf.h>
#include <vcl_memory.h>
#include <vcl_string.h>

class mbl_logger_root;
class mbl_logger;

//: Allows stream-like syntax on logger.
class mbl_log_streambuf: public vcl_streambuf
{
  mbl_logger* logger_;
public:
  mbl_log_streambuf(mbl_logger* logger): logger_(logger) {}
  virtual int sync ();
  virtual int overflow (int ch);
  virtual vcl_streamsize xsputn(const char *ptr, vcl_streamsize count);
};


//: Handles final output of a logging event.
class mbl_log_output
{
  //: A pointer to the stream where logging finally gets sent.
  vcl_ostream* real_stream_;
  //: true if a log entry is in progress.
  bool has_started_;
  //: Information from the logger object;
  vcl_string id_;
public:
  mbl_log_output(vcl_ostream* real_stream, const vcl_string &id):
    real_stream_(real_stream), has_started_(false), id_(id) {}

  //: The stream buffer to which logging info really gets sent.
  vcl_streambuf & real_streambuf() { return *real_stream_->rdbuf(); }
  //: If it hasn't already been started, this prints out the beginning of a log entry.
  void start_entry();
  //: If it hasn't already been stopped, this prints out the end of a log entry.
  void stop_entry();
  friend class mbl_logger;
};

//: Main user logging class - represents a category.
class mbl_logger
{

  int level_;
  mbl_log_output output_;
  mbl_log_streambuf streambuf_;
  vcl_ostream logstream_;

  //: Default constructor only availabel to root's default logger.
  mbl_logger():
    level_(ALL),
    output_(&vcl_cerr, ""),
    streambuf_(this),
    logstream_(&streambuf_)
  {}
  static vcl_auto_ptr<mbl_logger_root> root_;

public:
  static mbl_logger_root &root();
  mbl_logger(const vcl_string & id);

  void set(int level, const mbl_log_output&);
  //: Higher means more output.
  int level() { return level_; }
  vcl_ostream &log() { return logstream_; }

  //: Log priority levels.
  // Based on POSIX syslog API
  enum levels
  {
    EMERG=0, ALERT=100, CRIT=200, ERR=300, WARN=400,
    NOTICE=500, INFO=600, DEBUG=700, ALL=800
  };
  friend class mbl_log_streambuf;
  friend class mbl_logger_root;
};



//: Singleton, keeps records of logging state.
class mbl_logger_root
{
  friend class mbl_logger;
  mbl_logger_root() {}
public:

  mbl_logger default_logger;

};

#endif // mbl_log_h
