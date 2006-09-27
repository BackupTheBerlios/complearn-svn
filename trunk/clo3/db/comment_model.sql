CREATE TABLE comments (
  `id` int(11) NOT NULL auto_increment,
  `lab_id` int(11) NOT NULL,
  `username` VARCHAR(255),
  `str`      VARCHAR(1023),
  created_at datetime,
  PRIMARY KEY  (`id`)
) TYPE=MyISAM;
