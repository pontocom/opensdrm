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

require_once("Zend/Db.php");
require_once("Zend/Config/Ini.php");
require_once 'Zend/Log.php';
require_once 'Zend/Log/Writer/Stream.php';
include_once("../lib/common/PublicKey.class.php");
include_once("../lib/common/PrivateKey.class.php");
include_once("../lib/common/KeyGenerator.class.php");
include_once("../lib/common/Certificate.class.php");
include_once("../lib/common/AESEncrypt.class.php");
include_once("../lib/phplibrary/include.file.php");
include_once("../lib/common/tool.php");
//require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/nusoap/nusoap.php");
//include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/Logger.class.php");
include_once("../lib/common/openssl/PrivateKey.class.php");
include_once("../lib/common/openssl/X509Certificate.class.php");

class CFS_Administrator {

    private $dbcfg;
    private $l;
    
    protected $login;
    protected $password;
    protected $pubkey;
    protected $privkey;
    protected $cert;

    /**
     * Constructor of the Class. Just initializes the database connection.
     */
    function __construct() {
        $this->dbcfg = new Zend_Config_Ini(getenv("DOCUMENT_ROOT") . '/config.ini', 'opensdrm.cfs');
        $writer = new Zend_Log_Writer_Stream('./cfs.log');
        $this->l = new Zend_Log($writer);

        $this->l->info('In constructor of the CFS_Administrator class');
    }

    /**
     * Checks if the pair username, password is correct or not.
     * @param username The username passed.
     * @param password The password passed.
     * @return Returns true if the user is valid or false if the user is not valid.
     */
    public function verifyLogin($username, $password) {
        $db = Zend_Db::factory($this->dbcfg->database->type, array('host' => $this->dbcfg->database->host, 'username' => $this->dbcfg->database->username, 'password' => $this->dbcfg->database->password, 'dbname' => $this->dbcfg->database->name));
        $sql = "SELECT * FROM cfsws_admin WHERE login = '".$username."' AND password = '".$password."'";

        try {
            $db->setFetchMode(Zend_Db::FETCH_OBJ);
            $result = $db->fetchAll($sql, 0);
            if ($result[0]->username != 0) {
                return true;
            } else {
                return false;
            }
        } catch (Zend_Exception $e) {
            $db->closeConnection();
            throw new CFS_AdministratorException('Problems verifying the login ->' . $e->getMessage());
        }
    }

    public function verifyLogin_($username, $password) {
        $db = Zend_Db::factory($this->dbcfg->database->type, array('host' => $this->dbcfg->database->host, 'username' => $this->dbcfg->database->username, 'password' => $this->dbcfg->database->password, 'dbname' => $this->dbcfg->database->name));
        $sql = "SELECT * FROM cfsws_admin WHERE login = '".$username."' AND password = '".md5($password)."'";

        try {
            $db->setFetchMode(Zend_Db::FETCH_OBJ);
            $result = $db->fetchAll($sql, 0);
            if ($result[0]->username != 0) {
                return true;
            } else {
                return false;
            }
        } catch (Zend_Exception $e) {
            $db->closeConnection();
            throw new CFS_AdministratorException('Problems verifying the login ->' . $e->getMessage());
        }
    }

    /**
     * Checks if the Admin account is setup or not.
     * @return Returns true if the admin is setup or false if not.
     */
    public function verifyAdminStatus() {
        $db = Zend_Db::factory($this->dbcfg->database->type, array('host' => $this->dbcfg->database->host, 'username' => $this->dbcfg->database->username, 'password' => $this->dbcfg->database->password, 'dbname' => $this->dbcfg->database->name));
        $sql = "SELECT COUNT(*) AS admin FROM cfsws_admin";

        $rs = $this->db_con->executeQuery("SELECT COUNT(*) AS admin FROM " . $GLOBALS['CFS_DB_DBNAME'] . ".cfsws_admin");

        try {
            $db->setFetchMode(Zend_Db::FETCH_OBJ);
            $result = $db->fetchAll($sql, 0);
            if ($result[0]->admin == 0) {
                return false;
            } else {
                return true;
            }
        } catch (Zend_Exception $e) {
            $db->closeConnection();
            throw new CFS_AdministratorException('Problems verifying the admin status ->' . $e->getMessage());
        }
    }

    /**
     * Creates a new Administrator account.
     * @param username The username passed.
     * @param password The password passed.
     * @param ksize The size in bits of the key to be generated.
     * @version 20070517
     */
    function createNewAdminAccount_($username, $password) {
        $db = Zend_Db::factory($this->dbcfg->database->type, array('host' => $this->dbcfg->database->host, 'username' => $this->dbcfg->database->username, 'password' => $this->dbcfg->database->password, 'dbname' => $this->dbcfg->database->name));
        $this->l->info("CFS:CFS_Administrator");

        /* Crete a new keypair */
        $private_key = new opsslPrivateKey();
        $private_key->create();

        $ths->l->Log($private_key->_spKey);

        // TODO
        //$soapclient = new nusoap_client($AUS_LOCATION); // read from the configuration file
        
        // create a CSR
        $mycsr = new X509Certificate();
        $mycsr->createCSR(array("countryName" => 'PT', "stateOrProvinceName" => 'Lisboa', "localityName" => 'Lisboa', "organizationName" => 'ISCTE', "organizationalUnitName" => 'ADETTI', "commonName" => 'CFS', "emailAddress" => 'cfs@adetti.pt'), $private_key);

        // TODO
        /*
        $parameters = array(
            "arbitrary_data" => "<name>Configuration Server</name>",
            "public_key" => "" . $mycsr->_scsr,
            "password" => "" . $password
        );
        
        $result = $soapclient->call('AUSrequestComponentSubscription_', $parameters);
        echo "<br>Parameters:";
        print_r($parameters);
        echo "<br>Results:";
        print_r($result);

        if ($result["result_message"] != "+OK") {
            echo "" . strstr($result["result_message"], "-ERR:");
            echo "error in web service.<br>";
            return false;
        }
        */

        /* inserts all data on the table */
        $data = array(
            'login' => $username, 
            'password' => md5($password), 
            'public_key_xml' => $private_key->_spKey, 
            'private_key_xml' => $private_key->_spKey, 
            'certificate' => $mycsr->_scsr
                );


        try {
            $db->insert('cfsws_admin', $data);
            $db->closeConnection();
            return true;
        } catch (Zend_Exception $e) {
            $db->closeConnection();
            $this->error = "ERROR: An error has occured while trying to insert data on the database.<br>";
            throw new UserException('Problems saving the admin data ->' . $e->getMessage());
        }

    }

    public function returnLastError() {
        return $this->error;
    }

    public function getData() {
        $db = Zend_Db::factory($this->dbcfg->database->type, array('host' => $this->dbcfg->database->host, 'username' => $this->dbcfg->database->username, 'password' => $this->dbcfg->database->password, 'dbname' => $this->dbcfg->database->name));
        $sql = "SELECT * FROM cfsws_admin";
        
        try {
            $result = $db->fetchAll($sql, 0);
            
            $this->login = $result[0]['login'];
            $this->password = $result[0]['password'];
            $this->pubkey = new PublicKey();
            $this->pubkey->loadXMLkey($result[0]['public_key_xml']);
            $this->privkey = new PrivateKey();
            $this->privkey->loadXMLkey($result[0]['private_key_xml']);
            $this->cert = new Certificate();
            $this->cert->loadXMLCertificate($result[0]['certificate']);
            
            $db->closeConnection();
            return true;
        } catch (Zend_Exception $e) {
            $db->closeConnection();
            throw new CFS_AdministratorException('Problems getting data ->' . $e->getMessage());
            return false;
        }
        
    }

    public function exportData() {
        CreateFile($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['INSTALLATION_DIR'] . "/CFS/admin/exports/publickey.xml", $this->pubkey->returnXMLKey());
        $aes = new AESEncrypt();
        CreateFile($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['INSTALLATION_DIR'] . "/CFS/admin/exports/privatekey.xml.enc", $aes->AES_encrypt($this->privkey->returnXMLKey(), $this->password));
        CreateFile($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['INSTALLATION_DIR'] . "/CFS/admin/exports/certificate.xml", $this->cert->returnXMLCertificate());
    }

    public function printInternalData() {
        echo "
<table width=\"500\" border=\"1\" cellspacing=\"0\" cellpadding=\"0\">
  <tr bgcolor=\"#000000\" width=500>
    <td colspan=\"2\" with=500><font color=\"#FFFFFF\" size=\"2\" face=\"Verdana, Arial, Helvetica, sans-serif\"><strong>Administrator
      Data</strong></font></td>
  </tr>
  <tr>
    <td width=\"100\" bgcolor=\"#000000\"><font color=\"#FFFFFF\" size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">Username</font></td>
    <td width=500><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">" . $this->login . "</font></td>
  </tr>
  <tr>
    <td bgcolor=\"#000000\" width=100><font color=\"#FFFFFF\" size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">Public
      Key</font></td>
    <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\"><textarea rows=6 cols=120 disabled>" . htmlentities($this->pubkey->returnXMLkey()) . "</textarea></font></td>
  </tr>
  <tr>
    <td bgcolor=\"#000000\" width=100><font color=\"#FFFFFF\" size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">AUS issued
      Certificate</font></td>
    <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\"><textarea rows=20 cols=120 disabled>" . htmlentities($this->cert->returnXMLCertificate()) . "</textarea></font></td>
  </tr>
</table>";
    }

    public function listServers() {
        $rs = $this->db_con->executeQuery("SELECT * FROM " . $GLOBALS['CFS_DB_DBNAME'] . ".location");

        echo "<form>
  <table width=\"100%\" border=\"1\" cellspacing=\"0\" cellpadding=\"0\">
    <tr>
      <td height=\"18\" bgcolor=\"#000000\" width=100><strong><font color=\"#FFFFFF\" size=\"2\" face=\"Verdana, Arial, Helvetica, sans-serif\">Server ID</font></strong></td>
      <td bgcolor=\"#000000\"><strong><font color=\"#FFFFFF\" size=\"2\" face=\"Verdana, Arial, Helvetica, sans-serif\">Location</font></strong></td>
      <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">&nbsp;</font></td>
    </tr>";
        while (!$rs->EOF) {
            echo "<tr>
      <td width=100><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">" . $rs->fields[0] . "</font></td>
      <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\">" . $rs->fields[1] . "</font></td>
      <td><font size=\"1\" face=\"Verdana, Arial, Helvetica, sans-serif\"><strong><a href=\"main.php?op=APG_L&id=" . $rs->fields[0] . "\">DEL</a></strong></font></td>
    </tr>";
            $rs->MoveNext();
        }
        echo "</table>
</form>
";
    }

    public function listServers_() {
        $rs = $this->db_con->executeQuery("SELECT * FROM " . $GLOBALS['CFS_DB_DBNAME'] . ".location");

        $servers = array();
        $n = 0;
        while (!$rs->EOF) {
            $servers[$n] = $rs->fields[0];
            $n++;
            $servers[$n] = $rs->fields[1];
            $n++;

            $rs->MoveNext();
        }
        return $servers;
    }

    public function addServer($sid, $location) {
        $this->db_con->executeQuery("INSERT INTO " . $GLOBALS['CFS_DB_DBNAME'] . ".location VALUES ('$sid', '$location')");
    }

    public function deleteServer($sid) {
        $this->db_con->executeQuery("DELETE FROM " . $GLOBALS['CFS_DB_DBNAME'] . ".location WHERE id='$sid'");
    }

    public function getServerLocation($sid) {
        $rs = $this->db_con->executeQuery("SELECT location FROM " . $GLOBALS['CFS_DB_DBNAME'] . ".location WHERE id='$sid'");
        if ($rs->EOF)
            return "-ERR";
        else
            return $rs->fields[0];
    }

}


class CFS_AdministratorException extends Exception {

    // Redefine the exception so message isn't optional
    public function __construct($message, $code = 0) {
        // some code
        // make sure everything is assigned properly
        parent::__construct($message, $code);
    }

    // custom string representation of object
    public function __toString() {
        return __CLASS__ . ": [{$this->code}]: {$this->message}";
    }

}
?>
