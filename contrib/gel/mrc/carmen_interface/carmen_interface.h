#ifndef carmen_interface_h_
#define carmen_interface_h_

//:
// \file
// \brief test interface to Carmen
// An interface to Carmen
//
// \author
//             J. Mundy April 3, 2001
//             GE CRD
//
//-----------------------------------------------------------------------------

class Carmen;

class carmen_interface
{
  // PUBLIC INTERFACE----------------------------------------------------------
 public:

  // Constructors/Initializers/Destructors-------------------------------------
  carmen_interface();
  ~carmen_interface();

  // Data Access---------------------------------------------------------------
  bool load_correspondence_file(const string& file_path);
  void save_camera(const string& file_path);
  void print_results();
  // Utility Methods-------------------------------------------------------
  void set_carmen_camera(int view_no);
  void solve();

 protected:
  // Utilities
  bool add_full_correspondence(int view_no, int point_id,
                               double x, double y, double z, float u, float ves);
  // Data Members--------------------------------------------------------------
  Carmen* _carmen;
};

#endif // carmen_interface_h_
