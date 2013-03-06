<?php
require_once 'Zend/Rest/Server.php';
require_once 'AUSws.php';

$server = new Zend_Rest_Server();
$server->setClass('AUSws');
$server->handle();
?>