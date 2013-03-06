<?php
session_start();
if($_SESSION['LOGIN']!=1) {
	header("Location: index.php");
}
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/AUS/admin/business/AUS_Administrator.class.php");

$ausadm = new AUS_Administrator();


if($_REQUEST['what']=='remove')
{
	$ausadm->deleteComponent_($_REQUEST['cid']);
	header('Location: ausManager.php');	
}
if($_REQUEST['what']=='logout')
{
	$_SESSION['LOGIN'] = 0;
	unset($_SESSION['LOGIN']);
	session_destroy();
	header('Location: ausManager.php');	
}

?>
