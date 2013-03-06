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
//class LicenseProducer

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
require_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS["LIB_DIR"]."/common/xmlize.inc.php");

include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/LIS/LicenseTemplateManager.class.php");


class LicenseProducer extends DatabaseHandler
{
	var $db_con;

	/**
	 * Constructor of the Class. Just initializes the database connection.
	 */
	function LicenseProducer()
	{
		global $LIS_DB_SERVER, $LIS_DB_DBNAME, $LIS_DB_USRNAME, $LIS_DB_PWD;
		$this->db_con = new DatabaseHandler();
		$this->db_con->connectTo($LIS_DB_SERVER, $LIS_DB_DBNAME, $LIS_DB_USRNAME, $LIS_DB_PWD);
	}

/*	function produceLicense($uid, $cid, $licdata, &$ec) {
		$ltm = new LicenseTemplateManager();

		if(($val=$ltm->getTemplateParametersNumber($licdata['license_template'])) == false) {
			$ec = "No license template found";
			return false;
		} else {
			if(($lt=$ltm->getLicenseTemplate($licdata['license_template'])) == false) {
				$ec = "Couldn't retrieve license template from database";
				return false;
			} else {
				if(($key=$ltm->retrieveKey($cid)) == false) {
					$ec = "Couldn't retrieve content key from database";
					return false;
				} else {
					$lic = $lt;
					for($n=0; $n<$val; $n++) {
						$vtr = "%PARAM_".($n+1)."%";
						$lic = str_replace($vtr, $licdata[$n], $lic);
					}
					$lic = str_replace("%KEY%", $key, $lic);
					return $lic;
				}
			}
		}
	}
*/
	function produceLicense($uid, $cid, $licdata, &$ec) {
		$ltm = new LicenseTemplateManager();

		if(($val=$ltm->getTemplateParametersNumber($licdata["license_template"])) == false) {
			$ec = "No license template found";
			return false;
		} else {
			if(($lt=$ltm->getLicenseTemplate($licdata["license_template"])) == false) {
				$ec = "Couldn't retrieve license template from database";
				return false;
			} else {
				if(($key=$ltm->retrieveKey($cid)) == false) {
					$ec = "Couldn't retrieve content key from database";
					return false;
				} else {
					$lic = $lt;
					for($n=0; $n<$val; $n++) {
						$vtr = "%PARAM_".($n+1)."%";
						$lic = str_replace($vtr, $licdata[$n], $lic);
					}
					$lic = str_replace("%KEY%", $key, $lic);
					return $lic;
				}
			}
		}
	}

	/**
	 * Closes the database access.
	 */
	function closeLicenseProducer()
	{
		// close the database access
		$this->db_con->close();
	}
}
?>
