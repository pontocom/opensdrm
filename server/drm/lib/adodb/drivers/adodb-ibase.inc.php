<?php
/*
V3.00 6 Jan 2003  (c) 2000-2003 John Lim (jlim@natsoft.com.my). All rights reserved.  
  Released under both BSD license and Lesser GPL library license. 
  Whenever there is any discrepancy between the two licenses, 
  the BSD license will take precedence.

  Latest version is available at http://php.weblogs.com/
  
  Interbase data driver. Requires interbase client. Works on Windows and Unix.

  3 Jan 2002 -- suggestions by Hans-Peter Oeri <kampfcaspar75@oeri.ch>
  	changed transaction handling and added experimental blob stuff
  
  Docs to interbase at the website
   http://www.synectics.co.za/php3/tutorial/IB_PHP3_API.html
   
  To use gen_id(), see
   http://www.volny.cz/iprenosil/interbase/ip_ib_code.htm#_code_creategen
   
   $rs = $conn->Execute('select gen_id(adodb,1) from rdb$database');
   $id = $rs->fields[0];
   $conn->Execute("insert into table (id, col1,...) values ($id, $val1,...)");
*/


class ADODB_ibase extends ADOConnection {
	var $databaseType = "ibase";
	var $dataProvider = "ibase";
	var $replaceQuote = "\'"; // string to use to replace quotes
	var $fmtDate = "'Y-m-d'";
	var $fmtTimeStamp = "'Y-m-d, H:i:s'";
	var $concat_operator='||';
	var $_transactionID;
	var $metaTablesSQL = "select rdb\$relation_name from rdb\$relations where rdb\$relation_name not like 'RDB\$%'";
	var $metaColumnsSQL = "select a.rdb\$field_name,b.rdb\$field_type,b.rdb\$field_length from rdb\$relation_fields a join rdb\$fields b on a.rdb\$field_source=b.rdb\$field_name where rdb\$relation_name ='%s'";
	var $ibasetrans = IBASE_DEFAULT;
	var $hasGenID = true;
	var $_bindInputArray = true;
	var $buffers = 0;
	var $dialect = 1;
	var $sysDate = "cast('TODAY' as date)";
	var $sysTimeStamp = "cast('NOW' as timestamp)";
	var $ansiOuter = true;
	var $hasAffectedRows = false;
	var $poorAffectedRows = true;
	var $blobEncodeType = 'C';
	
	function ADODB_ibase() 
	{
		ibase_timefmt('%Y-%m-%d');
  	}
	
	function ServerInfo()
	{
		$arr['dialect'] = $this->dialect;
		switch($arr['dialect']) {
		case '': 
		case '1': $s = 'Interbase 5.5 or earlier'; break;
		case '2': $s = 'Interbase 5.6'; break;
		default:
		case '3': $s = 'Interbase 6.0'; break;
		}
		$arr['version'] = ADOConnection::_findvers($s);
		$arr['description'] = $s;
		return $arr;
	}

	function BeginTrans()
	{	 
		if ($this->transOff) return true;
		$this->transCnt += 1;
		$this->autoCommit = false;
	 	$this->_transactionID = $this->_connectionID;//ibase_trans($this->ibasetrans, $this->_connectionID);
		return $this->_transactionID;
	}
	
	function CommitTrans($ok=true) 
	{ 
		if (!$ok) return $this->RollbackTrans();
		if ($this->transOff) return true;
		if ($this->transCnt) $this->transCnt -= 1;
		$ret = false;
		$this->autoCommit = true;
		if ($this->_transactionID) {
			   		//print ' commit ';
			$ret = ibase_commit($this->_transactionID);
		}
		$this->_transactionID = false;
		return $ret;
	}
	
	function RollbackTrans()
	{
		if ($this->transOff) return true;
		if ($this->transCnt) $this->transCnt -= 1;
		$ret = false;
		$this->autoCommit = true;
		if ($this->_transactionID) 
				  $ret = ibase_rollback($this->_transactionID);
		$this->_transactionID = false;   
		
		return $ret;
	}
	
	// See http://community.borland.com/article/0,1410,25844,00.html
	function RowLock($tables,$where,$col)
	{
		if ($this->autoCommit) $this->BeginTrans();
		$this->Execute("UPDATE $table SET $col=$col WHERE $where "); // is this correct - jlim?
		return 1;
	}
	
	// use delete and insert instead
	function Replace($table, $fieldArray, $keyCol,$autoQuote=false)
	{
		if (count($fieldArray) == 0) return 0;

		if (!is_array($keyCol)) {
			$keyCol = array($keyCol);
		}
		
		if ($autoQuote)
			foreach($fieldArray as $k => $v) {
				if (!is_numeric($v) and $v[0] != "'" and strcasecmp($v,'null')!=0) {
					$v = $this->qstr($v);
					$fieldArray[$k] = $v;
				}
			}
			
		$first = true;
		foreach ($keyCol as $v) {
			if ($first) {
				$first = false;
				$where = "$v=$fieldArray[$v]";
			} else {
				$where .= " and $v=$fieldArray[$v]";
			}
		}
			
		$first = true;
		foreach($fieldArray as $k => $v) {
			if ($first) {
				$first = false;			
				$iCols = "$k";
				$iVals = "$v";
			} else {
				$iCols .= ",$k";
				$iVals .= ",$v";
			}				
		}
		$this->BeginTrans();
			$this->Execute("DELETE FROM $table WHERE $where");
			$ok = $this->Execute("INSERT INTO $table ($iCols) VALUES ($iVals)");
		$this->CommitTrans();
		
		return ($ok) ? 2 : 0;
	}
	
	function CreateSequence($seqname,$startID=1)
	{
		$ok = $this->Execute(("INSERT INTO RDB\$GENERATORS (RDB\$GENERATOR_NAME) VALUES (UPPER('$seqname'))" ));
		if (!$ok) return false;
		return $this->Execute("SET GENERATOR $seqname TO ".($startID-1).';');
	}
	
	function DropSequence($seqname)
	{
		$seqname = strtoupper($seqname);
		$this->Execute("delete from RDB\$GENERATORS where RDB\$GENERATOR_NAME='$seqname'");
	}
	
	function GenID($seqname='adodbseq',$startID=1)
	{
		$getnext = ("SELECT Gen_ID($seqname,1) FROM RDB\$DATABASE");
		$rs = @$this->Execute($getnext);
		if (!$rs) {
			$this->Execute(("INSERT INTO RDB\$GENERATORS (RDB\$GENERATOR_NAME) VALUES (UPPER('$seqname'))" ));
			$this->Execute("SET GENERATOR $seqname TO ".($startID-1).';');
			$rs = $this->Execute($getnext);
		}
		if ($rs && !$rs->EOF) $this->genID = (integer) reset($rs->fields);
		else $this->genID = 0; // false
		
		if ($rs) $rs->Close();
		
		return $this->genID;
	}

	function SelectDB($dbName) {
		   return false;
	}

	function _handleerror()
	{
		$this->_errorMsg = ibase_errmsg();
	}

	function ErrorNo() {
	if (preg_match('/error code = ([\-0-9]*)/i', $this->_errorMsg,$arr)) return (integer) $arr[1];
	else return 0;
	}

	function ErrorMsg() {
			return $this->_errorMsg;
	}

	   // returns true or false
	function _connect($argHostname, $argUsername, $argPassword, $argDatabasename)
	{  

		$this->_connectionID = ibase_connect($argHostname,$argUsername,$argPassword,$this->charSet,$this->buffers,$this->dialect);
	 	if ($this->dialect != 1) { // http://www.ibphoenix.com/ibp_60_del_id_ds.html
			$this->replaceQuote = "''";
		}
		if ($this->_connectionID === false) {
			$this->_handleerror();
			return false;
		}
	
		return true;
	}
	   // returns true or false
	function _pconnect($argHostname, $argUsername, $argPassword, $argDatabasename)
	{
		$this->_connectionID = ibase_pconnect($argHostname,$argUsername,$argPassword,$this->charSet,$this->buffers,$this->dialect);
		if ($this->dialect != 1) { // http://www.ibphoenix.com/ibp_60_del_id_ds.html
			$this->replaceQuote = "''";
		}
		if ($this->_connectionID === false) {
			$this->_handleerror();
			return false;
		}
		
		return true;
	}	
	
	function Prepare($sql)
	{
		return $sql;
		$stmt = ibase_prepare($sql);
		if (!$stmt) return false;
		return array($sql,$stmt);
	}

	   // returns query ID if successful, otherwise false
	   // there have been reports of problems with nested queries - the code is probably not re-entrant?
	function _query($sql,$iarr=false)
	{ 
		if (is_array($sql)) {
			$fn = 'ibase_execute';
			$sql = $sql[1];
		} else
			$fn = 'ibase_query';
		
		if (!$this->autoCommit && $this->_transactionID) {
			$conn = $this->_transactionID;
			$docommit = false;
		} else {
			$conn = $this->_connectionID;
			$docommit = true;
		}
		if (is_array($iarr)) {	
			switch(sizeof($iarr)) {
			case 1: $ret = $fn($conn,$sql,$iarr[0]); break;
			case 2: $ret = $fn($conn,$sql,$iarr[0],$iarr[1]); break;
			case 3: $ret = $fn($conn,$sql,$iarr[0],$iarr[1],$iarr[2]); break;
			case 4: $ret = $fn($conn,$sql,$iarr[0],$iarr[1],$iarr[2],$iarr[3]); break;
			case 5: $ret = $fn($conn,$sql,$iarr[0],$iarr[1],$iarr[2],$iarr[3],$iarr[4]); break;
			case 6: $ret = $fn($conn,$sql,$iarr[0],$iarr[1],$iarr[2],$iarr[3],$iarr[4],$iarr[5]); break;
			case 7: $ret = $fn($conn,$sql,$iarr[0],$iarr[1],$iarr[2],$iarr[3],$iarr[4],$iarr[5],$iarr[6]); break;
			default: ADOConnection::outp( "Too many parameters to ibase query $sql");
			case 8: $ret = $fn($conn,$sql,$iarr[0],$iarr[1],$iarr[2],$iarr[3],$iarr[4],$iarr[5],$iarr[6],$iarr[7]); break;
			}
		} else $ret = $fn($conn,$sql); 
		   
		if ($docommit && $ret === true) ibase_commit($this->_connectionID);

		$this->_handleerror();
		return $ret;
	}

	 // returns true or false
	 function _close()
	 {	   
		if (!$this->autoCommit) @ibase_rollback($this->_connectionID);
		return @ibase_close($this->_connectionID);
	 }
	
		// returns array of ADOFieldObjects for current table
	function &MetaColumns($table) 
	{
	global $ADODB_FETCH_MODE;
		
		if ($this->metaColumnsSQL) {
		
			$save = $ADODB_FETCH_MODE;
			$ADODB_FETCH_MODE = ADODB_FETCH_NUM;
		
			$rs = $this->Execute(sprintf($this->metaColumnsSQL,strtoupper($table)));
		
			$ADODB_FETCH_MODE = $save;
			if ($rs === false) return false;

			$retarr = array();
			while (!$rs->EOF) { //print_r($rs->fields);
				$fld = new ADOFieldObject();
				$fld->name = $rs->fields[0];
				$tt = $rs->fields[1];
				switch($tt)
				{
				case 7:
				case 8:
				case 9:$tt = 'INTEGER'; break;
				case 10:
				case 27:
				case 11:$tt = 'FLOAT'; break;
				default:
				case 40:
				case 14:$tt = 'CHAR'; break;
				case 35:$tt = 'DATE'; break;
				case 37:$tt = 'VARCHAR'; break;
				case 261:$tt = 'BLOB'; break;
				case 13:
				case 35:$tt = 'TIMESTAMP'; break;
				}
				$fld->type = $tt;
				$fld->max_length = $rs->fields[2];
				$retarr[strtoupper($fld->name)] = $fld;	
				
				$rs->MoveNext();
			}
			$rs->Close();
			return $retarr;	
		}
		return false;
	}
	
	function BlobEncode( $blob ) 
	{
		$blobid = ibase_blob_create( $this->_connectionID);
		ibase_blob_add( $blobid, $blob );
		return ibase_blob_close( $blobid );
	}
	
	// since we auto-decode all blob's since 2.42, 
	// BlobDecode should not do any transforms
	function BlobDecode($blob)
	{
		return $blob; 
	}
	
	// old blobdecode function
	// still used to auto-decode all blob's
	function _BlobDecode( $blob ) 
	{
		$blobid = ibase_blob_open( $blob );
		$realblob = ibase_blob_get( $blobid,MAX_BLOB_SIZE); // 2nd param is max size of blob -- Kevin Boillet <kevinboillet@yahoo.fr>
		while($string = ibase_blob_get($blobid, 8192)){ 
			$realblob .= $string; 
		}
		ibase_blob_close( $blobid );

		return( $realblob );
	} 
	
	function UpdateBlobFile($table,$column,$path,$where,$blobtype='BLOB') 
	{ 
		$fd = fopen($path,'rb'); 
		if ($fd === false) return false; 
		$blob_id = ibase_blob_create($this->_connectionID); 
		
		/* fill with data */ 
		
		while ($val = fread($fd,8192)){ 
			ibase_blob_add($blob_id, $val); 
		} 
		
		/* close and get $blob_id_str for inserting into table */ 
		$blob_id_str = ibase_blob_close($blob_id); 
		
		fclose($fd); 
		return $this->Execute("UPDATE $table SET $column=(?) WHERE $where",array($blob_id_str)) != false; 
	} 
	
	/*
		Insert a null into the blob field of the table first.
		Then use UpdateBlob to store the blob.
		
		Usage:
		 
		$conn->Execute('INSERT INTO blobtable (id, blobcol) VALUES (1, null)');
		$conn->UpdateBlob('blobtable','blobcol',$blob,'id=1');
	*/
	function UpdateBlob($table,$column,$val,$where,$blobtype='BLOB')
	{
		$blob_id = ibase_blob_create($this->_connectionID);
		ibase_blob_add($blob_id, $val);
		$blob_id_str = ibase_blob_close($blob_id);
		return $this->Execute("UPDATE $table SET $column=(?) WHERE $where",array($blob_id_str)) != false;
	}
}

/*--------------------------------------------------------------------------------------
		 Class Name: Recordset
--------------------------------------------------------------------------------------*/

class ADORecordset_ibase extends ADORecordSet 
{

	var $databaseType = "ibase";
	var $bind=false;
	var $_cacheType;
	
	function ADORecordset_ibase($id,$mode=false)
	{
	global $ADODB_FETCH_MODE;
	
			$this->fetchMode = ($mode === false) ? $ADODB_FETCH_MODE : $mode;
			return $this->ADORecordSet($id);
	}

	/*		Returns: an object containing field information.
			Get column information in the Recordset object. fetchField() can be used in order to obtain information about
			fields in a certain query result. If the field offset isn't specified, the next field that wasn't yet retrieved by
			fetchField() is retrieved.		*/

	function &FetchField($fieldOffset = -1)
	{
			 $fld = new ADOFieldObject;
			 $ibf = ibase_field_info($this->_queryID,$fieldOffset);
		
			 $fld->name = strtolower($ibf['alias']);
			 if (empty($fld->name)) $fld->name = strtolower($ibf['name']);
			 $fld->type = $ibf['type'];
			 $fld->max_length = $ibf['length'];
			 return $fld;
	}

	function _initrs()
	{
			$this->_numOfRows = -1;
			$this->_numOfFields = @ibase_num_fields($this->_queryID);
			
			// cache types for blob decode check
			for ($i=0, $max = $this->_numOfFields; $i < $max; $i++) { 
				$f1 = $this->FetchField($i); 
				$this->_cacheType[] = $f1->type;
			}				
	}

	function _seek($row)
	{
		return false;
	}

	function _fetch() 
	{
		$f = ibase_fetch_row($this->_queryID); 
		if ($f === false) return false;
		
		// OPN stuff start - optimized
		// fix missing nulls and decode blobs automatically 
		// WARNING - this could break bc.
		for ($i=0, $max = $this->_numOfFields; $i < $max; $i++) { 
			if ($this->_cacheType[$i]=="BLOB") { 
				if (isset($f[$i])) { 
					$f[$i] = ADODB_ibase::_BlobDecode($f[$i]); 
				} else { 
					$f[$i] = null; 
				} 
			} else { 
				if (!isset($f[$i])) { 
					$f[$i] = null; 
				} 
			} 
		} 
		// OPN stuff end 
		
		$this->fields = $f;
		if ($this->fetchMode & ADODB_FETCH_ASSOC) {
			$this->fields = $this->GetRowAssoc(ADODB_ASSOC_CASE);
		}
		return true;
	}

	/* Use associative array to get fields array */
	function Fields($colname)
	{
		if ($this->fetchMode & ADODB_FETCH_ASSOC) return $this->fields[$colname];
		if (!$this->bind) {
			$this->bind = array();
			for ($i=0; $i < $this->_numOfFields; $i++) {
				$o = $this->FetchField($i);
				$this->bind[strtoupper($o->name)] = $i;
			}
		}
		
		 return $this->fields[$this->bind[strtoupper($colname)]];
		
	}
	

	function _close() 
	{
			return @ibase_free_result($this->_queryID);
	}

	function MetaType($t,$len=-1,$fieldobj=false)
	{
		if (is_object($t)) {
			$fieldobj = $t;
			$t = $fieldobj->type;
			$len = $fieldobj->max_length;
		}
		switch (strtoupper($t)) {
		case 'CHAR':
			return 'C';
			
		case 'TEXT':
		case 'VARCHAR':
		case 'VARYING':
		if ($len <= $this->blobSize) return 'C';
			return 'X';
		case 'BLOB':
			return 'B';
			   
		case 'TIMESTAMP':
		case 'DATE': return 'D';
				
				//case 'T': return 'T';

				//case 'L': return 'L';
		case 'INT': 
		case 'SHORT':
		case 'INTEGER': return 'I';
		default: return 'N';
		}
	}

}
?>