# PlanetC
*A powerful user interface for Stellarium.*

** WORK IN PROGRESS: THE PlanetC REPOSITORY IS UNDER CONSTRUCTION **

Planetarium Control, or **PlanetC** for short, allows easy control of almost all [Stellarium](http://stellarium.org/) functionalities through a powerful and intuituive graphical user interface.  Its main purpose is to help lecturers and operators to show astronomical phenomena using the Stellarium simulation software, either projected on a screen or on a planetarium dome.  A preliminary documentation on **PlanetC** can be found in the [wiki pages](https://github.com/gcalderone/PlanetC/wiki).


**PlanetC** main features are:

- an easy-to-use graphical user interface shown on a separate window with respect to the main Stellarium window;
- extensive control over (almost) all Stellarium functionalities;
- a "live" image to check in real time what's being shown on the main Stellarium window;
- draw each constellation art in different colors;
- script handling (start, pause, stop);
- show movies and images;
- an enhanced zomm/unzoom facility;
- a set/restore facility to easily handle global Stellarium state in scripts;
- customizable list of "quick" object selection buttons;
- customized date, time, location and cardinal points display, specifically designed for use in planetariums;
- ability to clone and rotate movie and image views, to be displayed as twin pictures in planetarium domes;

All **PlanetC** operations are performed in its own window, without influencing the main Stellarium view, allowing the operators to properly control Stellarium while the audience focuses on the sky simulation.


**PlanetC** is developed as an independent Stellarium plugin, however it requires a few tweaks in the original  code to work properly.  Hence we provide both the Stellarium modified code and the plugin code in a single package.  We tried to kept the modifications to the original Stellarium code as small as possible, in order to closely follow each Stellarium release.  Currently, we are able to deploy a new **PlanetC** version a few hours after each Stellarium release.

**PlanetC** has the same dependencies and compilation procedure of Stellarium: once you are able to compile the original Stellarium code, you can compile **PlanetC** without any additional effort.


**PlanetC** has been developed in close collaboration with [Star Freedom](http://www.starfreedom.it/), a non-profit association involved in science lecturing and astronomy activities in several schools of south Italy.  The Star Freedom operators successfully employed **PlanetC** as main control software in many hundreds shows performed within their travelling planetarium.

**PlanetC** is free software: you can redistribute it and/or modify it under the terms of the GPL (see the LICENSE file for more details).
