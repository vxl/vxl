#ifndef mbl_log_h
#define mbl_log_h
//:
// \file
// \author Ian Scott
// \date 16-Jan-2005
// \brief Flexible and efficient status and error logging.
//
// Good log statements are left in the code, and switched on or off at runtime.
// To make this feasible, the decision about whether to send a log message
// should be as efficient as possible.
// The runtime cost of a switched off mbl_log statement will be no more than an
// integer comparison, if you have a decent optimising compiler.
//
// These classes are patterned after the log4j logging library,
// although without all of the sophistication. (We have not copied any
// code from log4j (or other logging libraries) - just the ideas.)
//
// To send a message to the log you need to do two things - create a logger object, and
// send a message to it.
// By creating a logger, you identify a category of logging events. You should create
// a separate category for each module of your program. However, creating loggers may not
// be cheap - so it is best not to do it too often.
// \code
// mbl_logger my_log("imorphics.easy_stuff.does_halt");
// \endcode
// The easiest way to send a log message looks like:
// \code
// MBL_LOG(WARN, my_log, "Infinite loop: Time wasted so far " << time() );
// \endcode
// The system will have been setup to assign a log level to a particular category.
// The log levels are EMERG < ALERT < CRIT < ERR < WARN < NOTICE < INFO < DEBUG. The
// above log message will be sent if the level for category "imorphics.easy_stuff.does_halt"
// is set to WARN or higher.
// You can avoid using macros by calling the logger directly
// \code
// my_log.log(mbl_logger::WARN) << "Infinite loop: Time wasted so far " << time() << vcl_endl;
// \endcode
// The vcl_endl (or a vcl_flush) is necessary to terminate the log line. The problem with this
// explicit version is that the stream insertion and call to the time() function take place,
// even if the logging level is set below WARN. Ideally, we would like unsent log messages
// to take as little time as possible - so that there is no overhead in leaving the
// log message in your code. To achieve this we can test the log level first
// \code
// if (my_log.level() < mbl_logger::WARN)
//   my_log.log(mbl_logger::WARN) << "Infinite loop: Time wasted so far " << time() << vcl_endl;
// \endcode
// Of course, you should just use MBL_LOG which handles this for you. Additionally
// MBL_LOG sorts out termination of the log message without using vcl_endl, allowing you to send
// multi-line messages in a single log output.

#include <vcl_fstream.h>
#include <vcl_streambuf.h>
#include <vcl_memory.h>
#include <vcl_string.h>
#include <vcl_ostream.h>
#include <vcl_set.h>
#include <vcl_map.h>

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

//: A null streambuf ignores all input.
class mbl_log_null_streambuf: public vcl_streambuf
{
};


//: Base class for destinations of a logging event.
class mbl_log_output_base
{
 public:
  virtual ~mbl_log_output_base(){};
  //: Start a new log entry, with id info.
  // Future calls to terminate_flush will be ignored.
  virtual void start_with_manual_termination(int level, const char *srcfile, int srcline)=0;
  //: Start a new log entry, with id info.
  // Future calls to terminate_flush will be honoured.
  virtual void start_with_flush_termination(int level, const char *srcfile, int srcline)=0;
  //: Add contents to the existing log entry.
  virtual void append(const char * contents, vcl_streamsize n_chars)=0;
  //: Finish the log entry, sent from a stream flush.
  virtual void terminate_manual()=0;
  //: Finish the log entry, sent from explicit function call, e.g. by MBL_LOG.
  virtual void terminate_flush()=0;
  //: Which logger id are we using.
  virtual const char *id()=0;
};

//: Outputs log messages to an existing stream (e.g. vcl_cerr).
class mbl_log_output_stream: public mbl_log_output_base
{
  //: A pointer to the stream where logging finally gets sent.
  vcl_ostream* real_stream_;
  //: logger identity
  const char * id_;
  //: true if a log entry is in progress.
  bool has_started_;
  // Start a new log entry. Pass id info in via append()
  void start();
  //: Ignore calls to terminate_flush.
  // The current log message should be manually terminated.
  bool ignore_flush_;
 public:
  mbl_log_output_stream(vcl_ostream& real_stream, const char *id);
  //: Start a new log entry, with id info.
  // Future calls to terminate_flush will be ignored.
  virtual void start_with_manual_termination(int level, const char *srcfile, int srcline);
  //: Start a new log entry, with id info.
  // Future calls to terminate_flush will be honoured.
  virtual void start_with_flush_termination(int level, const char *srcfile, int srcline);
  //: Add contents to the existing log entry.
  virtual void append(const char * contents, vcl_streamsize n_chars);
  //: Finish the log entry, sent from a stream flush.
  virtual void terminate_manual();
  //: Finish the log entry, sent from explicit function call, e.g. by MBL_LOG.
  virtual void terminate_flush();
  //: Which logger id are we using.
  virtual const char *id() {return id_;}
};

//: Outputs log messages to a named file.
class mbl_log_output_file: public mbl_log_output_base
{
  //: The file where logging finally gets sent.
  vcl_ofstream file_;
  //: logger identity
  const char * id_;
  //: true if a log entry is in progress.
  bool has_started_;
  //; Start a new log entry. Pass id info in via append()
  void start();
  //: Ignore calls to terminate_flush.
  // The current log message should be manually terminated.
  bool ignore_flush_;
 public:
  mbl_log_output_file(const vcl_string &filename, const char *id);
  //: Start a new log entry, with id info.
  // Future calls to terminate_flush will be ignored.
  virtual void start_with_manual_termination(int level, const char *srcfile, int srcline);
  //: Start a new log entry, with id info.
  // Future calls to terminate_flush will be honoured.
  virtual void start_with_flush_termination(int level, const char *srcfile, int srcline);
  //: Add contents to the existing log entry.
  virtual void append(const char * contents, vcl_streamsize n_chars);
  //: Finish the log entry, sent from a stream flush.
  virtual void terminate_manual();
  //: Finish the log entry, sent from explicit function call, e.g. by MBL_LOG.
  virtual void terminate_flush();
  //: Which logger id are we using.
  virtual const char *id() {return id_;}
};


//: Main user logging class - represents a category.
class mbl_logger
{
  int level_;
  mbl_log_output_base *output_;
  mbl_log_streambuf streambuf_;
  vcl_ostream logstream_;
  vcl_ostream *mt_logstream_;

  //: Default constructor only available to root's default logger.
  mbl_logger();

  mbl_logger(const mbl_logger&); // Hide copy constructor.

  //: Update settings in light of changes to the root / configuration.
  void reinitialise();

 public:
  static mbl_logger_root &root();
  mbl_logger(const char *id);
  ~mbl_logger();

  //: logger will take ownership of output
  void set(int level, mbl_log_output_base* output);
  //: Higher means more output.
  int level() { return level_; }
  vcl_ostream &log(int level, const char * srcfile="", int srcline=0);

  //: A log with manual event start and stop requirements.
  vcl_ostream &mtlog() {return *mt_logstream_;}
  void mtstart(int level, const char * srcfile="", int srcline=0);
  void mtstop();

  //: Log priority levels.
  // Based on POSIX syslog API
  enum levels
  {
    NONE=-100, EMERG=0, ALERT=100, CRIT=200, ERR=300, WARN=400,
    NOTICE=500, INFO=600, DEBUG=700, ALL=800
  };
  friend class mbl_log_streambuf;
  friend class mbl_logger_root;
};


//: This class handles category lists.
class mbl_log_categories
{
 public:
  struct cat_spec
  {
    int level;
    enum output_type {FILE_OUT, NAMED_STREAM} output;
    vcl_string name;
    vcl_ostream *stream;
  };

  mbl_log_categories();

  //: Configure whole category list from a file.
  // New entries are added to any existing category details.
  void config(vcl_istream&, const vcl_map<vcl_string, vcl_ostream *>& stream_names);

  //: Make the category list empty;
  // An "empty" list still contains a root entry.
  void clear();

  //: Get the closest matching entry;
  const cat_spec& get(const vcl_string& category) const;

  void print(vcl_ostream& ss) const;

 private:
  vcl_map<vcl_string, cat_spec> cat_list_;
};


//: Singleton, keeps records of logging state.
class mbl_logger_root
{
  friend class mbl_logger;

  mbl_log_categories categories_;

  mbl_log_null_streambuf null_streambuf_;
  vcl_ostream null_stream_;

  vcl_set<mbl_logger*> all_loggers_;

  mbl_logger_root():
    null_stream_(&null_streambuf_) {}

 public:

  //: List of category entries.
  const mbl_log_categories &categories() {return categories_;}

  typedef vcl_map<vcl_string, vcl_ostream *> stream_names_t;

  //:Load a default configuration file
  // This function will look for a configuration file called
  // "mbl_log.properties" in the current directory, or if no there,
  // in the user's home directory. Unix users can call it
  // ".mbl_log.properties" in their home directory.
  // Each section of the file should begin with a category.
  // A logger named "A.B.C" will be controlled by a section labelled A.B.C
  // The categories labels have a hierarchical structure, so if A.B.C
  // doesn't exist, the code will look for a section labelled A.B,
  // or A. If none of these exist it will use the "root" category.
  // Each category has a level. If a log message has the same or higher
  // priority than the given level, the message will be output.
  // Finally the category desination must be specified as a
  // "file_output" and the filename or "stream_output" and the stream name.
  // The known stream names are "vcl_cout" and "vcl_cerr", and additional
  // names can be provided through \p stream_names.
  //\verbatim
  // root: { level: INFO stream_output: test }
  // obj3: { level: INFO stream_output: vcl_cout }\n
  // obj3.obj6: { level: INFO file_output: results.txt }\n
  //\endverbatim
  // where LEVEL is an integer - setting the logging level.
  // see mbl_logger:levels for useful values.
  void load_log_config_file(const stream_names_t &stream_names = stream_names_t());
  //: Force all loggers to update themselves in light of changes to the root and configuration.
  // This is already called automatically by load_log_config_file().
  void update_all_loggers();
};


#define MBL_LOG(my_level, logger, message) \
  if (logger.level() >= mbl_logger:: my_level) {\
    logger.mtstart(mbl_logger:: my_level, __FILE__, __LINE__); \
    logger.mtlog() << message << vcl_endl; \
    logger.mtstop(); }


#endif // mbl_log_h
