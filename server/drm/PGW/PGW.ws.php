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
//PGW Web Service

include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS['INSTALLATION_DIR']."/PGW/configPGW.inc.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/nusoap/nusoap.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/RSASignature.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/Logger.class.php");
include_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS['INSTALLATION_DIR']."/PGW/admin/business/PGW_Administrator.class.php");

$NAMESPACE = "http://www.adetti.pt/opensdrmws";

$pgw_server = new soap_server();

$pgw_server->debug_flag=false;
$pgw_server->configureWSDL('opensdrmws', $NAMESPACE);
$pgw_server->wsdl->schemaTargetNamespace = $NAMESPACE;


$pgw_server->register('PGWrequestPaymentClearence',
						array('identification'=>'xsd:string',
						'signature_algorithm_identifier'=>'xsd:string',
						'data'=>'xsd:string',
						'signature'=>'xsd:string'),
						array('result_message'=>'xsd:string',
						'transaction_number'=>'xsd:string'),
						$NAMESPACE);
//$identification, $signature_algorithm_identifier, $data, $signature

$pgw_server->register('PGWrequestPaymentCapture',
						array('identification'=>'xsd:string',
						'signature_algorithm_identifier'=>'xsd:string',
						'tid'=>'xsd:string',
						'signature'=>'xsd:string'),
						array('result_message'=>'xsd:string',
						'transaction_number'=>'xsd:string'),
						$NAMESPACE);
//$identification, $signature_algorithm_identifier, $tid, $signature

$pgw_server->register('PGWrequestCOSSubscribe',
						array('identification'=>'xsd:string',
						'signature_algorithm_identifier'=>'xsd:string',
						'certificate'=>'xsd:string',
						'signature'=>'xsd:string'),
						array('result_message'=>'xsd:string',
						'cert'=>'xsd:string'),
						$NAMESPACE);
//$identification, $signature_algorithm_identifier, $certificate, $signature

//	<message name="requestPaymentClearence">
//		<part name="message_type" type="xsd1:message_type"/>
//		<part name="identification" type="xsd1:identification"/>
//		<part name="signature-algorithm" type="xsd1:signature-algorithm"/>
//		<part name="certificate" type="xsd1:certificate"/>
//		<part name="value" type="xsd1:value"/>
//		<part name="signature" type="xsd1:signature"/>
//	</message>
//	<message name="requestPaymentClearenceResult">
//		<part name="message_type" type="xsd1:message_type"/>
//		<part name="identification" type="xsd1:identification"/>
//		<part name="signature-algorithm" type="xsd1:signature-algorithm"/>
//		<part name="arbitrary_data" type="xsd1:arbitrary_data"/>
//		<part name="signature" type="xsd1:signature"/>
//	</message>


/*
      <message name="PGWrequestCOSSubscribe">
		<part name="identification" type="xsd1:identification"/>
		<part name="signature-algorithm" type="xsd1:signature-algorithm"/>
		<part name="signature" type="xsd1:signature"/>
		<part name="public_key" type="xsd1:arbitrary_data"/>
		<part name="name" type="xsd1:arbitrary_data"/>
		<part name="address" type="xsd1:arbitrary_data"/>
      </message>
     <message name="PGWrequestCOSSubscribeResult">
		<part name="identification" type="xsd1:identification"/>
		<part name="signature-algorithm" type="xsd1:signature-algorithm"/>
		<part name="signature" type="xsd1:signature"/>
		<part name="certificate" type="xsd1:arbitrary_data"/>
      </message>

*/

/**
* Description: This function is used to verify the payment quality of the payment instrument supplied by the user. It does only this verification, but doesn't get the money transfer.
* @param message_type The message type.
* @param identification The identification of the message sender.
* @param signature_algorithm_identifier The identifier of the signature algorithm used to sign the message.
* @param certificate The certificated conteining the appropriate payment method data.
* @param value The value to be collected from the payment instrument.
* @param signature The signature of the message.
* @return array The resulting array of the function call.
*/
function PGWrequestPaymentClearence($identification, $signature_algorithm_identifier, $data, $signature)
{
	//$l = new Logger("PGW");
	//verify data signature and check if it is correct and valid
	if($GLOBALS['SECURITY_VAR']=="ON") {
		$data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $data));
	}
	//$l->Log("PGW:PGW.ws.php:PGWrequestPaymentClearence:data_to_verify: ".$data_to_verify);
	$admin = new PGW_Administrator();
	if($admin->verifySignature($identification, $data_to_verify, $signature)==true) {
		if(($tn=$admin->saveTransactionInformation($identification, $data))!=false) {
    		return array(
    			"result_message" => "+OK",
    			"transaction_number" => $tn
    			);
		} else{
    		return array(
    			"result_message" => "-ERR: Error while registering transaction..."
    			);
		}
	} else {
		return array(
			"result_message" => "-ERR: Error verifying the signature..."
			);
	}
}

//	<message name="requestPaymentCapture">
//		<part name="message_type" type="xsd1:message_type"/>
//		<part name="identification" type="xsd1:identification"/>
//		<part name="signature-algorithm" type="xsd1:signature-algorithm"/>
//		<part name="certificate" type="xsd1:certificate"/>
//		<part name="value" type="xsd1:value"/>
//		<part name="signature" type="xsd1:signature"/>
//	</message>
//	<message name="requestPaymentCaptureResult">
//		<part name="message_type" type="xsd1:message_type"/>
//		<part name="identification" type="xsd1:identification"/>
//		<part name="signature-algorithm" type="xsd1:signature-algorithm"/>
//		<part name="arbitrary_data" type="xsd1:arbitrary_data"/>
//		<part name="signature" type="xsd1:signature"/>
//	</message>

/**
* Description: After successfull validation of the payment instrument the transaction must be completed by calling this function.
* @param message_type The message type.
* @param identification The identification of the message sender.
* @param signature_algorithm_identifier The identifier of the signature algorithm used to sign the message.
* @param certificate The certificated conteining the appropriate payment method data.
* @param value The value to be collected from the payment instrument.
* @param signature The signature of the message.
* @return array The resulting array of the function call.
*/
function PGWrequestPaymentCapture($identification, $signature_algorithm_identifier, $tid, $signature)
{
	//verify data signature and check if it is correct and valid
	//$l = new Logger("PGW");
	//verify data signature and check if it is correct and valid
	if($GLOBALS['SECURITY_VAR']=="ON") {
		$data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $tid));
	}
	//$l->Log("PGW:PGW.ws.php:PGWrequestPaymentCapture:data_to_verify: ".$data_to_verify);
	$admin = new PGW_Administrator();
	if($admin->verifySignature($identification, $data_to_verify, $signature)==true) {
		//$l->Log("PGW:PGW.ws.php:PGWrequestPaymentCapture:signature is OK... continuing...");
		if(($admin->checkTransactionStatus($identification, $tid))==false) {
    		return array(
    			"result_message" => "-ERR: Transaction doesn't exist..."
    			);
		}

		if(($tn=$admin->modifyTransactionInformation($identification, $tid))!=false) {
    		return array(
    			"result_message" => "+OK",
    			"transaction_number" => $tn
    			);
		} else{
    		return array(
    			"result_message" => "-ERR: Error while capturing transaction..."
    			);
		}
	} else {
		return array(
			"result_message" => "-ERR: Error verifying the signature..."
			);
	}
}


/*
      <message name="PGWrequestCOSSubscribe">
		<part name="identification" type="xsd1:identification"/>
		<part name="signature-algorithm" type="xsd1:signature-algorithm"/>
		<part name="signature" type="xsd1:signature"/>
		<part name="public_key" type="xsd1:arbitrary_data"/>
		<part name="name" type="xsd1:arbitrary_data"/>
		<part name="address" type="xsd1:arbitrary_data"/>
      </message>
     <message name="PGWrequestCOSSubscribeResult">
		<part name="identification" type="xsd1:identification"/>
		<part name="signature-algorithm" type="xsd1:signature-algorithm"/>
		<part name="signature" type="xsd1:signature"/>
		<part name="certificate" type="xsd1:arbitrary_data"/>
      </message>

*/
function PGWrequestCOSSubscribe($identification, $signature_algorithm_identifier, $certificate, $signature)
{
	//TRICKY: Uma vez que o PGW não conhece o COS, este recebe o certificado emitido pelo AUS sobre o COS,
	//vai verificar a assinatura do certificado com a chave pública do AUS, e depois vai verificar a
	//assinatura da mensagem com a chave pública do AUS contida no certificado, UFA.

	//VERIFY THE AUS-COS CERTIFICATE TRUST
	//CHECK CERTIFICATE SIGNATURE
	$admin = new PGW_Administrator();
	if($admin->checkPGWCert($certificate)==false) {
		return array(
			"result_message" => "-ERR: Invalid AUS certificate...",
			"cert" => $certificate
			);
	} else {
		//OK, THE CERTIFICATE SIGNATURE IS CORRECT
		//NOW CHECK THE MESSAGE SIGNATURE
    	//verify data signature and check if it is correct and valid
    	$data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $certificate));
		if($admin->verifyMessageSignature($data_to_verify, $signature, $certificate)==true) {
    		//IF SIGNATURE IS OK, THE REGISTER COS AND RETURN STATUS
			if($admin->registerCOS($identification, $certificate)==true) {
        		return array(
        			"result_message" => "+OK"
        			);
			} else {
        		return array(
        			"result_message" => "-ERR: Error in database insertion..."
        			);
			}
		} else {
    		return array(
    			"result_message" => "-ERR: Invalid signature in message..."
    			);
		}
	}

}


$pgw_server->service($HTTP_RAW_POST_DATA);
exit(0);

?>
