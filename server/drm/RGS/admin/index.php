<?
session_start();
if(isset($_SESSION['LOGIN']) && $_SESSION['LOGIN']==1) {
	header("Location: rgsManager.php");
}
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/RGS/admin/business/RGS_Administrator.class.php");
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>RGS Manager</title>
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
p.footer {
	font-family: Verdana, Arial, Helvetica, sans-serif;
	font-size: 9px;
	color: #666666;
}
-->
</style>
<script language="javascript">
function validateFormAuthentication()
{
	if(document.authform.username.value=="")
	{
		alert("Forget to enter the username");
	} else{
		if(document.authform.username.password=="")
		{
			alert("Forget to enter the password");
		} else{
			document.authform.submit();
		}
	}
}

function validateFormRegistration()
{
	if(document.registform.username.value=="")
	{
		alert("Forget to enter the username");
	} else{
		if(document.registform.username.password=="")
		{
			alert("Forget to enter the password");
		} else{
			if(document.registform.username.repassword=="")
			{
				alert("Forget to re-type the password");
			} else {
				if(document.registform.username.password!=document.registform.username.repassword) {
					alert("The two passwords are not equal.");
				}else {
					document.registform.submit();
				}
			}
		}
	}
}
</script>
</head>
<body>
<p align="center"><img src="opensdrm-banner.jpg" /></p>
<?php
	$rgs_admin = new RGS_Administrator();
	if($rgs_admin->verifyAdminStatus())
	{
?>
<!-- Part of the login form -->
<form action="processValidation.php" method="post" name="authform">
  <table border="0" align="center">
    <tr>
      <td colspan="2" class="head">Administrator authentication</td>
    </tr>
    <tr>
      <td class="required"> Username: </td>
      <td><input type="text" name="username" id="username"></td>
    </tr>
    <tr>
      <td class="required"> Password: </td>
      <td><input type="password" name="password" id="password"><input type="hidden" name="op" value="LOGIN"></td>
    </tr>
    <tr>
      <td>  </td>
      <td><input type="button" value="Authenticate" onclick="javascript:validateFormAuthentication()"></td>
    </tr>
  </table>
</form>
<?php
	} else 
	{
?>
<!-- Part of the registration form -->
<form action="processValidation.php" method="post" name="registform">
  <table border="0" align="center">
    <tr>
      <td colspan="2" class="head">Administrator registration</td>
    </tr>
    <tr>
      <td class="required"> Username: </td>
      <td><input type="text" name="username" id="username"></td>
    </tr>
    <tr>
      <td> Email: </td>
      <td><input type="text" name="email" id="email"></td>
    </tr>
    <tr>
      <td class="required"> Password: </td>
      <td><input type="password" name="password" id="password"></td>
    </tr>
    <tr>
      <td class="required"> Re-type password: </td>
      <td><input type="password" name="repassword" id="repassword"><input type="hidden" name="op" value="REGISTRATION"></td>
    </tr>
    <tr>
      <td></td>
      <td><input type="button" value="Register new" onclick="javascript:validateFormRegistration()">
      </td>
    </tr>
  </table>
</form>
<?php
	}
?>
<p align="center" class="footer"><?php echo $GLOBALS['VERSION'].$GLOBALS['FOOTER_NOTE'];?></p>
</body>
</html>
