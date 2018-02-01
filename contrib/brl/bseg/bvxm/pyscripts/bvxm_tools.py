def get_satellite_uncertainty(sat_name):
    if "QuickBird" in sat_name:
        return 40
    if "WorldView1" in sat_name:
        return 14
    if "WorldView2" in sat_name:
        return 24
    if "GeoEye" in sat_name:
        return 5
    return 40
