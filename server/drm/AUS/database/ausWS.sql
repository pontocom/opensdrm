-- phpMyAdmin SQL Dump
-- version 2.6.0-pl2
-- http://www.phpmyadmin.net
-- 
-- Máquina: localhost
-- Data de Criação: 08-Nov-2004 às 13:29
-- Versão do servidor: 4.0.20
-- versão do PHP: 4.3.9
-- 
-- Base de Dados: `ausws_dev`
-- 

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `ausws_admin`
-- 

CREATE TABLE `ausws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) TYPE=MyISAM;


-- --------------------------------------------------------

-- 
-- Estrutura da tabela `ausws_component`
-- 

CREATE TABLE `ausws_component` (
  `identification` varchar(32) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `password` varchar(20) NOT NULL default '',
  `certificate_xml` text,
  `other_data_xml` text,
  PRIMARY KEY  (`identification`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `ausws_user`
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
) TYPE=MyISAM;
