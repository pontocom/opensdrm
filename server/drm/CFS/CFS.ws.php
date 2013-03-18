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

// include nusoap library
require_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["INSTALLATION_DIR"]."/CFS/configCFS.inc.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/nusoap/nusoap.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/Logger.class.php");

include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["INSTALLATION_DIR"]."/CFS/admin/business/CFS_Administrator.class.php");

/// \var SOAP_SERVER $MyServer
/// \brief Object to set up the soap server
$NAMESPACE = "http://www.adetti.pt/opensdrmws";

$cfs_server = new soap_server();

$cfs_server->debug_flag=false;
$cfs_server->configureWSDL('opensdrmws', $NAMESPACE);
$cfs_server->wsdl->schemaTargetNamespace = $NAMESPACE;


// register the functions of the soap server
$cfs_server->register('CFSrequestServerLocation',
						array('id'=>'xsd:string'),
						array('result_message'=>'xsd:string',
						'location'=>'xsd:string'),
						$NAMESPACE);

$cfs_server->register('CFSrequestLocationStorage',
						array('id'=>'xsd:string',
						'location'=>'xsd:string'),
						array('result_message'=>'xsd:string'),
						$NAMESPACE);

$cfs_server->register('CFSrequestLocationDelete',
						array('id'=>'xsd:string'),
						array('result_message'=>'xsd:string'),
						$NAMESPACE);



$op = $HTTP_GET_VARS['op'];
$id = $HTTP_GET_VARS['id'];

// if the request is made directly via HTTP
if(($op == "CFSrequestServerLocation") && $id) {
    $msg = CFSrequestServerLocation($id);
    if($msg["result_message"][0]=='-') {
    	echo "-ERR: location not found...";
    } else {
    	echo $msg["location"];
    	//print_r($msg);
    }
    return;
}

/**
 * @param $id Contains the id of the service
 * @return array The location of the service and the result code
 */
function CFSrequestServerLocation($id) {
	$l = new Logger();
	$l->Log("CFS:CFSrequestServerLocation");
	$l->Log("$id");

	$cfs = new CFS_Administrator();
	$msg = $cfs->getServerLocation($id);

	$l->Log("$msg");

    if($msg[0]=='-') {
    	return array(
    		"result_message" => "-ERR: location not found..."
    		);
    } else {
    	return array(
    		"result_message" => "+OK",
    		"location" => $msg
    		);
    }
}// end CFSrequestServerLocation

function CFSrequestLocationStorage($id, $location) {
	$l = new Logger();
	$l->Log("CFS:CFSrequestLocationStorage");
	$l->Log("$id");
	$l->Log("$location");

	$cfs = new CFS_Administrator();
	$msg = $cfs->addServer($id, $location);

	$l->Log("$msg");

    if($msg[0]=='-') {
    	return array(
    		"result_message" => "-ERR: location not added..."
    		);
    } else {
    	return array(
    		"result_message" => "+OK"
    		);
    }
}// end CFSrequestLocationStorage

function CFSrequestLocationDelete($id) {
	$l = new Logger();
	$l->Log("CFS:CFSrequestLocationDelete");
	$l->Log("$id");

	$cfs = new CFS_Administrator();
	$msg = $cfs->deleteServer($id);

	$l->Log("$msg");

    if($msg[0]=='-') {
    	return array(
    		"result_message" => "-ERR: location not deleted..."
    		);
    } else {
    	return array(
    		"result_message" => "+OK"
    		);
    }
}// end CFSrequestLocationDelete

$cfs_server->service($HTTP_RAW_POST_DATA);
exit();

?>
