/***************************************************************************
 *   Copyright (C) 2011 by Alex Graf                                       *
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
 *   It is prohibited to server or run this code over network p.e. as web  *
 *   service in combination with closed source.                            *
 ***************************************************************************/
var airspaces = [];
var route = null;
var map = null;
var flight = null;
var plot = null;
var measure;
var selectedAirspace = -1;

function fl_init()
{
  map = L.map('map');

  L.tileLayer('http://{s}.tile.opentopomap.org/{z}/{x}/{y}.png', {
          maxZoom: 15,
          attribution: 'Map data: &copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>, <a href="http://viewfinderpanoramas.org">SRTM</a> | Map style: &copy; <a href="https://opentopomap.org">OpenTopoMap</a> (<a href="https://creativecommons.org/licenses/by-sa/3.0/">CC-BY-SA</a>)'
  }).addTo(map);

  map.on('load', function()
  {
    flight = new Flight(map);

    route = new Route(map);
    route.setChangeCallback(routeChanged);

    plot = new Plot(document.getElementById('plot'));
    plot.setValueLabels(["TIME", "ALT", "GND", "AGL", "SOG", "VARIO"]);
    plot.setUpdateCursorPosCb(updateCursorPos);
    plot.setUpdateCursorClickCb(updateCursorClick);

    measure = new Measure(map);
    measure.setChangeCallback(measureChanged);

    ph_initPhotoList(map);

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

function rt_setName(name)
{
  var locInput;

  locInput = document.getElementById("sstart");
  locInput.innerHTML = name;
}

function fl_setPlotSize(width, height)
{
  var left;
  var top;

  if(plot !== null)
  {
    left = (window.innerWidth - width - 5);
    top = (window.innerHeight - height);
    plot.setGeometry(left, top, width, height);
  }
}

function fl_setFlightTime(timeList, start, duration)
{
  flight.setTimeList(timeList);
  route.setDuration(duration / 3600);
  updateDurationAndSpeed();
}

function fl_setFlightAlt(altList, minAlt, maxAlt)
{
  flight.setAltList(altList, minAlt, maxAlt);
}

function fl_setFlightElevation(elevations)
{
  flight.setElevationList(elevations);
}

function fl_setFlightLatLon(latlngs)
{
  var bounds = L.latLngBounds(latlngs);
  var latlng;
  var posNr;
  var path = [];

  for(posNr=0; posNr<latlngs.length; posNr++)
  {
    latlng = L.latLng(latlngs[posNr][0], latlngs[posNr][1]);
    path.push(latlng);
  }

  flight.setTrackPts(path);
  map.fitBounds(bounds);
}

function fl_setSog(sogList)
{
  flight.setSogList(sogList);
}

function fl_setVario(varioList)
{
  flight.setVarioList(varioList);
}

function fl_pushPhoto(opts)
{
  ph_addPhoto(map, opts);
}

function fl_showPlot()
{
  var index;
  var maxIndex;
  var data = [];

  maxIndex = flight.getTimeList().length;

  for(index=0; index<maxIndex; index++)
  {
    data.push({valueX: flight.getTimeAt(index),
               valueY: flight.getAltAt(index),
               valueY2: flight.getElevationAt(index)});
  }

  plot.adjustMinMaxX(data);
  plot.setMinMaxY(0, flight.getMaxAlt());
  plot.plot(data);
  flight.moveGliderTo(0);
}

function fl_setOk(ok)
{
  wm_emitOk(ok);
}

function fl_speedUp()
{
  var speed;

  speed = route.getSpeed();

  if(speed < 50.0)
  {
    route.setSpeed(speed + 0.5);
    updateDurationAndSpeed();
  }
}

function fl_speedDown()
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

function fl_durationUp()
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

function fl_durationDown()
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

function fl_measure(div)
{
  var show;

  show = (div.className === "button_up");
  measure.show(show);

  if(show)
  {
    div.className = "button_down";
    wm_setDivValue("measure", "0.0 km", false);
  }
  else
  {
    div.className = "button_up";
    wm_setDivValue("measure", "Off", false);
  }
}

function routeChanged()
{
  wm_setDivValue("type", Route.ScoreTypeText[route.getType()], false);
  wm_setDivValue("distance", route.getDist().toFixed(2) + " km", false);
  wm_setDivValue("score", route.getScore().toFixed(2) + " points", false);
  wm_setDivValue("track", route.getTrackDist().toFixed(0) + " km", false);
  updateDurationAndSpeed();
}

function updateDurationAndSpeed()
{
  wm_setDivValue("speed", route.getSpeed().toFixed(1) + " km/h", true);
  wm_setDivValue("duration", route.getDuration().toFixed(1) + " h", true);
}

function updateCursorPos(pos)
{
  var index;
  var date;
  var time;
  var alt;
  var altValue;
  var elevation;
  var elevationValue;
  var agl;
  var sog;
  var vario;
  var timeList;
  var duration;

  // legend-values
  timeList = flight.getTimeList();
  duration = (timeList[timeList.length - 1] - timeList[0]);
  time = timeList[0] + Math.round(pos * duration / plot.getPlotAreaWidth());
  index = getIndex(timeList, time);
  date = new Date(flight.getTimeAt(index) * 1000);
  time = date.getHours() + ":";

  if(date.getMinutes() < 10)
  {
    time += '0';
  }

  time += date.getMinutes() + " UTC";
  altValue = flight.getAltAt(index);
  alt = altValue.toFixed(0) + " m";
  elevationValue = flight.getElevationAt(index);
  elevation = elevationValue.toFixed(0) + " m";
  agl = (altValue - elevationValue).toFixed(0) + " m";
  sog = flight.getSogAt(index).toFixed(1) + " km/h";
  vario = flight.getVarioAt(index).toFixed(1) + " m/s";
  plot.setValues([time, alt, elevation, agl, sog, vario]);

  // glider
  index = Math.round(pos * flight.getTrackPts().length / plot.getPlotAreaWidth());
  flight.moveGliderTo(index);
}

function updateCursorClick(pos)
{
  var index;

  index = Math.round(pos * flight.getTrackPts().length / plot.getPlotAreaWidth());
  map.panTo(flight.getTrackPtAt(index));
  map.setZoom(14);
}

function measureChanged()
{
  wm_setDivValue("measure", measure.getDist().toFixed(1) + " km", false);
}

function findAirspaceById(id)
{
  var nr;
  var airspace = null;

  for(nr=0; nr<airspaces.length; nr++)
  {
    if(airspaces[nr].getId() === id)
    {
      airspace = airspaces[nr];
      break;
    }
  }

  return airspace;
}

function getIndex(arr, value)
{
  var index;
  var deltaLow = 0;
  var deltaHigh;

  index = bSearch(arr, 0, (arr.length - 1), value);

  if(index > 0)
  {
    deltaLow = (value - arr[index - 1]);
  }

  deltaHigh = (arr[index] - value);

  if(((deltaLow > 0) && (deltaHigh > 0)) && (deltaLow < deltaHigh))
  {
    index--;
  }

  return index;
}

function bSearch(arr, low, high, key)
{
  var mid;

  if(low === high)
  {
     mid = low;
  }
  else
  {
    mid = Math.floor((low + high) / 2);

    if(key > arr[mid])
    {
      mid = bSearch(arr, mid + 1, high, key);
    }
    else if(key < arr[mid])
    {
      mid = bSearch(arr, low, mid, key);
    }
  }

  return mid;
}
