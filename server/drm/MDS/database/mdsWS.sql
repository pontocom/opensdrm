-- phpMyAdmin SQL Dump
-- version 2.6.0-pl2
-- http://www.phpmyadmin.net
-- 
-- Máquina: localhost
-- Data de Criação: 08-Nov-2004 às 13:32
-- Versão do servidor: 4.0.20
-- versão do PHP: 4.3.9
-- 
-- Base de Dados: `mdsws_dev`
-- 

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `content_delivery`
-- 

CREATE TABLE `content_delivery` (
  `id` varchar(100) NOT NULL default '',
  `cid` varchar(100) NOT NULL default '',
  `uid` varchar(100) NOT NULL default '',
  `status` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`id`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `content_location`
-- 

CREATE TABLE `content_location` (
  `id` varchar(100) NOT NULL default '',
  `cid` varchar(100) NOT NULL default '',
  `type` varchar(100) NOT NULL default '',
  `protocol` varchar(100) NOT NULL default '',
  `location` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`id`,`cid`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `mdsws_admin`
-- 

CREATE TABLE `mdsws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) TYPE=MyISAM;
        
