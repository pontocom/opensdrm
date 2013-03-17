<?php
session_start();
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/MDS/admin/business/MDS_Administrator.class.php");

if($_REQUEST['op']=="REGISTRATION") {
	$mds_admin = new MDS_Administrator();
	//if($mds_admin->createNewAdminAccount_($_REQUEST['username'], $_REQUEST['password'])==true){
    if($mds_admin->createNewAdminAccount($_REQUEST['username'], $_REQUEST['password'])==true){
		$_SESSION['LOGIN']=1;
		header("Location: mdsManager.php");
	} else {
		header("Location: index.php");
	}
} else {
	$mds_admin = new MDS_Administrator();
	//if($mds_admin->verifyLogin_($_REQUEST['username'], $_REQUEST['password'])==true){
    if($mds_admin->verifyLogin($_REQUEST['username'], $_REQUEST['password'])==true){
		$_SESSION['LOGIN']=1;
		header("Location: mdsManager.php");
	} else {
		header("Location: index.php");
	}
}
?>
