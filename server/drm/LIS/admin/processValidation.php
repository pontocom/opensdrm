<?php
session_start();
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/LIS/admin/business/LIS_Administrator.class.php");

if($_REQUEST['op']=="REGISTRATION") {
	$lis_admin = new LIS_Administrator();
	//if($lis_admin->createNewAdminAccount_($_REQUEST['username'], $_REQUEST['password'])==true){
    if($lis_admin->createNewAdminAccount($_REQUEST['username'], $_REQUEST['password'])==true){
		$_SESSION['LOGIN']=1;
		header("Location: lisManager.php");
	} else {
		header("Location: index.php");
	}
} else {
	$lis_admin = new LIS_Administrator();
	//if($lis_admin->verifyLogin_($_REQUEST['username'], $_REQUEST['password'])==true){
    if($lis_admin->verifyLogin($_REQUEST['username'], $_REQUEST['password'])==true){
		$_SESSION['LOGIN']=1;
		header("Location: lisManager.php");
	} else {
		header("Location: index.php");
	}
}
?>
