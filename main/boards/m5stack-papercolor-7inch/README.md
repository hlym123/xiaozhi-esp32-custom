# M5Stack PaperColor 7Inch

Xiaozhi board support for the ESP32-S3-WROOM-1-N16R8 based PaperColor 7Inch.
The e-paper display is intentionally not initialized by this board definition.

## Supported hardware

- ES8311 audio codec on I2C0 at `0x18`
- AW8737A speaker amplifier enable on GPIO45
- M5PM1 codec power enable on PMIC GPIO4
- KEY1 on GPIO3 for Wi-Fi setup and chat toggle
- KEY2 on GPIO9 for push-to-talk

## Audio pins

| Signal | GPIO |
| --- | ---: |
| I2C SDA | 1 |
| I2C SCL | 2 |
| MCLK | 42 |
| BCLK | 41 |
| LRCK | 39 |
| MCU data out | 38 |
| MCU data in | 40 |
| PA shutdown | 45 |
