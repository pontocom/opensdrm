<?
///////////////////////////////////////////////////////////////////////////////
// Filename: include.cache.php
// Date: 23.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

class CCache
{
	var $m_sCachePath;

	function CCache()
	{
		$this->m_sCachePath = '';
		$this->m_sPrefix = '';
		$this->m_bCache = TRUE;
	}

	function Invalidate()
	{
		$aIdentifiers = Func_Get_Args();
		UnLink($this->FileName($aIdentifiers));
	}

	function IsCached()
	{
		$aIdentifiers = Func_Get_Args();
		return File_Exists($this->FileName($aIdentifiers));
	}

	function Cache()
	{
		$aIdentifiers = Func_Get_Args();
		$xData = $aIdentifiers[Count($aIdentifiers) - 1];
		$aIdentifiers = Array_Slice($aIdentifiers, 0, Count($aIdentifiers) - 1);

		if($this->m_bCache)
			CreateFile($this->FileName($aIdentifiers), Serialize($xData));
	}

	function Retrieve()
	{
		$aIdentifiers = Func_Get_Args();
		return UnSerialize(GetFile($this->FileName($aIdentifiers)));
	}

	function FileName($aIdentifiers)
	{
		for($i = 0; $i < Count($aIdentifiers); $i++)
			$aIdentifiers[$i] = StrPadLeft($aIdentifiers[$i], 10, '0');

		return $this->m_sCachePath . $this->m_sPrefix . Implode('-', $aIdentifiers) . '.txt';
	}

}

function TestClassCache()
{
	$aTests[] = '$cCache = new CCache; return $cCache->IsCached(1) == FALSE;';
	$aTests[] = '$cCache = new CCache; $cCache->Cache(1, "MyCache"); return $cCache->IsCached(1) == TRUE;';
	$aTests[] = '$cCache = new CCache; return $cCache->Retrieve(1) == "MyCache";';
	$aTests[] = '$cCache = new CCache; $cCache->Invalidate(1); return $cCache->IsCached(1) == FALSE;';

	return Test('ClassCache', $aTests);
}

?>