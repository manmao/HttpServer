#
#包含了其他makefile都用到的模式规则
#

# cross compile...
CROSS_COMPILE =

CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar

 #生成的目标文件名.......
TARGET := http_cpp

#顶层目录
ROOT := $(shell pwd)

#lib库
LIB =-lstdc++ -lpthread  -lz

##需要扫描的文件
TARGET_DIRS :=src/  \
				src/common  \
				src/pool \
				src/socket \
				src/cgi \
				src/http_codec \
				src/http_codec/api src/http_codec/http  src/http_codec/json \


#.h 头文件目录
INCLUDE_DIR :=  \
 	-I$(MAKEROOT)/inc  \
	-I$(MAKEROOT)/src/common   \
	-I$(MAKEROOT)/src/pool \
	-I$(MAKEROOT)/src/socket \
	-I$(MAKEROOT)/src/cgi \
	-I$(MAKEROOT)/src/http_codec \
	-I$(MAKEROOT)/src/http_codec\api \
	-I$(MAKEROOT)/src/http_codec\http \
	-I$(MAKEROOT)/src/http_codec\json \

#lib静态库搜索路劲
LIB_DIR := -L$(MAKEROOT)/lib \

CFLAGS  := $(INCLUDE_DIR)

CFLAGS += -c -Wall -O2 -g -D_USE_HTTP_PROTO_

#对所有的.o文件以.c文件创建它

$(MAKEROOT)/obj/%.o :  %.c
	@echo "Compling: " $(addsuffix .c, $(basename $(notdir $@)))
	${CC} ${CFLAGS} -c $< -o $@

$(MAKEROOT)/obj/%.o :  %.cpp
	@echo "Compling: " $(addsuffix .cpp, $(basename $(notdir $@)))
	${CXX} ${CFLAGS} -c $< -o $@