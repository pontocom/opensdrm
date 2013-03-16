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
//class LIS_Administrator

include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/LIS/configLIS.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/DatabaseHandler.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/PublicKey.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/PrivateKey.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/KeyGenerator.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/Certificate.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/AESEncrypt.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/phplibrary/include.file.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/tool.php");
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/nusoap/nusoap.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/Logger.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/openssl/PrivateKey.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/openssl/X509Certificate.class.php");

class LIS_Administrator extends DatabaseHandler
{
	var $db_con;

	/**
	 * Constructor of the Class. Just initializes the database connection.
	 */
	function LIS_Administrator()
	{
		global $LIS_DB_SERVER, $LIS_DB_DBNAME, $LIS_DB_USRNAME, $LIS_DB_PWD;
		$this->db_con = new DatabaseHandler();
		$this->db_con->connectTo($LIS_DB_SERVER, $LIS_DB_DBNAME, $LIS_DB_USRNAME, $LIS_DB_PWD);
	}

	/**
	 * Checks if the pair username, password is correct or not.
	 * @param username The username passed.
	 * @param password The password passed.
	 * @return Returns true if the user is valid or false if the user is not valid.
	 */
	function verifyLogin($username, $password){
		$rs=$this->db_con->executeQuery("SELECT * FROM lisWS_Admin WHERE login='$username' AND password='$password'");
		/* verificar resultado */
		if($rs->RecordCount()==0) return false;
		return true;
	}
	
	function verifyLogin_($username, $password){
		$rs=$this->db_con->executeQuery("SELECT * FROM lisWS_Admin WHERE login='$username' AND password='".md5($password)."'");
		/* verificar resultado */
		if($rs->RecordCount()==0) return false;
		return true;
	}

	/**
	 * Checks if the Admin account is setup or not.
	 * @return Returns true if the admin is setup or false if not.
	 */
	function verifyAdminStatus() {
		$rs=$this->db_con->executeQuery("SELECT COUNT(*) AS admin FROM lisWS_Admin");
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
		//global $RGS_PROTOCOL, $RGS_SERVER, $RGSWS_LOCATION;
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

		//echo "".$RGS_PROTOCOL."://".$RGS_SERVER."".$RGSWS_LOCATION;


		//determine what is my IP address
		//$ip = gethostbyname($_SERVER['SERVER_NAME']);


		$parameters = array (
			"arbitrary_data" => "<name>License Server</name>",
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
		$sql = "INSERT INTO lisws_admin VALUES ('$username', '$password', '".$this->pubkey->returnXMLkey()."', '".$this->privkey->returnXMLkey()."', '".$result["certificate"]."')";
		$rs=$this->db_con->executeQuery($sql);
		if($rs==false) {
			$this->error = "ERROR: An error has occured while trying to insert data on the database.<br>";
			return false;
		}
		return true;

	}

	function createNewAdminAccount_($username, $password) {
		$l = new Logger();
		//global $RGS_PROTOCOL, $RGS_SERVER, $RGSWS_LOCATION;
		$AUS_WS = getLocation('AUS');

		$private_key = new opsslPrivateKey();
		$private_key->create();
		
		$l->Log($private_key->_spKey);
		
		$soapclient = new nusoap_client($AUS_WS); // read from the configuration file

		//echo "".$RGS_PROTOCOL."://".$RGS_SERVER."".$RGSWS_LOCATION;
		
		// create a CSR
		$mycsr = new X509Certificate();
		$mycsr->createCSR(array("countryName" => 'PT', "stateOrProvinceName" => 'Lisboa', "localityName" => 'Lisboa', "organizationName" => 'ISCTE', "organizationalUnitName" => 'ADETTI', "commonName" => 'LIS', "emailAddress" => 'lis@adetti.pt'),$private_key);


		//determine what is my IP address
		//$ip = gethostbyname($_SERVER['SERVER_NAME']);


		$parameters = array (
			"arbitrary_data" => "<name>License Server</name>",
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
		$sql = "INSERT INTO lisws_admin VALUES ('$username', '".md5($password)."', '".$private_key->_spKey."', '".$result["certificate"]."')";
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
		$rs=$this->db_con->executeQuery("SELECT * FROM lisWS_Admin");
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

	function exportData() {
		CreateFile($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/LIS/admin/exports/publickey.xml", $this->pubkey->returnXMLKey());
		$aes = new AESEncrypt();
		CreateFile($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/LIS/admin/exports/privatekey.xml.enc", $aes->AES_encrypt($this->privkey->returnXMLKey(), $this->password));
		CreateFile($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/LIS/admin/exports/certificate.xml", $this->cert->returnXMLCertificate());
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

    /**
     * Function that lists all the licenses created at the license server
     */
    public function listAllLicenses(){
        $sql = 'SELECT * FROM license';
        $rs=$this->db_con->executeQuery($sql);
        if($rs->RecordCount()==0) { // no results were found
            $result = -1;
        } else { // we build an XML structure to provide the needed license template list
            $result = array();
            $n=0;
            while(!$rs->EOF) {
                $result[$n] = array($rs->fields["id"], $rs->fields["template_id"], $rs->fields["uid"], $rs->fields["cid"], $rs->fields["license"], $rs->fields["cid_hash"]);
                $n++;
                $rs->MoveNext();
            }
        }

        return $result;
    }

    /**
     * Function that lists all the license templates
     */
    public function listAllLicenseTemplates(){
        $sql = 'SELECT * FROM license_template';
        $rs=$this->db_con->executeQuery($sql);
        if($rs->RecordCount()==0) { // no results were found
            $result = -1;
        } else { // we build an XML structure to provide the needed license template list
            $result = array();
            $n=0;
            while(!$rs->EOF) {
                $result[$n] = array($rs->fields["id"], $rs->fields["type"], $rs->fields["description"], $rs->fields["content_type"], $rs->fields["num_params"], $rs->fields["license_template"]);
                $n++;
                $rs->MoveNext();
            }
        }

        return $result;
    }


	/**
	 * Closes the database access.
	 */
	function closeLIS_Administrator()
	{
		// close the database access
		$this->db_con->close();
	}
}
?>
