<?php
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
/* TODO: Add code here */
include_once($_SERVER["DOCUMENT_ROOT"] . "/config.inc.php");
require_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['LIB_DIR'] . "/nusoap/nusoap.php");
require_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['LIB_DIR'] . "/common/tool.php");
require_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['LIB_DIR'] . "/common/Logger.class.php");

$aus = new soapclient('http://127.0.0.1/opensdrm/AUS/AUS.ws.php');

$parameters = array(
			'identification'=>'identification',
			'signature_algorithm_identifier'=>'signature_algorithm_identifier',
			'authentication_type'=>'authentication_type',
			'signature'=>'signature'
		);

$result = $aus->call('AUSrequestListOfPGW', $parameters);

echo '<xmp>'.$aus->request.'</xmp>';
echo '<xmp>'.$aus->response.'</xmp>';

print_r($result);

?>