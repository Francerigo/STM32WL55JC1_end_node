# **v03** - STM32 end node operation for bees charge monitoring

- This version includes analog comparator and ADC
- The comparator triggers an interrupt when the sensed charge surpasses a certain threshold
- It enables the ADC conversion
- An algorithm calculates the peak maximum voltage, which is stored in a buffer of 7 positions (each with a data structure composed of a 32 bits timestamp and a 16 bits voltage peak value)
- When the buffer is full, the system sends it via LoRaWAN


- Keep attention! If you are adding ADC channels, the .ioc file may not generate anymore the adc_if.h and adc_if.c files. To solve this issue, be sure that ADC is selected in the .ioc files under Middleware and Software Packs -> Platform settings
- 

- in utilities_def.h hai aggiunto il task CFG_SEQ_Task_ADC_Acquisition
- in lora_app.c hai inserito la funzione di callback HAL_COMP_TriggerCallback per il comparatore, che chiami UTIL_SEQ_SetTask ( ...ADC_Acquisition) se chiamata, con priorità zero
- hai registrato il task

- Ho static LmHandlerAppData_t AppData = { 0, 0, AppDataBuffer };
Con static uint8_t AppDataBuffer [LORAWAN_APP_DATA_BUFFER_MAX_SIZE]
Dove LORAWAN_APP_DATA_BUFFER_MAX_SIZE è 242

Io, quando entro nella funzione acquirePeak, devo iniziare e ripeterla in loop dove faccio polling per leggere i dati dell'ADC e devo inserire valori di 16 bit dell'ADC (sarebbero 12, ma ci sto in 16) e i 16 bit del timestamp nel buffer AppData

