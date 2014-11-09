EteriumRift
===========

Oculus Rift driver for XNA game Eterium that I worked on. Written in c++. Split into a pure native component and a c++ CLI wrapper. Has dependency on Oculus Rift SDK.

Libs folder has a prebuilt dll you can just add as a reference to your XNA project. 

If you want to build yourself, modify OculusSDK.props to point to where you put the Rift SDK. Projects are Visual Studio 2012. The built DLL can be used in a
Visual Studio 2010 XNA project though.

See examples folder for code snippets to use as a guide. There aren't any true examples yet. 

Note that this does not work in Direct mode. I have not figured out a way to convince XNA to work in Direct mode. Use extended mode instead.
