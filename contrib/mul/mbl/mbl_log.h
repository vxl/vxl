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
// my_log.log(mbl_logger::WARN) << "Infinite loop: Time wasted so far " << time() << std::endl;
// \endcode
// The std::endl (or a std::flush) is necessary to terminate the log line. The problem with this
// explicit version is that the stream insertion and call to the time() function take place,
// even if the logging level is set below WARN. Ideally, we would like unsent log messages
// to take as little time as possible - so that there is no overhead in leaving the
// log message in your code. To achieve this we can test the log level first
// \code
// if (my_log.level() >= mbl_logger::WARN)
//   my_log.log(mbl_logger::WARN) << "Infinite loop: Time wasted so far " << time() << std::endl;
// \endcode
// Of course, you should just use MBL_LOG which handles this for you. Additionally
// MBL_LOG sorts out termination of the log message without using std::endl, allowing you to send
// multi-line messages in a single log output.
//
// You can also use the logger to control the dumping of data files straight to the filesystem.
// \code
// if (my_log.level() >= mbl_logger::INFO && my_log.dump())
// {
//   std::string filename=my_log.dump_prefix()+"my_image.png";
//   if (vil_save(my_image, filename.c_str()))
//     MBL_LOG(INFO, my_log, "Saved my_image to " << filename);
//   else
//     MBL_LOG(ERR, my_log, "Failed to save my_image to " << filename);
// }
// \endcode
//
// You can find all logger names in all source code in some subtree using the following command:
// \code
//  find . -type f | fgrep -v /.svn/ | xargs perl -ne 'print if s/.*\bmbl_logger(\s+\w+)?\s*\(\"(.*)\"\).*/$2\n/;' | sort -u
// \endcode


#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>
#include <ostream>
#include <set>
#include <map>
#include <ios>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


// define MBL_LOG_DISABLE_ALL_LOGGING to stop all logging.

class mbl_logger_root;
class mbl_logger;

//: Allows stream-like syntax on logger.
class mbl_log_streambuf: public std::streambuf
{
  mbl_logger* logger_;
 public:
  mbl_log_streambuf(mbl_logger* logger): logger_(logger) {}
  int sync () override;
  int overflow (int ch) override;
  std::streamsize xsputn(const char *ptr, std::streamsize count) override;
};

//: A null streambuf ignores all input.
class mbl_log_null_streambuf: public std::streambuf
{
};


//: Base class for destinations of a logging event.
class mbl_log_output_base
{
 public:
  virtual ~mbl_log_output_base() = default;
  //: Start a new log entry, with id info.
  // Future calls to terminate_flush will be ignored.
  virtual void start_with_manual_termination(int level, const char *srcfile, int srcline)=0;
  //: Start a new log entry, with id info.
  // Future calls to terminate_flush will be honoured.
  virtual void start_with_flush_termination(int level, const char *srcfile, int srcline)=0;
  //: Add contents to the existing log entry.
  virtual void append(const char * contents, std::streamsize n_chars)=0;
  //: Finish the log entry, sent from a stream flush.
  virtual void terminate_manual()=0;
  //: Finish the log entry, sent from explicit function call, e.g. by MBL_LOG.
  virtual void terminate_flush()=0;
  //: Which logger id are we using.
  // char * for efficiency, since logger name is in executable's static_data.
  virtual const char *id()=0;
};

#if !defined MBL_LOG_DISABLE_ALL_LOGGING
//: Outputs log messages to an existing stream (e.g. std::cerr).
class mbl_log_output_stream: public mbl_log_output_base
{
  //: A pointer to the stream where logging finally gets sent.
  std::ostream* real_stream_;
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
  mbl_log_output_stream(std::ostream& real_stream, const char *id);
  //: Start a new log entry, with id info.
  // Future calls to terminate_flush will be ignored.
  void start_with_manual_termination(int level, const char *srcfile, int srcline) override;
  //: Start a new log entry, with id info.
  // Future calls to terminate_flush will be honoured.
  void start_with_flush_termination(int level, const char *srcfile, int srcline) override;
  //: Add contents to the existing log entry.
  void append(const char * contents, std::streamsize n_chars) override;
  //: Finish the log entry, sent from a stream flush.
  void terminate_manual() override;
  //: Finish the log entry, sent from explicit function call, e.g. by MBL_LOG.
  void terminate_flush() override;
  //: Which logger id are we using.
  const char *id() override {return id_;}
};

//: Outputs log messages to a named file.
class mbl_log_output_file: public mbl_log_output_base
{
  //: The file where logging finally gets sent.
  std::ofstream file_;
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
  mbl_log_output_file(const std::string &filename, const char *id);
  //: Start a new log entry, with id info.
  // Future calls to terminate_flush will be ignored.
  void start_with_manual_termination(int level, const char *srcfile, int srcline) override;
  //: Start a new log entry, with id info.
  // Future calls to terminate_flush will be honoured.
  void start_with_flush_termination(int level, const char *srcfile, int srcline) override;
  //: Add contents to the existing log entry.
  void append(const char * contents, std::streamsize n_chars) override;
  //: Finish the log entry, sent from a stream flush.
  void terminate_manual() override;
  //: Finish the log entry, sent from explicit function call, e.g. by MBL_LOG.
  void terminate_flush() override;
  //: Which logger id are we using.
  const char *id() override {return id_;}
};

#endif

//: Main user logging class - represents a category.
class mbl_logger
{
#ifdef MBL_LOG_DISABLE_ALL_LOGGING
  mbl_logger(): nullstream_(&nullbuf_) {}
  mbl_log_null_streambuf nullbuf_;
  std::ostream nullstream_;
 public:  mbl_logger(const char *id): nullstream_(&nullbuf_) {}
  int level() const { return -1000; }
  std::ostream &log(int level) { return nullstream_; }
  std::ostream &mtlog() {return nullstream_;}
  bool dump() const { return false; }
  const std::string& dump_prefix() const { return ""; }
  const std::string& timestamp() const { return ""; }
#else
  int level_;
  mbl_log_output_base *output_;
  mbl_log_streambuf streambuf_;
  std::ostream logstream_;
  std::ostream *mt_logstream_;
  //: File location to dump files.
  // If empty - don't dump files.
  std::string dump_prefix_;
  //: Time stamp format.
  // If empty - don't print time stamp.
  std::string timestamp_;
  //: Default constructor only available to root's default logger.
  mbl_logger();

  mbl_logger(const mbl_logger&) = delete; // Hide copy constructor.

  //: Update settings in light of changes to the root / configuration.
  void reinitialise();

 public:
  mbl_logger(const char *id);
  ~mbl_logger();

  //: logger will take ownership of output
  void set(int level, mbl_log_output_base* output);
  //: Higher means more output.
  int level() const { return level_; }
  std::ostream &log(int level, const char * srcfile="", int srcline=0);

  //: A log with manual event start and stop requirements.
  std::ostream &mtlog() {return *mt_logstream_;}
  void mtstart(int level, const char * srcfile="", int srcline=0);
  void mtstop();

  //: Is this logger allowed to dump data files directly to the filestore?
  bool dump() const {return !dump_prefix_.empty();}

  //: A filepath prefix that this logger should use when creating dump files.
  // Normal behaviour is to treat this as a prefix rather than a dir, so
  // "./my_dump_area/foo_" indicates the program should create files
  // that begin with "foo_" in sub-directory my_dump_area of the cwd.
  const std::string& dump_prefix() const {return dump_prefix_;}

  //: Time stamp format. Don't save timestamp if empty. Not Yet Implemented
  const std::string& timestamp() const { return timestamp_; }

#endif // MBL_LOG_DISABLE_ALL_LOGGING

  static mbl_logger_root &root();

  //: Log priority levels.
  // Based on POSIX syslog API.
  // Default level is NOTICE.
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
    std::string name;
    std::string dump_prefix;
    std::string timestamp;
    std::ostream *stream;
  };

  mbl_log_categories();

  //: Configure whole category list from a file.
  // New entries are added to any existing category details.
  void config(std::istream&, const std::map<std::string, std::ostream *>& stream_names);

  //: Make the category list empty;
  // An "empty" list still contains a root entry.
  void clear();

  //: Get the closest matching entry;
  const cat_spec& get(const std::string& category) const;

  void print(std::ostream& ss) const;

 private:
  std::map<std::string, cat_spec> cat_list_;
};


//: Singleton, keeps records of logging state.
class mbl_logger_root
{
  friend class mbl_logger;
  mbl_log_categories categories_;

#ifndef MBL_LOG_DISABLE_ALL_LOGGING
  mbl_log_null_streambuf null_streambuf_;
  std::ostream null_stream_;

  std::set<mbl_logger*> all_loggers_;

  mbl_logger_root():
    null_stream_(&null_streambuf_) {}
#endif
 public:

  //: List of category entries.
  const mbl_log_categories &categories() {return categories_;}

  typedef std::map<std::string, std::ostream *> stream_names_t;

  //:Load a default configuration file
  // This function will look for a configuration file called
  // "mbl_log.properties" in the current directory, or if no there,
  // in the user's home directory. Unix users can call it
  // ".mbl_log.properties" in their home directory.
  // See load_log_config() for description of config file format, and \p stream_names.
  void load_log_config_file(const stream_names_t &stream_names = stream_names_t());

  //: Load a configuration from a stream.
  // Each section of the text should begin with a category.
  // A logger named "A.B.C" will be controlled by a section labelled A.B.C
  // The categories labels have a hierarchical structure, so if A.B.C
  // doesn't exist, the code will look for a section labelled A.B,
  // or A. If none of these exist it will use the "root" category.
  // Each category has a level. If a log message has the same or higher
  // priority than the given level, the message will be output.
  // Finally the category destination must be specified as a
  // "file_output" and the filename or "stream_output" and the stream name.
  // The known stream names are "std::cout" and "std::cerr", and additional
  // names can be provided through \p stream_names.
  // The
  //\verbatim
  // root: { level: INFO stream_output: test }
  // obj3: { level: INFO stream_output: std::cout }\n
  // obj3.obj6: { level: INFO file_output: results.txt }\n
  // obj3.obj7.images: { level: INFO stream_output: std::cout dump_prefix: ./logging_dir/ }\n
  //\endverbatim
  // where LEVEL is an integer - setting the logging level.
  // see mbl_logger:levels for useful values.
  void load_log_config(std::istream &config, const stream_names_t &stream_names = stream_names_t());

  //: Force all loggers to update themselves in light of changes to the root and configuration.
  // This is already called automatically by load_log_config_file().
  void update_all_loggers();
};

#ifdef MBL_LOG_DISABLE_ALL_LOGGING
# define MBL_LOG(my_level, logger, message) do {} while (false)
#else
//: Log a message.
// This macro wraps up normal uses of the logger efficiently in source code-length and run-time.
// \code
// MBL_LOG(DEBUG, my_logger, "time: " << time() << "\nstatus: " << my_data);
// \endcode
// No function evaluations (e.g. of time() ) will take place unless the logger is enabled.
// The logger will also work correctly even if \c operator<<(my_data_t&) flushes the stream.
# define MBL_LOG(my_level, logger, message) \
  do { mbl_logger &rlogger = logger; \
    if (rlogger.level() >= mbl_logger:: my_level) {\
      rlogger.mtstart(mbl_logger:: my_level, __FILE__, __LINE__); \
      std::ios::fmtflags flags=rlogger.mtlog().flags(); \
      std::streamsize precision=rlogger.mtlog().precision(); \
      std::streamsize width=rlogger.mtlog().width(); \
      rlogger.mtlog() << message << std::endl; \
      rlogger.mtlog().width(width); \
      rlogger.mtlog().precision(precision); \
      rlogger.mtlog().flags(flags); \
      rlogger.mtstop(); } \
  } while (false)
#endif

#endif // mbl_log_h
