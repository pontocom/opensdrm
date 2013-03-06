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

//class AUS_Administrator
//include_once(getenv('OPENSDRM_INSTALL_DIR')."config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["INSTALLATION_DIR"]."/AUS/configAUS.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/DatabaseHandler.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/PublicKey.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/PrivateKey.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/KeyGenerator.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/Certificate.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/AESEncrypt.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/RSAEncrypt.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/phplibrary/include.file.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/Logger.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/openssl/PrivateKey.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/openssl/X509Certificate.class.php");

class AUS_Administrator
{
	var $db_con;
	var $login;
	var $password;
	var $pubkey;
	var $privkey;
	var $cert;

	var $error;
	
	public $keypair;

	/**
	 * Constructor of the Class. Just initializes the database connection.
	 */
	function AUS_Administrator()
	{
		//global $AUS_DB_SERVER, $AUS_DB_DBNAME, $AUS_DB_USRNAME, $AUS_DB_PWD;
		$this->db_con = new DatabaseHandler();
		$this->db_con->connectTo($GLOBALS['AUS_DB_SERVER'], $GLOBALS['AUS_DB_DBNAME'], $GLOBALS['AUS_DB_USRNAME'], $GLOBALS['AUS_DB_PWD']);
	}

	/**
	 * Checks if the pair username, password is correct or not.
	 * @param username The username passed.
	 * @param password The password passed.
	 * @return Returns true if the user is valid or false if the user is not valid.
	 */
	function verifyLogin($username, $password){
		$rs=$this->db_con->executeQuery("SELECT * FROM ".$GLOBALS['AUS_DB_DBNAME'].".ausws_admin WHERE login='$username' AND password='$password'");

		/* verificar resultado */
		if($rs->RecordCount()==0) return false;
		return true;
	}

	function verifyLogin_($username, $password){
		$rs=$this->db_con->executeQuery("SELECT * FROM ".$GLOBALS['AUS_DB_DBNAME'].".ausws_admin WHERE login='$username' AND password='".md5($password)."'");

		/* verificar resultado */
		if($rs->RecordCount()==0) return false;
		return true;
	}
	
	function specialVerifyUser($uid, $login, $hash) {
		$l = new Logger();
		$l->Log("AUS:AUS_Administrator:specialVerifyUser");
		$rs=$this->db_con->executeQuery("SELECT password FROM ".$GLOBALS['AUS_DB_DBNAME'].".ausws_user WHERE identification='$uid' AND login='$login'");
		$l->Log("SELECT password FROM ".$GLOBALS['AUS_DB_DBNAME'].".ausws_user WHERE identification='$uid' AND login='$login'");
		if($rs->RecordCount()==0) return false;
		else {
			$data = "".$login."".$rs->fields["password"];
			$chash=md5($data);
			if($hash==$chash) {
				$l->Log("returning true");
				return true;
			} else {
				$l->Log("returning false");
				return false;
			}
		}
	}

	function checkWallet($uid) {
		$l = new Logger();
		$l->Log("AUS:AUS_Administrator:checkforWallet");
		$rs=$this->db_con->executeQuery("SELECT other_data_xml FROM ".$GLOBALS['AUS_DB_DBNAME'].".ausws_user WHERE identification='$uid'");
		$l->Log("SELECT other_data_xml FROM ausws_user WHERE identification='".$uid."'");
		if($rs->RecordCount()==0) {
			$l->Log("returning false because user was not found!!!");
			return false;
		}
		else {
			if(strstr($rs->fields["other_data_xml"], "WALLET")==false) {
				$l->Log("return false: user was found but no wallet");
				return false;
			} else {
				$l->Log("returning true: user has wallet");
				return true;
			}
		}
	}

	/**
	 * Checks if the Admin account is setup or not.
	 * @return Returns true if the admin is setup or false if not.
	 */
	function verifyAdminStatus() {
		$rs=$this->db_con->executeQuery("select count(*) as admin from ".$GLOBALS['AUS_DB_DBNAME'].".ausws_admin");

		//echo $rs->Fields("admin");
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
		/*Crete a new keypair */
		$keygen = new KeyGenerator();
		$keygen->RSAKeyGenerate($ksize);

		/* instanciates the public and private keys of this class */
		$this->pubkey = new PublicKey();
		$this->pubkey->set("RSA", $keygen->pubkey->getN(), $keygen->pubkey->getE());

		$this->privkey = new PrivateKey();
		$this->privkey->set("RSA", $keygen->privkey->getN(), $keygen->privkey->getE(), $keygen->privkey->getD(), $keygen->privkey->getP(), $keygen->privkey->getQ());

		/* creates a new self signed certificate */
		$this->cert = new Certificate();
		$this->cert->createCertificateFrom($this->pubkey, $this->privkey, $this->pubkey->getHashOfKey(), $this->pubkey);

		/* inserts all data on the table */
		$sql = "INSERT INTO ".$GLOBALS['AUS_DB_DBNAME'].".ausws_admin VALUES ('$username', '$password', '".$this->pubkey->returnXMLkey()."', '".$this->privkey->returnXMLkey()."', '".$this->cert->returnXMLcertificate()."')";
		$rs=$this->db_con->executeQuery($sql);
		if($rs==false) {
			$this->error = "ERROR: An error has occured while trying to insert data on the database.<br>";
			return false;
		}
		return true;
	}
	
	function createNewAdminAccount_($username, $password) {
		/*Crete a new keypair */
		$keys = new opsslPrivateKey();
		$keys->create();
		
		/* create a csr */
		$mycsr = new X509Certificate();
		$mycsr->createCSR(array("countryName" => 'PT', "stateOrProvinceName" => 'Lisboa', "localityName" => 'Lisboa', "organizationName" => 'ISCTE', "organizationalUnitName" => 'ADETTI', "commonName" => 'AUS', "emailAddress" => 'aus@adetti.pt'), $keys);
		
		/* creates a new self signed certificate */
		$mycert = new X509Certificate();
		$mycert->createCert($mycsr->_csr,null, $keys, null, 5*365, null);
		
		/* inserts all data on the table */
		$sql = "INSERT INTO ".$GLOBALS['AUS_DB_DBNAME'].".ausws_admin VALUES ('$username', '".md5($password)."', '".$mycert->getCert(1)."', '".$keys->_spKey."', '".$mycert->getCert(1)."')";
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
		//$rs2=$this->db_con->executeQuery("SELECT * FROM ausWS.ausWS_Admin");
		$rs=$this->db_con->executeQuery("SELECT * FROM ".$GLOBALS['AUS_DB_DBNAME'].".ausws_admin");

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
		$l->Log("AUS:AUS_Administrator:getData_");
		//$rs2=$this->db_con->executeQuery("SELECT * FROM ausWS.ausWS_Admin");
		$rs=$this->db_con->executeQuery("SELECT * FROM ".$GLOBALS['AUS_DB_DBNAME'].".ausws_admin");

		/* verificar resultado */
		if($rs->RecordCount()==0) return false;
		else {
			$this->login=$rs->fields[0];
			$this->password=$rs->fields[1];
			$kp = $rs->fields[3];
			$l->Log($kp);
			$this->keypair = new opsslPrivateKey();
			$this->keypair->set($kp);
			$cert = $rs->fields[4];
			$l->Log($cert);
			$this->cert = new X509Certificate();
			$this->cert->set($cert);
		}
		return true;
	}
	
	function exportData() {
		CreateFile($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/AUS/admin/exports/publickey.xml", $this->pubkey->returnXMLKey());
		$aes = new AESEncrypt();
		CreateFile($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/AUS/admin/exports/privatekey.xml.enc", $aes->AES_encrypt($this->privkey->returnXMLKey(), $this->password));
		CreateFile($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/AUS/admin/exports/certificate.xml", $this->cert->returnXMLCertificate());
	}

/*	function listUsers() {
		$rs=$this->db_con->executeQuery("SELECT * FROM ausWS_User");

		echo "<form>
  <table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"0\">
    <tr>
      <td height=\"18\" bgcolor=\"#000000\" width=100><strong><font color=\"#FFFFFF\" size=\"2\" face=\"Verdana, Arial, Helvetica, sans-serif\">identification</font></strong></td>
      <td bgcolor=\"#000000\"><strong><font color=\"#FFFFFF\" size=\"2\" face=\"Verdana, Arial, Helvetica, sans-serif\">username</font></strong></td>
      <td bgcolor=\"#000000\"><strong><font color=\"#FFFFFF\" size=\"2\" face=\"Verdana, Arial, Helvetica, sans-serif\">public
        key</font></strong></td>
      <td bgcolor=\"#000000\"><strong><font color=\"#FFFFFF\" size=\"2\" face=\"Verdana, Arial, Helvetica, sans-serif\">name</font></strong></td>
      <td bgcolor=\"#000000\"><strong><font color=\"#FFFFFF\" size=\"2\" face=\"Verdana, Arial, Helvetica, sans-serif\">address</font></strong></td>
      <td bgcolor=\"#000000\"><strong><font color=\"#FFFFFF\" size=\"2\" face=\"Verdana, Arial, Helvetica, sans-serif\">email</font></strong></td>
      <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">&nbsp;</font></td>
    </tr>";
		while(!$rs->EOF) {
			echo "<tr>
      <td width=100><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">".$rs->fields[0]."</font></td>
      <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">".$rs->fields[1]."</font></td>
      <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">
        <textarea name=\"textarea\" cols=\"40\" rows=\"3\">".$rs->fields[3]."</textarea>
        </font></td>
      <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">".$rs->fields[4]."</font></td>
      <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">".$rs->fields[5]."</font></td>
      <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">".$rs->fields[6]."</font></td>
      <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\"><strong><a href=\"main.php?op=APG_U&id=".$rs->fields[0]."\">DELETE</a></strong></font></td>
    </tr>";
			$rs->MoveNext();
		}
  echo "</table>
</form>
";
	}
*/
	function listUsers() {
		$rs=$this->db_con->executeQuery("SELECT * FROM ".$GLOBALS['AUS_DB_DBNAME'].".ausws_user");
		$dataA = Array();
		$l=0;
		while(!$rs->EOF) {
			$dataA[$l] = Array($rs->fields[0], $rs->fields[1], $rs->fields[4], $rs->fields[5], $rs->fields[6]);
			$rs->MoveNext();
			$l++;
		}
		return $dataA;
	}

	function listComponents() {
		$rs=$this->db_con->executeQuery("SELECT * FROM ".$GLOBALS['AUS_DB_DBNAME'].".ausws_component");
		$dataA = Array();
		$l=0;
		while(!$rs->EOF) {
			$dataA[$l] = Array($rs->fields[0], $rs->fields[4]);
			$rs->MoveNext();
			$l++;
		}
		return $dataA;
	}

	function getDetailsData($who, $id) {
		if($who=="U") $rs=$this->db_con->executeQuery("SELECT * FROM ".$GLOBALS['AUS_DB_DBNAME'].".ausws_user WHERE identification='$id'");
		if($who=="C") $rs=$this->db_con->executeQuery("SELECT * FROM ".$GLOBALS['AUS_DB_DBNAME'].".ausws_component WHERE identification='$id'");

		$nfields = $rs->FieldCount();

		$data = Array();

		for($n=0; $n<$nfields; $n++) array_push($data, $rs->fields[$n]);

		return $data;
	}

/*	function listComponents() {
		$rs=$this->db_con->executeQuery("SELECT * FROM ausWS_Component");

		echo "<form>
  <table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"0\">
    <tr>
      <td height=\"18\" bgcolor=\"#000000\"><strong><font color=\"#FFFFFF\" size=\"2\" face=\"Verdana, Arial, Helvetica, sans-serif\">identification</font></strong></td>
      <td bgcolor=\"#000000\"><strong><font color=\"#FFFFFF\" size=\"2\" face=\"Verdana, Arial, Helvetica, sans-serif\">public
        key</font></strong></td>
      <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">&nbsp;</font></td>
    </tr>";
		while(!$rs->EOF) {
			echo "<tr>
      <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">".$rs->fields[0]."</font></td>
      <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">
        <textarea name=\"textarea\" cols=\"40\" rows=\"3\">".$rs->fields[1]."</textarea>
        </font></td>
      <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\"><strong><a href=\"main.php?op=APG_C&id=".$rs->fields[0]."\">DELETE</a></strong></font></td>
    </tr>";
			$rs->MoveNext();
		}
		echo "  </table>
</form>
";

	}
*/

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
    <td bgcolor=\"#000000\" width=100><font color=\"#FFFFFF\" size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">Self
      Certificate</font></td>
    <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\"><textarea rows=20 cols=120 disabled>".htmlentities($this->cert->returnXMLCertificate())."</textarea></font></td>
  </tr>
</table>";
	}

	function deleteUsers($users) {
		if(count($users)==0) return;
		$sql_cmd = "delete from ".$GLOBALS['AUS_DB_DBNAME'].".ausws_user where ";
		for($n=0; $n<count($users); $n++) {
			$sql_cmd=$sql_cmd."identification = '".$users[$n]."' ";
			if($n+1	!=count($users)) {
				$sql_cmd=$sql_cmd."and ";
			}
		}
		return $this->db_con->executeQuery($sql_cmd);
	}

	function deleteComponents($components) {
		if(count($components)==0) return;
		$sql_cmd = "delete from ".$GLOBALS['AUS_DB_DBNAME'].".ausws_component where ";
		for($n=0; $n<count($components); $n++) {
			$sql_cmd=$sql_cmd."identification = '".$components[$n]."' ";
			if($n+1	!=count($components)) {
				$sql_cmd=$sql_cmd."and ";
			}
		}
		return $this->db_con->executeQuery($sql_cmd);
	}
	
	function deleteComponent_($component) 
	{
		$l= new Logger();
		$sql_cmd = "delete from ".$GLOBALS['AUS_DB_DBNAME'].".ausws_component where identification='".$component."'";
		$l->Log($sql_cmd);
		return $this->db_con->executeQuery($sql_cmd);
	}

	function listAvailablePGW() {
		$sql_cmd = "SELECT * FROM ".$GLOBALS['AUS_DB_DBNAME'].".ausws_component where other_data_xml LIKE '%Payment Gateway%'";
		$rs = $this->db_con->executeQuery($sql_cmd);
		if($rs->RecordCount()==0) return false;
		else {
			$rxml="<list_pgw><count>".$rs->RecordCount()."</count>";
        	while(!$rs->EOF) {
        		$rxml = $rxml."<pgw><id>".$rs->fields[0]."</id><ip_data>".$rs->fields[4]."</ip_data>".$rs->fields[3]."</pgw>";
        		$rs->MoveNext();
        	}
        	$rxml =$rxml."</list_pgw>";
			return $rxml;
		}
	}

	function prepareDataForPGW($pgw_identification, $user_identification, $value) {
		//$l = new Logger("AUS");
		//$l->Log("AUS:AUS_Administrator:prepareDataForPGW");

		$sql_cmd = "SELECT * FROM ".$GLOBALS['AUS_DB_DBNAME'].".ausws_user where identification ='".$user_identification."'";

		//$l->Log("AUS:AUS_Administrator:prepareDataForPGW:sql_cmd:".$sql_cmd);

		$rs = $this->db_con->executeQuery($sql_cmd);
		if($rs->RecordCount()==0) return false;
		else {
			$data_to_encript = $rs->fields[8].$value;
			//$l->Log("AUS:AUS_Administrator:prepareDataForPGW:data_to_encript:".$data_to_encript);

			// get the public_key of the PGW.
			$sql_cmd2 = "SELECT * FROM ".$GLOBALS['AUS_DB_DBNAME'].".ausws_component where identification ='".$pgw_identification."'";
			//$l->Log("AUS:AUS_Administrator:prepareDataForPGW:sql_cmd2:".$sql_cmd2);

    		$rs2 = $this->db_con->executeQuery($sql_cmd2);
    		if($rs2->RecordCount()==0) return false;
    		else {
    			$pubkey = $rs2->fields[1];
				//$l->Log("AUS:AUS_Administrator:prepareDataForPGW:pubkey:".$pubkey);

				if($SECURITY_VAR=="ON") {
					$pubk = new PublicKey();
					$pubk->loadXMLkey($pubkey);

					$rencrypt = new RSAEncrypt();
					$data_enc = $rencrypt->RSA_encrypt($data_to_encript, $pubk);
				} else {
					$data_enc = $data_to_encript;
				}

				//$l->Log("AUS:AUS_Administrator:prepareDataForPGW:data_enc:".$data_enc);

				return $data_enc;
    		}
		}
	}

	/**
	 * Closes the database access.
	 */
	function closeAUS_Component()
	{
		// close the database access
		$this->db_con->close();
	}
}