<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.image.php
// Date: 26.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

class CImage
{
	var $m_hImage, $m_fRed, $m_fGreen, $m_fBlue, $Width, $Height, $m_sPath;

	function CImage()
	{
		//Weights R = 0.3086, G = 0.6094, B = 0.0820
		//NTSC Weights R = 0.299, G = 0.587, B = 0.144
		$this->m_sPath = 'c:/winnt/fonts/';
		$this->m_fRed = 0.299;
		$this->m_fGreen = 0.587;
		$this->m_fBlue = 0.144;
	}

	function Image()
	{
		return $this->m_hImage;
	}

	function LoadPNG($sFileName)
	{
		$this->m_hImage = ImageCreateFromPNG($sFileName); 
	}

	function LoadJPEG($sFileName)
	{
		$this->m_hImage = ImageCreateFromJPEG($sFileName); 
	}

	function Create($iWidth, $iHeight, $iRed = 255, $iGreen = 0, $iBlue = 255)
	{
		//iRed, iGreen, iBlue => Transparent Color
		$this->Width = $iWidth;
		$this->Height = $iHeight;
		$this->m_hImage = ImageCreate($iWidth, $iHeight);
		$iColor = ImageColorAllocate($this->m_hImage, $iRed, $iGreen, $iBlue);
		ImageColorTransparent($this->m_hImage, $iColor);
	}

	function GetPixel($iX, $iY)
	{
		$iColor = ImageColorAt($this->m_hImage, $iX, $iY);
		return ImageColorsForIndex($this->m_hImage, $iColor);
	}

	function SetPixel($iX, $iY, $iRed, $iGreen, $iBlue)
	{
		$iColor = ImageColorResolve($this->m_hImage, $iRed, $iGreen, $iBlue);
		ImageSetPixel($this->m_hImage, $iX,  $iY, $iColor);
	}
	
	function NewColor($iRed = 0, $iGreen = 0, $iBlue = 0)
	{
		return ImageColorAllocate($this->m_hImage, $iRed, $iGreen, $iBlue);
	}

	function Text($iX, $iY, $sText, $iColor, $iAngle = 0)
	{
		ImageString($this->m_hImage, 4, $iX, $iY, $sText, $iColor);
	}

	function TextTTF($iX, $iY, $sText, $iColor, $sTTF = 'Arial', $iFontSize = 10, $iAngle = 0)
	{
		ImageTTFText($this->m_hImage, $iFontSize, $iAngle, $iX, $iY, $iColor, $this->m_sPath . $sTTF . '.ttf', $sText);
	}

	function TTFWidth($sText, $sTTF = 'Arial', $iFontSize = 10, $iAngle = 0)
	{
		$aBox = ImageTTFBBox($iFontSize, $iAngle, $this->m_sPath . $sTTF . '.ttf', $sText);
		return $aBox[2] - $aBox[6];
	}

	function TTFHeight($sText, $sTTF = 'Arial', $iFontSize = 10, $iAngle = 0)
	{
		$aBox = ImageTTFBBox($iFontSize, $iAngle, $this->m_sPath . $sTTF . '.ttf', $sText);
		return $aBox[3] - $aBox[7];
	}

	function Line($iX1, $iY1, $iX2, $Y2, $iColor)
	{
		ImageLine($this->m_hImage, $iX1, $iY1, $iX2, $Y2, $iColor);
	}

	function GetPixel($iX, $iY)
	{
		$iColor = ImageColorAt($this->m_hImage, $iX, $iY);
		return ImageColorsForIndex($this->m_hImage, $iColor);
	}

	function SetPixel($iX, $iY, $iColor)
	{
		ImageSetPixel($this->m_hImage, $iX,  $iY, $iColor);
	}

	function SetNearPixel($iX, $iY, $iRed, $iGreen, $iBlue)
	{
		$iColor = ImageColorResolve($this->m_hImage, $iRed, $iGreen, $iBlue);
		ImageSetPixel($this->m_hImage, $iX,  $iY, $iColor);
	}

	function Negative()
	{
		for($iIndex = 0; $iIndex < ImageColorsTotal($this->m_hImage); $iIndex++) 
		{ 
			$aColor = ImageColorsForIndex($this->m_hImage, $iIndex); 
			$R = 255 - $aColor['red'];
			$G = 255 - $aColor['green'];
			$B = 255 - $aColor['blue'];
			ImageColorSet($this->m_hImage, $iIndex, $R, $G, $B); 
		} 
	}

	function GrayScale()
	{
		for($iIndex = 0; $iIndex < ImageColorsTotal($this->m_hImage); $iIndex++) 
		{ 
			$aColor = ImageColorsForIndex($this->m_hImage, $iIndex); 
			$R = UpperLimit($this->m_fRed * ($aColor['red']) + $this->m_fGreen * ($aColor['green']) + $this->m_fBlue * ($aColor['blue']), 255); 
			$G = UpperLimit($this->m_fRed * ($aColor['red']) + $this->m_fGreen * ($aColor['green']) + $this->m_fBlue * ($aColor['blue']), 255); 
			$B = UpperLimit($this->m_fRed * ($aColor['red']) + $this->m_fGreen * ($aColor['green']) + $this->m_fBlue * ($aColor['blue']), 255); 
			ImageColorSet($this->m_hImage, $iIndex, $R, $G, $B); 
		} 
	}

	function ColorBrightness($fRed = 1.0, $fGreen = 1.0, $fBlue = 1.0)
	{
		for($iIndex = 0; $iIndex < ImageColorsTotal($this->m_hImage); $iIndex++) 
		{ 
			$aColor = ImageColorsForIndex($this->m_hImage, $iIndex); 
			$R = UpperLimit($aColor['red'] * $fRed, 255);
			$G = UpperLimit($aColor['green'] * $fGreen, 255);
			$B = UpperLimit($aColor['blue'] * $fBlue, 255);
			ImageColorSet($this->m_hImage, $iIndex, $R, $G, $B); 
		} 
	}

	function Brightness($fFactor = 1.0)
	{
		$this->ColorBrightness($fFactor, $fFactor, $fFactor);
	}

	function ColorSaturation($fRed = 1.0, $fGreen = 1.0, $fBlue = 1.)
	{
		for($iIndex = 0; $iIndex < ImageColorsTotal($this->m_hImage); $iIndex++) 
		{ 
			$aColor = ImageColorsForIndex($this->m_hImage, $iIndex); 
			$R = UpperLimit(((1.0 - $fRed) * $this->m_fRed + $fRed) * ($aColor['red']) + ((1.0 - $fGreen) * $this->m_fGreen) * ($aColor['green']) + ((1.0 - $fBlue) * $this->m_fBlue) * ($aColor['blue']), 255);
			$G = UpperLimit(((1.0 - $fRed) * $this->m_fRed) * ($aColor['red']) + ((1.0 - $fGreen) * $this->m_fGreen + $fGreen) * ($aColor['green']) + ((1.0 - $fBlue) * $this->m_fBlue) * ($aColor['blue']), 255);
			$B = UpperLimit(((1.0 - $fRed) * $this->m_fRed) * ($aColor['red']) + ((1.0 - $fGreen) * $this->m_fGreen) * ($aColor['green']) + ((1.0 - $fBlue) * $this->m_fBlue + $fBlue) * ($aColor['blue']), 255);
			ImageColorSet($this->m_hImage, $iIndex, $R, $G, $B); 
		} 
	}

	function Saturation($fFactor = 1.0)
	{
		$this->ColorSaturation($fFactor, $fFactor, $fFactor);
	}

	function ColorContrast($fRed = 1.0, $fGreen = 1.0, $fBlue = 1.0)
	{
		for($iIndex = 0; $iIndex < ImageColorsTotal($this->m_hImage); $iIndex++) 
		{ 
			$aColor = ImageColorsForIndex($this->m_hImage, $iIndex); 
			$R = Limit($aColor['red'] * $fRed + (1.0 - $fRed) * 128, 0, 255);
			$G = Limit($aColor['green'] * $fGreen + (1.0 - $fGreen) * 128, 0, 255);
			$B = Limit($aColor['blue'] * $fBlue +(1.0 - $fBlue) * 128, 0, 255);
			ImageColorSet($this->m_hImage, $iIndex, $R, $G, $B); 
		} 
	}

	function Contrast($fFactor = 1.0)
	{
		$this->ColorContrast($fFactor, $fFactor, $fFactor);
	}

	function Width()
	{
		return ImageSX($this->m_hImage);
	}

	function Height()
	{
		return ImageSY($this->m_hImage);
	}

	function ColorBlur($fRed = 1.0, $fGreen = 1.0, $fBlue = 1.0)
	{
		$iWidth = ImageSX($this->m_hImage);
		$iHeight = ImageSY($this->m_hImage);
		$hImage = ImageCreate($iWidth, $iHeight);

		ImageCopy($hImage, $this->m_hImage, 0, 0, 0, 0, $iWidth, $iHeight);

		for($y = 1; $y < ($iHeight - 1); $y++)
		{
			for($x = 1; $x < ($iWidth - 1); $x++)
			{
				$P0 = GetPixel($this->m_hImage, $x, $y);
				$P1 = GetPixel($this->m_hImage, $x + 1, $y);
				$P2 = GetPixel($this->m_hImage, $x - 1, $y);
				$P3 = GetPixel($this->m_hImage, $x, $y + 1);
				$P4 = GetPixel($this->m_hImage, $x, $y - 1);
				$R = $P1['red'] + $P2['red'] + $P3['red'] + $P4['red']; 
				$G = $P1['green'] + $P2['green'] + $P3['green'] + $P4['green']; 
				$B = $P1['blue'] + $P2['blue'] + $P3['blue'] + $P4['blue'];

				$R = $P0['red'] * $fRed + (1.0 - $fRed) * ($R / 4);
				$G = $P0['green'] * $fGreen + (1.0 - $fGreen) * ($G / 4);
				$B = $P0['blue'] * $fBlue + (1.0 - $fBlue) * ($B / 4);
				
				SetPixel($hImage, $x, $y, Limit($R, 0, 255), Limit($G, 0, 255), Limit($B, 0, 255));
			}
		}
		ImageDestroy($this->m_hImage);
		$this->m_hImage = $hImage;
	}

	function Blur($fFactor = 1.0)
	{
		$this->ColorBlur($fFactor, $fFactor, $fFactor);
	}

	function Resample($iWidth, $iHeight)
	{
		$hImage = ImageCreate($iWidth, $iHeight);
		ImageCopyResized($hImage, $this->m_hImage, 0, 0, 0, 0, $iWidth, $iHeight, ImageSX($this->m_hImage), ImageSY($this->m_hImage));
		ImageDestroy($this->m_hImage);
		$this->m_hImage = $hImage;
	}

	function JPEG($iQuality = 100)
	{
		$iQuality = Limit($iQuality, 0, 100);

		OB_Start();
		ImageJPEG($this->m_hImage, '', $iQuality);
		$sData = OB_Get_Contents();
		OB_End_Clean();
		return $sData;
	}

	function PNG()
	{
		OB_Start(); 
		ImagePNG($this->m_hImage);
		$sData = OB_Get_Contents();
		OB_End_Clean();
	}
}

/*
$cImage = new CImage();
$cImage->LoadJPEG('myjpeg.jpg');

$cImage->Contrast(2.0);
$cImage->Brightness(2.0);
$cImage->Resample(250, 250);

CreateFile('mynewjepg.jpg', $cImage->JPEG(50));
*/

function TestClassImage()
{
	$aTests[] = '$cImage = new CImage(); $cImage->LoadJPEG("myjpeg.jpg"); CreateFile("mynewjpeg.jpg", $cImage->JPEG(50)); return FileSize("mynewjpeg.jpg") > 70000;';
	$aTests[] = 'DeleteFile("mynewjpeg.jpg"); return TRUE;';

	return Test('ClassImage', $aTests);
}

?>