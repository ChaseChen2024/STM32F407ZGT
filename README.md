STM32F407ZGT-Makefile

项目工程介绍：

基于启明欣欣的STM32F407ZGT开发板实现的的一个SDK工程，后续将不断添加他支持的功能

工程将支持freertos,lwip,paho-mqtt,letter-shell,sfud,fal,fatfs,easylogger,lvgl,cmbacktrace,nmea0183...第三方组件
作为bootloader使用https://github.com/ChaseChen2024/STM32F407ZGT-BOOTLOADER.git，实现fota升级。


项目芯片型号：STM32F407ZGT6

资源介绍：32位 cortex-M4  时钟最高168M, 板载192k ram  1024k flash, pin 144 , 3个12位AD,2个12位DA,16个DMA通道，17个定时器，3个i2c,6个串口，3个spi,2个can2.0,2个usb otg,1个SDIO
工程使用makefile进行管理，gcc进行编译。

资源分配
| 外设   | 	资源 	 |		管理	| 											GPIO 													|
| -------|----------|---------------|---------------------------------------------------------------------------------------------------|
| SPI1   | w25q128	| sfud+fal+fatfs| sck--PB3,miso--PB4,mosi--PB5,cs--PG8																|
| SPI2   | st7789   | LVGL          | rst--PB0,cs--PB12,dc--PB14,blk--PB1,miso--PB13,mosi--PB15											| 
| SDIO   | SD       | fatfs         | 暂留																								| 
| ETH    | LAN8720  | lwip          | MDIO--PA2,MDC--PC1,CLK--PA1,DV--PA7,RXD0--PC4,RXD1--PC5,TXEN--PB11,EXD0--PG13,TXD1--PG14,RST--VCC | 
| FSMC   | SRAM     | 暂定           | ...(太多了，我就不列了)																			  | 
| USART3 | DEBUG	| easylogger    | tx1--PA9,rx1--PA10																				| 
| USART1 | shell    | letter-shell  | tx3--PB10,rx3--PB11																				| 
| USART6 | gnss     | NMEAS0183     | tx6--PC6，rx6--PC7																				| 
| I2C    | AT24C02  | 暂定           | scl--PB8,sda--PB9																				| 
| GPIOG13| LED0	    | 暂定           | PG13																								| 
| GPIOG14| LED1     | 暂定           | PG14																								| 
| GPIOF6 | KEY0     | 暂定           | PF6																								| 
| GPIOF7 | KEY1     | 暂定           | PF7																								| 



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

make down 下载bootloader固件和app固件，make down_app 下载app固件

down:
	-openocd -f TOOL/debug/stlink.cfg -f TOOL/debug/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase Release/bootloader.bin 0x08000000" -c reset -c shutdown \
	&& openocd -f TOOL/debug/stlink.cfg -f TOOL/debug/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase build/$(TARGET).bin 0x08020000" -c reset -c shutdown 


down_app:
	-openocd -f TOOL/debug/stlink.cfg -f TOOL/debug/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase build/$(TARGET).bin 0x08020000" -c reset -c shutdown 
	

#使用build_new.bat脚本编译完后，使用该指令烧录
down_all:
	-openocd -f TOOL/debug/stlink.cfg -f TOOL/debug/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase Release/bootloader.bin 0x08000000" -c reset -c shutdown \
	&& openocd -f TOOL/debug/stlink.cfg -f TOOL/debug/stm32f4x.cfg -c init -c "reset halt;wait_halt;flash write_image erase Release/$(TARGET).bin 0x08020000" -c reset -c shutdown 

bootloader编译，需要进入STM32F407ZGT\Component\bootloader\STM32F407ZGT-BOOTLOADER目录下，make编译bootloader工程，之后将\STM32F407ZGT-BOOTLOADER\Build\QT201-BOOT.bin复制到STM32F407ZGT\Release路径下并重命名为bootloader.bin，之后就能在STM32F407ZGT工程使用make down进行bootloader和application一同烧录到开发板中了
也可以在STM32F407ZGT目录下，打开终端，使用build_new.bat脚本编译,编译完成后，脚本会把所有需要用到的文件复制到STM32F407ZGT\Release路径下，在STM32F407ZGT目录下可以使用make down_all 执行下载


TFTP使用：
工具路径TOOL\tftpd\tftpd64.exe ，打开后，选择Tftp Client ，在Host 输入开发板的地址（可以在shell上使用ifconfig指令查询），Port填入tftp的默认端口 69

Local File 填入本地的文件路径
Remote file填入要在开发板中打开或创建的文件名
其他默认
点击put就会将Local File传递到开发板中，如果Remote file文件存在，就会进行内容覆盖，不存在就会以Remote file为名创建新的文件。
点击get就会从开发板获取Remote file文件传输到本地，并以Local File 创建或覆盖文件



# 分支描述

release-1.0 支持基本的fatfs lvgl nmea0183解析器

release-1.1 支持rt-fota bootloader、sfud、fal、lwip 、easyloger、cmbacktrace、letter-shell


# 使用到的开源库


| 功能   | 	库地址 	 |
| -------|----------|
|NMEA0183|https://gitee.com/armxu/NMEA0183-C.git|
|paho.mqtt.embedded-c |https://github.com/eclipse/paho.mqtt.embedded-c|
|letter-shell|https://github.com/NevermindZZT/letter-shell|
|cmbacktrace |https://gitee.com/Armink/CmBacktrace|
|easylogger|https://github.com/armink/EasyLogger|
|SFUD|https://github.com/armink/SFUD/tree/master|
|FAL |https://gitee.com/RT-Thread-Mirror/fal|
|rt-fota |https://gitee.com/spunky_973/rt-fota |
|lwip|https://github.com/lwip-tcpip/lwip|
|lvgl|https://github.com/lvgl/lvgl|


