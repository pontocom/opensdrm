-- phpMyAdmin SQL Dump
-- version 2.6.0-pl2
-- http://www.phpmyadmin.net
-- 
-- Host: localhost
-- Generation Time: Dec 02, 2004 at 03:38 PM
-- Server version: 4.1.7
-- PHP Version: 5.0.2
-- 
-- Database: `ausws_dev`
-- 
CREATE DATABASE `ausws_dev` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE ausws_dev;

-- --------------------------------------------------------

-- 
-- Table structure for table `ausws_admin`
-- 

CREATE TABLE `ausws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `ausws_admin`
-- 

INSERT INTO `ausws_admin` (`login`, `password`, `public_key_xml`, `private_key_xml`, `certificate`) VALUES ('admin', 'admin', '<publicKey><algorithmIdentifier>RSA</algorithmIdentifier><component>db12be99ac97b17b8d3229b352322cf01654709af74aa0c4ba47f0f7dc0901d0d74128651106ecc2a6492c787c01f206f4d8f08624e31003814ea7493a316bf0c7686dc1672dec74b9e77fb9a5a7fb12a8ef7f34b594634abe0793cdc464b7838d7b1d5553cbb23b4fe6e2042a87e5d9da96dcd332ec8631497db07a4e983ee5</component><component>11</component></publicKey>', '<privateKey><algorithmIdentifier>RSA</algorithmIdentifier><component>db12be99ac97b17b8d3229b352322cf01654709af74aa0c4ba47f0f7dc0901d0d74128651106ecc2a6492c787c01f206f4d8f08624e31003814ea7493a316bf0c7686dc1672dec74b9e77fb9a5a7fb12a8ef7f34b594634abe0793cdc464b7838d7b1d5553cbb23b4fe6e2042a87e5d9da96dcd332ec8631497db07a4e983ee5</component><component>11</component><component>53c366fe85c187a02e750ff1bd8ba7c535b6dfc2c7f6e31e0afd63aa105dcbfd07025335b3afd2ff217654c4a7e2a04df434d46f86930da6fcbc30e74af4c762e25c7666ea29dabb5d5796901490e13ebf08001cfe57a22d54da7f3c0b43f03cd3b210e0934852169ba81b1c9b0cb0124ec7245731d5d9858f9f1ce22f43f87f</component><component>e5d66e1779937a13e0d659e73e8e9e9c81cfda1f5c386d8c07d7d93fe07cc0b1a97df8f5deaff39290a9266523afaa235190cef0527f642177efb5709bf166bb</component><component>f402a29cc440ac76d13fc36d4500e7f96e9491184a009c5bd8430b535049f8a8f56682141b4ae7f776e88867e77bf9107fe98774fba04b9ed0153957ad3026df</component></privateKey>', '<certificate><issuerData><identification>cc2f2f40522f523c681afc635745cf17</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>db12be99ac97b17b8d3229b352322cf01654709af74aa0c4ba47f0f7dc0901d0d74128651106ecc2a6492c787c01f206f4d8f08624e31003814ea7493a316bf0c7686dc1672dec74b9e77fb9a5a7fb12a8ef7f34b594634abe0793cdc464b7838d7b1d5553cbb23b4fe6e2042a87e5d9da96dcd332ec8631497db07a4e983ee5</component><component>11</component></publicKey></issuerData><subjectData><identification>cc2f2f40522f523c681afc635745cf17</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>db12be99ac97b17b8d3229b352322cf01654709af74aa0c4ba47f0f7dc0901d0d74128651106ecc2a6492c787c01f206f4d8f08624e31003814ea7493a316bf0c7686dc1672dec74b9e77fb9a5a7fb12a8ef7f34b594634abe0793cdc464b7838d7b1d5553cbb23b4fe6e2042a87e5d9da96dcd332ec8631497db07a4e983ee5</component><component>11</component></publicKey></subjectData><validity><notBefore>17:10:00-20041105</notBefore><notAfter>17:10:00-20051105</notAfter></validity><signature>2c2870a6568ae665b8f5a5bb19a083af441b3a5389db63c5e28d3968105110e75f091bc6867db4892f1c60e4dc177612f7c54ed7a702e2d34a847f31b303f88bc09ab7251c0c54ccb2e71111a314c75d58f9170738abda427e27cd8375bd29143e24dbb8d41c9148b67fa8e01f1e1c50460fc6a5854b36cccf31426690c0286c</signature></certificate>');

-- --------------------------------------------------------

-- 
-- Table structure for table `ausws_component`
-- 

CREATE TABLE `ausws_component` (
  `identification` varchar(32) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `password` varchar(20) NOT NULL default '',
  `certificate_xml` text,
  `other_data_xml` text,
  PRIMARY KEY  (`identification`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `ausws_component`
-- 

INSERT INTO `ausws_component` (`identification`, `public_key_xml`, `password`, `certificate_xml`, `other_data_xml`) VALUES ('aadc9d54f6c5fa45a4fd906b78ae5727', '<publicKey><algorithmIdentifier>RSA</algorithmIdentifier><component>de60ea18905a5035f1b9984857364c9bd830c9925e5f77bbb31e9568a1ff0cc3b85caf4aa46449d75817aef870b5e9022e1821abdcc1880123b0b2a7b73550a5253f8b0eb3b4e35b42d13cf7487433cfb6ca83c9025f985645e442dae401766b76c7534d0962000422e55f7b986c4aff045f3fcb217c7f23018296975c05d30f</component><component>13</component></publicKey>', 'admin', '<certificate><issuerData><identification>553a0c3e15e065827e97be9e37126b94</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>cd3f8b62683016f9c2a85e48d09bcc46bf9a9545223521c85f5fce2b1b7ef48817ca3c180ad84e2ddd357e32819f2c1ce04bb08387e87e6f5c0d8fc2a008784d8bd046b1225c168bad855aa9183d6abd1df58a5cef717cc7db14dfcfcd1a2090f457af00ed9fc8fc50707e35601f5514c7108e277b1de35c26ab108029d44e87</component><component>11</component></publicKey></issuerData><subjectData><identification>aadc9d54f6c5fa45a4fd906b78ae5727</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>de60ea18905a5035f1b9984857364c9bd830c9925e5f77bbb31e9568a1ff0cc3b85caf4aa46449d75817aef870b5e9022e1821abdcc1880123b0b2a7b73550a5253f8b0eb3b4e35b42d13cf7487433cfb6ca83c9025f985645e442dae401766b76c7534d0962000422e55f7b986c4aff045f3fcb217c7f23018296975c05d30f</component><component>13</component></publicKey></subjectData><validity><notBefore>12:53:54-20041108</notBefore><notAfter>12:53:54-20051108</notAfter></validity><signature>35dcdde183b5542c024a2729222b9fe796013303b12d354b0b6852807d36f8ccf5c2f88117842219c4ed4b6dc35980a094b92d17d6e156be4ff0bbd05f6593674e535946c23f1726c4d5992ad56307e1b733745bb39bf45dfa87cf218e5b85358f902e7f1ecff5ca79dc26474247cb968d9ff38228339718501ff6583d3c1f50</signature></certificate>', '<name>Media Delivery Server</name>');
INSERT INTO `ausws_component` (`identification`, `public_key_xml`, `password`, `certificate_xml`, `other_data_xml`) VALUES ('5d164c7633cab663b027bc8e7f8510c1', '<publicKey><algorithmIdentifier>RSA</algorithmIdentifier><component>92bf1fd218f4f3a243439e60b68d05a5a1298e940a0aef2c5c3e9371657635d7aab7604c6dcadebc84300d7f4ba619c7bd871fe3dfa7358918cc5f8c080c5ac660e14115cadf17757a48f82b67d3faebfdbcda576105d465346c67df50ab65dd587990ca51dd1b74e20f6b5e635aa9fbe036af72273f62e9379eac0e63415809</component><component>11</component></publicKey>', 'admin', '<certificate><issuerData><identification>553a0c3e15e065827e97be9e37126b94</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>cd3f8b62683016f9c2a85e48d09bcc46bf9a9545223521c85f5fce2b1b7ef48817ca3c180ad84e2ddd357e32819f2c1ce04bb08387e87e6f5c0d8fc2a008784d8bd046b1225c168bad855aa9183d6abd1df58a5cef717cc7db14dfcfcd1a2090f457af00ed9fc8fc50707e35601f5514c7108e277b1de35c26ab108029d44e87</component><component>11</component></publicKey></issuerData><subjectData><identification>5d164c7633cab663b027bc8e7f8510c1</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>92bf1fd218f4f3a243439e60b68d05a5a1298e940a0aef2c5c3e9371657635d7aab7604c6dcadebc84300d7f4ba619c7bd871fe3dfa7358918cc5f8c080c5ac660e14115cadf17757a48f82b67d3faebfdbcda576105d465346c67df50ab65dd587990ca51dd1b74e20f6b5e635aa9fbe036af72273f62e9379eac0e63415809</component><component>11</component></publicKey></subjectData><validity><notBefore>20:15:15-20041105</notBefore><notAfter>20:15:15-20051105</notAfter></validity><signature>37ade71a2c04753dd1cd9165a6e625076ed517b8951cb1caaa543ce6a8994fba5179a81010d2787664a96a443d197a1db92ce1ffbc9531b74fbb4d7fe61e40a980d55be2f08b3c7de8b3452ba93c16457adfe966f43bbfe305d766ce9a851bce3651ef0397fb609088167993f699684deecaccf4b851d1245604b53299e9a4c8</signature></certificate>', '<name>License Server</name>');
INSERT INTO `ausws_component` (`identification`, `public_key_xml`, `password`, `certificate_xml`, `other_data_xml`) VALUES ('d8cf805ee1a4f70eccbe8c74bc8a6652', '<publicKey><algorithmIdentifier>RSA</algorithmIdentifier><component>b8f21a19cb40dbe52132d16b2639ce9cb8e6e037cca2962e211fa43e6e647a810fafa45ce2be13a620896c6fd2d7a0a28c9df691845e5c48fe52a3ca7099a42a82c9d65b093eb4cadf4a358348d9933215eeb2573605c04256bb48334c5b8872f047dfa55aaf8293d0429ce00b8e7f6c29a5b489298c338a1a4e828a58f835c9</component><component>11</component></publicKey>', 'admin', '<certificate><issuerData><identification>553a0c3e15e065827e97be9e37126b94</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>cd3f8b62683016f9c2a85e48d09bcc46bf9a9545223521c85f5fce2b1b7ef48817ca3c180ad84e2ddd357e32819f2c1ce04bb08387e87e6f5c0d8fc2a008784d8bd046b1225c168bad855aa9183d6abd1df58a5cef717cc7db14dfcfcd1a2090f457af00ed9fc8fc50707e35601f5514c7108e277b1de35c26ab108029d44e87</component><component>11</component></publicKey></issuerData><subjectData><identification>d8cf805ee1a4f70eccbe8c74bc8a6652</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>b8f21a19cb40dbe52132d16b2639ce9cb8e6e037cca2962e211fa43e6e647a810fafa45ce2be13a620896c6fd2d7a0a28c9df691845e5c48fe52a3ca7099a42a82c9d65b093eb4cadf4a358348d9933215eeb2573605c04256bb48334c5b8872f047dfa55aaf8293d0429ce00b8e7f6c29a5b489298c338a1a4e828a58f835c9</component><component>11</component></publicKey></subjectData><validity><notBefore>20:00:52-20041105</notBefore><notAfter>20:00:52-20051105</notAfter></validity><signature>0f944603790626cfe6b637dfe92ec43456c398b752351e7eed8c6c7787686f7c9b8b8ba4cc18cf35fde98bcdbd46f9038eeff7f9b6951065d902accf42c90e1b657ae2a4c6b011368b23569017589afcb727e3bc9717eb6bd240b95b32e8c8f131696f2dffa6b172992118d7e527a23766440162763e5b6e842fe1f94efa1bf5</signature></certificate>', '<name>Payment Gateway</name><ip>127.0.0.1</ip>');
INSERT INTO `ausws_component` (`identification`, `public_key_xml`, `password`, `certificate_xml`, `other_data_xml`) VALUES ('de2a131dbfa518b1827b97aa81795755', '<publicKey><algorithmIdentifier>RSA</algorithmIdentifier><component>bb0f4d5c811e6ded07f9e7c7274777b7e70893c00cced890904a5c888805c4e1bab8a255b29d206bfbd9530e4eabbde7b20d0c574b0cf3f36586571cc5c8f61b89983accf52a97d61828442339ccb4a54d778567026179f4b2a90b3a5d005ed17b3eafefa24f3a873e2d1ce134d64769268e2ef11555d05c1635216c4309f835</component><component>11</component></publicKey>', 'admin', '<certificate><issuerData><identification>553a0c3e15e065827e97be9e37126b94</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>cd3f8b62683016f9c2a85e48d09bcc46bf9a9545223521c85f5fce2b1b7ef48817ca3c180ad84e2ddd357e32819f2c1ce04bb08387e87e6f5c0d8fc2a008784d8bd046b1225c168bad855aa9183d6abd1df58a5cef717cc7db14dfcfcd1a2090f457af00ed9fc8fc50707e35601f5514c7108e277b1de35c26ab108029d44e87</component><component>11</component></publicKey></issuerData><subjectData><identification>de2a131dbfa518b1827b97aa81795755</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>bb0f4d5c811e6ded07f9e7c7274777b7e70893c00cced890904a5c888805c4e1bab8a255b29d206bfbd9530e4eabbde7b20d0c574b0cf3f36586571cc5c8f61b89983accf52a97d61828442339ccb4a54d778567026179f4b2a90b3a5d005ed17b3eafefa24f3a873e2d1ce134d64769268e2ef11555d05c1635216c4309f835</component><component>11</component></publicKey></subjectData><validity><notBefore>17:15:33-20041105</notBefore><notAfter>17:15:33-20051105</notAfter></validity><signature>a50e7b236b327d8559ec6ab94d3ef0a03753e44c41975008d98b9f1f010ad3a6b4f375d2e426739baec13d603df478a9340332c5b5ee46c5c8324e08dd01aefb23f99fc6a7886d0e9f9c13ce4d0f11b0248407dae7e6996a2a57a00b0da086979a3617c8ad78d7b932095ffd88fbe690b24494f4a15c3d3c36989bedd2aae024</signature></certificate>', '<name>Configuration Server</name>');
INSERT INTO `ausws_component` (`identification`, `public_key_xml`, `password`, `certificate_xml`, `other_data_xml`) VALUES ('97cef8b99bbcd0e2870d0d4adb000102', '<publicKey><algorithmIdentifier>RSA</algorithmIdentifier><component>c16172812cb2c21e65cf363cc8093bb391757d0ab0f677349efec550fec19a3b11ac79fd640d953c4d828171a5871ef89799234a298ce184ad3347fe4427b6e5ad7025d7677144ffe0d71cd37983f18421298e4f13556fa513b1cc038f8eb58b753641ddb61cd33085c4817071ebcaaa4916f14ab8d37d9de41f3b4d545bd7ab</component><component>11</component></publicKey>', 'admin', '<certificate><issuerData><identification>553a0c3e15e065827e97be9e37126b94</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>cd3f8b62683016f9c2a85e48d09bcc46bf9a9545223521c85f5fce2b1b7ef48817ca3c180ad84e2ddd357e32819f2c1ce04bb08387e87e6f5c0d8fc2a008784d8bd046b1225c168bad855aa9183d6abd1df58a5cef717cc7db14dfcfcd1a2090f457af00ed9fc8fc50707e35601f5514c7108e277b1de35c26ab108029d44e87</component><component>11</component></publicKey></issuerData><subjectData><identification>97cef8b99bbcd0e2870d0d4adb000102</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>c16172812cb2c21e65cf363cc8093bb391757d0ab0f677349efec550fec19a3b11ac79fd640d953c4d828171a5871ef89799234a298ce184ad3347fe4427b6e5ad7025d7677144ffe0d71cd37983f18421298e4f13556fa513b1cc038f8eb58b753641ddb61cd33085c4817071ebcaaa4916f14ab8d37d9de41f3b4d545bd7ab</component><component>11</component></publicKey></subjectData><validity><notBefore>17:20:46-20041105</notBefore><notAfter>17:20:46-20051105</notAfter></validity><signature>c035f80d174fc30e9e3a10330fe03c366ab09794851de31ac8e133aeae4b429c8e7beb8238e02b948cf5f490f634e5e0b59fd2adac6741a7ca9bb27249f03cd381b85cba9c5b6e2a0ccec98c366bdc645f2073671f1b1b86292358efadc06d75ae6f9212bc24058e701b40ea2cc787d0523b7ec601f329a092f27035f09b8c65</signature></certificate>', '<name>IPMP Tools Server</name>');
INSERT INTO `ausws_component` (`identification`, `public_key_xml`, `password`, `certificate_xml`, `other_data_xml`) VALUES ('e691088a2a3caf864aa8495cae71667a', '<publicKey><algorithmIdentifier>RSA</algorithmIdentifier><component>d65fb3a081087f6d941599ca1a6408611ca4d03edf6949df73e9f5120f4d33abc63a7f8ebe878059ab8203b7cdb7a1536e03c0546b3381ef0fabd8596bfa0161c99a39b6b5af65ade02813af8107372d54f55b4192a784c398b3e2592a6a772b9dbc89814714baaaa364435cc7c757d251f08cdd613bd3afade4bacc5165e96f</component><component>11</component></publicKey>', 'admin', '<certificate><issuerData><identification>553a0c3e15e065827e97be9e37126b94</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>cd3f8b62683016f9c2a85e48d09bcc46bf9a9545223521c85f5fce2b1b7ef48817ca3c180ad84e2ddd357e32819f2c1ce04bb08387e87e6f5c0d8fc2a008784d8bd046b1225c168bad855aa9183d6abd1df58a5cef717cc7db14dfcfcd1a2090f457af00ed9fc8fc50707e35601f5514c7108e277b1de35c26ab108029d44e87</component><component>11</component></publicKey></issuerData><subjectData><identification>e691088a2a3caf864aa8495cae71667a</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>d65fb3a081087f6d941599ca1a6408611ca4d03edf6949df73e9f5120f4d33abc63a7f8ebe878059ab8203b7cdb7a1536e03c0546b3381ef0fabd8596bfa0161c99a39b6b5af65ade02813af8107372d54f55b4192a784c398b3e2592a6a772b9dbc89814714baaaa364435cc7c757d251f08cdd613bd3afade4bacc5165e96f</component><component>11</component></publicKey></subjectData><validity><notBefore>19:58:13-20041105</notBefore><notAfter>19:58:13-20051105</notAfter></validity><signature>939829f3cf22ebc32b872465f73e1e2d34eea4c8c1956cd60eb8df54b7fa681312d458ea9bcb5867c46a6cdc70ea7ad2ed0e034f9aaf41a0a566aaee59769af80e3e87af518281604f78488543f4d1108133f8b5fcf3d045e5a5ad0043fa416024927c7bf3fcec0d8fcc45fbd9280d22106355d88d4e072628ef4f2737ddcbec</signature></certificate>', '<name>Registration Server</name>');
INSERT INTO `ausws_component` (`identification`, `public_key_xml`, `password`, `certificate_xml`, `other_data_xml`) VALUES ('cbdf37bb250d7786d7840d64b1aee54d', '<publicKey><algorithmIdentifier>RSA</algorithmIdentifier><component>b07af2514ce5d888a6583bf0fbaadde7ed459ffdff572d0cde3b206fd58fe9bd143fd0ee69a49fc56661aa2c7af26f155cb295e8e439e8a23eefe72d0ab0e6ab0812ce8f9a35c265972f0f0f63e2c218e9dede65672cafa0fa5b63d41724d5043fd95b5e22fc74957dd3475fef2182da0f726f4c7b52920bf1afc8be7649efe7</component><component>11</component></publicKey>', 'admin', '<certificate><issuerData><identification>553a0c3e15e065827e97be9e37126b94</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>cd3f8b62683016f9c2a85e48d09bcc46bf9a9545223521c85f5fce2b1b7ef48817ca3c180ad84e2ddd357e32819f2c1ce04bb08387e87e6f5c0d8fc2a008784d8bd046b1225c168bad855aa9183d6abd1df58a5cef717cc7db14dfcfcd1a2090f457af00ed9fc8fc50707e35601f5514c7108e277b1de35c26ab108029d44e87</component><component>11</component></publicKey></issuerData><subjectData><identification>cbdf37bb250d7786d7840d64b1aee54d</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>b07af2514ce5d888a6583bf0fbaadde7ed459ffdff572d0cde3b206fd58fe9bd143fd0ee69a49fc56661aa2c7af26f155cb295e8e439e8a23eefe72d0ab0e6ab0812ce8f9a35c265972f0f0f63e2c218e9dede65672cafa0fa5b63d41724d5043fd95b5e22fc74957dd3475fef2182da0f726f4c7b52920bf1afc8be7649efe7</component><component>11</component></publicKey></subjectData><validity><notBefore>13:00:19-20041108</notBefore><notAfter>13:00:19-20051108</notAfter></validity><signature>bad719dba99efa7a2bfa3827b57590d44ae5b7cde31bb8b10f1abb6e3144f7afde4a342ad687f7a5368c81b0b59446fa0f07982d3325c3744c998533192c426d04a0014fc22bd69253a270162304bab6350f9372e2761125db0a0da3de18c31cccaeb3e352b447a248c0771673588568353b87a255d809a79bc747d3e44e0665</signature></certificate>', 'Commerce Server');

-- --------------------------------------------------------

-- 
-- Table structure for table `ausws_user`
-- 

CREATE TABLE `ausws_user` (
  `identification` varchar(32) NOT NULL default '',
  `login` varchar(20) NOT NULL default '',
  `password` varchar(20) NOT NULL default '',
  `id` int(11) NOT NULL default '0',
  `public_key_xml` text NOT NULL,
  `name` text,
  `address` text,
  `email` text,
  `authentication` text,
  `other_data_xml` text,
  `certificate_xml` text,
  `uid_wm` varchar(4) default NULL,
  PRIMARY KEY  (`identification`,`login`,`password`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `ausws_user`
-- 

-- 
-- Database: `cfsws_dev`
-- 
CREATE DATABASE `cfsws_dev` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE cfsws_dev;

-- --------------------------------------------------------

-- 
-- Table structure for table `cfsws_admin`
-- 

CREATE TABLE `cfsws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `cfsws_admin`
-- 

INSERT INTO `cfsws_admin` (`login`, `password`, `public_key_xml`, `private_key_xml`, `certificate`) VALUES ('admin', 'admin', '<publicKey><algorithmIdentifier>RSA</algorithmIdentifier><component>bb0f4d5c811e6ded07f9e7c7274777b7e70893c00cced890904a5c888805c4e1bab8a255b29d206bfbd9530e4eabbde7b20d0c574b0cf3f36586571cc5c8f61b89983accf52a97d61828442339ccb4a54d778567026179f4b2a90b3a5d005ed17b3eafefa24f3a873e2d1ce134d64769268e2ef11555d05c1635216c4309f835</component><component>11</component></publicKey>', '<privateKey><algorithmIdentifier>RSA</algorithmIdentifier><component>bb0f4d5c811e6ded07f9e7c7274777b7e70893c00cced890904a5c888805c4e1bab8a255b29d206bfbd9530e4eabbde7b20d0c574b0cf3f36586571cc5c8f61b89983accf52a97d61828442339ccb4a54d778567026179f4b2a90b3a5d005ed17b3eafefa24f3a873e2d1ce134d64769268e2ef11555d05c1635216c4309f835</component><component>11</component><component>13419341fe3b9a5fedff5f638f566a710c7d1e44b6063bf0c38f2f2c2c1eb6264bb11fc50e9f3bcee1747d41790a265f616339bdb123373af0191f8e418964a0c653b837502d1dbec324a5fcacb30d3eed96220d831ebf242bc3134f3dd417cc9b194494e3edf30f1f1c1688678569eaf06643217249e303ee9ad83d6ab91759</component><component>be98874ef5c1f78bef69f7246028d3bf593ab5829581225119d506d1a35bb256b92888171c07894c7a8368a759ccfee45e5732505fa1da405c28eecdf92ad88d</component><component>fb4023adcffb7f562ec8008cde463ba73965ce3c97b5a91fa69191f93c73c54971b2fb56bb8953834c4fb4c3a442682ccd7a275bcd2f58d0f5509049aad91849</component></privateKey>', '<certificate><issuerData><identification>553a0c3e15e065827e97be9e37126b94</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>cd3f8b62683016f9c2a85e48d09bcc46bf9a9545223521c85f5fce2b1b7ef48817ca3c180ad84e2ddd357e32819f2c1ce04bb08387e87e6f5c0d8fc2a008784d8bd046b1225c168bad855aa9183d6abd1df58a5cef717cc7db14dfcfcd1a2090f457af00ed9fc8fc50707e35601f5514c7108e277b1de35c26ab108029d44e87</component><component>11</component></publicKey></issuerData><subjectData><identification>de2a131dbfa518b1827b97aa81795755</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>bb0f4d5c811e6ded07f9e7c7274777b7e70893c00cced890904a5c888805c4e1bab8a255b29d206bfbd9530e4eabbde7b20d0c574b0cf3f36586571cc5c8f61b89983accf52a97d61828442339ccb4a54d778567026179f4b2a90b3a5d005ed17b3eafefa24f3a873e2d1ce134d64769268e2ef11555d05c1635216c4309f835</component><component>11</component></publicKey></subjectData><validity><notBefore>17:15:33-20041105</notBefore><notAfter>17:15:33-20051105</notAfter></validity><signature>a50e7b236b327d8559ec6ab94d3ef0a03753e44c41975008d98b9f1f010ad3a6b4f375d2e426739baec13d603df478a9340332c5b5ee46c5c8324e08dd01aefb23f99fc6a7886d0e9f9c13ce4d0f11b0248407dae7e6996a2a57a00b0da086979a3617c8ad78d7b932095ffd88fbe690b24494f4a15c3d3c36989bedd2aae024</signature></certificate>');

-- --------------------------------------------------------

-- 
-- Table structure for table `location`
-- 

CREATE TABLE `location` (
  `id` varchar(32) NOT NULL default '',
  `location` varchar(250) NOT NULL default '',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `location`
-- 

INSERT INTO `location` (`id`, `location`) VALUES ('RGS', 'https://localhost/opensdrm/RGS/RGS.ws.php');
INSERT INTO `location` (`id`, `location`) VALUES ('PGW', 'https://localhost/opensdrm/PGW/PGW.ws.php');
INSERT INTO `location` (`id`, `location`) VALUES ('AUS', 'https://localhost/opensdrm/AUS/AUS.ws.php');
INSERT INTO `location` (`id`, `location`) VALUES ('LIS', 'https://localhost/opensdrm/LIS/LIS.ws.php');
INSERT INTO `location` (`id`, `location`) VALUES ('MDS', 'https://localhost/opensdrm/MDS/MDS.ws.php');
INSERT INTO `location` (`id`, `location`) VALUES ('h2kLIS', 'https://localhost/opensdrm/h2kLIS/LIS.ws.php');
-- 
-- Database: `cosws_dev`
-- 
CREATE DATABASE `cosws_dev` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE cosws_dev;

-- --------------------------------------------------------

-- 
-- Table structure for table `autoincr_account`
-- 

CREATE TABLE `autoincr_account` (
  `id` int(11) NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `autoincr_account`
-- 


-- --------------------------------------------------------

-- 
-- Table structure for table `autoincr_personalinfo`
-- 

CREATE TABLE `autoincr_personalinfo` (
  `id` int(11) NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `autoincr_personalinfo`
-- 


-- --------------------------------------------------------

-- 
-- Table structure for table `cosws_admin`
-- 

CREATE TABLE `cosws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `cosws_admin`
-- 

INSERT INTO `cosws_admin` (`login`, `password`, `public_key_xml`, `private_key_xml`, `certificate`) VALUES ('admin', 'admin', '<publicKey><algorithmIdentifier>RSA</algorithmIdentifier><component>b07af2514ce5d888a6583bf0fbaadde7ed459ffdff572d0cde3b206fd58fe9bd143fd0ee69a49fc56661aa2c7af26f155cb295e8e439e8a23eefe72d0ab0e6ab0812ce8f9a35c265972f0f0f63e2c218e9dede65672cafa0fa5b63d41724d5043fd95b5e22fc74957dd3475fef2182da0f726f4c7b52920bf1afc8be7649efe7</component><component>11</component></publicKey>', '<privateKey><algorithmIdentifier>RSA</algorithmIdentifier><component>b07af2514ce5d888a6583bf0fbaadde7ed459ffdff572d0cde3b206fd58fe9bd143fd0ee69a49fc56661aa2c7af26f155cb295e8e439e8a23eefe72d0ab0e6ab0812ce8f9a35c265972f0f0f63e2c218e9dede65672cafa0fa5b63d41724d5043fd95b5e22fc74957dd3475fef2182da0f726f4c7b52920bf1afc8be7649efe7</component><component>11</component><component>0f9260acd21447cfd27132772543c8492b8624b48778a217b94173cda1e70d18367e19f6eb342c371090259a8351a063c46a1c49415067599c243a0b80f17dc3a6794ba3e158cfad0feb298093aa845440dbdf645454a30ae80473f5b2e34f660c6223bcd457ea1f54f31b4ac9c171a3ca0002e7a0029267a84e854838d191f5</component><component>cd3349f015e926be420439180ab597bf195903ae6c7c2f0b879753c039cd915c724b66436efa8002dc9161c318c83e049de8d8d003c5b3d06ffb42709330a3e3</component><component>dc2b80b38a5e14a8a0c1aa4624f5f9499c1f4c45e99bf36f2d3c4333474715234135b5694d72ec84de7eb04d3d1e3cea8034203b621a62fa0e3a9fc609d22b2d</component></privateKey>', '<certificate><issuerData><identification>553a0c3e15e065827e97be9e37126b94</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>cd3f8b62683016f9c2a85e48d09bcc46bf9a9545223521c85f5fce2b1b7ef48817ca3c180ad84e2ddd357e32819f2c1ce04bb08387e87e6f5c0d8fc2a008784d8bd046b1225c168bad855aa9183d6abd1df58a5cef717cc7db14dfcfcd1a2090f457af00ed9fc8fc50707e35601f5514c7108e277b1de35c26ab108029d44e87</component><component>11</component></publicKey></issuerData><subjectData><identification>cbdf37bb250d7786d7840d64b1aee54d</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>b07af2514ce5d888a6583bf0fbaadde7ed459ffdff572d0cde3b206fd58fe9bd143fd0ee69a49fc56661aa2c7af26f155cb295e8e439e8a23eefe72d0ab0e6ab0812ce8f9a35c265972f0f0f63e2c218e9dede65672cafa0fa5b63d41724d5043fd95b5e22fc74957dd3475fef2182da0f726f4c7b52920bf1afc8be7649efe7</component><component>11</component></publicKey></subjectData><validity><notBefore>13:00:19-20041108</notBefore><notAfter>13:00:19-20051108</notAfter></validity><signature>bad719dba99efa7a2bfa3827b57590d44ae5b7cde31bb8b10f1abb6e3144f7afde4a342ad687f7a5368c81b0b59446fa0f07982d3325c3744c998533192c426d04a0014fc22bd69253a270162304bab6350f9372e2761125db0a0da3de18c31cccaeb3e352b447a248c0771673588568353b87a255d809a79bc747d3e44e0665</signature></certificate>');

-- --------------------------------------------------------

-- 
-- Table structure for table `cosws_pgw`
-- 

CREATE TABLE `cosws_pgw` (
  `cos_IP` varchar(100) NOT NULL default '',
  `identification` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`cos_IP`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `cosws_pgw`
-- 

INSERT INTO `cosws_pgw` (`cos_IP`, `identification`) VALUES ('127.0.0.1', 'd8cf805ee1a4f70eccbe8c74bc8a6652');
-- 
-- Database: `cpsws_dev`
-- 
CREATE DATABASE `cpsws_dev` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE cpsws_dev;

-- --------------------------------------------------------

-- 
-- Table structure for table `cpsws_admin`
-- 

CREATE TABLE `cpsws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `cpsws_admin`
-- 

INSERT INTO `cpsws_admin` (`login`, `password`, `public_key_xml`, `private_key_xml`, `certificate`) VALUES ('admin', 'admin', '<publicKey><algorithmIdentifier>RSA</algorithmIdentifier><component>a808b9a5deb76e48dbae0c2ec6bdff307623617a70d85cb5994c5b11894a43446343e374b42db7ac90fba10ff99ff19013bd94cd9a090a5922970a9a0c0a641bd34ec3dce888de8c1df0b3a95f45a9c6b59cdd29d772575f29e3e607c49dc04b4fc6de5e7ccb5fc01bc67fa3066c8109cf280196257452a874cf710c0225bc5b</component><component>13</component></publicKey>', '<privateKey><algorithmIdentifier>RSA</algorithmIdentifier><component>a808b9a5deb76e48dbae0c2ec6bdff307623617a70d85cb5994c5b11894a43446343e374b42db7ac90fba10ff99ff19013bd94cd9a090a5922970a9a0c0a641bd34ec3dce888de8c1df0b3a95f45a9c6b59cdd29d772575f29e3e607c49dc04b4fc6de5e7ccb5fc01bc67fa3066c8109cf280196257452a874cf710c0225bc5b</component><component>13</component><component>236027157241907b20c6536829d7284018df070c4da6d0263b387ef637d9bd51c40e4ad53367f0c603934a54348d763947863a463b603812c3e9e748da1d228c40a5e546041d3b3844428db1b23cc451e5ecfc81801db5e35bea287991643408235b4d394fbfb1e5fba81b5e364e0dd945daac3c42e0516dd2558013752aac7b</component><component>bef033f2b4c44abda48b1286d7124003335a6952cff1296658df741c375bc95042cfff2fc27e5110e24d05822bfab6e8689ed2bcfe31430fa811e4a4a0fe298d</component><component>e14a8edda039ba83352980167992c53e7e1cc46fe6f40e00dc6c31aa1aa5ffd46505305e7f7e81aace1af821587f08595aba7cbae9998ccf25a76c0af49cdf87</component></privateKey>', '<certificate><issuerData><identification>553a0c3e15e065827e97be9e37126b94</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>cd3f8b62683016f9c2a85e48d09bcc46bf9a9545223521c85f5fce2b1b7ef48817ca3c180ad84e2ddd357e32819f2c1ce04bb08387e87e6f5c0d8fc2a008784d8bd046b1225c168bad855aa9183d6abd1df58a5cef717cc7db14dfcfcd1a2090f457af00ed9fc8fc50707e35601f5514c7108e277b1de35c26ab108029d44e87</component><component>11</component></publicKey></issuerData><subjectData><identification>72965aa6d51717ca5729dae767d7ee34</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>a808b9a5deb76e48dbae0c2ec6bdff307623617a70d85cb5994c5b11894a43446343e374b42db7ac90fba10ff99ff19013bd94cd9a090a5922970a9a0c0a641bd34ec3dce888de8c1df0b3a95f45a9c6b59cdd29d772575f29e3e607c49dc04b4fc6de5e7ccb5fc01bc67fa3066c8109cf280196257452a874cf710c0225bc5b</component><component>13</component></publicKey></subjectData><validity><notBefore>18:58:51-20040304</notBefore><notAfter>18:58:51-20050304</notAfter></validity><signature>17816b964d56c156bf51426dcc89e9d132dc6e7512dce0f8e72652916759a321ccea1a55a5e4f9e9116c1902ffa92167d1dc2dbe985754cc78b8ddf1320549cb3c91146999cccfb52beb7e115d50359c30804aa85766b66e72d77e4f04e7eb58a2d2bd3aede276f8e90375fe55c909e9b2d2fe81d26bccee3bd55046b42a701a</signature></certificate>');
-- 
-- Database: `h2klis`
-- 
CREATE DATABASE `h2klis` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE h2klis;

-- --------------------------------------------------------

-- 
-- Table structure for table `content`
-- 

CREATE TABLE `content` (
  `content_id` varchar(200) NOT NULL default '',
  `key_list` text NOT NULL,
  PRIMARY KEY  (`content_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `content`
-- 

-- --------------------------------------------------------

-- 
-- Table structure for table `license`
-- 

CREATE TABLE `license` (
  `uid` varchar(100) NOT NULL default '',
  `cid` varchar(100) NOT NULL default '',
  `lic_data` text NOT NULL,
  PRIMARY KEY  (`uid`,`cid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `license`
-- 

-- --------------------------------------------------------

-- 
-- Table structure for table `loglicense`
-- 

CREATE TABLE `loglicense` (
  `id` varchar(100) NOT NULL default '',
  `uid` varchar(100) NOT NULL default '',
  `cid` varchar(100) NOT NULL default '',
  `licdata` text NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `loglicense`
-- 

-- 
-- Database: `itsws_dev`
-- 
CREATE DATABASE `itsws_dev` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE itsws_dev;

-- --------------------------------------------------------

-- 
-- Table structure for table `ipmptool`
-- 

CREATE TABLE `ipmptool` (
  `ipmptoolid` varchar(128) NOT NULL default '',
  `ipmptool_filename` varchar(255) NOT NULL default '',
  `ipmptool_url` text NOT NULL,
  `ipmptool_description` text NOT NULL,
  PRIMARY KEY  (`ipmptoolid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `ipmptool`
-- 


-- --------------------------------------------------------

-- 
-- Table structure for table `itsws_admin`
-- 

CREATE TABLE `itsws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `itsws_admin`
-- 

INSERT INTO `itsws_admin` (`login`, `password`, `public_key_xml`, `private_key_xml`, `certificate`) VALUES ('admin', 'admin', '<publicKey><algorithmIdentifier>RSA</algorithmIdentifier><component>c16172812cb2c21e65cf363cc8093bb391757d0ab0f677349efec550fec19a3b11ac79fd640d953c4d828171a5871ef89799234a298ce184ad3347fe4427b6e5ad7025d7677144ffe0d71cd37983f18421298e4f13556fa513b1cc038f8eb58b753641ddb61cd33085c4817071ebcaaa4916f14ab8d37d9de41f3b4d545bd7ab</component><component>11</component></publicKey>', '<privateKey><algorithmIdentifier>RSA</algorithmIdentifier><component>c16172812cb2c21e65cf363cc8093bb391757d0ab0f677349efec550fec19a3b11ac79fd640d953c4d828171a5871ef89799234a298ce184ad3347fe4427b6e5ad7025d7677144ffe0d71cd37983f18421298e4f13556fa513b1cc038f8eb58b753641ddb61cd33085c4817071ebcaaa4916f14ab8d37d9de41f3b4d545bd7ab</component><component>11</component><component>079563dce8a7a33868627a98f8c9257a7e2cc3a610fa9b4351877b305a4ddde432e3a0644a32ba8ee9f10a188900477d331f1a7b610081507f43441402ac395e458a69b31fd1841b1c3906a5038ddc6953aaa3381be9ba3bfb6f8491be0caa809b4657f02db3b3f5e89a10c5ca8d4f91785dd7d6c04919454de530e8fc571921</component><component>fa9d7234c5c0571d438d32dec4efb72061237446c0e8ddfcaf96b00ee5a548f3c29536aeca2648167f769ad96985cbbcf8b8383e028c90f42b177340d52fa8f7</component><component>c5892c4af652452f4d9bc084d9f2c5e5ea86d7f18aa484aed87f67703ba670c83b1f48c25e901e1b5af53ae35b5291efd305b9288efee84276b368d75c7f2ded</component></privateKey>', '<certificate><issuerData><identification>553a0c3e15e065827e97be9e37126b94</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>cd3f8b62683016f9c2a85e48d09bcc46bf9a9545223521c85f5fce2b1b7ef48817ca3c180ad84e2ddd357e32819f2c1ce04bb08387e87e6f5c0d8fc2a008784d8bd046b1225c168bad855aa9183d6abd1df58a5cef717cc7db14dfcfcd1a2090f457af00ed9fc8fc50707e35601f5514c7108e277b1de35c26ab108029d44e87</component><component>11</component></publicKey></issuerData><subjectData><identification>97cef8b99bbcd0e2870d0d4adb000102</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>c16172812cb2c21e65cf363cc8093bb391757d0ab0f677349efec550fec19a3b11ac79fd640d953c4d828171a5871ef89799234a298ce184ad3347fe4427b6e5ad7025d7677144ffe0d71cd37983f18421298e4f13556fa513b1cc038f8eb58b753641ddb61cd33085c4817071ebcaaa4916f14ab8d37d9de41f3b4d545bd7ab</component><component>11</component></publicKey></subjectData><validity><notBefore>17:20:46-20041105</notBefore><notAfter>17:20:46-20051105</notAfter></validity><signature>c035f80d174fc30e9e3a10330fe03c366ab09794851de31ac8e133aeae4b429c8e7beb8238e02b948cf5f490f634e5e0b59fd2adac6741a7ca9bb27249f03cd381b85cba9c5b6e2a0ccec98c366bdc645f2073671f1b1b86292358efadc06d75ae6f9212bc24058e701b40ea2cc787d0523b7ec601f329a092f27035f09b8c65</signature></certificate>');

-- --------------------------------------------------------

-- 
-- Table structure for table `itsws_ipmptools`
-- 

CREATE TABLE `itsws_ipmptools` (
  `identification` varchar(32) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `password` varchar(20) NOT NULL default '',
  `certificate_xml` text,
  `other_data_xml` text,
  PRIMARY KEY  (`identification`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `itsws_ipmptools`
-- 

-- 
-- Database: `lisws_dev`
-- 
CREATE DATABASE `lisws_dev` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE lisws_dev;

-- --------------------------------------------------------

-- 
-- Table structure for table `content_keys`
-- 

CREATE TABLE `content_keys` (
  `cid` varchar(100) NOT NULL default '',
  `ckey` varchar(100) NOT NULL default '',
  `hash_cid` varchar(200) NOT NULL default '',
  PRIMARY KEY  (`cid`,`ckey`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `content_keys`
-- 

-- --------------------------------------------------------

-- 
-- Table structure for table `license`
-- 

CREATE TABLE `license` (
  `id` varchar(100) NOT NULL default '0',
  `template_id` varchar(100) NOT NULL default '',
  `uid` varchar(100) NOT NULL default '',
  `cid` varchar(100) NOT NULL default '',
  `license` text NOT NULL,
  `cid_hash` varchar(200) NOT NULL default '',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `license`
-- 


-- --------------------------------------------------------

-- 
-- Table structure for table `license_template`
-- 

CREATE TABLE `license_template` (
  `id` int(11) NOT NULL default '0',
  `type` varchar(100) NOT NULL default '',
  `description` text NOT NULL,
  `content_type` varchar(100) NOT NULL default '',
  `num_params` int(11) NOT NULL default '0',
  `license_template` text NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `license_template`
-- 

INSERT INTO `license_template` (`id`, `type`, `description`, `content_type`, `num_params`, `license_template`) VALUES (1, 'ODRL 1.1', 'An ODRL license for the MUSIC-4YOU music service.', 'MUSIC in MPEG-4 format.', 6, '<?xml version="1.0" encoding="UTF-8" ?> \r\n<o-ex:rights xmlns:o-ex="http://odrl.net/1.1/ODRL-EX" \r\n			xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" \r\n			xmlns:o-dd="http://odrl.net/1.1/ODRL-DD" \r\n			xmlns:ds="http://odrl.net/1.1/ODRL-DD" \r\n			xsi:schemaLocation="http://odrl.net/1.1/ODRL-EX ../schemas/ODRL-EX-11.xsd \r\n			http://odrl.net/1.1/ODRL-DD ../schemas/ODRL-DD-11.xsd"> \r\n	<o-ex:agreement>\r\n		<o-ex:asset>\r\n			<ds:keyInfo>\r\n				<ds:keyValue>%KEY%</ds:keyValue>\r\n			</ds:keyInfo>\r\n			<o-ex:context>\r\n				<o-dd:uid>%PARAM_1%</o-dd:uid>\r\n				<o-dd:name>%PARAM_2%</o-dd:name> \r\n			</o-ex:context>\r\n		</o-ex:asset>\r\n		<o-ex:permission>\r\n			<o-dd:play>\r\n				<o-ex:constraint>\r\n					<o-dd:individual>%PARAM_3%</o-dd:individual>\r\n					<o-dd:count>%PARAM_4%</o-dd:count>\r\n					<o-dd:datetime>\r\n						<o-dd:start>%PARAM_5%</o-dd:start>\r\n						<o-dd:end>%PARAM_6%</o-dd:end>\r\n					</o-dd:datetime>\r\n				</o-ex:constraint>\r\n			</o-dd:play>\r\n		</o-ex:permission>\r\n	</o-ex:agreement>\r\n</o-ex:rights>');

-- --------------------------------------------------------

-- 
-- Table structure for table `lisws_admin`
-- 

CREATE TABLE `lisws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `lisws_admin`
-- 

INSERT INTO `lisws_admin` (`login`, `password`, `public_key_xml`, `private_key_xml`, `certificate`) VALUES ('admin', 'admin', '<publicKey><algorithmIdentifier>RSA</algorithmIdentifier><component>92bf1fd218f4f3a243439e60b68d05a5a1298e940a0aef2c5c3e9371657635d7aab7604c6dcadebc84300d7f4ba619c7bd871fe3dfa7358918cc5f8c080c5ac660e14115cadf17757a48f82b67d3faebfdbcda576105d465346c67df50ab65dd587990ca51dd1b74e20f6b5e635aa9fbe036af72273f62e9379eac0e63415809</component><component>11</component></publicKey>', '<privateKey><algorithmIdentifier>RSA</algorithmIdentifier><component>92bf1fd218f4f3a243439e60b68d05a5a1298e940a0aef2c5c3e9371657635d7aab7604c6dcadebc84300d7f4ba619c7bd871fe3dfa7358918cc5f8c080c5ac660e14115cadf17757a48f82b67d3faebfdbcda576105d465346c67df50ab65dd587990ca51dd1b74e20f6b5e635aa9fbe036af72273f62e9379eac0e63415809</component><component>11</component><component>0450ea5900bbe90c4d45c0e4ba1334df397229d72d78cacc994d227446bfb64a1b9bfb4d8ac233ba401f87ed27df3cfe5865db4a7784eafc80bab7841e5ab760249f648c703959f8d5549ed6b66780dca9beaa72b42824def91a10419343f34c8bb3015668a4c5b7af281ba8d86c05caa546bea7e8e16f341e4c1df0002941f9</component><component>be491a9fa9ea7427d616891b6cb29fe4bbca9501fa9e36f46609ab71607f81b0e8337569f5ae91832e2168bec4930725ad2bbd39020dc06c863a692c3f439f4d</component><component>c56ccbcf3956b0414ef5568bc1623db8b69fa2197912b7d3b8ec93b861259401e27fede6764c478c709a5632e06eddec3fa59fec3740dd90ab4849021e82f5ad</component></privateKey>', '<certificate><issuerData><identification>553a0c3e15e065827e97be9e37126b94</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>cd3f8b62683016f9c2a85e48d09bcc46bf9a9545223521c85f5fce2b1b7ef48817ca3c180ad84e2ddd357e32819f2c1ce04bb08387e87e6f5c0d8fc2a008784d8bd046b1225c168bad855aa9183d6abd1df58a5cef717cc7db14dfcfcd1a2090f457af00ed9fc8fc50707e35601f5514c7108e277b1de35c26ab108029d44e87</component><component>11</component></publicKey></issuerData><subjectData><identification>5d164c7633cab663b027bc8e7f8510c1</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>92bf1fd218f4f3a243439e60b68d05a5a1298e940a0aef2c5c3e9371657635d7aab7604c6dcadebc84300d7f4ba619c7bd871fe3dfa7358918cc5f8c080c5ac660e14115cadf17757a48f82b67d3faebfdbcda576105d465346c67df50ab65dd587990ca51dd1b74e20f6b5e635aa9fbe036af72273f62e9379eac0e63415809</component><component>11</component></publicKey></subjectData><validity><notBefore>20:15:15-20041105</notBefore><notAfter>20:15:15-20051105</notAfter></validity><signature>37ade71a2c04753dd1cd9165a6e625076ed517b8951cb1caaa543ce6a8994fba5179a81010d2787664a96a443d197a1db92ce1ffbc9531b74fbb4d7fe61e40a980d55be2f08b3c7de8b3452ba93c16457adfe966f43bbfe305d766ce9a851bce3651ef0397fb609088167993f699684deecaccf4b851d1245604b53299e9a4c8</signature></certificate>');

-- --------------------------------------------------------

-- 
-- Table structure for table `log_licenses`
-- 

CREATE TABLE `log_licenses` (
  `timestamp` varchar(100) NOT NULL default '',
  `operation` varchar(100) NOT NULL default '',
  `description` text NOT NULL,
  `uid` varchar(100) NOT NULL default '',
  `lic_id` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`timestamp`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `log_licenses`
-- 

-- 
-- Database: `mdsws_dev`
-- 
CREATE DATABASE `mdsws_dev` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE mdsws_dev;

-- --------------------------------------------------------

-- 
-- Table structure for table `content_delivery`
-- 

CREATE TABLE `content_delivery` (
  `id` varchar(100) NOT NULL default '',
  `cid` varchar(100) NOT NULL default '',
  `uid` varchar(100) NOT NULL default '',
  `status` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `content_delivery`
-- 


-- --------------------------------------------------------

-- 
-- Table structure for table `content_location`
-- 

CREATE TABLE `content_location` (
  `id` varchar(100) NOT NULL default '',
  `cid` varchar(100) NOT NULL default '',
  `type` varchar(100) NOT NULL default '',
  `protocol` varchar(100) NOT NULL default '',
  `location` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`id`,`cid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `content_location`
-- 

-- --------------------------------------------------------

-- 
-- Table structure for table `mdsws_admin`
-- 

CREATE TABLE `mdsws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `mdsws_admin`
-- 

INSERT INTO `mdsws_admin` (`login`, `password`, `public_key_xml`, `private_key_xml`, `certificate`) VALUES ('admin', 'admin', '<publicKey><algorithmIdentifier>RSA</algorithmIdentifier><component>de60ea18905a5035f1b9984857364c9bd830c9925e5f77bbb31e9568a1ff0cc3b85caf4aa46449d75817aef870b5e9022e1821abdcc1880123b0b2a7b73550a5253f8b0eb3b4e35b42d13cf7487433cfb6ca83c9025f985645e442dae401766b76c7534d0962000422e55f7b986c4aff045f3fcb217c7f23018296975c05d30f</component><component>13</component></publicKey>', '<privateKey><algorithmIdentifier>RSA</algorithmIdentifier><component>de60ea18905a5035f1b9984857364c9bd830c9925e5f77bbb31e9568a1ff0cc3b85caf4aa46449d75817aef870b5e9022e1821abdcc1880123b0b2a7b73550a5253f8b0eb3b4e35b42d13cf7487433cfb6ca83c9025f985645e442dae401766b76c7534d0962000422e55f7b986c4aff045f3fcb217c7f23018296975c05d30f</component><component>13</component><component>34ab29f8581563d6e1abf4e8b656f732625c65a2aa8fdfb9edca9ca641509df875c51c0af10a47622913153ad753814a9f20a9a8b448c8a1f43e0f5d9deaf16a0a19373922e2d374f2f8fed403398a228007e0f7001c984d861cfa08e73b5d92156e41f8a70adabcf258ed2b0a6dfda25df8350988a2b942d8ef22f41897e7a3</component><component>dfdc90ffa936c18de4a9f8e4e29564021cee0200638d7cca815507d011f7999356790dadc83759e5b0f3d6705850588845226ec031411fe260c1bdd3fb849309</component><component>fe4dd8015ac06c1878b69e5a5841333bef10930b10207c0b8e4dc601ac648aa837992f139c522700ab9683391412a71e87cf7f54766fdf42d3eb9a83dc38c357</component></privateKey>', '<certificate><issuerData><identification>553a0c3e15e065827e97be9e37126b94</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>cd3f8b62683016f9c2a85e48d09bcc46bf9a9545223521c85f5fce2b1b7ef48817ca3c180ad84e2ddd357e32819f2c1ce04bb08387e87e6f5c0d8fc2a008784d8bd046b1225c168bad855aa9183d6abd1df58a5cef717cc7db14dfcfcd1a2090f457af00ed9fc8fc50707e35601f5514c7108e277b1de35c26ab108029d44e87</component><component>11</component></publicKey></issuerData><subjectData><identification>aadc9d54f6c5fa45a4fd906b78ae5727</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>de60ea18905a5035f1b9984857364c9bd830c9925e5f77bbb31e9568a1ff0cc3b85caf4aa46449d75817aef870b5e9022e1821abdcc1880123b0b2a7b73550a5253f8b0eb3b4e35b42d13cf7487433cfb6ca83c9025f985645e442dae401766b76c7534d0962000422e55f7b986c4aff045f3fcb217c7f23018296975c05d30f</component><component>13</component></publicKey></subjectData><validity><notBefore>12:53:54-20041108</notBefore><notAfter>12:53:54-20051108</notAfter></validity><signature>35dcdde183b5542c024a2729222b9fe796013303b12d354b0b6852807d36f8ccf5c2f88117842219c4ed4b6dc35980a094b92d17d6e156be4ff0bbd05f6593674e535946c23f1726c4d5992ad56307e1b733745bb39bf45dfa87cf218e5b85358f902e7f1ecff5ca79dc26474247cb968d9ff38228339718501ff6583d3c1f50</signature></certificate>');
-- 
-- Database: `pgwws_dev`
-- 
CREATE DATABASE `pgwws_dev` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE pgwws_dev;

-- --------------------------------------------------------

-- 
-- Table structure for table `pgwws_admin`
-- 

CREATE TABLE `pgwws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate_AUS` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `pgwws_admin`
-- 

INSERT INTO `pgwws_admin` (`login`, `password`, `public_key_xml`, `private_key_xml`, `certificate_AUS`) VALUES ('admin', 'admin', '<publicKey><algorithmIdentifier>RSA</algorithmIdentifier><component>b8f21a19cb40dbe52132d16b2639ce9cb8e6e037cca2962e211fa43e6e647a810fafa45ce2be13a620896c6fd2d7a0a28c9df691845e5c48fe52a3ca7099a42a82c9d65b093eb4cadf4a358348d9933215eeb2573605c04256bb48334c5b8872f047dfa55aaf8293d0429ce00b8e7f6c29a5b489298c338a1a4e828a58f835c9</component><component>11</component></publicKey>', '<privateKey><algorithmIdentifier>RSA</algorithmIdentifier><component>b8f21a19cb40dbe52132d16b2639ce9cb8e6e037cca2962e211fa43e6e647a810fafa45ce2be13a620896c6fd2d7a0a28c9df691845e5c48fe52a3ca7099a42a82c9d65b093eb4cadf4a358348d9933215eeb2573605c04256bb48334c5b8872f047dfa55aaf8293d0429ce00b8e7f6c29a5b489298c338a1a4e828a58f835c9</component><component>11</component><component>2613ba145e8d5a72f03f2b1d96edc8a7cbb7100b7cf44c188e59568552f691b128e0659aa7272229bb67965341ff37a8fed53a4b22c8220f072f21b8bcd45680de9993e2a7ed2c0b7fb09178794f330efd9a71f00eddc2081449c6f3beeafc78891235fae0f5055b15d559dbd675016d86b776c6bf62b836ea263b010db56497</component><component>bda3c66e0da16e6de08c0decf3cb1cc65b999cafd1ed4d750f45ee3a68540a84324c8797f2606785f04c15f73474963c04cba88a656498138f4c16c29cdfa917</component><component>f9a9d3e939af950092891bdf758dc790a166ec3df7510d81773393a220246a80b67e51dd23a9010a5157f706332abdae71f138cbb4da65d9876d4d3054f0a41f</component></privateKey>', '<certificate><issuerData><identification>553a0c3e15e065827e97be9e37126b94</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>cd3f8b62683016f9c2a85e48d09bcc46bf9a9545223521c85f5fce2b1b7ef48817ca3c180ad84e2ddd357e32819f2c1ce04bb08387e87e6f5c0d8fc2a008784d8bd046b1225c168bad855aa9183d6abd1df58a5cef717cc7db14dfcfcd1a2090f457af00ed9fc8fc50707e35601f5514c7108e277b1de35c26ab108029d44e87</component><component>11</component></publicKey></issuerData><subjectData><identification>d8cf805ee1a4f70eccbe8c74bc8a6652</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>b8f21a19cb40dbe52132d16b2639ce9cb8e6e037cca2962e211fa43e6e647a810fafa45ce2be13a620896c6fd2d7a0a28c9df691845e5c48fe52a3ca7099a42a82c9d65b093eb4cadf4a358348d9933215eeb2573605c04256bb48334c5b8872f047dfa55aaf8293d0429ce00b8e7f6c29a5b489298c338a1a4e828a58f835c9</component><component>11</component></publicKey></subjectData><validity><notBefore>20:00:52-20041105</notBefore><notAfter>20:00:52-20051105</notAfter></validity><signature>0f944603790626cfe6b637dfe92ec43456c398b752351e7eed8c6c7787686f7c9b8b8ba4cc18cf35fde98bcdbd46f9038eeff7f9b6951065d902accf42c90e1b657ae2a4c6b011368b23569017589afcb727e3bc9717eb6bd240b95b32e8c8f131696f2dffa6b172992118d7e527a23766440162763e5b6e842fe1f94efa1bf5</signature></certificate>');

-- --------------------------------------------------------

-- 
-- Table structure for table `pgwws_cos`
-- 

CREATE TABLE `pgwws_cos` (
  `identification` varchar(32) NOT NULL default '',
  `certificate_xml` text,
  `other_data_xml` text,
  PRIMARY KEY  (`identification`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `pgwws_cos`
-- 

INSERT INTO `pgwws_cos` (`identification`, `certificate_xml`, `other_data_xml`) VALUES ('cbdf37bb250d7786d7840d64b1aee54d', '<certificate><issuerData><identification>553a0c3e15e065827e97be9e37126b94</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>cd3f8b62683016f9c2a85e48d09bcc46bf9a9545223521c85f5fce2b1b7ef48817ca3c180ad84e2ddd357e32819f2c1ce04bb08387e87e6f5c0d8fc2a008784d8bd046b1225c168bad855aa9183d6abd1df58a5cef717cc7db14dfcfcd1a2090f457af00ed9fc8fc50707e35601f5514c7108e277b1de35c26ab108029d44e87</component><component>11</component></publicKey></issuerData><subjectData><identification>cbdf37bb250d7786d7840d64b1aee54d</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>b07af2514ce5d888a6583bf0fbaadde7ed459ffdff572d0cde3b206fd58fe9bd143fd0ee69a49fc56661aa2c7af26f155cb295e8e439e8a23eefe72d0ab0e6ab0812ce8f9a35c265972f0f0f63e2c218e9dede65672cafa0fa5b63d41724d5043fd95b5e22fc74957dd3475fef2182da0f726f4c7b52920bf1afc8be7649efe7</component><component>11</component></publicKey></subjectData><validity><notBefore>13:00:19-20041108</notBefore><notAfter>13:00:19-20051108</notAfter></validity><signature>bad719dba99efa7a2bfa3827b57590d44ae5b7cde31bb8b10f1abb6e3144f7afde4a342ad687f7a5368c81b0b59446fa0f07982d3325c3744c998533192c426d04a0014fc22bd69253a270162304bab6350f9372e2761125db0a0da3de18c31cccaeb3e352b447a248c0771673588568353b87a255d809a79bc747d3e44e0665</signature></certificate>', '');

-- --------------------------------------------------------

-- 
-- Table structure for table `pgwws_transaction`
-- 

CREATE TABLE `pgwws_transaction` (
  `transaction_number` varchar(100) NOT NULL default '',
  `cos_id` varchar(100) NOT NULL default '',
  `pay_data` text NOT NULL,
  `status` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`transaction_number`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `pgwws_transaction`
-- 

-- 
-- Database: `rgsws_dev`
-- 
CREATE DATABASE `rgsws_dev` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE rgsws_dev;

-- --------------------------------------------------------

-- 
-- Table structure for table `rgsws_admin`
-- 

CREATE TABLE `rgsws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `rgsws_admin`
-- 

INSERT INTO `rgsws_admin` (`login`, `password`, `public_key_xml`, `private_key_xml`, `certificate`) VALUES ('admin', 'admin', '<publicKey><algorithmIdentifier>RSA</algorithmIdentifier><component>d65fb3a081087f6d941599ca1a6408611ca4d03edf6949df73e9f5120f4d33abc63a7f8ebe878059ab8203b7cdb7a1536e03c0546b3381ef0fabd8596bfa0161c99a39b6b5af65ade02813af8107372d54f55b4192a784c398b3e2592a6a772b9dbc89814714baaaa364435cc7c757d251f08cdd613bd3afade4bacc5165e96f</component><component>11</component></publicKey>', '<privateKey><algorithmIdentifier>RSA</algorithmIdentifier><component>d65fb3a081087f6d941599ca1a6408611ca4d03edf6949df73e9f5120f4d33abc63a7f8ebe878059ab8203b7cdb7a1536e03c0546b3381ef0fabd8596bfa0161c99a39b6b5af65ade02813af8107372d54f55b4192a784c398b3e2592a6a772b9dbc89814714baaaa364435cc7c757d251f08cdd613bd3afade4bacc5165e96f</component><component>11</component><component>1f868b5b5e4503ae3b6c969db8963d77b16388093ef8e53772edb3193e7c4b5d069021d13a22fc496c0b97228f3197b9720f9c48a65a65ee733eeb1c3586a5d967c74e615b6ae6fd7869cfbdf738497a20121a2e680d8a2e6e903c81997db13ec018e0f50d207df3249bf03a2b85bd409fcc6921c6c0a442f62ce6ba709c00f9</component><component>e3dbfda42fa487e271f64cfe3bd894608d12c20e79b225a26bb5d888b8b3c29f62c18cddba7d89b02f92e87cadaceaa488d10c97cdf9ee76e8fc73896144bf27</component><component>f0d95a494b9a54a96e95753f1a7caf27edcde72abbcc804bd6c2d4c5912ccc14ba51cfbacced721eae135fbaf2269976574b1bc64bbd883e3aea264f2595f079</component></privateKey>', '<certificate><issuerData><identification>553a0c3e15e065827e97be9e37126b94</identification><signatureAlgorithmIdentifier>RSA_WITH_MD5</signatureAlgorithmIdentifier><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>cd3f8b62683016f9c2a85e48d09bcc46bf9a9545223521c85f5fce2b1b7ef48817ca3c180ad84e2ddd357e32819f2c1ce04bb08387e87e6f5c0d8fc2a008784d8bd046b1225c168bad855aa9183d6abd1df58a5cef717cc7db14dfcfcd1a2090f457af00ed9fc8fc50707e35601f5514c7108e277b1de35c26ab108029d44e87</component><component>11</component></publicKey></issuerData><subjectData><identification>e691088a2a3caf864aa8495cae71667a</identification><publicKey><algorithmIdentifier>RSA-1024</algorithmIdentifier><component>d65fb3a081087f6d941599ca1a6408611ca4d03edf6949df73e9f5120f4d33abc63a7f8ebe878059ab8203b7cdb7a1536e03c0546b3381ef0fabd8596bfa0161c99a39b6b5af65ade02813af8107372d54f55b4192a784c398b3e2592a6a772b9dbc89814714baaaa364435cc7c757d251f08cdd613bd3afade4bacc5165e96f</component><component>11</component></publicKey></subjectData><validity><notBefore>19:58:13-20041105</notBefore><notAfter>19:58:13-20051105</notAfter></validity><signature>939829f3cf22ebc32b872465f73e1e2d34eea4c8c1956cd60eb8df54b7fa681312d458ea9bcb5867c46a6cdc70ea7ad2ed0e034f9aaf41a0a566aaee59769af80e3e87af518281604f78488543f4d1108133f8b5fcf3d045e5a5ad0043fa416024927c7bf3fcec0d8fcc45fbd9280d22106355d88d4e072628ef4f2737ddcbec</signature></certificate>');

-- --------------------------------------------------------

-- 
-- Table structure for table `rgsws_content`
-- 

CREATE TABLE `rgsws_content` (
  `content_id` varchar(128) NOT NULL default '',
  `internal_id` int(11) default NULL,
  `hash` text NOT NULL,
  `file` varchar(254) NOT NULL default '',
  `metadata` text,
  PRIMARY KEY  (`content_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `rgsws_content`
-- 

-- 
-- Database: `wbwallet`
-- 
CREATE DATABASE `wbwallet` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE wbwallet;

-- --------------------------------------------------------

-- 
-- Table structure for table `license`
-- 

CREATE TABLE `license` (
  `id` varchar(100) NOT NULL default '',
  `cid` varchar(100) NOT NULL default '',
  `uid` varchar(100) NOT NULL default '',
  `license` text NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `license`
-- 


-- --------------------------------------------------------

-- 
-- Table structure for table `user`
-- 

CREATE TABLE `user` (
  `id` varchar(100) NOT NULL default '',
  `uid` text NOT NULL,
  `username` text NOT NULL,
  `password` text NOT NULL,
  `public-key` text NOT NULL,
  `private-key` text NOT NULL,
  `certificate` text NOT NULL,
  `name` text NOT NULL,
  `address` text NOT NULL,
  `email` text NOT NULL,
  `paymethod` text NOT NULL,
  `paymethod_details` text NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- 
-- Dumping data for table `user`
-- 

