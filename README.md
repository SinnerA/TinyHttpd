# TinyHttpd
小型的http服务器

1.服务器的架构为epoll + 多线程 + sendfile

2.暂时只支持GET和HEAD方法

3.可配置的，domain就是网站域名，docroot是文件根目录

4.编译方式: make

5.运行方式: ./main ./TinyHttpd/tinyhttpd.config
