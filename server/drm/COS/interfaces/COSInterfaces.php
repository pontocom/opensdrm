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
 /*
     +------------------------------------------------+
     |                  music4-you                    |
     |                     (COS)                      |
     +================================================+
     #                 COSInterfaces                  # <------  This referes to this file
     +=================+=====+=====+=====+=====+======+
     |COS_Administrator| AUS | RGS | PGW | LIS |  MDS |
     +-----------------+-----+-----+-----+-----+------+
*/



include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/COS/configCOS.inc.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/nusoap/nusoap.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/AESEncrypt.class.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/KeyGenerator.class.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/RSASignature.class.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/PrivateKey.class.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/PublicKey.class.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/DatabaseHandler.class.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/tool.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/Logger.class.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/COS/admin/business/COS_Administrator.class.php");

// List of interfaces to be used by Music4-You to use the AUS, RGS and PGW Web services
// TEST 1
//print_r(subscribeNormalUser("teste", "teste", "Test User", "Test Location", "test@adetti.pt", ""));
//exit;

// TEST 2
// echo verifyUserLogin("pontocom", "teste");


/*$ausid=1;
$userid=11;

$guid = (($ausid << 22) | $userid);

echo dechex($guid);
echo sprintf("%08x", $guid);
*/

// TEST 3
/*
$results = array();
getUserData("00400002", "cjcs", "teste", $results);
print_r($results);
*/

// TEST 4
//print_r(alterNormalUserData("00400001", "cjcs", "teste", "Pedro Fonseca", "Av. das Rosas Fracas, n.12 - Framboesas de Cima - Almoqurel", "pmmf@xit.com"));

// TEST 5
/*
$oresults = array();
requestPaymentValidation("00400001", "<order><nitems>1</nitems><item_desc>Madonna Music</item_desc><item_price>100 euros</item_price><total>100 euros</total></order>", &$oresults);
echo "<br>Resultados:<br><br>";
print_r($oresults);
*/

// TEST 6
// T-1059231350
/*
$oresults = array();
requestPaymentCapture("T-1059231350", &$oresults);
echo "<br>Resultados:<br><br>";
print_r($oresults);
*/

// TEST LIS
/*
$oresults ="";
requestLicenseProduction("0040001e", "jol", "20040808", &$oresults);
echo $oresults;
*/

// TEST MDS
/*
$oresults ="";
requestContentDelivery("a44a", "aaaabbbbaaaabbbb", &$oresults);
echo $oresults;
*/

//echo hasUserWallet("00400003");
//exit;

//testeAUSMutual("00400022", "joca", "joly");

function testeAUSMutual($uid, $login, $password) {
	$hash=md5("".$login."".$password);
	$AUS_WS = getLocation('AUS');

	$parameters = array(
		"uid" => $uid,
		"login" => $login,
		"hash" => $hash
	);

	//print_r($parameters);

	//$AUS_WS = "$COS_PROTOCOL://$COS_SERVER".""."$COSWS_LOCATION";

	$soapclient = new soapclient($AUS_WS);
	$result = $soapclient->call('AUSrequestMutualValidation', $parameters);

	//print_r($result);
}

// This function checks whether a User has or not been registered yet on another service and has a wallet
function hasUserWallet($uid) {
	$AUS_WS = getLocation('AUS');

	$parameters = array(
		"uid" => $uid
	);

	//print_r($parameters);

	$soapclient = new soapclient($AUS_WS);
	//$soapclient = new soapclient("https://127.0.0.1/WServices/opensdrmWS/AUS/AUS.ws.php");
	$result = $soapclient->call('AUSrequestWalletVerification', $parameters);

	//echo $result;
	//echo $result[0];
	//exit;

	//avaliar o primeiro caracter do resultado...
	if ($result[0]=='-') {
		return false;
	} else {
		return true;
	}
}

// This function is used to call the AUS WS in order to subscribe a noraml user for the M4U web site
// NOTE: To be used by a normal site user, without the possibility to buy content
function subscribeNormalUser($login, $password, $name, $address, $email_address, $arbitrary_data) {
    // Call the web-service
    // function AUSrequestUserSubscription($identification, $signature_algorithm_identifier, $name, $address, $email_address, $authentication_type, $username, $password, $public_key, $arbitrary_data, $signature)

    // arbitrary data should contain the payment details, if any...
	// global $COS_PROTOCOL, $COS_SERVER, $COSWS_LOCATION;
	$AUS_WS = getLocation('AUS');

	//print($AUS_WS);
	//echo "<br><br>";

	$adm = new COS_Administrator();
	$adm->getData();

	if($GLOBALS['SECURITY_VAR']=="ON")	 {
		$data_to_sign = implode("", array($adm->pubkey->getHashOfKey(), "RSA_WITH_MD5", $name, $address, $email_address, "LOGINPASSWORD", $login, $password, "", ""));

		$sign = new RSASignature();
		$signature = $sign->RSA_sign($data_to_sign, $adm->privkey);
	}

	$parameters = array(
		"identification" => $adm->pubkey->getHashOfKey(),
		"signature_algorithm_identifier" => "RSA_WITH_MD5",
		"name" => $name,
		"address" => $address,
		"email_address" => $email_address,
		"authentication_type" => "LOGINPASSWORD",
		"username" => $login,
		"password" => $password,
		"public_key" => "",
		"arbitrary_data" => "",
		"signature" => $signature
	);

	//print_r($parameters);

	//$AUS_WS = "$COS_PROTOCOL://$COS_SERVER".""."$COSWS_LOCATION";

	$soapclient = new soapclient($AUS_WS);
	//$soapclient = new soapclient("https://127.0.0.1/WServices/opensdrmWS/AUS/AUS.ws.php");
	$result = $soapclient->call('AUSrequestUserSubscription', $parameters);

	//avaliar o primeiro caracter do resultado...
	/*if ($result["result_message"][0]=='-') {
		$this->last_error=$result["result_message"];
		return $result["result_message"];
	} else {
		return $result["user_id"];
	}*/
	return $result;
    // returns a unique id for the user

}

// This function is used to verify a login-password pair at the AUS WS
function verifyUserLogin($login, $password) {
    // Call the web-service
    // function AUSrequestAuthentication($identification, $signature_algorithm_identifier, $authentication_type, $username, $password, $signature)
    // $identification = COS id
    // $signature_algorithm_identifier = RSA_MD5
    // $authentication_type = LOGINPASSWORD
    // $username = login
    // $password = password

	//global $COS_PROTOCOL, $COS_SERVER, $COSWS_LOCATION;
	$AUS_WS = getLocation('AUS');

    $adm = new COS_Administrator();
	$adm->getData();


	if($GLOBALS['SECURITY_VAR']=="ON")	 {
		$data_to_sign = implode("", array($adm->pubkey->getHashOfKey(), "RSA_WITH_MD5", "LOGINPASSWORD", $login, $password));

		$sign = new RSASignature();
		$signature = $sign->RSA_sign($data_to_sign, $adm->privkey);
	}

	$parameters = array(
		"identification" => $adm->pubkey->getHashOfKey(),
		"signature_algorithm_identifier" => "RSA_WITH_MD5",
		"authentication_type" => "LOGINPASSWORD",
		"username" => $login,
		"password" => $password,
		"signature" => $signature
	);

	//print_r($parameters);

	//$AUS_WS = "$COS_PROTOCOL://$COS_SERVER".""."$COSWS_LOCATION";

	//echo $AUS_WS;

	$soapclient = new soapclient($AUS_WS);
	$result = $soapclient->call('AUSrequestAuthentication', $parameters);

	//print_r($result);
	//exit;


	//avaliar o primeiro caracter do resultado...
/*	if ($result["result_message"][0]=='-') {
		$this->last_error=$result["result_message"];
		return $result;
	} else {
		return $result;
	}*/
	return $result;

    // returns a unique id for the user
}

function getUserData($uid, $login, $password, &$results) {
	//AUSrequestUserInfo($identification, $signature_algorithm_identifier, $authentication_type, $uid, $username, $password, $signature)
	//global $COS_PROTOCOL, $COS_SERVER, $COSWS_LOCATION;
	$AUS_WS = getLocation('AUS');

    $adm = new COS_Administrator();
	$adm->getData();


	if($GLOBALS['SECURITY_VAR']=="ON")	 {
		$data_to_sign = implode("", array($adm->pubkey->getHashOfKey(), "RSA_WITH_MD5", "LOGINPASSWORD", $uid, $login, $password));

		$sign = new RSASignature();
		$signature = $sign->RSA_sign($data_to_sign, $adm->privkey);
	}

	$parameters = array(
		"identification" => $adm->pubkey->getHashOfKey(),
		"signature_algorithm_identifier" => "RSA_WITH_MD5",
		"authentication_type" => "LOGINPASSWORD",
		"uid" => $uid,
		"username" => $login,
		"password" => $password,
		"signature" => $signature
	);

	//print_r($parameters);

	//$AUS_WS = "$COS_PROTOCOL://$COS_SERVER".""."$COSWS_LOCATION";

	//echo $AUS_WS;

	$soapclient = new soapclient($AUS_WS);
	$result = $soapclient->call('AUSrequestUserInfo', $parameters);

	//print_r($result);

	$results = $result;

	//avaliar o primeiro caracter do resultado...
	if ($result["result_message"][0]=='-') {
		$this->last_error=$result["result_message"];
		return false;
	} else {
		return true;
	}
}

function alterNormalUserData($uid, $login, $password, $name, $address, $email_address) {
	//AUSrequestModifyUserSubscription($identification, $signature_algorithm_identifier, $name, $address, $email_address, $authentication_type, $uid, $username, $password, $arbitrary_data, $signature)
	//global $COS_PROTOCOL, $COS_SERVER, $COSWS_LOCATION;
	$AUS_WS = getLocation('AUS');

    $adm = new COS_Administrator();
	$adm->getData();

	if($GLOBALS['SECURITY_VAR']=="ON")	 {
		$data_to_sign = implode("", array($adm->pubkey->getHashOfKey(), "RSA_WITH_MD5", $name, $address, $email_address, "LOGINPASSWORD", $uid, $login, $password, ""));

		$sign = new RSASignature();
		$signature = $sign->RSA_sign($data_to_sign, $adm->privkey);
	}

	//AUSrequestModifyUserSubscription($identification, $signature_algorithm_identifier, $name, $address, $email_address,
	//$authentication_type, $uid, $username, $password, $arbitrary_data, $signature)
	$parameters = array(
		"identification" => $adm->pubkey->getHashOfKey(),
		"signature_algorithm_identifier" => "RSA_WITH_MD5",
		"name" => $name,
		"address" => $address,
		"email_address" => $email_address,
		"authentication_type" => "LOGINPASSWORD",
		"uid" => $uid,
		"username" => $login,
		"password" => $password,
		"arbitrary_data" => "",
		"signature" => $signature
	);

	// print_r($parameters);

	//$AUS_WS = "$COS_PROTOCOL://$COS_SERVER".""."$COSWS_LOCATION";

	//echo $AUS_WS;

	$soapclient = new soapclient($AUS_WS);
	$result = $soapclient->call('AUSrequestModifyUserSubscription', $parameters);

	//print_r($result);

	//avaliar o primeiro caracter do resultado...
	/*if ($result["result_message"][0]=='-') {
		$this->last_error=$result["result_message"];
		return $result["result_message"];
	} else {
		return 0;
	}*/
	return $result;
}

function listContent($criteria, &$list_results) {
	// receives a criteria from the COS and passes it to the RGS returning the list
	//global $RGS_PROTOCOL, $RGS_SERVER, $RGSWS_LOCATION;

	$RGS_WS = getLocation('RGS');

    $adm = new COS_Administrator();
	$adm->getData();


	if($GLOBALS['SECURITY_VAR']=="ON")	 {
		$data_to_sign = implode("", array($adm->pubkey->getHashOfKey(), "RSA_WITH_MD5", $criteria, $adm->cert->returnXMLcertificate()));

		$sign = $adm->signData($data_to_sign);
	}

	$parameters = array(
		"identification" => $adm->pubkey->getHashOfKey(),
		"signature_algorithm" => "RSA_WITH_MD5",
		"criteria" => $criteria,
		"aus_cert" => $adm->cert->returnXMLcertificate(),
		"signature" => $sign
		);

	//$RGS_WS = "$RGS_PROTOCOL://$RGS_SERVER".""."$RGSWS_LOCATION";
	//print_r($parameters);

	//echo $RGS_WS;

	$soapclient = new soapclient($RGS_WS);

	//echo "connecting to RGS server...<br><br>";
	//flush();

	$result = $soapclient->call('RGSrequestListAvailableContent', $parameters);

	//echo "<br>Results:<br><br>";
	//print_r($result);
	//exit;

	if ($result["result_message"][0]=='-') {
		$this->last_error=$result["result_message"];
		$list_results = $result;
		return false;
	} else {
		$list_results = $result;
		return true;
	}
}

function listMetadata($cid, &$metadata) {
	// receives a content id from the COS, passes to the RGS and returns the metadata associated
	// global $RGS_PROTOCOL, $RGS_SERVER, $RGSWS_LOCATION;
	$RGS_WS = getLocation('RGS');


    $adm = new COS_Administrator();
	$adm->getData();

	if($GLOBALS['SECURITY_VAR']=="ON")	 {
		$data_to_sign = implode("", array($adm->pubkey->getHashOfKey(), "RSA_WITH_MD5", $cid, $adm->cert->returnXMLcertificate()));

		$sign = $adm->signData($data_to_sign);
	}

	$parameters = array(
		"identification" => $adm->pubkey->getHashOfKey(),
		"signature_algorithm" => "RSA_WITH_MD5",
		"content_id" => $cid,
		"aus_cert" => $adm->cert->returnXMLcertificate(),
		"signature" => $sign
		);

	//$RGS_WS = "$RGS_PROTOCOL://$RGS_SERVER".""."$RGSWS_LOCATION";

	//print_r($parameters);
	//flush();

	//echo "connecting to RGS server...<br><br>";
	//flush();

	$soapclient = new soapclient($RGS_WS);

	$result = $soapclient->call('RGSrequestListMetadata', $parameters);

	//echo "<br>Results:<br><br>";
	//print_r($result);

	if ($result["result_message"][0]=='-') {
		$this->last_error=$result["result_message"];
		$metadata = $result;
		return false;
	} else {
		$metadata = $result;
		return true;
	}
}

function requestPaymentValidation($uid, $order, &$oresults) {
	// validates the payment information sent by the user
	// echo "uid=".$uid."<br>";
	// echo "order=".$order."<br>";

	$adm = new COS_Administrator();
	//$adm->requestPGWPaymentClereance("ad008875e058e023a178739b91e381c0","<order><nitems>1</nitems><item_desc>Madonna Music</item_desc><item_price>100 euros</item_price><total>100 euros</total></order>"
	if($adm->requestPGWPaymentClereance($uid, $order, $results)==true) {
		//echo "PAYMENT CLEARENCE OK.";
		$oresults = $results;
		return true;
	} else {
		//echo "PAYMENT CLEARENCE KO.";
		$oresults = $results;
		return false;
	}
}

function requestPaymentCapture($tid, &$oresults) {
	// orders the payment capture through the payment gateway

	$adm = new COS_Administrator();
	if($adm->requestPGWPaymentCapture($tid, $results)==true) {
		//echo "PAYMENT CAPTURE OK.";
		$oresults = $results;
		return true;
	} else {
		//echo "PAYMENT CAPTURE KO.";
		$oresults = $results;
		return false;
	}
}

function requestLicenseProduction($uid, $cid, $license_parameters, &$oresults) {
	// contact the LIS and requests the creation of a license with the specified parameters
	/* function LISrequestLicenseCreation($uid,$cid,$licdata) {global $LicenseManager;
return $LicenseManager->LISrequestLicenseCreation($uid,"$cid",$licdata);} */

	$LIS_WS = getLocation('LIS');

    $soapclient = new soapclient($LIS_WS);

    $licdt = "<licdata><template>1</template><num>6</num><param>".$cid."</param><param></param><param>".$uid."</param><param>".$license_parameters."</param><param></param><param></param></licdata>";

    $parameters = array("uid"=>$uid,"cid"=>$cid,"licdata"=>$licdt);

    //print_r($params);

    $result = $soapclient->call("LISrequestLicenseCreation",$parameters);

	if($result[0] == '+') {
		$oresults = $result;
		return true;
	}
	else {
		$oresults = $result;
		return false;
	}
}

function requestContentDelivery($uid, $cid, &$oresults) {
	// contact the MDS and requests the delivery of a given content to a user
	$MDS_WS = getLocation('MDS');

	//print_r($MDS_WS);

    $client = new soapclient($MDS_WS);

    $params = array("content_id"=>$cid, "user_id"=>$uid);
	//print_r($params);

	//$ns = 'MDSHandler';

    $response = $client->call("MDSrequestContentDelivery",$params);

    $x = $response;
    print_r($x, true);
    $l = new Logger();
    $l->Log("response was = ".$x);


	if($response['0'] == '+') {
		$oresults = $response;
		return true;
	}
	else {
		$oresults = $response;
		return false;
	}
}

function requestContentDownload($uid, $cid, &$oresults) {
	// contact the MDS and requests the delivery of a given content to a user
	$MDS_WS = getLocation('MDS');

	//print_r($MDS_WS);

    $client = new soapclient($MDS_WS);

    $params = array("content_id"=>$cid, "user_id"=>$uid);
	//print_r($params);

	//$ns = 'MDSHandler';

    $response = $client->call("MDSrequestContentDownload",$params);

    //print_r($response);

    //exit;


	if($response['result_message'][0] == '+') {
		$oresults = $response;
		return true;
	}
	else {
		$oresults = $response;
		return false;
	}
}


?>