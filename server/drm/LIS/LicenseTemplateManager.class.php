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
//class LicenseTemplateManager

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


class LicenseTemplateManager extends DatabaseHandler
{
	var $db_con;

	/**
	 * Constructor of the Class. Just initializes the database connection.
	 */
	function LicenseTemplateManager()
	{
		global $LIS_DB_SERVER, $LIS_DB_DBNAME, $LIS_DB_USRNAME, $LIS_DB_PWD;
		$this->db_con = new DatabaseHandler();
		$this->db_con->connectTo($LIS_DB_SERVER, $LIS_DB_DBNAME, $LIS_DB_USRNAME, $LIS_DB_PWD);
	}

	function storeContentKey($key, $cid) {
		$h_cid = md5($cid);
		if($this->db_con->Execute("INSERT INTO content_keys VALUES ('$cid', '$key', '$h_cid')")==false)
			return false;
		else return true;
	}

	function deleteContentKey($key, $cid) {
		if($this->db_con->Execute("DELETE FROM content_keys WHERE cid='$cid' AND key='$key'")==false)
			return false;
		else return true;
	}

	function modifyContentKey($oldkey, $newkey, $cid) {
		$timestamp = time();
		if($this->db_con->Execute("MODIFY content_keys SET key='$newkey' WHERE key='$oldkey' and cid='$cid')")==false)
			return false;
		else return true;
	}

	function checkTemplate($cid) {
		$rs=$this->db_con->Execute("SELECT * FROM content_keys WHERE cid='$cid'");
		if($rs==false) return false;
		else return true;
	}

	function getTemplateParametersNumber($tid) {
		$rs=$this->db_con->Execute("SELECT num_params FROM license_template WHERE id=$tid");
		//echo "SELECT num_params FROM license_template WHERE id=$tid";
		if($rs==false) return false;
		else return $rs->fields['num_params'];
	}

	function getLicenseTemplate($tid) {
		$rs=$this->db_con->Execute("SELECT license_template FROM license_template WHERE id=$tid");
		if($rs==false) return false;
		else return $rs->fields['license_template'];
	}

	function retrieveKey($cid) {
		$rs=$this->db_con->Execute("SELECT ckey FROM content_keys WHERE cid='$cid'");
		//echo "SELECT ckey FROM content_keys WHERE cid=$cid";
		if($rs==false) return false;
		else return $rs->fields['ckey'];
	}

	/**
	 * Closes the database access.
	 */
	function closeLicenseTemplateManager()
	{
		// close the database access
		$this->db_con->close();
	}
}
?>
