# 实验报告

**`171240501 匡舒磊`** 

**`kuangsl@foxmail.com 基础班`**

## 已实现内容

完成了所有必做内容与选做内容。实现了部分比较简单的错误处理。

## 代码框架

`lexical.l` : 词法内容识别

`syntax.y` : 语法内容分析，并建立相应的语法树节点

`common.h` : 函数定义，语法树节点定义

`ast.c` : 语法树的相关函数，包括添加节点打印语法树等

`main.c`: 运行分析

## 如何编译

使用助教提供的Makefile进行编译生成parser

## 实验环境

GNU Linux Realease: Ubuntu 18.04.4 LTS

GCC version 7.5.0 

GNU Flex 2.6.4

GNU Bison 3.0.4
