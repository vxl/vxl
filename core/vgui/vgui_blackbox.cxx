// This is ./oxl/vgui/vgui_blackbox.cxx

//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   13 Oct 99

//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif

#include "vgui_blackbox.h"

#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_fstream.h>

#include <vpl/vpl.h>

#include <vgui/vgui.h>
#include <vgui/vgui_utils.h>

vgui_blackbox::vgui_blackbox(vgui_tableau_sptr const& t) :
  vgui_wrapper_tableau(t),
  recording(false),
  playback(false)
{
}

vgui_blackbox::~vgui_blackbox()
{
}

vcl_string vgui_blackbox::type_name() const { return "vgui_blackbox"; }


static void help() {
  vcl_cerr << vcl_endl;
  vcl_cerr << "-- vgui_blackbox ---------" << vcl_endl;
  vcl_cerr << "|     keys               |"  << vcl_endl;
  vcl_cerr << "| `,'  start/stop record |"  << vcl_endl;
  vcl_cerr << "| `.'           playback |"  << vcl_endl;
  vcl_cerr << "| `s'   playback w. dump |"  << vcl_endl;
  vcl_cerr << "| `/'       print events |"  << vcl_endl;
  vcl_cerr << "| `#'       clear events |"  << vcl_endl;
  vcl_cerr << "--------------------------" << vcl_endl;
  vcl_cerr << vcl_endl;
}


bool vgui_blackbox::handle(const vgui_event& event) {
  // manage the recording and playback
  if (event.type == vgui_KEY_PRESS) {
    bool do_save= false;
    switch (event.key) {
    case ',':
      recording = !recording;
      if (recording) {
        vgui::out << "blackbox: starting record" << vcl_endl;
        //vgui_event start_e;
        //events.push_back(start_e);
      }
      else {
        vgui::out << "blackbox: ending record" << vcl_endl;
      }
      return true;

    case 's':
      // Play and save.
      do_save = true;

    case '.':
      if (recording) {
        vgui::out << "blackbox: stop recording before playback..." << vcl_endl;
      } else {
        vgui::out << "blackbox: starting playback" << vcl_endl;
        if (child) {
          vgui_event *old_e = 0;
          int frame_number = 0;
          int t = 0;
          vcl_ofstream story("/tmp/vgui_blackbox.story");

          for (vcl_vector<vgui_event>::iterator e_iter=events.begin();
               e_iter != events.end(); ++e_iter) {

            // if first event then don't wait
            int dt = 0;
            if (old_e) {
              dt = (e_iter->timestamp - old_e->timestamp);
              t += dt;
              vpl_usleep(dt * 1000);
            }

            if ((*e_iter).type == vgui_DRAW) {
              // Draw events are different: post a redraw and run_till_idle.
              child->post_redraw();
              vgui::run_till_idle();
              vgui::flush();
            } else {
              // Regular event, handle it
              child->handle(*e_iter);
            }

            // Remember this event, at least for timestamping.
            old_e = &(*e_iter);

            // Save frames
            {
              char buf[1024];
              vcl_sprintf(buf, "/tmp/vgui_blackbox.%03d.ppm", frame_number);
              if (do_save)  vgui_utils::dump_colour_buffer(buf);
              if (old_e) {
                double d = (dt * 1e-3);
                story << "delay " <<  d << vcl_endl;
                story << "image " << buf << vcl_endl;
                vgui::out << "blackbox: Saving frame " << buf << ", delay " << dt << vcl_endl;
              }

              ++frame_number;
            }
          }
        }
        vgui::out << "blackbox: ending playback" << vcl_endl;
      }
      return true;

    case '/':
      if (!recording) {
        vcl_cerr << "vgui_blackbox EVENTS" << vcl_endl;
        for (vcl_vector<vgui_event>::iterator e_iter=events.begin();
             e_iter != events.end(); ++e_iter) {
          vcl_cerr << *e_iter << vcl_endl;
        }
        return true;
      }
      break;

    case '#':
      if (!recording) {
        vgui::out << "blackbox: clearing events" << vcl_endl;
        events.clear();
        return true;
      }
      break;

    case '?':
      if (!recording) {
        ::help();
      }
      break;
    default:
      break;
    }
  }

  vgui_event copy = event;
  bool used = child && child->handle(event);

  if (recording) {
    if (used)
      events.push_back(copy);
    else
      vcl_cerr << "blackbox: Ignoring " << copy << vcl_endl;
  }

  return used;
}
