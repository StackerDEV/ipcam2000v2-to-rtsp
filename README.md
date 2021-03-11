# IPCam2000v2 to RTSP stream

* This IoT security camera doesn't allow you to set-up local RTSP streaming for your software DVR because the Android/iOS application makes a cryptographic salted hash of your password and configures it on the device using avSendIOCtrl/avRecvIOCtrl.

* After some reverse engineering I found out the client application uses TuTk's IOTC SDK. The SDK headers and libraries where uploaded to GitHub, cool let's git clone.

## Basic Concept

The concept was to make a client from the SDK source that allowed to pass through the video stream to a local Live555 RTSP server in LAN mode so that we can add our RTSP to our software DVR so you don't have to connect the device to the cloud.
This is unlikely going to happen since this cloud feature is embedded in the firmware itself. Beside the project folder there's also another folder ;)

## Remote firmware updating 

There is no HTTPS used, do a mitm or bind dns redirect to a local webserver serving your modified firm or with see bins in bonus folder (use binwalk) not sure if there's a checksum and for unbricking. You have to first crack open the camera and test if there's a JTAG without password and if there is try to look in the firm bin with binwalk in /etc/ for the file passwd first check this before flashing and bricking it.. But again not sure have not opend the camera is not was mine, maybe there is a external eeprom soic8 and you can easily make it in-circuit flashing compatible with litle mod.

## Todo

Add send and receive handler for password change from AVAPI Client, or use the alternative Android application..

## Usage

### Enter your Trustsmartcloud password
After setting up your camera in your network you should start AVAPI Client as follows;

> AVAPIs_Client mypassword

Replace "mypassword" with your password of your Trustsmartcloud account. After this it will generate a salted SHA256 password and show you the RTSP streaming url after searching in the LAN network and after querying for device information.

Example

![](https://github.com/StackerDEV/ipcam2000v2-to-rtsp/blob/master/example.png)

### Device
Some device information

Device name      | uboot image             | firmware           | Manufacturer | Manufacturer model number |
---------------- | ----------------------- | ------------------ | ------------ | ------------------------- |
IPCAM-2000(v2)   | JVS-HI3518ES-8188       | 0.1.9.0            | Sharetronic  | S510                      |


### Penetration Capability
You should add the mac address to your firewall and block ALL traffic from this device to wan!
This device will connect to the cloud in many different ways.

```
trustsmartcloud2.com
iotcplatform.com
m1.iotcplatform.com
m2.iotcplatform.com
m3.iotcplatform.com
m4.iotcplatform.com
m5.iotcplatform.com
www.jovision.com
www.jovetech.com
becloud.vn
www.sunywo.com
www.onvif.org
fconfig.jovecloud.com
fconfigus.afdvr.com
jovephonealarmftpftp.yoosee.cc
appchannelen.afdvr.com
aa.sddyys.com
lfx1jovision.com
www.baidu.com
www.afdvr.com
int.dpool.sina.com.cn
```

#### Android
There's a alternative application less buggy and more user convenience.

https://play.google.com/store/apps/details?id=com.tutk.kalay


## License

> The license for the Kalay SDK can be found in the `LICENSE` file parallel to this `README.md`.
