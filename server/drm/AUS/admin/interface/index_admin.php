<?
session_start();
session_register('admin');
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>Authentication Server Administrator</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<link rel="stylesheet" href="aus_admin.css" type="text/css">
</head>

<body leftmargin="0" topmargin="0" marginwidth="0" marginheight="0">
<table width="100%" border="0" cellpadding="0">
  <tr> 
    <td bgcolor="#000000"><div align="right"><font color="#FFFFFF" size="4" face="Verdana, Arial, Helvetica, sans-serif">Authentication 
        Server Administrator<br>
        <font size="1">version xpto.0</font></font></div></td>
  </tr>
  <tr> 
    <td><div align="center">
<?
	if(isset($admin) && $admin=="ok") {
?>
    <font size="1" face="Verdana, Arial, Helvetica, sans-serif"><a href="<? echo $PHP_SELF."?type=iadmin";?>">internal 
        management</a> | <a href="<? echo $PHP_SELF."?type=uadmin";?>">user administration</a> | <a href="<? echo $PHP_SELF."?type=cadmin";?>">component administration</a></font></div>
<?
	} else {
?>
    <font size="1" face="Verdana, Arial, Helvetica, sans-serif">internal 
        management | user administration | component administration</font></div>
<?
	}
?>
        </td>
  </tr>
  <tr> 
    <td>
<?
include_once("../business/AUS_Administrator.class.php");

if(isset($admin) && $admin=="ok") {
	if($type=="iadmin") {
?>
<p><font size="2" face="Verdana, Arial, Helvetica, sans-serif"><strong>Internal Administration</strong></font></p>
<p><font size="1" face="Verdana, Arial, Helvetica, sans-serif"><a href="index_admin.php?type=iadmin&sub=view">view data</a> | <a href="index_admin.php?type=iadmin&sub=export">export data</a></font></p>
<?
		if($sub=="view") {
			$adm = new AUS_Administrator();
			$adm->getData();
			$adm->printInternalData();
		}
		if($sub=="export") {
			$adm = new AUS_Administrator();
			$adm->getData();
			$adm->exportData();
			echo "Data exported...<br>";
		}
	}
	if($type=="uadmin") {
?>
<p><font size="2" face="Verdana, Arial, Helvetica, sans-serif"><strong>User Administration</strong></font></p>
<?
		$adm = new AUS_Administrator();
		$adm->getData();
		$adm->listUsers();
	}
	if($type=="cadmin") {
?>
<p><font size="2" face="Verdana, Arial, Helvetica, sans-serif"><strong>Component Administration</strong></font></p>
<?
		$adm = new AUS_Administrator();
		$adm->getData();
		$adm->listComponents();
	}
} else {
	$admn = new AUS_Administrator();
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
    <td bgcolor="#000000"><div align="right"><font color="#FFFFFF" size="1" face="Verdana, Arial, Helvetica, sans-serif">OpenSDRM 
        - Open Secure Digital Rights Management</font></div></td>
  </tr>
</table>
</body>
</html>
