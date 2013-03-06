-- phpMyAdmin SQL Dump
-- version 2.6.0-pl2
-- http://www.phpmyadmin.net
-- 
-- Máquina: localhost
-- Data de Criação: 08-Nov-2004 às 13:32
-- Versão do servidor: 4.0.20
-- versão do PHP: 4.3.9
-- 
-- Base de Dados: `lisws_dev`
-- 

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `content_keys`
-- 

CREATE TABLE `content_keys` (
  `cid` varchar(100) NOT NULL default '',
  `ckey` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`cid`,`ckey`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `license`
-- 

CREATE TABLE `license` (
  `id` varchar(100) NOT NULL default '0',
  `template_id` varchar(100) NOT NULL default '',
  `uid` varchar(100) NOT NULL default '',
  `cid` varchar(100) NOT NULL default '',
  `license` text NOT NULL,
  PRIMARY KEY  (`id`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `license_template`
-- 

CREATE TABLE `license_template` (
  `id` int(11) NOT NULL default '0',
  `type` varchar(100) NOT NULL default '',
  `description` text NOT NULL,
  `content_type` varchar(100) NOT NULL default '',
  `num_params` int(11) NOT NULL default '0',
  `license_template` text NOT NULL,
  PRIMARY KEY  (`id`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `lisws_admin`
-- 

CREATE TABLE `lisws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `log_licenses`
-- 

CREATE TABLE `log_licenses` (
  `timestamp` varchar(100) NOT NULL default '',
  `operation` varchar(100) NOT NULL default '',
  `description` text NOT NULL,
  `uid` varchar(100) NOT NULL default '',
  `lic_id` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`timestamp`)
) TYPE=MyISAM;
        		
