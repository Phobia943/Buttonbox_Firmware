# Buttonbox Config Tool

A Python-based GUI tool to configure and update the firmware of your Buttonbox.

## Features
*   **Connect via Serial:** Automatically detects and connects to the Buttonbox.
*   **View Configuration:** Reads the current key mapping from the device (JSON format).
*   **Update Firmware:** Easily flash a new `.uf2` firmware file.
    *   Automatically sends the "Bootloader" command to the Pico.
    *   Copies the firmware file to the device.

## Installation

1.  Ensure you have Python 3 installed.
2.  Install dependencies:
    ```bash
    pip install -r requirements.txt
    ```

## Usage

Run the application:
```bash
python main.py
```

### Firmware Update Steps
1.  Connect your Buttonbox.
2.  Select the COM port and click "Connect".
3.  Go to the "Firmware Update" tab.
4.  Click **"1. Reboot to Bootloader"**. The device will disconnect and reappear as a storage drive (RPI-RP2).
5.  Click **"2. Select .uf2 File"** and choose your firmware file.
6.  Click **"3. Flash Firmware"**.

## Configuration
(Work in Progress)
Currently, you can view the raw JSON configuration received from the device in the "Configuration" tab.
