<?php
session_start();

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

// VARIABLES
// TODO: MUDAR PARA VARIÁVEIS DE CONFIGURAÇÂO
$M4U_SERVER = $GLOBALS_CONFIG['M4U_SERVER'];

// Starting the debug Logger class
$log = new Logger();

// Setup of the CFS
$CFS_soapclient = new soapclient($GLOBALS['CFS_LOCATION']);









/////////////////////////////////////////////////////////////
//
// AUTHENTICATION
//
/////////////////////////////////////////////////////////////

// Calling of the CFS
$CFS_parameters = array('id' => 'AUS');
$CFS_result     = $CFS_soapclient->call('CFSrequestServerLocation', $CFS_parameters);


// Calling of the AUS
$uid       = $_REQUEST['AUS_id'];
$login     = $_REQUEST['username'];
$hash      = $_REQUEST['hval'];
$idaccount = $_REQUEST['idaccount'];

$AUS_parameters = array(
                        'uid'   => $uid,
                        'login' => $login,
                        'hash'  => $hash
                        );
$AUS_soapclient = new soapclient($CFS_result['location']);
$AUS_result     = $AUS_soapclient->call('AUSrequestMutualValidation', $AUS_parameters);
//print_r($AUS_parameters);
//echo $AUS_result;
//exit;

$log->Log('uploadProtectedContent.php, AUS_result:'.print_r($AUS_result,TRUE));
//exit;

// Checking of the AUS result message
if ( $AUS_result[0] == '-' /*||
     !isset($_SESSION['PERSISTENTVAR_USER_LOGGEDIN']) ||
     $_SESSION['PERSISTENTVAR_USER_LOGGEDIN'] != TRUE*/)
{
  $error_message = urlencode('User can not be validated by AUS - OpenSDRM.');
  header('Location: ' . $M4U_SERVER . 'music_new.php?idband=' . $_REQUEST['idband'].'&idaccount='.$idaccount.'&err_msg='.$error_message.'&idmusic='.$_REQUEST['idmusic'].'&success=FALSE');
  exit;
}
else {
  $_SESSION['PERSISTENTVAR_USER_LOGGEDIN'] = TRUE;
}










/////////////////////////////////////////////////////////////
//
// VARIABLE DECLARATION AND CHECKS
//
/////////////////////////////////////////////////////////////
$title = $_REQUEST['title'];
$album = $_REQUEST['album'];
// $authors = $_POST['authors'];
$count = "1";
$category = $_REQUEST['combobox_music_categories'];
$band = $_REQUEST['idband'];
$formation_day = 'XX';
$formation_month = 'XX';
$formation_year = 'XX';
$elements = '';
$b_contact = '';
$b_descr = '';
$b_history = '';
$content_length = $_REQUEST['combobox_minutes'].':'.$_REQUEST['combobox_seconds'];
$CLEN = $content_length; // a copy variable, it seems the other gets destroyed in the process!!!
//echo $content_length;

$r_year = $_REQUEST['recording_year'];
$r_place = $_REQUEST['recording_place'];
$lyrics = '';
$info = $_REQUEST['notes'];
$filename = $_REQUEST['filename'];
//$filename = 'demo.mp3';
$date = date("dmy");

if (isset($_REQUEST['finger']) && $_REQUEST['finger']==1){
	$_REQUEST['ver'] == 'PC' ? $fingerprint = "-w" : $fingerprint = "-wmk";
}

function Hash($sMessage)
{
	return MD5($sMessage) . CRC32($sMessage);
}











// -------------- GETS DATA FROM THE CPS DATABASE
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
// create a new PrivateKey object based on the XML private key $private_xml
//$pk = new PrivateKey();
//$pk -> loadXMLkey($priv_xml);
//$sig = new RSASignature();
//$data_to_sign = implode("", array("$identification", "RSA", "$hash", "$filename", "<data>$arbitrary_data</data>", "$cert_xml"));
//$signature = $sig -> RSA_sign($data_to_sign, $pk);
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
                        'additional_data'                => '<data>'.$arbitrary_data.'</data>',
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
//$data_to_sign = implode("", array("$identification", "RSA", "$content_id", "$metadata", "$cert_xml"));
//$signature = $sig -> RSA_sign($data_to_sign, $pk);
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
// CONTENT PREPARATION (PDA and PC)
// This was where the old TILab tool would be used
//
/////////////////////////////////////////////////////////////

//if ($_REQUEST['ver'] == 'PDA') {
//  echo 'PDA version of content preparation no longer in use.';
//  exit;
//}

//
// PC VERSION. PDA version no longer used...
//
//else if ($_REQUEST['ver'] == 'PC') {
$input_dir = $_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR'].'/CPS/input/'.$filename;

  /*
    $result     = move_uploaded_file($filename, $uploaddir);
    if ($result){
    // print "<br><br>File is valid, and was successfully uploaded.\n";
    // print_r($_FILES);
    //}else{
    //print "<br><br>File is NOT valid, and was NOT successfully uploaded:\n";
    //echo "<br><br><br><BR><p align=center> <a href='http://localhost/music-4you/src/home.php' class='LinkPreto'>HOME</a> ||  <a href='upload.php?uid=$uid&login=$login&hash=$hash' class='LinkPreto'>Upload more content</a></P><br><br><br>";
    // print_r($_FILES);
    //die ();
    }
  */

  // TODO: This code will probably be used later in the game
  // when our content cipherer uses dynamic keys...
  /*
   // Ok, in this case we use time to compute an hash, replace all the 'E' and even replace the last two bytes by zeros -> Player problem
    $keyValue = str_replace("E", "0", strtoupper(md5(time())));
    $keyValue[strlen($keyValue)-1] = '0';
    $keyValue[strlen($keyValue)-2] = '0';
    $encript = "-e ".$keyValue;
  */

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
  	system("move /Y ".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\input\\".$filename." ".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\input\\".$filename.".mp3");

  	$filename = $filename.".mp3";
  	//echo 'd:\\WWW\\DemoSiemens\\opensdrm-development\\CPS\\external\\aesmp3.exe -e d:\\WWW\\DemoSiemens\\opensdrm-development\\CPS\\input\\'.$filename.' '.$kkey.' '.$ccid;
	if($_REQUEST['ver'] == 'PDA') {
		$exec_result = exec($GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\external\\aesmp3_pda.exe -e ".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\input\\".$filename." ".$kkey." ".$ccid);
	} else {
		$exec_result = exec($GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\external\\aesmp3.exe -e ".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\input\\".$filename." ".$kkey." ".$ccid);
	}
	//$log->Log('uploadProtectedContent.php, exec_result:'.print_r($exec_result,TRUE));

	// copy the result file to the output directory

	system("move /Y ".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\input\\".$filename.".aes ".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\output\\".$filename);
	//system("del /Y d:\\WWW\\DemoSiemens\\opensdrm-development\\CPS\\input\\".$filename.".aes");
	system("del /F ".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\input\\".$filename);

// CARLOS SERRAO: Also send an email, notifying the COS admin that a
// new file is waiting to be produced and uploaded. This should be
// temporary and for debugging purposes only.
if($GLOBALS_CONFIG['USE_MAIL_NOTIF']=='On' || $GLOBALS_CONFIG['USE_MAIL_NOTIF']=='on') {
  ini_set('SMTP', $GLOBALS_CONFIG['SMTP_SERVER']);
  mail($GLOBALS_CONFIG['EMAIL_CPS_ADMIN'], '[M4U-Adm]: New content produced: '.$content_id, "A new content ".$content_id." was produced.", "From: webmaster@{$_SERVER['SERVER_NAME']}\r\n");
}
//}





////////////////////////////////////////////////////////////
//
// LIS
//
////////////////////////////////////////////////////////////

// Calling of the CFS
$LIS_WS = getLocation('LIS');

//print_r($LIS_WS);

// Calling of the LIS
$parameters = array(
                    'key'   => $kkey,
                    'cid'    => $content_id
                    );

$soapclient = new soapclient($LIS_WS);
$result = $soapclient->call('LISrequestContentKeyStore', $parameters);

$log->Log('uploadProtectedContent.php, LIS_result:'.print_r($result,TRUE));

//print_r($result);

if ($result[0]=='-') {
	$error_message = 'There was an error in LISrequestContentKeyStore!!!';
	header('Location: ' . $M4U_SERVER . 'music_new.php?idband=' . $_REQUEST['idband'].'&idaccount='.$idaccount.'&err_msg='.$error_message.'&idmusic='.$_REQUEST['idmusic'].'&success=FALSE');
	return false;
}


////////////////////////////////////////////////////////////
//
// MDS
//
////////////////////////////////////////////////////////////

// Calling of the CFS
$MDS_WS = getLocation('MDS');

//print_r($MDS_WS);

// Calling of the MDS
$parameters = array(
                    'cid'   => $content_id,
                    'filetype'    => "MP3",
                    'protocol' => "HTTP",
                    'location' => "http://".gethostbyname($_SERVER['SERVER_NAME']).$GLOBALS['INSTALLATION_DIR']."/MDS/storage/".$filename
                    );

//print_r($parameters);

$soapclient = new soapclient($MDS_WS);
$result = $soapclient->call('MDSrequestContentStorage', $parameters);

//print_r($result);

if ($result[0]=='-') {
	$error_message = 'There was an error in MDSrequestContentStorage!!!';
	header('Location: ' . $M4U_SERVER . 'music_new.php?idband=' . $_REQUEST['idband'].'&idaccount='.$idaccount.'&err_msg='.$error_message.'&idmusic='.$_REQUEST['idmusic'].'&success=FALSE');
	return false;
}

system("move /Y ".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\CPS\\output\\".$filename." ".$GLOBALS_CONFIG['ROOT_PATH']."\\opensdrm\\MDS\\storage\\".$filename);


  // Final successfull redirection
  $error_message = urlencode('Content was uploaded and license template was created with success.');
  header('Location: ' . $M4U_SERVER . 'music_new.php?idband=' . $_REQUEST['idband'].'&idaccount='.$idaccount.'&err_msg='.$error_message.'&cid='.$content_id.'&idmusic='.$_REQUEST['idmusic'].'&success=TRUE');
  exit;

?>
