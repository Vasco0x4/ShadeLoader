# ShadeLoader

## Description

ShadeLoader is a shellcode loader that utilizes **process hollowing** and **XOR encryption** techniques to download, decrypt, and inject shellcode into a legitimate process.


![345470702-c013f61b-a0ef-42dd-9583-9b66548fbb5d](https://github.com/Vasco0x4/ShadeLoader/assets/63432221/0e3c9d4b-c227-4592-94b6-9b5f7b85220e)

## How It Works & Features

1. **Initial Access (main.exe)**:
    - `main.exe` starts a legitimate process, `cmd.exe`, in suspended mode.

2. **Downloading and Decrypting Shellcode**:
    - `cmd.exe` downloads the encrypted shellcode from a specified URL.
    - The shellcode is encrypted using XOR to obfuscate its content during transport.
    - Once downloaded, `cmd.exe` decrypts the shellcode using a key to obtain the clear executable code.

3. **Process Hollowing (Injection into RuntimeBroker.exe)**:
    - `cmd.exe` creates a new process, `RuntimeBroker.exe`, in suspended mode.
    - The decrypted shellcode is injected into the allocated memory of `RuntimeBroker.exe`.
    - `cmd.exe` creates a thread in `RuntimeBroker.exe` to execute the injected shellcode.

4. **Self-destruction**:
    - Once the shellcode is injected and executed, `main.exe` self-destructs.
  
5. **VM Protection and Anti-Debugging**
   
   The shellcode loader detect and protect against virtual machine environments and debugging tools.

    - If the malware detects that it is running in a virtual machine (VM) environment.
    - If the malware detects the presence of a debugger.
    - If the malware detects common VM artifacts or tools indicating analysis.

## How to setup

### config.h
you can setup shellcode loader 0 = disable | 1 = enable 

```
ENABLE_DEBUG 0 //  if enable its open windows with debug logs 
AUTODESTRUCT 1  // auto destuct after execution 
SUSPEND_DOWNLOAD 1  //  waiting time before download shellcode from web 
PROTECTION 1 // anti VM / disable IF YOU USE IN ON VM !!! 

```
### main.cpp
in main file you need to configure 2 think 
- shellcode URL | line 49
- XOR key use same key for encryption !!! | line 59

### demo / exemple 

Debug mode 

```cpp
#define ENABLE_DEBUG 1
#define AUTODESTRUCT 1 
#define SUSPEND_DOWNLOAD 0 
#define PROTECTION 0 
```


https://github.com/Vasco0x4/ShadeLoader/assets/63432221/abb22b64-c835-4ddc-8bb0-4b2abe450ca5



__________________________________________________________________________________________



Anti VM
```cpp
#define ENABLE_DEBUG 1
#define AUTODESTRUCT 1  
#define SUSPEND_DOWNLOAD 0 
#define PROTECTION 1 

```



https://github.com/Vasco0x4/ShadeLoader/assets/63432221/a5c6a2ed-88d5-4114-9224-e77d3869e294



__________________________________________________________________________________________


Silent  
```cpp
#define ENABLE_DEBUG 0
#define AUTODESTRUCT 1
#define SUSPEND_DOWNLOAD 1
#define PROTECTION 0
```


https://github.com/Vasco0x4/ShadeLoader/assets/63432221/4f167e13-8dd0-4b68-b8c3-76923f6df320




__________________________________________________________________________________________


### ShadeLoader vs AVs 2/40

https://kleenscan.com/scan_result/e116638a3bac264968b2a86f32e6a24db53f7f27335810caba68233ca5771e4d

### encrypt.py
also includes a encrypt.py file to encrypt the shellcode before downloading. You can use it to prepare your shellcode before deployment.

### Disclaimer
This project is for educational purposes only. Any malicious use of this code is strictly prohibited
