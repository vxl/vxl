#
# ./oxl/osl/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

ADD_LIBRARY(osl osl_sources)

SOURCE_FILES(osl_sources
 osl_OrthogRegress          osl_edge_detector
 osl_break_edge             osl_edge_detector_params
 osl_canny_base             osl_edgel_chain
 osl_canny_gradient         osl_fit_lines
 osl_canny_nms              osl_fit_lines_params
 osl_canny_ox               osl_harris
 osl_canny_ox_params        osl_harris_params
 osl_canny_port             osl_kernel
 osl_canny_rothwell         osl_load_topology
 osl_canny_rothwell_params  osl_ortho_regress
 osl_canny_smooth           osl_roi_window
 osl_chamfer                osl_save_topology
 osl_convolve               osl_topology
 osl_easy_canny             osl_vertex
 internals/droid            osl_edge
 internals/osl_reorder_chain
)

AUX_SOURCE_DIRECTORY(Templates osl_sources)

SUBDIRS(tests examples)
