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
 _     _               _  _____  _____  _____  _____
| |   (_)             | |/ __  \|  _  ||  _  ||  _  |
| |__  _  ___ ___   __| |`' / /'| |/' || |/' || |/' |
| '_ \| |/ __/ _ \ / _` |  / /  |  /| ||  /| ||  /| |
| | | | | (_| (_) | (_| |./ /___\ |_/ /\ |_/ /\ |_/ /
|_| |_|_|\___\___/ \__,_|\_____/ \___/  \___/  \___/

+-+-+-+-+-+-+-+-+
|b|l|a|c|k|b|o|x|
+-+-+-+-+-+-+-+-+
*/

include_once("osdrmIntegrator.php");

/* register the content on the registration server */
echo 'file = '.$_REQUEST['file'];

if(($res=registerNewContent($_REQUEST['file'].".jp2", ""))==true) {
	$cmd = "jpsec.exe -i ".$_REQUEST['file'].".jp2 -o ".$_REQUEST['file']."_enc.jp2 -k ".$_REQUEST['file'].".keys -urnp ".$res;

    // create file with the id
    $idh = fopen($_REQUEST['file'].".cid", "w");
    fwrite($idh, $res);
    fclose($idh);

	// create the keys
    $khandle = fopen($_REQUEST['file'].".keys", "w");

    for($n=0; $n<6; $n++)
        fwrite($khandle, substr(md5((time()*$n)),0, 16)."\n");

    fclose($khandle);

    echo $cmd."\n";
	system($cmd);

    if(($rs=registerContentKeys($res, $_REQUEST['file'].".keys"))) {
        $fp = fopen($_REQUEST['file'].".ok", "w");
        fclose($fp);
    } else {
        $fp = fopen($_REQUEST['file'].".ko", "w");
        fclose($fp);
    }

} else {
    $fp = fopen($_REQUEST['file'].".ok", "w");
    fclose($fp);
}
?>