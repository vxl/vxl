// :
// \file
// \brief Load in a set of points, and a list of indices. Write out given subset.
// \author Tim Cootes

#include <msm/msm_points.h>
#include <vcl_cstdlib.h>

#include <vul/vul_arg.h>

void print_usage()
{
  vcl_cout << "points_subset index_file in_pts out_pts" << vcl_endl;
  vcl_cout << "Load list of indices from text file index_file" << vcl_endl;
  vcl_cout << "Load set of points from in_pts" << vcl_endl;
  vcl_cout << "Select subset of in_pts, using indices in index_file" << vcl_endl;
  vcl_cout << "Save to out_pts" << vcl_endl;
  vcl_cout << "Index file just lists integers, in range [0,n_pts-1], to be used." << vcl_endl;
  vcl_cout << "Note, it can have comments using //" << vcl_endl;
  vcl_cout << "For instance:" << vcl_endl;
  vcl_cout << " 0 1 // Eyes" << vcl_endl;
  vcl_cout << " 5 6 7 8 9 // Mouth" << vcl_endl;
}

bool load_index(const char* path, size_t n_points, vcl_vector<int>& index)
{
  // Load in indices, and check they are in [0,n-1]
  vcl_ifstream ifs(path, vcl_ios_in);

  if( !ifs )
    {
    vcl_cerr << "Unable to open " << path << vcl_endl;
    return false;
    }

  const int MAXLEN = 255;
  char      comment[MAXLEN];

  ifs >> vcl_ws;
  while( !ifs.eof() )
    {
    vcl_string s;
    ifs >> s;
    if( (s.size() >= 2) && (s[0] == '/') && (s[1] == '/') )
      {
      // Comment line, so read to end
      ifs.getline(comment, MAXLEN);
      ifs >> vcl_ws;
      continue;
      }

    int i = atoi(s.c_str() );

    if( i < 0 || i >= n_points )
      {
      vcl_cerr << "Illegal index: " << i << " is not in range [0.." << n_points - 1 << "]" << vcl_endl;
      return false;
      }

    index.push_back(i);
    ifs >> vcl_ws;
    }

  ifs.close();
  return true;
}

int main(int argc, char* * argv)
{
  if( argc != 4 )
    {
    print_usage();
    return 0;
    }

  msm_points points, new_points;

  vcl_string in_path = vcl_string(argv[2]);
  if( !points.read_text_file(in_path) )
    {
    vcl_cerr << "Failed to load points from " << in_path << vcl_endl;
    return 1;
    }
  vcl_cout << "Loaded " << points.size() << " points from " << in_path << vcl_endl;

  vcl_vector<int> index;
  if( !load_index(argv[1], points.size(), index) ) {return 2; }

  vcl_cout << "Loaded " << index.size() << " indices" << vcl_endl;

  // Create subset
  int                               n = index.size();
  vcl_vector<vgl_point_2d<double> > new_pts(n);
  for( int i = 0; i < n; ++i )
    {
    new_pts[i] = points[index[i]];
    }

  new_points.set_points(new_pts);

  vcl_string out_path = vcl_string(argv[3]);
  if( !new_points.write_text_file(out_path) )
    {
    vcl_cerr << "Failed to write points to " << out_path << vcl_endl;
    return 3;
    }
  vcl_cout << new_points.size() << " points saved to " << out_path << vcl_endl;

  return 0;
}
