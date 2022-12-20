INCDIRS  += -I ${ETH_PATH}/src \
	    -I ${ETH_PATH}/api	\
	    -I ${ETH_PATH}/cfg \
	    -I ${ETH_PATH}/src/bsp/

LDFLAGS  += -g
CFLAGS   += -Werror ${INCDIRS} -g
ASFLAGS  += ${INCDIRS} -g

$(info compiling Lin source files)


ETH_OBJS := \
	${ETH_PATH}/src/Lin.o \
	${ETH_PATH}/cfg/Lin_cfg.o

