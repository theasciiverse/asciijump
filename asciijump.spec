# $Revision: 1.8 $, $Date: 2003/03/02 21:10:14 $
# $Id: asciijump.spec,v 1.8 2003/03/02 21:10:14 loth Exp $
Summary:	asciijump game.
Name:		asciijump
Version:	1.0.2beta
Vendor:		Grzegorz Moskal <eldevarth@nemerle.org>
Release:	1
License:	GPL
Group:		Applications/Games
Source0:	http://asciijump.prv.pl/%{name}-%{version}.tar.gz
BuildRequires:	slang-devel
BuildRoot:	%{tmpdir}/%{name}-%{version}-root-%(id -u -n)

%description
Ski jumping in text mode.

%description -l pl
Skoki narciarskie w trybie tekstowym.

%prep
%setup -q

%build
%configure
%{__make}

%install
rm -rf $RPM_BUILD_ROOT
%{__make} install DESTDIR=$RPM_BUILD_ROOT

%post   -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(644,root,root,755)
%doc README-pl README
%attr(755,root,games) %{_bindir}/asciijump
%attr(755,root,games) %{_bindir}/aj-server
%{_datadir}/asciijump
%{_mandir}/man6/*
%{_applnkdir}/Games/Arcade/asciijump.desktop
%{_pixmapsdir}/asciijump.png
%attr(777,root,games) /var/games/asciijump

%define date	%(echo `LC_ALL="C" date +"%a %b %d %Y"`)
%changelog
* %{date} PLD Team <feedback@pld.org.pl>
All persons listed below can be reached at <cvs_login>@pld.org.pl
