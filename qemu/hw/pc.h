#ifndef HW_PC_H
#define HW_PC_H
/* PC-style peripherals (also used by other machines).  */

/* serial.c */

SerialState *serial_init(int base, qemu_irq irq, int baudbase,
                         CharDriverState *chr);
SerialState *serial_mm_init (target_phys_addr_t base, int it_shift,
                             qemu_irq irq, int baudbase,
                             CharDriverState *chr, int ioregister);
uint32_t serial_mm_readb (void *opaque, target_phys_addr_t addr);
void serial_mm_writeb (void *opaque, target_phys_addr_t addr, uint32_t value);
uint32_t serial_mm_readw (void *opaque, target_phys_addr_t addr);
void serial_mm_writew (void *opaque, target_phys_addr_t addr, uint32_t value);
uint32_t serial_mm_readl (void *opaque, target_phys_addr_t addr);
void serial_mm_writel (void *opaque, target_phys_addr_t addr, uint32_t value);

/* parallel.c */

typedef struct ParallelState ParallelState;
ParallelState *parallel_init(int base, qemu_irq irq, CharDriverState *chr);
ParallelState *parallel_mm_init(target_phys_addr_t base, int it_shift, qemu_irq irq, CharDriverState *chr);

/* i8259.c */

typedef struct PicState2 PicState2;
extern PicState2 *isa_pic;
void pic_set_irq(int irq, int level);
void pic_set_irq_new(void *opaque, int irq, int level);
qemu_irq *i8259_init(qemu_irq parent_irq);
void pic_set_alt_irq_func(PicState2 *s, SetIRQFunc *alt_irq_func,
                          void *alt_irq_opaque);
int pic_read_irq(PicState2 *s);
void pic_update_irq(PicState2 *s);
uint32_t pic_intack_read(PicState2 *s);
void pic_info(void);
void irq_info(void);

/* APIC */
typedef struct IOAPICState IOAPICState;

int apic_init(CPUState *env);
int apic_accept_pic_intr(CPUState *env);
void apic_deliver_pic_intr(CPUState *env, int level);
int apic_get_interrupt(CPUState *env);
IOAPICState *ioapic_init(void);
void ioapic_set_irq(void *opaque, int vector, int level);
void apic_reset_irq_delivered(void);
int apic_get_irq_delivered(void);
void apic_set_irq_delivered(void);

/* i8254.c */

#define PIT_FREQ 1193182

typedef struct PITState PITState;

PITState *pit_init(int base, qemu_irq irq);
void pit_set_gate(PITState *pit, int channel, int val);
int pit_get_gate(PITState *pit, int channel);
int pit_get_initial_count(PITState *pit, int channel);
int pit_get_mode(PITState *pit, int channel);
int pit_get_out(PITState *pit, int channel, int64_t current_time);

/* i8254-kvm.c */

PITState *kvm_pit_init(int base, qemu_irq irq);

void hpet_pit_disable(void);
void hpet_pit_enable(void);

/* vmport.c */
void vmport_init(void);
void vmport_register(unsigned char command, IOPortReadFunc *func, void *opaque);

/* vmmouse.c */
void *vmmouse_init(void *m);

/* pckbd.c */

void i8042_init(qemu_irq kbd_irq, qemu_irq mouse_irq, uint32_t io_base);
void i8042_mm_init(qemu_irq kbd_irq, qemu_irq mouse_irq,
                   target_phys_addr_t base, ram_addr_t size,
                   target_phys_addr_t mask);

/* mc146818rtc.c */

typedef struct RTCState RTCState;

RTCState *rtc_init(int base, qemu_irq irq);
RTCState *rtc_mm_init(target_phys_addr_t base, int it_shift, qemu_irq irq);
void rtc_set_memory(RTCState *s, int addr, int val);
void rtc_set_date(RTCState *s, const struct tm *tm);
void cmos_set_s3_resume(void);

/* pc.c */
extern int fd_bootchk;

void ioport_set_a20(int enable);
int ioport_get_a20(void);
CPUState *pc_new_cpu(int cpu, const char *cpu_model, int pci_enabled);

/* acpi.c */
extern int acpi_enabled;
i2c_bus *piix4_pm_init(PCIBus *bus, int devfn, uint32_t smb_io_base,
                       qemu_irq sci_irq);
void piix4_smbus_register_device(SMBusDevice *dev, uint8_t addr);
void acpi_bios_init(void);
int acpi_table_add(const char *table_desc);

/* hpet.c */
extern int no_hpet;

/* pcspk.c */
void pcspk_init(PITState *);
int pcspk_audio_init(AudioState *, qemu_irq *pic);

/* piix_pci.c */
/* config space register for IRQ routing */
#define PIIX_CONFIG_IRQ_ROUTE 0x60

PCIBus *i440fx_init(PCIDevice **pi440fx_state, qemu_irq *pic);
void i440fx_set_smm(PCIDevice *d, int val);
int piix3_init(PCIBus *bus, int devfn);
void i440fx_init_memory_mappings(PCIDevice *d);

extern PCIDevice *piix4_dev;
int piix4_init(PCIBus *bus, int devfn);

int piix_get_irq(int pin);

int ipf_map_irq(PCIDevice *pci_dev, int irq_num);

/* vga.c */
enum vga_retrace_method {
    VGA_RETRACE_DUMB,
    VGA_RETRACE_PRECISE
};

extern enum vga_retrace_method vga_retrace_method;

#ifndef TARGET_SPARC
#define VGA_RAM_SIZE (16 * 1024 * 1024)
#else
#define VGA_RAM_SIZE (17 * 1024 * 1024)
#endif

int isa_vga_init(DisplayState *ds, uint8_t *vga_ram_base,
                 unsigned long vga_ram_offset, int vga_ram_size);
int pci_vga_init(PCIBus *bus, DisplayState *ds, uint8_t *vga_ram_base,
                 unsigned long vga_ram_offset, int vga_ram_size,
                 unsigned long vga_bios_offset, int vga_bios_size);
int isa_vga_mm_init(DisplayState *ds, uint8_t *vga_ram_base,
                    unsigned long vga_ram_offset, int vga_ram_size,
                    target_phys_addr_t vram_base, target_phys_addr_t ctrl_base,
                    int it_shift);

/* cirrus_vga.c */
void pci_cirrus_vga_init(PCIBus *bus, DisplayState *ds, uint8_t *vga_ram_base,
                         ram_addr_t vga_ram_offset, int vga_ram_size);
void isa_cirrus_vga_init(DisplayState *ds, uint8_t *vga_ram_base,
                         ram_addr_t vga_ram_offset, int vga_ram_size);

/*qxl.c*/
#ifdef CONFIG_QXL

#define QXL_MAX_RAM_SIZE (256 * 1024 * 1024)
#define QXL_MAX_MONITORS 4

extern int using_qxl;
extern int num_qxl_device;
extern uint32_t qxl_ram_size;

typedef struct QXLAddressRange {
	target_phys_addr_t phys_start;
	target_phys_addr_t phys_end;
	unsigned long virt_start;
	unsigned long virt_end;
} QXLAddressRange;

void qxl_init(PCIBus *bus, uint8_t *vram,
              unsigned long vram_offset,
              uint32_t vram_size, uint32_t in_ram_size,
			  QXLAddressRange *address_ranges,
			  uint8_t num_ranges);

void qxl_init_display(DisplayState *ds);
int qxl_vga_touch(void);
void qxl_do_set_log_level(int log_level);
uint32_t qxl_get_total_mem_size(uint32_t in_ram_size);
uint32_t qxl_get_min_ram_size(void);

#endif

/* ide.c */
void isa_ide_init(int iobase, int iobase2, qemu_irq irq,
                  BlockDriverState *hd0, BlockDriverState *hd1);
void pci_cmd646_ide_init(PCIBus *bus, BlockDriverState **hd_table,
                         int secondary_ide_enabled);
void pci_piix3_ide_init(PCIBus *bus, BlockDriverState **hd_table, int devfn,
                        qemu_irq *pic);
void pci_piix4_ide_init(PCIBus *bus, BlockDriverState **hd_table, int devfn,
                        qemu_irq *pic);

/* ne2000.c */

void isa_ne2000_init(int base, qemu_irq irq, NICInfo *nd);

/* extboot.c */

void extboot_init(BlockDriverState *bs, int cmd);

#endif
