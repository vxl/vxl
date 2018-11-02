//:
// \file
// \author Ian Scott
// \date 16-Jan-2005
// \brief Flexible Status and Error Logging.
// These classes are patterned after the log4j logging library,
// although without all of the sophistication.

// n.b. We have not copied any code from log4j (or other logging libraries)
// - just the ideas.

#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <algorithm>
#include <sstream>
#include <utility>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_string.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include "mbl_log.h"

std::ostream& operator<<(std::ostream&os, mbl_logger::levels level)
{
  switch (level)
  {
   case mbl_logger::NONE:
    os << "NONE";
    break;
   case mbl_logger::EMERG:
    os << "EMERG";
    break;
   case mbl_logger::ALERT:
    os << "ALERT";
    break;
   case mbl_logger::CRIT:
    os << "CRIT";
    break;
   case mbl_logger::ERR:
    os << "ERR";
    break;
   case mbl_logger::WARN:
    os << "WARN";
    break;
   case mbl_logger::NOTICE:
    os << "NOTICE";
    break;
   case mbl_logger::INFO:
    os << "INFO";
    break;
   case mbl_logger::DEBUG:
    os << "DEBUG";
    break;
   case mbl_logger::ALL:
    os << "ALL";
    break;
   default:
    os << "LOG" << level;
    break;
  }
  return os;
}


std::ostream& operator<<(std::ostream&os, const mbl_log_categories::cat_spec& spec)
{
  os << "{ level: " << static_cast<mbl_logger::levels>(spec.level);
  switch (spec.output)
  {
   case mbl_log_categories::cat_spec::FILE_OUT:
    os << " file_output: " << spec.name;
    break;
   case mbl_log_categories::cat_spec::NAMED_STREAM:
    os << " stream_output: " << spec.name;
    break;
   default:
    assert(!"This should not happen: invalid spec.output");
    break;
  }
  if (!spec.dump_prefix.empty())
    os << " dump_prefix: " << spec.dump_prefix;

  os << " }";
  return os;
}

//Notes - use two different stream bufs to hendle the mt_log() and the log().
// one should response to flushes with a terminate - the other not.


// Got the a streambuf example from vul_redirector and hacked around with it.
// It passes on all stuff direct to the real_streambuf, whilst calling
// extra functions on the output object to print message headers and footers.


int mbl_log_streambuf::sync()
{
#ifndef MBL_LOG_DISABLE_ALL_LOGGING

  int n = static_cast<int>(pptr() - pbase()); // has to be int because pbump only takes int

  if (n)
    logger_->output_->append(pbase(), n);
  logger_->output_->terminate_flush();

  pbump(-n);  // Reset pptr().
#endif
  return 0;
}

int mbl_log_streambuf::overflow(int ch)
{
#ifndef MBL_LOG_DISABLE_ALL_LOGGING
  int n = static_cast<int>(pptr() - pbase()); // has to be int because pbump only takes int

  if (n)
    logger_->output_->append(pbase(), n);
  pbump(-n);  // Reset pptr().

  if (ch == EOF)
    return 0;

  char cbuf = (char)ch;
  logger_->output_->append(&cbuf, 1);
  return ch;
#else
  return EOF;
#endif
}

std::streamsize mbl_log_streambuf::xsputn( const char *ptr, std::streamsize nchar)
{
#ifndef MBL_LOG_DISABLE_ALL_LOGGING

  // Output anything already in buffer
  int n = static_cast<int>(pptr() - pbase()); // has to be int because pbump only takes int
  if (n)
    logger_->output_->append(pbase(), n);
  pbump(-n);  // Reset pptr().

  logger_->output_->append(ptr, nchar);
  return nchar;
#else
  return 0;
#endif
}

#ifndef MBL_LOG_DISABLE_ALL_LOGGING

//: Default constructor only available to root's default logger.
mbl_logger::mbl_logger():
  level_(NOTICE),
  output_(new mbl_log_output_stream(std::cerr, "")),
  streambuf_(this),
  logstream_(&streambuf_),
  mt_logstream_(&logstream_)
{
  // This will have to change to support proper hierarchical control over categories.
//  logstream_.tie(output_.real_stream_);
  // Don't insert default root logger - this would cause infinite recursion.
  root().all_loggers_.insert(this);
}

mbl_logger::~mbl_logger()
{
  root().all_loggers_.erase(this);
  delete output_;
}

mbl_log_output_stream::mbl_log_output_stream(std::ostream& real_stream, const char *id):
  real_stream_(&real_stream), id_(id), has_started_(false)
{}

//: Start a new log entry, with id info.
void mbl_log_output_stream::start()
{
  // Deal with unfinished log message
  if (has_started_)
    (*real_stream_) << "LOG_MESSAGE_TERMINATED_EARLY\n";

  // Avoid interspersed output.
  real_stream_->flush();

  has_started_=true;
}

//: Start a new log entry, with id info.
// Future calls to terminate_flush will be ignored.
void mbl_log_output_stream::start_with_manual_termination(int level, const char * /*srcfile*/, int  /*srcline*/)
{
  ignore_flush_=true;
  start();
  (*real_stream_) << static_cast<mbl_logger::levels>(level) << ": " << id_ << ' ';
}

//: Start a new log entry, with id info.
// Future calls to terminate_flush will be honoured.
void mbl_log_output_stream::start_with_flush_termination(int level, const char * /*srcfile*/, int  /*srcline*/)
{
  ignore_flush_=false;
  start();
  (*real_stream_) << static_cast<mbl_logger::levels>(level) << ": " << id_ << ' ';
}
//: add contents to the existing log entry.
void mbl_log_output_stream::append(const char * contents, std::streamsize n_chars)
{
  // Deal with unfinished log message
  if (!has_started_)
  {
    (*real_stream_) << "UNKNOWN_START_LOG: " << id_ << ' ';
    has_started_ = true;
    ignore_flush_ = true;
  }

  real_stream_->rdbuf()->sputn(contents, n_chars);
}

//: Finish the log entry, sent from a stream flush.
void mbl_log_output_stream::terminate_manual()
{
  real_stream_->flush();
  has_started_=false;
}

//: Finish the log entry, sent from explicit function call, e.g. by MBL_LOG.
void mbl_log_output_stream::terminate_flush()
{
  if (!ignore_flush_)
  {
    real_stream_->flush();
    has_started_=false;
  }
}

mbl_log_output_file::mbl_log_output_file(const std::string &filename, const char *id):
  file_(filename.c_str(), std::ios::app), id_(id), has_started_(false)
{}

//: Start a new log entry, with id info.
void mbl_log_output_file::start()
{
  // Deal with unfinished log message
  if (has_started_)
    file_ << "LOG_MESSAGE_TERMINATED_EARLY\n";

  // Avoid interspersed output.
  file_.flush();

  has_started_=true;
}

//: Start a new log entry, with id info.
// Future calls to terminate_flush will be ignored.
void mbl_log_output_file::start_with_manual_termination(int level, const char * /*srcfile*/, int  /*srcline*/)
{
  ignore_flush_=true;
  start();
  file_ << static_cast<mbl_logger::levels>(level) << ": " << id_ << ' ';
}

//: Start a new log entry, with id info.
// Future calls to terminate_flush will be honoured.
void mbl_log_output_file::start_with_flush_termination(int level, const char * /*srcfile*/, int  /*srcline*/)
{
  ignore_flush_=false;
  start();
  file_ << static_cast<mbl_logger::levels>(level) << ": " << id_ << ' ';
}

//: add contents to the existing log entry.
void mbl_log_output_file::append(const char * contents, std::streamsize n_chars)
{
  // Deal with unstarted log message
  if (!has_started_)
  {
    file_ << "UNKNOWN_START_LOG: " << id_ << ' ';
    has_started_=true;
    ignore_flush_=false;
  }

  file_.rdbuf()->sputn(contents, n_chars);
}

//: Finish the log entry, sent from a stream flush.
void mbl_log_output_file::terminate_manual()
{
  file_.flush();
  has_started_=false;
}

//: Finish the log entry, sent from explicit function call, e.g. by MBL_LOG.
void mbl_log_output_file::terminate_flush()
{
  if (!ignore_flush_)
  {
    file_.flush();
    has_started_=false;
  }
}

#if 0 // This logger causes gcc to throw a recursive_init
      // Not entirely surprising, by MSVC didn't complain.
      // FIXME This logger is useful - for finding every logger in use
      // in a program
static mbl_logger& local_logger()
{
  static mbl_logger l("mul.mbl.log");
  return l;
}
#endif

mbl_logger::mbl_logger(const char *id):
  output_(nullptr),
  streambuf_(this),
  logstream_(&streambuf_),
  mt_logstream_(&logstream_)
{
#if 0 // FIXME
  MBL_LOG(INFO, local_logger(), "Creating logger: " << id);
#endif
  const mbl_log_categories::cat_spec &cat =
    mbl_logger::root().categories().get(id);
#if 0 // FIXME
  MBL_LOG(DEBUG, local_logger(), "Using cat_spec: " << cat);
#endif

  level_ = cat.level;
  dump_prefix_ = cat.dump_prefix;

  if (cat.output == mbl_log_categories::cat_spec::NAMED_STREAM)
  {
    output_ = new mbl_log_output_stream(*cat.stream, id);
//    logstream_.tie(output_.real_stream_);
  }
  else if (cat.output == mbl_log_categories::cat_spec::FILE_OUT)
  {
    output_ = new mbl_log_output_file(cat.name, id);
//    logstream_.tie(output_.real_stream_);
  }

  root().all_loggers_.insert(this);
}

void mbl_logger::reinitialise()
{
  const char *id = output_->id();
  const mbl_log_categories::cat_spec &cat =
    mbl_logger::root().categories().get(id);

  level_ = cat.level;
  dump_prefix_ = cat.dump_prefix;

  if (cat.output == mbl_log_categories::cat_spec::NAMED_STREAM)
  {
    delete output_;
    output_ = new mbl_log_output_stream(*cat.stream, id);
//    logstream_.tie(output_.real_stream_);
  }
  else if (cat.output == mbl_log_categories::cat_spec::FILE_OUT)
  {
    delete output_;
    output_ = new mbl_log_output_file(cat.name, id);
//    logstream_.tie(output_.real_stream_);
  }
}

void mbl_logger::set(int level, mbl_log_output_base* output)
{
  level_ = level;
  delete output_;
  output_ = output;
//  logstream_.tie(output_.real_stream_);
}

std::ostream &mbl_logger::log(int level, const char * srcfile, int srcline)
{
  if (level_ < level)
    return root().null_stream_;
  output_->start_with_flush_termination(level, srcfile, srcline);
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
  output_->start_with_manual_termination(level, srcfile, srcline);
}

void mbl_logger::mtstop()
{
  logstream_.flush();
  output_->terminate_manual();
}
#endif


mbl_logger_root &mbl_logger::root()
{
  static std::unique_ptr<mbl_logger_root> root_;

  if (!root_.get())
    root_ = std::unique_ptr<mbl_logger_root>(new mbl_logger_root());
  return *root_;
}


//:Load a default configuration file
// Current Format is
// \verbatim
//LEVEL
// \endverbatim
// where LEVEL is an integer - setting the logging level.
// see mbl_logger:levels for useful values.
void mbl_logger_root::load_log_config_file(
  const std::map<std::string, std::ostream *> &stream_names)
{
#ifndef MBL_LOG_DISABLE_ALL_LOGGING
  // Make sure this list of mbl_log.properties locations code
  // stays in sync with mul/contrib/tools/print_mbl_log_properties.cxx
  std::ifstream config_file("mbl_log.properties");
  if (!config_file.is_open())
    config_file.open("~/mbl_log.properties");
  if (!config_file.is_open())
    config_file.open("~/.mbl_log.properties");
  if (!config_file.is_open())
  {
    std::string home1("${HOME}/mbl_log.properties");
    std::string home2("${HOME}/.mbl_log.properties");
    std::string home3("${HOMESHARE}/mbl_log.properties");
    std::string home4("${HOMEDRIVE}${HOMEDIR}/mbl_log.properties");
    std::string home5("${HOMEDRIVE}${HOMEPATH}/mbl_log.properties");
    std::string home6("${USERPROFILE}/mbl_log.properties");
    if (vul_string_expand_var(home1))
      config_file.open(home1.c_str());
    if (!config_file.is_open() && vul_string_expand_var(home2))
      config_file.open(home2.c_str());
    if (!config_file.is_open() && vul_string_expand_var(home3))
      config_file.open(home3.c_str());
    if (!config_file.is_open() && vul_string_expand_var(home4))
      config_file.open(home4.c_str());
    if (!config_file.is_open() && vul_string_expand_var(home5))
      config_file.open(home5.c_str());
    if (!config_file.is_open() && vul_string_expand_var(home6))
      config_file.open(home6.c_str());
  }
  if (!config_file.is_open())
    config_file.open("C:\\mbl_log.properties");

  if (!config_file.is_open())
  {
    std::cerr << "WARNING: No mbl_log.properties file found.\n";
    return;
  }

  config_file.clear(); // May have been set to fail on failed open.
  load_log_config(config_file, stream_names);
#endif
}


//:Load a default configuration file
// Current Format is
// \verbatim
//LEVEL
// \endverbatim
// where LEVEL is an integer - setting the logging level.
// see mbl_logger:levels for useful values.
void mbl_logger_root::load_log_config(std::istream& is,
                                      const std::map<std::string, std::ostream *> &stream_names)
{
#ifndef MBL_LOG_DISABLE_ALL_LOGGING
  categories_.config(is, stream_names);
  update_all_loggers();
#endif
}

// Make sure all known loggers reinitialise themselves.
void mbl_logger_root::update_all_loggers()
{
#ifndef MBL_LOG_DISABLE_ALL_LOGGING
  for (auto all_logger : all_loggers_)
    all_logger->reinitialise();
#endif
}


mbl_log_categories::mbl_log_categories()
{
  cat_spec default_spec;
  default_spec.level = mbl_logger::NOTICE;
  default_spec.output = cat_spec::NAMED_STREAM;
  default_spec.name = "std::cerr";
  default_spec.stream = &std::cerr;
  default_spec.dump_prefix = "";
  cat_list_[""] = default_spec;
}

typedef std::map<std::string, std::ostream*> stream_names_t;


inline mbl_log_categories::cat_spec parse_cat_spec(const std::string &str,
                                                   const stream_names_t& stream_names)
{
  mbl_log_categories::cat_spec spec;
  std::istringstream ss(str);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  std::string s = props.get_required_property("level");
  if (s == "NONE")
    spec.level = mbl_logger::NONE;
  else if (s == "EMERG")
    spec.level = mbl_logger::EMERG;
  else if (s == "ALERT")
    spec.level = mbl_logger::ALERT;
  else if (s == "CRIT")
    spec.level = mbl_logger::CRIT;
  else if (s == "ERR")
    spec.level = mbl_logger::ERR;
  else if (s == "WARN")
    spec.level = mbl_logger::WARN;
  else if (s == "NOTICE")
    spec.level = mbl_logger::NOTICE;
  else if (s == "INFO")
    spec.level = mbl_logger::INFO;
  else if (s == "DEBUG")
    spec.level = mbl_logger::DEBUG;
  else if (s == "ALL")
    spec.level = mbl_logger::ALL;
  else
  {
    mbl_exception_warning(
      mbl_exception_parse_error(
        std::string("mbl_log_categories.cxx:parse_cat_spec: unknown level: ") + s) );
    // Default to NOTICE if no exceptions.
    spec.level = mbl_logger::NOTICE;
  }

  spec.dump_prefix = props.get_optional_property("dump_prefix");
  spec.timestamp = props.get_optional_property("timestamp");

  if (props.find("file_output") != props.end())
  {
    spec.output = mbl_log_categories::cat_spec::FILE_OUT;
    spec.name = props["file_output"];
    props.erase("file_output");
  }
  else if (props.find("stream_output") != props.end())
  {
    spec.name = "";
    std::string s = props["stream_output"];
    spec.output = mbl_log_categories::cat_spec::NAMED_STREAM;
    spec.name = s;
    auto it = stream_names.find(s);

    if (s == "cout" || s == "std::cout" || s == "std::cout")
      spec.stream = &std::cout;
    else if (s == "cerr" || s == "std::cerr" || s == "std::cerr")
      spec.stream = &std::cerr;
    else if (it != stream_names.end())
      spec.stream = it->second;
    else
    {
      mbl_exception_warning(
        mbl_exception_parse_error(
          std::string("mbl_log.cxx:parse_cat_spec: unknown stream output name: ")
          + props["stream_output"]) );
      // Default to CERR if no exceptions.
      spec.stream = &std::cerr;
      spec.name = "std::cerr";
    }
    props.erase("stream_output");
  }
  else
  {
    spec.output = mbl_log_categories::cat_spec::NAMED_STREAM;
    spec.stream = &std::cerr;
    spec.name = "std::cerr";
  }

  mbl_read_props_look_for_unused_props("mbl_log.cxx::parse_cat_spec", props);

  return spec;
}

//: Configure whole category list from a file.
// New entries are added to any existing category details.
void mbl_log_categories::config(std::istream&s, const stream_names_t& stream_names)
{
  mbl_read_props_type props = mbl_read_props_ws(s);

  //Deal with "root" special case.
  auto it1=props.find("root");
  if (it1 == props.end())
    it1 = props.find("ROOT");
  if (it1 != props.end())
  {
    cat_spec spec = parse_cat_spec(it1->second, stream_names);
    cat_list_[""] = spec;
    props.erase(it1);
  }

  for (mbl_read_props_type::const_iterator it2=props.begin(), end = props.end();
       it2 != end; ++it2)
  {
    cat_spec spec = parse_cat_spec(it2->second, stream_names);
    cat_list_[it2->first] = spec;
  }
}

//: Make the category list empty;
// An "empty" list still contains a root entry.
void mbl_log_categories::clear()
{
  cat_list_.clear();
  cat_spec default_spec;
  default_spec.level = mbl_logger::NOTICE;
  default_spec.name = "cerr";
  default_spec.stream = &std::cerr;
  default_spec.output = cat_spec::NAMED_STREAM;
  default_spec.dump_prefix = "";
  cat_list_[""] = default_spec;
}

struct mbl_log_prefix_comp
{
  std::string s2;
  mbl_log_prefix_comp(std::string  s): s2(std::move(s)) {}

  bool operator() (const std::pair<std::string, mbl_log_categories::cat_spec>& s1)
  {
// simple version:     return s1.first == s2.substr(0,s1.first.size());
// However this would allow s1=AA.11 to match against AA.111

    if (s1.first.size() == s2.size())
      return s1.first == s2;
    else if (s1.first.size() > s2.size())
      return false;
    else if (s1.first.empty()) // always match against root.
      return true;
    else
      return s1.first == s2.substr(0,s1.first.size()) && s2[s1.first.size()] == '.';
  }
};


const mbl_log_categories::cat_spec&
  mbl_log_categories::get(const std::string& category) const
{
  typedef std::map<std::string, cat_spec>::const_reverse_iterator iter;

  auto it = std::find_if(cat_list_.rbegin(), cat_list_.rend(),
                        mbl_log_prefix_comp(category));
  // The search shouldn't get past the first (root) entry.
  assert(it != cat_list_.rend());

  // std::cerr << "MBL_LOG: Using category \"" << it->first << '\"' << '\n';
  return it->second;
}


void mbl_log_categories::print(std::ostream& os) const
{
  assert(!cat_list_.empty());

  auto it = cat_list_.begin(), end = cat_list_.end();
  assert(it->first.empty());

  os << "root:\n  " << it->second << '\n';

  ++it;
  for (; it!=end; ++it)
    os << it->first << ":\n  " << it->second << '\n';

  os.flush();
};
