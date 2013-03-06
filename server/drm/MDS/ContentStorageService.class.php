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
//class ContentStorageService

include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/MDS/configMDS.inc.php");
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


class ContentStorageService extends DatabaseHandler
{
	var $db_con;

	/**
	 * Constructor of the Class. Just initializes the database connection.
	 */
	function ContentStorageService()
	{
		global $MDS_DB_SERVER, $MDS_DB_DBNAME, $MDS_DB_USRNAME, $MDS_DB_PWD;
		$this->db_con = new DatabaseHandler();
		$this->db_con->connectTo($MDS_DB_SERVER, $MDS_DB_DBNAME, $MDS_DB_USRNAME, $MDS_DB_PWD);
	}

	function storeContent($cid, $filetype, $protocol, $location, &$ec) {
		$l = new Logger();
		if($cid=="" || $filetype=="" || $protocol=="" || $location =="") {
			$ec = "Error, there are empty fields...";
			return false;
		} else {
			$id = time();
			$query="INSERT INTO content_location VALUES('$id', '$cid', '$filetype', '$protocol', '$location')";
			$l->Log($query);
			$rs=$this->db_con->Execute($query);

			if($rs==false) {
				$ec = "Problems in inserting the content on the database...";
				return false;
			} else {
				return true;
			}
		}
	}

	function deleteContent($content_id, &$ec) {
		if($content_id=="") {
			$ec="Invalid Content Id...";
			return false;
		} else {
			$query = "DELETE FROM content_location WHERE cid='$content_id'";
			$rs=$this->db_con->Execute($query);

			if($rs==false) {
				$ec = "Problems in deleting the content from the database...";
				return false;
			} else {
				return true;
			}
		}
	}

	function listContent() {
		$query = "SELECT * FROM content_location";
		$rs=$this->db_con->Execute($query);

		if($rs==false) return false;

		$cl = array();
		while(!$rs->EOF) {
			array_push($cl, $rs->fields['cid']);
			array_push($cl, $rs->fields['location']);
			$rs->MoveNext();
		}

		return $cl;
	}

	function checkContent($cid, &$ec) {
		if($cid=="") {
			$ec = "Error, content id cannot be empty...";
			return false;
		} else {
			$query = "SELECT * FROM content_location WHERE cid='$cid'";
			$rs=$this->db_con->Execute($query);

			if($rs==false) {
				return false;
			}
			else {
				if($cid==$rs->fields['cid']) {
					return true;
				} else {
					return false;
				}
			}

		}
	}

	function updateContent($cid, $filetype, $protocol, $location, &$ec) {
		if($cid=="") {
			$ec = "Error, content id cannot be empty...";
			return false;
		} else {
			$query="UPDATE content_location SET type='$filetype', protocol='$protocol', location='$location' WHERE cid='$cid'";
			$rs=$this->db_con->Execute($query);

			if($rs==false) {
				$ec = "Problems in updating the content on the database...";
				return false;
			} else {
				return true;
			}
		}
	}

	function getLocation($cid) {
		$query = "SELECT * FROM content_location WHERE cid='".$cid."'";
		$rs=$this->db_con->Execute($query);

		if($rs==false) return false;
		else return $rs->fields['location'];
	}


	/**
	 * Closes the database access.
	 */
	function closeContentStorageService()
	{
		// close the database access
		$this->db_con->close();
	}
}
?>
