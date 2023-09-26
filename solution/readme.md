# Documentation

## Video

[![Watch the video](https://img.youtube.com/vi/LV-JbSkoToc/hqdefault.jpg)](https://www.youtube.com/embed/LV-JbSkoToc)

## [Code](./src/main/main.ino)

### Development environment

I used Arduino IDE 2.0 to program the board.
This [project](https://github.com/stm32duino/Arduino_Core_STM32) adds the support of STM32 MCU in Arduino IDE.

Useful links which helped me to setup the development environment:

- <https://github.com/stm32duino/Arduino_Core_STM32>
- <https://github.com/stm32duino/Arduino_Core_STM32/wiki/Getting-Started>
- <https://wiki.archlinux.org/title/AVR>

### Technical Documentation

- [STM32G474RE microprocessor docs](https://www.st.com/en/microcontrollers-microprocessors/stm32g474re.html#documentation)
- [NUCLEO-G474RE dev board docs](https://www.st.com/en/evaluation-tools/nucleo-g474re.html#documentation)
- [Board connectors pinout](https://www.st.com/content/ccc/resource/technical/layouts_and_diagrams/schematic_pack/97/ba/59/c2/f3/83/4d/fe/x-nucleo-plc01a1_schematic.pdf/files/x-nucleo-plc01a1_schematic.pdf/jcr:content/translations/en.x-nucleo-plc01a1_schematic.pdf)

#### Timers

- [HarwareTimer Library docs](https://github.com/stm32duino/Arduino_Core_STM32/wiki/HardwareTimer-library)

*STM32G474RE Clock = 170Mhz*

#### LDR

Light Dependent Resistor

- [Documentation](./ldr/readme.md)
