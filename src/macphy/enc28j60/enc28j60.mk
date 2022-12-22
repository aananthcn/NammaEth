INCDIRS  += -I ${ETH_PATH}/src/macphy/enc28j60

LDFLAGS  += -g
CFLAGS   += -Werror ${INCDIRS} -g
ASFLAGS  += ${INCDIRS} -g

$(info compiling ENC28J60 source files)


ETH_OBJS := \
	${ETH_PATH}/src/macphy/enc28j60/enc28j60.o 