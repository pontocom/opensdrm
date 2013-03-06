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
// include nusoap.php, must eb in the same directory
/*define("ROOT_PATH", "d:/www"); // THIS MUST BE CHANGED
if(version_compare(phpversion(), "5.0.0")<0) { // version less than 5
	if(!isset($_SERVER["DOCUMENT_ROOT"])) {
		require_once(ROOT_PATH."/config.inc.php");
		require_once(ROOT_PATH.$GLOBALS['LIB_DIR']."/nusoap/nusoap.php");
	} else {
		require_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
		require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/nusoap/nusoap.php");
	}
} else {
	if($_SERVER["DOCUMENT_ROOT"]=="") {
		require_once(ROOT_PATH."/config.inc.php");
		require_once(ROOT_PATH.$GLOBALS['LIB_DIR']."/nusoap/nusoap.php");
	} else {
		require_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
		require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/nusoap/nusoap.php");
	}
}*/

if(isset($_SERVER["DOCUMENT_ROOT"]) && $_SERVER["DOCUMENT_ROOT"]!="") {
	require_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
	require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/nusoap/nusoap.php");
	include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/Logger.class.php");
} else {
	require_once("d:/www/config.inc.php");
	require_once("d:/www/opensdrm/lib/nusoap/nusoap.php");
	include_once("Logger.class.php");
}


/// resolve location from configuration server
function getLocation($sid) {
		$l = new Logger();
        $url = $GLOBALS['CFS_LOCATION'];

        //$cfsc = new soapclient($url);
        $client = new nusoap_client($url);

        $parameters = array("id" => $sid);

        $l->Log(print_r($url, true));
        $l->Log(print_r($parameters, true));

        $result = $client->call("CFSrequestServerLocation",$parameters);

        $l->Log(print_r($parameters, true));
        //echo '<xmp>'.$client->request.'</xmp>';
        //echo '<xmp>'.$client->response.'</xmp>';

        // parse the response and retrieve the location
        $location = $result['location'];

        // print out location or set error message in log fields
        if(empty($location)) return new soap_fault('Server','','location can not be resolved');

        return $location;
}

// perform hex to bin reconversion
function convertHexToBin($msg) {
         // reconversion, verified
        for($a=0;$a<strlen($msg);$a+=2) {
              $sub = substr($msg,$a,2);
              $tmp =  sscanf("$sub","%x");
              if($tmp<=0) $msga = $msga.chr($sub[1]);
              else $msga = $msga.chr($tmp[0]);
        }
        return $msga;
}// end reconvertHexToBin

// perform bin to hex conversion
function convertBinToHex($msg) {
        // conversion, verified
        for($a=0;$a<strlen($msg);$a++) {
              if(ord($msg[$a])<16) {$msga = $msga . "0";}
              $msga = $msga.sprintf("%x",ord($msg[$a]));
        }
        return $msga;
}// end convertBinToHex

//        getLocation('AUS');
?>
