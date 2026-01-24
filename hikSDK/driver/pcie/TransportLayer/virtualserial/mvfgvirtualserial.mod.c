#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x8a35b432, "sme_me_mask" },
	{ 0x587f22d7, "devmap_managed_key" },
	{ 0xc1514a3b, "free_irq" },
	{ 0x1e2abf24, "get_user_pages_fast" },
	{ 0xa78af5f3, "ioread32" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x18b0f0a7, "uart_get_baud_rate" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x4a453f53, "iowrite32" },
	{ 0x4a77885d, "platform_driver_unregister" },
	{ 0x656e4a6e, "snprintf" },
	{ 0xc5b6f236, "queue_work_on" },
	{ 0xc8c85086, "sg_free_table" },
	{ 0x48d88a2c, "__SCT__preempt_schedule" },
	{ 0xf0a2251d, "tty_flip_buffer_push" },
	{ 0x92540fbf, "finish_wait" },
	{ 0x75646747, "class_destroy" },
	{ 0x85abef3e, "uart_register_driver" },
	{ 0xc21a853a, "kernel_bind" },
	{ 0x69acdf38, "memcpy" },
	{ 0x94961283, "vunmap" },
	{ 0x37a0cba, "kfree" },
	{ 0x44be8471, "pcpu_hot" },
	{ 0x9c2073a, "__put_devmap_managed_page_refs" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0xe2964344, "__wake_up" },
	{ 0x148653, "vsnprintf" },
	{ 0x34db050b, "_raw_spin_lock_irqsave" },
	{ 0xba8fbd64, "_raw_spin_lock" },
	{ 0xcc5005fe, "msleep_interruptible" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xf87c611c, "wake_up_process" },
	{ 0x65487097, "__x86_indirect_thunk_rax" },
	{ 0x122c3a7e, "_printk" },
	{ 0x8427cc7b, "_raw_spin_lock_irq" },
	{ 0x8ddd8aad, "schedule_timeout" },
	{ 0x1000e51, "schedule" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xe46021ca, "_raw_spin_unlock_bh" },
	{ 0x73fdb083, "platform_device_register" },
	{ 0xd1485b29, "__free_pages" },
	{ 0x9f984513, "strrchr" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x7cd8d75e, "page_offset_base" },
	{ 0xb3f985a8, "sg_alloc_table" },
	{ 0x6b732375, "cdev_add" },
	{ 0xbcb36fe4, "hugetlb_optimize_vmemmap_key" },
	{ 0xea3c74e, "tasklet_kill" },
	{ 0xfe5d4bb2, "sys_tz" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x1392369d, "init_net" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0x32a08632, "platform_device_unregister" },
	{ 0x3b69de06, "device_create" },
	{ 0x2364c85a, "tasklet_init" },
	{ 0x6ca9b86a, "class_create" },
	{ 0xf1969a8e, "__usecs_to_jiffies" },
	{ 0x4c03a563, "random_kmalloc_seed" },
	{ 0x84323a64, "__tty_insert_flip_string_flags" },
	{ 0x715a5ed0, "vprintk" },
	{ 0x4b750f53, "_raw_spin_unlock_irq" },
	{ 0x9d2ab8ac, "__tasklet_schedule" },
	{ 0x10129806, "vmap" },
	{ 0x9ec6ca96, "ktime_get_real_ts64" },
	{ 0x2ef1b23, "kthread_stop" },
	{ 0xd35cce70, "_raw_spin_unlock_irqrestore" },
	{ 0xd38cd261, "__default_kernel_pte_mask" },
	{ 0xfb578fc5, "memset" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x356461c8, "rtc_time64_to_tm" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xd9a5ea54, "__init_waitqueue_head" },
	{ 0x53be26d8, "kthread_bind" },
	{ 0xaefd579d, "uart_remove_one_port" },
	{ 0x23509fba, "__platform_driver_register" },
	{ 0x1f337bd7, "kthread_create_on_node" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x97651e6c, "vmemmap_base" },
	{ 0x298510e3, "uart_update_timeout" },
	{ 0x999e8297, "vfree" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x5c35a1ea, "sock_create_kern" },
	{ 0xc402aed6, "uart_unregister_driver" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0xdea9ed44, "alloc_pages" },
	{ 0x9cbc9023, "__folio_put" },
	{ 0x5b40b481, "device_destroy" },
	{ 0xc3690fc, "_raw_spin_lock_bh" },
	{ 0xdc900178, "sock_release" },
	{ 0x1d8f10ec, "dma_unmap_sg_attrs" },
	{ 0xd0c3484c, "kmalloc_trace" },
	{ 0x54b1fac6, "__ubsan_handle_load_invalid_value" },
	{ 0x754d539c, "strlen" },
	{ 0xbf6afdb6, "uart_add_one_port" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0xb5b54b34, "_raw_spin_unlock" },
	{ 0x46b615f3, "kernel_sendmsg" },
	{ 0xf9a482f9, "msleep" },
	{ 0x858c69be, "cdev_init" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0xe2c17b5d, "__SCT__might_resched" },
	{ 0x8da0819, "kmalloc_caches" },
	{ 0xc892ac3e, "cdev_del" },
	{ 0x72bc5090, "dma_map_sg_attrs" },
	{ 0x2d3385d3, "system_wq" },
	{ 0x2f2c95c4, "flush_work" },
	{ 0xe2fd41e5, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "DB76EB404D29B2FCB3F91DE");
