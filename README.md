STM32F407ZGT
基于STM32F407ZGT的一个共享工程，后续将不断添加他支持的功能


项目工程介绍：

项目芯片型号：STM32F407ZGT6
资源介绍：32位 cortex-M4  时钟最高168M, 板载192k ram  1024k flash, pin 144 , 3个12位AD,2个12位DA,16个DMA通道，17个定时器，3个i2c,6个串口，3个spi,2个can2.0,2个usb otg,1个SDIO
工程使用makefile进行管理，gcc进行编译。


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




# 20240505修改——大调整。以实现码表为方向

1、包含三方库有 FatFs\LVGL\LWIP。 LWIP默认裁剪掉。

2、使用FreeRTOS进行任务调度，使用heap4进行内存管理。使用内部SRAM。

3、开启了外部SRAM，外部SRAM 速度受限，仅用于要求不高的任务中的大变量。

4、SPI1用于外部FLASH,SDIO用于SD卡，使用第三方库FATFS进行文件管理。

5、SPI2+DMA用于ST7789 屏幕，并使用LVGL进行页面绘制显示。并在my_gui文件中添加了码表的基础页面。

6、开启RTC时钟。

7、使用UART6+DMA与GPS进行数据交互。
