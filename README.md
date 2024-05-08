<p align="center">
    <img src="assets/ficon.png">
</p>

## IconJector
This is a Windows Explorer DLL injection technique that uses the change icon dialog on Windows. 


https://github.com/0xda568/IconJector/assets/61665703/bbf53775-ad8d-422b-b186-663ff5074976


## How does it work?
Firstly, a folder is created in the temp directory, and the properties of the folder are opened using SHObjectProperties. To retrieve the handle of the window independently of the system language, EnumWindows is used with a callback function that checks for the distinct folder name in every open window. 

![callback function](assets/callback1.png) 

Through the properties page, the change icon dialog is invoked, whose handle is also retrieved with EnumWindows. Lastly, the icon path is changed to a DLL (which has a .ico extension in this case), which causes the explorer to load the DLL after the OK button is pressed.

## The icon
The "icon" is actually a simple DLL file with an .ico extension that opens the calculator app with an icon resource that was added using the [Resource Hacker]("https://www.angusj.com/resourcehacker/").


There are some anomalies when it comes to the appearance of the icon in the folder view. I have tested this on two different Windows 10 versions (the newest and an older one) and on Windows 11.

### Windows 10
On both of the Widnows 10 versions, the icon stored inside the DLL was not displayed when the extension was changed to .ico

![win10 dll](assets/win10_dll.png)

On the newest Windows 10 version, however, it did work until I tried to open the "icon" inside of the photo app. Since then, I have not been able to recreate it.

![new win10 icon](assets/applied_icon.png)

### Windows 11
On Windows 11, the icon was displayed just like a normal icon in the Explorer view. When it is opened in the photo app, however, an error is displayed.


https://github.com/0xda568/IconJector/assets/61665703/748c1012-1d2c-47dd-915f-860ffc6d1b05


This behavior is certainly interesting, since a user who simply wants to change the icon of a folder may instead inject a malicious DLL into their explorer.

## Why?
This is a very stealthy way to inject a DLL into the Explorer process because it doesn't use the usual DLL-injection API. It does produce two short popups, which aren't too disturbing, however. So it can be used as an AV evasion technique by a threat actor to execute code through the legitimate explorer process or to load a UMD rootkit. 

Also, spreading DLLs masked as .ico files may be an interesting attack vector.

## Tools used
To get the right window handles, I used Spy++, which is a tool that gets installed with Visual Studio. 

![spy++](assets/spy_pp1.png)

## Credits
Many thanks to Bill G. from Microsoft for the beautiful folder icon!

Thank you, random guy on the internet, for the beautiful injection icon!
