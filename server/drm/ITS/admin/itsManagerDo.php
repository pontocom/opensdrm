<?php
session_start();
if($_SESSION['LOGIN']!=1) {
	header("Location: index.php");
}
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/ITS/admin/business/ITS_Administrator.class.php");

$itsadm = new ITS_Administrator();

if($_REQUEST['what']=='addnew')
{
	//$itsadm->addServer($_REQUEST['ctype'], $_REQUEST['location']);
    //$itsadm->addnewIPMPtool(/*IPMPTOOLID*/, /*IPMPTOOLURL*/, /*IPMPTOOLDESCRIPTION*/);
	header('Location: itsManager.php');	
}
if($_REQUEST['what']=='remove')
{
	$itsadm->deleteServer($_REQUEST['ctype']);
	header('Location: itsManager.php');	
}
if($_REQUEST['what']=='logout')
{
	$_SESSION['LOGIN'] = 0;
	unset($_SESSION['LOGIN']);
	session_destroy();
	header('Location: itsManager.php');	
}

?>
