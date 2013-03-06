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


//MDS Web Service
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["INSTALLATION_DIR"]."/MDS/configMDS.inc.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/nusoap/nusoap.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/RSASignature.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/Logger.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["INSTALLATION_DIR"]."/MDS/admin/business/MDS_Administrator.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["INSTALLATION_DIR"]."/MDS/ContentStorageService.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["INSTALLATION_DIR"]."/MDS/ContentDeliveryService.class.php");

$NAMESPACE = "http://www.adetti.pt/opensdrmws";

$mds_server = new soap_server();

$mds_server->debug_flag=false;
$mds_server->configureWSDL('opensdrmws', $NAMESPACE);
$mds_server->wsdl->schemaTargetNamespace = $NAMESPACE;


$mds_server->register('MDSrequestContentStorage',
						array('cid'=> 'xsd:string',
						'filetype'=> 'xsd:string',
						'protocol'=> 'xsd:string',
						'location'=> 'xsd:string'),
						array('result_message'=> 'xsd:string'),
						$NAMESPACE);

$mds_server->register('MDSrequestContentDelivery',
						array('content_id'=> 'xsd:string',
						'user_id'=> 'xsd:string'),
						array('result_message'=> 'xsd:string'),
						$NAMESPACE);

$mds_server->register('MDSrequestContentDownload',
						array('content_id'=> 'xsd:string',
						'user_id'=> 'xsd:string'),
						array('result_message'=> 'xsd:string',
						'url' => 'xsd:string'),
						$NAMESPACE);

$mds_server->register('MDSrequestContentDelete',
						array('content_id'=> 'xsd:string'),
						array('result_message'=> 'xsd:string'),
						$NAMESPACE);

$mds_server->register('MDSrequestDirectoryList',
						array(),
						array('result_message'=> 'xsd:string',
						'clist'=> 'xsd:string'),
						$NAMESPACE);

$mds_server->register('MDSrequestUpdate',
						array('cid'=> 'xsd:string',
						'filetype'=> 'xsd:string',
						'protocol'=> 'xsd:string',
						'location'=> 'xsd:string'),
						array('result_message'=> 'xsd:string'),
						$NAMESPACE);

function MDSrequestContentStorage($cid, $filetype, $protocol, $location) {
	$css = new ContentStorageService();
	if($css->storeContent($cid, $filetype, $protocol, $location, $ec)==false) {
		return array (
			"result_message" => "-ERR: Error while storing the content..."
			);
	} else {
		return array (
			"result_message" => "+OK"
			);
	}
}

function MDSrequestContentDelivery($content_id, $user_id) {
	$cds = new ContentDeliveryService();
	if($cds->deliverContent($content_id, $user_id, $ec)==false) {
		return "-ERR: Error while notifying the delivery service...";
	} else {
		return "+OK";
	}
}

function MDSrequestContentDownload($content_id, $user_id) {
	$cds = new ContentDeliveryService();
	if($cds->downloadContent($content_id, $user_id, $lurl, $ec)==false) {
		return array (
			"result_message" => "-ERR: Error while notifying the download service...:".$ec
			);
	} else {
		return array (
			"result_message" => "+OK",
			"url" => $lurl
			);
	}
}

function MDSrequestContentDelete($content_id) {
	$css = new ContentStorageService();
	if($css->deleteContent($content_id, $ec)==false) {
		return array (
			"result_message" => "-ERR: Error while deleting the content..."
			);
	} else {
		return array (
			"result_message" => "+OK"
			);
	}
}

function MDSrequestDirectoryList() {
	$css = new ContentStorageService();
	if(($lc=$css->listContent())==false) {
		return array (
			"result_message" => "-ERR: Error while listing the content..."
			);
	} else {
		return array (
			"result_message" => "+OK",
			"clist" => $lc
			);
	}
}

function MDSrequestUpdate($cid, $filetype, $protocol, $location) {
	$css = new ContentStorageService();
	if($css->updateContent($cid, $filetype, $protocol, $location, $ec)==false) {
		return array (
			"result_message" => "-ERR: Error while updating the content..."
			);
	} else {
		return array (
			"result_message" => "+OK"
			);
	}
}

$op  = $HTTP_GET_VARS['op'];
$cid = $HTTP_GET_VARS['cid'];
$uid = $HTTP_GET_VARS['uid'];

if(($op == "MDSrequestContentDownload") && $cid && $uid) {
	$msg = MDSrequestContentDownload($uid,$cid);

	if(!is_object($msg)) {
	        echo "result_message:OK+";
	        echo '<xmp>'.$msg.'</xmp>';
	}
	else
	        echo "result_message:-ERR download error";
	return;
}


$mds_server->service($HTTP_RAW_POST_DATA);
exit();

?>