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
/**
/**
 _     _               _  _____  _____  _____  _____
| |   (_)             | |/ __  \|  _  ||  _  ||  _  |
| |__  _  ___ ___   __| |`' / /'| |/' || |/' || |/' |
| '_ \| |/ __/ _ \ / _` |  / /  |  /| ||  /| ||  /| |
| | | | | (_| (_) | (_| |./ /___\ |_/ /\ |_/ /\ |_/ /
|_| |_|_|\___\___/ \__,_|\_____/ \___/  \___/  \___/

+-+-+-+-+-+-+-+-+
|b|l|a|c|k|b|o|x|
+-+-+-+-+-+-+-+-+

OpenSDRM Integrator
This file "glues" the HICOD2000 solution with the OpenSDRM platform
*/
define("ROOT_PATH", "d:/www");
include_once(ROOT_PATH."/config.inc.php");
require_once(ROOT_PATH.$GLOBALS['LIB_DIR']."/common/tool.php");


/* function RGSrequestContentRegistration(
$identification,
$signature_algorithm,
$hash,
$file,
$additional_data,
$aus_cert,
$signature
)*/
function registerNewContent($_filename, $_aditional_data) {
	if($_filename=='') {
	 writeUsage();
	 return false;
	}

	// calculates the file hash
	$_md5_file_hash = md5_file($_filename);

	echo "FILENAME:".$_filename."<br>";
	echo "FILENAME HASH:".$_md5_file_hash."<br>";

	$RGS_WS = getLocation('RGS');
	echo "Contacting RGS at: ";
	print_r($RGS_WS);
	echo "<br>";

	//exit;

	$parameters = array(
	"identification" => "",
	"signature_algorithm" => "",
	"hash" => $_md5_file_hash,
	"file" => $_filename,
	"additional_data" => $_aditional_data,
	"aus_cert" => "",
	"signature" => ""
	);

	$soapclient = new soapclient($RGS_WS);
	$result = $soapclient->call('RGSrequestContentRegistration', $parameters);

	//echo "Answer received from the web service was:<br>";
	//print_r($result);
	//echo "<br>";

	if ($result["result_message"][0]=='-') {
		//echo "An error has occurred...<br>";
		//echo "ERROR: ".$result["result_message"]."<br><br>";
		return false;
	} else {
		//echo "The content identifier is: ".$result["content_id"]."<br>";;
		return $result["content_id"];
	}
}

/**
$lis_server->register('LISrequestContentKeyStore',
						array('key_list'=>'xsd:string',
						'cid'=>'xsd:string'),
						array('msg'=>'xsd:string'),
						$NAMESPACE);
*/
function registerContentKeys($_cid, $_kl_file) {
	if($_cid=='' || $_kl_file=='') {
		writeUsage();
		return false;
	}

	if(($fp=fopen($_kl_file, "r"))==false) {
		writeUsage();
		echo "Couldn't open the file: ".$_kl_file." for reading.<br><br>";
		return false;
	}

	$klst = "";

    $n=0;
	while(!feof($fp)) {
        $line=fgets($fp);
		$klst .= $n.":".substr($line, 0, strlen($line)-1).":";
        $n++;
	}
	fclose($fp);

	$LIS_WS = getLocation('H2KLIS');
	//echo "Contacting LIS at: ";
	//print_r($LIS_WS);
	//echo "<br>";

	$parameters = array(
		"key_list" => $klst,
		"cid" => $_cid
	);
	//echo "Sending: ";
	//print_r($parameters);
	//echo "<br>";

	$soapclient = new soapclient($LIS_WS);
	$result = $soapclient->call('LISrequestContentKeyStore', $parameters);

	//echo "Answer received from the web service was:<br>";
	//print_r($result);
	//echo "<br>";

	if ($result[0]=='-') {
		//echo "An error has occurred...<br>";
		//echo "ERROR: ".$result."<br><br>";
		return false;
	} else {
		//echo "Keys were stored with success.<br>";;
		return true;
	}
}

function registerContentKey($_cid, $_key) {
	if($_cid=='' || $_key=='') {
		writeUsage();
		return false;
	}

	$LIS_WS = getLocation('LIS');
	echo "Contacting LIS at: ";
	print_r($LIS_WS);
	echo "<br>";

	$parameters = array(
		"key" => $_key,
		"cid" => $_cid
	);
	echo "Sending: ";
	print_r($parameters);
	echo "<br>";

	$soapclient = new soapclient($LIS_WS);
	$result = $soapclient->call('LISrequestContentKeyStore', $parameters);

	echo "Answer received from the web service was:<br>";
	print_r($result);
	echo "<br>";

	if ($result[0]=='-') {
		echo "An error has occurred...<br>";
		echo "ERROR: ".$result."<br><br>";
		return false;
	} else {
		echo "Keys were stored with success.<br>";;
		return true;
	}
}

/**
$lis_server->register('LISrequestLicenseCreation',
						array('uid'=>'xsd:string',
						'cid'=>'xsd:string',
						'licdata'=>'xsd:string'),
						array('msg'=>'xsd:string'),
						$NAMESPACE);
*/
function requestLicenseProduction($_uid, $_cid, $_ldfile) {
	if($_uid=='' || $_cid=='' || $_ldfile=='') {
		writeUsage();
		return false;
	}
	if(($fp=fopen($_ldfile, "r"))==false) {
		writeUsage();
		echo "Couldn't open the file: ".$_ldfile." for reading.<br><br>";
		return false;
	}

	$ldt = "";

	while(!feof($fp)) {
		$ldt .= fgets($fp, 4096);
	}
	fclose($fp);

	$LIS_WS = getLocation('H2KLIS');
	echo "Contacting LIS at: ";
	print_r($LIS_WS);
	echo "<br>";

	$parameters = array(
		"uid" => $_uid,
		"cid" => $_cid,
		"licdata" => $ldt
	);
	echo "Sending: ";
	print_r($parameters);
	echo "<br>";

	$soapclient = new soapclient($LIS_WS);
	$result = $soapclient->call('LISrequestLicenseCreation', $parameters);

	echo "Answer received from the web service was:<br>";
	print_r($result);
	echo "<br>";

	if ($result[0]=='-') {
		echo "An error has occurred...<br>";
		echo "ERROR: ".$result."<br><br>";
		return false;
	} else {
		echo "License was produced with success.<br>";;
		return true;
	}
}

function requestLicenseProductionSpecial($_uid, $_cid) {
    if($_uid=='' || $_cid=='') {
        writeUsage();
        return false;
    }

	/*$licdt = array(
	"license_template" => 1,"urn:mpeg:mpeg21:diid:doi:pt/adetti/music/00000001","","00040001","10","",""
	);*/

	$licdt = "<licdata><template>1</template><num>6</num><param>urn:mpeg:mpeg21:diid:doi:pt/adetti/music/00000001</param><param></param><param>00040001</param><param>3</param><param></param><param></param></licdata>";


	/**
	xml ????

	<licdata>
	<template>1</template>
	<param n="1">urn:mpeg:mpeg21:diid:doi:pt/adetti/music/00000001</param>
	<param n="2"></param>
	<param n="3">00040001</param>
	<param n="4">10</param>
	<param n="5"></param>
	<param n="6"></param>
	</licdata>
	*/
    /*if(($fp=fopen($_ldfile, "r"))==false) {
        writeUsage();
        echo "Couldn't open the file: ".$_ldfile." for reading.<br><br>";
        return false;
    }

    $ldt = "";

    while(!feof($fp)) {
        $ldt .= fgets($fp, 4096);
    }
    fclose($fp);*/

    $LIS_WS = getLocation('LIS');
    echo "Contacting LIS at: ";
    print_r($LIS_WS);
    echo "<br>";

    $parameters = array(
        "uid" => $_uid,
        "cid" => $_cid,
        "licdata" => $licdt
    );
    echo "Sending: ";
    print_r($parameters);
    echo "<br>";

    $soapclient = new soapclient($LIS_WS);
    $result = $soapclient->call('LISrequestLicenseCreation', $parameters);

    echo "Answer received from the web service was:<br>";
    print_r($result);
    echo "<br>";

    if ($result[0]=='-') {
        echo "An error has occurred...<br>";
        echo "ERROR: ".$result."<br><br>";
        return false;
    } else {
        echo "License was produced with success.<br>";;
        return true;
    }
}


function requestLicenseDownload($_uid, $_cid)
{
	if($_uid=='' || $_cid=='') {
		echo "Parameter empty...<br>";
		writeUsage();
		return false;
	}


	$LIS_WS = getLocation('LIS');
	echo "Contacting LIS at: ";
	print_r($LIS_WS);
	echo "<br>";

	$parameters = array(
		"uid" => $_uid,
		"cid" => $_cid,
		"authdata" => ""
	);
	echo "Sending: ";
	print_r($parameters);
	echo "<br>";

	$soapclient = new soapclient($LIS_WS);
	$result = $soapclient->call('LISrequestLicenseDownload', $parameters);

	echo "Answer received from the web service was:<br>";
	print_r($result);
	echo "<br>";

	if ($result["msg"][0]=='-') {
		echo "An error has occurred...<br>";
		echo "ERROR: ".$result["msg"]."<br><br>";
		return false;
	} else {
		echo "License was downloaded with success.<br>";;
		return true;
	}
}

function requestLicenseDownloadFromWallet($_cid) {
	if($_cid=='') {
		echo "Parameter empty...<br>";
		writeUsage();
		return false;
	}

	$address = gethostbyname("localhost");
	$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
	if ($socket < 0) {
		echo "socket_create() failed: reason: " . socket_strerror($socket) . "<br>";
		return false;
	}
	$result = socket_connect($socket, $address, "8001");
	if ($result < 0) {
		echo "socket_connect() failed.<br>Reason: ($result) " . socket_strerror($result) . "<br>";
		return false;
	}

	$in = "GETKEY ".md5($_cid)."\n";
	echo "writing: ".$in."<br>";
	$w = socket_write($socket, $in, strlen($in));
	echo "writed: ".$w."<br>";;

	while ($out = socket_read($socket, 2048)) {
		echo $out;
	}

	echo "Closing socket...";
	socket_close($socket);
	echo "OK.<br><br>";

	return true;
}

function writeUsage() {
	echo "+------------------------------------------------------------------+<br>";
	echo "| opensDRM integrator for HICOD2000                                |<br>";
	echo "| ---------------------------------                                |<br>";
	echo "| Usage:                                                           |<br>";
	echo "| php.exe osdrmIntegrator.php op=[REG_CONT|KEY_REG|LIC_PROD|USAGE] |<br>";
	echo "+------------------------------------------------------------------+<br><br>";
	echo "REG_CONT:  Registers a new image on the OpenSDRM Registration Server.<br>";
	echo "           filename        =  the content file to be registered<br>";
	echo "           additional_data =  an xml file containing additional data about the content file<br><br>";
	echo "KEY_REG:   Registers a key (or set of keys) on the OpenSDRM License Server.<br>";
	echo "           cid             =  the content id to be registered<br>";
	echo "           key         =  a content key to be registered<br><br>";
	echo "LIC_PROD:  Requests a new license to be produced, according to a set of parameters.<br>";
	echo "           uid             =  the user id to be registered<br>";
	echo "           cid             =  the content id to be registered<br>";
	echo "TL_DLND:   Tests the license download.<br>";
	echo "           uid             =  the user id <br>";
	echo "           cid             =  the content id<br>";
	echo "           ad_file         =  auth_data file<br><br>";
	echo "GET_LICW:  Get license from Wallet.<br>";
	echo "           rlvl            =  resolution level <br>";
	echo "           cid             =  the content id<br>";
	echo "USAGE:     Displays this usage screen.<br><br>";
}

/*GETKEY RES_LVL 2 IMG urn:mpeg:mpeg21:diid:doi:pt/adetti/music/00000004*/

/* check the DRM operation to perform */
/*
switch($_REQUEST['op']) {
	case 'REG_CONT': registerNewContent($_REQUEST['filename'], $_REQUEST['additional_data']);
			break;
	case 'KEY_REG': registerContentKeys($_REQUEST['cid'], $_REQUEST['kl_file']);
			break;
	case 'LIC_PROD': requestLicenseProduction($_REQUEST['uid'], $_REQUEST['cid'], $_REQUEST['ld_file']);
			break;
	case 'TL_DLND': requestLicenseDownload($_REQUEST['uid'], $_REQUEST['cid'], $_REQUEST['ad_file']);
			break;
	case 'GET_LICW': requestLicenseDownloadFromWallet($_REQUEST['rlvl'], $_REQUEST['cid']);
			break;
	case 'USAGE': writeUsage();
			break;
	default: writeUsage();
			echo "No valid options supplied...<br>Please enter a valid option...<br><br>";
			break;
}
*/

?>