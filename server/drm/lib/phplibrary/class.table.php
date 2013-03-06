<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.table.php
// Date: 28.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

Define('c_iTableStyleNothing', 100);
Define('c_iTableStyleSimple', 101);
Define('c_iTableStyleInsert', 102);
Define('c_iTableStyleDelete', 103);
Define('c_iTableStyleDarkLight', 104);

class CTable
{
	var $Table, $ColWidth, $RowClass, $RowStyle, $ColStyle, $ColClass, $Class, $CellPadding, $CellSpacing, $BackgroundColor, $Width, $NoWrap;
	var $m_bDebug;

	function CTable()
	{
		$this->m_bDebug = FALSE;
		$this->Clear();
	}

	function ClearStyle()
	{
		UnSet($this->ColClass);
		UnSet($this->RowClass);
		UnSet($this->ColStyle);
		UnSet($this->RowStyle);
		UnSet($this->ColWidth);
	}

	function Clear()
	{
		UnSet($this->Table);

		$this->ClearStyle();

		$this->Class = '';
		$this->CellPadding = '0';
		$this->CellSpacing = '0';
		$this->BackgroundColor = '';
		$this->Width = '100%';
		$this->NoWrap = FALSE;
	}

	function Cols()
	{
		if(IsSet($this->Table))
			return Count($this->Table);
		return 0;
	}

	function Rows()
	{
		$iRowCount = 0;
		if(IsSet($this->Table))
		{
			for($i = 0; $i < $this->Cols(); $i++)
			{
				$aKeys = Array_Keys($this->Table[$i]);
				$iCurrentRowCount = $aKeys[Count($aKeys) - 1] + 1;
				if($iCurrentRowCount > $iRowCount)
					$iRowCount = $iCurrentRowCount;
			}
		}
		return $iRowCount;
	}

	function AddString($sString)
	{
		$aDummy[1] = $sString;
		$this->AddRow($aDummy);
	}

	function AddRow($Row)
	{
		$iCol = 0;
		$iRowCount = $this->Rows();
		for(Reset($Row); $i = Key($Row); Next($Row))
		{
			$this->Table[$iCol][$iRowCount] = $Row[$i];
			$iCol++;
		}
	}

	function Style($iStyle = c_iTableStyleNothing)
	{
		$this->ClearStyle();
		switch($iStyle)
		{
			case c_iTableStyleSimple:
				for($i = 0; $i < $this->Rows(); $i++)
					$this->RowClass[$i] = 'Content';
				break;
			case c_iTableStyleInsert:
				for($i = 0; $i < $this->Rows(); $i++)
					$this->RowClass[$i] = 'Content';
				$this->ColStyle[0] = 'font-weight : bold;';
				$this->ColWidth[0] = 100;
				break;
			case c_iTableStyleDelete:
				$this->RowClass[0] = 'Caption';
				for($i = 1; $i < $this->Rows(); $i++)
					$this->RowClass[$i] = 'Content';
				$iCol = $this->Cols() - 1;
				$this->ColStyle[$iCol] = 'text-align : center;';
				$this->ColWidth[$iCol] = 100;
				break;
			case c_iTableStyleDarkLight:
				for($i = 0; $i < $this->Rows(); $i++)
					$this->RowClass[$i] = $i % 2 == 0 ? 'Dark' : 'Light';
				break;
		}
	}

	function HTML()
	{
		$iColCount = $this->Cols();
		$iRowCount = $this->Rows();

		$sHTML = '';

		if($this->m_bDebug)
			$sHTML .= phpCrLf;

		for($iRow = 0; $iRow < $iRowCount; $iRow++)
		{
			$sHTML .= '<tr>';
			for($iCol = 0; $iCol < $iColCount; $iCol++)
			{
				$sColClass = IsData($this->ColClass[$iCol]) ? $this->ColClass[$iCol] : '';
				$sRowClass = IsData($this->RowClass[$iRow]) ? $this->RowClass[$iRow] : '';
				$sColStyle = IsData($this->ColStyle[$iCol]) ? $this->ColStyle[$iCol] : '';
				$sRowStyle = IsData($this->RowStyle[$iRow]) ? $this->RowStyle[$iRow] : '';
				$sColWidth = IsData($this->ColWidth[$iCol]) ? $this->ColWidth[$iCol] : '';
				$sCell = IsData($this->Table[$iCol][$iRow]) ? $this->Table[$iCol][$iRow] : '&nbsp;';
				$sNoWrap = BooleanToStr($this->NoWrap);
				$sHTML .= Tag('td', $sCell, 'class', $sColClass, 'class', $sRowClass, 'style', $sColStyle, 'style', $sRowStyle, 'width', $sColWidth);
			}
			$sHTML .= '</tr>';

			if($this->m_bDebug)
				$sHTML .= phpCrLf;
		}

		if(IsData($sHTML))
			$sHTML = Tag('table', $sHTML, 'width', $this->Width, 'cellpadding', $this->CellPadding, 'cellspacing', $this->CellSpacing, 'class', $this->Class, 'backgroundcolor', $this->BackgroundColor);

		if($this->m_bDebug)
			$sHTML = phpCrLf . phpCrLf . $sHTML . phpCrLf . phpCrLf;

		return $sHTML;
	}
}

/*
$cTable = new CTable;

for($iRow = 0; $iRow < 5; $iRow++)
	for($iCol = 0; $iCol < 5; $iCol++)
		$cTable->Table[$iCol][$iRow] = $iRow . '-' . $iCol;

$cTable->CellSpacing = 1;
$cTable->CellPadding = 1;

echo $cTable->HTML();

$cTable->Style(c_iTableStyleDelete);
echo $cTable->HTML();

$cTable->Style(c_iTableStyleDarkLight);
echo $cTable->HTML();

*/

function TestClassTable()
{
	$aTests[] = '$c = new CTable; return StrLen($c->HTML()) == 0;';
	$aTests[] = '$c = new CTable; $c->Table[0][0] = "X"; return StrLen($c->HTML()) > 70;';

	return Test('ClassTable', $aTests);
}


?>