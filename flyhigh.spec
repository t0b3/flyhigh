Name:          flyhigh
License:       GPL-2.0
Group:         Productivity/Databases/Clients
Summary:       A GPS flight device manager
Version:       1.0.0
Release:       1
URL:           http://flyhigh.sourceforge.net/
BuildRoot:     %{_tmppath}/%{name}-%{version}-build
Source0:       %name-%{version}.tar.gz

Obsoletes:     flyhigh < %{version}

%if 0%{?suse_version}
BuildRequires: libQt5Core-devel libQt5WebKit5-devel libQt5Sql-devel libQt5PrintSupport-devel libqt5-qtserialport-devel libQt5Xml-devel libQt5WebKitWidgets-devel
BuildRequires: cmake update-desktop-files libudev-devel jshint
Requires:      gnuplot update-desktop-files
%endif

%if 0%{?fedora}
BuildRequires: qt5-qtbase-devel qt5-qtwebkit-devel qt5-qtserialport-devel 
BuildRequires: cmake gcc-c++ desktop-file-utils libudev-devel jshint
Requires:      gnuplot gnu-free-sans-fonts
%endif


%description
Configuration of GPS flight devices, r/d tasks, r/w/d waypoints, r/w/d routes,
r/w/d CTRs, flight book. Show flight as graphic using gnuplot.
Supported devices: Flytec 5020, 6015, Competino and IQ Basic from Brauniger.


%prep
# extract the source and go into the flyhigh directory
%setup -q

%build

# prepare for out-of-source build
mkdir build
cd build

# build
cmake -DCMAKE_BUILD_TYPE=Release -DFLYHIGH_BIN_DIR=%{_bindir} -DFLYHIGH_APPLNK_DIR=%{_datadir}/applications -DFLYHIGH_ICON_DIR=%{_datadir}/pixmaps -DFLYHIGH_DOC_DIR=%{_docdir}/%{name} ..

# run make
make %{?_smp_mflags}



%install

cd build
make install DESTDIR=$RPM_BUILD_ROOT

# proper position in the start menu
%if 0%{?suse_version}
%suse_update_desktop_file -c %{name} %{name} "Flight device & flight book manager" %{name} %{name} Graphics Database Archiving Utility Viewer
%endif

%if 0%{?fedora}
desktop-file-install --add-category="Database" --delete-original --dir=$RPM_BUILD_ROOT%{_datadir}/applications $RPM_BUILD_ROOT%{_datadir}/applications/%{name}.desktop
%endif


%clean
# clean up the hard disc after build
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png
%doc %{_docdir}/%{name}
%doc %{_docdir}/%{name}/AUTHORS
%doc %{_docdir}/%{name}/COPYING
%doc %{_docdir}/%{name}/README
%doc %{_docdir}/%{name}/TODO
%doc %{_docdir}/%{name}/migrate2dbv2
%doc %{_docdir}/%{name}/xtrSwiss100




%changelog
* Fri Apr 28 2017 - ja_kern@sf.net
- updated to flyhigh-1.0.0

* Tue Jan 06 2015 - ja_kern@sf.net
- updated to flyhigh-0.9.9
- removed support for mandriva, rhel and centos

* Fri Dec 13 2013 - ja_kern@sf.net
- updated to flyhigh-0.9.8

* Sun May 19 2013 - ja_kern@sf.net
- updated to flyhigh-0.9.7

* Wed Mar 20 2013 - ja_kern@sf.net
- updated to flyhigh-0.9.6

* Wed Dec 12 2012 - ja_kern@sf.net
- updated to flyhigh-0.9.5

* Mon Jan 30 2012 - ja_kern@sf.net
- updated to flyhigh-0.9.3

* Tue Jan 03 2012 - ja_kern@sf.net
- updated to flyhigh-0.9.2

* Thu Dec 01 2011 - ja_kern@sf.net
- updated to flyhigh-0.9.1

* Tue Oct 04 2011 - ja_kern@sf.net
- updated to flyhigh-0.9.0

* Tue Feb 21 2011 - ja_kern@sf.net
- updated to flyhigh-0.8.2

* Tue Feb 08 2011 - ja_kern@sf.net
- updated to flyhigh-0.8.1

* Wed Jan 05 2011 - ja_kern@sf.net
- updated to flyhigh-0.8.0

* Tue Nov 09 2010 - ja_kern@sf.net
- support for mandriva

* Mon Nov 08 2010 - ja_kern@sf.net
- support for fedora, rhel and centos

* Mon Oct 11 2010 - ja_kern@sf.net
- initial package created
