<?
session_start();
if($_SESSION['LOGIN']!=1) {
	header("Location: index.php");
}
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/RGS/admin/business/RGS_Administrator.class.php");
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>Registration Server Manager</title>
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
<p align="center"><a href="rgsManagerDo.php?what=logout" class="logout">Logout</a></p>

<?php
	$rgs_admin = new RGS_Administrator();
	$c = $rgs_admin->listAllContent();
?>
<table border="1" align="center">
<tr>
<td class="head">
Component Identifier
</td>
<td class="head">
Internal ID
</td>
    <td class="head">
        Hash
    </td>
    <td class="head">
        File
    </td>
    <td class="head">
        Metadata
    </td>
    <td class="head">
</td>
</tr>
    <?php
    if($c != -1){
        foreach($c as $content)
        {
            ?>
            <tr>
                <td>
                    <?php echo $content[0]; ?>
                </td>
                <td>
                    <?php echo $content[1]; ?>
                </td>
                <td>
                    <?php echo $content[2]; ?>
                </td>
                <td>
                    <?php echo $content[3]; ?>
                </td>
                <td>
                    <?php echo $content[4]; ?>
                </td>
                <td>
                    <a href="rsManagerDo.php?what=remove&ctype=<?php echo $content[0]; ?>">Remove</a>
                </td>
            </tr>
        <?php
        }
    }
    ?>

</table>
<p align="center" class="footer"><?php echo $GLOBALS['VERSION'].$GLOBALS['FOOTER_NOTE'];?></p>
</body>
</html>
