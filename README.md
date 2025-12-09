# STM32WL55JC1 End Node

This repository contains code implementations for a LoRaWAN end node using the STM32WL55JC1 Nucleo Board, starting from the example "LoRaWAN_End_Node" provided in STM32CubeIDE.

## Repository Structure

### **"setup_files" branch**
This folder includes the necessary files to implement the base example "LoRaWAN_End_Node" available in STM32CubeIDE. It serves as the starting point for further modifications.

### **"main" branch** - I2C Sensor Integration (BMP280)
- This version modifies the base example to integrate an I2C sensor (BMP280).
- The end node reads temperature and pressure data from the sensor and transmits it using a custom-written library.
- The required libraries can be found at `""` (ensure to add the correct path to the libraries).
- **Low power mode is enabled** and properly handled.

### **"end_node_with_comparator_and_ADC" branch** - Voltage peak detector with comparator and ADC
- This version is an end node project, programmed to do peak detection
- It uses a comparator, to act as a trigger for ADC acquisition
- It acquires a voltage peak, and saves data on buffers, to be transmitted through LoRaWAN

## Getting Started
1. Clone this repository:
   ```bash
   git clone https://github.com/your-repo-name.git
   ```
2. Open the project in **STM32CubeIDE**.
3. Select the desired version (`v01` or `v02`) and flash it to the **STM32WL55JC1 Nucleo Board**.

## Contributions
Feel free to contribute by improving the existing code or adding new features. Open an issue or submit a pull request if you have any suggestions.

## License
This project is licensed under the MIT License. See the LICENSE file for details.


---

### Setting Up the STM32WL55JC1 as a LoRaWAN End-Node

1. Open STM32CubeIDE:
   - Navigate to File > New > STM32 Project from an Existing STM32CubeMX Configuration File (.ioc).

2. Import the Project:
   - Select the `.ioc` file from your folder and import it into your workspace.
   - Enter your preferred project title when prompted.

3. Handling LoRaWAN Platform Warning:
   - If a warning about the LoRaWAN Platform appears, select "No" to continue without changing the platform setting.

4. Configure LoRaWAN Middleware:
   - In the Project Explorer, go to Middleware and Software Packs > LoRaWAN.
   - In the LoRaWAN Middleware settings:
     - Uncheck "Region USA freq: 915" to disable the 915 MHz frequency band.
     - Set the Default Activation Type to "ABP" (Activation By Personalization).

5. Set LoRaWAN Commissioning Parameters:
   - Under LoRaWAN Commissioning, configure the following parameters:
     - **Device Address**
     - **Network Session Key**
     - **Application Session Key**

6. Save and Generate Code:
   - Save the `.ioc` file and generate the code.
   - Confirm with "Yes" to any warnings generated during code generation.

7. Enable Debugging:
   - In the Project Explorer, go to Core > Inc > sys_conf.h.
   - Set `DEBUGGER_ENABLED` to `1`.

8. Add BSP Folder:
   - Copy the provided BSP folder into the following directory:
     - `..\#your_workspace#\#your_project_name#\Drivers`

9. Include the BSP Folder in the Project:
   - Right-click on your project in STM32CubeIDE and select Properties.
   - Navigate to C/C++ General > Paths and Symbols.
   - On the right side, click Add and type `Drivers/BSP/STM32WLxx_Nucleo`.
   - Click OK, then Apply and Close.
   - If prompted to Rebuild Index, select that option.

10. Replace the LoRaWAN Application File:
    - In the Project Explorer, navigate to `..\#your_workspace#\#your_project_name#\LoRaWAN\App`.
    - Replace `lora_app.c` with the provided version.

11. Save and Debug:
    - Click Save All, then Debug to compile and run your project.

---

Your STM32WL55JC1 Nucleo Board is now configured and ready to operate as a LoRaWAN End-Node.
