<?
session_start();
if($_SESSION['LOGIN']!=1) {
	header("Location: index.php");
}
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/COS/admin/business/COS_Administrator.class.php");
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>COS Manager</title>
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
<p align="center"><a href="cosManagerDo.php?what=subscribePGW" class="logout">Subscribe Payment Gateway</a>
<a href="cosManagerDo.php?what=logout" class="logout">Logout</a></p>

<?php

?>

<!-- Part of the login form -->
<!-- <form action="cosManagerDo.php" method="POST" name="ncform">
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
-->
<p align="center" class="footer"><?php echo $GLOBALS['VERSION'].$GLOBALS['FOOTER_NOTE'];?></p>
</body>
</html>
<?php
/* } else if ($_REQUEST['op']=="COS_SUB") {
include_once("../business/COS_Administrator.class.php"); echo "
<strong><font size=\ "3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Payment
Gateway Subscription</font></strong>
<br>"; echo "
<strong><font size=\ "3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Payment
Gateway</font></strong>
<br>"; echo "
<br>
<br>Please select one of the available Payment Gateways:
<br>"; $adm = new COS_Administrator(); $list_pgw = $adm->getListPGW();
if($list_pgw==false) { echo "There are no available Payment Gateways at
the moment.
<br>"; } else { echo "
<form method=post action=main.php>"; echo "<input type=hidden name=op
	value='COS_SUB2'>"; echo "Payment Gateways: "; echo "<select
	name=ip_add>
	"; // DEAL WITH THE NUMBER OF PGWS $xmlpgw = xmlize($list_pgw); $number
	= $xmlpgw["list_pgw"]["#"]["count"][0]["#"]; for($n=0; $n<$number;
	$n++) { echo "
	<option value=\"<pgw><ip>".$xmlpgw["list_pgw"]["#"]["pgw"][$n]["#"]["ip_data"][0]["#"]["ip"][0]["#"]."</ip><id>".$xmlpgw["list_pgw"]["#"]["pgw"][$n]["#"]["id"][0]["#"]."</id></pgw>\">".$xmlpgw["list_pgw"]["#"]["pgw"][$n]["#"]["ip_data"][0]["#"]["ip"][0]["#"];
	} //echo "
	
	
	<option>".htmlentities($list_pgw); echo "

</select>"; echo "<input type=submit value='Subscribe this PGW >>'>";
echo "</form>
"; } } else if ($_REQUEST['op']=="COS_SUB2") {
include_once("../business/COS_Administrator.class.php"); echo "
<strong><font size=\ "3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Payment
Gateway Subscription</font></strong>
<br>"; echo "
<strong><font size=\ "3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Payment
Gateway</font></strong>
<br>"; echo "
<br>
<br>"; //echo "REQUEST = ".htmlentities($_REQUEST['ip_add']); $xmlpgw =
xmlize($_REQUEST['ip_add']); $ip_address =
$xmlpgw["pgw"]["#"]["ip"][0]["#"]; $id =
$xmlpgw["pgw"]["#"]["id"][0]["#"]; //echo "IP=".$ip_address."; ID=".$id;
$adm = new COS_Administrator(); if($adm->subscribePGW($ip_address,
$id)==true) { echo "COS subscribed with success at PGW.
<br>
<br>"; } else { echo "Some error as occured in the subscription at PGW.
<br>
<br>"; } } */ 
?>