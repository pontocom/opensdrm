<?php
/*
 OpenSDRM - Open Secure Digital Rights Management
 Copyright (c) 2002 Adetti

 This software was originaly developed under the MOSES project (IST-2001-34144)

________                         _________________ __________    _____
\_____  \ ______   ____   ____  /   _____/\______ \\______   \  /     \
 /   |   \\____ \_/ __ \ /    \ \_____  \  |    |  \|       _/ /  \ /  \
/    |    \  |_> >  ___/|   |  \/        \ |    `   \    |   \/    Y    \
\_______  /   __/ \___  >___|  /_______  //_______  /____|_  /\____|__  /
        \/|__|        \/     \/        \/         \/       \/         \/

This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 2.1 of the License, or (at your option)
any later version.

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License along
with this library; if not, write to the Free Software Foundation, Inc., 59
Temple Place, Suite 330, Boston, MA 02111-1307 USA


*/
/**
/**
 _     _               _  _____  _____  _____  _____
| |   (_)             | |/ __  \|  _  ||  _  ||  _  |
| |__  _  ___ ___   __| |`' / /'| |/' || |/' || |/' |
| '_ \| |/ __/ _ \ / _` |  / /  |  /| ||  /| ||  /| |
| | | | | (_| (_) | (_| |./ /___\ |_/ /\ |_/ /\ |_/ /
|_| |_|_|\___\___/ \__,_|\_____/ \___/  \___/  \___/

+-+-+-+-+-+-+-+-+
|b|l|a|c|k|b|o|x|
+-+-+-+-+-+-+-+-+

OpenSDRM Integra
This file "glues" the HICOD2000 solution with the OpenSDRM platform from command line
*/
include_once("osdrmIntegrator.php");


/*GETKEY RES_LVL 2 IMG urn:mpeg:mpeg21:diid:doi:pt/adetti/music/00000004*/

/* check the DRM operation to perform */

switch($_REQUEST['op']) {
	case 'REG_CONT': registerNewContent($_REQUEST['filename'], $_REQUEST['additional_data']);
			break;
	case 'KEY_REG': registerContentKey($_REQUEST['cid'], $_REQUEST['key']);
			break;
	case 'LIC_PROD': requestLicenseProductionSpecial("00400001", "urn:mpeg:mpeg21:diid:doi:pt/adetti/music/00000001");
			break;
	case 'TL_DLND': requestLicenseDownloadFromWallet("urn:mpeg:mpeg21:diid:doi:pt/adetti/music/00000001");
			break;
	case 'GET_LICW': requestLicenseDownloadFromWallet("urn:mpeg:mpeg21:diid:doi:pt/adetti/music/00000001");
			break;
	case 'USAGE': writeUsage();
			break;
	default: writeUsage();
			echo "No valid options supplied...\nPlease enter a valid option...\n\n";
			break;
}

//echo md5("urn:mpeg:mpeg21:diid:doi:pt/adetti/music/00000001");

?>