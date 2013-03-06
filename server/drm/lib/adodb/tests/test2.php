<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<html>
<head>
	<title>Untitled</title>
</head>

<body>
<?php
/*
  V3.00 6 Jan 2003  (c) 2000-2003 John Lim (jlim@natsoft.com.my). All rights reserved.
  Released under both BSD license and Lesser GPL library license. 
  Whenever there is any discrepancy between the two licenses, 
  the BSD license will take precedence.
  Set tabs to 8.
 */
#
# test connecting to 2 MySQL databases simultaneously and ensure that each connection
# is independant.
#
include("../tohtml.inc.php");
include("../adodb.inc.php");	

ADOLoadCode('mysql');

$c1 = ADONewConnection('oci8');

if (!$c1->PConnect('','scott','tiger')) 
	die("Cannot connect to server");
$c1->debug=1;

$rs = $c1->CacheExecute(1200,'select rownum, p1.firstname,p2.lastname,p2.firstname,p1.lastname from adoxyz p1, adoxyz p2');
print "Records=".$rs->RecordCount()."<br>";
rs2html($rs);
?>


</body>
</html>
