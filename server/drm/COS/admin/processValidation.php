<?php
session_start();
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/COS/admin/business/COS_Administrator.class.php");

if($_REQUEST['op']=="REGISTRATION") {
	$cos_admin = new COS_Administrator();
	if($cos_admin->createNewAdminAccount($_REQUEST['username'], $_REQUEST['password'])==true){
		$_SESSION['LOGIN']=1;
		header("Location: cosManager.php");
	} else {
		header("Location: index.php");
	}
} else {
	$cos_admin = new COS_Administrator();
	if($cos_admin->verifyLogin($_REQUEST['username'], $_REQUEST['password'])==true){
		$_SESSION['LOGIN']=1;
		header("Location: cosManager.php");
	} else {
		header("Location: index.php");
	}
}
?>
