# BMP280 end node with STM32WL55JC1 Nucleo boar

## Working operation
1. Under Core > Src > stm32_lpm_if you can find all the functions regulating the entering and exiting low powr modes (standy, off, stop).
2. Whenever the device is set to operate in low power mode (by setting 'LOW_POWER_DISABLE' flag to 0 in Core > Inc > sys_conf.h ) it enters and exits stop2 mode depending on the sequencer operation.
3. Entering stop2 mode disables the I2C peripheral. This code reinitializes it in PWR_ExitStopMode function, by just doing 'MX_I2C1_Init();'


