<?php
session_start();
if($_SESSION['LOGIN']!=1) {
	header("Location: index.php");
}
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/PGW/admin/business/PGW_Administrator.class.php");

$pgwadm = new PGW_Administrator();

if($_REQUEST['what']=='addnew')
{
	$pgwadm->addServer($_REQUEST['ctype'], $_REQUEST['location']);
	header('Location: pgwManager.php');	
}
if($_REQUEST['what']=='remove')
{
	$pgwadm->deleteServer($_REQUEST['ctype']);
	header('Location: pgwManager.php');	
}
if($_REQUEST['what']=='logout')
{
	$_SESSION['LOGIN'] = 0;
	unset($_SESSION['LOGIN']);
	session_destroy();
	header('Location: pgwManager.php');	
}

?>
