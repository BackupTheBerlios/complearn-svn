create table r_files (
  `id` int(11) NOT NULL auto_increment,
  `fullpath` VARCHAR(1024),
  `devicename` VARCHAR(255) NOT NULL,
  `mimetype` VARCHAR(128),
  `encoding` VARCHAR(128),
  `contents` BLOB,
  `created_at` DATETIME,
  `modified_at` DATETIME,
  PRIMARY KEY  (`id`),
  INDEX USING BTREE (fullpath(1024)),
  INDEX USING BTREE (contents(128)),
  INDEX USING BTREE (devicename),
  INDEX USING BTREE (mimetype),
  INDEX USING BTREE (created_at),
  INDEX USING BTREE (modified_at)
) TYPE=MyISAM;
