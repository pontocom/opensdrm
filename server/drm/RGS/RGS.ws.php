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

//RGS Web Service

include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/RGS/configRGS.inc.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/nusoap/nusoap.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/RSASignature.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/PublicKey.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/RGS/admin/business/RGS_Administrator.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/RGS/Content.class.php");

$NAMESPACE = "http://www.adetti.pt/opensdrmws";

$rgs_server = new soap_server();

$rgs_server->debug_flag=false;
$rgs_server->configureWSDL('opensdrmws', $NAMESPACE);
$rgs_server->wsdl->schemaTargetNamespace = $NAMESPACE;

$rgs_server->register('RGSrequestContentRegistration',
						array('identification'=>'xsd:string',
						'signature_algorithm'=>'xsd:string',
						'hash'=>'xsd:string',
						'file'=>'xsd:string',
						'additional_data'=>'xsd:string',
						'aus_cert'=>'xsd:string',
						'signature'=>'xsd:string'),
						array('status_message'=>'xsd:string',
						'content_id'=>'xsd:string'),
						$NAMESPACE);
//$identification, $signature_algorithm, $hash, $file, $additional_data, $aus_cert, $signature

$rgs_server->register('RGSrequestMetadataRegistration',
						array('identification'=>'xsd:string',
						'signature_algorithm'=>'xsd:string',
						'content_id'=>'xsd:string',
						'metadata'=>'xsd:string',
						'aus_cert'=>'xsd:string',
						'signature'=>'xsd:string'),
						array('status_message'=>'xsd:string'),
						$NAMESPACE);
//$identification, $signature_algorithm, $content_id, $metadata, $aus_cert, $signature

$rgs_server->register('RGSrequestListAvailableContent',
						array('identification'=>'xsd:string',
						'signature_algorithm_identifier'=>'xsd:string',
						'criteria'=>'xsd:string',
						'aus_cert'=>'xsd:string',
						'signature'=>'xsd:string'),
						array('status_message'=>'xsd:string',
						'content'=>'xsd:string'),
						$NAMESPACE);
//$identification, $signature_algorithm_identifier, $criteria, $aus_cert, $signature

$rgs_server->register('RGSrequestListMetadata',
						array('identification'=>'xsd:string',
						'signature_algorithm_identifier'=>'xsd:string',
						'content_id'=>'xsd:string',
						'aus_cert'=>'xsd:string',
						'signature'=>'xsd:string'),
						array('status_message'=>'xsd:string',
						'content_id'=>'xsd:string',
						'metadata'=>'xsd:string'),
						$NAMESPACE);
//$identification, $signature_algorithm_identifier, $content_id, $aus_cert, $signature

// this is a simple test message
$rgs_server->register('RGStestFunction',
						array('param1'=>'xsd:string',
						'param2'=>'xsd:string'),
						array('status_message'=>'xsd:string',
						'real_message' => 'xsd:string'),
						$NAMESPACE);

// FORMAT OF CONTENT ID = urn:mpeg:mpeg21:diid:sss:nnn
// sss identification system and nnn the numbering
// urn:mpeg:mpeg21:diid:doi" and the dinamic part would be for instance "pt/adetti/music/0001".


/*IDEAS:
after registering the content the User need to call a COS service indicating the id of the content to sell, base price, and where it is registered...
*/

/**
* Description: This function is used to register content and issue an unique identification number.
* @param message_type The message type.
* @param identification The identification of the message sender.
* @param signature_algorithm_identifier The identifier of the signature algorithm used to sign the message.
* @param hash The hash value of the original content.
* @param file The small thumbnail or preview file.
* @param additional_data Some additional data that might be usefull for categorizing the content.
* @param aus_cert The AUS certificate.
* @param signature The signature of the message.
* @return array The resulting array of the function call.
*/
function RGSrequestContentRegistration($identification, $signature_algorithm, $hash, $file, $additional_data, $aus_cert, $signature)
{
	//verify data signature and check if it is correct and valid
	//we join all the data in the message in order to verify that the signature is valid
	if($SECURITY_VAR=="ON") {
		$data_to_verify = implode("", array($identification, $signature_algorithm, $hash, $file, $additional_data, $aus_cert));
	}
	// the WS verifies the signature contained in the message
	// using the $identification field as a key retrieves the appropriate public-key
	// WS gets the public-key
	$admin = new RGS_Administrator();
	$status = $admin->checkRGSCert($aus_cert);
	if($status==false) {
		return array (
			"status_message" => "-ERR:Invalid AUS certificate."
			);
	} else {
		// go fetch aus_cert subject public_key
		// this is used to verify the signature in the message
		// $admin->getData();

		$cert = new Certificate();
		$cert->loadXMLcertificate($aus_cert);
		$spk = new PublicKey();
		$spk->loadXMLkey($cert->getSubjectPublicKey());

		// and use it to verify the message signature
		if($SECURITY_VAR=="ON") {
			$sign = new RSASignature();
			$value = $sign->RSA_verify($data_to_verify, $signature, $spk);
		} else {
			$value=true;
		}
		if($value==false) {
			return array(
				"status_message" => "-ERR: Unable to verify the signature."
				);
		} else {
			$content = new Content();
			$content->set($hash, $file, $additional_data);
			$content_id = $content->register();
			if($content_id==false) {
				return array(
					"status_message" => "-ERR: error registering content..."
					);
			} else {
				return array(
					"status_message" => "+OK",
					"content_id" => $content_id
					);
			}
		}
	}
}

/**
* Description: This function is used to register metadata associated to content.
* @param message_type The message type.
* @param identification The identification of the message sender.
* @param signature_algorithm_identifier The identifier of the signature algorithm used to sign the message.
* @param content_id The identification of the content.
* @param metadata The metadata associated to the content.
* @param aus_cert The AUS cert.
* @param signature The signature of the message.
* @return array The resulting array of the function call.
*/
function RGSrequestMetadataRegistration($identification, $signature_algorithm, $content_id, $metadata, $aus_cert, $signature)
{
	//verify data signature and check if it is correct and valid
	//we join all the data in the message in order to verify that the signature is valid
	if($SECURITY_VAR=="ON") {
		$data_to_verify = implode("", array($identification, $signature_algorithm, $content_id, $metadata, $aus_cert));
	}

	// the WS verifies the signature contained in the message
	// using the $identification field as a key retrieves the appropriate public-key
	// WS gets the public-key
	$admin = new RGS_Administrator();
	$status = $admin->checkRGSCert($aus_cert);
	if($status==false) {
		return array (
			"status_message" => "-ERR:Invalid AUS certificate."
			);
	} else {
		// go fetch aus_cert subject public_key
		//$admin->getData();

		//$spk = new PublicKey();
		//$spk->loadXMLkey($admin->cert->getSubjectPublicKey());

		$cert = new Certificate();
		$cert->loadXMLcertificate($aus_cert);
		$spk = new PublicKey();
		$spk->loadXMLkey($cert->getSubjectPublicKey());

		// and use it to verify the message signature
		if($SECURITY_VAR=="ON") {
			$sign = new RSASignature();
			$value = $sign->RSA_verify($data_to_verify, $signature, $spk);
		} else {
			$value=true;
		}

		if($value==false) {
			return array(
				"status_message" => "-ERR: Unable to verify the signature."
				);
		} else {
			// this class is still to be implemented
			$meta = new Content();
			$result = $meta->store_metadata($content_id, $metadata);
			if($result==false) {
				return array(
					"status_message" => "-ERR: error registering metadata..."
					);
			} else {
				return array(
					"status_message" => "+OK"
					);
			}
		}
	}
}

/**
* Description: This function is used to list the available content registered.
* @param message_type The message type.
* @param signature_algorithm_identifier The identifier of the signature algorithm used to sign the message.
* @param criteria The criteria to list the content.
* @param aus_cert The certificate issued by an AUS in which the RGS trusts.
* @param signature The signature of the message.
* @return array The resulting array of the function call.
*/
function RGSrequestListAvailableContent($identification, $signature_algorithm_identifier, $criteria, $aus_cert, $signature)
{
	//verify data signature and check if it is correct and valid
	//we join all the data in the message in order to verify that the signature is valid
	if($SECURITY_VAR=="ON") {
		$data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $criteria, $aus_cert));
	}

	// the WS verifies the signature contained in the message
	// using the $identification field as a key retrieves the appropriate public-key
	// WS gets the public-key
	$admin = new RGS_Administrator();
	$status = $admin->checkRGSCert($aus_cert);
	if($status==false) {
		return array (
			"status_message" => "-ERR:Invalid AUS certificate."
			);
	} else {
		// go fetch aus_cert subject public_key
		// this is used to verify the signature in the message

		$cert = new Certificate();
		$cert->loadXMLcertificate($aus_cert);
		$spk = new PublicKey();
		$spk->loadXMLkey($cert->getSubjectPublicKey());

		// and use it to verify the message signature
		if($SECURITY_VAR=="ON") {
			$sign = new RSASignature();
			$value = $sign->RSA_verify($data_to_verify, $signature, $spk);
		} else {
			$value=true;
		}

		if($value==false) {
			return array(
				"status_message" => "-ERR: Unable to verify the signature."
				);
		} else {
			$content = new Content();
			$result = $content->handleQuery($criteria);
			if($result == false) {
            	return array(
            		"status_message" => "-ERR: Content not available."
            		);
			} else {
            	return array(
            		"status_message" => "+OK",
            		"content" => $result
            		);
			}
		}
	}
}

/**
* Description: This function is used to list the metadata associated with content.
* @param message_type The message type.
* @param identification The identification of the message sender.
* @param signature_algorithm_identifier The identifier of the signature algorithm used to sign the message.
* @param content_id The unique identifier of the content, that will be used to retrieve the metadata for the content.
* @param signature The signature of the message.
* @return array The resulting array of the function call.
*/
function RGSrequestListMetadata($identification, $signature_algorithm_identifier, $content_id, $aus_cert, $signature)
{
	//verify data signature and check if it is correct and valid
	//we join all the data in the message in order to verify that the signature is valid
	if($SECURITY_VAR=="ON") {
		$data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $content_id, $aus_cert));
	}

	// the WS verifies the signature contained in the message
	// using the $identification field as a key retrieves the appropriate public-key
	// WS gets the public-key
	$admin = new RGS_Administrator();
	$status = $admin->checkRGSCert($aus_cert);
	if($status==false) {
		return array (
			"status_message" => "-ERR:Invalid AUS certificate."
			);
	} else {
		// go fetch aus_cert subject public_key
		// this is used to verify the signature in the message

		$cert = new Certificate();
		$cert->loadXMLcertificate($aus_cert);
		$spk = new PublicKey();
		$spk->loadXMLkey($cert->getSubjectPublicKey());

		// and use it to verify the message signature
		if($SECURITY_VAR=="ON") {
			$sign = new RSASignature();
			$value = $sign->RSA_verify($data_to_verify, $signature, $spk);
		} else {
			$value=true;
		}

		if($value==false) {
			return array(
				"status_message" => "-ERR: Unable to verify the signature."
				);
		} else {
			$content = new Content();
			$result = $content->returnMetadata($content_id);
			if($result == false) {
            	return array(
            		"status_message" => "-ERR: Content id :".$content_id.": doesn't exist."
            		);
			} else {
            	return array(
            		"status_message" => "+OK",
            		"content_id" => $content_id,
            		"metadata" => $result
            		);
			}
		}
	}
}

function RGStestFunction($param1, $param2)
{
	return array(
		"status_message" => "message ok",
		"real_message" => "this is the actual real message"
		);
	/*return array(
		"status_message" => "message ok"
	);*/
}

$rgs_server->service($HTTP_RAW_POST_DATA);
exit(0);

?>