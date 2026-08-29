# The 'Missile-Bot Commando' Robot (LEGO Technic + HandyBoard SBC + Two Wii Controllers) - From 2010

This repo documents the evolution and development of a 'Missile-Bot Commando' robot based on LEGO Technic, a 6811 [HandyBoard SBC](http://handyboard.com) (or this [link](https://www.cs.uml.edu/~fredm/handyboard.com/hb.html)), two [Wii controllers](https://en.wikipedia.org/wiki/Wii_Remote), [Bluetooth](https://en.wikipedia.org/wiki/Bluetooth) + [RS-232](https://en.wikipedia.org/wiki/RS-232) communications and a Windows host controller computer. While this does not fall into my "1970s and 1980s retrocomputing" camp it is getting there in terms of its age and antiquity given its 2000-era HandyBoard SBC.

The project went through many variations before settling on the final design presented in this repo.

<div style="text-align:center">
<img src="/Images/Facing left.webp" alt="" style="width:75%; height:auto;">
</div>

## The Premise for the Build

The whimsical premise behind this project was to create a child's play toy which acted as an intelligent, autonomous robot which could play “Nerf Tag” with them or act as a stand-in opponent in a game of Nerf Tag. There are children who would like to play Nerf Tag but they do not have friends to play the game or their friends are not available to play. It would basically be a 'computerized Play Buddy'.

The basic idea is for a child to wear special glasses which emit an infra-red light source. The Wii Controller’s infra-red camera on the robot would detect the location of the child, then move and orient the robot + its Gatling gun to point at the child. The Gatling gun would then fire its rubber darts.

The setup was expanded with a second Wii controller + numchucks which allowed the user/child to manually move and orient the robot as well as fire off the darts from the Gatling gun.

## How it Works

The following block diagram provides a cursory overview its structure:

<div style="text-align:center">
<img src="/Images/block_diagram.webp" alt="" style="width:75%; height:auto;">
</div>

- The core of the system is a HandyBoard single-board-computer which sits on the LEGO Technic robot:

<div style="text-align:center">
<img src="/Images/HandyBoard_PCB.webp" alt="" style="width:50%; height:auto;">
</div>

- It controls various devices:

  - Two Technic motors for the wheels.

  - One Technic motor for the rotation of the turret (which holds the Gatling gun).

  - One Technic motor to rotate the "Wii controller infrared detection head" slightly up and down to help aim the Gatling gun.

  - One R/C servo motor to precisely control the incremental rotation of the Gatling gun.

  - Sonar and infrared sensors for obstacle detection.

<div style="text-align:center">
<img src="/Images/DSC_0048.webp" alt="" style="width:75%; height:auto;">
</div>

- A Windows laptop runs a Win32 C++ <a href="/Images/master_controller_ui.webp">master control program</a> that controls:

  - Sending commands to the HandyBoard computer via a wired RS-232 interface

  - Talking to the Wii hand-held controller #1 (via Bluetooth), held by the child, which allows them to move and orient the robot

  - Talking to the Wii hand-held controller #2 (via Bluetooth), which sits on top of the robot, to determine when it detects the infra-red light source emitted from the child's glasses or "light pen"

- For this project we converted a yellow marker pen into a portable infra-red emitting light source with a small button (which was taped to glasses worn by the test-subject child).

<div style="text-align:center">
<img src="/Images/Infrared light pen.webp" alt="" style="width:25%; height:auto;">
</div>

## The Gatling Gun

Rather than firing Nerf Darts we came to design a complex mechanism to fire 'Lego Cyberslam Missiles' using a rotating Gatling-gun type of firing mechanism. This, in the end, was the most damning and complex aspect of the entire project. The mechanism is rotated by way of a precise R/C servo motor:

<div style="text-align:center">
<img src="/Images/robot_top.webp" alt="" style="width:75%; height:auto;">
</div>

## Building the Robot

The building stage was broken into several parts:

- The design of the base was important since it would hold the HandyBoard computer, the pivoting mechanism, the wheels and the sensors. Hence, it needed to be strong, resilient to stresses and well balanced. We thought about a Mars Rover Design, a “Diffento” 4-wheel “Syncho Drive”, a simple “Bot” (2 wheels and a skid), a “Rocker Boogey” and others. An additional innovation was in the method by which the HandyBoard computer was attached to the base platform. Originally the base had 4 wheels but then we discovered that it could not easily "spin in place" so the front two wheels were replaced with a single pivot wheel.

<div style="text-align:center">
<img src="/Images/robot_base.webp" alt="" style="width:75%; height:auto;">
</div>

<div style="text-align:center">
<img src="/Images/DSC_0026.webp" alt="" style="width:75%; height:auto;">
</div>

- We did not know what kind of shooting mechanism to initially make and reviewed many existing LEGO Cyberslam Missiles shooting designs which did not impress us. We ultimately came up with our own design based on 6 incremental design improvements. We opted to mount the 6 missiles on a Technic tire rim which itself connected to a R/C airplane servo mounted to a Lego 40 tooth gear via 4 screws.

<div style="text-align:center">
<img src="/Images/DSC_0031.webp" alt="" style="width:45%; height:auto;">
<img src="/Images/DSC_0074.webp" alt="" style="width:45%; height:auto;">
</div>

- A core design problem was "how to mount the Gatling gun + Wii Controller so that they could be rotated and tilted together", such that this mechanism could locate and shoot at an opponent. We knew this would be the biggest design problem to overcome. We did some incremental work with LEGO, starting from the 6 Cyberslam Missiles and working backwards towards the base of the robot. We added a cradle for the Wii Remote which was a neat and innovative idea. The rotation pivot + motor was difficult and caused many headaches. It was also a bit top-heavy, with the gun + Wii Remote placing the center-of-gravity too high.

<div style="text-align:center">
<img src="/Images/DSC_0035.webp" alt="" style="width:auto; height:75%;">
</div>

<div style="text-align:center">
<img src="/Images/DSC_0040.webp" alt="" style="width:auto; height:75%;">
</div>

<div style="text-align:center">
<img src="/Images/DSC_0059.webp" alt="" style="width:auto; height:75%;">
</div>

- The last big design decision was how to rotate the mechanism so that it could spin around in 360 degrees, allowing it to look for opponents around the robot. As shown in the images, we ultimately came up with a complex design to place the top mechanism on a rotating platform controlled by a Technic motor, which was strong enough and rigid enough to keep the Wii controller and Gatling gun stable.

<div style="text-align:center">
<img src="/Images/rotating_base_up_close.webp" alt="" style="width:75%; height:auto;">
</div>

<div style="text-align:center">
<img src="/Images/DSC_0045.webp" alt="" style="width:auto; height:75%;">
</div>

- A core problem was that the Technic motor used to tilt the "head" up and down was too powerful (not geared down enough) and hence did not allow for fine control over the angle of the head.

- The heavy HandyBoard computer was mounted over the main wheel axial to provide for better traction and also to counter-balance the top-heavy pivoting mechanism.

## Communications

- The Windows-based host controller needed to talk with the Wii controller mounted on top of the robot to acquire its tilt rotational information and its 4 moving Infra-Red target locations. We purchased two different Bluetooth USB adapters as the first one would not work reliably. We also needed to find a Wii C++ library to communicate with the Wii controller and opted for the [WiiYourself! by gl.tter](/Src/Wii_yourself_c++_library) library.

- The Windows-based host controller also needed to communicate with a second Wii controller (and attached numchuck) which the child/user could use to control the movement/orientation of the robot and the firing of the Gatling gun themselves.

- The Windows-based host controller additionally needed to communicate with the HandyBoard SBC which was done over a RS-232 serial interface. The open source [CSerial - A C++ Class for Serial Communications](<Src/Windows Host Master Controller/Serial.cpp>) from Codeguru was used to speed up the coding process.

## Programming

Two programs were written in C++ and IC4:

- A Win32-based [Master Controller](<Src/Windows Host Master Controller/master_controller.cpp>) UI interface was written for the Microsoft Windows laptop computer in C++ and Visual Studio.  Its job was to coordinate data between the Wii Remote sensor (for tracking the opponent via Infra-Red light) and the HandyBoard computer for invoking the commands of the laptop computer.

<div style="text-align:center">
<img src="/Images/master_controller_ui.webp" alt="" style="width:75%; height:auto;">
</div>

- A [client program](<Src/HandyBoard Client-Side Controller/HandyBoard IC4 Client-Side Controller.ic>) written in '[KISS Interactive C](https://en.wikipedia.org/wiki/Interactive_C)' (IC4) residing on the HandyBoard computer of the robot which would receive commands from the laptop computer via a RS-232 serial wire. It would use these commands to drive the robot around the floor, to rotate the “head” of the robot and to fire the Cyberslam Missiles.

## Runtime Prerequisites

- DLINK DBT-120 Bluetooth USB adapter. Use the Toshiba Stack which is known to work.

- Windows-based laptop with the following installed:

  - Toshiba Bluetooth stack
  - KISS Interactive IC4 compiler version 8.0.2
  - Microsoft Visual Studio C++ compiler

## How to Compile and Run the System

NOTE: You cannot have the "IC4" compiler running at the same time when this master controller is running since they both hog the COM1 serial port.

- Compile the master controller program via Microsoft Visual Studio

- Attach the HandyBoard power/serial board to the RS-232 port of laptop

- Attach the telephone cord from the HandyBoard to the power/serial board

- Wii #1 = Wii controller + numchuck

- Wii #2 = Wii controller (this sits on top of the robot)

- Download the firmware to the HandyBoard via IC4

- Start IC4 and load in "HandyBoard IC4 Client-Side Controller.ic". Compile then download to the HandyBoard.

- Start the Toshiba Bluetooth manager from the laptop's tray then:

  - Connect to the Wii#1 first (with numchuck). You will have to press the red button inside the Wii remote.

  - Connect to the Wii#2 second (the one attached to the top of the robot)

- Start the Master_controller.exe host program on the laptop.

## Button Key Sequences on the Main Wii Controller

- Arrow keys = Moves the robot base forward, backward, left and right

- Minus key (-) = Reset Gatling gun servo to start position

- Plus key (+) = Increment Gatling gun servo 1000 steps to fire a missile

- Numchuck joystick left = Rotate turret to the left

- Numchuck joystick right = Rotate turret to the right

- Numchuck large button = Reset Gatling gun servo to start position

- Numchuck small button = Increment Gatling gun servo 1000 steps to fire a missile

- "A" key = Allow infra-red camera on Wii#2 to start controlling the robot

- "A" key + "B" (trigger) key = Allow robot to fire the next missile, when it is beeping

## How The Robot Tracks The <a href="/Images/Infrared light pen.webp">Infrared Light</a>

- You will need the special "infra-red yellow pen" created for this project. This acts as our infra-red light source for the robot to track the user's position.

- Point the pen towards the infra-red camera on Wii#2, sitting on top of the robot.

- Press the "A" key on Wii#1 so that the robot can begin to move the turret left and right. Move the infrared light source.

- When the infra-red light source is roughly in the middle of the FOV of the Wii controller's camera, the robot will beep. Press the "B" trigger button to allow the robot to fire a missile.

- When all missiles have been fired, press the "-" key to reset the Gatling gun's position.

## [Master_Controller.exe](<Src/Windows Host Master Controller/master_controller.cpp>)

This resides on the laptop and talks to the 2 Wii controllers, as well as providing a keyboard (mouse) UI. It translates commands into RS-232 single-character commands sent to the robot at 1200 baud.

The two list boxes of the Win32 UI provide real-time information about the connected Wii controllers and the values that they are reporting.
