<?php
/* 
V3.00 6 Jan 2003  (c) 2000-2003 John Lim (jlim@natsoft.com.my). All rights reserved.
  Released under both BSD license and Lesser GPL library license. 
  Whenever there is any discrepancy between the two licenses, 
  the BSD license will take precedence. 
Set tabs to 4 for best viewing.
  
  Latest version is available at http://php.weblogs.com/
  
  MSSQL support via ODBC. Requires ODBC. Works on Windows and Unix. 
  For Unix configuration, see http://phpbuilder.com/columns/alberto20000919.php3
*/

if (!defined('_ADODB_ODBC_LAYER')) {
	include(ADODB_DIR."/drivers/adodb-odbc.inc.php");
}

 
class  ADODB_odbc_mssql extends ADODB_odbc {	
	var $databaseType = 'odbc_mssql';
	var $fmtDate = "'Y-m-d'";
	var $fmtTimeStamp = "'Y-m-d h:i:sA'";
	var $_bindInputArray = true;
	var $metaTablesSQL="select name from sysobjects where type='U' or type='V' and (name not in ('sysallocations','syscolumns','syscomments','sysdepends','sysfilegroups','sysfiles','sysfiles1','sysforeignkeys','sysfulltextcatalogs','sysindexes','sysindexkeys','sysmembers','sysobjects','syspermissions','sysprotects','sysreferences','systypes','sysusers','sysalternates','sysconstraints','syssegments','REFERENTIAL_CONSTRAINTS','CHECK_CONSTRAINTS','CONSTRAINT_TABLE_USAGE','CONSTRAINT_COLUMN_USAGE','VIEWS','VIEW_TABLE_USAGE','VIEW_COLUMN_USAGE','SCHEMATA','TABLES','TABLE_CONSTRAINTS','TABLE_PRIVILEGES','COLUMNS','COLUMN_DOMAIN_USAGE','COLUMN_PRIVILEGES','DOMAINS','DOMAIN_CONSTRAINTS','KEY_COLUMN_USAGE'))";
	var $metaColumnsSQL = "select c.name,t.name,c.length from syscolumns c join systypes t on t.xusertype=c.xusertype join sysobjects o on o.id=c.id where o.name='%s'";
	var $hasTop = 'top';		// support mssql/interbase SELECT TOP 10 * FROM TABLE
	var $sysDate = 'GetDate()';
	var $sysTimeStamp = 'GetDate()';
	var $leftOuter = '*=';
	var $rightOuter = '=*';
	var $ansiOuter = true; // for mssql7 or later
	
	function ADODB_odbc_mssql()
	{
		$this->ADODB_odbc();
	}

	function xServerInfo()
	{
		$row = $this->GetRow("execute sp_server_info 2");
		$arr['description'] = $row[2];
		$arr['version'] = ADOConnection::_findvers($arr['description']);
		return $arr;
	}
	
	
	function MetaTables()
	{
		return ADOConnection::MetaTables();
	}
	
	function MetaColumns($table)
	{
		return ADOConnection::MetaColumns($table);
	}
	
	// Format date column in sql string given an input format that understands Y M D
	function SQLDate($fmt, $col=false)
	{	
		if (!$col) $col = $this->sysDate;
		$s = '';
		
		$len = strlen($fmt);
		for ($i=0; $i < $len; $i++) {
			if ($s) $s .= '+';
			$ch = $fmt[$i];
			switch($ch) {
			case 'Y':
			case 'y':
				$s .= "datename(yyyy,$col)";
				break;
			case 'M':
			case 'm':
				$s .= "replace(str(month($col),2),' ','0')";
				break;
			
			case 'Q':
			case 'q':
				$s .= "datename(quarter,$col)";
				break;
				
			case 'D':
			case 'd':
				$s .= "replace(str(day($col),2),' ','0')";
				break;
			default:
				if ($ch == '\\') {
					$i++;
					$ch = substr($fmt,$i,1);
				}
				$s .= $this->qstr($ch);
				break;
			}
		}
		return $s;
	}
} 
 
class  ADORecordSet_odbc_mssql extends ADORecordSet_odbc {	
	
	var $databaseType = 'odbc_mssql';
	
	function ADORecordSet_odbc_mssql($id,$mode=false)
	{
		return $this->ADORecordSet_odbc($id,$mode);
	}
	
}
?>