#ifndef mbl_log_h
#define mbl_log_h

//:
// \file
// \author Ian Scott
// \date 16-Jan-2005
// \brief Flexible and efficient status and error logging.
// Good log statements are left in the code, and switched on or off at runtime.
// To make this feasible, the decision about whther to send a log message
// should be as efficient as possible.
// The runtime cost of a switched off mbl_log statement will be no more that a
// integer comparison, if you have a decent optimising compiler.
//
// These classes are patterned after the log4j logging library,
// although without all of the sophistication. (We have not copied any
// code from log4j (or other logging libraries
// - just the ideas.)
//
// This code hasn't been finished yet and will likely be subject to
// significant API changes.
// TODO: Categories, Passing more info to output, output format selection, config file.
//
// 
// To send a message to the log you need to do two things - create a logger object, and
// send a message to it.
// By creating a logger, you identify a category of logging events. You should create
// a separate category for each module of your program. However, creating loggers may not
// be cheap - so it is best not to do it too often.
// \verbatim
// mbl_logger my_log("imorphics.easy_stuff.does_halt");
// \endverbatim
// The easiest way to send a log message looks like:
// \verbatim
// MBL_LOG(WARN, my_log, "Infinite loop: Time wasted so far " << time() );
// \endverbatim
// The system will have been setup to assign a log level to a particular category.
// The log levels are EMERG < ALERT < CRIT < ERR < WARN < NOTICE< INFO < DEBUG. The
// above log message will be sent if the level for category "imorphics.easy_stuff.does_halt"
// is set to WARN or higher.
// You can avoid using macros by calling the logger directly
// \verbatim
// my_log.log(mbl_logger::WARN) << "Infinite loop: Time wasted so far " << time() << vcl_endl;
// \endverbatim
// The vcl_endl (or a vcl_flush) is necessary to terminate the log line. The problem with this
// explicit version is that the stream insertion and call to the time() function take place,
// even if the logging level is set below WARN. Ideally, we would like unsent log messages
// to take as little time as poosible - so that there is no overhead in leaving the
// log message in your code. To achieve this we can test the log level first
// \verbatim
// if (my_log.level() < mbl_logger::WARN
//   my_log.log(mbl_logger::WARN) << "Infinite loop: Time wasted so far " << time() << vcl_endl;
// \endverbatim
// Of course, this is exactly what the MBL_LOG macro does.

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


//: Allows stream-like syntax on logger.
class mbl_log_null_streambuf: public vcl_streambuf
{
public:
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
  //: Information about the current logging event.
  int next_level_;
  const char * next_srcfile_;
  int next_srcline_;

public:
  mbl_log_output(vcl_ostream* real_stream, const vcl_string &id):
    real_stream_(real_stream), has_started_(false), id_(id),
    next_level_(1000) {}

  //: The stream buffer to which logging info really gets sent.
  vcl_streambuf & real_streambuf() { return *real_stream_->rdbuf(); }
  void set_next_event_info(int level, const char *srcfile, int srcline);

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
  vcl_ostream &log(int level, const char * srcfile="", int srcline=0);

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
  mbl_log_null_streambuf null_streambuf_;
  vcl_ostream null_stream_;
  mbl_logger_root():
    null_stream_(&null_streambuf_) {}
  
public:
  mbl_logger default_logger;

};


#define MBL_LOG(my_level, logger, message) \
  if (logger.level() >= mbl_logger:: my_level) \
    logger.log(mbl_logger:: my_level, __FILE__, __LINE__) << message << vcl_endl




#endif // mbl_log_h
