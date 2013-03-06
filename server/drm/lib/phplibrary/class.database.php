<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.sql.php
// Date: 26.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

//Protocol Layer
Define('c_iDatabaseConnectionODBC', 100);
Define('c_iDatabaseConnectionMySQL', 101);
Define('c_iDatabaseConnectionMSSQL', 102);
Define('c_iDatabaseConnectionInterbase', 103);
Define('c_iDatabaseConnectionOracle', 104);
Define('c_iDatabaseConnectionIngres', 105);
Define('c_iDatabaseConnectionMSQL', 106);
Define('c_iDatabaseConnectionSybase', 107);
Define('c_iDatabaseConnectionSesam', 108);

//Software Layer - Not Implemented
Define('c_iDatabaseTypeAny', 200);
Define('c_iDatabaseTypeMySQL', 201);
Define('c_iDatabaseTypeMSSQL', 202);
Define('c_iDatabaseTypeInterbase', 203);
Define('c_iDatabaseTypeOracle', 204);
Define('c_iDatabaseTypeIngres', 205);
Define('c_iDatabaseTypeMSQL', 206);
Define('c_iDatabaseTypeSybase', 207);
Define('c_iDatabaseTypeSesam', 208);

class CDatabase
{
	var $m_iDatabaseConnection, $m_iDatabaseType; 
	var $m_iConnectionID, $m_iResultID;
	var $m_bPersistent;
	var $m_CLog;
	var $m_iQueryCount, $m_iQueryTime;

	function CDatabase()
	{
		$this->m_iQueryCount = 0;
		$this->m_iQueryTime = 0;

		$this->m_CLog = New CLog;
		$this->m_CLog->Prefix('CDatabase');
		$this->m_CLog->Off();
	}

	function LogOn()
	{
		$this->m_CLog->On();
	}

	function LogOff()
	{
		$this->m_CLog->Off();
	}

	function Close()
	{
		switch($this->m_iDatabaseConnection)
		{
			case c_iDatabaseConnectionMySQL:
				MySQL_Close($this->m_iConnectionID);
				break;
			case c_iDatabaseConnectionMSSQL:
				MSSQL_Close($this->m_iConnectionID);
				break;
			case c_iDatabaseConnectionODBC:
				ODBC_Close($this->m_iConnectionID);
				break;
		}
	}

	//@ODBC Host => System DSN Name
	function Connect($sHost, $sUser, $sPassword, $sDatabase, $sDatabaseConnection, $sDatabaseType = c_iDatabaseTypeAny, $bPersistent = FALSE)
	{
		$this->m_iDatabaseConnection = $sDatabaseConnection;
		$this->m_iDatabaseType = $sDatabaseType;
		$this->m_bPersistent = $bPersistent;

		switch($this->m_iDatabaseConnection)
		{
			case c_iDatabaseConnectionMySQL:
				if($this->m_bPersistent)
					$this->m_iConnectionID = MySQL_PConnect($sHost, $sUser, $sPassword);
				else
					$this->m_iConnectionID = MySQL_Connect($sHost, $sUser, $sPassword);
				$this->SelectDatabase($sDatabase);
				break;
			case c_iDatabaseConnectionMSSQL:
				if($this->m_bPersistent)
					$this->m_iConnectionID = MSSQL_PConnect($sHost, $sUser, $sPassword);
				else
					$this->m_iConnectionID = MSSQL_Connect($sHost, $sUser, $sPassword);
				$this->SelectDatabase($sDatabase);
				break;
			case c_iDatabaseConnectionODBC:
				if($this->m_bPersistent)
					$this->m_iConnectionID = ODBC_PConnect($sHost, $sUser, $sPassword);
				else
					$this->m_iConnectionID = ODBC_Connect($sHost, $sUser, $sPassword);
				break;
		}
	}

	function SelectDatabase($sDatabase)
	{
		if(StrLen($sDatabase) > 0)
			switch($this->m_iDatabaseConnection)
			{
			case c_iDatabaseConnectionMySQL:
  				MySQL_select_DB($sDatabase, $this->m_iConnectionID);
					break;
			case c_iDatabaseConnectionMSSQL:
  				MSSQL_select_DB($sDatabase, $this->m_iConnectionID);
					break;
			case c_iDatabaseConnectionODBC:
					break;
			}
	}

	function Query($sSQL)
	{
		$this->m_CLog->Write($sSQL);

		$cTimer = new CTimer;

		switch($this->m_iDatabaseConnection)
		{
			case c_iDatabaseConnectionMySQL:
				$this->m_iResultID = MySQL_Query($sSQL, $this->m_iConnectionID);
				break;
			case c_iDatabaseConnectionMSSQL:
				$this->m_iResultID = MSSQL_Query($sSQL, $this->m_iConnectionID);
				break;
			case c_iDatabaseConnectionODBC:
				$this->m_iResultID = ODBC_Exec($this->m_iConnectionID, $sSQL);
				break;
		}

		$cTimer->Stop();

		if($this->m_iResultID === FALSE)
			$this->m_CLog->Write('ERROR');

		$this->m_iQueryCount++;
		$this->m_iQueryTime = MathAdd($this->m_iQueryTime, $cTimer->Delay());
	}

	function Fetch()
	{
		switch($this->m_iDatabaseConnection)
		{
			case c_iDatabaseConnectionMySQL:
				$aFields = MySQL_Fetch_Array($this->m_iResultID);
				break;
			case c_iDatabaseConnectionMSSQL:
				$aFields = MSSQL_Fetch_Array($this->m_iResultID);
				break;
			case c_iDatabaseConnectionODBC:
				ODBC_Fetch_Row($this->m_iResultID);
				for($i = 1; $i <= ODBC_Num_Fields($this->m_iResultID); $i++)
					$aFields[ODBC_Field_Name($this->m_iResultID, $i)] = ODBC_Result($this->m_iResultID, $i);
		}
		return $aFields;
	}

	function QueryCount()
	{
		return $this->m_iQueryCount;
	}

	function QueryTime()
	{
		$cTime = new CTime();
		$cTime->Set($this->m_iQueryTime);
		return $cTime->Format('hh:mm:ss.t.u');
	}

	//$aElements[] = array('Name' => '?', 'Value' => '?');
	function Insert($sTable, $aElements)
	{
		for($i = 0; $i < Count($aElements); $i++)
		{
			$aNames[] = $aElements[$i]['Name'];
			$aValues[] = $aElements[$i]['Value'];
		}

		$sSQL = 'INSERT INTO ' . $sTable . ' (' . Implode(',', $aNames) . ') VALUES (' . Implode(',', $aValues). ')';
		
		$this->Query($sSQL);
	}

	//$aElements[] = array('Name' => '?', 'Value' => '?');
	function Update($sTable, $aElements, $sWhere)
	{
		for($i = 0; $i < Count($aElements); $i++)
			$aValues[] = $aElements[$i]['Name'] . '=' . $aElements[$i]['Value'];

		$sSQL = 'UPDATE ' . $sTable . ' SET ' . Implode(',', $aValues);
		If(StrLen($sWhere) > 0)
			$sSQL .= ' WHERE ' . $sWhere;

		$this->Query($sSQL);
	}

	function Delete($sTable, $sWhere)
	{
		$sSQL = 'DELETE FROM ' . $sTable;
		If(StrLen($sWhere) > 0)
			$sSQL .= ' WHERE ' . $sWhere;
	
		$this->Query($sSQL);
	}

	//$aElements = array('?', '?', ...);
	function Select($sTable, $aElements, $sWhere = '', $sOrder = '')
	{
		$sSQL = 'SELECT ' . Implode(',', $aElements) . ' FROM ' . $sTable;
		If(StrLen($sWhere) > 0)
			$sSQL .= ' WHERE ' . $sWhere;
		If(StrLen($sOrder) > 0)
			$sSQL .= ' ORDER BY ' . $sOrder;

		$this->Query($sSQL);
	}
}

/*
$cDB = new CDatabase;
$cDB->LogOn();
$cDB->Connect('127.0.0.1', 'root', '', 'mysql', c_iDatabaseConnectionMySQL);
$cDB->Query('SELECT * FROM User');
while($aRow = $cDB->Fetch())
	W('*', $aRow['Password'], '*');
W('QueryCount', $cDB->QueryCount());
W('QueryCount', $cDB->QueryTime());
*/

function TestClassDatabase()
{
	$aTests[] = '$cDB = new CDatabase; return $cDB->QueryCount() == 0;';

	return Test('ClassDatabase', $aTests);
}

?>