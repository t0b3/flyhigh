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

function Flight(map)
{
  var flight = this;

  this.map = map;
  this.track = null;
  this.glider = null;
  this.stpos = null;
  this.trackPts = null;
  this.sogList = null;
  this.varioList = null;
  this.altList = null;
  this.elevationList = null;
  this.minAlt = 0;
  this.maxAlt = 0;
  this.timeList = null;
}

Flight.prototype.getMap = function()
{
  return this.map;
};

Flight.prototype.setTrackPts = function(trackPts)
{
  var icon;

  icon = L.icon({
      iconUrl: 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=FFFFFF,FFFFFF,000000&ext=.png',
      iconSize:     [20, 32],
      iconAnchor:   [10, 32],
      popupAnchor:  [-3, -76]
  });

  this.trackPts = trackPts;
  this.track = L.polyline(trackPts, {color: '#FF0000', weight: 3}).addTo(this.getMap());
  this.glider = L.marker(trackPts[0], {icon: icon, draggable: false}).addTo(this.getMap());
};

Flight.prototype.getTrackPts = function()
{
  return this.trackPts;
};

Flight.prototype.getTrackPtAt = function(index)
{
  var trackPt = null;

  if((this.trackPts !== null) && (index < this.trackPts.length))
  {
    trackPt = this.trackPts[index];
  }

  return trackPt;
};

Flight.prototype.setSogList = function(sogList)
{
  this.sogList = sogList;
};

Flight.prototype.getSogAt = function(index)
{
  var sog = null;

  if((this.sogList !== null) && (index < this.sogList.length))
  {
    sog = this.sogList[index];
  }

  return sog;
};

Flight.prototype.setVarioList = function(varioList)
{
  this.varioList = varioList;
};

Flight.prototype.getVarioAt = function(index)
{
  var vario = null;

  if((this.varioList !== null) && (index < this.varioList.length))
  {
    vario = this.varioList[index];
  }

  return vario;
};

Flight.prototype.setAltList= function(altList, minAlt, maxAlt)
{
  this.altList = altList;
  this.minAlt = minAlt;
  this.maxAlt = maxAlt;
};

Flight.prototype.getAltList= function()
{
  return this.altList;
};

Flight.prototype.getAltAt = function(index)
{
  var alt = null;

  if((this.altList !== null) && (index < this.altList.length))
  {
    alt = this.altList[index];
  }

  return alt;
};

Flight.prototype.getMinAlt = function()
{
  return this.minAlt;
};

Flight.prototype.getMaxAlt = function()
{
  return this.maxAlt;
};

Flight.prototype.setElevationList= function(elevationList)
{
  this.elevationList = elevationList;
};

Flight.prototype.getElevationList = function()
{
  return this.elevationList;
};

Flight.prototype.getElevationAt = function(index)
{
  var elevation = null;

  if((this.elevationList !== null) && (index < this.elevationList.length))
  {
    elevation = this.elevationList[index];
  }

  return elevation;
};

Flight.prototype.setTimeList = function(timeList)
{
  this.timeList = timeList;
};

Flight.prototype.getTimeList = function()
{
  return this.timeList;
};

Flight.prototype.getTimeAt = function(index)
{
  var time = null;

  if((this.timeList !== null) && (index < this.timeList.length))
  {
    time = this.timeList[index];
  }

  return time;
};

Flight.prototype.moveGliderTo = function(index)
{
  var time = null;

  if((this.trackPts !== null) && (index < this.trackPts.length))
  {
    this.glider.setLatLng(this.trackPts[index]);
  }
};

Flight.prototype.getGliderPosition = function()
{
  return this.glider.getLatLng();
};
