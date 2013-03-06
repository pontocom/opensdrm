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

include_once($_SERVER["DOCUMENT_ROOT"] . "/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS["INSTALLATION_DIR"]."/CPS/configCPS.inc.php");
require_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['LIB_DIR'] . "/nusoap/nusoap.php");
require_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['LIB_DIR'] . "/common/DatabaseHandler.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['LIB_DIR'] . "/common/RSASignature.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['LIB_DIR'] . "/common/Certificate.class.php");
require_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['LIB_DIR'] . "/common/tool.php");
require_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS['LIB_DIR'] . "/common/Logger.class.php");






// Database details
$dbHost = "localhost";
$dbUser = "root";
$dbPass = "";
$dbName = "mediastore";



// Common functions

/*********************************************************
** Function: dbconnect()                                **
**     Desc: Perform database server connection and     **
**           database selection operations              **
*********************************************************/
function dbConnect() {
    // Access global variables
    global $dbHost;
    global $dbUser;
    global $dbPass;
    global $dbName;

    // Attempt to connect to database server
    $link = @mysql_connect($dbHost, $dbUser, $dbPass);

    // If connection failed...
    if (!$link) {
        // Inform Flash of error and quit
        fail("Couldn't connect to database server");
    }

    // Attempt to select our database. If failed...
    if (!@mysql_select_db($dbName)) {
        // Inform Flash of error and quit
        fail("Couldn't find database $dbName");
    }

    return $link;
}


/*********************************************************
** Function: fail()                                     **
**   Params: $errorMsg - Custom error information       **
**     Desc: Report error information back to Flash     **
**           movie and exit the script.                 **
*********************************************************/
function fail($errorMsg) {
    // URL-Encode error message
    $errorMsg = urlencode($errorMsg);

    // Output error information and exit
    print "&result=Fail&errormsg=$errorMsg";
    exit;
}














function Hash($sMessage)
{
	return MD5($sMessage) . CRC32($sMessage);
}


// Starting the debug Logger class
$log = new Logger();

// Setup of the CFS
$CFS_soapclient = new soapclient($GLOBALS['CFS_LOCATION']);


/////////////////////////////////////////////////////////////
//
// VARIABLE DECLARATION AND CHECKS
//
/////////////////////////////////////////////////////////////
$title = $_REQUEST['MN']; // Music Name
$album = $_REQUEST['AN']; // Album Name
$authors = $_REQUEST['ANM']; // Artist Name
$content_length = $_REQUEST['MLM'].':'.$_REQUEST['MLS'];
$filename = $_FILES['filename']['name'];


$hash = md5_file($_FILES['filename']['tmp_name']);

$uploadfile = 'input/'.$_FILES['filename']['name'];

if (move_uploaded_file($_FILES['filename']['tmp_name'], $uploadfile)) {
    echo "File is valid, and was successfully uploaded.<br>";
} else {
    echo "Possible file upload attack!<br>";
}



$date = date("dmy");



/**
 * UPDATE CPS DATABASE
 */
$db_con = new DatabaseHandler();
$db_con -> connectTo($CPS_DB_SERVER, $CPS_DB_DBNAME, $CPS_DB_USRNAME, $CPS_DB_PWD);
$sql = "select private_key_xml, public_key_xml, certificate from cpsws_admin";
$RS = $db_con -> executeQuery($sql);
// Retrieve Private key  and $identification
$public_key_xml = $RS -> fields['public_key_xml'];
$priv_xml = $RS -> fields['private_key_xml'];
$cert_xml = $RS -> fields['certificate'];
$pubkey = new PublicKey();
$pubkey -> loadXMLKey($public_key_xml);
$identification = $pubkey -> getHashOfKey();
$signature="";



/////////////////////////////////////////////////////////////
//
// RGS
// The RGS is called in order to register the content
//
/////////////////////////////////////////////////////////////

// Calling of the CFS
$CFS_parameters = array('id' => 'RGS');
$CFS_result     = $CFS_soapclient->call('CFSrequestServerLocation', $CFS_parameters);


// Connecting to RGS to receive a Content ID
$RGS_parameters = array('identification'                 => $identification,
                        'signature_algorithm_identifier' => 'RSA',
                        'hash'                           => $hash,
                        'filename'                       => $filename,
                        'additional_data'                => '<data>MediaNet MediaBox</data>',
                        'aus_cert'                       => $cert_xml,
                        'signature'                      => $signature
                        );
$RGS_soapclient = new soapclient($CFS_result['location']);
$RGS_result     = $RGS_soapclient->call('RGSrequestContentRegistration', $RGS_parameters);


$content_id = $RGS_result['content_id'];


// Problems with Content ID?
if ( ! $content_id || empty($content_id) ) {
  $error_msg = 'Problems with content ID, CID:'.print_r($RGS_result,TRUE);
  echo $error_msg;
  $log->Log('uploadProtectedContent.php: '.$error_msg);
  exit;
}





// Metadata to the RGS
$type           = 'Music';
$filetype       = substr(strrchr($filename, '.'), 1);
$filesize       = filesize('input/' . $filename);
$formation_date = mktime (0, 0, 0, $formation_month, $formation_day, $formation_year);
$metadata       = "<content><type>$type</type><filesize>$filesize</filesize><filetype>$filetype</filetype><content_specific><title>$title</title><album>$album</album><music_category>$category</music_category><band><name>$band</name><formation_date>$formation_date</formation_date><num_elements>$elements</num_elements><elements><name></name><role></role></elements><band_contact>$b_contact</band_contact><short_description>$b_descr</short_description><short_history>$b_history</short_history></band><music_lenght>$content_length</music_lenght><recording_year>$r_year</recording_year><recording_place>$r_place</recording_place><lyrics>$lyrics</lyrics><other_info>$info</other_info><content_specific></content>";

// Security
$signature = '';


// Connecting to RGS to send content metadata
$RGS_parameters = array(
                        'identification'                 => $identification,
                        'signature_algorithm_identifier' => 'RSA',
                        'content_id'                     => $content_id,
                        'metadata'                       => $metadata,
                        'aus_cert'                       => $cert_xml,
                        'signature'                      => $signature
                        );
$RGS_result     = $RGS_soapclient->call('RGSrequestMetadataRegistration', $RGS_parameters);
//$log->Log('uploadProtectedContent.php, RGS_result:'.print_r($RGS_result,TRUE));












/////////////////////////////////////////////////////////////
//
// CONTENT PREPARATION (PC)
//
/////////////////////////////////////////////////////////////

$input_dir = $_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR'].'/CPS/input/'.$filename;

  // External command for the content production
  // TODO: Can we have error resilient code while calling the external tool?
  /**
  Just for the purpose of the Siemens DEMO
  Author: CJCS
  */

  /**
  To encrypt: aesmp3 -e <mp3 file> <key> <content id>
  To decrypt: aesmp3 -d <mp3 file> <key>
  */

  	$kkey = strtoupper(md5(time()));
  	$ccid = md5($content_id);

  	/** to avoid the MP3 verification */
  	echo "move /Y \"".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\input\\".$filename."\" \"".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\input\\".$filename.".mp3\"<br>";
  	system("move /Y \"".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\input\\".$filename."\" \"".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\input\\".$filename.".mp3\"");


  	echo $GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\external\\aesmp3.exe -e \"".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\input\\".$filename.".mp3\" ".$kkey." ".$ccid."<br>";
	$exec_result = exec($GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\external\\aesmp3.exe -e \"".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\input\\".$filename.".mp3\" ".$kkey." ".$ccid);

	//$log->Log('uploadProtectedContent.php, exec_result:'.print_r($exec_result,TRUE));

	// copy the result file to the output directory
	echo "move /Y \"".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\input\\".$filename.".mp3.aes\" \"".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\output\\".$filename."\"<br>";
	system("move /Y \"".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\input\\".$filename.".mp3.aes\" \"".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\output\\".$filename."\"");

	echo "del /F \"".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\input\\".$filename.".mp3\"<br>";
	system("del /F \"".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\input\\".$filename.".mp3\"");

// CARLOS SERRAO: Also send an email, notifying the COS admin that a
// new file is waiting to be produced and uploaded. This should be
// temporary and for debugging purposes only.
if($GLOBALS_CONFIG['USE_MAIL_NOTIF']=='On' || $GLOBALS_CONFIG['USE_MAIL_NOTIF']=='on') {
  ini_set('SMTP', $GLOBALS_CONFIG['SMTP_SERVER']);
  mail($GLOBALS_CONFIG['EMAIL_CPS_ADMIN'], '[M4U-Adm]: New content produced: '.$content_id, "A new content ".$content_id." was produced.", "From: webmaster@{$_SERVER['SERVER_NAME']}\r\n");
}




////////////////////////////////////////////////////////////
//
// LIS
//
////////////////////////////////////////////////////////////

// Calling of the CFS
$LIS_WS = getLocation('LIS');

// Calling of the LIS
$parameters = array(
                    'key'   => $kkey,
                    'cid'    => $content_id
                    );

$soapclient = new soapclient($LIS_WS);
$result = $soapclient->call('LISrequestContentKeyStore', $parameters);

$log->Log('uploadProtectedContent.php, LIS_result:'.print_r($result,TRUE));


if ($result[0]=='-') {
	echo 'There was an error in LISrequestContentKeyStore!!!<br>';
	exit;
}


////////////////////////////////////////////////////////////
//
// MDS
//
////////////////////////////////////////////////////////////

// Calling of the CFS
$MDS_WS = getLocation('MDS');

// Calling of the MDS
$parameters = array(
                    'cid'   => $content_id,
                    'filetype'    => "MP3",
                    'protocol' => "HTTP",
                    'location' => "http://".gethostbyname($_SERVER['SERVER_NAME']).$GLOBALS['INSTALLATION_DIR']."/MDS/storage/".$filename
                    );

$soapclient = new soapclient($MDS_WS);
$result = $soapclient->call('MDSrequestContentStorage', $parameters);

if ($result[0]=='-') {
	echo 'There was an error in MDSrequestContentStorage!!!<br>';
	exit;
}

echo "move /Y \"".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\output\\".$filename."\" \"".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\MDS\\storage\\".$filename."\"<br>";
system("move /Y \"".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\output\\".$filename."\" \"".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\MDS\\storage\\".$filename."\"");

















// METE-SE AS COISAS NA MEDIASTORE
// Calcula-se o tempo em segundos
$tempo_segundos = $_REQUEST['MLM']*60+$_REQUEST['MLS'];

$link = dbConnect();
$query1 = "INSERT INTO music (idmusic, title, album, author, length_seconds, recording_year, notes) VALUES('$content_id','$title','MediaNet Album','$authors',$tempo_segundos,'2005','')";

$query2 = "INSERT INTO file (idfile, filename, path, creation_date_stp, modification_stp, size_bytes, idmusic) VALUES('$content_id','".$filename."','".$_FILES['filename']['tmp_name']."',".time().",".time().",NULL,'$content_id')";

$query3 = "INSERT INTO genre_music VALUES('$content_id',2)";

$query4 = "INSERT INTO album_music VALUES('1', '$content_id')";

/*
echo '<BR>Query1: '.$query1;
echo '<BR>Query2: '.$query2;
echo '<BR>Query3: '.$query3;
*/
$result1 = mysql_query($query1);
$result2 = mysql_query($query2);
$result3 = mysql_query($query3);
$result4 = mysql_query($query4);
















  // Final successfull redirection
  echo 'Content was uploaded and license template was created with success.<br>';
  exit;

?>
