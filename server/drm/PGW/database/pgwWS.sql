-- phpMyAdmin SQL Dump
-- version 2.6.0-pl2
-- http://www.phpmyadmin.net
-- 
-- Máquina: localhost
-- Data de Criação: 08-Nov-2004 às 13:33
-- Versão do servidor: 4.0.20
-- versão do PHP: 4.3.9
-- 
-- Base de Dados: `pgwws_dev`
-- 

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `pgwws_admin`
-- 

CREATE TABLE `pgwws_admin` (
  `login` varchar(20) NOT NULL default '',
  `password` varchar(10) NOT NULL default '',
  `public_key_xml` text NOT NULL,
  `private_key_xml` text NOT NULL,
  `certificate_AUS` text NOT NULL,
  PRIMARY KEY  (`login`,`password`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `pgwws_cos`
-- 

CREATE TABLE `pgwws_cos` (
  `identification` varchar(32) NOT NULL default '',
  `certificate_xml` text,
  `other_data_xml` text,
  PRIMARY KEY  (`identification`)
) TYPE=MyISAM;

-- --------------------------------------------------------

-- 
-- Estrutura da tabela `pgwws_transaction`
-- 

CREATE TABLE `pgwws_transaction` (
  `transaction_number` varchar(100) NOT NULL default '',
  `cos_id` varchar(100) NOT NULL default '',
  `pay_data` text NOT NULL,
  `status` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`transaction_number`)
) TYPE=MyISAM;
        
