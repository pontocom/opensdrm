-- phpMyAdmin SQL Dump
-- version 2.6.0-pl2
-- http://www.phpmyadmin.net
-- 
-- Máquina: localhost
-- Data de Criação: 08-Nov-2004 às 13:34
-- Versão do servidor: 4.0.20
-- versão do PHP: 4.3.9
-- 
-- Base de Dados: `rgsws_dev`
-- 

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `rgsws_admin`
-- 

CREATE TABLE `rgsws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `rgsws_content`
-- 

CREATE TABLE `rgsws_content` (
  `content_id` varchar(128) NOT NULL default '',
  `internal_id` int(11) default NULL,
  `hash` text NOT NULL,
  `file` varchar(254) NOT NULL default '',
  `metadata` text,
  PRIMARY KEY  (`content_id`)
) TYPE=MyISAM;
        
