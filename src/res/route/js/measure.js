/***************************************************************************
 *   Copyright (C) 2012 by Alex Graf                                       *
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

function Measure(map)
{
  this.map = map;
  this.changeCallback = null;
  this.clickListener = null;
  this.moveListener = null;
  this.markers = [];
  this.enable = false;
  this.line = null;

/*
  this.line = new google.maps.Polyline({
    strokeColor: '#5500ff',
    strokeOpacity: 1.0,
    strokeWeight: 1,
    map: null,
    zIndex: 3,
    clickable: false
  });
*/
}

Measure.prototype.getMap = function()
{
  return this.map;
};

Measure.prototype.setChangeCallback = function(callback)
{
  this.changeCallback = callback;
};

Measure.prototype.show = function(enable)
{
  var nr;
  var measure = this;
  var map = this.getMap();

  this.enable = enable;

  if(enable)
  {
    document.getElementById('map').style.cursor = 'crosshair';
//    map.setOptions({draggableCursor: 'crosshair'});
    map.on('click', this.click, this);
    map.on('mousemove', this.mousemove, this);
/*
    this.clickListener = google.maps.event.addListener(map, 'click', function(event){ms_click(measure, event);});
    this.moveListener = google.maps.event.addListener(map, 'mousemove', function(event){ms_mousemove(measure, event);});
*/
  }
  else
  {
    document.getElementById('map').style.cursor = '';
//    map.setOptions({draggableCursor: 'default'});
    map.off('click', this.click, this);
    map.off('mousemove', this.mousemove, this);

/*
    google.maps.event.removeListener(this.clickListener);
    google.maps.event.removeListener(this.moveListener);
    this.line.getPath().clear();
//    this.line.setMap(null);
*/
    if(this.line !== null)
    {
      this.line.remove();
      this.line = null;
    }

    this.clearMarkers();
  }
};

Measure.prototype.getEnable = function()
{
  return this.enable;
};

Measure.prototype.getDist = function()
{
  var latlngs = 0;

  if(this.line !== null)
  {
    latlngs = this.line.getLatLngs();

    if(latlngs.length === 2)
    {
      dist = latlngs[0].distanceTo(latlngs[1]);
//      dist = google.maps.geometry.spherical.computeDistanceBetween(path.getAt(0), path.getAt(1));
      dist /= 1000.0;
    }
  }

  return dist;
};

Measure.prototype.click = function(event)
{
  var latlngs = [];
  var length;
  var marker;

  if(this.markers.length === 2)
  {
    this.line.remove();
    this.line = null;
    this.clearMarkers();
  }

  if(this.markers.length === 0)
  {
    // only on first click
    latlngs.push(event.latlng);
    latlngs.push(event.latlng);
    this.line = L.polyline(latlngs, {color: '#5500ff', weight: 1}).addTo(this.getMap());
  }

  marker = new MeasureMarker(this.getMap(), event.latlng);
  this.markers.push(marker);
  this.changeCallback();
};

Measure.prototype.mousemove = function(event)
{
  var latlngs;
  var length;

  if(this.markers.length === 1)
  {
    latlngs = this.line.getLatLngs();
    length = latlngs.length;
    latlngs[length - 1] = event.latlng;
    this.line.redraw();
    this.changeCallback();
  }
};

Measure.prototype.clearMarkers = function()
{
  var nr;

  for(nr=0; nr<this.markers.length; nr++)
  {
    this.markers[nr].remove();
  }

  this.markers = [];
};

function ms_click(measure, event)
{
  measure.click(event.latlng);
}

function ms_mousemove(measure, event)
{
  measure.mousemove(event.latlng);
}

function MeasureMarker(map, latlng)
{
  var measure = this;
  var marker;

  this.stpos = null;

  var icon = L.icon({
      iconUrl: 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=FFFFFF,5500ff,000000&ext=.png',
      shadowUrl: 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=FFFFFF,5500ff,000000&ext=.png',
      draggable: true,
      iconSize: [20, 32],
      shadowSize: [0, 0],
      iconAnchor: [10, 32],
      shadowAnchor: [0, 0],
      popupAnchor: [-3, -76]
  });

  marker = L.marker(latlng, {draggable: false, icon: icon});
  marker.addTo(map);
  this.marker = marker;

/*
  this.marker = new google.maps.Marker({
    position: latlng,
    map: map,
    draggable: true,
    raiseOnDrag: false,
    icon: 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=FFFFFF,5500ff,000000&ext=.png'
  });

  google.maps.event.addListener(this.marker, 'dragstart', function(event) {mm_dragstart(marker);});
  google.maps.event.addListener(this.marker, 'drag', function(event) {mm_drag(marker);});
*/
}

MeasureMarker.prototype.setMap = function(map)
{
  this.marker.setMap(map);
};

MeasureMarker.prototype.setPosition = function(latlng)
{
  this.marker.setPosition(latlng);
};

MeasureMarker.prototype.getPosition = function()
{
  return this.marker.getLatLng();
};

MeasureMarker.prototype.remove = function()
{
  this.marker.remove();
};

/*
  This is an ugly hack, to restore position while drag. Because Qt 4.6 won't display
  markers which are not draggable. In a later version, this should be fixed through
  setting markers draggable=this.editable.
*/
/*
MeasureMarker.prototype.storePos = function()
{
  this.stpos = new google.maps.LatLng(this.getPosition().lat(), this.getPosition().lng());
};

MeasureMarker.prototype.restorePos = function()
{
  if(this.stpos !== null)
  {
    this.setPosition(this.stpos);
  }
};

function mm_dragstart(marker)
{
  marker.storePos();
}

function mm_drag(marker)
{
  marker.restorePos();
}
*/
