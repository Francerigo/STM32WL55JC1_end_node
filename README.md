# **v03** - STM32 end node operation for bees charge monitoring

- This version includes analog comparator and ADC
- The comparator triggers an interrupt when the sensed charge surpasses a certain threshold
- It enables the ADC conversion
- An algorithm calculates the peak maximum voltage, which is stored in a buffer of 7 positions (each with a data structure composed of a 32 bits timestamp and a 16 bits voltage peak value)
- When the buffer is full, the system sends via LoRaWAN a buf


- Keep attention! If you are adding ADC channels, the .ioc file may not generate anymore the adc_if.h and adc_if.c files. To solve this issue, be sure that ADC is selected in the .ioc files under Middleware and Software Packs -> Platform settings
- 

