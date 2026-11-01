<div align="center">

<img width="250" alt="SONDER" src="https://github.com/user-attachments/assets/1672acc3-2ada-43d8-b0ae-82f933966cd7" />

<h1>
  <b>
    SONDER
  </b> 
</h1>

<h3> <b> An ESP32 SD Card MP3 Player with a PCM5102 DAC, a 0.96" OLED and a Rotary Encoder! </b> </h3>

</div>

## **Overview**

An MP3 player built around an **ESP32** dev board and a **PCM5102** I2S DAC. It plays music from an SD card, shows the current track on a 0.96" OLED, and has 4 buttons (previous, play/pause, next, menu) plus a rotary encoder with a push switch. It lives in a 3D printed case with openings for the aux jack and USB-B port.

### Why?

It started as "wouldn't it be funny if I built my own music player" and then I spent my time fighting KiCad. Further I really wanted to make a project that deals with embedded audio decoding in C/ARDUINO IDE.

### How?
Use the firmware in [FIRMWARE](Firmware). I used ARDUINO IDE with some basic libraries like SPI, SD, vector, Audio & U8g2lib

## CAD
CAD files are available at [CAD](https://cad.onshape.com/documents/66bd644b255fcec0873fcebe/w/d2b2d33a9ff288106fcfa12e/e/8a27dd1411d3c83e2866f539?renderMode=0&uiState=6ae774908fb952af58be5994)

The case is a two part design: a main body that holds the PCB, and a separate front plate with cutouts for the OLED, the buttons and the encoder. The plate outline was drawn in KiCad on the Edge.Cuts layer and exported as a DXF. The PCB mounts to the case with heat set inserts and M2 screws, and the whole thing is 3D printed.

## Images
<div align="center">
  
<img width="608" height="410" alt="image-removebg-preview(1)" src="https://github.com/user-attachments/assets/1672acc3-2ada-43d8-b0ae-82f933966cd7" />

<br> Full render<br>

---

<img width="735" height="496" alt="image" src="https://github.com/user-attachments/assets/23f94105-5a83-45db-bf7d-e1eea2775f41" />

<br> Case body

---

<img width="735" height="496" alt="image" src="https://github.com/user-attachments/assets/fc3e939c-0061-4be1-99c5-eaa835358f35" />

<br> Front plate

---

<img width="735" height="542" alt="image" src="https://github.com/user-attachments/assets/39731f83-5aa0-4a2e-9a7f-f6a4cf15d728" />

<br> PCB

---

<img width="735" height="474" alt="image" src="https://github.com/user-attachments/assets/bd641787-4072-44be-b436-ceff850a789a" />

<br> Sch

---

<img width="735" height="596" alt="image" src="https://github.com/user-attachments/assets/67a8db25-71a9-407d-ba67-87b123a51546" />
<br> 3D Viewer

---

</div>


## BOM

| Part | What it's for | Qty | Unit | Total | Vendor |
| --- | --- | --- | --- | --- | --- |
| [PCB](https://jlcpcb.com/) | Printed circuit board | 1 | $1.50 | $1.50 | [JLCPCB](https://jlcpcb.com/) |
| [PCM5102 I2S Digital Audio DAC Decoder Module](https://hubtronics.in/pcm5102-i2s-digital-audio-dac-decoder-module) | I2S audio output | 1 | $5.00 | $5.00 | [Hubtronics](https://hubtronics.in/pcm5102-i2s-digital-audio-dac-decoder-module) |
| [Robocraze ESP32 38 Pin](https://www.amazon.in/dp/B078MNG9D5) | MCU | 1 | $6.50 | $6.50 | [Amazon](https://www.amazon.in/dp/B078MNG9D5) |
| [SD Card Reader Module](https://robu.in/product/micro-sd-card-module/) | Reads the SD card over SPI | 1 | $1.00 | $1.00 | [Robu.in](https://robu.in/product/micro-sd-card-module/) |
| [0.96 Inch I2C/IIC 4pin OLED Display](https://robu.in/product/4pin-oled-display-module-blue-color/) | Shows the current track and volume | 1 | $2.00 | $2.00 | [Robu.in](https://robu.in/product/4pin-oled-display-module-blue-color/) |
| [Rotary Encoder with Switch](https://robu.in/product/hongyan-ec11h-7ce20p1zd20-rotary-encoder-with-push-button-switch-vertical-plug-in-5-pin/) | Volume and push | 1 | $1.00 | $1.00 | [Robu.in](https://robu.in/product/hongyan-ec11h-7ce20p1zd20-rotary-encoder-with-push-button-switch-vertical-plug-in-5-pin/) |
| [Tactile push buttons 6x6x8 [Pack of 20]](https://robu.in/product/6x6x8mm-tactile-push-button-switch-pack-of-20/) | Previous, play/pause, next, menu | 1 | $0.50 | $0.50 | [Robu.in](https://robu.in/product/6x6x8mm-tactile-push-button-switch-pack-of-20/) |
| [HIKVISION Extreme 16GB microSDXC Memory Card](https://www.amazon.in/HIKVISION-microSDXC-Compitable-Smartphones-Protection/dp/B0GVJZ56K5) | Stores the music | 1 | $6.50 | $6.50 | [Amazon](https://www.amazon.in/HIKVISION-microSDXC-Compitable-Smartphones-Protection/dp/B0GVJZ56K5) |
| [Case 3D printing](https://hackclub.slack.com/archives/C083P4FJM46/p1793539514844309) | Case | 1 | $3.00 | $3.00 | [Print Legion](https://hackclub.slack.com/archives/C083P4FJM46/p1793539514844309) |
| **Parts subtotal** | — | — | — | **$27.00** | — |
| **Tax & shipping** | — | — | — | **$3.00** | — |
| **Total** | — | — | — | **$30.00** | — |
