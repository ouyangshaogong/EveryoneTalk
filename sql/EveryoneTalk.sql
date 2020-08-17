#创建用户表
CREATE DATABASE IF NOT EXISTS everyone_talk ;
#删除数据库
DROP DATABASE IF EXISTS everyone_talk;

#删除表
DROP TABLE IF EXISTS `user_info`;
#创建用户表
CREATE TABLE user_info(
	id INT NOT NULL AUTO_INCREMENT,#编号
	username VARCHAR(20) NOT NULL,#用户名
	passwd VARCHAR(20) NOT NULL,#密码
	sex  VARCHAR(20) NOT NULL,#性别
	birth DATETIME NOT NULL,#出生日期
	hometown  VARCHAR(20) NOT NULL,
	PRIMARY KEY (`id`)
)ENGINE=INNODB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS `login_info`;
#创建登录表
CREATE TABLE login_info (
	id INT NOT NULL AUTO_INCREMENT,#编号
	user_name VARCHAR(20) NOT NULL,#用户名
	c_ip VARCHAR(20) NOT NULL,#客户端ip
	c_port INT NOT NULL,#客户端port
	login_time DATETIME NOT NULL,#出生日期
	state  INT NOT NULL,
	user_id INT NOT NULL,
	PRIMARY KEY (id),
	FOREIGN KEY(user_id) REFERENCES user_info(id)
)ENGINE=INNODB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;

#创建用户表
CREATE TABLE friend_info (
	id INT NOT NULL AUTO_INCREMENT,#编号
	user_id INT NOT NULL,
	friend_id INT NOT NULL,
	PRIMARY KEY (id),
	FOREIGN KEY(user_id) REFERENCES user_info(id),
	FOREIGN KEY(friend_id) REFERENCES user_info(id)
)ENGINE=INNODB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8;

INSERT INTO user_info(id,username,passwd,sex,birth,hometown) VALUES (1, 'James', 'qazwsx', 'man', '1992-04-03 00:00:00', 'New York');
INSERT INTO user_info(id,username,passwd,sex,birth,hometown) VALUES (2, 'joy', 'qazwsx', 'woman', '1982-04-03 00:00:00', 'xi.an');
INSERT INTO user_info(id,username,passwd,sex,birth,hometown) VALUES (3, 'zhangwuji', 'qazwsx', 'man', '1992-04-03 00:00:00', 'chengdou');
INSERT INTO user_info(id,username,passwd,sex,birth,hometown) VALUES (4, 'Bob', 'qazwsx', 'man', '1992-04-03 00:00:00', 'beijing');

SELECT * FROM user_info

UPDATE user_info SET username='zhangwuji' WHERE id = 3;

SELECT * FROM login_info
DELETE FROM login_info

UPDATE login_info SET state=2 WHERE user_name='waner' AND login_time='2020-8-6 3:5:52'

INSERT INTO login_info(user_name,ip,login_time,state,user_id) VALUES ('James','127.0.0.1','1992-04-03 00:00:00',0,1);

UPDATE login_info SET state=0 WHERE id=60;
UPDATE login_info SET state=1 WHERE user_name = 'waner' AND state != 1 ;

SELECT * FROM friend_info

INSERT INTO friend_info(user_id,friend_id) VALUES (7,4);
INSERT INTO friend_info(user_id,friend_id) VALUES (7,5);