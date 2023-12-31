#pragma once

// 命令 (Page 34)
#define ADS1256_CMD_WAKEUP   0x00 // Completes SYNC and Exits Standby Mode
#define ADS1256_CMD_RDATA    0x01 // Read Data
#define ADS1256_CMD_RDATAC   0x03 // Read Data Continuously
#define ADS1256_CMD_SDATAC   0x0F // Stop Read Data Continuously
#define ADS1256_CMD_RREG     0x10 // Read from REG (The real command is 0b0001rrrr, where rrrr repersents the REG)
#define ADS1256_CMD_WREG     0x50 // Write to REG  (The real command is 0b0101rrrr, where rrrr repersents the REG)
#define ADS1256_CMD_SELFCAL  0xF0 // Offset and Gain Self-Calibration
#define ADS1256_CMD_SELFOCAL 0xF1 // Offset Self-Calibration
#define ADS1256_CMD_SELFGCAL 0xF2 // Gain Self-Calibration
#define ADS1256_CMD_SYSOCAL  0xF3 // System Offset Calibration
#define ADS1256_CMD_SYSGCAL  0xF4 // System Gain Calibration
#define ADS1256_CMD_SYNC     0xFC // Synchronize the A/D Conversion
#define ADS1256_CMD_STANDBY  0xFD // Begin Standby Mode
#define ADS1256_CMD_RESET    0xFE // Reset to Power-Up Values

// 寄存器地址 (Page 30)
#define ADS1256_STATUS 0x00
#define ADS1256_MUX    0x01
#define ADS1256_ADCON  0x02
#define ADS1256_DRATE  0x03
#define ADS1256_IO     0x04
#define ADS1256_OFC0   0x05
#define ADS1256_OFC1   0x06
#define ADS1256_OFC2   0x07
#define ADS1256_FSC0   0x08
#define ADS1256_FSC1   0x09
#define ADS1256_FSC2   0x0A
