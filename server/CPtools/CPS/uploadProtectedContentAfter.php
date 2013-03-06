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

if(!isset($_REQUEST['go'])) {
?>
<form action="uploadProtectedContentAfter.php" method=post>
Filename: <input type="text" name="file"><br>
Content ID: <input type="text" name="c_id"><br>
<input type="hidden" value="1" name="go">
<input type="submit" value="Enter">
</form>
<?php
}
else {
session_start();

include_once($_SERVER["DOCUMENT_ROOT"] . "/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["INSTALLATION_DIR"]."/CPS/configCPS.inc.php");
require_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['LIB_DIR'] . "/nusoap/nusoap.php");
require_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['LIB_DIR'] . "/common/DatabaseHandler.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['LIB_DIR'] . "/common/RSASignature.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['LIB_DIR'] . "/common/Certificate.class.php");
require_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['LIB_DIR'] . "/common/tool.php");
require_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['LIB_DIR'] . "/common/Logger.class.php");

// VARIABLES
// TODO: MUDAR PARA VARS DE CONFIG
$M4U_SERVER = $GLOBALS_CONFIG['M4U_SERVER'];

//$CPS_DIR_INSTALL = $GLOBALS_CONFIG['CPS_DIR_INSTALL'];

$CONTENT_ID = $_REQUEST['c_id'];
$FILE_TO_UPLOAD = $_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/CPS/output/".$_REQUEST['file'];

$uploadfile = $FILE_TO_UPLOAD;

$fp = fopen($uploadfile, "rb");
$filesize = filesize($uploadfile);

$content = fread($fp, filesize($uploadfile));
$content_base_64 = base64_encode($content);
fclose($fp);

//$sig = new RSASignature();
//$data_to_sign = implode("", array("$identification", "RSA", "$content_id", "$content_base_64", "<content_length>$content_length</content_length>"));
//$signature = $sig -> RSA_sign($data_to_sign, $pk);

/**
 * <part name="identification" type="xsd1:identification"/>
 * <part name="signature-algorithm" type="xsd1:signature-algorithm"/>
 * <part name="signature" type="xsd1:signature"/>
 * <part name="content_id" type="xsd1:content_id"/>
 * <part name="protected_content_file" type="xsd1:filename"/>
 * <part name="content_length" type="xsd1: arbitrary_data "/>
 */

/**
 * $parameters = array("identification" => "$identification",
 * "signature_algorithm_identifier" => "RSA",
 * "signature" => "$signature",
 * "content_id" => "$content_id",
 * "protected_content_file" => array($content_base_64),
 * "content_length" => "120"
 * );
 */
$parameters = array("content_id" => "$CONTENT_ID",
	"protected_content_file" => $content_base_64,
	"filetype" => "trif"
   );

$MDS_WS = getLocation('MDS');
$soapclient = new soapclient($MDS_WS);
$result = $soapclient -> call('MDSrequestContentStorage', $parameters, $ns);
$mystring = $result;
$findme = 'HTTP/1.1 200 OK';
$pos = strpos($mystring, $findme);
if ($pos === 0){
	echo "<BR><BR>It looks an ERROR has occured.";
	exit;
}else{
	echo "<BR><BR>It looks OK.";
}
}
?>
