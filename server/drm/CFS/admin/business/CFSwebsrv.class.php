<?php

require_once ("Zend/Db.php");
require_once ("Zend/Config/Ini.php");
require_once 'Zend/Log.php';
require_once 'Zend/Log/Writer/Stream.php';

class CFSwebsrv {

    private $dbcfg;
    private $l;

    /**
     *
     */
    function __construct() {
        $this->dbcfg = new Zend_Config_Ini(getenv("DOCUMENT_ROOT") . '/config.ini', 'opensdrm.cfs');

        $writer = new Zend_Log_Writer_Stream('./cfs.log');
        $this->l = new Zend_Log($writer);

        $this->l->info('In constructor of the CFSwebsrv class');
    }

    public function Ping($name) {
        return array("msg" => "+OK",
            "msg_out" => "Service is alive. The name is: " . $name);
    }

    public function CFSrequestServerLocation($id) {
        $this->l->info("CFS:CFSrequestServerLocation");
        $this->l->info("$id");

        $cfs = new CFS_Administrator();
        $msg = $cfs->getServerLocation($id);

        $this->l->info("$msg");

        if ($msg[0] == '-') {
            return array(
                "result_message" => "-ERR: location not found..."
            );
        } else {
            return array(
                "result_message" => "+OK",
                "location" => $msg
            );
        }
    }


    public function CFSrequestLocationStorage($id, $location) {
        $this->l->info("CFS:CFSrequestLocationStorage");
        $this->l->info("$id");
        $this->l->info("$location");

        $cfs = new CFS_Administrator();
        $msg = $cfs->addServer($id, $location);

        $this->l->info("$msg");

        if ($msg[0] == '-') {
            return array(
                "result_message" => "-ERR: location not added..."
            );
        } else {
            return array(
                "result_message" => "+OK"
            );
        }
    }


    public function CFSrequestLocationDelete($id) {
        $this->l->info("CFS:CFSrequestLocationDelete");
        $this->l->info("$id");

        $cfs = new CFS_Administrator();
        $msg = $cfs->deleteServer($id);

        $this->l->info("$msg");

        if ($msg[0] == '-') {
            return array(
                "result_message" => "-ERR: location not deleted..."
            );
        } else {
            return array(
                "result_message" => "+OK"
            );
        }
    }
}

class CFSwebsrvException extends Exception {

    // Redefine the exception so message isn't optional
    public function __construct($message, $code = 0) {
        // some code
        // make sure everything is assigned properly
        parent::__construct($message, $code);
    }

    // custom string representation of object
    public function __toString() {
        return __CLASS__ . ": [{$this->code}]: {$this->message}";
    }

}

?>
