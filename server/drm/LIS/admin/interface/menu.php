<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>Untitled Document</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<link rel="stylesheet" href="rgs_admin.css" type="text/css">
<style type="text/css">
<!--
body {
	background-color: #663399;
}
-->
</style></head>

<body>
<font size="2" face="Verdana, Arial, Helvetica, sans-serif"> <strong>Main Menu</strong><br>
<?php
if($_REQUEST['op']=="OFF") {
?>
<br>
<font size="1">[<strong>Internal Management</strong>]<br>
</font>
<?php
} else if ($_REQUEST['op']=="ON") {
?>
<br>
<font size="1">[<strong><a href="main.php?op=IN_MAN" target="mainFrame" class="menu">Internal Management</a></strong>]<br>
</font>
<?php
}
?> 
</body>
</html>
