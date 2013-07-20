// This file is derived from ucore.

#include <stddef.h>
#include <stdio.h>
#include <pcore/trap.h>
#include <pcore/picirq.h>
#include <dev/ide.h>
#include <asm/io.h>
#include <assert.h>
#include <string.h>

#define ISA_DATA                0x00
#define ISA_ERROR               0x01
#define ISA_PRECOMP             0x01
#define ISA_CTRL                0x02
#define ISA_SECCNT              0x02
#define ISA_SECTOR              0x03
#define ISA_CYL_LO              0x04
#define ISA_CYL_HI              0x05
#define ISA_SDH                 0x06
#define ISA_COMMAND             0x07
#define ISA_STATUS              0x07

#define IDE_BSY                 0x80
#define IDE_DRDY                0x40
#define IDE_DF                  0x20
#define IDE_DRQ                 0x08
#define IDE_ERR                 0x01

#define IDE_CMD_READ            0x20
#define IDE_CMD_WRITE           0x30
#define IDE_CMD_IDENTIFY        0xEC

#define IDE_IDENT_SECTORS       20
#define IDE_IDENT_MODEL         54
#define IDE_IDENT_CAPABILITIES  98
#define IDE_IDENT_CMDSETS       164
#define IDE_IDENT_MAX_LBA       120
#define IDE_IDENT_MAX_LBA_EXT   200

#define IO_BASE0                0x1F0
#define IO_BASE1                0x170
#define IO_CTRL0                0x3F4
#define IO_CTRL1                0x374

#define MAX_IDE                 4
#define MAX_NSECS               128
#define MAX_DISK_NSECS          0x10000000U
#define VALID_IDE(ideno)        (((ideno) >= 0) && ((ideno) < MAX_IDE) && (ide_devices[ideno].valid))

static const struct {
  uint16_t base;        // I/O Base
  uint16_t ctrl;        // Control Base
} channels[2] = {
  {IO_BASE0, IO_CTRL0},
  {IO_BASE1, IO_CTRL1},
};

#define IO_BASE(ideno)          (channels[(ideno) >> 1].base)
#define IO_CTRL(ideno)          (channels[(ideno) >> 1].ctrl)

static struct ide_device {
  unsigned char valid;        // 0 or 1 (If Device Really Exists)
  uint32_t sets;          // Commend Sets Supported
  uint32_t size;          // Size in Sectors
  unsigned char model[41];    // Model in String
} ide_devices[MAX_IDE];

static int ide_wait_ready(uint16_t iobase, bool check_error) {
  int r;
  while ((r = inb(iobase + ISA_STATUS)) & IDE_BSY)
    /* nothing */;
  if (check_error && (r & (IDE_DF | IDE_ERR)) != 0) {
    return -1;
  }
  return 0;
}

int ide_find(const char* model) {
  int i;
  for (i=0; i<karraysize(ide_devices); ++i) {
    if (ide_devices[i].valid) {
      if (strcmp(model, (const char*)ide_devices[i].model) == 0)
        return i;
    }
  }
  return -1;
}

void ide_init(void) {
  kstatic_assert((SECTSIZE % 4) == 0);
  uint16_t ideno, iobase;
  for (ideno = 0; ideno < MAX_IDE; ideno ++) {
    /* assume that no device here */
    ide_devices[ideno].valid = 0;

    iobase = IO_BASE(ideno);

    /* wait device ready */
    ide_wait_ready(iobase, 0);

    /* step1: select drive */
    outb(iobase + ISA_SDH, 0xE0 | ((ideno & 1) << 4));
    ide_wait_ready(iobase, 0);

    /* step2: send ATA identify command */
    outb(iobase + ISA_COMMAND, IDE_CMD_IDENTIFY);
    ide_wait_ready(iobase, 0);

    /* step3: polling */
    if (inb(iobase + ISA_STATUS) == 0 || ide_wait_ready(iobase, 1) != 0) {
      continue ;
    }

    /* device is ok */
    ide_devices[ideno].valid = 1;

    /* read identification space of the device */
    uint32_t buffer[128];
    insl(iobase + ISA_DATA, buffer, sizeof(buffer) / sizeof(uint32_t));

    unsigned char *ident = (unsigned char *)buffer;
    uint32_t sectors;
    uint32_t cmdsets = *(uint32_t *)(ident + IDE_IDENT_CMDSETS);
    /* device use 48-bits or 28-bits addressing */
    if (cmdsets & (1 << 26)) {
      sectors = *(uint32_t *)(ident + IDE_IDENT_MAX_LBA_EXT);
    }
    else {
      sectors = *(uint32_t *)(ident + IDE_IDENT_MAX_LBA);
    }
    ide_devices[ideno].sets = cmdsets;
    ide_devices[ideno].size = sectors;

    /* check if supports LBA */
    assert((*(uint16_t *)(ident + IDE_IDENT_CAPABILITIES) & 0x200) != 0);

    unsigned char *model = ide_devices[ideno].model, 
      *data = ident + IDE_IDENT_MODEL;
    uint32_t i, length = 40;
    for (i = 0; i < length; i += 2) {
      model[i] = data[i + 1], model[i + 1] = data[i];
    }
    do {
      model[i] = '\0';
    } while (i-- > 0 && model[i] == ' ');

    printf("[ide] ide %d: %u(sectors), '%s'.\n", ideno, 
           ide_devices[ideno].size, ide_devices[ideno].model);
  }

  // enable ide interrupt
  pic_enable(IRQ_IDE1);
  pic_enable(IRQ_IDE2);
}

bool ide_device_valid(uint16_t ideno) {
  return VALID_IDE(ideno);
}

size_t ide_device_size(uint16_t ideno) {
  if (ide_device_valid(ideno)) {
    return ide_devices[ideno].size;
  }
  return 0;
}

int ide_read_secs(uint16_t ideno, uint32_t secno, void *dst, size_t nsecs) {
  assert(nsecs <= MAX_NSECS && VALID_IDE(ideno));
  assert(secno < MAX_DISK_NSECS && secno + nsecs <= MAX_DISK_NSECS);
  uint16_t iobase = IO_BASE(ideno), ioctrl = IO_CTRL(ideno);

  ide_wait_ready(iobase, 0);

  // generate interrupt
  outb(ioctrl + ISA_CTRL, 0);
  outb(iobase + ISA_SECCNT, nsecs);
  outb(iobase + ISA_SECTOR, secno & 0xFF);
  outb(iobase + ISA_CYL_LO, (secno >> 8) & 0xFF);
  outb(iobase + ISA_CYL_HI, (secno >> 16) & 0xFF);
  outb(iobase + ISA_SDH, 0xE0 | ((ideno & 1) << 4) | ((secno >> 24) & 0xF));
  outb(iobase + ISA_COMMAND, IDE_CMD_READ);

  int ret = 0;
  for (; nsecs > 0; nsecs --, dst += SECTSIZE) {
    if ((ret = ide_wait_ready(iobase, 1)) != 0) {
      goto out;
    }
    insl(iobase, dst, SECTSIZE / sizeof(uint32_t));
  }

out:
  return ret;
}

int
ide_write_secs(uint16_t ideno, uint32_t secno, const void *src, size_t nsecs) {
  assert(nsecs <= MAX_NSECS && VALID_IDE(ideno));
  assert(secno < MAX_DISK_NSECS && secno + nsecs <= MAX_DISK_NSECS);
  uint16_t iobase = IO_BASE(ideno), ioctrl = IO_CTRL(ideno);

  ide_wait_ready(iobase, 0);

  // generate interrupt
  outb(ioctrl + ISA_CTRL, 0);
  outb(iobase + ISA_SECCNT, nsecs);
  outb(iobase + ISA_SECTOR, secno & 0xFF);
  outb(iobase + ISA_CYL_LO, (secno >> 8) & 0xFF);
  outb(iobase + ISA_CYL_HI, (secno >> 16) & 0xFF);
  outb(iobase + ISA_SDH, 0xE0 | ((ideno & 1) << 4) | ((secno >> 24) & 0xF));
  outb(iobase + ISA_COMMAND, IDE_CMD_WRITE);

  int ret = 0;
  for (; nsecs > 0; nsecs --, src += SECTSIZE) {
    if ((ret = ide_wait_ready(iobase, 1)) != 0) {
      goto out;
    }
    outsl(iobase, src, SECTSIZE / sizeof(uint32_t));
  }

out:
  return ret;
}

