<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>--:: OpenSDRM DRM components List ::--</title>
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

  <table border="0" align="center">
    <tr>
      <td colspan="2" class="head">OpenSDRM DRM Components List</td>
    </tr>
    <tr>
      <td class="required"><a href="AUS/admin/">Authentication Server</a> </td>
      <td><a href="AUS/AUS.ws.php">WSDL</a></td>
    </tr>
    <tr>
      <td class="required"><a href="CFS/admin/">Configuration Server</a> </td>
      <td><a href="CFS/CFS.ws.php">WSDL</a></td>
    </tr>
    <tr>
      <td class="required"><a href="COS/admin/">Commerce Server</a> </td>
      <td><a href="COS/COS.ws.php">WSDL</a></td>
    </tr>
    <tr>
      <td class="required"><a href="ITS/admin/">Intellectual Property Protection Tools Server</a> </td>
      <td><a href="ITS/ITS.ws.php">WSDL</a></td>
    </tr>
    <tr>
      <td class="required"><a href="LIS/admin/">License Server</a> </td>
      <td><a href="LIS/LIS.ws.php">WSDL</a></td>
    </tr>
    <tr>
      <td class="required"><a href="MDS/admin/">Media Delivery Server</a> </td>
      <td><a href="MDS/MDS.ws.php">WSDL</a></td>
    </tr>
    <tr>
      <td class="required"><a href="PGW/admin/">Payment Gateway Server</a> </td>
      <td><a href="PGW/PGW.ws.php">WSDL</a></td>
    </tr>
    <tr>
      <td class="required"><a href="RGS/admin/">Registration Server</a> </td>
      <td><a href="RGS/RGS.ws.php">WSDL</a></td>
    </tr>    
  </table>

<p align="center" class="footer">OpenSDRM &copy; Adetti 2007 - Produced by ADETTI (Carlos Serrao)</p>
</body>
</html>
