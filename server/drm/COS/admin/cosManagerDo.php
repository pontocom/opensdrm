<?php
session_start();
if($_SESSION['LOGIN']!=1) {
	header("Location: index.php");
}
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/COS/admin/business/COS_Administrator.class.php");

$cfsadm = new COS_Administrator();

if($_REQUEST['what']=='addnew')
{
	$cfsadm->addServer($_REQUEST['ctype'], $_REQUEST['location']);
	header('Location: cosManager.php');	
}
if($_REQUEST['what']=='remove')
{
	$cfsadm->deleteServer($_REQUEST['ctype']);
	header('Location: cosManager.php');	
}
if($_REQUEST['what']=='subscribePGW')
{
	$cos_admin = new COS_Administrator();
	$list_pgw=$cos_admin->getListPGW();
	if($list_pgw==false)
	{
		$retval = implode('*', array('ERROR'=>'<font color=red>There are no available PGW.</font>'));
	} else {
		$retval = implode('*', array('LIST'=>$list_pgw));
	}
	echo "<script>document.location='cosManager.php?next=".$retval."'</script>";
}
if($_REQUEST['what']=='logout')
{
	$_SESSION['LOGIN'] = 0;
	unset($_SESSION['LOGIN']);
	session_destroy();
	header('Location: cosManager.php');	
}

?>
