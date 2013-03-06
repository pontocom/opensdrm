<?php
session_start();
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/AUS/admin/business/AUS_Administrator.class.php");

if($_REQUEST['op']=="REGISTRATION") {
	$aus_admin = new AUS_Administrator();
	if($aus_admin->createNewAdminAccount_($_REQUEST['username'], $_REQUEST['password'])==true){
		$_SESSION['LOGIN']=1;
		header("Location: ausManager.php");
	} else {
		header("Location: index.php");
	}
} else {
	$aus_admin = new AUS_Administrator();
	if($aus_admin->verifyLogin_($_REQUEST['username'], $_REQUEST['password'])==true){
		$_SESSION['LOGIN']=1;
		header("Location: ausManager.php");
	} else {
		header("Location: index.php");
	}
}
?>
