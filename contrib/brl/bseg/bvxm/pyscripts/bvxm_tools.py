from bvxm_register import bvxm_batch,dbvalue;
from bvxm_adaptor import *
from bvxm_volm_adaptor import *
from bvxm_vpgl_adaptor import *
from bvxm_vil_adaptor import *
import os, time, glob, sys
#############################################################################
# PROVIDES higher level python functions to make bvxm_batch
# code more readable/refactored
#############################################################################

def get_satellite_uncertainty(sat_name):
  if "QuickBird" in sat_name:
    return 40;
  if "WorldView1" in sat_name:
    return 14;
  if "WorldView2" in sat_name:
    return 24;
  if "GeoEye" in sat_name:
    return 5;
  return 40;
