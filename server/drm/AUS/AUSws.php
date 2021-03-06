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

//AUS Web Service


require_once ("Zend/Db.php");
require_once ("Zend/Config/Ini.php");
require_once 'Zend/Log.php';
require_once 'Zend/Log/Writer/Stream.php';
include_once("AUS_Component.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS["LIB_DIR"] . "/common/RSASignature.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS["LIB_DIR"] . "/common/Logger.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS["INSTALLATION_DIR"] . "/AUS/admin/business/AUS_Administrator.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS["INSTALLATION_DIR"] . "/AUS/AUS_User.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS["LIB_DIR"] . "/common/openssl/PrivateKey.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS["LIB_DIR"] . "/common/openssl/X509Certificate.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS["LIB_DIR"] . "/common/openssl/RSADSig.class.php");
include_once($_SERVER["DOCUMENT_ROOT"] . $GLOBALS["LIB_DIR"] . "/common/Logger.class.php");

class AUSws {

    private $dbcfg;
    private $l;

    function __construct() {
        $this->dbcfg = new Zend_Config_Ini(getenv("DOCUMENT_ROOT") . '/config.ini', 'sig.modeve');
        
        $writer = new Zend_Log_Writer_Stream('./aus.log');
        $this->l = new Zend_Log($writer);
        
        $this->l->info('In constructor of the AUSws class');
    }

    /**
     * Description: This function is used to modify the user proporties on the registration server.
     * @param message_type The message type.
     * @param identification The identification of the message sender.
     * @param signature_algorithm The identifier of the signature algorithm used to sign the message.
     * @param name The name of the user to be registered.
     * @param address The address of the user to be registered.
     * @param email_address The email_address of the user to be registered.
     * @param authentication_type The authentication type of the user to be registered.
     * @param uid The user identification of the user to be registered.
     * @param username The username.
     * @param password The password.
     * @param arbitrary_data Some additional data used to identify the user.
     * @param signature The signature of the message.
     * @return array The resulting array of the function call.
     */
    public function AUSrequestModifyUserSubscription($identification, $signature_algorithm_identifier, $name, $address, $email_address, $authentication_type, $uid, $username, $password, $arbitrary_data, $signature) {
        //$l = new Logger();
        //$l->Log("AUS:AUS.ws.php:AUSrequestModifyUserSubscription");

        if ($authentication_type == "LOGINPASSWORD") {
            //verify data signature and check if it is correct and valid
            //we join all the data in the message in order to verify that the signature is valid
            $uuid = sprintf("%08s", $uid);

            if ($GLOBALS['SECURITY_VAR'] == "ON") {
                $data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $name, $address, $email_address, $authentication_type, $uuid, $username, $password, $arbitrary_data));
            }

            // the WS verifies the signature contained in the message
            // using the $identification field as a key retrieves the appropriate public-key
            // WS gets the public-key
            $comp = new AUS_Component();
            $comp->loadAllInformation($identification);

            if ($GLOBALS['SECURITY_VAR'] == "ON") {
                $sign = new RSASignature();
                $value = $sign->RSA_verify($data_to_verify, $signature, $comp->pubkey);
            } else {
                $value = true;
            }

            if ($value == false) {
                return array(
                    "result_message" => "-ERR: Invalid signature..."
                );
            } else {
                $user = new AUS_User();
                //modifyUser($uid, $username, $password, $name, $address, $email_address)
                if ($user->modifyUser($uuid, $username, $password, $name, $address, $email_address) == false) {
                    return array(
                        "result_message" => "-ERR: Error in database: : $user->db_con->returnError()"
                    );
                } else {
                    // TODO: we should verify if the user exists or not...
                    // TODO: we should also verify if some of the fields is not correct
                    return array(
                        "result_message" => "+OK"
                    );
                }
            }
        } else if ($authentication_type == "PUBLICKEY") {
            //verify data signature and check if it is correct and valid
            //we join all the data in the message in order to verify that the signature is valid
            $uuid = sprintf("%08s", $uid);

            if ($GLOBALS['SECURITY_VAR'] == "ON") {
                $data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $name, $address, $email_address, $authentication_type, $uuid, $username, $password, $arbitrary_data));
            }

            // the WS verifies the signature contained in the message
            // using the $identification field as a key retrieves the appropriate public-key
            // WS gets the public-key
            $comp = new AUS_Component();
            $comp->loadAllInformation($identification);

            if ($GLOBALS['SECURITY_VAR'] == "ON") {
                $sign = new RSASignature();
                $value = $sign->RSA_verify($data_to_verify, $signature, $comp->pubkey);
            } else {
                $value = true;
            }

            if ($value == false) {
                return array(
                    "result_message" => "-ERR: Invalid signature..."
                );
            } else {
                $user = new AUS_User();
                $ret = $user->modifyUser($uuid, $username, $password, $name, $address, $email_address, $authentication_type, $arbitrary_data);
                // TODO: we should verify if the user exists or not...
                // TODO: we should also verify if some of the fields is not correct
                return array(
                    "result_message" => "+OK"
                );
            }
        }
    }

    /**
     * Description: This function is used to process the authentication validation of a user.
     * @param message_type The message type.
     * @param identification The identification of the message sender.
     * @param signature_algorithm The identifier of the signature algorithm used to sign the message.
     * @param authentication_type The authentication type of the user to be registered.
     * @param username The username.
     * @param password The password.
     * @param signature The signature of the message.
     * @return array The resulting array of the function call.
     */
    public function AUSrequestAuthentication($identification, $signature_algorithm_identifier, $authentication_type, $username, $password, $signature) {
        //$l = new Logger();
        //$l->Log("AUS:AUS.ws.php:AUSrequestAuthentication");

        if ($authentication_type == "LOGINPASSWORD") {
            //verify data signature and check if it is correct and valid
            //we join all the data in the message in order to verify that the signature is valid

            if ($GLOBALS['SECURITY_VAR'] == "ON") {
                $data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $authentication_type, $username, $password));
            }

            // the WS verifies the signature contained in the message
            // using the $identification field as a key retrieves the appropriate public-key
            // WS gets the public-key
            $comp = new AUS_Component();
            $comp->loadAllInformation($identification);

            if ($GLOBALS['SECURITY_VAR'] == "ON") {
                $sign = new RSASignature();
                $value = $sign->RSA_verify($data_to_verify, $signature, $comp->pubkey);
            } else {
                $value = true;
            }

            if ($value == false) {
                return array(
                    "result_message" => "-ERR: Invalid signature..."
                );
            } else {
                $user = new AUS_User();
                if ($user->verifyUser($username, $password) == false) {
                    return array(
                        "result_message" => "-ERR: Invalid user..."
                    );
                } else {
                    $user->loadAllInformation($username, $password);
                    return array(
                        "result_message" => "+OK",
                        "user_id" => $user->uid
                    );
                }
            }
        } else if ($authentication_type == "PUBLICKEY") {
            //verify data signature and check if it is correct and valid
            //we join all the data in the message in order to verify that the signature is valid
            if ($GLOBALS['SECURITY_VAR'] == "ON") {
                $data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $authentication_type, $username, $password));
            }
            //echo $data_to_verify."<br>";
            // the WS verifies the signature contained in the message
            // using the $identification field as a key retrieves the appropriate public-key
            // WS gets the public-key
            $comp = new AUS_Component();
            $comp->loadAllInformation($identification);

            if ($GLOBALS['SECURITY_VAR'] == "ON") {
                $sign = new RSASignature();
                $value = $sign->RSA_verify($data_to_verify, $signature, $comp->pubkey);
            } else {
                $value = true;
            }

            if ($value == false) {
                return array(
                    "result_message" => "-ERR: Invalid signature...",
                    "data_signed" => $data_to_verify,
                    "signature" => $value,
                    "pubkey" => $comp->pubkey->returnXMLkey()
                );
            } else {
                $user = new AUS_User();
                if ($user->verifyUser($username, $password) == false) {
                    return array(
                        "result_message" => "-ERR: Invalid user..."
                    );
                } else {
                    return array(
                        "result_message" => "+OK"
                    );
                }
            }
        } else if ($authentication_type == "WALLET") {
            //verify data signature and check if it is correct and valid
            //we join all the data in the message in order to verify that the signature is valid
            //$data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $authentication_type, $username, $password));
            //echo "WS data to verify: ".$data_to_verify."<br>";
            // the WS verifies the signature contained in the message
            // using the $identification field as a key retrieves the appropriate public-key
            // WS gets the public-key
            //$usr = new AUS_User();
            //$usr->loadAllInformation($username, $password);
            //echo "USR PUB KEY: ".$usr->pubkey."<br>";
            //echo "SIG: ".$signature;
            //$sign = new RSASignature();
            //$value = $sign->RSA_verify($data_to_verify, $signature, $usr->pubkey);
            //if($value==false) {
            //	return array(
            //		"message_type" => "AUTHENTICATION_REQUEST_RESULT",
            //		"result_message" => "-ERR: Invalid signature..."
            //		);
            //} else {
            $user = new AUS_User();
            if ($user->verifyUser($username, $password) == false) {
                return array(
                    "result_message" => "-ERR: Invalid user..."
                );
            } else {
                return array(
                    "result_message" => "+OK"
                );
            }
        } else {
            return array(
                "result_message" => "-ERR:  Authentication not VALID."
            );
        }
        return array(
            "result_message" => "-ERR: Unreachable control point."
        );
    }

    /**
     * Description: This function is used to register a new software component on the server.
     * @param message_type The message type.
     * @param arbitrary_data Some arbitraty data that might be used to identify the component.
     * @param public_key The public key of the component.
     * @param password The password.
     * @version 20070518
     * @return array The resulting array of the function call.
     */
    public function AUSrequestComponentSubscription($arbitrary_data, $csr, $password) {
        $this->l->info("AUS:AUS.ws.php:AUSrequestComponentSubscription");
        // verify if public_key or password is empty
        if ($csr == "") {
            return array(
                "result_message" => "-ERR: CSR cannot be empty."
            );
        }
        if ($password == "") {
            return array(
                "result_message" => "-ERR: Password cannot be empty."
            );
        }

        $comp = new AUS_Component();
        $comp->set($arbitrary_data, $csr, $password);
        if ($comp->createCertificate() == false) {
            return array(
                "result_message" => "-ERR: Error while creating the certificate... why?"
            );
        }

        // registers the new component on the database
        if ($comp->saveAllInformation() == -1) {
            return array(
                "result_message" => "-ERR: Already exists a user with the same id..."
            );
        }

        return array(
            "result_message" => "+OK",
            "certificate" => $comp->cert
        );
    }

    /**
     * Description: This function is used to delete a user.
     * @param message_type The message type.
     * @param identification The identification of the message sender.
     * @param signature_algorithm The identifier of the signature algorithm used to sign the message.
     * @param uid The user identification of the user to be registered.
     * @param username The username.
     * @param password The password.
     * @param arbitrary_data Some additional data used to identify the user.
     * @param signature The signature of the message.
     * @return array The resulting array of the function call.
     */
    public function AUSrequestDeleteUserSubscription($identification, $signature_algorithm_identifier, $uid, $username, $password, $arbitrary_data, $signature) {
        //$l = new Logger();
        //$l->Log("AUS:AUS.ws.php:AUSrequestDeleteUserSubscription");
        //verify data signature and check if it is correct and valid
        //we join all the data in the message in order to verify that the signature is valid
        $uuid = sprintf("%08s", $uid);

        if ($GLOBALS['SECURITY_VAR'] == "ON") {
            $data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $uuid, $username, $password, $arbitrary_data));
        }

        // verify if uid, username, or password are empty
        if ($uid == "") {
            return array(
                "result_message" => "-ERR: UserId cannot be empty."
            );
        }
        if ($username == "") {
            return array(
                "result_message" => "-ERR: Username cannot be empty."
            );
        }
        if ($password == "") {
            return array(
                "result_message" => "-ERR:  Password cannot be empty."
            );
        }

        // the WS verifies the signature contained in the message
        // using the $identification field as a key retrieves the appropriate public-key
        // WS gets the public-key
        $comp = new AUS_Component();
        $comp->loadAllInformation($identification);

        if ($GLOBALS['SECURITY_VAR'] == "ON") {
            $sign = new RSASignature();
            $value = $sign->RSA_verify($data_to_verify, $signature, $comp->pubkey);
        } else {
            $value = true;
        }

        if ($value == false) {
            return array(
                "result_message" => "-ERR: Invalid signature..."
            );
        } else {
            $user = new AUS_User();
            $user->deleteUser($uuid, $username, $password);
            // TODO: Verify if the User really exists
            return array(
                "result_message" => "+OK"
            );
        }
    }

    /**
     * Description: This function is used to register a new user on the system.
     * @param message_type The message type.
     * @param identification The identification of the message sender.
     * @param signature_algorithm The identifier of the signature algorithm used to sign the message.
     * @param name The name of the user to be registered.
     * @param address The address of the user to be registered.
     * @param email_address The email_address of the user to be registered.
     * @param authentication_type The authentication type of the user to be registered.
     * @param username The username.
     * @param password The password.
     * @param public_key The user public-key.
     * @param arbitrary_data Some additional data used to identify the user.
     * @param signature The signature of the message.
     * @return array The resulting array of the function call.
     */
    public function AUSrequestUserSubscription($identification, $signature_algorithm_identifier, $name, $address, $email_address, $authentication_type, $username, $password, $public_key, $arbitrary_data, $signature) {
        //$l = new Logger();
        //$l->Log("AUS:AUS.ws.php:AUSrequestUserSubscription");

        if ($authentication_type == "LOGINPASSWORD") {
            // verify if username, password are empty
            if ($username == "") {
                return array(
                    "result_message" => "-ERR: Username canot be empty."
                );
            }
            if ($password == "") {
                return array(
                    "result_message" => "-ERR: Password canot be empty."
                );
            }

            if (stristr($arbitrary_data, "WALLET") == false) {

                //verify data signature and check if it is correct and valid
                //we join all the data in the message in order to verify that the signature is valid
                if ($GLOBALS['SECURITY_VAR'] == "ON") {
                    $data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $name, $address, $email_address, $authentication_type, $username, $password, $public_key, $arbitrary_data));
                }
                // the WS verifies the signature contained in the message
                // using the $identification field as a key retrieves the appropriate public-key
                // WS gets the public-key
                $comp = new AUS_Component();
                $comp->loadAllInformation($identification);

                if ($GLOBALS['SECURITY_VAR'] == "ON") {
                    $sign = new RSASignature();
                    $value = $sign->RSA_verify($data_to_verify, $signature, $comp->pubkey);
                } else {
                    $value = true;
                }

                if ($value == false) {
                    return array(
                        "identification" => $identification,
                        "imploded_data" => $data_to_verify,
                        "result_message" => "-ERR: Invalid signature..."
                    );
                } else {
                    $user = new AUS_User();
                    $user->set($username, $password, $address, $email_address, $authentication_type, $arbitrary_data, $public_key, $name);

                    // $user->createCertificate();

                    if ($user->verifyUser($username, $password) == true) {
                        return array(
                            "result_message" => "-ERR: User already exists..."
                        );
                    }

                    if ($user->saveAllInformation() == false) {
                        return array(
                            "result_message" => "-ERR: User already exists..."
                        );
                    } else {
                        return array(
                            "result_message" => "+OK",
                            "user_id" => $user->uid
                        );
                    }
                }
            } else if (stristr($arbitrary_data, "WALLET") != false) {
                $user = new AUS_User();
                $user->set($username, $password, $address, $email_address, $authentication_type, $arbitrary_data, $public_key, $name);

                // $user->createCertificate();

                if ($user->verifyUser($username, $password) == true) {
                    return array(
                        "result_message" => "-ERR: User already exists..."
                    );
                }

                if ($user->saveAllInformation() == false) {
                    return array(
                        "result_message" => "-ERR: User already exists..."
                    );
                } else {
                    return array(
                        "result_message" => "+OK",
                        "user_id" => $user->uid
                    );
                }
            }
        } else if ($authentication_type == "PUBLICKEY") {
            //verify data signature and check if it is correct and valid
            //we join all the data in the message in order to verify that the signature is valid
            if ($GLOBALS['SECURITY_VAR'] == "ON") {
                $data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $name, $address, $email_address, $authentication_type, $username, $password, $public_key, $arbitrary_data));
            }

            // verify if username, password, or public-key are empty
            if ($username == "") {
                return array(
                    "result_message" => "-ERR: Username cannot be empty."
                );
            }
            if ($password == "") {
                return array(
                    "result_message" => "-ERR: Password cannot be empty."
                );
            }
            if ($public_key == "") {
                if ($GLOBALS['SECURITY_VAR'] == "ON") {
                    return array(
                        "result_message" => "-ERR:  Public Key cannot be empty."
                    );
                }
            }

            if (stristr($arbitrary_data, "WALLET") == false) {
                // the WS verifies the signature contained in the message
                // using the $identification field as a key retrieves the appropriate public-key
                // WS gets the public-key
                $comp = new AUS_Component();
                $comp->loadAllInformation($identification);

                if ($GLOBALS['SECURITY_VAR'] == "ON") {
                    $sign = new RSASignature();
                    $value = $sign->RSA_verify($data_to_verify, $signature, $comp->pubkey);
                } else {
                    $value = true;
                }

                if ($value == false) {
                    return array(
                        "result_message" => "-ERR: Invalid signature..."
                    );
                } else {
                    $user = new AUS_User();
                    $user->set($username, $password, $address, $email_address, $authentication_type, $arbitrary_data, $public_key, $name);
                    // TODO: check if this user already exists
                    //verify a duplicate user
                    //check on the database if there is another repetition of the username, password and key
                    //to remember: a user might have multiple keys (for simplifying a user as just one key)

                    $user->createCertificate();
                    //the user data is ok, so generate a public-key certificate for this user
                    //store the certificate on the database, also
                    //return all the data to the user
                    if ($user->saveAllInformation() == false) {
                        return array(
                            "result_message" => "-ERR: Error while inserting on the database."
                        );
                    }
                    // TODO: verify return value
                    // TODO: return the certificate also
                    return array(
                        "result_message" => "+OK",
                        "user_id" => $user->cert
                    );
                }
            } else if (stristr($arbitrary_data, "WALLET") != false) {
                $user = new AUS_User();
                $user->set($username, $password, $address, $email_address, $authentication_type, $arbitrary_data, $public_key, $name);
                // TODO: check if this user already exists
                //verify a duplicate user
                //check on the database if there is another repetition of the username, password and key
                //to remember: a user might have multiple keys (for simplifying a user as just one key)

                $user->createCertificate();

                //the user data is ok, so generate a public-key certificate for this user
                //store the certificate on the database, also
                //return all the data to the user
                if ($user->saveAllInformation() == false) {
                    return array(
                        "result_message" => "-ERR: Error while inserting on the database: $user->db_con->returnError()"
                    );
                }
                // TODO: verify return value
                // TODO: return the certificate also
                return array(
                    "result_message" => "+OK",
                    "user_id" => $user->cert
                );
            }
            return array(
                "result_message" => "-ERR: Unreachable control point."
            );
        } else {
            return array(
                "result_message" => "-ERR:  Authentication not VALID."
            );
        }

        return array(
            "result_message" => "-ERR: Unreachable control point."
        );
    }

    /**
     * Description: This function is used to get information about a user.
     * @param message_type The message type.
     * @param identification The identification of the message sender.
     * @param signature_algorithm The identifier of the signature algorithm used to sign the message.
     * @param uid This is the identifier of the user.
     * @param username The username.
     * @param password The password.
     * @param signature The signature of the message.
     * @return array The resulting array of the function call.
     */
    /*
      <message name=" AUSrequestUserPaymentInfo ">
      <part name="identification" type="xsd1:identification"/>
      <part name="signature-algorithm-identifier" type="xsd1:signature-algorithm-identifier"/>
      <part name="pgw_identification" type="xsd1:identification"/>
      <part name="user_identification" type="xsd1:identification"/>
      <part name="transaction_identification" type="xsd1:identification"/>
      <part name="value" type="xsd1:identification"/>
      <part name="signature" type="xsd1:identification"/>
      </message>
      <message name=" AUSrequestUserPaymentInfoResult ">
      <part name="identification" type="xsd1:identification"/>
      <part name="signature-algorithm-identifier" type="xsd1:signature-algorithm-identifier"/>
      <part name="clearence" type="xsd1:identification"/>
      <part name="signature" type="xsd1:identification"/>
      </message>
     */
    public function AUSrequestUserPaymentInfo($identification, $signature_algorithm_identifier, $pgw_identification, $user_identification, $pvalue, $signature) {
        $l = new Logger();
        $l->Log("AUS:AUS.ws.php:AUSrequestUserPaymentInfo");
        $l->Log("AUS:AUS.ws.php:AUSrequestUserPaymentInfo:identification:" . $identification);
        //verify data signature and check if it is correct and valid
        //we join all the data in the message in order to verify that the signature is valid
        //-----------------------
        //cjcs: 20030721 - 19:30
        //This change had to be made in order to have a user id with 8 characters
        //-----------------------
        $uid = sprintf("%08s", $user_identification);
        if ($GLOBALS['SECURITY_VAR'] == "ON") {
            $data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $pgw_identification, $uid, $pvalue));
        }
        //$l->Log("AUS:AUS.ws.php:AUSrequestUserPaymentInfo:data_to_verify:".$data_to_verify);
        // the WS verifies the signature contained in the message
        // using the $identification field as a key retrieves the appropriate public-key
        // WS gets the public-key
        $comp = new AUS_Component();
        $comp->loadAllInformation($identification);

        if ($GLOBALS['SECURITY_VAR'] == "ON") {
            $sign = new RSASignature();
            $value = $sign->RSA_verify($data_to_verify, $signature, $comp->pubkey);
        } else {
            $value = true;
        }
        if ($value == false) {
            return array(
                "result_message" => "-ERR: Invalid signature..."
            );
        } else {
            // get Data from user and prepare it to be sent to the PGW (encrypt with PGW publickey)
            $admin = new AUS_Administrator();
            if (($data_for_pgw = $admin->prepareDataForPGW($pgw_identification, $uid, $pvalue)) == false) {
                return array(
                    "result_message" => "-ERR: Error while preparing data for the PGW. Probably this User doesn't exist..."
                );
            } else {
                return array(
                    "result_message" => "+OK",
                    "payclearer" => $data_for_pgw
                );
            }
        }
    }

    /*
      $parameters = array(
      "identification" => md5($this->epubk),
      "signature_algorithm_identifier" => "RSA_WITH_MD5",
      "authentication_type" => "PUBLICKEY",
      "uid" => md5($this->epubk),
      "username" => $this->username,
      "password" => $this->password,
      "signature" => $signature
      );
     */

    public function AUSrequestUserInfo($identification, $signature_algorithm_identifier, $authentication_type, $uid, $username, $password, $signature) {
        //$l = new Logger();
        //$l->Log("AUS:AUS.ws.php:AUSrequestUserInfo");
        if ($authentication_type == "LOGINPASSWORD") {
            //verify data signature and check if it is correct and valid
            //we join all the data in the message in order to verify that the signature is valid
            $uuid = sprintf("%08s", $uid);
            if ($GLOBALS['SECURITY_VAR'] == "ON") {
                $data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $authentication_type, $uuid, $username, $password));
            }

            // the WS verifies the signature contained in the message
            // using the $identification field as a key retrieves the appropriate public-key
            // WS gets the public-key
            $comp = new AUS_Component();
            $comp->loadAllInformation($identification);

            if ($GLOBALS['SECURITY_VAR'] == "ON") {
                $sign = new RSASignature();
                $value = $sign->RSA_verify($data_to_verify, $signature, $comp->pubkey);
            } else {
                $value = true;
            }

            if ($value == false) {
                return array(
                    "result_message" => "-ERR: Invalid signature..."
                );
            } else {
                $user = new AUS_User();
                $user->loadAllInformation($username, $password);

                return array(
                    "result_message" => "+OK",
                    "uid" => "" . $user->uid,
                    "name" => "" . $user->name,
                    "address" => "" . $user->address,
                    "email" => "" . $user->email_address,
                    "other_data_xml" => "" . $user->other_data,
                    "certificate" => "" . $user->cert
                );
            }
        } else if ($authentication_type == "PUBLICKEY") {
            //verify data signature and check if it is correct and valid
            //we join all the data in the message in order to verify that the signature is valid
            $uuid = sprintf("%08s", $uid);
            if ($GLOBALS['SECURITY_VAR'] == "ON") {
                $data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $authentication_type, $uuid, $username, $password));
            }

            // the WS verifies the signature contained in the message
            // using the $identification field as a key retrieves the appropriate public-key
            // WS gets the public-key
            $comp = new AUS_Component();
            $comp->loadAllInformation($identification);

            if ($GLOBALS['SECURITY_VAR'] == "ON") {
                $sign = new RSASignature();
                $value = $sign->RSA_verify($data_to_verify, $signature, $comp->pubkey);
            } else {
                $value = true;
            }

            if ($value == false) {
                return array(
                    "result_message" => "-ERR: Invalid signature..."
                );
            } else {
                $user = new AUS_User();
                $user->loadAllInformation($username, $password);

                return array(
                    "result_message" => "+OK",
                    "uid" => "" . $user->uid,
                    "name" => "" . $user->name,
                    "address" => "" . $user->address,
                    "email" => "" . $user->email_address,
                    "other_data_xml" => "" . $user->other_data,
                    "certificate" => "" . $user->cert
                );
            }
        }
    }

    public function AUSrequestListOfPGW($identification, $signature_algorithm_identifier, $authentication_type, $signature) {
        //gets the list of registered Payment Gateways and returns them to the user
        //verify data signature and check if it is correct and valid
        //we join all the data in the message in order to verify that the signature is valid
        //$l = new Logger();
        //$l->Log("AUS:AUS.ws.php:AUSrequestListOfPGW");

        if ($GLOBALS['SECURITY_VAR'] == "ON") {
            $data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $authentication_type));
        }

        //echo $data_to_verify."<br>";
        // the WS verifies the signature contained in the message
        // using the $identification field as a key retrieves the appropriate public-key
        // WS gets the public-key
        $comp = new AUS_Component();
        $comp->loadAllInformation($identification);

        // TODO: MAYBE WE NEED TO VERIFY IF THE REQUEST REALLY ARRIVES FROM A COS

        if ($GLOBALS['SECURITY_VAR'] == "ON") {
            $sign = new RSASignature();
            $value = $sign->RSA_verify($data_to_verify, $signature, $comp->pubkey);
        } else {
            $value = true;
        }

        if ($value == false) {
            return array(
                "result_message" => "-ERR: Invalid signature..."
            );
        } else {
            // MESSAGE IS VALID, LISTING AVAILABLE PGWs
            $admin = new AUS_Administrator();
            if (($pgws = $admin->listAvailablePGW()) == false) {
                return array(
                    "result_message" => "-ERR: There are not any available PGWs at the moment..."
                );
            } else {
                return array(
                    "result_message" => "+OK",
                    "list_of_pgw" => $pgws
                );
            }
        }
    }

    public function AUSrequestListOfPGW_($identification, $signature) {
        //gets the list of registered Payment Gateways and returns them to the user
        //verify data signature and check if it is correct and valid
        //we join all the data in the message in order to verify that the signature is valid
        $l = new Logger();
        $l->Log("AUS:AUS.ws.php:AUSrequestListOfPGW_");

        /* if($GLOBALS['SECURITY_VAR']=="ON") {
          $data_to_verify = implode("", array($identification, $signature_algorithm_identifier, $authentication_type));
          } */

        //echo $data_to_verify."<br>";
        // the WS verifies the signature contained in the message
        // using the $identification field as a key retrieves the appropriate public-key
        // WS gets the public-key
        $comp = new AUS_Component();
        $comp->loadAllInformation_($identification);

        // TODO: MAYBE WE NEED TO VERIFY IF THE REQUEST REALLY ARRIVES FROM A COS

        /* if($GLOBALS['SECURITY_VAR']=="ON") {
          $sign = new RSASignature();
          $value = $sign->RSA_verify($data_to_verify, $signature, $comp->pubkey);
          } else {
          $value=true;
          } */

        $sign = new RSADSig();
        $certtouse = new X509Certificate();
        $l->Log($comp->cert);
        $certtouse->set($comp->cert);
        $value = $sign->verify($identification, base64_decode($signature), $certtouse);

        if ($value == false) {
            return array(
                "result_message" => "-ERR: Invalid signature..."
            );
        } else {
            // MESSAGE IS VALID, LISTING AVAILABLE PGWs
            $admin = new AUS_Administrator();
            if (($pgws = $admin->listAvailablePGW()) == false) {
                return array(
                    "result_message" => "-ERR: There are not any available PGWs at the moment..."
                );
            } else {
                return array(
                    "result_message" => "+OK",
                    "list_of_pgw" => $pgws
                );
            }
        }
    }

    /* This interface was added due to a necessity identified during the Lisbon integration meeting due to the fact the
     * M4U and CPS need to share users :)) */

    public function AUSrequestMutualValidation($uid, $login, $hash) {
        //$log = new Logger();
        //$log->Log("AUS:AUS.ws.php:AUSrequestMutualValidation");

        $uuid = sprintf("%08s", $uid);
        $admin = new AUS_Administrator();
        $value = $admin->specialVerifyUser($uuid, $login, $hash);
        if ($value == false) {
            /* return array (
              "result_message" => "-ERR: Invalid user..."
              ); */
            return "-ERR: Invalid user...";
        } else if ($value == true) {
            /* return array (
              "result_message" => "+OK"
              ); */
            return "+OK";
        }
    }

    /* This interface is used to verify either a user has already been registed or not via-wallet */

    public function AUSrequestWalletVerification($uid) {
        $uuid = sprintf("%08s", $uid);
        $admin = new AUS_Administrator();
        $value = $admin->checkWallet($uuid);
        if ($value == false) {
            return "-ERR: No Wallet...";
            /* return array (
              "result_message" => "-ERR: No Wallet..."
              ); */
        } else if ($value == true) {
            return "+OK";
            /* return array (
              "result_message" => "+OK"
              ); */
        }
    }

}

class AUSwsException extends Exception {

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
