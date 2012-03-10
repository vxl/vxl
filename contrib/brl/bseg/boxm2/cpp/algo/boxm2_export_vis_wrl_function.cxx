#include "boxm2_export_vis_wrl_function.h"
//:
// \file

void boxm2_compute_export_vis_wrl_function::exportVisToWRL(boxm2_block_metadata data, boxm2_block* blk, boxm2_data_base* alphas,
                                                           boxm2_data_base* vis, boxm2_data_base* points, vcl_ofstream& file)
{
    boxm2_block_id id = blk->block_id();
    vcl_size_t dataSize = alphas->buffer_length();
    boxm2_data_traits<BOXM2_AUX0>::datatype *   vis_data = (boxm2_data_traits<BOXM2_AUX0>::datatype*) vis->data_buffer();
    boxm2_data_traits<BOXM2_POINT>::datatype *  p_data = (boxm2_data_traits<BOXM2_POINT>::datatype*) points->data_buffer();
    file << "Shape {\n"
         <<"  geometry PointSet {\n"
         <<"     coord DEF mypts Coordinate { point [ ";
    float maxVis = 0;
    float minVis = 1;
    for (unsigned i = 0; i < dataSize /  boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());i++ ) {
        if (vis_data[i] != -1 && vis_data[i] > 0) {
            file << p_data[i][0] << ' ' << p_data[i][1] << ' ' << p_data[i][2] << ",\n";
            if (maxVis < vis_data[i])
                maxVis = vis_data[i];
            if (minVis > vis_data[i])
                minVis = vis_data[i];
        }
    }
    vcl_cout << "Min vis: " << minVis << vcl_endl
             << "Max vis :" << maxVis << vcl_endl;
    file << " ] }\n"
         <<"     color Color { color [ ";
    for (unsigned i = 0; i < dataSize /  boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());i++ ) {
        if (vis_data[i] != -1 && vis_data[i] > 0) {
#if 0
            if (maxVis-minVis != 0)
                file << (vis_data[i]-minVis)/(maxVis-minVis) << ' ' << (vis_data[i]-minVis)/(maxVis-minVis) << ' ' << (vis_data[i]-minVis)/(maxVis-minVis) << ",\n";
            else
#endif
                file << (vis_data[i]) << ' ' << (vis_data[i]) << ' ' << (vis_data[i]) << ",\n";
        }
    }
    file << " ] }\n"
         << "}\n"
         << "}\n";

#ifdef DEBUG
    unsigned count = 0;
    for (unsigned i = 0; i < dataSize /  boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());i++ ) {
        if (vis_data[i] == 1) {
            ++count;
        }
    }
    vcl_cout << count << vcl_endl;
#endif
}

