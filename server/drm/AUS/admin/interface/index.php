<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Frameset//EN" "http://www.w3.org/TR/html4/frameset.dtd">
<html>
<head>
<title>[Administration - AUS Web Service]</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>

<frameset rows="80,*,27" cols="*" framespacing="0" frameborder="NO" border="0">
  <frame src="title.php" name="topFrame" scrolling="NO" noresize >
<?php
if(!isset($_REQUEST['op']) || $_REQUEST['op']=="OFF") {
?>
  <frameset cols="180,*" frameborder="NO" border="0" framespacing="0">
    <frame src="menu.php?op=OFF" name="menuFrame" scrolling="NO" noresize >
    <frame src="main.php?op=MAIN" name="mainFrame" scrolling="NO" noresize >
  </frameset>
<?php
} else if($_REQUEST['op']=="ON") {
?>
  <frameset cols="180,*" frameborder="NO" border="0" framespacing="0">
    <frame src="menu.php?op=ON" name="menuFrame" scrolling="NO" noresize >
    <frame src="main.php?op=IN_MAN" name="mainFrame" scrolling="AUTO" noresize >
  </frameset>
<?php
}
?>
  <frame src="footer.php" name="bottomFrame" scrolling="NO" noresize>
</frameset>
<noframes><body>

</body></noframes>
</html>
