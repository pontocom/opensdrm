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

include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/MDS/ContentStorageService.class.php");


class ContentDeliveryService extends DatabaseHandler
{
	var $db_con;

	/**
	 * Constructor of the Class. Just initializes the database connection.
	 */
	function ContentDeliveryService()
	{
		global $MDS_DB_SERVER, $MDS_DB_DBNAME, $MDS_DB_USRNAME, $MDS_DB_PWD;
		$this->db_con = new DatabaseHandler();
		$this->db_con->connectTo($MDS_DB_SERVER, $MDS_DB_DBNAME, $MDS_DB_USRNAME, $MDS_DB_PWD);
	}

	function deliverContent($cid, $uid, &$ec) {
		if($cid=="" || $uid =="") {
			$ec = "Error, there are empty fields...";
			return false;
		} else {
			$css = new ContentStorageService();
			if($css->checkContent($cid, $ec)==false) {
				return false;
			} else {
				$id = time();
				$query="INSERT INTO content_delivery VALUES('$id', '$cid', '$uid', 'MARKED_TO_BE_DELIVERED')";
				$rs=$this->db_con->Execute($query);

				if($rs==false) {
					$ec = "Problems in marking content to be delivered...";
					return false;
				} else {
					return true;
				}
			}
		}
	}

	function downloadContent($cid, $uid, &$lurl, &$ec) {
		$l = new Logger();
		if($cid=="" || $uid =="") {
			$ec = "Error, there are empty fields...";
			//echo $ec;
			return false;
		} else {
			$css = new ContentStorageService();
			if($css->checkContent($cid, $ec)==false) {
				$ec = "Error checking content...";
				//echo $ec;
				return false;
			} else {
				$query="SELECT * FROM content_delivery WHERE cid='$cid' AND uid='$uid' AND status='MARKED_TO_BE_DELIVERED'";
				$l->Log($query);
				$rs=$this->db_con->Execute($query);

				if($rs==false || $rs->EOF) {
					$ec = "Problems in marking content to be downloaded...";
					//echo $ec;
					return false;
				} else {
					//$DURL = $rs->fields['location'];
					$tid = $rs->fields['id'];

					$DURL = $css->getLocation($cid);
					$l->Log($DURL);

					$query = "UPDATE content_delivery SET status='DOWNLOADED' WHERE id='$tid'";
					//echo $query;
					$rs=$this->db_con->Execute($query);
					if($rs==false) {
						$ec = "Problems in updating the table...";
						//echo $ec;
						return false;
					} else {
						$lurl = $DURL;
						//echo $lurl;
						return true;
					}
				}
			}
		}
	}

	/**
	 * Closes the database access.
	 */
	function closeContentDeliveryService()
	{
		// close the database access
		$this->db_con->close();
	}
}
?>
