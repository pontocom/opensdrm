<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.pdf.php
// Date: 26.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

Define('c_sPDFLinuxPath', '/');
Define('c_sPDFWindows2000Path', '');

Define('c_iAlignLeft', 1);
Define('c_iAlignRight', 2);
Define('c_iAlignCenter', 3);
Define('c_iAlignJustification', 4);

Define('c_sPDFReplacer', '[...]');

//Millimeter to Post Script Points
function MMToPSP($iValue)
{
	return Floor($iValue * 2.835);
}

class CPDF
{
	var $m_hDoc;
	var $m_iMarginLeft, $m_iMarginRight, $m_iMarginTop, $m_iMarginBottom;
	var $m_iPageWidth, $m_iPageHeight;
	var $m_iContentWidth, $m_iContentHeight;
	var $m_sFont, $m_iFontSize;
	var $m_iFontSpace;
	var $m_iY, $m_iX;

	function CPDF($sTitle = 'Title', $sSubject = 'Subject', $sAuthor = 'Author', $sCreator = 'Creator')
	{
		$this->m_hDoc = PDF_New();
		PDF_Open_File($this->m_hDoc, '');

		$this->m_iMarginLeft = MMToPSP(10);
		$this->m_iMarginRight = MMToPSP(10);
		$this->m_iMarginTop = MMToPSP(10);
		$this->m_iMarginBottom = MMToPSP(20);

		$this->m_iPageWidth = MMToPSP(210);
		$this->m_iPageHeight = MMToPSP(297);

		$this->m_iContentWidth = $this->m_iPageWidth - $this->m_iMarginLeft - $this->m_iMarginRight;
		$this->m_iContentHeight = $this->m_iPageHeight - $this->m_iMarginTop - $this->m_iMarginBottom;

		$this->SetFont();

		PDF_Set_Info_Title($this->m_hDoc , $sTitle); //Titel: ...
		PDF_Set_Info_Subject($this->m_hDoc , $sSubject); //Subject: ...
		PDF_Set_Info_Author($this->m_hDoc , $sAuthor); //Autor: ...
		PDF_Set_Info_Creator($this->m_hDoc , $sCreator); //Erstellt mit: ...
	}

	function SetFont($sFont = 'Arial', $iFontSize = 10, $fFontSpace = 0.2)
	{
		$this->m_sFont = $sFont;
		$this->m_iFontSize = $iFontSize;
		$this->m_iFontSpace = Floor($iFontSize * $fFontSpace);
	}

	function CloseAndGetBuffer()
	{
		PDF_Close($this->m_hDoc);
		return PDF_Get_Buffer($this->m_hDoc);
	}

	function ApplyFont()
	{
		global $g_iOperatingSystem;
		switch($g_iOperatingSystem)
		{
			case c_iOperatingSystemLinux:
				$sPath = c_sPDFLinuxPath;
				break;
			case c_iOperatingSystemWindows2000:
				$sPath = c_sPDFWindows2000Path;
				break;
		}

		PDF_Set_Parameter($this->m_hDoc, 'FontOutline', $this->m_sFont . '=' . $sPath . StrToLower($this->m_sFont) . '.ttf');

		$iBuiltIn = 0;
		$hFont = PDF_FindFont($this->m_hDoc, $this->m_sFont, 'winansi', $iBuiltIn);
		PDF_SetFont($this->m_hDoc, $hFont, $this->m_iFontSize);
	}

	function BeginPage()
	{
		PDF_Begin_Page($this->m_hDoc , $this->m_iPageWidth, $this->m_iPageHeight);
		$this->ApplyFont();

		$this->m_iY = $this->m_iPageHeight - $this->m_iMarginTop;
		$this->m_iX = 0;
	}

	function EndPage()
	{
		PDF_End_Page($this->m_hDoc);
	}

	function NewPage()
	{
		$this->EndPage();
		$this->BeginPage();
	}

	function TextToLines($sText)
	{
		$aOutputLines = array();
		$aInputWords = Explode(' ', $sText);
		$iPosition = 0;

		while($iPosition < Count($aInputWords))
		{
			$sCurrentLine = '';
			$iCurrentPosition = 0;
			
			while(
				(($iCurrentPosition + $iPosition) < Count($aInputWords))
				And
				(PDF_StringWidth($this->m_hDoc, $sCurrentLine . ($aInputWords[$iPosition + $iCurrentPosition])) < $this->m_iContentWidth)
			)
			{
				$sCurrentLine .= $aInputWords[$iPosition + $iCurrentPosition] . ' ';
				$iCurrentPosition++;
			}

			if(StrLen($sCurrentLine) == 0)
			{
				$sCurrentLine = c_sPDFReplacer;
				$iCurrentPosition++;
			}
			
			$aOutputLines[] = Trim($sCurrentLine);
			$iPosition += $iCurrentPosition;
		}

		return $aOutputLines;
	}

	function CalcPos($iWidth, $iHeight, $iAlign)
	{
		if($this->m_iY - $this->m_iMarginBottom - $iHeight < 0)
			$this->NewPage();

		$this->m_iY -= $iHeight;

		switch($iAlign)
		{
			case c_iAlignRight:
				$this->m_iX = $this->m_iPageWidth - $this->m_iMarginRight - $iWidth;
				break;
			case c_iAlignCenter:
				$this->m_iX = Floor(($this->m_iContentWidth - $iWidth) / 2) + $this->m_iMarginLeft;
				break;
			default:
				$this->m_iX = $this->m_iMarginLeft;
				break;
		}
	}

	function AddImage(&$cImage, $iAlign = c_iAlignLeft, $iQuality = 100, $fScale = 1.0)
	{
		$iWidth = $cImage->Width() * $fScale;
		$iHeight = $cImage->Height() * $fScale;

		$this->CalcPos($iWidth, $iHeight, $iAlign);

		$sFileName = UniqueFileName();
		CreateFile($sFileName, $cImage->JPEG($iQuality));

		$hImage = PDF_Open_Image_File($this->m_hDoc, 'jpeg', $sFileName);
		PDF_Place_Image($this->m_hDoc, $hImage, $this->m_iX, $this->m_iY, $fScale);
		PDF_Close_Image($this->m_hDoc, $hImage);

		DeleteFile($sFileName);
	}

	function AddTextLine($sText, $iAlign)
	{
		$iWidth = PDF_StringWidth($this->m_hDoc, $sText);
		$iHeight = $this->m_iFontSize;

		$this->CalcPos($iWidth, $iHeight, $iAlign);

		switch($iAlign)
		{
			case c_iAlignJustification:
				$aWords = Explode(' ', $sText);
				$fSpaceWidth = ($this->m_iContentWidth - PDF_StringWidth($this->m_hDoc, StrReplace($sText, ' ', ''))) / (Count($aWords) - 1);

				$x = $this->m_iX;
				for($i = 0; $i < Count($aWords); $i++)
				{
					PDF_Show_XY($this->m_hDoc, $aWords[$i], IntVal($x), $this->m_iY);
					$x += PDF_StringWidth($this->m_hDoc, $aWords[$i]) + $fSpaceWidth;
				}
				break;
			default:
				PDF_Show_XY($this->m_hDoc, $sText, $this->m_iX, $this->m_iY);
				break;
		}

		$this->m_iY -= $this->m_iFontSpace;
	}

	function AddText($sText, $iAlign = c_iAlignLeft)
	{
		$aLines = $this->TextToLines($sText);
		switch($iAlign)
		{
			case c_iAlignJustification:
				for($i = 0; $i < Count($aLines) - 1; $i++)
					$this->AddTextLine($aLines[$i], $iAlign);
				$this->AddTextLine($aLines[$i], c_iAlignLeft);
				break;
			default:
				for($i = 0; $i < Count($aLines); $i++)
					$this->AddTextLine($aLines[$i], $iAlign);
				break;
		}
	}

	function AddParagraph()
	{
		$this->AddText('');
	}
}

/*
$cImage = new CImage();
$cImage->LoadJPEG('myjpeg.jpg');

$cPDF = New CPDF;
$cPDF->BeginPage();
$cPDF->AddText(StrRandomSentence(), c_iAlignLeft);
$cPDF->AddParagraph();
$cPDF->AddText(StrRandomSentence(), c_iAlignCenter);
$cPDF->AddParagraph();
$cPDF->AddText(StrRandomSentence(), c_iAlignRight);
$cPDF->AddParagraph();
$cPDF->AddText(StrRandomSentence(), c_iAlignJustification);
$cPDF->AddParagraph();
$cPDF->AddImage($cImage, c_iAlignCenter, 50, 0.5);

$cPDF->EndPage();

CreateFile('mypdf.pdf', $cPDF->CloseAndGetBuffer());
*/

function TestClassPDF()
{
	$aTests[] = '$cPDF = New CPDF; $cPDF->BeginPage(); $cPDF->AddText(StrRandomSentence()); $cPDF->EndPage(); CreateFile("mypdf.pdf", $cPDF->CloseAndGetBuffer()); return FileSize("mypdf.pdf") > 4000;';
	$aTests[] = 'DeleteFile("mypdf.pdf"); return TRUE;';

	return Test('ClassPDF', $aTests);
}

?>