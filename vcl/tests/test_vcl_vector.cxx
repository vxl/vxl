
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_algorithm.h>
#include <vcl/vcl_iterator.h>
#include <vcl/vcl_vector.h>

int main()
{
  typedef vcl_vector<int> container;
  container m;
  
  m.push_back(1);
  m.push_back(2);

  for(container::iterator p = m.begin(); p != m.end(); ++p)
    cout << (*p) << endl;

  //   copy(m.begin(), m.end(), vcl_ostream_iterator<int>(cerr));
  
  return 0;
}
