# Getting Connected to your RPi Zero

![5 RPi's loaded and tested with PSU's](/Documentation/5RPis.jpg)

Your RPi has been fitted with an SD which we have preloaded with Raspberry Pi 32bit Bookworm.  It has also been set up to connect to a specific WiFi and has been enabled with RealVNC server.

You need to connect your laptop via RealVNC viewer on your laptop. You can download it from [here](https://www.realvnc.com/en/connect/download/viewer/).  Of course you will need Internet access to do it!  If you don't have access (eg. phone hot spot access) see Mick or one of the others.

There is a label on the back of the of the RPi showing its number and IP Address (without the dots though!)

When you connect up through the micro USB (PWR) you will see the green LED flashing.
After a little while your RPi will stop flashing and it should be connected to the WiFi AP (MRPMG)

Now switch your laptop WiFi to the AP, MRPMG with password 'password'. This AP is only a local WLAN and has no Internet connection.

Open your RealVNC viewer and set up a new connection using the IP address provided with the board.
   Use IP address 10.1.1.10x (x is the pi nbr)
![A config on RealVNC viewer](/Documentation/RealVNC-Config.png)

   Double click you new session to launch
   ![Select your newly configured session](/Documentation/Session_definitions_on_RealVNC_viewr.png)

   Accept the certificate warning
   Use pi as the user ID
   Use password as the p/w
   Check the remember box, hit enter

If you are successful you will see the Bookworm background image. !
![Here are 5 active sessions!](Documentation/5_active_ sessions.png)
 **Well done!**
