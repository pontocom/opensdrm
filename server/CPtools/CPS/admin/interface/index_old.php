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
    <td><div align="center"><font size="1" face="Verdana, Arial, Helvetica, sans-serif">internal 
        management</font></div></td>
  </tr>
  <tr> 
    <td>
<?
include_once("COS_Administrator.class.php");


	if(isset($type)) {
		if($type=="setup" && !isset($go_s)) {
			require("setup_administrator.php");
		} else {
			if($type=="setup" && $go_s="go") {
				$admin = new COS_Administrator();
				$result = $admin->createNewAdminAccount($username, $password, $rsa_key_size);
				if ($result==false) {
					echo $admin->returnLastError();
				} else {
					echo "Public Key value :<br>";
					echo "n = ".$admin->pubkey->getN()."<br>";
					echo "e = ".$admin->pubkey->getE()."<br>";
?>
<form action="<? echo $PHP_SELF;?>" method=post>
<input type=submit value="Proceed to login >>">
</form>
<?
				}
			}
		}
	} else {
		$admin = new COS_Administrator();
		$astatus=$admin->verifyAdminStatus();
		if($astatus==false) {
?>
  <br>
You have not yet created an account.<br>
<?
echo getenv('OPENSDRM_INSTALL_DIR');
?>
<form action="<?echo $PHP_SELF; ?>" method="POST">
<INPUT type="hidden" name="type" value="setup">
<input type=submit value="Create an Account >>">
</form>
<?		
		} else {
			require("authentication_dlg.php");
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
