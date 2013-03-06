<?
///////////////////////////////////////////////////////////////////////////////
// Filename: class.email.php
// Date: 26.08.2002
// Author: Thomas Stauffer (thomas.stauffer@deepsource.ch)
//

/*
[Priority]

2 = Low
3 = Normal
4 = High
*/

///////////////////////////////////////////////////////////////////////////////
// CSMTPEClient

function ValidEMail($sEMail, $bCheckMailServer = FALSE)
{ 
	//if(ERegI('^[0-9a-z]([-_.]?[0-9a-z])*@[0-9a-z]([-.]?[0-9a-z])*\.[a-z]{2,3}$', $sEMail, $aCheck))
	if(PReg_Match('/^[0-9a-z]([-_.]?[0-9a-z])*@[0-9a-z]([-.]?[0-9a-z])*\.[a-z]{2,3}$/is', $sEMail, $aMatch))
	{ 
		if($bCheckMailServer)
		{
			$sHost = SubStr(StrStr($aMatch[0], '@'), 1);
			$aHosts = array();
			if(GetMXRR($sHost, $aHosts))
				return TRUE; 
		}
		else
		{
			return TRUE;
		}
	} 
	return FALSE; 
}

class CSMTPClient extends CClient
{
	var $EMailFrom, $From, $Subject, $Message, $Priority;

	var $m_aToRecipients, $m_aCCRecipients, $m_aBCCRecipients, $m_sMailer;

	function CSMTPClient()
	{
		$this->m_iBlockSize = 4096;
		$this->m_iPort = 25;

		$this->EMailFrom = '';
		$this->From = '';
		$this->Subject = '';
		$this->Message = '';
		$this->Priority = 3;

		$this->m_aToRecipients = array();
		$this->m_aCCRecipients = array();
		$this->m_aBCCRecipients = array();
		$this->m_sMailer = 'DeepDevMailer 1.0';
	}

	function EMailHeader($sEMailTo, $sTo)
	{
		if(StrLen($sTo) == 0)
			$sTo = $sEMailTo;	

		return StrQuote($sTo) . ' <' . $sEMailTo . '>';
	}

	function AddTo($sEMailTo, $sTo = '')
	{
		$this->m_aToRecipients[] = $this->EMailHeader($sEMailTo, $sTo);
	}

	function AddCC($sEMailTo, $sTo = '')
	{
		$this->m_aCCRecipients[] = $this->EMailHeader($sEMailTo, $sTo);
	}

	function AddBCC($sEMailTo, $sTo = '')
	{
		$this->m_aBCCRecipients[] = $this->EMailHeader($sEMailTo, $sTo);
	}

	function Mail($sHost)
	{
		$sMailFrom = $this->EMailHeader($this->EMailFrom, $this->From);
	
		$sMessage = $this->Message;

		//Remove End Tag
		$sMessage = StrReplace($sMessage, phpCrLf . '.' . phpCrLf, phpCrLf . '. ' . phpCrLf);

		$aHeader[] = 'From: ' . $sMailFrom;
		$aHeader[] = 'Subject: ' . $this->Subject;
		$aHeader[] = 'X-Priority: ' . $this->Priority;
		$aHeader[] = 'X-Mailer: ' . $this->m_sMailer;
		$aHeader[] = 'CC: ' . Implode(',', $this->m_aCCRecipients);
		$aHeader[] = 'Message-ID: ' . MD5(CurrentTimeStamp());
		$sHeader = Implode(phpCrLf, $aHeader);
		//$aHeader[] = 'Date: Tue, 1 Jan 1970 00:00:00 +0000';
		
		$this->Connect('tcp', $sHost, $this->m_iPort);

		$this->SendAndReceiveLine('EHLO Any' . phpCrLf);
		$this->SendAndReceiveLine('MAIL FROM: ' . $sMailFrom . phpCrLf);
		foreach($this->m_aToRecipients as $sToRecipient)
			$this->SendAndReceiveLine('RCPT TO: ' . $sToRecipient . phpCrLf);
		foreach($this->m_aCCRecipients as $sCCRecipient)
			$this->SendAndReceiveLine('RCPT TO: ' . $sCCRecipient . phpCrLf);
		foreach($this->m_aBCCRecipients as $sBCCRecipient)
			$this->SendAndReceiveLine('RCPT TO: ' . $sBCCRecipient . phpCrLf);
		$this->SendAndReceiveLine('DATA' . phpCrLf);
		$this->SendAndReceiveLine($sHeader . phpCrLf . phpCrLf . $sMessage . phpCrLf . '.' . phpCrLf);
		$this->SendAndReceiveLine('QUIT' . phpCrLf);

		$this->Disconnect();
	}
}

/*
$cMail = new CSMTPClient;

$cMail->EMailFrom = 'thomas.stauffer@gmx.ch';
$cMail->From = 'Thomas Stauffer';
$cMail->Subject = 'MyMailSubject';
$cMail->Message = 'MyMailMessage';

$cMail->AddTo('ts@deepsource.ch');
$cMail->AddCC('t.stauffer@deepsource.ch');
$cMail->AddBCC('thomas.stauffer@deepsource.ch');

$cMail->Mail('mail.gmx.ch');
*/

class CPOPClient
{
	var $m_hMailBox;

	function Connect($sHost, $sUserName, $sPassword)
	{
		$sProtocol = '110/pop3';
		//$sProtocol = '143';
		$this->m_hMailBox = @IMAP_Open('{' . $sHost . ':' . $sProtocol . '}INBOX', $sUserName, $sPassword);
	}

	function Close()
	{
		IMAP_Close($this->m_hMailBox, CL_EXPUNGE);
	}

	function Delete($iNumber)
	{
		IMAP_Delete($this->m_hMailBox, $iNumber + 1);
	}

	function Count()
	{
		return IMAP_Num_Msg($this->m_hMailBox);
	}

	function Header($iNumber)
	{
		$aDummy = IMAP_Fetch_Overview($this->m_hMailBox, $iNumber + 1);
		$oHeader = $aDummy[0];

		$aHeader['Subject'] = D($oHeader->subject);
		$aHeader['From'] = D($oHeader->from);
		$aHeader['To'] = D($oHeader->to);
		$aHeader['Date'] = D($oHeader->date);
		$aHeader['MessageID'] = D($oHeader->message_id);
		$aHeader['MD5'] = MD5(Implode('', $aHeader));

		return $aHeader;
	}

	function GetParameter($oPart, $sParameter)
	{
		foreach($oPart->parameters as $oParameter)
			if(StrToUpper($oParameter->attribute) == StrToUpper($sParameter))
				return $oParameter->value;
		return '';
	}

	function Message($iNumber)
	{
		$aMimeType = array('TEXT', 'MULTIPART', 'MESSAGE', 'APPLICATION', 'AUDIO', 'IMAGE', 'VIDEO', 'OTHER'); 
		$oStructure = IMAP_FetchStructure($this->m_hMailBox, $iNumber + 1);

		if(IsSet($oStructure->parts))
			$aParts = $oStructure->parts;
		else
			$aParts[0] = $oStructure;
		$iParts = Count($aParts);

		for($i = 0; $i < $iParts; $i++)
		{
			$aStructure[$i]['Encoding'] = $aParts[$i]->encoding;
			$aStructure[$i]['MimeType'] = StrToUpper($aMimeType[$aParts[$i]->type] . '/' . $aParts[$i]->subtype);
			$aStructure[$i]['CharSet'] = $this->GetParameter($aParts[$i], 'CharSet');
			$aStructure[$i]['FileName'] = $this->GetParameter($aParts[$i], 'Name');

			$sBody = IMAP_FetchBody($this->m_hMailBox, $iNumber + 1, $i + 1);

			switch($aStructure[$i]['Encoding'])
			{
				case 3:
					//Base64
					$aStructure[$i]['Body'] = IMAP_Base64($sBody);
				  break;
				case 4:
					//Quoted Printable
					$aStructure[$i]['Body'] = IMAP_QPrint($sBody);
					break;
				/*
				case 6:
					//UU Encoded
					//$aStructure[$i]['Body'] = UUDecode($sBody);
					$aStructure[$i]['Body'] = $sBody;
				*/
				default:
					$aStructure[$i]['Body'] = $sBody;
					break;
			}
		}

		return $aStructure;
	}


}

/*
$cMail = new CPOPClient;
$cMail->Connect('mail.domain.net', 'name', 'password');

for($i = 0; $i < $cMail->Count(); $i++)
{
	echo '<b>' . $i . '</b>';
	$aHeader = $cMail->Header($i);
	W($aHeader);

	$aParts = $cMail->Message($i);
	for($j = 0; $j < Count($aParts); $j++)
	{
		echo '<b>' . $i . ' - ' . $j . '</b>';

		$sMessage = $aParts[$j]['Body'];
		$aParts[$j]['Body'] = '[...]';

		W($aParts[$j]);
		echo '<pre>' . Chunk_Split(StrPrintable($sMessage)) . '</pre>';
	}
}

$cMail->Close();

*/

function TestClassEMail()
{
	$aTests[] = '$cMail = new CSMTPClient; return TRUE;';
	$aTests[] = '$cMail = new CPOPClient; return TRUE;';
	$aTests[] = 'return ValidEMail("thomas.stauffer@deepsource.ch");';
	$aTests[] = 'return ValidEMail("thomas.stauffer@deepsource.net.ch");';
	$aTests[] = 'return ValidEMail("thomas.stauffer@deepsource.c") == FALSE;';
	$aTests[] = 'return ValidEMail("thomas.stauffer@deepsource.chxx") == FALSE;';
	$aTests[] = 'return ValidEMail("thomas.stauffer-deepsource.ch") == FALSE;';
	$aTests[] = 'return ValidEMail("thomas-stauffer@deepsource.ch");';
	$aTests[] = 'return ValidEMail("x@x.xx");';
	$aTests[] = 'return ValidEMail("0@0.xx");';
	$aTests[] = 'return ValidEMail("0@0.0x") == FALSE;';
	$aTests[] = 'return ValidEMail("ä@x.xx") == FALSE;';

	return Test('ClassEMail', $aTests);
}

?>