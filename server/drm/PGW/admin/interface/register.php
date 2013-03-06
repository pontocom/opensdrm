<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
 <html>
<head>
<title>Untitled Document</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<link rel="stylesheet" href="cos_admin.css" type="text/css">
</head>

<body bgcolor="#FFFFC0">
<p><font size="2" face="Verdana, Arial, Helvetica, sans-serif"><strong>Setup Administrator 
  Data</strong></font></p>
<?php
if(!isset($_REQUEST['govar']) || $_REQUEST['govar']!="GO") {
?>
<form name="form1" method="post" action="<? echo $PHP_SELF;?>">
  <font size="2" face="Verdana, Arial, Helvetica, sans-serif">Username: 
  <input name="username" type="text" id="username" size="20" maxlength="20">
  (*)<br>
  Password: 
  <input name="password" type="password" id="password" size="10" maxlength="10">
  (*) Please Retype Password: 
  <input name="password2" type="password" id="password2" size="10" maxlength="10">
  (*)<br>
  <font color="#000000"> RSA Key size 
  <select name="rsa_key_size" size="1" id="rsa_key_size">
    <option value="128">128</option>
    <option value="256">256</option>
    <option value="512">512</option>
    <option value="1024" selected>1024</option>
    <option value="2048">2048</option>
  </select>
  (The biggest the keysize, the longest the time to be generated) <br>
  <br>
  <br>
  (Please be patient, this may take several minutes) <br>
  <input type="hidden" name="govar" value="GO">
  <input type="submit" name="Submit" value="Create Data">
  </font></font> 
</form>
<?php
} else if ($_REQUEST['govar']=="GO") {
	include_once("../business/PGW_Administrator.class.php");

	$admin = new PGW_Administrator();
	$result = $admin->createNewAdminAccount($_REQUEST['username'], $_REQUEST['password'], $_REQUEST['rsa_key_size']);
	if ($result==false) {
		echo "An error as occured...<br>";
		echo $admin->returnLastError();
	} else {
		echo "Public Key value :<br>";
		echo "n = ".$admin->pubkey->getN()."<br>";
		echo "e = ".$admin->pubkey->getE()."<br><br>";
		echo "You can perform your login [<a href=\"main.php\">NOW</a>].";
	}	
}?>
<p>&nbsp; </p>
</body>
</html>
