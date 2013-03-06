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

/* ODRL 1.1*/
/*
<?xml version="1.0" encoding="UTF-8" ?>
<o-ex:rights xmlns:o-ex="http://odrl.net/1.1/ODRL-EX"
			xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
			xmlns:o-dd="http://odrl.net/1.1/ODRL-DD"
			xsi:schemaLocation="http://odrl.net/1.1/ODRL-EX ../schemas/ODRL-EX-11.xsd
			http://odrl.net/1.1/ODRL-DD ../schemas/ODRL-DD-11.xsd">
	<o-ex:asset>
		<o-ex:context>
			<o-dd:uid>urn:ebook.world/999999/ebook/rossi-000001</o-dd:uid>
			<o-dd:name>Why Cats Sleep and We don't</o-dd:name>
		</o-ex:context>
	</o-ex:asset>
	<o-ex:permission>
		<o-dd:print>
			<o-ex:constraint>
				<o-dd:count>2</o-dd:count>
			</o-ex:constraint>
		</o-dd:print>
	</o-ex:permission>
	<o-ex:party>
		<o-ex:context>
			<o-dd:name>Alice</o-dd:name>
		</o-ex:context>
	</o-ex:party>
</o-ex:rights>
*/

/* MPEG-21 REL*/
/*
<?xml version="1.0" encoding="UTF-8" ?>
<r:license xmlns:dsig="http://www.w3.org/2000/09/xmldsig#"
			xmlns:mx="urn:mpeg:mpeg21:2003:01-REL-MX-NS"
			xmlns:r="urn:mpeg:mpeg21:2003:01-REL-R-NS"
			xmlns:sx="urn:mpeg:mpeg21:2003:01-REL-SX-NS"
			xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
			xsi:schemaLocation="urn:mpeg:mpeg21:2003:01-REL-R-NS ../schemas/rel-r.xsd
			urn:mpeg:mpeg21:2003:01-REL-SX-NS ../schemas/rel-sx.xsd
			urn:mpeg:mpeg21:2003:01-REL-MX-NS ../schemas/rel-mx.xsd">
	<r:grant>
		<r:keyHolder>
			<r:info>
				<dsig:KeyName>Alice</dsig:KeyName>
			</r:info>
		</r:keyHolder>
		<mx:print />
		<r:digitalResource>
			<r:nonSecureIndirect URI="urn:ebook.world/999999/ebook/rossi-000001" />
		</r:digitalResource>
		<r:allConditions>
			<sx:exerciseLimit>
				<sx:count>2</sx:count>
			</sx:exerciseLimit>
		</r:allConditions>
	</r:grant>
</r:license>
*/

//LIS Web Service
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["INSTALLATION_DIR"]."/LIS/configLIS.inc.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/nusoap/nusoap.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/RSASignature.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/Logger.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["INSTALLATION_DIR"]."/LIS/admin/business/LIS_Administrator.class.php");

include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/LIS/LicenseTemplateManager.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/LIS/LicenseManager.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/LIS/LicenseProducer.class.php");

$NAMESPACE = "http://www.adetti.pt/opensdrmws";

$lis_server = new soap_server();

$lis_server->debug_flag=false;
$lis_server->configureWSDL('opensdrmws', $NAMESPACE);
$lis_server->wsdl->schemaTargetNamespace = $NAMESPACE;


$lis_server->register('LISrequestContentKeyStore',
						array('key'=>'xsd:string',
						'cid'=>'xsd:string'),
						array('result_message'=>'xsd:string'),
						$NAMESPACE);

$lis_server->register('LISrequestLicenseList',
						array('uid'=>'xsd:string',
						'cid'=>'xsd:string'),
						array('result_message'=>'xsd:string',
						'list_of_licenses'=>'xsd:string'),
						$NAMESPACE);

$lis_server->register('LISrequestLicenseDownload',
						array('uid'=>'xsd:string',
						'cid'=>'xsd:string',
						'authData'=>'xsd:string'),
						array('result_message'=>'xsd:string',
						'license'=>'xsd:string'),
						$NAMESPACE);

$lis_server->register('LISrequestLicenseDownloadSpecial',
						array('uid'=>'xsd:string',
						'cid'=>'xsd:string',
						'authData'=>'xsd:string'),
						array('result_message'=>'xsd:string',
						'license'=>'xsd:string'),
						$NAMESPACE);

$lis_server->register('LISrequestLicenseDelete',
						array('uid'=>'xsd:string',
						'cid'=>'xsd:string'),
						array('result_message'=>'xsd:string'),
						$NAMESPACE);


$lis_server->register('LISrequestLicenseCreation',
						array('uid'=>'xsd:string',
						'cid'=>'xsd:string',
						'licdata'=>'xsd:string'),
						array('result_message'=>'xsd:string'),
						$NAMESPACE);

$lis_server->register('LISrequestLicenseUpdate',
						array('uid'=>'xsd:string',
						'cid'=>'xsd:string',
						'licdata'=>'xsd:string'),
						array('result_message'=>'xsd:string'),
						$NAMESPACE);

$lis_server->register('LISrequestLicensePassing',
						array('uid_src'=>'xsd:string',
						'uid_target'=>'xsd:string',
						'cid'=>'xsd:string',
						'rights'=>'xsd:string'),
						array('result_message'=>'xsd:string'),
						$NAMESPACE);

// stores a key for a specific content id
// and the appropriate template for the licenses
// this will be most probably the content type of license
// LIS will support 3 or 4 on ODRL and MPEG-21 REL
function LISrequestContentKeyStore($key, $cid)
{
	$ltm = new LicenseTemplateManager();
	if($ltm->storeContentKey($key, $cid)==false) {
		return array (
			"result_message" => "-ERR: Error inserting the content key..."
			);
	} else {
		return array (
			"result_message" => "+OK"
			);
	}
}

// request the license listing for a specific user or for a specific content
function LISrequestLicenseList($uid, $cid) {
	$lm = new LicenseManager();
	if(($ll=$lm->requestLicenseList($uid,$cid, $ec))==false) {
		return array (
			"result_message" => "-ERR: Error listing licenses...:".$ec
			);
	} else {
		return array (
			"result_message" => "+OK",
			"list_of_licenses" => $ll
			);
	}
}

// request the license download for a givem user and for a specific content id
// authdata for now is used at low level only
function LISrequestLicenseDownload($uid,$cid,$authData) {
	$lm = new LicenseManager();
	if(($lic=$lm->requestLicenseDownload($uid,$cid, $ec))==false) {
		return array (
			"result_message" => "-ERR: Error downloading licenses...:".$ec
			);
	} else { // if the user has more than one license we return the oldest
		return array (
			"result_message" => "+OK",
			"license" => $lic
			);
	}
}

function LISrequestLicenseDownloadSpecial($uid,$cid,$authData) {
	$lm = new LicenseManager();
	if(($lic=$lm->requestLicenseDownloadSpecial($uid,$cid, $ec))==false) {
		return array (
			"result_message" => "-ERR: Error downloading licenses...:".$ec
			);
	} else { // if the user has more than one license we return the oldest
		return array (
			"result_message" => "+OK",
			"license" => $lic
			);
	}
}


// requests license deletion for a specific user and specific content id
function LISrequestLicenseDelete($uid,$cid) {
	$lm = new LicenseManager();
	if($lm->requestLicenseDelete($uid, $cid, $ec)==false) {
		return array (
			"result_message" => "-ERR: Error deleting license...:".$ec
			);
	} else {
		return array (
			"result_message" => "+OK"
			);
	}
}

// requests the license creation for a specific uid and cid
// licdata is a xml containing several conditions as listed above:
/**
	xml ????

	<licdata>
	<template>1</template>
	<num>6</num>
	<param>urn:mpeg:mpeg21:diid:doi:pt/adetti/music/00000001</param>
	<param></param>
	<param>00040001</param>
	<param>10</param>
	<param></param>
	<param></param>
	</licdata>
	*/
function LISrequestLicenseCreation($uid,$cid,$licdata) {
	$l = new Logger();
	$lm = new LicenseManager();
	if($lm->requestLicenseCreation($uid,$cid,$licdata, $ec)==false) {
		return "-ERR: Error creating the license...:".$ec;
	} else {
		return 	"+OK";
	}
}


// this function is used to modifiy a specific license
function LISrequestLicenseUpdate($uid,$cid,$licdata) {
	$lm = new LicenseManager();
	if($lm->requestLicenseUpdate($uid,$cid,$licdata, $ec)==false) {
		return array (
			"result_message" => "-ERR: Error updating the license...:".$ec
			);
	} else {
		return array (
			"result_message" => "+OK"
			);
	}
}


// this function will handle the license passing requests between users for a given content
// the rights parameter is an array specifying the rights to be passed to the user
// if this parameter is empty, than all the rights are passed
function LISrequestLicensePassing($uid_src, $uid_target, $cid, $rights) {
	// for the time being rights is being ignored
	$lm = new LicenseManager();
	if($lm->requestLicensePassing($uid_src, $uid_target, $cid, $ec)==false) {
		return array (
			"result_message" => "-ERR: Error passing the license...:".$ec
			);
	} else {
		return array (
			"result_message" => "+OK"
			);
	}
}

//print_r($HTTP_GET_VARS);
$op = $HTTP_GET_VARS['op'];
$cid = $HTTP_GET_VARS['cid'];
$uid = $HTTP_GET_VARS['uid'];

$l = new Logger();
$l->Log("op=".$op." cid=".$cid." uid= ".$uid);

if(($op == "LISrequestLicenseDownloadSpecial") && $cid && $uid) {
	if($authData) {
		//echo "estou aqui 1";
		$msg = LISrequestLicenseDownloadSpecial($uid,$cid,$authData);
	} else {
		//echo "estou aqui 2";
		$msg = LISrequestLicenseDownloadSpecial($uid,$cid,$authData);
	}
	if(!is_object($msg)) {
		echo "result_message:+OK\n";
		echo "license:"; echo $msg["license"];
		$l->Log("license: ".$msg["license"]);
	} else {
		echo "result_message:-ERR download error";
	}

	return;
}

$lis_server->service($HTTP_RAW_POST_DATA);
exit();

?>