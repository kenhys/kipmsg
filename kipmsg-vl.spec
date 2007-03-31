%define name kipmsg
%define version 0.1.4
%define libipmsg_ver 0.1.3
%define release 0vl0

Summary: 	Ip Messenger for KDE
Summary(ja):KDE版 Ipメッセンジャー
Name: 		%{name}
Version: 	%{version}
Release: 	%{release}
Source0: 	%{name}-%{version}.tar.gz
License: 	GPL
Group:		User Interface/Desktops
BuildRoot: 	%{_tmppath}/%{name}-buildroot
Requires: 	kdelibs, kdebase, qt, libipmsg
BuildRequires: 	kdelibs-devel, kdebase-devel, qt-devel, libipmsg-devel = %{libipmsg_ver}

%description
Ip Messenger for KDE is the IpMessenger(famous windows application)'s KDE implementation.

%description -l ja
KDE版Ip Messenger はIpMessenger(Windows上の有名なアプリケーション)のKDE上での実装です。

%prep
%setup -q
%configure --prefix=/usr

%build
make

%install
# cleanup
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc ChangeLog README TODO AUTHORS COPYING
%{_prefix}/bin/kipmsg
%{_prefix}/share/*

%changelog
* Fri Mar 29 2007 Kuninobu Niki <nikikuni@yahoo.co.jp> 0.1.4-0vl0
- New upstream release.

* Wed Feb 28 2007 Kuninobu Niki <nikikuni@yahoo.co.jp> 0.1.3-0vl0
- New upstream release.

* Sat Jan 20 2007 Kuninobu Niki <nikikuni@yahoo.co.jp> 0.1.2-0vl0
- New upstream release.

* Fri Dec 29 2006 Kuninobu Niki <nikikuni@yahoo.co.jp> 0.1.1-0vl0
- New upstream release.

* Sat Nov 18 2006 Kuninobu Niki <nikikuni@yahoo.co.jp> 0.1.0-0vl0
- Initial version
