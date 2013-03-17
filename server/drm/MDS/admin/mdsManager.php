<?
session_start();
if($_SESSION['LOGIN']!=1) {
	header("Location: index.php");
}
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/MDS/admin/business/MDS_Administrator.class.php");
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>MDS Manager</title>
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
<p align="center"><a href="mdsManagerDo.php?what=logout" class="logout">Logout</a></p>

<?php
	$mds_admin = new MDS_Administrator();
	$cd = $mds_admin->listContentDelivery();

    $cl = $mds_admin->listContentLocation();
?>

<table border="1" align="center">
    <tr>
        <td class="head">
            ID
        </td>
        <td class="head">
            Content ID
        </td>
        <td class="head">
            User ID
        </td>
        <td class="head">
            Status
        </td>
        <td class="head">
        </td>
    </tr>
    <?php
    if($cd != -1){
        foreach($cd as $delivery)
        {
            ?>
            <tr>
                <td>
                    <?php echo $delivery[0]; ?>
                </td>
                <td>
                    <?php echo $delivery[1]; ?>
                </td>
                <td>
                    <?php echo $delivery[2]; ?>
                </td>
                <td>
                    <?php echo $delivery[3]; ?>
                </td>
                <td>
                    <a href="mdsManagerDo.php?what=remove&ctype=<?php echo $delivery[0]; ?>">Remove</a>
                </td>
            </tr>
        <?php
        }
    }
    ?>
</table>
<br><br>

<table border="1" align="center">
    <tr>
        <td class="head">
            ID
        </td>
        <td class="head">
            Content ID
        </td>
        <td class="head">
            Type
        </td>
        <td class="head">
            Protocol
        </td>
        <td class="head">
            Location
        </td>
        <td class="head">
        </td>
    </tr>
    <?php
    if($cl != -1){
        foreach($cl as $location)
        {
            ?>
            <tr>
                <td>
                    <?php echo $location[0]; ?>
                </td>
                <td>
                    <?php echo $location[1]; ?>
                </td>
                <td>
                    <?php echo $location[2]; ?>
                </td>
                <td>
                    <?php echo $location[3]; ?>
                </td>
                <td>
                    <?php echo $location[4]; ?>
                </td>
                <td>
                    <a href="mdsManagerDo.php?what=remove&ctype=<?php echo $location[0]; ?>">Remove</a>
                </td>
            </tr>
        <?php
        }
    }
    ?>
</table>
<br><br>


<p align="center" class="footer"><?php echo $GLOBALS['VERSION'].$GLOBALS['FOOTER_NOTE'];?></p>
</body>
</html>
