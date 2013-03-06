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
//class LicenseManager

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

include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/LIS/LicenseTemplateManager.class.php");


class LicenseManager extends DatabaseHandler
{
	var $db_con;

	/**
	 * Constructor of the Class. Just initializes the database connection.
	 */
	function LicenseManager()
	{
		global $LIS_DB_SERVER, $LIS_DB_DBNAME, $LIS_DB_USRNAME, $LIS_DB_PWD;
		$this->db_con = new DatabaseHandler();
		$this->db_con->connectTo($LIS_DB_SERVER, $LIS_DB_DBNAME, $LIS_DB_USRNAME, $LIS_DB_PWD);
	}

	function requestLicenseCreation($uid,$cid,$lic,&$ec) {
		//  put the XML inside an array and use it internally as well !!! This as LESS impact on the code!!!

		$licd = xmlize($lic);
		$licdata = array ("license_template" => $licd["licdata"]["#"]["template"][0]["#"]);
		$max_param = $licd["licdata"]["#"]["num"][0]["#"];

		//echo "template = ".$licd["licdata"]["#"]["template"][0]["#"];

		/*$n=0;
		echo "= ".$licd["licdata"]["#"]["param"][$n]["#"]."<br>";
		echo "= ".$licd["licdata"]["#"]["param"][$n+1]["#"]."<br>";
		echo "= ".$licd["licdata"]["#"]["param"][$n+2]["#"]."<br>";
		echo "= ".$licd["licdata"]["#"]["param"][$n+3]["#"]."<br>";
		echo "= ".$licd["licdata"]["#"]["param"][$n+4]["#"]."<br>";
		echo "= ".$licd["licdata"]["#"]["param"][$n+5]["#"]."<br>";*/

		for($n=0; $n < $max_param; $n++) {
			// add to array the values
			array_push($licdata, $licd["licdata"]["#"]["param"][$n]["#"]);
		}


		/*print_r($licdata);*/

		//return ;

		// first we need to check if there is a template for the cid
		$ltm = new LicenseTemplateManager();
		if($ltm->checkTemplate($cid, $e_code)==false) {
			$ec = "Failed on the checkTemplate";
			return false; // no key for that content
		} else {
			$lp=new LicenseProducer();
			if(($license = $lp->produceLicense($uid, $cid, $licdata, $ec))==false) {
				$ec = $ec."Error on the license production.";
				return false;
			} else {
				// save license
				$this->saveLicense($license, $licdata['license_template'], $uid, $cid);
				return true;
			}
		}
	}

	function requestLicenseUpdate($uid,$cid,$licdata, &$ec) {
		if($uid=="" || $cid=="") {
			$ec = "Invalid User ID or Content ID...";
			return false;
		} else {
			// check if a license for the uid,cid pair already exists
			$selquery = "SELECT * FROM license WHERE uid='$uid' AND cid='$cid' ORDER BY id ASC";
			$rs=$this->db_con->Execute($selquery);
			if($rs==false) {
				$ec = "error selecting license:".$selquery;
				return false;
			}

			$lic_id = $rs->fields['id'];

			// if yes, choose the oldest (this may be changed in the future if we allow the user to choose the license)
			// ok, now delete the old since we are going to update it

			$delquery = "DELETE FROM license WHERE id='$lic_id' AND uid='$uid' AND cid='$cid'";
			$rs=$this->db_con->Execute($delquery);
			if($rs==false) {
				$ec = "error deleting license".$delquery;
				return false;
			}

			// ok, now we create the new one
			if($this->requestLicenseCreation($uid,$cid,$licdata,$ec)==false) {
				$ec=$ec."error creating new replacing license";
				return false;
			} else {
				return true;
			}

		}
	}

	function saveLicense($lic, $tid, $uid, $cid) {
		$timestamp = time();
		$h_cid = md5($cid);
		if($this->db_con->Execute("INSERT INTO license VALUES ('$timestamp', '$tid', '$uid', '$cid', '$lic', '$h_cid')")==false)
			return false;
	}

	function requestLicenseList($uid,$cid, &$ec) {
		if($uid!="" && $cid!="") {
			$query = "SELECT license FROM license WHERE uid='$uid' AND cid='$cid'";
		} else if($uid!="" && $cid=="") {
			$query = "SELECT license FROM license WHERE uid='$uid'";
		} else if($uid=="" && $cid!="") {
			$query = "SELECT license FROM license WHERE cid='$cid'";
		}  else if($uid=="" && $cid=="") {
			$ec="Invalid listing request...";
			return false;
		}

		$rs=$this->db_con->Execute($query);
		if($rs==false) return false;

		$ll = array();
		while(!$rs->EOF) {
			array_push($ll, $rs->fields['license']);
			$rs->MoveNext();
		}

		return $ll;

	}

	function requestLicenseDownload($uid,$cid, &$ec) {
		if($uid=="" || $cid=="") {
			$ec = "Invalid User ID or Content ID...";
			return false;
		} else {
			// check the existence of the license and return the oldest -> this may parameterized on the future
			$query = "SELECT * FROM license WHERE uid='$uid' AND cid='$cid' ORDER BY id ASC";
			$ec = $query;

			$rs=$this->db_con->Execute($query);
			if($rs==false) {
				$ec = $query;
				return false;
			}

			//$rs->Move(0);
			//$rs->FetchRow();
			$lic_id = $rs->fields['id'];
			$lic = $rs->fields['license'];

			//$lic_id = $rs->Fields('id');
			//$lic = $rs->fields('license');

			$ec = $ec."-".$rs->NumRows()."-".$lic_id." - ".$lic;

			//return false;

			// delete the license
			$delquery = "DELETE FROM license WHERE id=".$lic_id;
			$rs=$this->db_con->Execute($delquery);
			if($rs==false) {
				$ec = $ec . $delquery;
				return false;
			}

			// log the license download
			$timestamp=time();
			$logquery = "INSERT INTO log_licenses VALUES ('$timestamp','LICENSE DOWNLOAD', 'The user=$uid has downloaded a license for content $cid. The license is = $lic', '$uid', '$lic_id')";
			$rs=$this->db_con->Execute($logquery);
			if($rs==false) {
				$ec = $logquery;
				return false;
			}

			// return the license
			return $lic;
		}
	}

	function requestLicenseDownloadSpecial($uid,$cid, &$ec) { // for the CID hash case
		if($uid=="" || $cid=="") {
			$ec = "Invalid User ID or Content ID...";
			return false;
		} else {
			// check the existence of the license and return the oldest -> this may parameterized on the future
			$query = "SELECT * FROM license WHERE uid='$uid' AND cid_hash='$cid' ORDER BY id ASC";
			$ec = $query;

			$rs=$this->db_con->Execute($query);
			if($rs==false) {
				$ec = $query;
				return false;
			}

			//$rs->Move(0);
			//$rs->FetchRow();
			$lic_id = $rs->fields['id'];
			$lic = $rs->fields['license'];

			//$lic_id = $rs->Fields('id');
			//$lic = $rs->fields('license');

			$ec = $ec."-".$rs->NumRows()."-".$lic_id." - ".$lic;

			//return false;

			// delete the license
			$delquery = "DELETE FROM license WHERE id=".$lic_id;
			$rs=$this->db_con->Execute($delquery);
			if($rs==false) {
				$ec = $ec . $delquery;
				return false;
			}

			// log the license download
			$timestamp=time();
			$logquery = "INSERT INTO log_licenses VALUES ('$timestamp','LICENSE DOWNLOAD', 'The user=$uid has downloaded a license for content $cid. The license is = $lic', '$uid', '$lic_id')";
			$rs=$this->db_con->Execute($logquery);
			if($rs==false) {
				$ec = $logquery;
				return false;
			}

			// return the license
			return $lic;
		}
	}


	function requestLicensePassing($uid_src, $uid_target, $cid, &$ec) {
		/* TODO */
		/* WHY? Too many issues involved:
		- Passing the all license;
		- Passing just some part of the license;
		- Via the wallet
		- Via the server
		- Security implications
		- License with the rights to pass licenses
		- etc.
		*/
	}

	function requestLicenseDelete($uid, $cid, &$ec) {
		if($uid=="" || $cid=="") {
			$ec="Invalid User Id or Content Id...";
			return false;
		} else {
			$query = "DELETE FROM license WHERE uid='$uid' AND cid='$cid'";
			$rs=$this->db_con->Execute($query);

			if($rs==false) {
				$ec = "Problems in deleting the license from the database...";
				return false;
			} else {
				return true;
			}
		}
	}


	/**
	 * Closes the database access.
	 */
	function closeLicenseManager()
	{
		// close the database access
		$this->db_con->close();
	}
}
?>
