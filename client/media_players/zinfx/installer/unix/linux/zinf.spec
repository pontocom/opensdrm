# -------------------------------------------------------------------------
# Global defines
# -------------------------------------------------------------------------
%define name    zinf
%define cname   Zinf
%define version 2.2.0
%define release 2
%define prefix  /usr  

# -------------------------------------------------------------------------
# Main Package defines
# -------------------------------------------------------------------------
Summary:     File and streaming MP3/OggVorbis/Audio CD player/jukebox.
Name:        %{name}
Version:     %{version}
Release:     %{release}
Copyright:   GPL
Group:       Applications/Multimedia
Source:      http://prdownloads.sourceforge.net/zinf/%{name}-%{version}.tar.gz
Prefix:      %{prefix}
BuildRoot:   /var/tmp/%{name}-%{version}-root
ExcludeArch: sparc
AutoReqProv: No 
Requires: freetype >= 2.0.9 musicbrainz >= 1.1.0 glibc >= 2.2.5 libstdc++-libc6.2-2.so.3 XFree86-libs >= 4.2.0 gtk+ >= 1.2.10 glib >= 1.2.10 gtk+ >= 1.2.10 

%description
%{cname} is an MP3/OggVorbis/AudioCD player/jukebox that can play local files,
SHOUTcast/icecast streams and RTP MPEG streams. It features a Themed interface,
which also supports WinAmp and KJofol skins, and it also supports automated
download from RMP enabled music sites like emusic.com.

# -------------------------------------------------------------------------
# ESD Package defines
# -------------------------------------------------------------------------
%package     esd
Summary:     %{cname} - %{cname} plugin for ESD output
Group:       Applications/Multimedia
Requires:    %{name} = %{version}
Requires:    esound >= 0.2.12

%description    esd
%{cname} plugin for ESD output

# -------------------------------------------------------------------------
# Alsa Package defines
# -------------------------------------------------------------------------
%package     alsa
Summary:     %{cname} - %{cname} plugin for ALSA output
Group:       Applications/Multimedia
Requires:    %{name} = %{version}
Requires:    alsa-lib >= 0.5.10

%description alsa
%{cname} plugin for ALSA output

# -------------------------------------------------------------------------
# Arts Package defines
# -------------------------------------------------------------------------
%package     arts
Summary:     %{cname} - %{cname} plugin for Arts output
Group:       Applications/Multimedia
Requires:    %{name} = %{version}
Requires:    arts >= 1.0.0

%description arts
%{cname} plugin for Arts output plugin

# -------------------------------------------------------------------------
# Vorbis Package defines
# -------------------------------------------------------------------------
%package     vorbis
Summary:     %{cname} - %{cname} plugin for Vorbis playback
Group:       Applications/Multimedia
Requires:    %{name} = %{version}
Requires:    libogg >= 1.0rc3, libvorbis >= 1.0rc3

%description vorbis
%{cname} plugin for Vorbis playback

# -------------------------------------------------------------------------
# Extras Package defines
# -------------------------------------------------------------------------
%package     extras
Summary:     Freeamp - All the remaining plugins (mpg123 ui, cmdline ui, ncurses ui, irman, lcd)
Group:       Applications/Multimedia
Requires:    %{name} = %{version}

%description extras
All the remaining plugins (mpg123 ui, cmdline ui, ncurses ui, irman, lcd)

# -------------------------------------------------------------------------
# Corba Package defines
# -------------------------------------------------------------------------
%package     corba
Summary:     Freeamp - Corba remote control interface
Group:       Applications/Multimedia
Requires:    %{name} = %{version}
Requires:    ORBit >= 0.5.13

%description corba
Corba remote control interface plugin

# -------------------------------------------------------------------------
# Prep and build stuff 
# -------------------------------------------------------------------------
%prep
%setup -q -n %{name}-%{version}

%build
./configure --prefix=%{prefix}
make

# -------------------------------------------------------------------------
# Install section
# -------------------------------------------------------------------------
%install
PATH=$PATH:/sbin:/usr/sbin ; export PATH
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{prefix}{/bin,/lib,/share}
mkdir -p $RPM_BUILD_ROOT%{prefix}/lib/%{name}/plugins
mkdir -p $RPM_BUILD_ROOT%{prefix}/share/%{name}/{help,themes} 
mkdir -p $RPM_BUILD_ROOT%{prefix}/share/gnome/apps/Audio
mkdir -p $RPM_BUILD_ROOT%{prefix}/share/pixmaps

install -s %{name} $RPM_BUILD_ROOT%{prefix}/bin
install themes/* $RPM_BUILD_ROOT%{prefix}/share/%{name}/themes
install -s plugins/* $RPM_BUILD_ROOT%{prefix}/lib/%{name}/plugins

install installer/unix/linux/Zinf.desktop $RPM_BUILD_ROOT%{prefix}/share/gnome/apps/Audio
install installer/unix/linux/zinf.png $RPM_BUILD_ROOT%{prefix}/share/pixmaps

%clean
rm -rf $RPM_BUILD_ROOT

%files 
%defattr(-,root,root)
%doc AUTHORS CHANGES COPYING INSTALL NEWS README README.linux
%{prefix}/bin/*
%{prefix}/share/%{name}
%{prefix}/lib/%{name}/plugins/cd.lmc
%{prefix}/lib/%{name}/plugins/cd.pmo
%{prefix}/lib/%{name}/plugins/download.ui
%{prefix}/lib/%{name}/plugins/zinf.ui
%{prefix}/lib/%{name}/plugins/httpinput.pmi
%{prefix}/lib/%{name}/plugins/id3v1.mdf
%{prefix}/lib/%{name}/plugins/id3v2.mdf
%{prefix}/lib/%{name}/plugins/kjofol.ftf
%{prefix}/lib/%{name}/plugins/localfileinput.pmi
%{prefix}/lib/%{name}/plugins/m3u.plf
%{prefix}/lib/%{name}/plugins/misc.mdf
%{prefix}/lib/%{name}/plugins/mbcd.mdf
%{prefix}/lib/%{name}/plugins/musicbrowser.ui
%{prefix}/lib/%{name}/plugins/obsinput.pmi
%{prefix}/lib/%{name}/plugins/pls.plf
%{prefix}/lib/%{name}/plugins/rmp.dlf
%{prefix}/lib/%{name}/plugins/soundcard.pmo
%{prefix}/lib/%{name}/plugins/vorbis.lmc
%{prefix}/lib/%{name}/plugins/vorbis.mdf
%{prefix}/lib/%{name}/plugins/wavout.pmo
%{prefix}/lib/%{name}/plugins/winamp.ftf
%{prefix}/lib/%{name}/plugins/xingmp3.lmc  
%{prefix}/lib/%{name}/plugins/wav.lmc  
%{prefix}/share/gnome/apps/Audio/Zinf.desktop
%{prefix}/share/pixmaps/zinf.png

%files esd 
%defattr(-,root,root)
%{prefix}/lib/%{name}/plugins/esound.pmo

%files alsa 
%defattr(-,root,root)
%{prefix}/lib/%{name}/plugins/alsa.pmo

%files arts 
%defattr(-,root,root)
%{prefix}/lib/%{name}/plugins/arts.pmo

%files vorbis 
%defattr(-,root,root)
%{prefix}/lib/%{name}/plugins/vorbis.lmc
%{prefix}/lib/%{name}/plugins/vorbis.mdf

%files extras 
%defattr(-,root,root)
%{prefix}/lib/%{name}/plugins/irman.ui
%{prefix}/lib/%{name}/plugins/lcd.ui
%{prefix}/lib/%{name}/plugins/zinfcmd.ui
%{prefix}/lib/%{name}/plugins/mpg123.ui
%{prefix}/lib/%{name}/plugins/ncurses.ui

%files corba 
%defattr(-,root,root)
%{prefix}/lib/%{name}/plugins/corba.ui

%changelog
* Thu Jul 25 2002 Hendrik Frenzel <hfrenzel@scunc.net>
- case fixed at zinf.png in the files secton of the spec file
- set the setup path to the untared dir
- made some dirs to install Zinf.desktop and zinf.png
- fixed some filenames (e.g. freeamp.ui->zinf.ui>
* Fri Sep 22 2000 Robert Kaye <rob@freeamp.org>
- Create several smaller rpms, rather than one big one..
* Thu Feb 17 2000 Robert Kaye <rob@freeamp.org>
- conversion from emusic to normal freeamp rpm
* Mon Jan 31 2000 Robert Kaye <rob@freeamp.org>
- conversion from freeamp to emusic
* Wed Jan 12 2000 Tim Powers <timp@redhat.com>
- initial build of freeamp for Powertools
- not origional spec file, violates Red Hat policy as to what can be done in
	the install section of the spec file (not bad, just violates policy)


