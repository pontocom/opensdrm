<?php
session_start();
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/ITS/admin/business/ITS_Administrator.class.php");

if($_REQUEST['op']=="REGISTRATION") {
	$its_admin = new ITS_Administrator();
	//if($its_admin->createNewAdminAccount_($_REQUEST['username'], $_REQUEST['password'])==true){
    if($its_admin->createNewAdminAccount($_REQUEST['username'], $_REQUEST['password'])==true){
		$_SESSION['LOGIN']=1;
		header("Location: itsManager.php");
	} else {
		header("Location: index.php");
	}
} else {
	$its_admin = new ITS_Administrator();
    //if($its_admin->verifyLogin_($_REQUEST['username'], $_REQUEST['password'])==true){
	if($its_admin->verifyLogin($_REQUEST['username'], $_REQUEST['password'])==true){
		$_SESSION['LOGIN']=1;
		header("Location: itsManager.php");
	} else {
		header("Location: index.php");
	}
}
?>
