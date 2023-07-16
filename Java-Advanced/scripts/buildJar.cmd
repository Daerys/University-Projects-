set source_dir=../java-advanced-2023/
set source_classes=%source_dir%modules/info.kgeorgiy.java.advanced.implementor/info/kgeorgiy/java/advanced/implementor/

mkdir ./bin
:NOTE: сдлеать в другусю сторону /
javac -d ./bin -cp %source_dir%artifacts/*;%source_dir%lib/*;  ../java-solutions/info/kgeorgiy/ja/vikulaev/implementor/Implementor.java
jar -c -m MANIFEST.MF   -f implementor.jar --module-path %source_dir%artifacts;%source_dir%lib -C ./bin .

rd /s /q ./bin
PAUSE

