# ROSIE
The **Recreational Output System for Interactive Exercise**, or **ROSIE**, is a tennis ball launcher.
This program is used to control the onboard ESP32 which controls the motors, ball spin, and handles
the connection to the app (see [ROSIE-Control-Panel](https://github.com/KennyC3200/ROSIE-Control-Panel)).

## Install
You will first require:
- [PlatformIO](https://platformio.org/) installed in VS Code
- ESP32 connected via USB to upload the program

The recommended way to install is from source:
```
git clone --recursive https://github.com/KennyC3200/ROSIE 
```
Then open the project in VS Code with PlatformIO, build the project, and then upload it to the ESP32
via USB.

## Hardware
### Pinout
The default pinout for the ESP32 (I used [this](https://www.amazon.ca/Freenove-Dual-core-Microcontroller-Development-Projects/dp/B0C9TGJRPH/ref=sr_1_7?crid=3FWIBI02XZUN2&dib=eyJ2IjoiMSJ9.beVRzgfFpa7S78WssaJYRdb5evEHRoevod-RLS0wM9-GctHnllApi-ldGJP_y5df6RK31S7D-Y56-V0E4qBnmKPwDRRb1bfoa93QHTk8Uy6cl6tLhtlEyUWDrnnxgUoz7id42R7jqk3-wptE5IJYRvk6E_kgU25gv92sJgZ7LVMBSRVncg2LMi07hZV9iKwV9WK3T69pEoTAVLJ2LvcQgs5cxDAgsb_9ErlI3Sqa8ALdZTEZKn3XofVAQlDr0OM5351VYd0VndN4Z_tYftHHXO2CIZtyAaYzJghXrvTMgz0.2LY868X0Bj4sKzE9R_I7PkxzItr2oEFMgNlbLsv2Zn8&dib_tag=se&keywords=freenove%2Besp32&qid=1777839130&sprefix=freenove%2Besp%2Caps%2C120&sr=8-7&th=1) ESP32 model, but any microcontroller can work):
| Pin | GPIO | Description |
|-----|------|-------------|
| PWM | 4 | Motor 1 PWM |
| L_EN | 16 | Motor 1 left enable |
| R_EN | 17 | Motor 1 right enable |
| PWM | 14 | Motor 2 PWM |
| L_EN | 26 | Motor 2 left enable |
| R_EN | 27 | Motor 2 right enable |
| Switch | 33 | On/off switch (INPUT_PULLUP) |

The pinout can be changed in the code.

### BLE Characteristics
| Name | UUID | Properties | Description |
|------|------|------------|-------------|
| motor_speed_percent | 7e77c276-... | READ, WRITE_NR, NOTIFY | Motor speed (0%-100%) |
| motor_1_RPM | 76bd9c05-... | READ, NOTIFY | Motor 1 RPM |
| motor_2_RPM | d7bc8ea0-... | READ, NOTIFY | Motor 2 RPM |
| ball_spin | 775c47cf-... | READ, WRITE_NR, NOTIFY | Ball spin enabled ("1") or disabled ("0") |

All the values can be found in the code as well.