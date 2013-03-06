<?php
 
require_once 'Zend/Rest/Client.php';

$client = new Zend_Rest_Client('http://opensdrm.cfs.local/server.cfs.php');
var_dump($client->Ping('Carlos Serrao')->get());
?>
