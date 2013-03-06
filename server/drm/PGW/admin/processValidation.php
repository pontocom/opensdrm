<?php
session_start();
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/PGW/admin/business/PGW_Administrator.class.php");

if($_REQUEST['op']=="REGISTRATION") {
	$pgw_admin = new PGW_Administrator();
	if($pgw_admin->createNewAdminAccount_($_REQUEST['username'], $_REQUEST['password'])==true){
		$_SESSION['LOGIN']=1;
		header("Location: pgwManager.php");
	} else {
		header("Location: index.php");
	}
} else {
	$pgw_admin = new PGW_Administrator();
	if($pgw_admin->verifyLogin_($_REQUEST['username'], $_REQUEST['password'])==true){
		$_SESSION['LOGIN']=1;
		header("Location: pgwManager.php");
	} else {
		header("Location: index.php");
	}
}
?>
