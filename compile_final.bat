cd /d "C:\Users\user\Downloads\JUCE-master\JUCE-master\extras\Projucer\Builds\VisualStudio2022\MyVST3Plugin\build"
call "C:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
cmake -G "NMake Makefiles" ..
nmake
