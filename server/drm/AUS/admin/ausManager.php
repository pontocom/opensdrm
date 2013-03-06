<?
session_start();
if($_SESSION['LOGIN']!=1) {
	header("Location: index.php");
}
include_once($_SERVER["DOCUMENT_ROOT"]."/config.inc.php");
include_once($_SERVER["DOCUMENT_ROOT"].$GLOBALS['INSTALLATION_DIR']."/AUS/admin/business/AUS_Administrator.class.php");
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>AUS Manager</title>
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
</head>
<body>
<p align="center"><img src="opensdrm-banner.jpg" /></p>
<p align="center">
<a href="ausManager.php?view=components" class="logout">Manage Components</a>
<a href="ausManager.php?view=users" class="logout">Manage Users</a>
<a href="ausManagerDo.php?what=logout" class="logout">Logout</a>
</p>

<?php 
if(!isset($_REQUEST['view']) || $_REQUEST['view']=='components')
{
?>
<?php
	$aus_admin = new AUS_Administrator();
	$components = $aus_admin->listComponents();
?>
<table border="1" align="center">
<tr>
<td class="head">
Component Identifier
</td>
<td class="head">
Type
</td>
<td class="head">
</td>
<td class="head">
</td>
</tr>
<?php 
	foreach($components as $component)
	{
?>		
<tr>
<td>
<?php echo $component[0]; ?>
</td>
<td>
<?php echo $component[1]; ?>
</td>
<td>
<a href="ausManagerDo.php?what=remove&cid=<?php echo $component[0]; ?>">Remove</a>
</td>
<td>
<a href="ausManager.php?view=components&detail=yes&cid=<?php echo $component[0]; ?>">View</a>
</td>
</tr>
<?php
	}
?>
</table>
<?php
}
?>
<?php
if($_REQUEST['view']=='users')
{
?>
<?php
	$aus_admin = new AUS_Administrator();
	$users = $aus_admin->listUsers();
?>
<table border="1" align="center">
<tr>
<td class="head">
Component Identifier
</td>
<td class="head">
Type
</td>
<td class="head">
Type
</td>
<td class="head">
Type
</td>
<td class="head">
Type
</td>
<td class="head">
</td>
<td class="head">
</td>
</tr>
<?php 
	foreach($users as $user)
	{
?>		
<tr>
<td>
<?php echo $user[0]; ?>
</td>
<td>
<?php echo $user[1]; ?>
</td>
<td>
<?php echo $user[2]; ?>
</td>
<td>
<?php echo $user[3]; ?>
</td>
<td>
<?php echo $user[4]; ?>
</td>
<td>
<a href="ausManager.php?view=remove&ctype=<?php echo $user[0]; ?>">Remove</a>
</td>
<td>
<a href="ausManager.php?view=remove&ctype=<?php echo $user[0]; ?>">View</a>
</td>
</tr>
<?php
	}
?>
</table>
<?php
}
?>
<?php
if($_REQUEST['view']=='components' && $_REQUEST['detail']=='yes')
{
	$aus_admin = new AUS_Administrator();
	$cdetails = $aus_admin->getDetailsData('C', $_REQUEST['cid']);
	//print_r($cdetails);
	$topics = array('Component Identifier', 'Public-Key', 'Password', 'Digital Certificate', 'Component Type');
?>
<table border="1" align="center">
<?php
$n=0;
foreach ($cdetails as $detail)
{
?>
<tr>
<td class="head"><?php echo $topics[$n];$n++; ?>
</td>
<td>
<?php echo $detail; ?>
</td>
</tr>
<?php	
}
?>
</table>
<?php
}
?>
<p align="center" class="footer"><?php echo $GLOBALS['VERSION'].$GLOBALS['FOOTER_NOTE'];?></p>
</body>
</html>
