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
// ITS Web Service

include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS['INSTALLATION_DIR']."/ITS/configITS.inc.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/nusoap/nusoap.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/RSASignature.class.php");
//include_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS['INSTALLATION_DIR']."/ITS/ITS_Component.class.php");
//include_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS['INSTALLATION_DIR']."/ITS/ITS_User.class.php");
include_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS['INSTALLATION_DIR']."/ITS/admin/business/ITS_Administrator.class.php");


$NAMESPACE = "http://www.adetti.pt/opensdrmws";

$its_server = new soap_server();

$its_server->debug_flag=false;
$its_server->configureWSDL('opensdrmws', $NAMESPACE);
$its_server->wsdl->schemaTargetNamespace = $NAMESPACE;


$its_server->register('ITSrequestIPMPToolsList',
						array(),
						array('result_message'=>'xsd:string',
						'ipmp_tools_list'=>'xsd:string'),
						$NAMESPACE);

$its_server->register('ITSrequestIPMPToolDownload',
						array('ipmp_tool_id'=>'xsd:string'),
						array('result_message'=>'xsd:string',
						'ipmp_tool_url'=>'xsd:string'),
						$NAMESPACE);

$its_server->register('ITSaddNewIPMPTool',
						array('ipmptoolid'=>'xsd:string',
						'ipmptoolurl'=>'xsd:string',
						'ipmptooldesc'=>'xsd:string'),
						array('result_message'=>'xsd:string'),
						$NAMESPACE);

$its_server->register('ITSrequestIPMPToolDetails',
						array('ipmptoolid'=>'xsd:string'),
						array('result_message'=>'xsd:string',
						'ipmptoolid'=>'xsd:string',
						'ipmptoolurl'=>'xsd:string',
						'ipmptooldesc'=>'xsd:string'),
						$NAMESPACE);



/**
* Description: This function is used to list all the available IPMP tools on the server.
* @param message_type The message type.
* @param identification The identification of the message sender.
* @param signature_algorithm The identifier of the signature algorithm used to sign the message.
* @param signature The signature of the message.
* @return array The resulting array of the function call.
*/
function ITSrequestIPMPToolsList(/*$identification, $signature_algorithm, $signature*/)
{
	$itsadmin = new ITS_Administrator();
	$result = $itsadmin->listIPMPtools();

	$itsadmin->closeITS_Administrator();

	// now we need to check what is the result value
	if($result==-1) { // an error has occured and I couldnt find any IPMP tools
		return array (
			"result_message" => "-ERR: There are no IPMP tools registered on the system..."
		);
	} else { // the IPMP tools was found and the list of IPMP Tools IDs will be returned
		return array(
			"result_message" => "+OK",
			"ipmp_tools_list" => $result
			);
	}
}



/**
* Description: This function is used to list all the available IPMP tools on the server.
* @param message_type The message type.
* @param identification The identification of the message sender.
* @param signature_algorithm The identifier of the signature algorithm used to sign the message.
* @param ipmp_tool_id The identifier of the IPMP tool.
* @param signature The signature of the message.
* @return array The resulting array of the function call.
*/
function ITSrequestIPMPToolDownload(/*$identification, $signature_algorithm, */$ipmp_tool_id/*, $signature*/)
{
	if($ipmp_tool_id=='') {
		// IPMP tool ID parameter is empty, so we must return an error message
		return array(
			"result_message" => "-ERR: Didn't received the IPMP_TOOL_ID parameter..."
			);
	} else { // ok, we need to check if this ID is on the database
		$itsadmin = new ITS_Administrator();
		$result = $itsadmin->checkIPMPtool($ipmp_tool_id);

		$itsadmin->closeITS_Administrator();

		// now we need to check what is the result value
		if($result==-1) { // an error has occured and I couldnt find the IPMP tool
			return array (
				"result_message" => "-ERR: IPMP Tool couldn't be found..."
			);
		} else { // the IPMP tool was found and the url will be returned
			return array(
				"result_message" => "+OK",
				"ipmp_tool_url" => $result
				);
		}
	}
}

function ITSaddNewIPMPTool($ipmptoolid, $ipmptoolurl, $ipmptooldesc)
{
	if($ipmptoolid=='') {
		// IPMP tool ID parameter is empty, so we must return an error message
		return array(
			"result_message" => "-ERR: Missing IPMP tool ID parameter..."
			);
	}
	if($ipmptoolurl=='') {
		// IPMP tool ID parameter is empty, so we must return an error message
		return array(
			"result_message" => "-ERR: Missing IPMP tool URL parameter..."
			);
	}
	if($ipmptooldesc=='') {
		// IPMP tool ID parameter is empty, so we must return an error message
		return array(
			"result_message" => "-ERR: Missing IPMP tool DESC parameter..."
			);
	}

	$itsadmin = new ITS_Administrator();
	$result = $itsadmin->addnewIPMPtool($ipmptoolid, $ipmptoolurl, $ipmptooldesc);

	$itsadmin->closeITS_Administrator();

	// now we need to check what is the result value
	if($result!=1) { // an error has occured and I couldnt find any IPMP tools
		return array (
			"result_message" => "-ERR: There was an error adding the IPMP tool to the system...".$result
		);
	} else { // the IPMP tools was found and the list of IPMP Tools IDs will be returned
		return array(
			"result_message" => "+OK"
			);
	}
}

function ITSrequestIPMPToolDetails($ipmptoolid) {
	if($ipmptoolid=='') { //ok, the parameter is empty, so we return an error message
		return array(
			"result_message" => "-ERR: Missing the IPMP tool parameter..."
			);
	}
	$itsadmin = new ITS_Administrator();
	$result = $itsadmin->checkIPMPtoolDetails($ipmptoolid);

	$itsadmin->closeITS_Administrator();

	// now we need to check what is the result value
	if($result==-1) { // an error has occured and I couldnt find the IPMP tool
		return array (
			"result_message" => "-ERR: IPMP Tool couldn't be found..."
		);
	} else { // the IPMP tool was found and the url will be returned
		return array(
			"result_message" => "+OK",
			"ipmptoolid" => $result['ipmptoolid'],
			"ipmptoolurl" => $result['ipmptoolurl'],
			"ipmptooldesc" => $result['ipmptooldesc']
			);
	}
}

$its_server->service($HTTP_RAW_POST_DATA);
exit();
?>