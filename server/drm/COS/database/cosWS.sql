-- phpMyAdmin SQL Dump
-- version 2.6.0-pl2
-- http://www.phpmyadmin.net
-- 
-- M�quina: localhost
-- Data de Cria��o: 08-Nov-2004 �s 13:31
-- Vers�o do servidor: 4.0.20
-- vers�o do PHP: 4.3.9
-- 
-- Base de Dados: `cosws_dev`
-- 

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `autoincr_account`
-- 

CREATE TABLE `autoincr_account` (
  `id` int(11) NOT NULL default '0'
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `autoincr_personalinfo`
-- 

CREATE TABLE `autoincr_personalinfo` (
  `id` int(11) NOT NULL default '0'
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `cosws_admin`
-- 

CREATE TABLE `cosws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `cosws_pgw`
-- 

CREATE TABLE `cosws_pgw` (
  `cos_IP` varchar(100) NOT NULL default '',
  `identification` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`cos_IP`)
) TYPE=MyISAM;

