/*
Navicat MySQL Data Transfer

Source Server         : 65_dev
Source Server Version : 50518
Source Host           : 192.168.1.65:3306
Source Database       : gds_2011

Target Server Type    : MYSQL
Target Server Version : 50518
File Encoding         : 65001

Date: 2011-12-13 20:51:15
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `config_t`
-- ----------------------------
DROP TABLE IF EXISTS `config_t`;
CREATE TABLE `config_t` (
  `para_name` varchar(255) NOT NULL,
  `para_value` varchar(255) DEFAULT NULL,
  `description` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`para_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of config_t
-- ----------------------------

-- ----------------------------
-- Table structure for `task_t`
-- ----------------------------
DROP TABLE IF EXISTS `task_t`;
CREATE TABLE `task_t` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `task_id` varchar(10) NOT NULL,
  `type` char(1) NOT NULL,
  `date_added` datetime NOT NULL,
  `path` varchar(500) NOT NULL,
  `description` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of task_t
-- ----------------------------
