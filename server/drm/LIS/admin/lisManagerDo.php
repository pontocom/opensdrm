<?php
session_start();
if($_SESSION['LOGIN']!=1) {
	header("Location: index.php");
}
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/LIS/admin/business/LIS_Administrator.class.php");

$lisadm = new LIS_Administrator();

if($_REQUEST['what']=='addnew')
{
	$lisadm->addServer($_REQUEST['ctype'], $_REQUEST['location']);
	header('Location: lisManager.php');	
}
if($_REQUEST['what']=='remove')
{
	$lisadm->deleteServer($_REQUEST['ctype']);
	header('Location: lisManager.php');	
}
if($_REQUEST['what']=='logout')
{
	$_SESSION['LOGIN'] = 0;
	unset($_SESSION['LOGIN']);
	session_destroy();
	header('Location: lisManager.php');	
}

?>
