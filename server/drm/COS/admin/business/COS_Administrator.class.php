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

//class COS_Administrator
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/COS/configCOS.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/DatabaseHandler.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/PublicKey.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/PrivateKey.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/KeyGenerator.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/Certificate.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/AESEncrypt.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/phplibrary/include.file.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/Logger.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/tool.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/nusoap/nusoap.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/openssl/PrivateKey.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/openssl/X509Certificate.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/openssl/RSADSig.class.php");

class COS_Administrator extends DatabaseHandler
{
	var $db_con;
	var $login;
	var $password;
	var $pubkey;
	var $privkey;
	public $cert;
	public $keypair;

	var $error;

	/**
	 * Constructor of the Class. Just initializes the database connection.
	 */
	function COS_Administrator()
	{
		global $COS_DB_SERVER, $COS_DB_DBNAME, $COS_DB_USRNAME, $COS_DB_PWD;
		$this->db_con = new DatabaseHandler();
		$this->db_con->connectTo($COS_DB_SERVER, $COS_DB_DBNAME, $COS_DB_USRNAME, $COS_DB_PWD);
	}

	/**
	 * Checks if the pair username, password is correct or not.
	 * @param username The username passed.
	 * @param password The password passed.
	 * @return Returns true if the user is valid or false if the user is not valid.
	 */
	function verifyLogin($username, $password){
		$rs=$this->db_con->executeQuery("SELECT * FROM cosWS_Admin WHERE login='$username' AND password='$password'");
		/* verificar resultado */
		if($rs->RecordCount()==0) return false;
		return true;
	}

	function verifyLogin_($username, $password){
		$rs=$this->db_con->executeQuery("SELECT * FROM cosWS_Admin WHERE login='$username' AND password='".md5($password)."'");
		/* verificar resultado */
		if($rs->RecordCount()==0) return false;
		return true;
	}

	/**
	 * Checks if the Admin account is setup or not.
	 * @return Returns true if the admin is setup or false if not.
	 */
	function verifyAdminStatus() {
		$rs=$this->db_con->executeQuery("SELECT COUNT(*) AS admin FROM cosWS_Admin");
		/* verify if true */
		if($rs->fields[0]==0) return false;
		return true;
	}

	/**
	 * Creates a new Administrator account.
	 * @param username The username passed.
	 * @param password The password passed.
	 * @param ksize The size in bits of the key to be generated.
	 */
	function createNewAdminAccount($username, $password, $ksize) {
		//global $COS_PROTOCOL, $COS_SERVER, $COSWS_LOCATION;
		$AUS_WS = getLocation('AUS');


		/*Crete a new keypair */
		$keygen = new KeyGenerator();
		$keygen->RSAKeyGenerate($ksize);

		/* instanciates the public and private keys of this class */
		$this->pubkey = new PublicKey();
		$this->pubkey->set("RSA", $keygen->pubkey->getN(), $keygen->pubkey->getE());

		$this->privkey = new PrivateKey();
		$this->privkey->set("RSA", $keygen->privkey->getN(), $keygen->privkey->getE(), $keygen->privkey->getD(), $keygen->privkey->getP(), $keygen->privkey->getQ());

		/* TODO: Request a COS certificate */
		//$message_type, $arbitrary_data, $public_key, $password

		$soapclient = new soapclient($AUS_WS);

		//echo "".$COS_PROTOCOL."://".$COS_SERVER."".$COSWS_LOCATION;

		$parameters = array (
		"arbitrary_data" => "Commerce Server",
		"public_key" => "".$this->pubkey->returnXMLkey(),
		"password" => "".$password
		);
		$result = $soapclient->call('AUSrequestComponentSubscription', $parameters);
		echo "<br>Parameters:";
		print_r($parameters);
		echo "<br>Results:";
		print_r($result);

		if($result["result_message"]!="+OK") {
			echo "".strstr($result["result_message"], "-ERR:");
			echo "error in web service.<br>";
			return false;
		}

		/* inserts all data on the table */
		$sql = "INSERT INTO cosWS_Admin VALUES ('$username', '$password', '".$this->pubkey->returnXMLkey()."', '".$this->privkey->returnXMLkey()."', '".$result["certificate"]."')";
		$rs=$this->db_con->executeQuery($sql);
		if($rs==false) {
			$this->error = "ERROR: An error has occured while trying to insert data on the database.<br>";
			return false;
		}
		return true;
	}

	function createNewAdminAccount_($username, $password) {
		$l=new Logger();
		$l->Log("COS:COS_Administrator");

		$AUS_WS = getLocation('AUS');
		//global $RGS_PROTOCOL, $RGS_SERVER, $RGSWS_LOCATION;
		//$AUS_WS = getLocation('AUS');

		/*Crete a new keypair */
		$private_key = new opsslPrivateKey();
		$private_key->create();

		$l->Log($private_key->_spKey);

		$soapclient = new nusoap_client($AUS_WS); // read from the configuration file

		//echo "".$RGS_PROTOCOL."://".$RGS_SERVER."".$RGSWS_LOCATION;

		// create a CSR
		$mycsr = new X509Certificate();
		$mycsr->createCSR(array("countryName" => 'PT', "stateOrProvinceName" => 'Lisboa', "localityName" => 'Lisboa', "organizationName" => 'ISCTE', "organizationalUnitName" => 'ADETTI', "commonName" => 'COS', "emailAddress" => 'cos@adetti.pt'),$private_key);


		//determine what is my IP address
		//$ip = gethostbyname($_SERVER['SERVER_NAME']);


		$parameters = array (
		"arbitrary_data" => "<name>Commerce Server</name>",
		"public_key" => "".$mycsr->_scsr,
		"password" => "".$password
		);
		$result = $soapclient->call('AUSrequestComponentSubscription_', $parameters);
		echo "<br>Parameters:";
		print_r($parameters);
		echo "<br>Results:";
		print_r($result);

		if($result["result_message"]!="+OK") {
			echo "".strstr($result["result_message"], "-ERR:");
			echo "error in web service.<br>";
			return false;
		}

		/* inserts all data on the table */
		$sql = "INSERT INTO ".$GLOBALS['COS_DB_DBNAME'].".cosws_admin VALUES ('$username', '".md5($password)."', '".$private_key->_spKey."', '".$result["certificate"]."')";
		$l->Log($sql);

		$rs=$this->db_con->executeQuery($sql);
		if($rs==false) {
			$this->error = "ERROR: An error has occured while trying to insert data on the database.<br>";
			return false;
		}
		return true;

	}


	function returnLastError() {
		return $this->error;
	}

	function getData() {
		$rs=$this->db_con->executeQuery("SELECT * FROM cosWS_Admin");
		/* verificar resultado */
		if($rs->RecordCount()==0) return false;
		else {
			$this->login=$rs->fields[0];
			$this->password=$rs->fields[1];
			$this->pubkey = new PublicKey();
			$this->pubkey->loadXMLkey($rs->fields[2]);
			$this->privkey = new PrivateKey();
			$this->privkey->loadXMLkey($rs->fields[3]);
			$this->cert = new Certificate();
			$this->cert->loadXMLCertificate($rs->fields[4]);
		}
		return true;
	}

	function getData_() {
		$l = new Logger();
		$l->Log("COS:COS_Administrator:getData_");
		//$rs2=$this->db_con->executeQuery("SELECT * FROM ausWS.ausWS_Admin");
		$rs=$this->db_con->executeQuery("SELECT * FROM ".$GLOBALS['COS_DB_DBNAME'].".cosws_admin");

		/* verificar resultado */
		if($rs->RecordCount()==0) return false;
		else {
			$this->login=$rs->fields[0];
			$this->password=$rs->fields[1];
			$kp = $rs->fields[2];
			$l->Log($kp);
			$this->keypair = new opsslPrivateKey();
			$this->keypair->set($kp);
			$cert = $rs->fields[3];
			$l->Log($cert);
			$this->cert = new X509Certificate();
			$this->cert->set($cert);
		}
		return true;
	}


	function exportData() {
		CreateFile($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/COS/admin/exports/publickey.xml", $this->pubkey->returnXMLKey());
		$aes = new AESEncrypt();
		CreateFile($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/COS/admin/exports/privatekey.xml.enc", $aes->AES_encrypt($this->privkey->returnXMLKey(), $this->password));
		CreateFile($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/COS/admin/exports/certificate.xml", $this->cert->returnXMLCertificate());
	}


	function printInternalData() {
		echo "
<table width=\"500\" border=\"1\" cellspacing=\"0\" cellpadding=\"0\">
  <tr bgcolor=\"#000000\" width=500>
    <td colspan=\"2\" with=500><font color=\"#FFFFFF\" size=\"2\" face=\"Verdana, Arial, Helvetica, sans-serif\"><strong>Administrator
      Data</strong></font></td>
  </tr>
  <tr>
    <td width=\"100\" bgcolor=\"#000000\"><font color=\"#FFFFFF\" size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">Username</font></td>
    <td width=500><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">".$this->login."</font></td>
  </tr>
  <tr>
    <td bgcolor=\"#000000\" width=100><font color=\"#FFFFFF\" size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">Public
      Key</font></td>
    <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\"><textarea rows=6 cols=120 disabled>".htmlentities($this->pubkey->returnXMLkey())."</textarea></font></td>
  </tr>
  <tr>
    <td bgcolor=\"#000000\" width=100><font color=\"#FFFFFF\" size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">AUS issued
      Certificate</font></td>
    <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\"><textarea rows=20 cols=120 disabled>".htmlentities($this->cert->returnXMLCertificate())."</textarea></font></td>
  </tr>
</table>";
	}

	function checkCOSCert($cosCert) {
		/* to check the COS Cert, it will be necessary to get the COS public key from the RGS database (certificate) and check the signature of the certificate */
		/* NOTE: if we have more than one AUS, it will be needed to get the ausCert issuer ID and check for the apropriate AUS public key on the database */

		if($this->getData()==false) {
			return false;
		} else {
			$C_COS_cert = new Certificate();
			$C_COS_cert->loadXMLcertificate($ausCert);

			$issuerPublicKey = new PublicKey();
			$issuerPublicKey->loadXMLkey($this->cert->getIssuerPublicKey());

			// verify signature of certificate $ausCert */
			if($GLOBALS['SECURITY_VAR']=="ON") {
				$sign_tool = new RSASignture();
				$sign_tool->RSA_verify($C_AUS_cert->getCertificateBody(), $C_COS_cert->getCertificateSignature(), $issuerPublicKey);
			} else {
				return true;
			}
		}
		return true;
	}

	function getListPGW() {
		// CONTACT AUS TO RETRIEVE LIST OF POSSIBLE PGWs

		//global $COS_PROTOCOL, $COS_SERVER, $COSWS_LOCATION;
		$AUS_WS = getLocation('AUS');

		$this->getData_();

		/*if($GLOBALS['SECURITY_VAR']=="ON") {
			$dtosign=implode("", array($this->pubkey->getHashOfKey(), "RSA_WITH_MD5", "PUBLICKEY"));

			if(($signature = $this->signData($dtosign))==false) {
			echo "error signing information";
			return false;
			}
			}*/


		$soapclient = new nusoap_client($AUS_WS);

		//echo "".$COS_PROTOCOL."://".$COS_SERVER."".$COSWS_LOCATION;

		// AUSrequestListOfPGW($identification, $signature_algorithm_identifier, $authentication_type, $signature)
		$parameters = array (
		"identification" => $this->cert->getCert(1),
		"signature" => base64_encode($this->signData_($this->cert->getCert(1)))
		);
		$result = $soapclient->call('AUSrequestListOfPGW_', $parameters);
//		echo "<br>Parameters:";
//		print_r($parameters);
//		echo "<br>Results:";
//		print_r($result);

		if($result["result_message"]!="+OK") {
			echo "".strstr($result["result_message"], "-ERR:");
			echo "error in web service:".$result["result_message"]."<br>";
			return false;
		} else {
			return $result["list_of_pgw"];
		}
	}

	function subscribePGW($ip_add, $ident) {
		//$l = new Logger("COS");

		$PGW_WS = getLocation('PGW');

		$this->getData();

		//PGWrequestCOSSubscribe($identification, $signature_algorithm_identifier, $certificate, $signature)
		if($GLOBALS['SECURITY_VAR']=="ON") {
			$dtosign=implode("", array($this->pubkey->getHashOfKey(), "RSA_WITH_MD5", $this->cert->returnXMLcertificate()));

			if(($signature = $this->signData($dtosign))==false) {
				echo "error signing information";
				return false;
			}
		}

		// TODO : should consult the CFS to get the PGW IP address
		//$soapclient = new soapclient("http://".$ip_add."/opensdrmWS/PGW/PGW.ws.php"); //REPLACE BY IP
		$soapclient = new soapclient($PGW_WS);
		//echo "http://".$ip_add."/WServices/opensdrmWS/PGW/PGW.ws.php";

		//echo "".$COS_PROTOCOL."://".$COS_SERVER."".$COSWS_LOCATION;

		// AUSrequestListOfPGW($identification, $signature_algorithm_identifier, $authentication_type, $signature)
		$parameters = array (
		"identification" => $this->pubkey->getHashOfKey(),
		"signature_algorithm_identifier" => "RSA_WITH_MD5",
		"certificate" => $this->cert->returnXMLcertificate(),
		"signature" => $signature
		);
		$result = $soapclient->call('PGWrequestCOSSubscribe', $parameters);
		//echo "<br>Parameters:";
		//print_r($parameters);
		//echo "<br>Results:";
		//print_r($result);

		if($result["result_message"]!="+OK") {
			//echo "".strstr($result["result_message"], "-ERR:");
			//echo "error in web service:".$result["result_message"]."<br>";
			return false;
		} else {
			//echo "web service ok:".$result["result_message"]."<br>";

			// register locally the pgw IP
			/* inserts all data on the table */
			$sql = "INSERT INTO cosWS_pgw VALUES ('".$ip_add."', '".$ident."')";

			//$l->Log("COS:COS_Administrator:subscribePGW:sql: ".$sql);

			$rs=$this->db_con->executeQuery($sql);

			if($rs==false) {
				$this->error = "ERROR: An error has occured while trying to insert data on the database.<br>";
				return false;
			}

			return true;
		}
	}

	function requestPGWPaymentClereance($uid, $value, &$results) { //this value, might be an list of itens and the total value or just a simple value (XML)
		//global $COS_PROTOCOL, $COS_SERVER, $COSWS_LOCATION;
		$AUS_WS = getLocation('AUS');
		//$AUS_WS = 'http://127.0.0.1/opensdrm/AUS/AUS.ws.php';
		$PGW_WS = getLocation('PGW');
		//$PGW_WS = 'http://127.0.0.1/opensdrm/PGW/PGW.ws.php';

		$l = new Logger("COS");
		$l->Log("COS:COS_Administrator:requestPGWPaymentClereance");
		$l->Log("COS:COS_Administrator:requestPGWPaymentClereance:value:".$value);
		$l->Log("COS:COS_Administrator:requestPGWPaymentClereance:UID:".$uid);

		$id_arr = $this->getPGWData();
		$pgw_id = $id_arr["identification"];
		$pgw_ip = $id_arr["ip_address"];

		//echo "IP=".$pgw_id.$pgw_ip;

		$l->Log("COS:COS_Administrator:requestPGWPaymentClereance:pgw_id:".$pgw_id);
		$l->Log("COS:COS_Administrator:requestPGWPaymentClereance:pgw_ip:".$pgw_ip);

		//AUSrequestUserPaymentInfo($identification, $signature_algorithm_identifier, $pgw_identification, $user_identification, $value, $signature)

		$this->getData();

		if($GLOBALS['SECURITY_VAR']=="ON") {
			$dtosign=implode("", array($this->pubkey->getHashOfKey(), "RSA_WITH_MD5", $pgw_id, $uid, $value));
			//$l->Log("COS:COS_Administrator:requestPGWPaymentClereance:dtosign:".$dtosign);

			$sig = new RSASignature();
			$signature = $sig->RSA_sign($dtosign, $this->privkey);
		}

		$parameters = array(
		"identification" => $this->pubkey->getHashOfKey(),
		"signature_algorithm_identifier" => "RSA_WITH_MD5",
		"pgw_identification" => $pgw_id,
		"user_identification" => $uid,
		"pvalue" => $value,
		"signature" => $signature
		);


		$soapclient = new soapclient($AUS_WS);

		//echo "".$COS_PROTOCOL."://".$COS_SERVER."".$COSWS_LOCATION;
		$l->Log("COS:COS_Administrator:requestPGWPaymentClereance:parameters:".print_r($parameters, true));
		$l->Log("COS:COS_Administrator:requestPGWPaymentClereance:AUS_location:".print_r($AUS_WS, true));
		$l->Log("COS:COS_Administrator:requestPGWPaymentClereance:PGW_location:".print_r($PGW_WS, true));

		$result = $soapclient->call('AUSrequestUserPaymentInfo', $parameters);
		//echo "<br>Parameters:";
		//print_r($parameters);
		//echo "<br>Results:";
		//print_r($result);
		$l->Log("COS:COS_Administrator:requestPGWPaymentClereance:dtosign:".print_r($result, true));

		if($result["result_message"]!="+OK") {
			//echo "".strstr($result["result_message"], "-ERR:");
			//echo "error in web service:".$result["result_message"]."<br>";
			$results = $result;
			return false;
		} else {
			//echo "web service ok:".$result["result_message"]."<br>";

			// OK: I have the pay data, I can send it to the PGW

			// PGWrequestPaymentClearence($identification, $signature_algorithm_identifier, $data, $signature)
			if($GLOBALS['SECURITY_VAR']=="ON") {
				$dtosign=implode("", array($this->pubkey->getHashOfKey(), "RSA_WITH_MD5", $result["payclearer"]));
				$l->Log("COS:COS_Administrator:requestPGWPaymentClereance:dtosign:".$dtosign);

				$sig = new RSASignature();
				$signature = $sig->RSA_sign($dtosign, $this->privkey);
			}

			$parameters = array(
			"identification" => $this->pubkey->getHashOfKey(),
			"signature_algorithm_identifier" => "RSA_WITH_MD5",
			"data" => $result["payclearer"],
			"signature" => $signature
			);

			//check the PGW IP
			$data = $this->getPGWData();
			$ip_add = $data["ip_address"];

			$soapclient = new soapclient($PGW_WS);
			//$soapclient = new soapclient("http://".$ip_add."/opensdrmWS/PGW/PGW.ws.php");
			//$ws_ip = "http://".$ip_add."/opensdrmWS/PGW/PGW.ws.php";
			//$l->Log("COS:COS_Administrator:requestPGWPaymentClereance:ws_ip:".$ws_ip);
			//$l->Log("COS:COS_Administrator:requestPGWPaymentClereance:this->pubkey:".$this->pubkey->returnXMLkey());

			$result = $soapclient->call('PGWrequestPaymentClearence', $parameters);
			//echo "<br>Parameters:";
			//print_r($parameters);
			//echo "<br>Results:";
			//print_r($result);

			$l->Log("COS:COS_Administrator:requestPGWPaymentClereance:dtosign:".print_r($result, true));

			if($result["result_message"]!="+OK") {
				//echo "".strstr($result["result_message"], "-ERR:");
				//echo "error in web service.<br>";
				$results = $result;
				return false;
			} else {
				$results = $result;
				return true;
			}
		}


		return true;
	}

	function requestPGWPaymentCapture($tid, &$results) {
		//$l = new Logger("COS");
		//$l->Log("COS:COS_Administrator:requestPGWPaymentCapture");
		$PGW_WS = getLocation('PGW');

		$this->getData();

		if($GLOBALS['SECURITY_VAR']=="ON") {
			//PGWrequestPaymentCapture($identification, $signature_algorithm_identifier, $tid, $signature)
			$dtosign=implode("", array($this->pubkey->getHashOfKey(), "RSA_WITH_MD5", $tid));
			//$l->Log("COS:COS_Administrator:requestPGWPaymentCapture:dtosign:".$dtosign);

			$sig = new RSASignature();
			$signature = $sig->RSA_sign($dtosign, $this->privkey);
		}

		$parameters = array(
		"identification" => $this->pubkey->getHashOfKey(),
		"signature_algorithm_identifier" => "RSA_WITH_MD5",
		"tid" => $tid,
		"signature" => $signature
		);

		//check the PGW IP
		$data = $this->getPGWData();
		$ip_add = $data["ip_address"];

		//$soapclient = new soapclient("http://".$ip_add."/opensdrmWS/PGW/PGW.ws.php");
		$soapclient = new soapclient($PGW_WS);
		//$ws_ip = "http://".$ip_add."/opensdrmWS/PGW/PGW.ws.php";
		//$l->Log("COS:COS_Administrator:requestPGWPaymentCapture:ws_ip:".$ws_ip);
		//$l->Log("COS:COS_Administrator:requestPGWPaymentCapture:this->pubkey:".$this->pubkey->returnXMLkey());

		$result = $soapclient->call('PGWrequestPaymentCapture', $parameters);
		//echo "<br>Parameters:";
		//print_r($parameters);
		//echo "<br>Results:";
		//print_r($result);

		if($result["result_message"]!="+OK") {
			//echo "".strstr($result["result_message"], "-ERR:");
			//echo "error in web service.<br>";
			$results = $result;
			return false;
		} else {
			$results = $result;
			return true;
		}
	}

	function getPGWData() { //returns the PGW data
		$rs=$this->db_con->executeQuery("SELECT * FROM cosWS_pgw");
		$pgw_data = array(
		"ip_address" => $rs->fields[0],
		"identification" => $rs->fields[1]
		);
		return $pgw_data;
	}

	// signs data and returns the signature
	function signData($data_to_sign) {
		if($this->getData()==false) {
			return false;
		} else {
			$sign_tool = new RSASignature();
			return $sign_tool->RSA_sign($data_to_sign, $this->privkey);
		}
	}

	function signData_($data_to_sign) {
		if($this->getData_()==false) {
			return false;
		} else {
			$sign_tool = new RSADSig();
			return $sign_tool->sign($data_to_sign, $this->keypair);
		}
	}

	/**
	 * Closes the database access.
	 */
	function closeCOS_Component()
	{
		// close the database access
		$this->db_con->close();
	}
}
