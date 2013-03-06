<?php
session_start();
if($_SESSION['LOGIN']!=1) {
	header("Location: index.php");
}
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/CFS/admin/business/CFS_Administrator.class.php");

$cfsadm = new CFS_Administrator();

if($_REQUEST['what']=='addnew')
{
	$cfsadm->addServer($_REQUEST['ctype'], $_REQUEST['location']);
	header('Location: cfsManager.php');	
}
if($_REQUEST['what']=='remove')
{
	$cfsadm->deleteServer($_REQUEST['ctype']);
	header('Location: cfsManager.php');	
}
if($_REQUEST['what']=='logout')
{
	$_SESSION['LOGIN'] = 0;
	unset($_SESSION['LOGIN']);
	session_destroy();
	header('Location: cfsManager.php');	
}

?>
