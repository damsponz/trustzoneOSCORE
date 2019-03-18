#trustzoneOSCORE
##Global informations
This project is a proof of concept to secure IoT device with ARM trustZone Cortex M feature. This code is design for Nuvoton M2351 (M2351KIAAE) board. We create a first step to implement OSCORE on constrained device with secure usage of the keys.

Two hardware features are enabled :

1.	AES crypto accelerator
2.	WiFi module (ESP8266)

Nuvoton and ARM split the memory in two type Secure and NonSecure. Each mode use a dedicated address range, and in fact NonSecure world can’t access to Secure memory. For more informations check this (url nuvoton).

##Files and functions
The project is organized with thoses files.

###Nuvoton\_M2351\_wifi\_module.h (Secure Folder)

All functions in this file can just be called by Secure World.

**void WIFI\_PORT\_Init(void);**

Init clock, set baud rate, set packet pattern on UART 3 port.

**void WIFI\_PORT\_Start(void);**

Init ESP8266 WiFi module and set the configuration as next :

1.	Set Access Point mode
2.	Enable DHCP as Access Point
3.	Set SSID « NuvotonBoard » with WPA2 PSK « oscorepass » on channel 5 for this AP
4.	Set IP and Gateway with 192.168.4.254 for this AP
5.	Start listen on UDP port 5386 (some lines can be comment and uncomment to change to TCP)

**void WIFI\_PORT\_Write(int print, char \*command, int nbCharCommand);**

	@param print          : if this param is set to 1, printf are active on UART0
	@param command        : command string to be send (without "\0" but with the end of AT command "\r\n")
	@param nbCharCommand  : command string length
	@details              : Send AT command on UART port to ESP module
	
**void WIFI\_PORT\_Read(int print);**

	@param print          : if this param is set to 1, printf are active on UART0
	@details              : Wait for receive answer, char by char, and wait response from ESP module "OK" message on UART port
	
###Nuvoton\_M2351\_crypto\_aes.h (Secure Folder)
All this functions are not available from Non Secure World.

**void Nuvoton\_M2351\_crypto\_init(uint8\_t channel, uint8\_t modeAES);**

	@param channel        : [0-3] Choose one of 4 hardware channels of AES crypto accelerator.
	@param modeAES        : Specified if cipher or decipher AES primitive is used.
		Two values are used for set modeAES:
		  #define ENCRYPT 1
		  #define DECRYPT 0

Init crypto module specified with channel parameter, and set AES mode. Set primitive as AES-ECB.

**void Nuvoton\_M2351\_crypto\_useSessionKey(uint8\_t channel);**

	@param channel        : [0-3] Choose one of 4 hardware channels of AES crypto accelerator. Must be init before use this function.

1. Call Nuvoton\_M2351\_crypto\_init() with the channel 1 and DECRYPT mode.
2. Call useMasterKey() to load masterKey in AES channel 1
3. Call Nuvoton\_M2351\_decrypt\_data() to decipher SessionKey
4. Load clear SessionKey in AES channel passed in parameter, load a zero IV in the same AES channel and delete SRAM memory about clear SessionKey and Initial Vector.

**void Nuvoton\_M2351\_crypto\_useMasterKey(void);**

Load hardware masterKey in AES channel 1 without pass any data in SRAM. The UID (96bits) +32bits of Nuvoton board is used as 128 bits master key. The design code don't push master key in SRAM, but directly in the AES register via dedicated AES DMA.

**void Nuvoton\_M2351\_encrypt\_data(uint8\_t channel, uint8\_t InputData[], uint8\_t OutputData[]);**

	@param channel        : [0-3] Choose one of 4 hardware channels of AES crypto accelerator. Must be init before use this function.
	@param InputData      : 16 bytes for 128 bits of plain data
	@param OutputData     : 16 bytes for 128 bits of return ciphered data

Start AES cipher primitive on the channel parameter passed. The channel must be init and a key must be loaded in the AES crypto accelerator before use this function.

**void Nuvoton\_M2351\_decrypt\_data(uint8\_t channel, uint8\_t InputData[], uint8\_t OutputData[]);**

	@param channel        : [0-3] Choose one of 4 hardware channels of AES crypto accelerator. Must be init before use this function.
	@param InputData      : 16 bytes for 128 bits of ciphered data
	@param OutputData     : 16 bytes for 128 bits of return plain data
	
Start AES decipher primitive on the channel parameter passed. The channel must be init and a key must be loaded in the AES crypto accelerator before use this function.

###Oscore.h (Secure Folder)

All functions in Oscore.c file (with __NONSECURE_ENTRY) can be called by Secure AND NonSecure World.

#####_Crypto_
***

**int32_t Encrypt_data(uint8_t \*plainData, uint8_t \*cipheredData);**

Encrypt plainData (16 bytes) and return 16 bytes in cipheredData. This primitive use AES ECB with the AES crypto accelerator. This function set AES channel to 0 and AES mode to ENCRYPT.

**int32\_t Decrypt\_data(uint8\_t \*cipheredData, uint8\_t \*resultData);**

Decrypt cipheredData (16 bytes) and return 16 bytes in resultData. This primitive use AES-1 ECB with the AES crypto accelerator. This function set AES channel to 0 and AES mode to DECRYPT.

**int32\_t Store\_key(uint8\_t \*newKey);**

Store 128 bits ciphered session key received by network. This key is stored as ciphered data and deciphered just when it's needing. A hardware key is use to deciphered session key. This master key is presume in the board like the OSCORE protocol design. For this demo we used UID (96bits) +32bits of Nuvoton board as 128 bits master key. The design code don't push master key in SRAM, but directly in the AES register via dedicated AES DMA.
	
**int32\_t Store\_iv(uint8\_t \*newIv);**

Store 128bits initial vector received by network.

#####_WiFi_
***
**typedef struct NETWORKDATA networkData;**

	@var0                 : char *data
	@var1                 : int lentgh

This structure use a int length variable and a list of char (char \*). This is use to receive and send data via wifi connection module.

**int WIFI\_PORT\_Receive_Data(int print, networkData \*netData);**

	@param print          : if this param is set to 1, printf are active on UART0
	@param netData        : data structure where received data will be return (length and data). Memory must be reserved before call this function.

This method is used to wait data packet from network. The waiting pattern is +IPD,[channelID],[lengthData]:[data]. The packet parsing is made in and return a networkData structure.

**int WIFI\_PORT\_Send\_Data(int print, networkData \*netData);**

	@param print          : if this param is set to 1, printf are active on UART0
	@param netData        : data structure to be sent (length and data). Memory must be reserved before call this function.

This method is used to send data packet defined in the networkData structure passed as parameter.

#####_Print & LED_
***
**int32_t printBlock(uint8_t \*block);**

Print 128 bits data on UART0 as matrix 4x4 view.

**int32\_t printSecure(char \*string, void \*ptr, uint8\_t val);**

	@param string         : Format string (first paramater of function printf)
	@param ptr            : pointer to data, related a variable present in format string (second parameter of function printf). Must set to NULL if no variable in the format string.
	@param val            : value like ptr parameter but for direct uint8\_t. Must set to NULL if no variable in the format string.

This function is used by NonSecure program to print informations in the UART0. This is usefull because UART0 is set as Secure peripheral. But be careful because the printf used in this function can access to Secure data and leak information is more simple. **This function don't be kept in production software implementation.**

**int32\_t printNetworkData(networkData \*netData);**

Print networkData structure on UART0.

**int32\_t Secure\_LED\_On(void);**

Set Green LED on

**int32\_t Secure\_LED\_Off(void);**

Set Green LED off

###main_ns.c (NonSecure Folder)

**typedef struct NETWORKDATA networkData;**

This structure is the same as NETWORKDATA structure defined in Secure World in Oscore.h file. This second declaration is used by NonSecure components to exchange data between the two mode.

**void stringKeyToKey(char \*stringKey, uint8\_t \*key);**

	@param stringKey      : Chain of ASCII characters
	@param key            : 16 bytes for 128 bits of data or AES key

Convert chain of 32 ASCII char into 16 bytes uint8\_t data.

**void keyToStringKey(uint8\_t \*key, char \*stringKey);**

	@param key            : 16 bytes for 128 bits of data or AES key
	@param stringKey      : Chain of ASCII characters

Convert 16 bytes uint8\_t into 32 ASCII char data.

**void demoPres(void);**

This function is an example of the first step to secure key with network bootstrap like OSCORE :

1.	NonSecure world wait to received data via network (wifi module in Secure World)
2.	Store the data as CipheredSessionKey in the Secure memory
3.	NonSecure world wait to received data via network (wifi module in Secure World)
4.	Store the second data packet as ciphered data
5.	Decrypt the second data packet with the sessionKey. (The sessionKey is deciphered by master key in Secure world before it and after usage the clear sessionkey is deleted)
6.	NonSecure world give back on UART0 the plain data and make an answer.
7.	The answer is ciphered with sessionKey. (The sessionKey is deciphered by master key in Secure world before it and after usage the clear sessionkey is deleted)
8.	The ciphered answer is send via wifi to the destination.

To make this demo a computer with netcat or something equivalent is required :

	#$> nc -4u 192.168.4.254 5386
	7f3591d36fd517a37b6de9e0df934b7a
	3b6fe4588f53c3a96f5e9776fbb20e9a

**void testCryptDeCrypt(void);**

This function is another exemple. This is a proof of concept about AES-ECB with FIPS-197 exemple on Nuvoton board. 128 bits of plaindata (32 43 f6 a8 88 5a 30 8d 31 31 98 a2 e0 37 07 34) are ciphered with (2b 7e 15 16 28 ae d2 a6 ab f7 15 88 09 cf 4f 3c) key. After the first result is deciphered and the second result is compared with the plaindata input. 

For implement the FIPS-197 example useMasterKey must be edit with FIPS Key. (comment / uncomment 4 lines keys). Also UID Board is used as masterKey.

**int main(void);**

At the start this main function is waiting of press on SW2 or SW3

- If SW2 is pressed : the _demoPres()_ function is start.
- If SW3 is pressed : the _testCryptdeCrypt()_ function is start.

After one loop SW2 or SW3 can be press to execute another demo.

###main.c (Secure Folder)

**int main(void);**

This file init the board (ex : clock), load WiFi module with _WIFI\_PORT\_Init()_ function and boot to the NonSecureWorld. The main_ns.c is launched.
