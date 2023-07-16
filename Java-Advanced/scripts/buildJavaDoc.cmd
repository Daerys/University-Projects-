@echo off
set source_dir=..\java-advanced-2023\
set classpath=%source_dir%source\modules\info.kgeorgiy.java.advanced.implementor\info\kgeorgiy\java\advanced\implementor;%source_dir%source\modules\info.kgeorgiy.java.advanced.base\info\kgeorgiy\java\advanced\base;%source_dir%lib\*
set implpath=..\java-solutions\info\kgeorgiy\ja\vikulaev\implementor\Implementor.java %source_dir%modules\info.kgeorgiy.java.advanced.implementor\info\kgeorgiy\java\advanced\implementor\Impler.java %source_dir%modules\info.kgeorgiy.java.advanced.implementor\info\kgeorgiy\java\advanced\implementor\JarImpler.java %source_dir%modules\info.kgeorgiy.java.advanced.implementor\info\kgeorgiy\java\advanced\implementor\ImplerException.java

mkdir ..\javadoc

javadoc -cp %source_dir%artifacts\*;%source_dir%lib\*; -d ..\javadoc -author -version -private -Xdoclint:none %implpath%

PAUSE