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
WayPoint.CallbackType =
{
  Selected: 0,
  Position: 1,
  DragEnd: 2
};

function WayPoint(map, cluster, opts)
{
  var wayPoint = this;
  var marker;
  var latlng = [opts.lat, opts.lng];
  this.map = map;
  this.cluster = cluster;
  this.id = opts.id;
  this.name = opts.name;
  this.spot = opts.spot;
  this.country = opts.country;
  this.alt = opts.alt;
  this.modified = opts.modified;
  this.selected = opts.selected;
  this.editable = opts.editable;
  this.stPos = null;
  this.changeCallback = null;


  marker = L.marker(latlng, {draggable: false});
  marker.on('click', function(event) {wp_click(wayPoint);});
  marker.on('dragstart', function(event) {wp_dragstart(wayPoint);});
  marker.on('drag', function(event) {wp_drag(wayPoint);});
  marker.on('dragend', function(event) {wp_dragend(wayPoint);});
  cluster.addLayer(marker);

/*
  marker.on('click', function(event) {wp_click(wayPoint);});
  marker.on('dragstart', function(event) {wp_dragstart(wayPoint);});
  marker.on('drag', function(event) {wp_drag(wayPoint);});
  marker.on('dragend', function(event) {wp_dragend(wayPoint);});
*/
//marker.addTo(map);

  this.marker = marker;

/*
  this.marker = new google.maps.Marker({
    map: map,
    draggable: true,
    raiseOnDrag: false,
    position: new google.maps.LatLng(opts.lat, opts.lng),
    zIndex: 10
  });

  google.maps.event.addListener(this.marker, 'click', function(event) {wp_click(wayPoint);});
  google.maps.event.addListener(this.marker, 'dragstart', function(event) {wp_dragstart(wayPoint);});
  google.maps.event.addListener(this.marker, 'drag', function(event) {wp_drag(wayPoint);});
  google.maps.event.addListener(this.marker, 'dragend', function(event) {wp_dragend(wayPoint);});
*/
  this.updateIcon();
}

WayPoint.prototype.getMap = function()
{
  return this.map;
};

WayPoint.prototype.getCluster = function()
{
  return this.cluster;
};

WayPoint.prototype.getMarker = function()
{
  return this.marker;
};

WayPoint.prototype.getId = function()
{
  return this.id;
};

WayPoint.prototype.setName = function(name)
{
  this.name = name;
  this.setModified(true);
};

WayPoint.prototype.getName = function()
{
  return this.name;
};

WayPoint.prototype.setSpot = function(spot)
{
  this.spot = spot;
  this.setModified(true);
};

WayPoint.prototype.getSpot = function()
{
  return this.spot;
};

WayPoint.prototype.setCountry = function(country)
{
  this.country = country;
  this.setModified(true);
};

WayPoint.prototype.getCountry = function()
{
  return this.country;
};

WayPoint.prototype.setAlt = function(alt)
{
  this.alt = alt;
  this.setModified(true);
};

WayPoint.prototype.getAlt = function()
{
  return this.alt;
};

WayPoint.prototype.setPosition = function(latlng)
{
  this.marker.setPosition(latlng);
  this.setModified(true);
};

WayPoint.prototype.setLat = function(lat)
{
  var latlng;
  latlng = this.getPosition();
  this.setPosition(L.latLng(lat, latlng.lng)); // new google.maps.LatLng(lat, pos.lng()));
  this.setModified(true);
};

WayPoint.prototype.setLng = function(lng)
{
  var latlng;
  latlng = this.getPosition();
  this.setPosition(L.latLng(latlng.lat, lng)); //new google.maps.LatLng(pos.lat(), lng));
  this.setModified(true);
};

WayPoint.prototype.getPosition = function()
{
  return this.marker.getLatLng();
};

WayPoint.prototype.setChangeCallback = function(callback)
{
  this.changeCallback = callback;
};

WayPoint.prototype.setEditable = function(editable)
{
  if(editable)
  {
    this.marker.dragging.enable();
  }
  else
  {
    this.marker.dragging.disable();
  }

  this.editable = editable;
};

WayPoint.prototype.getEditable = function()
{
  return this.editable;
};

WayPoint.prototype.setSelected = function(selected)
{
  this.selected = selected;
  this.updateIcon();

  if(selected && (this.changeCallback !== null))
  {
    this.emitChange(WayPoint.CallbackType.Selected);
  }
};

WayPoint.prototype.getSelected = function()
{
  return this.selected;
};

WayPoint.prototype.setModified = function(modified)
{
  this.modified = modified;
  this.updateIcon();
};

WayPoint.prototype.getModified = function()
{
  return this.modified;
};

WayPoint.prototype.emitChange = function(type)
{
  if(this.changeCallback !== null)
  {
    this.changeCallback(type, this);
  }
};

/*
  This is an ugly hack, to restore position while drag. Because Qt 4.6 won't display
  markers which are not draggable. In a later version, this should be fixed through
  setting markers draggable=this.editable.
*/
/*
WayPoint.prototype.storePos = function()
{
  this.stPos = new google.maps.LatLng(this.getPosition().lat(), this.getPosition().lng());
};

WayPoint.prototype.restorePos = function()
{
  if(this.stPos !== null)
  {
    this.marker.setPosition(this.stPos);
  }
};
*/
WayPoint.prototype.updateIcon = function()
{
  var icon;

  if(this.getSelected())
  {
    if(this.getModified())
    {
      icon = L.icon({
          iconUrl: 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=EE0000,FFFF00,000000&ext=.png',
          shadowUrl: 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=EE0000,FFFF00,000000&ext=.png',
          iconSize:     [20, 32],
          shadowSize:   [0, 0],
          iconAnchor:   [10, 32],
          shadowAnchor: [0, 0],
          popupAnchor:  [-3, -76]
      });
//      icon = 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=EE0000,FFFF00,000000&ext=.png';
    }
    else
    {
      icon = L.icon({
          iconUrl: 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=00EE00,FFFF00,000000&ext=.png',
          shadowUrl: 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=00EE00,FFFF00,000000&ext=.png',
          iconSize:     [20, 32],
          shadowSize:   [0, 0],
          iconAnchor:   [10, 32],
          shadowAnchor: [0, 0],
          popupAnchor:  [-3, -76]
      });
//      icon = 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=00EE00,FFFF00,000000&ext=.png';
    }
  }
  else
  {
    if(this.getModified())
    {
      icon = L.icon({
          iconUrl: 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=FFFFFF,EE0000,000000&ext=.png',
          shadowUrl: 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=FFFFFF,EE0000,000000&ext=.png',
          iconSize:     [20, 32],
          shadowSize:   [0, 0],
          iconAnchor:   [10, 32],
          shadowAnchor: [0, 0],
          popupAnchor:  [-3, -76]
      });
//      icon = 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=FFFFFF,EE0000,000000&ext=.png';
    }
    else
    {
      icon = L.icon({
          iconUrl: 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=FFFFFF,00EE00,000000&ext=.png',
          shadowUrl: 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=FFFFFF,00EE00,000000&ext=.png',
          iconSize:     [20, 32],
          shadowSize:   [0, 0],
          iconAnchor:   [10, 32],
          shadowAnchor: [0, 0],
          popupAnchor:  [-3, -76]
      });
//      icon = 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=FFFFFF,00EE00,000000&ext=.png';
    }
  }

  this.marker.setIcon(icon);
};

function wp_click(wayPoint)
{
  wayPoint.setSelected(true);
}

function wp_dragstart(wayPoint)
{
}

function wp_drag(wayPoint)
{
  if(wayPoint.getEditable())
  {
    wayPoint.emitChange(WayPoint.CallbackType.Position);
  }
}

function wp_dragend(wayPoint)
{
  if(wayPoint.getEditable())
  {
    wayPoint.emitChange(WayPoint.CallbackType.DragEnd);
    wayPoint.setSelected(true);
    wayPoint.setModified(true);
  }
}
