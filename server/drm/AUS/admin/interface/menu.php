<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>Untitled Document</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<link rel="stylesheet" href="aus_admin.css" type="text/css">
</head>

<body bgcolor="#666666">
<font size="2" face="Verdana, Arial, Helvetica, sans-serif"> <strong>Main Menu</strong><br>
<?php
if($_REQUEST['op']=="OFF") {
?>
<br>
<font size="1">[<strong>Internal Management</strong>]<br>
<br>
[<strong>User Administration</strong>]<br>
<br>
[<strong>Components Administration</strong>]</font></font>
<?php
} else if ($_REQUEST['op']=="ON") {
?>
<br>
<font size="1">[<strong><a href="main.php?op=IN_MAN" target="mainFrame" class="menu">Internal Management</a></strong>]<br>
<br>
[<strong><a href="main.php?op=U_ADM" target="mainFrame" class="menu">User Administration</a></strong>]<br>
<br>
[<a href="main.php?op=C_ADM" target="mainFrame" class="menu"><strong>Components Administration</strong></a>]</font></font>
<?php
}
?> 
</body>
</html>
