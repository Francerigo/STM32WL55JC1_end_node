# **v03** - STM32 end node operation for bees charge monitoring

- This version includes analog comparator and ADC
- The comparator triggers an interrupt when the sensed charge surpasses a certain threshold
- It enables the ADC conversion
- An algorithm calculates the peak maximum voltage, which is stored in a buffer of 8 peaks positions:
    - each peak is represented by 4 bytes (2 for the peak voltage, 2 for the time in ms equal to the delay in time with respect to the previous peak
- When the buffer is full, the system sends it via LoRaWAN


- Keep attention! If you are adding ADC channels, the .ioc file may not generate anymore the adc_if.h and adc_if.c files. To solve this issue, be sure that ADC is selected in the .ioc files under Middleware and Software Packs -> Platform settings
- Keep attention! To make coding easier, the end node skeleton is used. However, it allows just to select transmission on timer or on event. I keep on timer just as an example, but when the .ioc generates the code, the user needs to remember to comment the set task function in static void OnTxTimerEvent. Otherwise, the transmission will be triggered each x ms instead of when the buffer is full

- in utilities_def.h hai aggiunto il task CFG_SEQ_Task_ADC_Acquisition
- in lora_app.c hai inserito la funzione di callback HAL_COMP_TriggerCallback per il comparatore
- la funzione di callback chiama UTIL_SEQ_SetTask ( ...ADC_Acquisition), in modo da avviare l'acquisizione dell'ADC
- hai registrato il task

- Ho static LmHandlerAppData_t AppData = { 0, 0, AppDataBuffer };
Con static uint8_t AppDataBuffer [LORAWAN_APP_DATA_BUFFER_MAX_SIZE]
Dove LORAWAN_APP_DATA_BUFFER_MAX_SIZE è 242

Io, quando entro nella funzione acquirePeak, devo iniziare e ripeterla in loop dove faccio polling per leggere i dati dell'ADC e devo inserire valori di 16 bit dell'ADC (sarebbero 12, ma ci sto in 16) e i 16 bit del timestamp nel buffer AppData

