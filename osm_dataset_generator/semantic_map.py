import numpy as np
import math
import pyCoverageControl # Main library
from pyCoverageControl import BivariateNormalDistribution as BND # for defining bivariate normal distributions
from pyCoverageControl import GeoLocalTransform as GeoTransform

import json
import geojson

# We can visualize the map in python
import matplotlib.pylab as plt
import seaborn as sns
colormap = sns.color_palette("light:b", as_cmap=True)


def plot_map(map, args, title=None,index = None):
    plt.figure()
    ax = sns.heatmap(map.transpose(), vmax=1, cmap=colormap, square=True)
    ax.invert_yaxis()
    nrow, ncol = map.shape
    septicks = 10 ** (math.floor(math.log(nrow, 10)) - 1)
    plt.xticks(np.arange(0, nrow, septicks), np.arange(0, nrow, septicks))
    plt.yticks(np.arange(0, ncol, septicks), np.arange(0, ncol, septicks))
    #plt.show()
    plt.savefig(args.output_dir + title + str(index) + '.png')


################
## Parameters ##
################
# The parameters can be changed from python without needing to recompile.
# The parameters are given in params/parameters.yaml
# After changing the parameters, use the following function call to use the yaml file.
# Make sure the path of the file is correct
params_ = pyCoverageControl.Parameters('../core/params/parameters.yaml')

##############
## WorldIDF ##
##############
from pyCoverageControl import WorldIDF # for defining world idf
import smopy

def generate_semantic_image(file_name:str,args:dict,index = None):

    with open(file_name) as file_:
    # with open("leaflet_geojson_viz/data/semantic_data.json") as file_:
        semantic_data = geojson.load(file_)

    [origin_lon, origin_lat] = semantic_data.bbox[0:2]
    print(semantic_data.bbox)
    origin_alt = 0
    geo_transform = GeoTransform(origin_lat, origin_lon, origin_alt)

    # BivariateNormalDistribution with peak value of 1
    traffic_signals_sigma = 10
    traffic_signals_scale = 2 * math.pi * traffic_signals_sigma * traffic_signals_sigma

    # Uniform density polygon
    hostpital_importance = 0.9
    parking_importance = 0.6
    park_importance = 0.8
    world_idf_ts = WorldIDF(params_)
    world_idf_leisure = WorldIDF(params_)
    world_idf_amenity = WorldIDF(params_)


    flag = True
    for feature in semantic_data.features:
        if(feature['properties']['type'] == "traffic_signal"):
            coords = feature['geometry']['coordinates']
            mean = geo_transform.Forward(coords[1], coords[0], origin_alt)
            dist = BND(mean[0:2], traffic_signals_sigma, traffic_signals_scale) # circular gaussian
            world_idf_ts.AddNormalDistribution(dist)

        if(feature['properties']['type'] == "leisure" and feature['geometry']['type'] == "Polygon"):
            for coords_list in feature['geometry']['coordinates']:
                polygon_feature = pyCoverageControl.PolygonFeature()
                if(feature['properties']['subtype'] == "park"):
                    polygon_feature.imp = park_importance
                for pt in coords_list[:-1]:
                    cartesian_pt = geo_transform.Forward(pt[1], pt[0], origin_alt)
                    polygon_feature.poly.append(cartesian_pt[0:2])
                world_idf_leisure.AddUniformDistributionPolygon(polygon_feature)

        if(feature['properties']['type'] == "amenity" and feature['geometry']['type'] == "Polygon"):
            for coords_list in feature['geometry']['coordinates']:
                polygon_feature = pyCoverageControl.PolygonFeature()
                if(feature['properties']['subtype'] == "hospital"):
                    polygon_feature.imp = hostpital_importance
                if(feature['properties']['subtype'] == "parking"):
                    polygon_feature.imp = parking_importance
                for pt in coords_list[:-1]:
                    cartesian_pt = geo_transform.Forward(pt[1], pt[0], origin_alt)
                    polygon_feature.poly.append(cartesian_pt[0:2])
                world_idf_amenity.AddUniformDistributionPolygon(polygon_feature)

    world_idf_ts.GenerateMapCuda()
    world_idf_leisure.GenerateMapCuda()
    world_idf_amenity.GenerateMapCuda()
    traffic_map = world_idf_ts.GetWorldMap() # Generate map, use GenerateMap() for cpu version
    leisure_map = world_idf_leisure.GetWorldMap() # Generate map, use GenerateMap() for cpu version
    amenity_map = world_idf_amenity.GetWorldMap() # Generate map, use GenerateMap() for cpu version

    map = np.zeros(leisure_map.shape)
    map[amenity_map>0] = 3
    map[leisure_map>0] = 2
    map[traffic_map>0] = 1

    # plot_map(traffic_map,args,title = "traffic_signal",index = index)
    # plot_map(leisure_map,args,title = "leisure",index = index)
    # plot_map(amenity_map,args,title="amenity",index = index)
    #TODO: need to return the corresponding google map
    gmap = None
    gtmap = smopy.Map((semantic_data.bbox[1],semantic_data.bbox[0],semantic-_data.bbox[3],semantic_data.bbox[2]), z=1)
    gtmap_np = gtmap.to_numpy()


    return map,gtmap_np

def args():
    import argparse
    parser = argparse.ArgumentParser(description='Semantic Map Generator')
    parser.add_argument('--data_dir', type=str, default='../../../data/osm_city_data/', help='Directory of semantic data')
    parser.add_argument('--output_dir', type=str, default='map_images/', help='Directory of semantic map')
    parser.add_argument('--num_maps', type=int, default=50, help='Number of maps')
    parser.add_argument('--map_size', type=int, default=1000, help='Size of map')
    return parser.parse_args()


if __name__ == '__main__':
    args = args()
    import os
    if not os.path.exists(args.output_dir):
        os.makedirs(args.output_dir)

    for i in range(args.num_maps):
        map,gt_map = generate_semantic_image(args.data_dir + '/semantic_data{}.json'.format(str(i)),args,index=i)
        cmap = plt.get_cmap('brg', np.max(map) - np.min(map) + 1)
        # set limits .5 outside true range
        mat = plt.matshow(map, cmap=cmap, vmin=np.min(map) - 0.5,
                          vmax=np.max(map) + 0.5)
        # tell the colorbar to tick at integers
        cax = plt.colorbar(mat, ticks=np.arange(np.min(map), np.max(map) + 1))
        plt.savefig(args.output_dir + 'sem'+str(i) + '.png')

        plt.figure()
        plt.imshow(gt_map)
        plt.savefig(args.output_dir + 'map'+str(i) + '.png')

        np.save(args.output_dir + 'sem'+str(i)+'.npy',map)

        np.save(args.output_dir + 'gmap' + str(i) + '.npy', map)