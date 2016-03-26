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


##需要扫描的文件
TARGET_DIRS :=src/  \
				lib/  lib/error\
				src/common  \
				src/pool \
				src/socket \
				src/cgi \
				src/servlet \
				src/http_codec \
				src/http_codec/api src/http_codec/http  src/http_codec/json \
				src/shm \
				src/common/http_inf \
				src/ssl


#.h 头文件目录
INCLUDE_DIR :=  \
 	-I$(MAKEROOT)/inc  \
	-I$(MAKEROOT)/src/common   \
	-I$(MAKEROOT)/src/ssl   \
	-I$(MAKEROOT)/src/pool \
	-I$(MAKEROOT)/src/socket \
	-I$(MAKEROOT)/src/cgi \
	-I$(MAKEROOT)/src/shm \
	-I$(MAKEROOT)/src/servlet \
	-I$(MAKEROOT)/src/http_codec \
	-I$(MAKEROOT)/src/http_codec\api \
	-I$(MAKEROOT)/src/http_codec\http \
	-I$(MAKEROOT)/src/http_codec\json \
	-I$(MAKEROOT)/src/common/http_inf \


#lib库
LIB :=  -lstdc++ -lpthread  -lz

#lib静态库搜索路劲
LIB_DIR := -L$(MAKEROOT)/lib

CFLAGS  := $(INCLUDE_DIR)
CFLAGS += -g -Wall -O2


ifeq ($(USE_HTTP_SSL),TRUE)
	CFLAGS += -D_USE_HTTP_SSL
	LIB += -lssl -lcrypto
endif


#对所有的.o文件以.c文件创建它
$(MAKEROOT)/obj/%.o :  %.c
	@echo "Compling: " $(addsuffix .c, $(basename $(notdir $@)))
	${CC} ${CFLAGS} -c $< -o $@

$(MAKEROOT)/obj/%.o :  %.cpp
	@echo "Compling: " $(addsuffix .cpp, $(basename $(notdir $@)))
	${CXX} ${CFLAGS} -c $< -o $@