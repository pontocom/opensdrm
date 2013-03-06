<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>Untitled Document</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"><br>
<link rel="stylesheet" href="cos_admin.css" type="text/css"></head>
<body bgcolor="#FFC3B7">
<?php
if(!isset($_REQUEST['op']) || $_REQUEST['op']=="MAIN") {
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Administrator Login</font></strong><br>";
	if(!isset($_REQUEST['login_status']) || $_REQUEST['login_status']!="ON") {
		include_once("../business/COS_Administrator.class.php");
		$admin = new COS_Administrator();
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
		include_once("../business/COS_Administrator.class.php");
		$admn = new COS_Administrator();
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
	include_once("../business/COS_Administrator.class.php");
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Internal Administration</font></strong><br>";
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">View data</font></strong><br>";
	$adm = new COS_Administrator();
	$adm->getData();
	$adm->printInternalData();

} else if ($_REQUEST['op']=="U_ADM_EXPORT") {
	include_once("../business/COS_Administrator.class.php");
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Internal Administration</font></strong><br>";
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Export data</font></strong><br>";
	$adm = new COS_Administrator();
	$adm->getData();
	$adm->exportData();
	echo "Data exported...<br>";

} else if ($_REQUEST['op']=="COS_SUB") {
	include_once("../business/COS_Administrator.class.php");
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Payment Gateway Subscription</font></strong><br>";
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Payment Gateway</font></strong><br>";
	echo "<br><br>Please select one of the available Payment Gateways:<br>";
	$adm = new COS_Administrator();
	$list_pgw = $adm->getListPGW();
	if($list_pgw==false) {
		echo "There are no available Payment Gateways at the moment.<br>";
	} else {
		echo "<form method=post action=main.php>";
		echo "<input type=hidden name=op value='COS_SUB2'>";
		echo "Payment Gateways: ";
		echo "<select name=ip_add>";
		// DEAL WITH THE NUMBER OF PGWS
		$xmlpgw = xmlize($list_pgw);
		$number = $xmlpgw["list_pgw"]["#"]["count"][0]["#"];
		for($n=0; $n<$number; $n++) {
			echo "<option value=\"<pgw><ip>".$xmlpgw["list_pgw"]["#"]["pgw"][$n]["#"]["ip_data"][0]["#"]["ip"][0]["#"]."</ip><id>".$xmlpgw["list_pgw"]["#"]["pgw"][$n]["#"]["id"][0]["#"]."</id></pgw>\">".$xmlpgw["list_pgw"]["#"]["pgw"][$n]["#"]["ip_data"][0]["#"]["ip"][0]["#"];
		}
		//echo "<option>".htmlentities($list_pgw);
		echo "</select>";
		echo "<input type=submit value='Subscribe this PGW >>'>";
		echo "</form>";
	}
} else if ($_REQUEST['op']=="COS_SUB2") {
	include_once("../business/COS_Administrator.class.php");
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Payment Gateway Subscription</font></strong><br>";
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Payment Gateway</font></strong><br>";
	echo "<br><br>";
	//echo "REQUEST = ".htmlentities($_REQUEST['ip_add']);
	$xmlpgw = xmlize($_REQUEST['ip_add']);
	$ip_address = $xmlpgw["pgw"]["#"]["ip"][0]["#"];
	$id = $xmlpgw["pgw"]["#"]["id"][0]["#"];
	//echo "IP=".$ip_address."; ID=".$id;
	$adm = new COS_Administrator();
	if($adm->subscribePGW($ip_address, $id)==true) {
		echo "COS subscribed with success at PGW.<br><br>";
	} else {
		echo "Some error as occured in the subscription at PGW.<br><br>";
	}
} else if ($_REQUEST['op']=="COS_RPCL") { ///////////////DEMO///////////////
	include_once("../business/COS_Administrator.class.php");
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Payment Gateway Request Payment Clearence</font></strong><br>";
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">DEMO</font></strong><br>";
	echo "<br><br>";
	echo "Requesting payment clearence...";
	flush();
	$adm = new COS_Administrator();
	$res = "";
	if($adm->requestPGWPaymentClereance("ad008875e058e023a178739b91e381c0","<order><nitems>1</nitems><item_desc>Madonna Music</item_desc><item_price>100 euros</item_price><total>100 euros</total></order>", $res)==true) {
		print_r($res);
		echo "PAYMENT CLEARENCE OK.";
	} else {
		print_r($res);
		echo "PAYMENT CLEARENCE KO.";
	}
} else if ($_REQUEST['op']=="COS_RPCA") { ///////////////DEMO///////////////
	include_once("../business/COS_Administrator.class.php");
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Payment Gateway Request Payment Capture</font></strong><br>";
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">DEMO</font></strong><br>";
	echo "<br><br>";
	echo "<form action=main.php method=post>";
	echo "Transaction ID: <input type=text name=tid>";
	echo "<input type=hidden name=op value=\"COS_RPCA_GO\">";
	echo "<input type=submit value=\"Process Transaction\">";
	echo "</form>";
} else if ($_REQUEST['op']=="COS_RPCA_GO") { ///////////////DEMO///////////////
	include_once("../business/COS_Administrator.class.php");
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">Payment Gateway Request Payment Capture</font></strong><br>";
	echo "<strong><font size=\"3\" face=\"Verdana, Arial, Helvetica, sans-serif\">DEMO</font></strong><br>";
	echo "<br><br>";
	$res = "";
	$adm = new COS_Administrator();
	if($adm->requestPGWPaymentCapture($_REQUEST['tid'], $res)==true) {
		print_r($res);
		echo "PAYMENT CAPTURE OK.";
	} else {
		print_r($res);
		echo "PAYMENT CAPTURE KO.";
	}
}
?>

</body>
</html>
