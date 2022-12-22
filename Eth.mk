INCDIRS  += -I ${ETH_PATH}/src \
	    -I ${ETH_PATH}/api	\
	    -I ${ETH_PATH}/cfg \
	    -I ${ETH_PATH}/src/macphy \
	    -I ${ETH_PATH}/src/bsp

LDFLAGS  += -g
CFLAGS   += -Werror ${INCDIRS} -g
ASFLAGS  += ${INCDIRS} -g

$(info compiling Eth source files)


ETH_OBJS := \
	${ETH_PATH}/src/Eth.o \
	${ETH_PATH}/cfg/Eth_cfg.o

