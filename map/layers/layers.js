var baseLayer = new ol.layer.Group({
    'title': 'Base maps',
    layers: [
new ol.layer.Tile({
    'title': 'OSM',
    'type': 'base',
    source: new ol.source.OSM()
})
]
});
var lyr_hotspot2016_cmi = new ol.layer.Tile({
                        source: new ol.source.TileWMS(({
                          url: "http://tile.gistda.or.th/geoserver/gcon/wms?",
                          params: {
                            "LAYERS": "hotspot2016_cmi",
                            "TILED": "true",
                            "VERSION": "1.3.0"},
                        })),
                        title: "hotspot2016_cmi",
                        
                        
                      });

lyr_hotspot2016_cmi.setVisible(true);
var layersList = [baseLayer,lyr_hotspot2016_cmi];
