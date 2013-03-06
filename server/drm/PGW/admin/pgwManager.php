<?
session_start();
if($_SESSION['LOGIN']!=1) {
	header("Location: index.php");
}
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/PGW/admin/business/PGW_Administrator.class.php");
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>Payment Gateway Manager</title>
<style type="text/css">
<!--
td {
	font-family: Verdana, Arial, Helvetica, sans-serif;
	font-size: 16px;
	font-weight: normal;
}
td.required {
	background-color: #D09C9A;
}
td.head {
	font-family: Verdana, Arial, Helvetica, sans-serif;
	font-size: 18px;
	font-weight: bolder;
	color: #FFFFFF;
	background-color: #000066;
}
input {
	font-family: Verdana, Arial, Helvetica, sans-serif;
	font-size: 16px;
}
p {
	font-family: Verdana, Arial, Helvetica, sans-serif;
	font-size: 18px;
}
a.logout {
	font-family: Verdana, Arial, Helvetica, sans-serif;
	font-size: 18px;
	color: #FFFFFF;
	background-color: #FF0000;
	height: auto;
	width: auto;
	margin: 10px;
	padding: 10px;
}
p.footer {
	font-family: Verdana, Arial, Helvetica, sans-serif;
	font-size: 9px;
	color: #666666;
}
-->
</style>
<script language="javascript">
function validateFormAdd()
{
	if(document.ncform.ctype.value=="")
	{
		alert("Forget to enter the component type.");
	} else{
		if(document.ncform.location.password=="")
		{
			alert("Forget to enter the component location (URL)");
		} else{
			document.ncform.submit();
		}
	}
}
</script>
</head>
<body>
<p align="center"><img src="opensdrm-banner.jpg" /></p>
<p align="center"><a href="pgwManagerDo.php?what=logout" class="logout">Logout</a></p>

<?php
	$pgw_admin = new PGW_Administrator();
	$servers = $pgw_admin->listServers_();
?>
<table border="1" align="center">
<tr>
<td class="head">
Component Identifier
</td>
<td class="head">
Location
</td>
<td class="head">
</td>
</tr>
<?php 
	for($n=0; $n<count($servers); $n=$n+2)
	{
?>		
<tr>
<td>
<?php echo $servers[$n]; ?>
</td>
<td>
<?php echo $servers[$n+1]; ?>
</td>
<td>
<a href="pgwManagerDo.php?what=remove&ctype=<?php echo $servers[$n]; ?>">Remove</a>
</td>
</tr>
<?php
	}
?>
</table>
<!-- Part of the login form -->
<form action="pgwManagerDo.php" method="POST" name="ncform">
  <table border="0" align="center">
    <tr>
      <td colspan="2" class="head">Add new server</td>
    </tr>
    <tr>
      <td class="required"> Component Type: </td>
      <td><input type="text" name="ctype" id="ctype"><input type="hidden" name="what" value="addnew"></td>
    </tr>
    <tr>
      <td class="required"> Location (URL): </td>
      <td><input type="text" name="location" id="location"></td>
    </tr>
    <tr>
      <td>  </td>
      <td><input type="button" value="Add" onclick="javascript:validateFormAdd()"></td>
    </tr>
  </table>
</form>
<p align="center" class="footer"><?php echo $GLOBALS['VERSION'].$GLOBALS['FOOTER_NOTE'];?></p>
</body>
</html>
