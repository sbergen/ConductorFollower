ConductorFollower
=================

Virtual conductor follower:
- Implemented as a VST plugin (uses JUCE)
- Score is input as a standard midi file
- OpenNI is used for motion tracking

Made as part of my M.Sc. thesis work at the Media Technology lab at Aalto School of Science.

Project files only for VS2010 included, but the code should be portable.
Requires an external synthesis environment to work.
Also, interfacing the synthesis environment requires writing instrument definition files.
Sample files are included for Vienna Symphonic Library Special Editions 1, 1+, 2, and 2+.

External dependencies:
- Boost (http://www.boost.org/)
- OpenNI SDK (http://www.openni.org/)
- Juce (http://www.rawmaterialsoftware.com/juce.php)

Licensing: 
- All original code under GPL v3 for now, Copyright (C) Sakari Bergen 2012.
- Unofficial Boost libraries (sources included) are under the Boost license.
- JUCE ("amalgamated" sources included) is dual licensed. Non-commercial usage under the GPL.
