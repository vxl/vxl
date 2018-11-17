// This is gel/gst/gst_polygon_2d_operators.cxx
//:
// \file
// \author crossge@crd.ge.com

#include "gst_polygon_2d_operators.h"

std::vector<gst_polygon_2d_sptr> gst_make_polygons_2d( const std::vector<gst_edge_2d_sptr>& edges)
{
  // flags showing edges already used
  std::vector<int> used( edges.size(), 0);

  // repository of polygons as they are created
  std::vector<gst_polygon_2d_sptr> polygons;

  // start a polygon with each edge, and look for a closed cycle
  //  hopefully using a NEW edge
  for (unsigned int i=0; i< edges.size(); i++)
    {
      bool newface= false;
      bool closed= false;
      gst_polygon_2d_sptr thispoly= new gst_polygon_2d;

      // flags showing edges already used in this polygon
      std::vector<int> pused( edges.size(), 0);

      thispoly->add( edges[i]);

      if (!used[i])
          newface= true;

      used[i]= 1;
      pused[i]= 1;

      gst_vertex_2d_sptr start= edges[i]->get_start();
      gst_vertex_2d_sptr end  = edges[i]->get_end();

      // repeatedly look for the next edge in the cycle
      //  until we do a complete pass without finding any further
      //  edges
      bool added= true;

      while (added && !closed)
        {
          added= false;

          for (unsigned int j=0; j< edges.size() && !closed; j++)
            {
              if (edges[j]->get_start().ptr()== end.ptr() && !pused[j])
                {
                  thispoly->add( edges[j]);
                  added= true;

                  end= edges[j]->get_end();

                  if (!used[j]) newface= true;

                  used[j]= 1;
                  pused[j]= 1;

                  if (end.ptr()== start.ptr())
                    closed= true;
                }
            }
        }

      if (newface && closed)
        polygons.push_back( thispoly);
    }

  return polygons;
}


std::vector<gst_polygon_2d_sptr> gst_make_polygons_2d_unoriented( const std::vector<gst_edge_2d_sptr>& edges)
{
  // flags showing edges already used
  std::vector<int> used( edges.size(), 0);

  // repository of polygons as they are created
  std::vector<gst_polygon_2d_sptr> polygons;

  // start a polygon with each edge, and look for a closed cycle
  //  hopefully using a NEW edge
  for (unsigned int i=0; i< edges.size(); i++)
    {
      bool newface= false;
      bool closed= false;
      gst_polygon_2d_sptr thispoly= new gst_polygon_2d;
#if 0
      std::cerr << "Starting face by adding edge\n"
               << *edges[i] << std::endl;
#endif
      thispoly->add( edges[i]);

      if (!used[i])
        newface= true;

      used[i]= 1;

      gst_vertex_2d_sptr start= edges[i]->get_start();
      gst_vertex_2d_sptr end  = edges[i]->get_end();

      // repeatedly look for the next edge in the cycle
      //  until we do a complete pass without finding any further
      //  edges
      bool added= true;

      while (added && !closed)
        {
          added= false;

          for (unsigned int j=0; j< edges.size() && !closed; j++)
            {
              if (edges[j]->get_start().ptr()== end.ptr() && !used[j])
                {
#if 0
                  std::cerr << "Found unflip-necessary edge...\n";
                  std::cerr << *edges[j] << std::endl;
#endif
                  thispoly->add( edges[j]);
                  added= true;

                  end= edges[j]->get_end();

                  if (!used[j]) newface= true;

                  used[j]= 1;

                  if (end.ptr()== start.ptr())
                    {
                      closed= true;
                    }
                }
              else if (edges[j]->get_end().ptr()== end.ptr() && !used[j])
                {
#if 0
                  std::cerr << "Found flip-necessary edge...\n";
                  std::cerr << *edges[j] << " -- ";
#endif
                  edges[j]->flip();
#if 0
                  std::cerr << *edges[j] << std::endl;
#endif
                  thispoly->add( edges[j]);
                  added= true;

                  end= edges[j]->get_end();

                  if (!used[j]) newface= true;

                  used[j]= 1;

                  if (end.ptr()== start.ptr())
                    closed= true;
                }
            }
        }

      if (newface && closed)
        polygons.push_back( thispoly);
    }

#if 0
   for (unsigned int i=0; i< polygons.size(); i++)
   {
     gst_polygon_2d *p= polygons[i].ptr();
     std::cerr << "Polygon " << i << std::endl;
     std::cerr << *p << std::endl;
   }
#endif
  return polygons;
}
