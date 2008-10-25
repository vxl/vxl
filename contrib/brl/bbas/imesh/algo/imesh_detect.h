// This is brl/bbas/imesh/algo/imesh_detect.h
#ifndef imesh_detect_h_
#define imesh_detect_h_
//:
// \file
// \brief Detect features on meshes
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date May 30, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim


#include <imesh/imesh_mesh.h>
#include <vcl_set.h>
#include <vgl/vgl_vector_3d.h>


//: Return the set of triangles that are visible from this viewing direction
//  Backfacing triangles are not rendered or counted if \a backfacing == NULL
// if \a backfacing is valid, backfacing exterior triangles are also added to this set
vcl_set<unsigned int>
imesh_detect_exterior_faces(const imesh_mesh& mesh,
                            const vgl_vector_3d<double>& dir,
                            unsigned int img_size = 1000,
                            vcl_set<unsigned int> *backfacing = NULL);


//: Return the set of triangles that are visible in some of the many sample view directions
//  Does NOT render backfacing faces or count them as exterior
vcl_set<unsigned int>
imesh_detect_exterior_faces(const imesh_mesh& mesh,
                            unsigned int num_dir_samples = 8,
                            unsigned int img_size = 1000);


//: Return the set of triangles that are visible in some of the many sample view directions
//  Does render backfacing faces and classifies exterior faces as:
//  - frontfacing - seen only from the front
//  - backfacing  - seen only from the back
//  - bifacing    - seen from both sides
void
imesh_detect_exterior_faces(const imesh_mesh& mesh,
                            vcl_set<unsigned int>& frontfacing,
                            vcl_set<unsigned int>& backfacing,
                            vcl_set<unsigned int>& bifacing,
                            unsigned int num_dir_samples = 8,
                            unsigned int img_size = 1000);

#endif // imesh_detect_h_
