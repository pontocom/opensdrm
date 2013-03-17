<?php
session_start();
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/RGS/admin/business/RGS_Administrator.class.php");

if($_REQUEST['op']=="REGISTRATION") {
	$rgs_admin = new RGS_Administrator();
	//if($rgs_admin->createNewAdminAccount_($_REQUEST['username'], $_REQUEST['password'])==true){
    if($rgs_admin->createNewAdminAccount($_REQUEST['username'], $_REQUEST['password'])==true){
		$_SESSION['LOGIN']=1;
		header("Location: rgsManager.php");
	} else {
		header("Location: index.php");
	}
} else {
	$rgs_admin = new RGS_Administrator();
	//if($rgs_admin->verifyLogin_($_REQUEST['username'], $_REQUEST['password'])==true){
    if($rgs_admin->verifyLogin($_REQUEST['username'], $_REQUEST['password'])==true){
		$_SESSION['LOGIN']=1;
		header("Location: rgsManager.php");
	} else {
		header("Location: index.php");
	}
}
?>
