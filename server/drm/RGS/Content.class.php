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

include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/RGS/configRGS.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/DatabaseHandler.class.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['LIB_DIR']."/common/Logger.class.php");


class Content
{
	var $db_con;
	var $content_id;
	var $hash;
	var $file;
	var $metadata;


	function Content()
	{
		// instanciate the normal database connection using ADODB
		//global $RGS_DB_SERVER, $RGS_DB_DBNAME, $RGS_DB_USRNAME, $RGS_DB_PWD;
		$this->db_con = new DatabaseHandler();
		$this->db_con->connectTo($GLOBALS['RGS_DB_SERVER'], $GLOBALS['RGS_DB_DBNAME'], $GLOBALS['RGS_DB_USRNAME'], $GLOBALS['RGS_DB_PWD']);
	}

	function set($hash, $_file, $metadata) {
		$this->hash = $hash;
		$this->_file = $_file;
		$this->metadata = $metadata;
	}

	function register() {
    	//$l = new Logger("RGS");
    	//$l->Log("RGS:Content:register");

		global $REGISTRATION_NUMBER, $INTERNAL_ID;

		// go to the database and read the last value of 'internal_id'
		$sql_command = "SELECT MAX(internal_id) AS max_id FROM ".$GLOBALS['RGS_DB_DBNAME'].".rgsws_content";
		//$l->Log("RGS:Content:register:sql_command:".$sql_command);
		$rs = $this->db_con->executeQuery($sql_command);
		/*if($rs->RecordCount()==0) {
			$last_id = 0;
		}
		else {
			$last_id = $rs->fields["max_id"];
		}*/

		$n_id = $rs->fields["max_id"] + 1;
		$next_id = sprintf("%08d", $n_id);

		// issue a number which is based on the GLOBAL vars and the 'internal_id +1'
		// register everything on the database
		// return content id value.
		//$GLOBALS_CONFIG['REGISTRATION_NUMBER'] = 'urn:mpeg:mpeg21:diid:doi:'; // we are using DOI
		//$GLOBALS_CONFIG['INTERNAL_ID'] = 'pt/adetti/music/';

		$this->content_id = $REGISTRATION_NUMBER.$INTERNAL_ID.$next_id;

		$sql_command = "INSERT INTO ".$GLOBALS['RGS_DB_DBNAME'].".rgsws_content VALUES ('$this->content_id', '$n_id', '$this->hash', '$this->_file', '$this->metadata')";
		//$l->Log("RGS:Content:register:sql_command:".$sql_command);

		$rs=$this->db_con->executeQuery($sql_command);

		if($rs==false) return false;
		else return $this->content_id;
	}

	function store_metadata($content_id, $metadata) {
		// go to the database to the appropriate field and update it
		$sql_command = "UPDATE ".$GLOBALS['RGS_DB_DBNAME'].".rgsws_content SET metadata='$metadata' WHERE content_id='$content_id'";
    	//$l = new Logger("RGS");
    	//$l->Log("RGS:Content:register");
		//$l->Log("RGS:Content:register:sql_command:".$sql_command);

		$rs = $this->db_con->executeQuery($sql_command);
		if($rs==false) return false;
		else return true;
	}

	function handleQuery($query) {
    	//$l = new Logger("RGS");
    	//$l->Log("RGS:Content:handleQuery");

		/*$xmlquery = xmlize($query);
		$op = $xmlquery["query"]["#"]["operation"][0]["#"];

		if($op=="AND" || $op=="and") {
			$sql_command="SELECT * FROM rgsWS_Content WHERE ";

			$band= $xmlquery["query"]["#"]["band"][0]["#"];
			$album= $xmlquery["query"]["#"]["album"][0]["#"];
			$music_title= $xmlquery["query"]["#"]["music-title"][0]["#"];
			$music_category= $xmlquery["query"]["#"]["music-category"][0]["#"];

			if($band!="") $sql_command=$sql_command."metadata LIKE '%<name>".$band."</name>%' ";
			else $sql_command=$sql_command."metadata LIKE '%' ";

			if($album!="") $sql_command=$sql_command."AND metadata LIKE '%<album>".$album."</album>%' ";
			else $sql_command=$sql_command."AND metadata LIKE '%' ";

			if($music_title!="") $sql_command=$sql_command."AND metadata LIKE '%<title>".$music_title."</title>%' ";
			else $sql_command=$sql_command."AND metadata LIKE '%' ";

			if($music_category!="") $sql_command=$sql_command."AND metadata LIKE '%<music_category>".$music_category."</music_category>%' ";
			else $sql_command=$sql_command."AND metadata LIKE '%' ";

		} else if($op=="OR" || $op=="or") {
			$band= $xmlquery["query"]["#"]["band"][0]["#"];
			$album= $xmlquery["query"]["#"]["album"][0]["#"];;
			$music_title= $xmlquery["query"]["#"]["music-title"][0]["#"];
			$music_category= $xmlquery["query"]["#"]["music-category"][0]["#"];
			$sql_command="SELECT * FROM rgsWS_Content WHERE metadata LIKE '%<name>".$band."</name>%' OR metadata LIKE '%<album>".$album."</album>%' OR metadata LIKE '%<title>".$music_title."</title>%' OR metadata LIKE '%<music_category>".$music_category."</music_category>%'";
		} else if($op=="ALL" || $op=="all"){
			$sql_command="SELECT * FROM rgsWS_Content";
		}*/

		if($query=="") $sql_command="SELECT * FROM ".$GLOBALS['RGS_DB_DBNAME'].".rgsws_content";
		else {
			$sql_command="SELECT * FROM ".$GLOBALS['RGS_DB_DBNAME'].".rgsws_content WHERE ".$query;
		}

    	//$l->Log("RGS:Content:handleQuery:sql_command:".$sql_command );

		$rs = $this->db_con->executeQuery($sql_command);
		if($rs->RecordCount()==0) {
			return false;
		} else {
			$data="<content_list><number_items>".$rs->RecordCount()."</number_items>";
	    	//$l->Log("RGS:Content:handleQuery:data:".$data );
			while(!$rs->EOF) {
				$data=$data."<content_id>".$rs->fields["content_id"]."</content_id>";
				$rs->MoveNext();
			}
			$data=$data."</content_list>";
	    	//$l->Log("RGS:Content:handleQuery:data:".$data );

			return $data;
		}
	}

	function returnMetadata($contentId) {
		$sql_command = "SELECT * FROM ".$GLOBALS['RGS_DB_DBNAME'].".rgsws_content WHERE content_id='".$contentId."'";
		$rs = $this->db_con->executeQuery($sql_command);
		if($rs->RecordCount()==0) {
			return false;
		} else {
			return $rs->fields["metadata"];
		}
	}
}
?>
