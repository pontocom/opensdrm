<?php
session_start();
if($_SESSION['LOGIN']!=1) {
	header("Location: index.php");
}
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/MDS/admin/business/MDS_Administrator.class.php");

$mdsadm = new MDS_Administrator();

if($_REQUEST['what']=='addnew')
{
	$mdsadm->addServer($_REQUEST['ctype'], $_REQUEST['location']);
	header('Location: mdsManager.php');	
}
if($_REQUEST['what']=='remove')
{
	$mdsadm->deleteServer($_REQUEST['ctype']);
	header('Location: mdsManager.php');	
}
if($_REQUEST['what']=='logout')
{
	$_SESSION['LOGIN'] = 0;
	unset($_SESSION['LOGIN']);
	session_destroy();
	header('Location: mdsManager.php');	
}

?>
