# MB9AF314L\_AX88796C\_uIP Firmware #

This example project is a port of the uIP (micro IP) TCP/IP suite including
a small webserver to the FUJITSU MB9AF314L & ASIX AX88796C.

### Hardware Info ###

Target MCU: [Fujitsu **MB9AF314L**][MB9AF314L]

Ethernet controller: [ASIX **AX88796C**][ASIX]

Library version: [**uip1.0**][uIP]

More other info: [**Link**](/doc/hardware.md)

### Interface ###

SPI (MFS:CH7) `4MHz baudrate`   
UART (MFS:CH4) `115K baudrate`   
LED x3 (GPIO)    
Joystick x1 (GPIO)  
potentiomenter x1 (AN01)	  
AX88796C RESET & SPI_SS (GPIO)

### Link & Documents ###
- Main Site
- Developer's blog
- [More Documents(Chinese)](/doc/zh_TW/) 
	- [About 關於](/doc/zh_TW/ABOUT.md)
	- [Other Info 其他訊息](/doc/zh_TW/OTHER.md)
  
### Screenshot ###
![demo_1](https://lh4.googleusercontent.com/-CCpQ02i9L5Y/UpWkku1h0bI/AAAAAAAAAFI/Q32WA6yvkuc/w700-h557-no/fm9baf314l_ax88796c_uip_demo1.jpg)  
▲ Demo1 Screen    
![demo_2](https://lh4.googleusercontent.com/-5i8VeRAL-sE/UpWxZcF4BMI/AAAAAAAAAHE/bWHt_CB3bl8/w697-h589-no/fm9baf314l_ax88796c_uip_demo2.jpg)  
▲ Demo2 Screen  

[MB9AF314L]: http://www.spansion.com/Products/microcontrollers/32-bit-ARM-Core/fm3/Pages/overview_32fm3.aspx "FM3 Family"
[ASIX]: http://www.asix.com.tw/index.php?&width=1152 "ASIX"
[uIP]: http://dunkels.com/adam/ "Adam Dunkels"