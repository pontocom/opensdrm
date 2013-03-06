<?php
require_once 'Zend/Rest/Server.php';
require_once './admin/business/CFSwebserv.class.php';

$server = new Zend_Rest_Server();
$server->setClass('CFSwebsrv');
$server->handle();
?>