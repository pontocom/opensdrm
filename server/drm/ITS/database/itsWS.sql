-- phpMyAdmin SQL Dump
-- version 2.6.0-pl2
-- http://www.phpmyadmin.net
-- 
-- M�quina: localhost
-- Data de Cria��o: 08-Nov-2004 �s 13:32
-- Vers�o do servidor: 4.0.20
-- vers�o do PHP: 4.3.9
-- 
-- Base de Dados: `itsws_dev`
-- 

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `ipmptool`
-- 

CREATE TABLE `ipmptool` (
  `ipmptoolid` varchar(128) NOT NULL default '',
  `ipmptool_filename` varchar(255) NOT NULL default '',
  `ipmptool_url` text NOT NULL,
  `ipmptool_description` text NOT NULL,
  PRIMARY KEY  (`ipmptoolid`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `itsws_admin`
-- 

CREATE TABLE `itsws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `itsws_ipmptools`
-- 

CREATE TABLE `itsws_ipmptools` (
  `identification` varchar(32) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `password` varchar(20) NOT NULL default '',
  `certificate_xml` text,
  `other_data_xml` text,
  PRIMARY KEY  (`identification`)
) TYPE=MyISAM;

