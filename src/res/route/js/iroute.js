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

var airspaces = [];
var map;
var route;
var measure;
var selectedAirspace = -1;
var airspaceNr;

function rt_init()
{
  map = L.map('map');

  L.tileLayer('http://{s}.tile.opentopomap.org/{z}/{x}/{y}.png', {
          maxZoom: 15,
          attribution: 'Map data: &copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>, <a href="http://viewfinderpanoramas.org">SRTM</a> | Map style: &copy; <a href="https://opentopomap.org">OpenTopoMap</a> (<a href="https://creativecommons.org/licenses/by-sa/3.0/">CC-BY-SA</a>)'
  }).addTo(map);

  map.on('load', function()
  {
    route = new Route(map);
    route.setSpeed(22.0);
    route.setChangeCallback(routeChanged);
    measure = new Measure(map);
    measure.setChangeCallback(measureChanged);

    wm_emitAppReady();
  });

  map.on('click', function(event)
  {
    var inside = false;
    var airspaceNr;

    if(!measure.getEnable())
    {
      // select next airspace
      airspaceNr = selectedAirspace;

      for(var i=0; i<airspaces.length; i++)
      {
        airspaceNr = (airspaceNr + 1) % airspaces.length;
        inside = airspaces[airspaceNr].isInside(event.latlng);

        if(inside)
        {
          if(airspaceNr !== selectedAirspace)
          {
            break; // jump out of loop
          }
        }
      }

      if(inside)
      {
        as_selectAirSpaceNr(airspaceNr);
      }
      else
      {
        as_selectAirSpaceNr(-1);
      }
    }
  });

  map.setView([47.0, 8.5], 9);
}

function as_pushAirSpace(coords, opts)
{
  var latlngs = [];
  var latlng;
  var airspace;
  var nr;

  for(nr=0; nr<coords.length; nr++)
  {
    latlng = L.latLng(coords[nr][0], coords[nr][1]);
    latlngs.push(latlng);
  }

  airspace = new AirSpace(map, latlngs, opts);
  airspaces.push(airspace);
}

function as_selectAirSpaceNr(num)
{
  var airspace;

  if(num < airspaces.length)
  {
    if(selectedAirspace >= 0)
    {
      airspaces[selectedAirspace].setSelect(false);
    }

    selectedAirspace = num;

    if(num >= 0)
    {
      airspace = airspaces[num];
      airspace.setSelect(true);
      wm_setDivValue("airspace", airspace.getName(), false);

      if(airspace.getLow() === 0)
      {
        wm_setDivValue("low", "GND", false);
      }
      else
      {
        wm_setDivValue("low", airspace.getLow() + " m", false);
      }

      wm_setDivValue("high", airspace.getHigh() + " m", false);
      wm_setDivValue("class", airspace.getClass(), false);
    }
    else
    {
      wm_setDivValue("airspace", "", false);
      wm_setDivValue("low", "", false);
      wm_setDivValue("high", "", false);
      wm_setDivValue("class", "", false);
    }
  }
}

function rt_setName(name)
{
  var locInput;

  if(route.getEditable())
  {
    locInput = document.getElementById("name");
    locInput.value = name;
  }
  else
  {
    locInput = document.getElementById("sname");
    locInput.innerHTML = name;
  }
}

function rt_getName()
{
  var locInput;
  var name;

  if(route.getEditable())
  {
    locInput = document.getElementById("name");
    name = locInput.value;
  }
  else
  {
    locInput = document.getElementById("sname");
    name = locInput.innerHTML;
  }

  return name;
}

function rt_setTurnPts(turnPts)
{
  var turnPt;
  var tpNr;
  var bounds = L.latLngBounds(turnPts);
  var latlng;

  for(tpNr=0; tpNr<turnPts.length; tpNr++)
  {
    latlng = L.latLng(turnPts[tpNr][0], turnPts[tpNr][1]);
    turnPt = new TurnPt(route, latlng, TurnPt.Type.WayPoint);
    route.addTurnPt(turnPt);
  }

  map.fitBounds(bounds);
}

function rt_getTurnPts()
{
  var turnPts;
  var turnPt;
  var turnPtArray = [];
  var tpNr;

  turnPts = route.getTurnPts();

  for(tpNr=0; tpNr<turnPts.length; tpNr++)
  {
    turnPt = turnPts[tpNr];
    turnPtArray.push(new Array(turnPt.getPosition().lat, turnPt.getPosition().lng, turnPt.getAltitude()));
  }

  return JSON.stringify(turnPtArray);
}

function rt_getType()
{
  return route.getType();
}

function rt_getDist()
{
  return route.getDist();
}

function rt_setEditable(en)
{
  var nameInput;
  var snameInput;
  var name;

  name = rt_getName();
  route.setEditable(en);
  rt_setName(name);
  nameInput = document.getElementById("name");
  snameInput = document.getElementById("sname");

  if(en)
  {
    nameInput.style.display = "";
    snameInput.style.display = "none";
  }
  else
  {
    nameInput.style.display = "none";
    snameInput.style.display = "";
  }
}

function rt_setGlueToCenter(en)
{
  route.setGlueToCenter(en);
}

function rt_speedUp()
{
  var speed;

  if(route.getEditable())
  {
    speed = route.getSpeed();

    if(speed < 50.0)
    {
      route.setSpeed(speed + 0.5);
      updateDurationAndSpeed();
    }
  }
}

function rt_speedDown()
{
  var speed;

  if(route.getEditable())
  {
    speed = route.getSpeed();

    if(speed > 1.0)
    {
      route.setSpeed(speed - 0.5);
      updateDurationAndSpeed();
    }
  }
}

function rt_durationUp()
{
  var duration;

  if(route.getEditable())
  {
    duration = route.getDuration();

    if(duration < 24)
    {
      route.setDuration(duration + 0.5);
      updateDurationAndSpeed();
    }
  }
}

function rt_durationDown()
{
  var duration;

  if(route.getEditable())
  {
    duration = route.getDuration();

    if(duration > 1)
    {
      route.setDuration(duration - 0.5);
      updateDurationAndSpeed();
    }
  }
}

function rt_measure(div)
{
  var show;

  show = (div.className === "button_up");
  measure.show(show);

  if(show)
  {
    div.className = "button_down";
    div = document.getElementById("smeasure");
    div.innerHTML = "0.0 km";
  }
  else
  {
    div.className = "button_up";
    div = document.getElementById("smeasure");
    div.innerHTML = "Off";
  }
}

function rt_setOk(ok)
{
  wm_emitOk(ok);
}

function routeChanged()
{
  wm_setDivValue("type", Route.ScoreTypeText[route.getType()], false);
  wm_setDivValue("distance", route.getDist().toFixed(2) + " km", false);
  wm_setDivValue("score", route.getScore().toFixed(2) + " points", false);
  wm_setDivValue("track", route.getTrackDist().toFixed(0) + " km", false);
  updateDurationAndSpeed();
}

function measureChanged()
{
  var div;

  div = document.getElementById("smeasure");
  div.innerHTML = measure.getDist().toFixed(1) + " km";
}

function updateDurationAndSpeed()
{
  var locInput;

  locInput = document.getElementById("speed");
  locInput.value = route.getSpeed().toFixed(1) + " km/h";
  locInput = document.getElementById("duration");
  locInput.value = route.getDuration().toFixed(1) + " h";
}
