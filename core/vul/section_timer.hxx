// This is core/vul/section_timer.hxx

#ifndef section_timer_h_
#define section_timer_h_

#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <vector>
#include <memory>
#include <sstream>

#include "vul_timer.h"

//: A convenience class to profile a section of code and its 
//  immediate subsections
class SectionTimer {
private:
  vul_timer timer, total_timer;
  std::vector<std::pair<std::string, long>> named_times;

  //: Converts a formatted string along with its arguments into an std::string.
  // Implementation based on https://stackoverflow.com/a/26221725
  std::string to_string(const char* format, va_list args) {
    // Determine space needed for the formatted string
    va_list args_copy;
    va_copy(args_copy, args);
    int size_s = std::vsnprintf(nullptr, 0, format, args) + 1; // Extra space for '\0'
    if(size_s <= 0)
      throw std::runtime_error("Error during formatting.");
    
    // Create formatted string
    size_t size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::vsnprintf(buf.get(), size, format, args_copy);
    return std::string(buf.get(), buf.get() + size - 1);
  }

public:
  SectionTimer() {
    this->reset_all_timers();
  }

  //: Resets the subsection timer
  void reset_timer() {
    this->timer.mark();
  }

  //: Resets both the section (i.e. total) and subsection timer
  void reset_all_timers() {
    this->timer.mark();
    this->total_timer.mark();
  }

  //: Records the time spent since this function was last called,
  // the timer was constructed, or the subsection timer was reset,
  // along with a formatted name attached to that time.
  // Optionally prints out the name and time.
  void record_time(bool print, const char* format, ...) {
    long time = this->timer.real();
    va_list args;
    va_start(args, format);
    std::string fmt_str = this->to_string(format, args);
    va_end(args);

    if(print)
      std::cerr << fmt_str << ": " << time << " ms\n";
    this->named_times.emplace_back(fmt_str, time);

    this->timer.mark();
  }

  //: Print all subsection times recorded, along with the total time taken,
  // either to a file in CSV format, to stderr, or both.
  void print_summary(const char* filename = NULL, bool print_to_both = false) {
    if(!filename && print_to_both)
      throw std::runtime_error("No file specified when printing to both file and stderr!");
    this->named_times.emplace_back("total", this->total_timer.real());

    std::ostringstream file_oss, cerr_oss;
    for(auto& pair : this->named_times) {
      file_oss << pair.second << ",";
      cerr_oss << pair.first << "=" << pair.second << ", ";
    }
    this->named_times.clear();

    if(filename) {
      std::string to_print = file_oss.str();
      to_print.pop_back();
      to_print += "\n";

      int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
      if(fd < 0)
        throw std::runtime_error("open: " + std::string(strerror(errno)));
      if(write(fd, to_print.c_str(), to_print.size()) < 0)
        throw std::runtime_error("write: " + std::string(strerror(errno)));
    }
    if(!filename || print_to_both) {
      std::string to_print = cerr_oss.str();
      to_print.pop_back(); to_print.pop_back();

      std::cerr << to_print << std::endl;
    }

    this->reset_all_timers();
  }
};

#endif // section_timer_h_