# Pinouts

## Blue Pill

* USB at top
* Left side if 5V tolerant.
* Right side is not, except for PB10/11.

Left side:

```
PB12      (SPI2 SS?)
PB13  --> SPI2 CLK
PB14  <-- SPI2 MISO
PB15  --> SPI2 MOSI
PA8   <-- Left encoder A
PA9   <-- Left encoder B
PA10  --> Left Motor En A
PA11  --> Left Motor En B
PA12  --> Right Motor En A
PA15  --> Right Motor En B
PB3   --> Segger SWO
PB4
PB5
PB6   <-- Right encoder A
PB7   <-- Right encoder B
PB8   <-- Reserved: I2C1 SCL
PB9   <-> Reserved: I2C1 SDA
5V    <-- Power Supply
GND   <-- Power Supply
3V3
```

Right side:

```
GND
GND
3V3
NRST   <-- Segger RESET
PB11   <-> I2C2 SDA
PB10   --> I2C2 SCL
PB1
PB0
PA7    --> Reserved: SPI1 MOSI
PA6    <-- Reserved: SPI1 MISO
PA5    --> Reserved: SPI1 CLK
PA4    --> Reserved: SPI1 NSS
PA3    <-- Reserved: UART2 RX
PA2    --> Reserved: UART2 TX
PA1    --> Motor PWM2
PA0    --> Motor PWM1
PC15   x-x
PC14   x-x
PC13   --> LED
VBAT   x-x
```

Bottom:

```
3V3         --> Segger VREF
SWDIO(PA13) <-> Segger SWDIO
SWCLK(PA14) <-- Segger SWCLK
GND         --- Segger GND
```

## Segger 20-pin Connector

* Top left is 1.
* Tab faces left.

```
VREF   x
x      GND
x      GND
SWDIO  x
SWCLK  x
x      GND
SWO    x
RESET  x
x      GND
5V     x
```