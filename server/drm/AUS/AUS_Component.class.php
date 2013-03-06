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
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["INSTALLATION_DIR"]."/AUS/configAUS.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/DatabaseHandler.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/PublicKey.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/Logger.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["INSTALLATION_DIR"]."/AUS/admin/business/AUS_Administrator.class.php");

class AUS_Component
{
	var $db_con;
	var $uid;
	var $password;
	var $pubkey;
	var $cert;
	var $other_data;

	/**
	 * @return AUS_Component
	 * @desc Enter description here...
	 */
	function AUS_Component()
	{
		//global $AUS_DB_SERVER, $AUS_DB_DBNAME, $AUS_DB_USRNAME, $AUS_DB_PWD;
		$this->db_con = new DatabaseHandler();
		$this->db_con->connectTo($GLOBALS['AUS_DB_SERVER'], $GLOBALS['AUS_DB_DBNAME'], $GLOBALS['AUS_DB_USRNAME'], $GLOBALS['AUS_DB_PWD']);
	}

	function set($other_data, $pk_xml, $password)
	{
		$this->pubkey = new PublicKey();
		$this->pubkey->loadXMLKey($pk_xml);
		$this->uid = $this->pubkey->getHashOfKey();
		$this->other_data = $other_data;
		$this->password = $password;
	}

	function createCertificate() {
		$adm = new AUS_Administrator();
		if($adm->getData()==false) return false;

		$cert_op = new Certificate();
		$cert_op->createCertificateFrom($adm->pubkey, $adm->privkey, $this->uid, $this->pubkey);
		$this->cert = $cert_op->returnXMLcertificate();
		return true;
	}

	function loadAllInformation($user_id)
	{
		//PROBLEM: I don't KNOW WHY (YET), but the Logger doesn't WORK
		//$l = new Logger("AUS");
		//$l->Log("AUS:AUS_Component:loadAllInformation");
		//$l->Log("AUS:AUS_Component:loadAllInformation:user_id:".$user_id);

		// load information from the database
		$sql = "SELECT * FROM ".$GLOBALS['AUS_DB_DBNAME'].".ausws_component WHERE identification='".$user_id."'";
		$rs=$this->db_con->executeQuery($sql);

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

	function saveAllInformation()
	{
		// check if the specif component is already existing
		$sql_cmd = "SELECT COUNT(*) AS ct FROM ".$GLOBALS['AUS_DB_DBNAME'].".ausws_component WHERE identification='$this->uid'";
		$rs=$this->db_con->executeQuery($sql_cmd);
		if($rs->fields["ct"]!=0) {
			return -1; //already exists a component with the same id
		}


		// save information to the database
		$sql_command = "INSERT INTO ".$GLOBALS['AUS_DB_DBNAME'].".ausws_component VALUES ('".$this->uid."', '".$this->pubkey->returnXMLkey()."', '".$this->password."', '".$this->cert."', '".$this->other_data."')";
		$this->db_con->executeQuery($sql_command);
	}

	function getPublicKey()
	{
		return $this->pubkey->getAll();
	}

	function getCert()
	{
		return;
	}

	function getLogin()
	{
		return $this->login;
	}

	function getPassword()
	{
		return $this->password;
	}

	function closeAUS_Component()
	{
		// close the database access
	}
}