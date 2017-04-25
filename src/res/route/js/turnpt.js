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

TurnPt.Type = {
  Undefined: 0,
  WayPoint: 1,
  Cross: 2
};

function TurnPt(route, latlng, type)
{
  var turnPt = this;
  var marker;

  this.route = route;
  this.type = TurnPt.Type.Undefined;
  this.altitude = -1;
  this.nextTurnPt = null;
  this.prevLeg = null;
  this.nextLeg = null;
  this.editable = true;
  this.dragActive = false;
  this.infoBox = null;
  this.delta = null;

  marker = L.marker(latlng, {draggable: true});
  marker.on('click', L.DomEvent.stopPropagation);
  marker.on('dragstart', function(event) {tp_dragstart(turnPt);});
  marker.on('drag', function(event) {tp_drag(turnPt);});
  marker.on('dragend', function(event) {tp_dragend(turnPt);});
  marker.on('move', function(event){tp_position_changed(turnPt);});
  marker.on('dblclick', function(event){tp_dblclick(turnPt);});
  marker.on('mouseover', function(event){tp_mouseover(turnPt);});
  marker.on('mouseout', function(event){tp_mouseout(turnPt);});
  marker.addTo(route.getMap());

  this.marker = marker;
  this.setType(type);
}

TurnPt.prototype.getRoute = function()
{
  return this.route;
};

TurnPt.prototype.setType = function(type)
{
  if(this.type !== type)
  {
    this.type = type;
    this.updateIcon();
  }
};

TurnPt.prototype.getType = function()
{
  return this.type;
};

TurnPt.prototype.setDelta = function(latlng)
{
  this.delta = latlng;
};

TurnPt.prototype.getDelta = function()
{
  return this.delta;
};

TurnPt.prototype.remove = function()
{
  this.marker.remove();
};

TurnPt.prototype.setNextTurnPt = function(nextTurnPt)
{
  this.nextTurnPt = nextTurnPt;
};

TurnPt.prototype.getNextTurnPt = function()
{
  return this.nextTurnPt;
};

TurnPt.prototype.setNextLeg = function(leg)
{
  this.nextLeg = leg;
  this.updateIcon();
};

TurnPt.prototype.getNextLeg = function()
{
  return this.nextLeg;
};

TurnPt.prototype.setPrevLeg = function(leg)
{
  this.prevLeg = leg;
  this.updateIcon();
};

TurnPt.prototype.getPrevLeg = function()
{
  return this.prevLeg;
};

TurnPt.prototype.setPosition = function(latlng)
{
  this.marker.setLatLng(latlng);
};

TurnPt.prototype.getPosition = function()
{
  return this.marker.getLatLng();
};

TurnPt.prototype.setAltitude = function(alt)
{
  this.altitude = alt;
};

TurnPt.prototype.getAltitude = function()
{
  return this.altitude;
};

TurnPt.prototype.setEditable = function(en)
{
  this.editable = en;

  if(en)
  {
    this.marker.dragging.enable();
  }
  else
  {
    this.marker.dragging.disable();
  }
};

TurnPt.prototype.getEditable = function()
{
  return this.editable;
};

TurnPt.prototype.setDragActive = function(active)
{
  this.dragActive = active;
};

TurnPt.prototype.getDragActive = function()
{
  return this.dragActive;
};

TurnPt.prototype.getInfoBox = function()
{
  return this.infoBox;
};

TurnPt.prototype.updateIcon = function()
{
  var icon;

  if(!this.dragActive)
  {
    switch(this.getType())
    {
      case TurnPt.Type.WayPoint:
        if(this.getPrevLeg() === null)
        {
          icon = L.icon({
              iconUrl: 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=FFFFFF,00EE00,000000&ext=.png',
              iconSize:     [20, 32],
              iconAnchor:   [10, 32],
              popupAnchor:  [-3, -76]
          });

          this.marker.setIcon(icon);
        }
        else if(this.getNextLeg() === null)
        {
          icon = L.icon({
              iconUrl: 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=FFFFFF,EE0000,000000&ext=.png',
              iconSize:     [20, 32],
              iconAnchor:   [10, 32],
              popupAnchor:  [-3, -76]
          });

          this.marker.setIcon(icon);
        }
        else
        {
          icon = L.icon({
              iconUrl: 'http://chart.apis.google.com/chart?cht=mm&chs=20x32&chco=FFFFFF,0000EE,000000&ext=.png',
              iconSize:     [20, 32],
              iconAnchor:   [10, 32],
              popupAnchor:  [-3, -76]
          });

          this.marker.setIcon(icon);
        }

        this.infoBox = null;
      break;
      case TurnPt.Type.Cross:
        icon = L.icon({
            iconUrl: '../route/images/quad.png',
            iconSize:     [7, 7],
            iconAnchor:   [4, 4],
            popupAnchor:  [-3, -76]
        });

        this.marker.setIcon(icon);
      break;
    }
  }
};

function tp_position_changed(turnPt)
{
  if(turnPt.getEditable() && (turnPt.getType() === TurnPt.Type.WayPoint))
  {
    if(turnPt.getPrevLeg() !== null)
    {
      turnPt.getPrevLeg().setEndPosition(turnPt.getPosition());
    }

    if(turnPt.getNextLeg() !== null)
    {
      turnPt.getNextLeg().setBeginPosition(turnPt.getPosition());
    }

    turnPt.getRoute().update();
  }
}

function tp_dragstart(turnPt)
{
  if(turnPt.getEditable())
  {
    // this prevents icon change on drag (or we loose drag)
    turnPt.setDragActive(true);

    if((turnPt.getType() === TurnPt.Type.Cross) &&
        (turnPt.getPrevLeg() !== null))
    {
//      turnPt.getInfoBox().hide();
      turnPt.getRoute().spliceLeg(turnPt);
    }

    turnPt.getRoute().turnPtDrag();
  }
}

function tp_drag(turnPt)
{
}

function tp_dragend(turnPt)
{
  turnPt.setDragActive(false);
  turnPt.updateIcon();
}

function tp_dblclick(turnPt)
{
  if(turnPt.getEditable() && (turnPt.getType() === TurnPt.Type.WayPoint))
  {
    turnPt.getRoute().removeMarker(turnPt);
  }
}

function tp_mouseover(turnPt)
{
  var leg;
  var dist;
  var duration;
  var latlng;
  var msg;
  var beginPos;
  var endPos;

  if(turnPt.getInfoBox() !== null)
  {
    leg = turnPt.getPrevLeg();
    beginPos = leg.getBeginTurnPt().getPosition();
    endPos = leg.getEndTurnPt().getPosition();
    dist = endPos.distanceTo(beginPos);
    dist /= 1000.0;
    duration = dist / turnPt.getRoute().getSpeed();
    duration = Math.round(duration * 10) / 10;
    msg = "Distance: " + dist.toFixed(2) + " km ";
    msg += "Duration: " + duration.toFixed(1) + " h";

    latlng = turnPt.getPosition();
    turnPt.getInfoBox().show(latlng, 180, 50, msg);
  }
}

function tp_mouseout(turnPt)
{
  if(turnPt.getInfoBox() !== null)
  {
    turnPt.getInfoBox().hide();
  }
}
