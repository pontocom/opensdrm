<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>Untitled Document</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"><br>
<link rel="stylesheet" href="cos_admin.css" type="text/css"></head>
<body bgcolor="#FFFFC0"> 
<?php
if(!isset($_REQUEST['op']) || $_REQUEST['op']=="MAIN") {
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Administrator Login</font></strong><br>";
	if(!isset($_REQUEST['login_status']) || $_REQUEST['login_status']!="ON") {
		include_once("../business/PGW_Administrator.class.php");
		$admin = new PGW_Administrator();
		$astatus=$admin->verifyAdminStatus();
		if($astatus==true) {
?>
<form name="form1" method="post" action="<?php echo $PHP_SELF;?>">
  <font size="2" face="Verdana, Arial, Helvetica, sans-serif">Username: 
  <input name="username" type="text" id="username">
  <br>
  Password: 
  <input name="password" type="password" id="password">
  <input type="hidden" name="login_status" value="ON">
  <br>
  <input type="submit" name="Submit" value="Enter">
  </font> 
</form>
<?php
		} else if ($astatus==false) {
?>
<font size="2" face="Verdana, Arial, Helvetica, sans-serif">You have not setup an account yet. You can do it [<a href="register.php" target="mainFrame">NOW</a>].</font><br>
<?php
		}
	} else if ($_REQUEST['login_status']=="ON") {
		include_once("../business/PGW_Administrator.class.php");
		$admn = new PGW_Administrator();
		if($admn->verifyLogin($_REQUEST['username'], $_REQUEST['password'])!=true) {
			echo "<SCRIPT>document.location='main.php?login_status=OFF';</SCRIPT>";
		} else {
			$login_status="ON";
			echo "<SCRIPT>top.location='index.php?op=ON';</SCRIPT>";
		}
	}
	
} else if ($_REQUEST['op']=="IN_MAN") {
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Internal Management</font></strong><br>";
	echo "Please select operation:<br><br><a href=\"main.php?op=U_ADM_VIEW\">View Internal Data</a><br><br><a href=\"main.php?op=U_ADM_EXPORT\">Export Data to file</a></br>";
} else if ($_REQUEST['op']=="U_ADM_VIEW") {
	include_once("../business/PGW_Administrator.class.php");
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Internal Administration</font></strong><br>";
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">View data</font></strong><br>";
	$adm = new PGW_Administrator();
	$adm->getData();
	$adm->printInternalData();
	
} else if ($_REQUEST['op']=="U_ADM_EXPORT") {
	include_once("../business/PGW_Administrator.class.php");
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Internal Administration</font></strong><br>";
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Export data</font></strong><br>";
	$adm = new PGW_Administrator();
	$adm->getData();
	$adm->exportData();
	echo "Data exported...<br>";
	
}
?>

</body>
</html>
