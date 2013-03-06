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
//class AUS_User
include_once(getenv('OPENSDRM_INSTALL_DIR').'config.inc.php');
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS_CONFIG['INSTALLATION_DIR']."lib/common/DatabaseHandler.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS_CONFIG['INSTALLATION_DIR']."lib/common/PublicKey.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS_CONFIG['INSTALLATION_DIR']."opensdrmWS/AUS/Admin/AUS_Administrator.class.php");

class AUS_User
{
	var $db_con;
	var $uid;
	var $pubkey;
	var $cert;
	var $login;
	var $password;
	var $address;
	var $email_address;
	var $auth_type;
	var $other_data;
	var $name;

	function AUS_User()
	{
		global $AUS_DB_SERVER, $AUS_DB_DBNAME, $AUS_DB_USRNAME, $AUS_DB_PWD;
		$this->db_con = new DatabaseHandler();
		$this->db_con->connectTo($AUS_DB_SERVER, $AUS_DB_DBNAME, $AUS_DB_USRNAME, $AUS_DB_PWD);
	}

	function set($uname, $passwd, $address="", $email_address, $auth_type="LOGIN+PASSWORD", $other_data, $pk, $name)
	{
		$this->pubkey = new PublicKey();
		$this->pubkey->loadXMLkey($pk);
		$this->uid = $this->pubkey->getHashOfKey();
		$this->login = $uname;
		$this->address = $address;
		$this->email_address = $email_address;
		$this->auth_type = $auth_type;
		$this->other_data = $other_data;
		$this->name = $name;
		$this->password=$passwd;
	}

	function createCertificate() {
		$adm = new AUS_Administrator();
		$adm->getData(); //TODO: verify return value

		$cert_op = new Certificate();
		$cert_op->createCertificateFrom($adm->pubkey, $adm->privkey, $this->uid, $this->pubkey);
		$this->cert = $cert_op->returnXMLcertificate();
	}

	function loadAllInformation($uid, $username, $password)
	{
		// load information from the database
		$sql_command = "SELECT * FROM ausWS_User WHERE identification='$uid' AND login='$username' AND password='$password'";
		/*
			identification	VARCHAR(32) NOT NULL,
			login		VARCHAR(20) NOT NULL,
			password	VARCHAR(20) NOT NULL,
			public_key_xml	TEXT NOT NULL,
			name		TEXT,
			address		TEXT,
			email		TEXT,
			authentication	TEXT,
			other_data_xml	TEXT,
			certificate_xml	TEXT,
		*/
		$rs = $this->db_con->executeQuery($sql_command);

		$this->uid = $rs->fields["identification"];
		$this->login = $rs->fields["login"];
		$this->password = $rs->fields["password"];
		$this->pubkey = $rs->fields["public_key_xml"];
		$this->cert = $rs->fields["certificate_xml"];
		$this->address = $rs->fields["address"];
		$this->email_address=$rs->fields["email"];
		$this->auth_type = $rs->fields["authentication"];
		$this->other_data = $rs->fields["other_data_xml"];
		$this->name = $rs->fields["name"];

	}

	function saveAllInformation()
	{
		// save information to the database
		$sql_command = "INSERT INTO ausWS_User VALUES ('".$this->uid."', '".$this->login."', '".$this->password."', '".$this->pubkey->returnXMLkey()."', '".$this->name."', '".$this->address."', '".$this->email_address."', '".$this->auth_type."', '".$this->other_data."', '".$this->cert."')";
		$this->db_con->executeQuery($sql_command);
	}

	function deleteUser($uid, $username, $passwd) {
		$sql_comand = "DELETE FROM ausWS_User WHERE identification='$uid' AND login='$username' AND password='$passwd'";
		$this->db_con->executeQuery($sql_command);
	}

	function modifyUser($uid, $username, $password, $name, $address, $email_address, $authentication_type, $arbitrary_data) {
		$sql_command = "UPDATE ausWS_User SET password='$password', name='$name', address='$address', email='$email', authentication='$authentication', other_data_xml='$arbitrary_data' WHERE identification='$uid' AND login='$login' AND password='$password'";
		$res=$this->db_con->executeQuery($sql_command);
	}

	function verifyUser($uid, $login, $password) {
		$sql_command = "SELECT COUNT(*) AS ct FROM ausWS_User WHERE identification='$uid' AND login='$login' AND password='$password'";
		$rs = $this->db_con->executeQuery($sql_command);
		if ($rs->fields["ct"]==0) return false;
		else return true;
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

	function closeAUS_User()
	{
		// close the database access
	}
}