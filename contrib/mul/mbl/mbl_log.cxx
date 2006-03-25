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
#include <vcl_sstream.h>
#include <vcl_algorithm.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_cctype.h>
#include <vcl_utility.h>
#include <vul/vul_string.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>

vcl_ostream& operator<<(vcl_ostream&os, mbl_logger::levels level)
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


#if 0 // new version using categories

//Notes - use two different stream bufs to hendle the mt_log() and the log().
// one should response to flushes with a terminate - the other not.


// Got the a streambuf example from vul_redirector and hacked around with it.
// It passes on all stuff direct to the real_streambuf, whilst calling
// extra functions on the output object to print message headers and footers.


int mbl_log_streambuf::sync()
{
  vcl_ptrdiff_t n = pptr() - pbase();

  if (n)
    logger_->output_->append(pbase(), n);
  logger_->output_->terminate();

  pbump(-n);  // Reset pptr().
  return 0;
}

int mbl_log_streambuf::overflow(int ch)
{
  vcl_ptrdiff_t n = pptr() - pbase();

  if (n)
    logger_->output_->append(pbase(), n);
  pbump(-n);  // Reset pptr().

  if (ch == EOF)
  return 0;

  char cbuf = ch;
  logger_->output_->append(&cbuf, 1);
  return ch;
}

vcl_streamsize mbl_log_streambuf::xsputn( const char *ptr, vcl_streamsize nchar)
{
  // Output anything already in buffer
  long n = pptr() - pbase();
  if (n)
    logger_->output_->append(pbase(), n);
  pbump(-n);  // Reset pptr().

  logger_->output_->append(ptr, nchar);
  return nchar;
}

//: Default constructor only available to root's default logger.
mbl_logger::mbl_logger():
  level_(NOTICE),
  output_(new mbl_log_output_stream(vcl_cerr, "")),
  streambuf_(this),
  logstream_(&streambuf_),
  mt_logstream_(&logstream_)
{
  // This will have to change to support proper hierarchical control over categories.
//  logstream_.tie(output_.real_stream_);
  // Don't insert default root logger - this would cause infinite recursion.
}


mbl_log_output_stream::mbl_log_output_stream(vcl_ostream& real_stream, const char *id):
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
void mbl_log_output_stream::start(int level, const char *srcfile, int srcline)
{
  start();
  (*real_stream_) << static_cast<mbl_logger::levels>(level) << ": " << id_ << ' ';
}
//: add contents to the existing log entry.
void mbl_log_output_stream::append(const char * contents, vcl_streamsize n_chars)
{
  // Deal with unfinished log message
  if (!has_started_)
  {
    (*real_stream_) << "UNKNOWN_START_LOG: " << id_ << ' ';
    has_started_ = true;
  }

  real_stream_->rdbuf()->sputn(contents, n_chars);
}

//: Finish and flush the log entry.
void mbl_log_output_stream::terminate()
{
  real_stream_->flush();
  has_started_=false;
}

mbl_log_output_file::mbl_log_output_file(const vcl_string &filename, const char *id):
  file_(filename.c_str(), vcl_ios_app), id_(id), has_started_(false)
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
void mbl_log_output_file::start(int level, const char *srcfile, int srcline)
{
  start();
  file_ << static_cast<mbl_logger::levels>(level) << ": " << id_ << ' ';
}
//: add contents to the existing log entry.
void mbl_log_output_file::append(const char * contents, vcl_streamsize n_chars)
{
  // Deal with unstarted log message
  if (!has_started_)
  {
    file_ << "UNKNOWN_START_LOG: " << id_ << ' ';
    has_started_=true;
  }

  file_.rdbuf()->sputn(contents, n_chars);
}

//: Finish and flush the log entry.
void mbl_log_output_file::terminate()
{
  file_.flush();
  has_started_=false;
}


mbl_logger::mbl_logger(const char *id):
  streambuf_(this),
  logstream_(&streambuf_),
  mt_logstream_(&logstream_),
  output_(0)
{
  const mbl_log_categories::cat_spec &cat =
    mbl_logger::root().categories().get(id);

  level_ = cat.level;

  if (cat.output == mbl_log_categories::cat_spec::COUT)
  {
    output_ = new mbl_log_output_stream(vcl_cout, id);
//    logstream_.tie(output_.real_stream_);
  }
  else if (cat.output == mbl_log_categories::cat_spec::CERR)
  {
    output_ = new mbl_log_output_stream(vcl_cerr, id);
//    logstream_.tie(output_.real_stream_);
  }
  else if (cat.output == mbl_log_categories::cat_spec::REGISTERED_STREAM)
  {
    vcl_ostream * pstream = root().get_registered_stream(cat.name);
    if (pstream == 0)
    {
      mbl_exception_warning(
        mbl_exception_parse_error(
          vcl_string("mbl_logger::mbl_logger: unknown registered stream output name: ") + cat.name));
      // Default to CERR if no exceptions.
      output_ = new mbl_log_output_stream(vcl_cerr, id);
    }
    else
      output_ = new mbl_log_output_stream(*pstream, id);
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

  if (cat.output == mbl_log_categories::cat_spec::COUT)
  {
    delete output_;
    output_ = new mbl_log_output_stream(vcl_cout, id);
//    logstream_.tie(output_.real_stream_);
  }
  else if (cat.output == mbl_log_categories::cat_spec::CERR)
  {
    delete output_;
    output_ = new mbl_log_output_stream(vcl_cout, id);
//    logstream_.tie(output_.real_stream_);
  }
  else if (cat.output == mbl_log_categories::cat_spec::REGISTERED_STREAM)
  {
    delete output_;
    output_ = new mbl_log_output_stream(*root().registered_streams_[cat.name], id);
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

vcl_ostream &mbl_logger::log(int level, const char * srcfile, int srcline)
{
  if (level_ < level)
    return root().null_stream_;
  output_->start(level, srcfile, srcline);
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
}

void mbl_logger::mtstop()
{
  logstream_.flush();
  output_->terminate();
}


#else // old version


// Got a streambuf example from vul_redirector and hacked around with it.
// It passes on all stuff directly to the real_streambuf, whilst calling
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
  level_(NOTICE),
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
  level_(mbl_logger::root().default_logger.level_),
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
  (*real_stream_)<<static_cast<mbl_logger::levels>(next_level_);
  (*real_stream_)<< ": " << id_ << ' ';
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


#endif


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
  vcl_ifstream config_file("mbl_log.properties");
  if (!config_file.is_open())
    config_file.open("~/mbl_log.properties");
  if (!config_file.is_open())
    config_file.open("~/.mbl_log.properties");
  if (!config_file.is_open())
    config_file.open("~/.mbl_log.properties");
  if (!config_file.is_open())
    config_file.open("C:\\mbl_log.properties");

  if (!config_file.is_open())
    return;

  config_file >> vcl_ws;
  char a;
  config_file >> a;
  config_file.putback(a);
  if (vcl_isdigit(a))
  {
    int level;
    config_file >> level;
    default_logger.set(level, default_logger.output_);
  }
  else
  {
    categories_.config(config_file);
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


// This can then be referred to using the REGISTERED_STREAM output type.
// The stream must remain in scope while the stream may be being used.
void mbl_logger_root::register_stream(const vcl_string &name, vcl_ostream *p_stream)
{
  registered_streams_[name] = p_stream;
}
//: Remove a steram from the known list.
void mbl_logger_root::deregister_stream(const vcl_string &name)
{
  registered_streams_.erase(name);
}
//: Get a registered stream by name.
// \returns 0 if no such registered stream.
vcl_ostream* mbl_logger_root::get_registered_stream(const vcl_string &name)
{
  vcl_map<vcl_string, vcl_ostream*>::iterator it = registered_streams_.find(name);
  if (it == registered_streams_.end())
    return 0;
  return it->second;
}



mbl_log_categories::mbl_log_categories()
{
  cat_spec defult_spec; defult_spec.level = mbl_logger::NOTICE;
  cat_list_[""] = defult_spec;
}


inline mbl_log_categories::cat_spec parse_cat_spec(const vcl_string &str)
{
  mbl_log_categories::cat_spec spec;
  vcl_istringstream ss(str);
  mbl_read_props_type props = mbl_read_props_ws(ss);

  vcl_string s = props.get_required_property("level");
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
        vcl_string("mbl_log_categories.cxx:parse_cat_spec: unknown level: ") + s) );
    // Default to NOTICE if no exceptions.
    spec.level = mbl_logger::NOTICE;
  }

  if (props.find("file_output") != props.end())
  {
    spec.output = mbl_log_categories::cat_spec::FILE_OUT;
    spec.name = props["file_output"];
    props.erase("file_output");
  }
  else if (props.find("stream_output") != props.end())
  {
    spec.name = "";
    vcl_string s = props["stream_output"];
    vul_string_upcase(s);
    if (s == "COUT" || s == "VCL_COUT" || s == "STD::COUT")
      spec.output = mbl_log_categories::cat_spec::COUT;
    else if (s == "CERR" || s == "VCL_CERR" || s == "STD::CERR")
      spec.output = mbl_log_categories::cat_spec::CERR;
    else
    {
      mbl_exception_warning(
        mbl_exception_parse_error(
          vcl_string("mbl_log.cxx:parse_cat_spec: unknown stream output name: ")
          + props["stream_output"]) );
      // Default to CERR if no exceptions.
      spec.output = mbl_log_categories::cat_spec::CERR;
    }
    props.erase("stream_output");
  }
  else if (props.find("registered_stream_output") != props.end())
  {
    spec.output = mbl_log_categories::cat_spec::REGISTERED_STREAM;
    spec.name = props["registered_stream_output"];
    props.erase("registered_stream_output");
  }
  else
  {
    spec.output = mbl_log_categories::cat_spec::CERR;
    spec.name = "";
  }

  mbl_read_props_look_for_unused_props("mbl_log.cxx::parse_cat_spec", props);

  return spec;
}

//: Configure whole category list from a file.
// New entries are added to any existing category details.
void mbl_log_categories::config(vcl_istream&s)
{
  mbl_read_props_type props = mbl_read_props_ws(s);

  //Deal with "root" special case.
  mbl_read_props_type::iterator it1=props.find("root");
  if (it1 == props.end())
    it1 = props.find("ROOT");
  if (it1 != props.end())
  {
    cat_spec spec = parse_cat_spec(it1->second);
    cat_list_[""] = spec;
    props.erase(it1);
  }

  for (mbl_read_props_type::const_iterator it2=props.begin(), end = props.end();
       it2 != end; ++it2)
  {
    cat_spec spec = parse_cat_spec(it2->second);
    cat_list_[it2->first] = spec;
  }
}

//: Make the category list empty;
// An "empty" list still contains a root entry.
void mbl_log_categories::clear()
{
  cat_list_.clear();
  cat_spec defult_spec; defult_spec.level = mbl_logger::NOTICE;
  cat_list_[""] = defult_spec;
}

struct mbl_log_prefix_comp
{
  vcl_string s2;
  mbl_log_prefix_comp(const vcl_string& s): s2(s) {};

  bool operator() (const vcl_pair<vcl_string, mbl_log_categories::cat_spec>& s1)
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
mbl_log_categories::get(const vcl_string& category) const
{
  typedef vcl_map<vcl_string, cat_spec>::const_reverse_iterator iter;

  iter it = vcl_find_if(cat_list_.rbegin(), cat_list_.rend(),
                        mbl_log_prefix_comp(category));
  // The search shouldn't get past the first (root) entry.
  assert(it != cat_list_.rend());

  // vcl_cerr << "MBL_LOG: Using category \"" << it->first << '\"' << vcl_endl;
  return it->second;
}


vcl_ostream& operator<<(vcl_ostream&os, const mbl_log_categories::cat_spec& spec)
{
  os << "{ level: " << static_cast<mbl_logger::levels>(spec.level);
  switch (spec.output)
  {
   case mbl_log_categories::cat_spec::FILE_OUT:
    os << " file_output: " << spec.name;
    break;
   case mbl_log_categories::cat_spec::COUT:
    os << " stream_output: COUT";
    break;
   case mbl_log_categories::cat_spec::CERR:
    os << " stream_output: CERR";
    break;
  case mbl_log_categories::cat_spec::REGISTERED_STREAM:
    os << " registered_stream_output: " << spec.name;
    break;
  }
  os << " }";
  return os;
}

void mbl_log_categories::print(vcl_ostream& os) const
{
  typedef vcl_map<vcl_string, cat_spec>::const_iterator iter;
  assert(!cat_list_.empty());

  iter it = cat_list_.begin(), end = cat_list_.end();
  assert(it->first.empty());

  os << "root:\n  " << it->second << '\n';

  ++it;
  for (; it!=end; ++it)
    os << it->first << ":\n  " << it->second << '\n';

  os.flush();
};
