CauchyDB
=========


A Database



# Function List

## Structure

create database [db-name];

drop database [db-name];

use [db-name];


create table [table-name] (

[column-name] [int, char(x), float] {unique},

primary key ( [column name] )

);

drop table [table-name];

create index [index-name] on [table-name] ( [column-name] );

drop index [index-name] on [table-name];

## Query

select ... from [table-name] where [condition-list];

insert into [table-name] ([column-name], ...) values (value1, value2);



# LICENSE

THIS PROJECT IS OPEN SOURCE UNDER MIT LICENSE, BUT WITH A LIMITATION

WITH THE "HONEST RULE" REQUIRED BY Prof. Chen Yue, Zhejiang University 

ANYONE CANNOT USE ANY PART IN THIS PROJECT IN "DATABASE SYSTEM" COURSE IN ZJU.

根据浙江大学学术诚信守则。

任何人不可以使用本项目中的任何一部分代码作为“数据库系统”的大程序的组成部分。

违反本条例的任何后果, 本人不承担任何责任。



