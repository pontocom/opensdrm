<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>Untitled Document</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"><br>
<link rel="stylesheet" href="aus_admin.css" type="text/css"></head>
<body bgcolor="#CCCCCC"> 
<?php
if(!isset($_REQUEST['op']) || $_REQUEST['op']=="MAIN") {
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Administrator Login</font></strong><br>";
	if(!isset($_REQUEST['login_status']) || $_REQUEST['login_status']!="ON") {
		include_once("../business/AUS_Administrator.class.php");
		$admin = new AUS_Administrator();
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
		include_once("../business/AUS_Administrator.class.php");
		$admn = new AUS_Administrator();
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
} else if ($_REQUEST['op']=="U_ADM") {
	include_once($_SERVER['DOCUMENT_ROOT']."/config.inc.php");
	include_once("../business/AUS_Administrator.class.php");
	include_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS["LIB_DIR"]."/common/tableDrawer.class.php");
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">User Administration</font></strong><br>";
	$adm = new AUS_Administrator();
	$adm->getData();
	$tdata=$adm->listUsers();
	
	echo "<form action=\"$PHP_SELF\">";
	echo "<input type=hidden name=op value=\"APG_U\">";
	echo "<input type=hidden name=c value=".count($tdata).">";
	
	$td = new tableDrawer();
	$td->border=1;
	$td->h_c_b_color=black;
	$td->h_c_t_color=white;
	$title = Array("Identificação",	"Username",	"Name", "Address", "Email");
	$td->draw($title, $tdata, "main.php?op=DET_U&");
	
	echo "<br><input type=submit value=Delete>";
	echo "</form>";
	
} else if ($_REQUEST['op']=="C_ADM") {
	include_once($_SERVER['DOCUMENT_ROOT']."/config.inc.php");
	include_once("../business/AUS_Administrator.class.php");
	include_once($_SERVER['DOCUMENT_ROOT'].$GLOBALS["LIB_DIR"]."/common/tableDrawer.class.php");
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Component Administration</font></strong><br>";
	$adm = new AUS_Administrator();
	$adm->getData();
	$tdata=$adm->listComponents();

	echo "<form action=\"$PHP_SELF\">";
	echo "<input type=hidden name=op value=\"APG_C\">";
	echo "<input type=hidden name=c value=".count($tdata).">";
	
	$td = new tableDrawer();
	$td->border=1;
	$td->h_c_b_color=black;
	$td->h_c_t_color=white;
	$title = Array("Identificação",	"Other data");
	$td->draw($title, $tdata, "main.php?op=DET_C&");
	
	echo "<br><input type=submit value=Delete>";
	echo "</form>";

} else if ($_REQUEST['op']=="U_ADM_VIEW") {
	include_once("../business/AUS_Administrator.class.php");
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Internal Administration</font></strong><br>";
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">View data</font></strong><br>";
	$adm = new AUS_Administrator();
	$adm->getData();
	$adm->printInternalData();
	
} else if ($_REQUEST['op']=="U_ADM_EXPORT") {
	include_once("../business/AUS_Administrator.class.php");
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Internal Administration</font></strong><br>";
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Export data</font></strong><br>";
	$adm = new AUS_Administrator();
	$adm->getData();
	$adm->exportData();
	echo "Data exported...<br>";
	
} else if ($_REQUEST['op']=="APG_C") {
	include_once("../business/AUS_Administrator.class.php");

	$tdelete = Array();
	for($n=0; $n<$_REQUEST['c']; $n++) {
		if($_REQUEST['opd'][$n]==true) {
			array_push($tdelete, $_REQUEST['opd'][$n]);
		}
	}
	
	$adm = new AUS_Administrator();
	$adm->getData();
	$adm->deleteComponents($tdelete);	
	
	echo "<SCRIPT>document.location='main.php?op=C_ADM';</SCRIPT>";

} else if ($_REQUEST['op']=="APG_U") {
	include_once("../business/AUS_Administrator.class.php");

	$tdelete = Array();
	for($n=0; $n<$_REQUEST['c']; $n++) {
		if($_REQUEST['opd'][$n]==true) {
			array_push($tdelete, $_REQUEST['opd'][$n]);
		}
	}
	
	$adm = new AUS_Administrator();
	$adm->getData();
	$adm->deleteUsers($tdelete);	
	
	echo "<SCRIPT>document.location='main.php?op=U_ADM';</SCRIPT>";
} else if ($_REQUEST['op']=="DET_U") {
	include_once("../business/AUS_Administrator.class.php");
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">User Administration</font></strong><br>";
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">User details</font></strong><br>";

	$adm = new AUS_Administrator();
	$result = $adm->getDetailsData("U", $_REQUEST['id']);

	echo "
	<table>
		<tr><td>Identification</td><td><input type=text size=60 disabled value=$result[0]></td></tr>
		<tr><td>Login</td><td><input type=text size=60 disabled value=$result[1]></td></tr>
		<tr><td>Public Key</td><td><textarea cols=120 rows=6 disabled>".htmlentities($result[3])."</textarea></td></tr>
		<tr><td>Name</td><td><input type=text size=60 disabled value=$result[4]></td></tr>
		<tr><td>Address</td><td><textarea cols=120 rows=6 disabled>$result[5]</textarea></td></tr>
		<tr><td>Email</td><td><input type=text size=60 disabled value=$result[6]></td></tr>
		<tr><td>Authentication</td><td><input type=text size=60 disabled value=$result[7]></td></tr>
		<tr><td>Other data</td><td><textarea cols=120 rows=6 disabled>$result[8]</textarea></td></tr>
		<tr><td>Certificate</td><td><textarea cols=120 rows=6 disabled>".htmlentities($result[9])."</textarea></td></tr>
		<tr><td>Watermark</td><td><input type=text size=60 disabled value=$result[10]></td></tr>
	</table>
	";

} else if ($_REQUEST['op']=="DET_C") {
	include_once("../business/AUS_Administrator.class.php");
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Component Administration</font></strong><br>";
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Component details</font></strong><br>";

	$adm = new AUS_Administrator();
	$result = $adm->getDetailsData("C", $_REQUEST['id']);

	echo "
	<table>
		<tr><td>Identification</td><td><input type=text size=60 disabled value=".$result[0]."></td></tr>
		<tr><td>Public Key</td><td><textarea cols=120 rows=6 disabled>".htmlentities($result[1])."</textarea></td></tr>
		<tr><td>Certificate</td><td><textarea cols=120 rows=20 disabled>".htmlentities($result[3])."</textarea></td></tr>
		<tr><td>Other data</td><td><textarea rows=4 cols=60 disabled>".htmlentities($result[4])."</textarea></td></tr>
	</table>
	";

}
?>

</body>
</html>
