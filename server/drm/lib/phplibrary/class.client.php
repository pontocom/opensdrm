<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.client.php
// Date: 26.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

///////////////////////////////////////////////////////////////////////////////
// CClient (TCP, UDP)

class CClient
{
	var $m_hSocket, $m_iBlockSize;

	function Connect($sProtocol, $sHost, $iPort)
	{
		switch(StrToLower($sProtocol))
		{
			case 'udp':
				$this->m_hSocket = FSockOpen('udp://' . $sHost, $iPort);
				break;
			default:
				$this->m_hSocket = FSockOpen($sHost, $iPort);
				break;
		}
	}

	function Disconnect()
	{
		FClose($this->m_hSocket);
	}

	function Send($sRequest)
	{
		if($this->m_hSocket !== FALSE)
			FPuts($this->m_hSocket, $sRequest);
	}

	function ReceiveChar()
	{
		if($this->m_hSocket !== FALSE)
			return FRead($this->m_hSocket, 1);
		return '';
	}

	function ReceiveLine()
	{
		$sBuffer = '';
		while(StrRight($sBuffer, 2) != phpCrLf)
			$sBuffer .= $this->ReceiveChar();
		return $sBuffer;
	}

	function ReceiveAll()
	{
		$sResponse = '';
		if($this->m_hSocket !== FALSE)
			while(!FEoF($this->m_hSocket))
				$sResponse .= FRead($this->m_hSocket, $this->m_iBlockSize);
		return $sResponse;
	}

	function SendAndReceiveAll($sRequest)
	{
		$this->Send($sRequest);
		return $this->ReceiveAll();
	}

	function SendAndReceiveLine($sRequest)
	{
		$this->Send($sRequest);
		return $this->ReceiveLine();
	}
}

///////////////////////////////////////////////////////////////////////////////
// CTIMEClient

class CTIMEClient extends CClient
{
	function CTIMEClient()
	{
		$this->m_iBlockSize = 128;
		$this->m_iPort = 37;
	}

	function Get($sHost)
	{
		$this->Connect('tcp', $sHost, $this->m_iPort);
		$sResponse = $this->ReceiveAll();
		$this->Disconnect();

		//2'208'988'800 => 01.01.1970 00:00 GMT 
		return MathSub(MathStrToDec($sResponse), '2208988800');
	}

}

/*
$cTIME = new CTIMEClient;
$sData = $cTIME->Get('ptbtime1.ptb.de');
echo Date('d.m.Y G:i:s', $sData);
*/

///////////////////////////////////////////////////////////////////////////////
// CHTTPClient

class CHTTPClient extends CClient
{
  var $m_sProxyHost, $m_iProxyPort, $m_bProxy;
	var $m_sUserAgent;

	function CHTTPClient()
	{
		$this->m_sProxyHost = '';
		$this->m_iProxyPort = 3128;
		$this->m_bProxy = FALSE;

		$this->m_iBlockSize = 4096;
		$this->m_sUserAgent = 'DeepDevClient 1.0';
	}

	function HTTPRequest($sHost, $iPort, $aHeaders, $sData = '')
	{
		$this->Connect('tcp', $sHost, $iPort);

		$sRequest = Implode(phpCrLf,  $aHeaders) . phpCrLf . phpCrLf . $sData;
		$sResponse = $this->SendAnReceiveAll($sRequest);

		$this->Disconnect();

		$sFileFormat = FileFormat(SubStr($sResponse, 0, 60));
		$sBoundary = $sFileFormat . $sFileFormat;

		$iPosition = StrPos($sResponse, $sBoundary);
		$sResponse = SubStr($sResponse, $iPosition + StrLen($sBoundary));

		return $sResponse;
	}

	function Get($sHost, $iPort = 80, $sURL = '/')
	{
		if($this->m_bProxy)
		{
			$sThisURL = 'http://' . $sHost . ':' . $iPort . $sURL;
			$sThisHost = $this->m_sProxyHost;
			$iThisPort =  $this->m_iProxyPort;
		}
		else
		{
			$sThisURL = $sURL;
			$sThisHost = $sHost;
			$iThisPort =  $iPort;
		}

		$aHeaders[] = 'GET ' . $sThisURL . ' HTTP/1.0';
		$aHeaders[] = 'User-Agent: ' . $this->m_sUserAgent; 
		$aHeaders[] = 'Host: ' . $sHost;
		return $this->HTTPRequest($sThisHost, $iThisPort, $aHeaders);
	}

	function PostData($sHost, $iPort, $sURL, $sData)
	{
		if($this->m_bProxy)
		{
			$sThisURL = 'http://' . $sHost . ':' . $iPort . $sURL;
			$sThisHost = $this->m_sProxyHost;
			$iThisPort =  $this->m_iProxyPort;
		}
		else
		{
			$sThisURL = $sURL;
			$sThisHost = $sHost;
			$iThisPort =  $iPort;
		}

		$aHeaders[] = 'POST ' . $sThisURL . ' HTTP/1.0';
		$aHeaders[] = 'User-Agent: ' . $this->m_sUserAgent; 
		$aHeaders[] = 'Host: ' . $sHost; 
		$aHeaders[] = 'Content-Type: application/x-www-form-urlencoded';
		$aHeaders[] = 'Content-Length: ' . StrLen($sData); 

		return $this->HTTPRequest($sThisHost, $iThisPort, $aHeaders, $sData);
	}

	//$aElements = array('Name' => '?', 'Value' => '?');
	function Post($sHost, $iPort, $sURL, $aElements)
	{
		for($i = 0; $i < Count($aElements); $i++)
			$aData[] = $aElements[$i]['Name'] . '=' . URLEncode($aElements[$i]['Value']);
		$sData = Implode('&', $aData);

		return $this->PostData($sHost, $iPort, $sURL, $sData);
	}
}

/*
$aElements[0]['Name'] = 'MyName';
$aElements[0]['Value'] = 'MyValue';
echo $cHTTP->Post('localhost', '/index.php', $aElements);
*/

///////////////////////////////////////////////////////////////////////////////
// Network Functions

function ScanPort($sHost, $iPort, $sProtocol = 'tcp')
{
	$hSocket = FSockOpen($sHost, $iPort, $iError, $sError, 0);
	if(!$hSocket)
	{
		return FALSE;
	}
	else
	{
		FClose($hSocket);
		return TRUE;
	}
}

function TestClassClient()
{
	$aTests[] = '$cTIME = new CTIMEClient; return StrLen($cTIME->Get("ptbtime1.ptb.de")) > 8;';
	$aTests[] = '$cHTTP = new CHTTPClient; return StrLen($cHTTP->Get("www.google.ch", 80, "/")) > 2000;';
	$aTests[] = 'return ScanPort("www.google.ch", 80);';

	return Test('ClassClient', $aTests);
}

?>