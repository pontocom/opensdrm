<?
/*
 OpenSDRM - Open Secure Digital Rights Management
 Copyright (c) 2002 Adetti

 This software was originaly developed under the MOSES project (IST-2001-34144)

________                         _________________ __________    _____
\_____  \ ______   ____   ____  /   _____/\______ \\______   \  /     \
 /   |   \\____ \_/ __ \ /    \ \_____  \  |    |  \|       _/ /  \ /  \
/    |    \  |_> >  ___/|   |  \/        \ |    `   \    |   \/    Y    \
\_______  /   __/ \___  >___|  /_______  //_______  /____|_  /\____|__  /
        \/|__|        \/     \/        \/         \/       \/         \/

This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 2.1 of the License, or (at your option)
any later version.

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License along
with this library; if not, write to the Free Software Foundation, Inc., 59
Temple Place, Suite 330, Boston, MA 02111-1307 USA


*/
//General configuration file
// This variable holds the installation dir counting from the DOCUMENT_ROOT dir of the web server
$GLOBALS['INSTALLATION_DIR'] = '/server/drm';
// This variable holds the LIB installation dir from the DOCUMENT_ROOT dir of the web server
$GLOBALS['LIB_DIR'] = "/server/drm/lib";
// Holds the information of the software version
$GLOBALS['VERSION'] = '1.1b';
// Defines wether the security features are "ON" or "OFF"
$GLOBALS['SECURITY_VAR'] = 'OFF';
//ADODB CONFIG
$GLOBALS['DB_TYPE'] = 'mysql';
//CFS WS DIRECTIVES - This is the location of the Configuration Server
$GLOBALS['CFS_LOCATION']="http://127.0.0.1/opensdrm.sf/server/drm/CFS/CFS.ws.php";
?>