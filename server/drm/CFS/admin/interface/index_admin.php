<?
session_start();
session_register('admin');
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>Commerce Server Administrator</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>

<body leftmargin="0" topmargin="0" marginwidth="0" marginheight="0">
<table width="100%" border="0" cellpadding="0">
  <tr> 
    <td bgcolor="#ff0000"><div align="right"><font color="#FFFFFF" size="4" face="Verdana, Arial, Helvetica, sans-serif">Commerce 
        Server Administrator<br>
        <font size="1">version xpto.0</font></font></div></td>
  </tr>
  <tr> 
    <td><div align="center">
<?
	if(isset($admin) && $admin=="ok") {
?>
    <font size="1" face="Verdana, Arial, Helvetica, sans-serif"><a href="<? echo $PHP_SELF."?type=iadmin";?>">internal 
        management</a></font></div>
<?
	} else {
?>
    <font size="1" face="Verdana, Arial, Helvetica, sans-serif">internal 
        management</font></div>
<?
	}
?>
        </td>
  </tr>
  <tr> 
    <td>
<?
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS_CONFIG['INSTALLATION_DIR']."/RGS/admin/business/COS_Administrator.class.php");
//include_once(getenv('OPENSDRM_INSTALL_DIR')."config.inc.php");

if(isset($admin) && $admin=="ok") {
	if($type=="iadmin") {
?>
<p><font size="2" face="Verdana, Arial, Helvetica, sans-serif"><strong>Internal Administration</strong></font></p>
<p><font size="1" face="Verdana, Arial, Helvetica, sans-serif"><a href="index_admin.php?type=iadmin&sub=view">view data</a> | <a href="index_admin.php?type=iadmin&sub=export">export data</a></font></p>
<?
		if($sub=="view") {
			$adm = new COS_Administrator();
			$adm->getData();
			$adm->printInternalData();
		}
		if($sub=="export") {
			$adm = new COS_Administrator();
			$adm->getData();
			$adm->exportData();
			echo "Data exported...<br>";
		}
	}
} else {
	$admn = new COS_Administrator();
	if($admn->verifyLogin($username, $password)==true) {
		$admin = "ok";
	} else {
		echo "ERROR: Incorrect pair: username, login.<br>";
?>
<form method="POST" action="index.php">
<INPUT type="submit" value="Please try again...">
</FORM>
<?
	}
}
?>
    </td>
  </tr>
  <tr> 
    <td bgcolor="#ff0000"><div align="right"><font color="#FFFFFF" size="1" face="Verdana, Arial, Helvetica, sans-serif">OpenSDRM 
        - Open Secure Digital Rights Management</font></div></td>
  </tr>
</table>
</body>
</html>
