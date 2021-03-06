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
 *   It is prohibited to server or run this code over network p.e. as web  *
 *   service in combination with closed source.                            *
 ***************************************************************************/

var map = null;
var airspaces = [];
var selectedAirspace = -1;
var airspaceNr;

function as_init()
{
  map = L.map('map');

  L.tileLayer('http://{s}.tile.opentopomap.org/{z}/{x}/{y}.png', {
          maxZoom: 15,
          attribution: 'Map data: &copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>, <a href="http://viewfinderpanoramas.org">SRTM</a> | Map style: &copy; <a href="https://opentopomap.org">OpenTopoMap</a> (<a href="https://creativecommons.org/licenses/by-sa/3.0/">CC-BY-SA</a>)'
  }).addTo(map);

  map.on('load', function()
  {
    wm_emitAppReady();
  });

  map.on('click', function(event)
  {
    var airspaceNr;
    var inside = false;

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
          wm_emitLineChanged(airspaces[airspaceNr].getId());
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

function as_selectAirSpace(id)
{
  var airspaceNr;
  var airspace;

  for(airspaceNr=0; airspaceNr<airspaces.length; airspaceNr++)
  {
    airspace = airspaces[airspaceNr];

    if(airspace.getId() === id)
    {
      as_selectAirSpaceNr(airspaceNr);
      break;
    }
  }
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

function as_setOk(ok)
{
  wm_emitOk(ok);
}
