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
//AUS DB DIRECTIVES
$MDS_DB_TYPE = "mysql";
$MDS_DB_SERVER = "127.0.0.1";
$MDS_DB_DBNAME = "mdsws_dev";
$MDS_DB_USRNAME = "root";
$MDS_DB_PWD = "";

//// MDS configuration file
$GLOBALS['DEFPATH'] = "download/";
$GLOBALS['DEFSTORE'] = "storage/";
//$GLOBALS['URL'] = "http://192.168.0.100/opensdrmWS/MDS/";
$GLOBALS['URL'] = "http://".gethostbyname($_SERVER['SERVER_NAME']).$GLOBALS['INSTALLATION_DIR']."/MDS/";


//CFS WS DIRECTIVES
$GLOBALS['VERSION'] = "<a href='../../'>OpenSDRM</a> Media Delivery Server 0.2sf ";
$GLOBALS['FOOTER_NOTE'] = "&copy; Adetti 2007 - Produced by ADETTI (Carlos Serrao)";
?>
