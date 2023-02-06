# Requirements:
# - OSMPythonTools
# - geopy
# - geojson
# - geographiclib
# - pyyaml
# - shapely


import sys
import math
import yaml
import json
import geojson
import shapely
from shapely.geometry import LineString, Polygon
from shapely.ops import split

from geographiclib.geodesic import Geodesic
from OSMPythonTools.api import Api as OSMApi
from OSMPythonTools.nominatim import Nominatim
from OSMPythonTools.overpass import Overpass, overpassQueryBuilder
from OSMPythonTools.data import Data, dictRangeYears, ALL
from OSMPythonTools.cachingStrategy import CachingStrategy, JSON, Pickle
from CachingNone import CachingNone


def ClipWay(bbox, way):
    polyline = LineString(way)
    polygon = Polygon(bbox)
    way_split = shapely.ops.split(polyline, polygon)
    clipped_ways = []
    for shapely_way in way_split.geoms:
        clipped_way_coords = []
        for pt in shapely_way.coords:
            if pt[0] < bbox[0][0] or pt[0] > bbox[2][0] or pt[1] < bbox[0][1] or pt[1] > bbox[2][1]:
                break
            clipped_way_coords.append([pt[0], pt[1]])
        if len(clipped_way_coords) > 1:
            clipped_ways.append(clipped_way_coords)
    return clipped_ways


def OverpassOSMQuery(params, origin, semantic_data_filename):

    contains_data = False

    osmApi = OSMApi()
    overpass = Overpass()
    # osmApi = OSMApi("http://localhost:12346/api/")
    # overpass = Overpass("http://localhost:12346/api/")
    CachingStrategy.use(CachingNone)
    geod = Geodesic.WGS84

    feature_collection = geojson.FeatureCollection([])

    area_dim = params['area_dim']

    relative_pos = geod.Direct(origin['lat'], origin['lon'], 0, area_dim)
    bottom_right = {'lat': relative_pos['lat2'], 'lon': relative_pos['lon2']}
    relative_pos = geod.Direct(bottom_right['lat'], bottom_right['lon'], 90, area_dim)
    top_right = {'lat': relative_pos['lat2'], 'lon': relative_pos['lon2']}
    relative_pos = geod.Direct(top_right['lat'], top_right['lon'], 180, area_dim)
    top_left = {'lat': relative_pos['lat2'], 'lon': relative_pos['lon2']}
    poly_bounds = [origin, bottom_right, top_right, top_left]
    feature_collection['bbox'] = [origin['lon'], origin['lat'], top_right['lon'], top_right['lat']]
    bounding_polygon = [(origin['lon'], origin['lat']), (bottom_right['lon'], bottom_right['lat']), (top_right['lon'], top_right['lat']), (top_left['lon'], top_left['lat'])]

    feature_collection['center'] = [(origin['lat'] + top_right['lat'])/2., (origin['lon'] + top_right['lon'])/2.]

    out_str = "(._;>;);out geom;"
    bounds_str = '(poly: "'
    for i in range(0, len(poly_bounds)):
        pt = poly_bounds[i]
        if i == 0:
            bounds_str += str(pt['lat']) + ' ' + str(pt['lon'])
        else:
            bounds_str += ' ' + str(pt['lat']) + ' ' + str(pt['lon'])
    bounds_str += '");'

    semantic_objects = params['semantic_objects']
    road_network_query = 'way[highway~"^('
    road_network_tags = semantic_objects['road_network']
    for iTag in range(0, len(road_network_tags)):
        if iTag > 0:
            road_network_query += '|'
        road_network_query += road_network_tags[iTag]
    road_network_query += ')$"]' + bounds_str + out_str
    road_network = overpass.query(road_network_query)

    if road_network.ways():
        # contains_data = True
        for ways in road_network.ways():
            ways_coordinates = ways.geometry()["coordinates"]
            if ways.geometry()["type"] == "LineString":
                ways_coordinates = [ways_coordinates]
            clipped_ways_list = []
            for way_coords in ways_coordinates:
                clipped_ways = ClipWay(bounding_polygon, way_coords)
                for clipped_way in clipped_ways:
                    clipped_ways_list.append(clipped_way)
            for clipped_way in clipped_ways_list:
                if len(clipped_way) > 0:
                    way_feature = geojson.Feature(geometry = geojson.LineString(clipped_way), id = ways.id(), properties={"type": "road_network", "subtype": ways.tags()['highway'], "osmtype": "way"})
                feature_collection.features.append(way_feature)

    leisure_query = 'nwr[leisure~"^('
    leisure_tags = semantic_objects['leisure']
    for iTag in range(0, len(leisure_tags)):
        if iTag > 0:
            leisure_query += '|'
        leisure_query += leisure_tags[iTag]
    leisure_query += ')$"]' + bounds_str + out_str
    leisure = overpass.query(leisure_query)
    leisure_json = leisure.toJSON()

    if leisure.ways():
        contains_data = True
        for way in leisure.ways():
            tags = way.tags()
            if tags == None or 'leisure' not in tags:
                continue
            leisure_tag = way.tags()['leisure']
            if leisure_tag in semantic_objects['leisure']:
                coords_list = way.geometry()["coordinates"]
                poly_feature = geojson.Feature(geometry = geojson.Polygon(coords_list), id = way.id(), properties={"type": "leisure", "subtype": leisure_tag, "osmtype": "way"})
                feature_collection.features.append(poly_feature)
            
    amenity_query = 'nwr[amenity~"^('
    amenity_tags = semantic_objects['amenity']
    for iTag in range(0, len(amenity_tags)):
        if iTag > 0:
            amenity_query += '|'
        amenity_query += amenity_tags[iTag]
    amenity_query += ')$"]' + bounds_str + out_str
    amenity = overpass.query(amenity_query)
    amenity_json = amenity.toJSON()

    if amenity.ways():
        contains_data = True
        for way in amenity.ways():
            tags = way.tags()
            if tags == None or 'amenity' not in tags:
                continue
            amenity_tag = way.tags()['amenity']
            if amenity_tag in semantic_objects['amenity']:
                if 'parking' in tags and tags['parking'] == 'underground':
                    continue
                coords_list = way.geometry()["coordinates"]
                poly_feature = geojson.Feature(geometry = geojson.Polygon(coords_list), id = way.id(), properties={"type": "amenity", "subtype": amenity_tag, "osmtype": "way"})
                feature_collection.features.append(poly_feature)
            
    if semantic_objects['traffic_signals'] == True:
        traffic_signal_query = 'node[highway~"^(traffic_signals)$"]' + bounds_str + out_str
        traffic_signals = overpass.query(traffic_signal_query)

    if traffic_signals.nodes():
        contains_data = True
        for node in traffic_signals.nodes():
            feature = geojson.Feature(geometry=geojson.Point((node.lon(), node.lat())), id=node.id(), properties={"type": "traffic_signal", "osmtype": "node"})
            feature_collection.features.append(feature)

    if amenity.relations():
        contains_data = True
        for relation in amenity.relations():
            try:
                geom = relation.geometry()
            except:
                continue
            contains_data = True
            tags = relation.tags()
            if tags == None or 'amenity' not in tags:
                continue
            amenity_tag = relation.tags()['amenity']
            if amenity_tag in semantic_objects['amenity']:
                coords_list = geom["coordinates"]
                if geom["type"] == "Polygon":
                    coords_list = [coords_list]
                for coords in coords_list:
                    poly_feature = geojson.Feature(geometry = geojson.Polygon(coords), id = relation.id(), properties={"type": "amenity", "subtype": amenity_tag, "osmtype": "relation"})
                    feature_collection.features.append(poly_feature)

    if contains_data == False:
        return False

    with open(semantic_data_filename, 'w', encoding='utf-8') as f:
        geojson.dump(feature_collection, f)

    return True

def args():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--semantic_data_path', type=str, default='semantic_data/', help='YAML file containing semantic objects to query')

    args = parser.parse_args()
    return args

if __name__ == '__main__':
    params_filename = 'semantic_objects.yaml'
    if len(sys.argv) > 1:
        params_filename = sys.argv[1]
    with open(params_filename, 'r') as file:
        params = yaml.safe_load(file)

    # List of origins
    origins = [{'lat': 39.945827951386065, 'lon': -75.2047706396303}, # 1. UPenn
              {'lat': 40.74050005471615, 'lon': -74.1759877275644}, # 2. New York
              {'lat': 41.381775552849454, 'lon': -73.96846537685275}, # 3. West Point
              {'lat': 42.3601, 'lon': -71.0942}, # 4. MIT
              {'lat': 42.3770, 'lon': -71.1167}, # 5. Harvard
              {'lat': 37.4275, 'lon': -122.1697}, # 6. Stanford
              {'lat': 41.3163, 'lon': -72.9223},# 7. Yale
              {'lat': 40.3431, 'lon': -74.6551}, # 8. Princeton
              {'lat': 34.1377, 'lon': -118.1253}, # 9. Caltech
              {'lat': 37.8719, 'lon': -122.2635}, # 10. UCB
              {'lat': 34.0689, 'lon': -118.4452}, # 11. UCLA
              {'lat': 34.0224, 'lon': -118.2851}, #12. USC
              {'lat': 40.7295, 'lon': -73.9965}, # 13. NYU
              {'lat': 40.8075, 'lon': -73.9626}, # 14. Columbia
              {'lat': 42.4534, 'lon': -76.4735}, # 15. Cornell
              {'lat': 40.4432, 'lon': -79.9428}, # 16. CMU
              {'lat': 47.6553, 'lon': -122.3035}, # 17. UWash
              {'lat': 42.2780, 'lon': -83.7382}, # 18. Michigan
              {'lat': 33.7756, 'lon': -84.3963}, # 19. GATech
              {'lat': 51.7548, 'lon': -1.2544}, # 20. Oxford
              {'lat': 52.2054, 'lon': +0.1132}, # 21. Cambridge
              {'lat': 51.4988, 'lon': -0.1749}, #22. ICL
              {'lat': 47.3764, 'lon': +8.5476}, # 23. ETH Zurich
              {'lat': 1.2966, 'lon': +103.7764}, # 24. NUS
              {'lat': 1.3483, 'lon': +103.6831}] # 25. NTU

    args  = args()

    import os
    if not os.path.exists(args.semantic_data_path):
        os.makedirs(args.semantic_data_path)

    for j,coord in enumerate(origins):
        semantic_data_filename = args.semantic_data_path + 'semantic_data{}.json'.format(j)
        #semantic_data_filename = 'leaflet_geojson_viz/data/semantic_data.json'
        OverpassOSMQuery(params, coord, semantic_data_filename)
