################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CFG_SRCS += \
../MAX30100.cfg 

CPP_SRCS += \
../MAX30100.cpp 

CMD_SRCS += \
../CC2650STK.cmd 

C_SRCS += \
../CC2650STK.c \
../ccfg.c 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt 

GEN_MISC_DIRS += \
./configPkg/ 

C_DEPS += \
./CC2650STK.d \
./ccfg.d 

GEN_OPTS += \
./configPkg/compiler.opt 

OBJS += \
./CC2650STK.obj \
./MAX30100.obj \
./ccfg.obj 

CPP_DEPS += \
./MAX30100.d 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

OBJS__QUOTED += \
"CC2650STK.obj" \
"MAX30100.obj" \
"ccfg.obj" 

C_DEPS__QUOTED += \
"CC2650STK.d" \
"ccfg.d" 

CPP_DEPS__QUOTED += \
"MAX30100.d" 

GEN_FILES__QUOTED += \
"configPkg\linker.cmd" \
"configPkg\compiler.opt" 

C_SRCS__QUOTED += \
"../CC2650STK.c" \
"../ccfg.c" 

CPP_SRCS__QUOTED += \
"../MAX30100.cpp" 


