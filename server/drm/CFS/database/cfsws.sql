-- phpMyAdmin SQL Dump
-- version 2.6.0-pl2
-- http://www.phpmyadmin.net
-- 
-- M�quina: localhost
-- Data de Cria��o: 08-Nov-2004 �s 13:30
-- Vers�o do servidor: 4.0.20
-- vers�o do PHP: 4.3.9
-- 
-- Base de Dados: `cfsws_dev`
-- 

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `cfsws_admin`
-- 

CREATE TABLE `cfsws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `location`
-- 

CREATE TABLE `location` (
  `id` varchar(32) NOT NULL default '',
  `location` varchar(250) NOT NULL default '',
  PRIMARY KEY  (`id`)
) TYPE=MyISAM;
        
