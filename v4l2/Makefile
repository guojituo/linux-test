#Invoking: GCC C Compiler
#gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"testCamera.d" -MT"testCamera.o" -o "testCamera.o" "../testCamera.c"
#Finished building: ../testCamera.c
# 
#Building target: testCamera
#Invoking: GCC C Linker
#gcc  -o "testCamera"  ./camera.o ./testCamera.o   
#Finished building target: testCamera


#make all 
#Building file: ../camera.c
#Invoking: Cross GCC Compiler
#aarch64-linux-gnu-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"camera.d" -MT"camera.o" -o "camera.o" "../camera.c"
#Finished building: ../camera.c
 
#Building file: ../testCamera.c
#Invoking: Cross GCC Compiler
#aarch64-linux-gnu-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"testCamera.d" -MT"testCamera.o" -o "testCamera.o" "../testCamera.c"
#Finished building: ../testCamera.c
 
#Building target: t
#Invoking: Cross GCC Linker
#aarch64-linux-gnu-gcc  -o "t"  ./camera.o ./testCamera.o   
#Finished building target: t

#　　$^  代表所有的依赖文件
#　　$@  代表所有的目标文件 
#　　$<  代表第一个依赖文件 

CROSS_COMPILE := /home/ubuntu/rk3399-linux/prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
GCC := ${CROSS_COMPILE}gcc

C_SRCS += \
v4l2-capture.c 

OBJS += \
v4l2-capture.o 

C_DEPS += \
v4l2-capture.d 

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	${GCC} -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# All Target
all: testCamera

# Tool invocations
testCamera: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	${GCC} -o "t" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) t
	-@echo ' '

