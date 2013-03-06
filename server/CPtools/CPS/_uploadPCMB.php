<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<title>Upload and Protect Content</title>
<style type="text/css">
<!--
.style2 {
	font-size: xx-large;
	font-weight: bold;
}
-->
</style>
</head>

<body>
<form action="uploadProtectedContentMB.php" method="post" enctype="multipart/form-data">
<table width="80%" border="1">
  <tr>
    <td colspan="2"><span class="style2">Upload and Protect content </span></td>
  </tr>
  <tr>
    <td width="24%">Music Name: </td>
    <td width="76%"><label>
      <input name="MN" type="text" id="MN" size="50">
    </label></td>
  </tr>
  <tr>
    <td>Album name: </td>
    <td><label>
      <input name="AN" type="text" id="AN" size="50">
    </label></td>
  </tr>
  <tr>
    <td>Artist name: </td>
    <td><label>
      <input name="ANM" type="text" id="ANM" size="50">
    </label></td>
  </tr>
  <tr>
    <td>Music Lenght: </td>
    <td><label>
      <select name="MLM" id="MLM">
<?
for($n=0; $n<=60; $n++) {
	echo '<option value='.sprintf("%02d",$n).'>'.sprintf("%02d",$n);
}
?>	  
        </select>
        :
        <select name="MLS" id="MLS">
<?
for($n=0; $n<=60; $n++) {
	echo '<option value='.sprintf("%02d",$n).'>'.sprintf("%02d",$n);
}
?>	  
        </select>
    </label></td>
  </tr>
  <tr>
    <td>Filename:</td>
    <td><label>
      <input name="filename" type="file" id="filename" size="40">
    </label></td>
  </tr>
  <tr>
    <td>&nbsp;</td>
    <td><label>
      <input type="submit" name="Submit" value="Submit file">
    </label></td>
  </tr>
</table>
</form>
</body>
</html>
