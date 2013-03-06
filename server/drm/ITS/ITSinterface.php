<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
"http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<title>Untitled Document</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>

<body>
Add a new IPMP tool:<br>
<form name="form1" method="post" action="<?php echo $PHP_SELF; ?>">
  IPMP Tool ID: 
  <input name="ipmptoolid" type="text" id="ipmptoolid">
  <input type="hidden" name="op" value="add">
  <br>
  IPMP Tool URL: 
  <input name="ipmptoolurl" type="text" id="ipmptoolurl">
  <br>
  IPMP Tool XML Description: 
  <textarea name="ipmptooldesc" id="ipmptooldesc"></textarea>
  <br>
  <input type="submit" name="Submit" value="Enter new IPMP Tool"> 
</form>
Or List IPMP Tools:<br>
<form name="form2" method="post" action="<?php echo $PHP_SELF; ?>">
  <input type="hidden" name="op" value="list">
  <input type="submit" name="Submit2" value="List IPMP Tools">
</form> 
Or Download IPMP Tool:<br>
<form name="form3" method="post" action="<?php echo $PHP_SELF; ?>">
  <input type="hidden" name="op" value="download">
  <input name="ipmptoolid" type="text" id="ipmptoolid">
  <input type="submit" name="Submit3" value="Download IPMP Tool">
</form> 
Or Detail an IPMP Tool:<br>
<form name="form4" method="post" action="<?php echo $PHP_SELF; ?>">
  <input type="hidden" name="op" value="detail">
  <input name="ipmptoolid" type="text" id="ipmptoolid">
  <input type="submit" name="Submit4" value="Detail IPMP Tool">
</form> 
<table width=100% border=1>
<tr>
<td>
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

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/ITS/configITS.inc.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/nusoap/nusoap.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/AESEncrypt.class.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/KeyGenerator.class.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/RSASignature.class.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/PrivateKey.class.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/PublicKey.class.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/DatabaseHandler.class.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/tool.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/Logger.class.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/COS/admin/business/COS_Administrator.class.php");

if($_REQUEST['op']=='list') {
	$ITS_WS = getLocation('ITS');
	$soapclient = new soapclient($ITS_WS);
	$result = $soapclient->call('ITSrequestIPMPToolsList');
	print_r($result);
	if ($result['result_message']=='+OK') {
		echo '<xmp>'.$result['ipmp_tools_list'].'</xmp>';
	}
} else if ($_REQUEST['op']=='add'){
	$ITS_WS = getLocation('ITS');
	$soapclient = new soapclient($ITS_WS);
	$parameters = array (
		"ipmptoolid" => $_REQUEST['ipmptoolid'],
		"ipmptoolurl" => $_REQUEST['ipmptoolurl'],
		"ipmptooldesc" => $_REQUEST['ipmptooldesc']
		);
	$result = $soapclient->call('ITSaddNewIPMPTool', $parameters);
	print_r($result);
} else if ($_REQUEST['op']=='download'){
	$ITS_WS = getLocation('ITS');
	$soapclient = new soapclient($ITS_WS);
	$parameters = array (
		"ipmp_tool_id" => $_REQUEST['ipmptoolid']
		);
	$result = $soapclient->call('ITSrequestIPMPToolDownload', $parameters);
	print_r($result);
} else if ($_REQUEST['op']=='detail'){
	$ITS_WS = getLocation('ITS');
	$soapclient = new soapclient($ITS_WS);
	$parameters = array (
		"ipmp_tool_id" => $_REQUEST['ipmptoolid']
		);
	$result = $soapclient->call('ITSrequestIPMPToolDetails', $parameters);
	print_r($result);
}
?>
</td>
</tr>
</table>
</body>
</html>
