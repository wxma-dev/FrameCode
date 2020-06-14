#|----------------------------------------------------------------------------|
#|                           TOPMake 2.0                                      |
#| Copyright (c) 2000-2003 Shanghai Huateng Software Systems Co., Ltd.        |
#|    All Rights Reserved                                                     |
#|----------------------------------------------------------------------------|
#| FILE NAME    : bas.mak                                                     |
#| DESCRIPTIONS : Makefile for basic module                                   |
#|----------------------------------------------------------------------------|

include $(MKHOME)/mak/platform.mak
include $(MKHOME)/mak/database.mak
include $(MKHOME)/mak/tuxedo.mak

PRGPATH = $(MKHOME)/src/bas
PRGOBJS = \
	$(PRGPATH)/bas_date.o \
	$(PRGPATH)/bas_log.o \
	$(PRGPATH)/bas_err.o \
	$(PRGPATH)/bas_file.o \
	$(PRGPATH)/bas_string.o \
	$(PRGPATH)/bas_usrlog.o \
	$(PRGPATH)/bas_profile.o \
	$(PRGPATH)/bas_system.o \
	$(PRGPATH)/bas_genathcd.o \
	$(PRGPATH)/bas_hash_table.o \
	$(PRGPATH)/bas_shm_search_table.o \
	$(PRGPATH)/bas_shm_config.o \
	$(PRGPATH)/bas_expand_macro.o \
	$(PRGPATH)/bas_hex_util.o \
	$(PRGPATH)/bas_xml.o \
	$(PRGPATH)/bas_getopt.o \
	$(PRGPATH)/bas_encrypt.o \
	$(PRGPATH)/bas_encString.o \

PRGTARG = topbas
PRGDEFS =
PRGLIBS = -lxml2 $(ICONV)

# used for db2 database
PRGDBNM = 

debug all: debugdynamic
release: releasedynamic

# DO NOT modify any code below!!!

releasedynamic debugdynamic releasestatic debugstatic releaseexec debugexec clean:
	@make -f $(MKHOME)/mak/mkstand.mak $@ PUBTYPE=$@ TARGET="$(PRGTARG)" OBJS="$(PRGOBJS)" LIBS="$(PRGLIBS)" DEFS="$(PRGDEFS)" DBNM="$(PRGDBNM)"
