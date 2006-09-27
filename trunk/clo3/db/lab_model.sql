CREATE TABLE labs (
  `id` int(11) NOT NULL auto_increment,
  `codenum` int(11) NOT NULL,
  showcount int NOT NULL DEFAULT '0',
  created_at datetime,
  modified_at datetime,
  PRIMARY KEY  (`id`)
) TYPE=MyISAM;
