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

//class AUS_Component
//include_once(getenv('OPENSDRM_INSTALL_DIR')."config.inc.php");
require_once ("Zend/Db.php");
require_once ("Zend/Config/Ini.php");
require_once 'Zend/Log.php';
require_once 'Zend/Log/Writer/Stream.php';
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS["LIB_DIR"] . "/common/DatabaseHandler.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS["LIB_DIR"] . "/common/PublicKey.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS["LIB_DIR"] . "/common/Logger.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS["INSTALLATION_DIR"] . "/AUS/admin/business/AUS_Administrator.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS["LIB_DIR"] . "/common/openssl/PrivateKey.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS["LIB_DIR"] . "/common/openssl/X509Certificate.class.php");

class AUS_Component {

    private $dbcfg;
    private $l;
    public $uid;
    var $password;
    var $pubkey;
    public $cert;
    public $other_data;
    public $csr;

    /**
     * @return AUS_Component
     * @desc Enter description here...
     */
    function __construct() {
        $this->dbcfg = new Zend_Config_Ini(getenv("DOCUMENT_ROOT") . '/config.ini', 'sig.modeve');

        $writer = new Zend_Log_Writer_Stream('./aus.log');
        $this->l = new Zend_Log($writer);

        $this->l->info('In constructor of the AUS_Component class');
    }

    function set($other_data, $csr, $password) {
        $this->csr = $csr;
        $this->other_data = $other_data;
        $this->password = $password;
    }

    function createCertificate() {
        $this->l->info("AUS:AUS_Component:createCertificate");
        $adm = new AUS_Administrator();
        if ($adm->getData_() == false)
            return false;

        $newcert = new X509Certificate();
        $newcert->createCert($this->csr, $adm->cert, null, $adm->keypair, 365, null);
        $this->cert = $newcert->getCert(1);

        $this->uid = md5($newcert->getCert(1));
        return true;
    }

    function loadAllInformation($user_id) {
        //PROBLEM: I don't KNOW WHY (YET), but the Logger doesn't WORK
        //$l = new Logger("AUS");
        //$l->Log("AUS:AUS_Component:loadAllInformation");
        //$l->Log("AUS:AUS_Component:loadAllInformation:user_id:".$user_id);
        // load information from the database
        $sql = "SELECT * FROM " . $GLOBALS['AUS_DB_DBNAME'] . ".ausws_component WHERE identification='" . $user_id . "'";
        $rs = $this->db_con->executeQuery($sql);

        //$l->Log("AUS:AUS_Component:loadAllInformation:sql:".$sql);

        $this->pubkey = new PublicKey();
        $this->pubkey->loadXMLKey($rs->fields["public_key_xml"]);

        //$l->Log("AUS:AUS_Component:loadAllInformation:rs->fields[\"public_key_xml\"]:".$rs->fields["public_key_xml"]);

        $this->uid = $uid;
        $this->other_data = $rs->fields["other_data_xml"];
        $this->password = $rs->fields["password"];

        $this->cert = new Certificate();
        $this->cert->loadXMLCertificate($rs->fields["certificate"]);
    }

    function loadAllInformation_($comp_id) {
        //PROBLEM: I don't KNOW WHY (YET), but the Logger doesn't WORK
        $l = new Logger("AUS");
        $l->Log("AUS:AUS_Component:loadAllInformation");
        $l->Log("AUS:AUS_Component:loadAllInformation:comp_id:" . $comp_id);

        // load information from the database
        $sql = "SELECT * FROM " . $GLOBALS['AUS_DB_DBNAME'] . ".ausws_component WHERE identification='" . md5($comp_id) . "'";
        $rs = $this->db_con->executeQuery($sql);

        $l->Log("AUS:AUS_Component:loadAllInformation:sql:" . $sql);

        /* $this->pubkey = new PublicKey();
          $this->pubkey->loadXMLKey($rs->fields["public_key_xml"]); */

        $this->pubkey = $rs->fields["public_key_xml"];
        $this->cert = $rs->fields["certificate_xml"];

        $l->Log($this->pubkey);
        $l->Log($this->cert);

        //$l->Log("AUS:AUS_Component:loadAllInformation:rs->fields[\"public_key_xml\"]:".$rs->fields["public_key_xml"]);

        $this->uid = $uid;
        $this->other_data = $rs->fields["other_data_xml"];
        $this->password = $rs->fields["password"];

        /* $this->cert = new Certificate();
          $this->cert->loadXMLCertificate($rs->fields["certificate"]); */
    }

    function saveAllInformation() {
        $db = Zend_Db::factory($this->dbcfg->database->type, array('host' => $this->dbcfg->database->host, 'username' => $this->dbcfg->database->username, 'password' => $this->dbcfg->database->password, 'dbname' => $this->dbcfg->database->name));
        
        // check if the specif component is already existing
        $sql = "SELECT COUNT(*) AS ct FROM " . $this->dbcfg->database->name . ".ausws_component WHERE identification='$this->uid'";
        
        try
        {
              $result = $db->fetchAll($sql, 0);
              
              //TODO!!!!!
        } catch (ZendException $e)
        {
            $db->closeConnection();
            throw new AUS_ComponentException('Problems checking API key ->' . $e->getMessage());
        }
        
        $rs = $this->db_con->executeQuery($sql_cmd);
        if ($rs->fields["ct"] != 0) {
            return -1; //already exists a component with the same id
        }


        // save information to the database
        $sql_command = "INSERT INTO " . $GLOBALS['AUS_DB_DBNAME'] . ".ausws_component VALUES ('" . $this->uid . "', '" . $this->cert . "', '" . md5($this->password) . "', '" . $this->cert . "', '" . $this->other_data . "')";
        $this->db_con->executeQuery($sql_command);
    }

    function getPublicKey() {
        return $this->pubkey->getAll();
    }

    function getCert() {
        return;
    }

    function getLogin() {
        return $this->login;
    }

    function getPassword() {
        return $this->password;
    }

    function closeAUS_Component() {
        // close the database access
    }

}

class AUS_ComponentException extends Exception {

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