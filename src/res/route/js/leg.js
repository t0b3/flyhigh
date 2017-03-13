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

function Leg(route)
{
/*
  this.line = new google.maps.Polyline({
    strokeColor: '#FF0000',
    strokeOpacity: 1.0,
    strokeWeight: 1,
    map: route.getMap(),
    zIndex: 5
  });
*/
  this.line = null;
  this.route = route;
  this.beginTurnPt = null;
  this.endTurnPt = null;
  this.cross = null;
  this.editable = true;
}

Leg.prototype.getRoute = function()
{
  return this.route;
};

Leg.prototype.remove = function()
{
  if(this.cross !== null)
  {
    this.cross.remove();
    this.cross = null;
  }

  this.line.remove();
  this.beginTurnPt = null;
  this.endTurnPt = null;
};

Leg.prototype.setTurnPts = function(beginTurnPt, endTurnPt)
{
  var path = [beginTurnPt.getPosition(), endTurnPt.getPosition()];
  var latlng;

  this.beginTurnPt = beginTurnPt;
  this.endTurnPt = endTurnPt;

  this.line = L.polyline(path, {color: '#FF0000', weight: 1}).addTo(this.getRoute().getMap());
//  this.line.setPath(path);
  latlng = lg_getMidPoint(beginTurnPt.getPosition(), endTurnPt.getPosition());
  this.cross = new TurnPt(this.getRoute(), latlng, TurnPt.Type.Cross);
  this.cross.setEditable(this.getEditable());
  this.cross.setPrevLeg(this);
};

Leg.prototype.setBeginTurnPt = function(turnPt)
{
  this.beginTurnPt = turnPt;
  this.setBeginPosition(turnPt.getPosition());
};

Leg.prototype.getBeginTurnPt = function()
{
  return this.beginTurnPt;
};

Leg.prototype.setEndTurnPt = function(turnPt)
{
  this.endTurnPt = turnPt;
  this.setEndPosition(turnPt.getPosition());
};

Leg.prototype.getEndTurnPt = function()
{
  return this.endTurnPt;
};

Leg.prototype.setBeginPosition = function(pos)
{
  var latlngs = this.line.getLatLngs();
  latlngs[0] = pos;
//  this.line.getPath().setAt(0, pos);
  this.updateCross();
  this.line.redraw();
};

Leg.prototype.setEndPosition = function(pos)
{
  var latlngs = this.line.getLatLngs();
  latlngs[1] = pos;
//  this.line.getPath().setAt(1, pos);
  this.updateCross();
  this.line.redraw();
};

Leg.prototype.setEditable = function(en)
{
  this.editable = en;

  if(this.cross !== null)
  {
    this.cross.setEditable(en);
  }
};

Leg.prototype.getEditable = function()
{
  return this.editable;
};

Leg.prototype.updateCross = function()
{
  var latlng;

  latlng = lg_getMidPoint(this.beginTurnPt.getPosition(), this.endTurnPt.getPosition());
  this.cross.setPosition(latlng);
};

function lg_getMidPoint(pos1, pos2)
{
  var midX;
  var midY;

  var midLat = (pos2.lat - pos1.lat) / 2 + pos1.lat;
  var midLng = (pos2.lng - pos1.lng) / 2 + pos1.lng;

  return L.latLng(midLat, midLng);
}
