### <a name="MB9AF314L"></a>MCU Description ##
**MB9AF341L**  

32-bit ARM Cortex-M3 Core (r2p1)  
 - Up to 40MHz Frequency Operation  
 - Nested Vectored Interrupt Controller (NVIC)  
 - 24-bit System timer (SysTick)

On-chip Memories  
 - 256 Kbyte Flash  
 -  16 Kbyte SRAM for high-performance CPU  
 -  16 Kbyte SRAM for CPU/DMA Controller

USB Interface (Device / Host)  
 - USB2.0 Full/Low Speed supported  
 - Max. 6 EndPoint supported

Multi-function Serial Interface (Max. 8channels)  
 - UART, CSIO, LIN, I2C

DMA Controller (8channels)

A/D Converter (Max. 16channels)  
 - 12-bit A/D Converter  
 - Built-in 3unit  (MB9AF312L, F314L built-in 2unit)

Base Timer (Max. 8channels)  
 - Operation mode: 16-bit PWM, 16-bit PPG, 16/32-bit reload, 16/32-bit PWC

General Purpose I/O Port  
 - 51 fast I/O Ports

Multi-function Timer (Max. 2unit)

Quadrature Position/Revolution Counter (QPRC) (Max. 2unit)

Dual Timer (Two 32/16bit Down Counter)

Watch Counter

External Interrupt Controller Unit  
 - Up to 16 external vectors  
 - Include one non-maskable interrupt(NMI)

Watchdog Timer (2channels)

CRC (Cyclic Redundancy Check) Accelerator

Clock and Reset  
 - 5 clock sources (2 ext. osc, 2 CR osc, and PLL)  
 - Reset sources: INITX Pins, POR, SW, Watchdog, LVD, CSV

Clock Super Visor (CSV)  
 - Ext. OSC clock failure (clock stop) detect  
 - Ext. OSC frequency anomaly detect

Low Voltage Detector (LVD)  
 - LVD1: error reporting via interrupt  
 - LVD2: auto-reset operation

Low Power Mode  
 - 3 power saving modes (SLEEP, TIMER, STOP)

Debug  
 - Serial Wire JTAG Debug Port (SWJ-DP)

----------
### <a name="AX88796C"></a>Low-Power SPI or Non-PCI Ethernet Controller 
**AX88796C**  

- High-performance non-PCI local bus
	- Supports 8/16-bit SRAM-like - host interface (US Patent Approval), easily interfaced to most common embedded MCUs; or 8/16-bit local CPU interface including MCS-51 series, Renesas series CPUs  
	- Supports Slave-DMA to minimize CPU overhead and burst mode read & write access for frame reception & transmission on SRAM-like interface  
	- for high performance applications Supports variable voltage I/O (1.8/2.5/3.3V) and programmable driving strength (8/16mA)
	- Interrupt pin with programmable timer 
- High-performance SPI slave interface  
	- Supports SPI slave interface for CPU with SPI master. The SPI slave interface supports SPI timing mode 0 and 3, up to 40MHz of SPI CLK, variable voltage I/O and programmable driving strength
	- Supports optional Ready signal as flow control for SPI packet RX/TX
- Single-chip Fast Ethernet MAC/PHY controller
	- Embeds 14KB SRAM for packet buffers  
	- Supports IPv4/IPv6 packet Checksum Offload Engine to reduce CPU loading, including IPv4 IP/TCP/UDP/ICMP/IGMP & IPv6 TCP/UDP/ICM Pv6 checksum generation & check  
	- Supports VLAN match filter Integrates IEEE 802.3/802.3u standards compatible 10BASE-T/100BASE-TX (twisted pair copper ode) Fast Ethernet MAC/PHY transceiver in one single-chip
	- Supports twisted pair crossover detection and correction (HP Auto-MDIX)
	- Supports full duplex operation with IEEE 802.3x flow control and half duplex operation with back-pressure flow control
	- Supports auto-polling function
	- Supports 10/100Mbps N-way Auto-negotiation operation
- Advanced Power Management features
	- Supports dynamic power management to reduce power dissipation during idle or light traffic period
	- Supports very low power Wake-On-LAN (WOL) mode when the system enters sleep mode and waits for network event to awake it up. The wakeup events supported are network link state change, receipt of a Magic Packet or a pre-programmed Microsoft Wakeup Frame or through GPIO pin
	- Supports Protocol Offload (ARP & NS) for Windows 7 Networking Power Management
	- Supports complete I/O pins isolation during WOL mode or Remote Wakeup Ready mode to reduce leakage current on non-PCI and SPI slave host interface
- Supports optional EEPROM interface to store MAC address
- Supports up to four GPIOs and two of them support Wake-On-LAN
- Supports programmable LED pins for various network activity indications with variable voltage I/O and programmable driving strength Integrates voltage regulator, 25MHz crystal oscillator and power on reset circuit on chip
- Supports optional clock output (25, 50 or 100MHz) for system use, if 25MHz crystal is present
- Supports optional clock input (25MHz) from system
clock to save the 25MHz crystal cost 
- 64-pin LQFP RoHS compliant package 
- Operates over 0 to +70°C or -40 to +85°C temperature range  

----------
