STM32F407ZGT-Makefile

基于启明欣欣的STM32F407ZGT开发版实现的的一个共享工程，后续将不断添加他支持的功能

工程将支持freertos,lwip,paho-mqtt,letter-shell,sfud,fal,fatfs,easylogger,lvgl,cmbacktrace,nmea0183...第三方组件


项目工程介绍：

项目芯片型号：STM32F407ZGT6
资源介绍：32位 cortex-M4  时钟最高168M, 板载192k ram  1024k flash, pin 144 , 3个12位AD,2个12位DA,16个DMA通道，17个定时器，3个i2c,6个串口，3个spi,2个can2.0,2个usb otg,1个SDIO
工程使用makefile进行管理，gcc进行编译。

资源分配

外设		资源				管理					GPIO

SPI1		w25q128			sfud+fal+fatfs			sck--PB3,miso--PB4,mosi--PB5,cs--PG8

SPI2		st7789			LVGL					rst--PB0,cs--PB12,dc--PB14,blk--PB1,miso--PB13,mosi--PB15

SDIO		SD				fatfs					暂留

ETH			LAN8720			lwip					MDIO--PA2,MDC--PC1,CLK--PA1,DV--PA7,RXD0--PC4,RXD1--PC5,TXEN--PB11,EXD0--PG13,TXD1--PG14,RST--VCC

FSMC		SRAM			暂定					...(太多了，我就不列了)

USART1		DEBUG			easylogger				tx1--PA9,rx1--PA10

USART3		shell			letter-shell			tx3--PB10,rx3--PB11

USART6		gnss			NMEAS0183				tx6--PC6，rx6--PC7

I2C			AT24C02			暂定					scl--PB8,sda--PB9

GPIOG13		LED0			暂定					PG13

GPIOG14		LED1			暂定					PG14

GPIOF6		KEY0			暂定					PF6

GPIOF7		KEY1			暂定					PF7




编译环境搭建：

主机环境：windows 10/11

编译链工具：gcc-arm-none-eabi

make工具：MinGW

下载调试工具：OPENOCD

注：所有需要的工具存放在TOOL文件夹内

第一步：安装工具：

三个工具都为压缩文件，解压即可使用。其中因github的上传大小的限制编译链工具为分体压缩包，只需要解压其中一个，得到完整压缩包后在次解压即可。将三个得到的文件移动到C盘根目录下。

第二步：添加环境变量：

将三个路径中的\bin目录加入到环境变量Path中，详情如下

C:\gcc-arm-none-eabi-10.3-2021.10-win32\gcc-arm-none-eabi-10.3-2021.10\bin

C:\i686-8.1.0-release-posix-dwarf-rt v6-rev0\mingw32\bin

C:\openocd-20201228\OpenOCD-20201228-0.10.0\bin




第三步：检查环境

检查MinGW：在任意一个地方打开cmd或Powershell 使用gcc -v指令

检查编译链：使用arm-none-eabi-gcc -v命令

检查OPENOCD: openocd -v


第四步：修改MinGW需要将MinGW工具下的\mingw32\bin文件中的mingw32-make.exe文件复制粘贴到当前目录并重名了为make.exe(注：为了方便使用的make指令，而不是mingw32-make指令)


第五部：编译工程

cmd 进入工程目录或在工程目录打开cmd。

直接输入make命令即可进行编译

编译成功会有使用大小和生成文件的提示


如果为jLink、STLink、DAP等工具，在连接好设备后，如果使用dap调试器可以直接使用make download，兼容的命令如下，也可以自己定义，更多详情查看makefile文件

#烧录命令

download:

	-openocd -f TOOL/cmsis-dap-v1.cfg -f TOOL/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase build/$(TARGET).bin 0x08000000" -c reset -c shutdown
 
download_dap:

	-openocd -f TOOL/cmsis-dap-v1.cfg -f TOOL/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase build/$(TARGET).bin 0x08000000" -c reset -c shutdown
 
download_stlinkv2:

	-openocd -f TOOL/stlink-v2.cfg -f TOOL/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase build/$(TARGET).bin 0x08000000" -c reset -c shutdown
 
download_jlink:

	-openocd -f TOOL/jlink.cfg -f TOOL/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase build/$(TARGET).bin 0x08000000" -c reset -c shutdown
 

20240203修改-ChaseChen




# 20240505修改

1、包含三方库有 FatFs\LVGL\LWIP。 LWIP默认裁剪掉。

2、使用FreeRTOS进行任务调度，使用heap4进行内存管理。使用内部SRAM。

3、开启了外部SRAM，外部SRAM 速度受限，仅用于要求不高的任务中的大变量。

4、SPI1用于外部FLASH,SDIO用于SD卡，使用第三方库FATFS进行文件管理。

5、SPI2+DMA用于ST7789 屏幕，并使用LVGL进行页面绘制显示。并在my_gui文件中添加了码表的基础页面。

6、开启RTC时钟。

7、使用UART6+DMA与GPS进行数据交互。


# 20240510修改
1、添加开源NMEA0183解析器，开源地址 https://gitee.com/armxu/NMEA0183-C.git  仅做搭建时使用，后续有必要，需要重写

# 20240512修改
添加开源的LVGL页面管理器，完成基本的页面交互流程

# 20240826修改
一、移植paho.mqtt.embedded-c 库
链接：https://github.com/eclipse/paho.mqtt.embedded-c

库所在位置E:\STM32F407ZGT\Component\pahoMqtt

清理main.c 文件

功能宏如下：
BUILE_LWIP = y
BUILE_LVGL = n
BUILE_MQTT = y


# 20240828修改

封装Fatfs 的读写函数
nv_read() 直接调用进行数据
nv_write() 直接调用进行写数据

优化内存占用
   text    data     bss     dec     hex filename
 143172     164  101620  244956   3bcdc Build/QT201.elf


# 20240830修改

新增letter-shell 串口终端，用于串口调试功能。
功能源码来源：https://github.com/NevermindZZT/letter-shell

本工程使用uart3作为通信口，中断方式进行数据接收。

新增BUILE_LETTER_SHELL控制，且默认打开

# 20240831修改

1 修复letter-shell方向键不匹配问题
原因：串口丢数据导致。
解决方法：修改终端串口usart3为空闲中断 DMA接收不定长数据的方法。
在shellTask中将数据将数据逐个推到shellHandler()

优化：删除shellTask中的delay函数，采用信号量的方式进行，阻塞task.当串口进入空闲中断后，抛出信号量，通知shellTask读取数据进行处理


2 基于letter-shell ,实现简单的文件系统操作指令，ls,cd,mkdir,rm,nvread,nvwrite

3 基于letter-shell ,实现简单网络操作指令，dhcp(触发dhcp),ifconfig(仅能查询IP)

   text    data     bss     dec     hex filename

 349768     316  112580  462664   70f48 Build/QT201.elf


 # 20240905修改

 1、添加cmbacktrace 错误追踪库，移植源码：https://gitee.com/Armink/CmBacktrace

 2、添加os_asssert 指令主动dump模组

# 20240907修改

1、添加easylogger 日志库，源码：https://github.com/armink/EasyLogger

2、添加编译选项开启或关闭该功能，默认开启BUILE_EASYLOGGER = y

# 20240908修改
1、修改flash驱动，使用开源的SFUD 驱动框架管理flash，并对接fatfs文件系统，SFUD驱动地址：https://github.com/armink/SFUD/tree/master

2、添加FAL 中间层，并使用FAL 层对接SDUD ，FATFS 也使用FAL的接口进行操作。
