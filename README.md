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


