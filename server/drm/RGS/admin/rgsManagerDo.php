<?php
session_start();
if($_SESSION['LOGIN']!=1) {
	header("Location: index.php");
}
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/RGS/admin/business/RGS_Administrator.class.php");

$rgsadm = new RGS_Administrator();

if($_REQUEST['what']=='addnew')
{
	$rgsadm->addServer($_REQUEST['ctype'], $_REQUEST['location']);
	header('Location: rgsManager.php');	
}
if($_REQUEST['what']=='remove')
{
	$pgwadm->deleteServer($_REQUEST['ctype']);
	header('Location: rgsManager.php');	
}
if($_REQUEST['what']=='logout')
{
	$_SESSION['LOGIN'] = 0;
	unset($_SESSION['LOGIN']);
	session_destroy();
	header('Location: rgsManager.php');	
}

?>
