/***************************************************************************
 *   Copyright (C) 2005 by Alex Graf                                       *
 *   grafal@sourceforge.net                                                         *
 
// A C++ interface to gnuplot. 
//
// This is a direct translation from the C interface
// written by N. Devillard (which is available from
// http://ndevilla.free.fr/gnuplot/).
//
// As in the C interface this uses pipes and so wont
// run on a system that does'nt have POSIX pipe 
// support
//
// Rajarshi Guha
// <rajarshi@presidency.com>
//
// 07/03/03

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
 ***************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Error.h"
#include "GnuPlot.h"

#define PATH_MAXNAMESZ       4096
#define GP_MAX_TMP_FILES    64
#define GP_TMP_NAME_SIZE    512
#define GP_CMD_SIZE         1024
#define GP_TITLE_SIZE       80


GnuPlot::GnuPlot(void)
{
	char gnuplotExec[] = "/usr/bin/gnuplot";
	bool exist;
	
	m_nplots = 0;
	exist = (access(gnuplotExec, X_OK) == 0);
	
	if(exist)
	{
		m_pGnuPipe = popen(gnuplotExec, "w");
	}
	else
	{
		m_pGnuPipe = NULL;
		Error::verify(false, Error::GNUPLOT_OPEN);
	}

	setStyle("lines");
	execCmd("set mouse");
}

GnuPlot::~GnuPlot()
{
	clear();
	
	if(m_pGnuPipe != NULL)
	{
		pclose(m_pGnuPipe);
	}
}

void GnuPlot::clear()
{  
	uint fileNr;
	     
	if(m_filesToDel.size() > 0)
	{
		for(fileNr=0; fileNr<m_filesToDel.size(); fileNr++)
		{
			remove(m_filesToDel[fileNr].ascii());
		}
		m_filesToDel.clear();
	}
	
	m_nplots = 0;
	
	return;
}

void GnuPlot::setStyle(const QString &style)
{
	if(style != "lines" &&
		style != "points" &&
		style != "linespoints" &&
		style != "impulses" &&
		style != "dots" &&
		style != "steps" &&
		style != "errorbars" &&
		style != "boxes" &&
		style != "boxerrorbars")
	{
		m_style = "points";
	}
	else
	{
		m_style = style;
	}
}

void GnuPlot::execCmd(const QString &cmd)
{
	QString locCmd = cmd;
	
	if(m_pGnuPipe != NULL)
	{
		locCmd += "\n";
		fputs(locCmd.ascii(), m_pGnuPipe);
		fflush(m_pGnuPipe);
	}
}

void GnuPlot::setLabelX(const QString &label)
{
	QString cmdstr;

	cmdstr.sprintf("set xlabel \"%s\"", label.ascii());
	execCmd(cmdstr);
}

void GnuPlot::setLabelY(const QString &label)
{
	QString cmdstr;

	cmdstr.sprintf("set ylabel \"%s\"", label.ascii());
	execCmd(cmdstr);
}

void GnuPlot::setLabelZ(const QString &label)
{
	QString cmdstr;

	cmdstr.sprintf("set zlabel \"%s\"", label.ascii());
	execCmd(cmdstr);
}

// 
// Plots a linear equation (where you supply the
// slope and intercept)
//
void GnuPlot::plotSlope(double a, double b, const QString &title)
{
	QString cmdstr;

	if(m_nplots > 0)
	{
		cmdstr.sprintf("replot %f * x + % f title \"%s\" with %s", a, b, title.ascii(), m_style.ascii());
	}
	else
	{
		cmdstr.sprintf("plot %f * x + % f title \"%s\" with %s", a, b, title.ascii(), m_style.ascii());
	}
	
	setAxisData('x', Float);
	execCmd(cmdstr);
	m_nplots++;
}

//
// Plot an equation which is supplied as a string
// 
void GnuPlot::plotEquation(const QString &equation, const QString &title)
{
	QString cmdstr;
	
	if(m_nplots > 0)
	{
		cmdstr.sprintf("replot %s title \"%s\" with %s", equation.ascii(), title.ascii(), m_style.ascii());
	}
	else
	{
		cmdstr.sprintf("plot %s title \"%s\" with %s", equation.ascii(), title.ascii(), m_style.ascii());
	}

	setAxisData('x', Float);
	execCmd(cmdstr);
	m_nplots++;
}

void GnuPlot::plotX(PlotVectorType &x, const QString &title)
{
	QFile file;
	QString cmdstr;
	QString line;
	uint valueNr;

	if(openTmpFile(file))
	{
		for(valueNr=0; valueNr<x.size(); valueNr++)
		{
			line.sprintf("%f\n", x[valueNr]);
			file.write(line.ascii(), line.length());
		}
		file.close();
	
		if(m_nplots > 0)
		{
			cmdstr.sprintf("replot \"%s\" with %s", file.name().ascii(), m_style.ascii());
		}
		else
		{
			cmdstr.sprintf("plot \"%s\" title \"%s\" with %s", file.name().ascii(), title.ascii(), m_style.ascii());
		}
		
		setAxisData('x', Float);
		execCmd(cmdstr);
		m_nplots++;
	}
}
    
void GnuPlot::plotXY(PlotVectorType &x, PlotVectorType &y, const QString &title)
{
	QFile file;
	QString cmdstr;
	QString line;
	uint valueNr;

	if(openTmpFile(file))
	{
		for(valueNr=0; valueNr<x.size(); valueNr++)
		{
			line.sprintf("%f %f\n", x[valueNr], y[valueNr]);
			file.write(line.ascii(), line.length());
		}
		file.close();
	
		if(m_nplots > 0)
		{
			cmdstr.sprintf("replot \"%s\" with %s", file.name().ascii(), m_style.ascii());
		}
		else
		{
			cmdstr.sprintf("plot \"%s\" title \"%s\" with %s", file.name().ascii(), title.ascii(), m_style.ascii());
		}
		
		setAxisData('x', Float);
		execCmd(cmdstr);
		m_nplots++;
	}
}

void GnuPlot::plotXY(TimeVectorType &x, PlotVectorType &y, const QString &title)
{
	QFile file;
	QString cmdstr;
	QString line;
	uint valueNr;

	if(openTmpFile(file))
	{
		for(valueNr=0; valueNr<x.size(); valueNr++)
		{
			line.sprintf("%s %f\n", x[valueNr].toString(Qt::ISODate).ascii(), y[valueNr]);
			file.write(line.ascii(), line.length());
		}
		file.close();
	
		if(m_nplots > 0)
		{
			cmdstr.sprintf("replot \"%s\" using 1:2 with %s", file.name().ascii(), m_style.ascii());
		}
		else
		{
			cmdstr.sprintf("plot \"%s\" using 1:2 title \"%s\" with %s", file.name().ascii(), title.ascii(), m_style.ascii());
		}
		
		setAxisData('x', Time);
		execCmd(cmdstr);
		m_nplots++;
	}
}

void GnuPlot::plotXYZ(PlotVectorType &x, PlotVectorType &y, PlotVectorType &z, const QString  &title)
{
	QFile file;
	QString cmdstr;
	QString line;
	uint valueNr;
	
	if(openTmpFile(file))
	{
		for(valueNr=0; valueNr<x.size(); valueNr++)
		{
			line.sprintf("%f %f %f\n", x[valueNr], y[valueNr], z[valueNr]);
			file.write(line.ascii(), line.length());
		}
		file.close();
	
		if(m_nplots > 0)
		{
			cmdstr.sprintf("replot \"%s\" title \"%s\" with %s", file.name().ascii(), title.ascii(), m_style.ascii());
		}
		else
		{
			cmdstr.sprintf("splot \"%s\" title \"%s\" with %s", file.name().ascii(), title.ascii(), m_style.ascii());
		}
		
		setAxisData('x', Float);
		execCmd(cmdstr);
		m_nplots++;
	}
}

void GnuPlot::setMinMaxXYZ(double minX, double maxX, double minY, double maxY, double minZ, double maxZ)
{
	QString cmdstr;
	
	cmdstr.sprintf("set xrange [%f:%f]\n", minX, maxX);
	execCmd(cmdstr);
	
	cmdstr.sprintf("set yrange [%f:%f]\n", minY, maxY);
	execCmd(cmdstr);
	
	cmdstr.sprintf("set zrange [%f:%f]\n", minZ, maxZ);
	execCmd(cmdstr);
}

void GnuPlot::setOutput(const QString &name)
{
	int pos;
	QString cmdstr;
	QString term;
	
	// terminal by extension
	pos = name.findRev(".");
	term = name.right(pos+1);
	
	if(term == "png")
	{
		execCmd("set terminal png");
		cmdstr.sprintf("set output \"%s\"", name.ascii());
		execCmd(cmdstr);
	}
	else if(term == "ps")
	{
		execCmd("set terminal ps");
		cmdstr.sprintf("set output \"%s\"", name.ascii());
		execCmd(cmdstr);
	}
	else
	{
		execCmd("set terminal x11");
	}
}

bool GnuPlot::openTmpFile(QFile &file)
{
	char name[] = "/tmp/gnuplotiXXXXXX";
	bool success;

	success = (m_filesToDel.size() < GP_MAX_TMP_FILES);
	
	if(success)
	{
		// create temporary files for output
		success = (mkstemp(name) > 0);
		
		if(success)
		{
			file.setName(name);
			success = file.open(QIODevice::WriteOnly);
			
			if(success)
			{
				m_filesToDel.push_back(name);
			}
		}
	}
	
	return success;
}

void GnuPlot::setAxisData(const char axis, AxisDataType axisData)
{
	QString cmd;
	
	switch(axisData)
	{
		case Time:
			cmd.sprintf("set %cdata time", axis);
			execCmd(cmd);
			execCmd("set timefmt \"%H:%M:%S\"");
			cmd.sprintf("set format %c ", axis);
			cmd += "\"%H:%M:%S\"";
			execCmd(cmd);
		break;
		case Float:
			cmd.sprintf("set %cdata", axis);
			execCmd(cmd);
			cmd.sprintf("set format %c ", axis);
			cmd += "\"%g\"";
			execCmd(cmd);
		break;
	}
}
