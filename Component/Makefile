
ifeq ($(BUILE_LWIP),y)
# C sources
C_SOURCES +=  \
Component/LWIP/api/api_lib.c \
Component/LWIP/api/api_msg.c \
Component/LWIP/api/err.c \
Component/LWIP/api/if_api.c \
Component/LWIP/api/netbuf.c \
Component/LWIP/api/netdb.c \
Component/LWIP/api/sockets.c \
Component/LWIP/api/tcpip.c \
Component/LWIP/core/ipv4/autoip.c \
Component/LWIP/core/ipv4/dhcp.c \
Component/LWIP/core/ipv4/etharp.c \
Component/LWIP/core/ipv4/icmp.c \
Component/LWIP/core/ipv4/igmp.c \
Component/LWIP/core/ipv4/ip4.c \
Component/LWIP/core/ipv4/ip4_addr.c \
Component/LWIP/core/ipv4/ip4_frag.c \
Component/LWIP/core/altcp.c \
Component/LWIP/core/altcp_alloc.c \
Component/LWIP/core/altcp_tcp.c \
Component/LWIP/core/def.c \
Component/LWIP/core/dns.c \
Component/LWIP/core/inet_chksum.c \
Component/LWIP/core/init.c \
Component/LWIP/core/ip.c \
Component/LWIP/core/mem.c \
Component/LWIP/core/memp.c \
Component/LWIP/core/netif.c \
Component/LWIP/core/pbuf.c \
Component/LWIP/core/raw.c \
Component/LWIP/core/stats.c \
Component/LWIP/core/sys.c \
Component/LWIP/core/tcp.c \
Component/LWIP/core/tcp_in.c \
Component/LWIP/core/tcp_out.c \
Component/LWIP/core/timeouts.c \
Component/LWIP/core/udp.c \
Component/LWIP/netif/ethernet.c \
Component/LWIP/netif/lowpan6.c \
Component/LWIP/netif/lowpan6_ble.c \
Component/LWIP/netif/lowpan6_common.c \
Component/LWIP/netif/zepif.c \
Component/LWIP/netif/bridgeif.c \
Component/LWIP/apps/sntp/sntp.c \
Component/LWIP/apps/tftp/tftp_server.c \

endif

#fatfs
C_SOURCES +=  \
Component/FatFs/diskio.c \
Component/FatFs/ff.c \
Component/FatFs/option/cc936.c \
Component/FatFs/option/syscall.c \

#lvgl
C_SOURCES +=  \
Component/lvgl/src/core/lv_disp.c \
Component/lvgl/src/core/lv_event.c \
Component/lvgl/src/core/lv_group.c \
Component/lvgl/src/core/lv_indev_scroll.c \
Component/lvgl/src/core/lv_indev.c \
Component/lvgl/src/core/lv_obj_class.c \
Component/lvgl/src/core/lv_obj_draw.c \
Component/lvgl/src/core/lv_obj_pos.c \
Component/lvgl/src/core/lv_obj_scroll.c \
Component/lvgl/src/core/lv_obj_style_gen.c \
Component/lvgl/src/core/lv_obj_style.c \
Component/lvgl/src/core/lv_obj_tree.c \
Component/lvgl/src/core/lv_obj.c \
Component/lvgl/src/core/lv_refr.c \
Component/lvgl/src/core/lv_theme.c \
Component/lvgl/src/draw/sdl/lv_draw_sdl_arc.c \
Component/lvgl/src/draw/sdl/lv_draw_sdl_bg.c \
Component/lvgl/src/draw/sdl/lv_draw_sdl_composite.c \
Component/lvgl/src/draw/sdl/lv_draw_sdl_img.c \
Component/lvgl/src/draw/sdl/lv_draw_sdl_label.c \
Component/lvgl/src/draw/sdl/lv_draw_sdl_line.c \
Component/lvgl/src/draw/sdl/lv_draw_sdl_mask.c \
Component/lvgl/src/draw/sdl/lv_draw_sdl_polygon.c \
Component/lvgl/src/draw/sdl/lv_draw_sdl_rect.c \
Component/lvgl/src/draw/sdl/lv_draw_sdl_stack_blur.c \
Component/lvgl/src/draw/sdl/lv_draw_sdl_texture_cache.c \
Component/lvgl/src/draw/sdl/lv_draw_sdl_utils.c \
Component/lvgl/src/draw/sdl/lv_draw_sdl.c \
Component/lvgl/src/draw/stm32_dma2d/lv_gpu_stm32_dma2d.c \
Component/lvgl/src/draw/sw/lv_draw_sw_arc.c \
Component/lvgl/src/draw/sw/lv_draw_sw_blend.c \
Component/lvgl/src/draw/sw/lv_draw_sw_dither.c \
Component/lvgl/src/draw/sw/lv_draw_sw_gradient.c \
Component/lvgl/src/draw/sw/lv_draw_sw_img.c \
Component/lvgl/src/draw/sw/lv_draw_sw_letter.c \
Component/lvgl/src/draw/sw/lv_draw_sw_line.c \
Component/lvgl/src/draw/sw/lv_draw_sw_polygon.c \
Component/lvgl/src/draw/sw/lv_draw_sw_rect.c \
Component/lvgl/src/draw/sw/lv_draw_sw.c \
Component/lvgl/src/draw/lv_draw_arc.c \
Component/lvgl/src/draw/lv_draw_img.c \
Component/lvgl/src/draw/lv_draw_label.c \
Component/lvgl/src/draw/lv_draw_line.c \
Component/lvgl/src/draw/lv_draw_mask.c \
Component/lvgl/src/draw/lv_draw_rect.c \
Component/lvgl/src/draw/lv_draw_triangle.c \
Component/lvgl/src/draw/lv_img_buf.c \
Component/lvgl/src/draw/lv_img_cache.c \
Component/lvgl/src/draw/lv_img_decoder.c \
Component/lvgl/src/extra/layouts/flex/lv_flex.c \
Component/lvgl/src/extra/layouts/grid/lv_grid.c \
Component/lvgl/src/extra/libs/bmp/lv_bmp.c \
Component/lvgl/src/extra/libs/ffmpeg/lv_ffmpeg.c \
Component/lvgl/src/extra/libs/freetype/lv_freetype.c \
Component/lvgl/src/extra/libs/fsdrv/lv_fs_fatfs.c \
Component/lvgl/src/extra/libs/fsdrv/lv_fs_posix.c \
Component/lvgl/src/extra/libs/fsdrv/lv_fs_stdio.c \
Component/lvgl/src/extra/libs/fsdrv/lv_fs_win32.c \
Component/lvgl/src/extra/libs/gif/gifdec.c \
Component/lvgl/src/extra/libs/gif/lv_gif.c \
Component/lvgl/src/extra/libs/png/lodepng.c \
Component/lvgl/src/extra/libs/png/lv_png.c \
Component/lvgl/src/extra/libs/qrcode/lv_qrcode.c \
Component/lvgl/src/extra/libs/qrcode/qrcodegen.c \
Component/lvgl/src/extra/libs/rlottie/lv_rlottie.c \
Component/lvgl/src/extra/libs/sjpg/lv_sjpg.c \
Component/lvgl/src/extra/libs/sjpg/tjpgd.c \
Component/lvgl/src/extra/others/gridnav/lv_gridnav.c \
Component/lvgl/src/extra/others/monkey/lv_monkey.c \
Component/lvgl/src/extra/others/snapshot/lv_snapshot.c \
Component/lvgl/src/extra/themes/basic/lv_theme_basic.c \
Component/lvgl/src/extra/themes/default/lv_theme_default.c \
Component/lvgl/src/extra/themes/mono/lv_theme_mono.c \
Component/lvgl/src/extra/widgets/animimg/lv_animimg.c \
Component/lvgl/src/extra/widgets/calendar/lv_calendar_header_arrow.c \
Component/lvgl/src/extra/widgets/calendar/lv_calendar_header_dropdown.c \
Component/lvgl/src/extra/widgets/calendar/lv_calendar.c \
Component/lvgl/src/extra/widgets/chart/lv_chart.c \
Component/lvgl/src/extra/widgets/colorwheel/lv_colorwheel.c \
Component/lvgl/src/extra/widgets/imgbtn/lv_imgbtn.c \
Component/lvgl/src/extra/widgets/keyboard/lv_keyboard.c \
Component/lvgl/src/extra/widgets/led/lv_led.c \
Component/lvgl/src/extra/widgets/list/lv_list.c \
Component/lvgl/src/extra/widgets/menu/lv_menu.c \
Component/lvgl/src/extra/widgets/meter/lv_meter.c \
Component/lvgl/src/extra/widgets/msgbox/lv_msgbox.c \
Component/lvgl/src/extra/widgets/span/lv_span.c \
Component/lvgl/src/extra/widgets/spinbox/lv_spinbox.c \
Component/lvgl/src/extra/widgets/spinner/lv_spinner.c \
Component/lvgl/src/extra/widgets/tabview/lv_tabview.c \
Component/lvgl/src/extra/widgets/tileview/lv_tileview.c \
Component/lvgl/src/extra/widgets/win/lv_win.c \
Component/lvgl/src/extra/lv_extra.c \
Component/lvgl/src/font/lv_font_dejavu_16_persian_hebrew.c \
Component/lvgl/src/font/lv_font_fmt_txt.c \
Component/lvgl/src/font/lv_font_loader.c \
Component/lvgl/src/font/lv_font_montserrat_8.c \
Component/lvgl/src/font/lv_font_montserrat_10.c \
Component/lvgl/src/font/lv_font_montserrat_12.c \
Component/lvgl/src/font/lv_font_montserrat_12_subpx.c \
Component/lvgl/src/font/lv_font_montserrat_14.c \
Component/lvgl/src/font/lv_font_montserrat_16.c \
Component/lvgl/src/font/lv_font_montserrat_18.c \
Component/lvgl/src/font/lv_font_montserrat_20.c \
Component/lvgl/src/font/lv_font_montserrat_22.c \
Component/lvgl/src/font/lv_font_montserrat_24.c \
Component/lvgl/src/font/lv_font_montserrat_26.c \
Component/lvgl/src/font/lv_font_montserrat_28_compressed.c \
Component/lvgl/src/font/lv_font_montserrat_28.c \
Component/lvgl/src/font/lv_font_montserrat_30.c \
Component/lvgl/src/font/lv_font_montserrat_32.c \
Component/lvgl/src/font/lv_font_montserrat_34.c \
Component/lvgl/src/font/lv_font_montserrat_36.c \
Component/lvgl/src/font/lv_font_montserrat_38.c \
Component/lvgl/src/font/lv_font_montserrat_40.c \
Component/lvgl/src/font/lv_font_montserrat_42.c \
Component/lvgl/src/font/lv_font_montserrat_44.c \
Component/lvgl/src/font/lv_font_montserrat_46.c \
Component/lvgl/src/font/lv_font_montserrat_48.c \
Component/lvgl/src/font/lv_font_simsun_16_cjk.c \
Component/lvgl/src/font/lv_font_unscii_8.c \
Component/lvgl/src/font/lv_font_unscii_16.c \
Component/lvgl/src/font/lv_font.c \
Component/lvgl/src/hal/lv_hal_disp.c \
Component/lvgl/src/hal/lv_hal_indev.c \
Component/lvgl/src/hal/lv_hal_tick.c \
Component/lvgl/src/misc/lv_anim.c \
Component/lvgl/src/misc/lv_area.c \
Component/lvgl/src/misc/lv_async.c \
Component/lvgl/src/misc/lv_bidi.c \
Component/lvgl/src/misc/lv_color.c \
Component/lvgl/src/misc/lv_fs.c \
Component/lvgl/src/misc/lv_gc.c \
Component/lvgl/src/misc/lv_ll.c \
Component/lvgl/src/misc/lv_log.c \
Component/lvgl/src/misc/lv_math.c \
Component/lvgl/src/misc/lv_mem.c \
Component/lvgl/src/misc/lv_printf.c \
Component/lvgl/src/misc/lv_style_gen.c \
Component/lvgl/src/misc/lv_style.c \
Component/lvgl/src/misc/lv_templ.c \
Component/lvgl/src/misc/lv_timer.c \
Component/lvgl/src/misc/lv_tlsf.c \
Component/lvgl/src/misc/lv_txt_ap.c \
Component/lvgl/src/misc/lv_txt.c \
Component/lvgl/src/misc/lv_utils.c \
Component/lvgl/src/widgets/lv_arc.c \
Component/lvgl/src/widgets/lv_bar.c \
Component/lvgl/src/widgets/lv_btn.c \
Component/lvgl/src/widgets/lv_btnmatrix.c \
Component/lvgl/src/widgets/lv_canvas.c \
Component/lvgl/src/widgets/lv_checkbox.c \
Component/lvgl/src/widgets/lv_dropdown.c \
Component/lvgl/src/widgets/lv_img.c \
Component/lvgl/src/widgets/lv_label.c \
Component/lvgl/src/widgets/lv_line.c \
Component/lvgl/src/widgets/lv_objx_templ.c \
Component/lvgl/src/widgets/lv_roller.c \
Component/lvgl/src/widgets/lv_slider.c \
Component/lvgl/src/widgets/lv_switch.c \
Component/lvgl/src/widgets/lv_table.c \
Component/lvgl/src/widgets/lv_textarea.c \
Component/lvgl/examples/porting/lv_port_disp.c \
Component/lvgl/examples/porting/lv_port_indev.c \
Component/lvgl/demos/widgets/lv_demo_widgets.c \
Component/lvgl/demos/widgets/assets/img_clothes.c \
Component/lvgl/demos/widgets/assets/img_lvgl_logo.c \
Component/lvgl/demos/widgets/assets/img_demo_widgets_avatar.c \
Component/lvgl/demos/stress/lv_demo_stress.c \
Component/lvgl/demos/stress/assets/lv_font_montserrat_12_compr_az.c \
Component/lvgl/demos/stress/assets/lv_font_montserrat_16_compr_az.c \
Component/lvgl/demos/stress/assets/lv_font_montserrat_28_compr_az.c \


ifeq ($(BUILE_LWIP),y)
C_INCLUDES +=  \
-IComponent/LWIP/include \
-IComponent/LWIP/include/lwip \
-IComponent/LWIP/include/netif \
-IComponent/LWIP/include/compat \
-IComponent/LWIP/include/lwi/apps \
-IComponent/LWIP/include/lwip/priv \
-IComponent/LWIP/include/lwip/prot \

endif

#fatfs
C_INCLUDES +=  \
-IComponent/FatFs \

#lvgl
C_INCLUDES +=  \
-IComponent/lvgl \
-IComponent/lvgl/src/core \
-IComponent/lvgl/src/draw \
-IComponent/lvgl/src/draw/sdl \
-IComponent/lvgl/src/draw/stm32_dma2d \
-IComponent/lvgl/src/draw/sw \
-IComponent/lvgl/src/extra/layouts/flex \
-IComponent/lvgl/src/extra/layouts/grid \
-IComponent/lvgl/src/extra/layouts \
-IComponent/lvgl/src/extra/libs \
-IComponent/lvgl/src/extra/libs/bmp \
-IComponent/lvgl/src/extra/libs/ffmpeg \
-IComponent/lvgl/src/extra/libs/freetype \
-IComponent/lvgl/src/extra/libs/fsdrv \
-IComponent/lvgl/src/extra/libs/gif \
-IComponent/lvgl/src/extra/libs/png \
-IComponent/lvgl/src/extra/libs/qrcode \
-IComponent/lvgl/src/extra/libs/rlottie \
-IComponent/lvgl/src/extra/libs/sjpg \
-IComponent/lvgl/src/extra/others/gridnav \
-IComponent/lvgl/src/extra/others/monkey \
-IComponent/lvgl/src/extra/others/snapshot \
-IComponent/lvgl/src/extra/others \
-IComponent/lvgl/src/extra/themes/basic \
-IComponent/lvgl/src/extra/themes/default \
-IComponent/lvgl/src/extra/themes/mono \
-IComponent/lvgl/src/extra/themes \
-IComponent/lvgl/src/extra/widgets/animimg \
-IComponent/lvgl/src/extra/widgets/calendar \
-IComponent/lvgl/src/extra/widgets/chart \
-IComponent/lvgl/src/extra/widgets/colorwheel \
-IComponent/lvgl/src/extra/widgets/imgbtn \
-IComponent/lvgl/src/extra/widgets/keyboard \
-IComponent/lvgl/src/extra/widgets/led \
-IComponent/lvgl/src/extra/widgets/list \
-IComponent/lvgl/src/extra/widgets/meter \
-IComponent/lvgl/src/extra/widgets/msgbox \
-IComponent/lvgl/src/extra/widgets/span \
-IComponent/lvgl/src/extra/widgets/spinbox \
-IComponent/lvgl/src/extra/widgets/spinner \
-IComponent/lvgl/src/extra/widgets/tabview \
-IComponent/lvgl/src/extra/widgets/tileview \
-IComponent/lvgl/src/extra/widgets/win \
-IComponent/lvgl/src/extra/widgets \
-IComponent/lvgl/src/extra \
-IComponent/lvgl/src/font \
-IComponent/lvgl/src/gpu \
-IComponent/lvgl/src/hal \
-IComponent/lvgl/src/misc \
-IComponent/lvgl/src/widgets \
-IComponent/lvgl/src \
-IComponent/lvgl/demos/stress \
-IComponent/lvgl/demos/stress/assets \
-IComponent/lvgl/demos \
-IComponent/lvgl/demos/widgets \
-IComponent/lvgl/demos/widgets/assets \



CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections


# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif	


#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
