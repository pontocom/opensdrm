<?php
session_start();
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/CFS/admin/business/CFS_Administrator.class.php");

if($_REQUEST['op']=="REGISTRATION") {
	$cfs_admin = new CFS_Administrator();
	if($cfs_admin->createNewAdminAccount_($_REQUEST['username'], $_REQUEST['password'])==true){
		$_SESSION['LOGIN']=1;
		header("Location: cfsManager.php");
	} else {
		header("Location: index.php");
	}
} else {
	$cfs_admin = new CFS_Administrator();
	if($cfs_admin->verifyLogin($_REQUEST['username'], $_REQUEST['password'])==true){
		$_SESSION['LOGIN']=1;
		header("Location: cfsManager.php");
	} else {
		header("Location: index.php");
	}
}
?>
