#include ${SRCDIR}/etc/Make.defines

OBJ_YLGMT_QK = Bin_Text.o Bin_DelCodeAnnotation.o
EXE_YLGMT_QK = Bin_Text Bin_DelCodeAnnotation

#LIBS=-L../Frame_Lib/lib  -lPUB 

TARGETS = ${EXE_YLGMT_QK}

CLEANFILES = ${EXE_YLGMT_QK} \
		${OBJ_YLGMT_QK}

all:${TARGETS}

clean:
	rm -f ${CLEANFILES}

Bin_Text.o:
	${CC} -o Bin_Text.o -c Bin_Text.c -g

Bin_DelCodeAnnotation.o:
	${CC} -o Bin_DelCodeAnnotation.o -c Bin_DelCodeAnnotation.c -g

Bin_Text: Bin_Text.o
	${CC} -o Bin_Text Bin_Text.o ${LIBS} -g

Bin_DelCodeAnnotation: Bin_DelCodeAnnotation.o
	${CC} -o Bin_DelCodeAnnotation Bin_DelCodeAnnotation.o ${LIBS} -g


