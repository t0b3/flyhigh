/***************************************************************************
 *   Copyright (C) 2013 by Alex Graf                                       *
 *   grafal@sourceforge.net                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 *   It is prohibited to serve or run this code over network p.e. as web   *
 *   service in combination with closed source.                            *
 ***************************************************************************/

var ph_oms = null;
var ph_cluster = null;

function ph_initPhotoList(map)
{
  var infowin;

  ph_cluster = L.markerClusterGroup({showCoverageOnHover: false});
  map.addLayer(ph_cluster);

/*
  ph_oms = new OverlappingMarkerSpiderfier(map, {markersWontMove: true,
                                           markersWontHide: true,
                                           keepSpiderfied: true});
  infowin = new google.maps.InfoWindow();

  infowin.addListener('closeclick', function()
  {
    ph_oms.unspiderfy();
  });

  ph_oms.addListener('click', function(marker)
  {
    var content;

    content = '<img src="file://' + marker.path +
              '" alt="" style="max-width:600px;height:480px;width:auto;">';
    infowin.setContent(content);
    infowin.open(map, marker);
  });

  ph_oms.addListener('spiderfy', function(markers)
  {
    infowin.close();
  });
*/
}

function ph_addPhoto(map, opts)
{
  var photo;
  photo = new Photo(map, opts);
  ph_cluster.addLayer(photo.getMarker());
}

function Photo(map, opts)
{
  var photo = this;
  var marker;
  var icon;
  var latlng = [opts.lat, opts.lng];

  this.map = map;
  this.path = opts.path;

  icon = L.icon({
      iconUrl: 'qrc:/camera.png',
      shadowUrl: 'qrc:/camera.png',
      iconSize:     [24, 24],
      shadowSize:   [0, 0],
      iconAnchor:   [12, 12],
      shadowAnchor: [0, 0],
      popupAnchor:  [0, -6]
  });

  marker = L.marker(latlng, {draggable: false, icon: icon});
  marker.bindPopup('<img src="file://' + opts.path +
                   '" alt="" style="max-width:600px;height:480px;width:auto;">',
                   {maxWidth: 'auto'});

  this.marker = marker;

/*
  this.marker = new google.maps.Marker({
    map: map,
    draggable: false,
    raiseOnDrag: false,
    position: new google.maps.LatLng(opts.lat, opts.lng),
    zIndex: 12
  });
*/
}

Photo.prototype.getMap = function()
{
  return this.map;
};

Photo.prototype.getMarker = function()
{
  return this.marker;
};

Photo.prototype.getPath = function()
{
  return this.path;
};
