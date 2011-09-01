Generating
----------

Following files need to be generated:

Features.xml
Resources.xml
Compile.hxml (not needed if you specify parameters to haxe directly)


Compiling
---------

swfmill simple src\Resources.xml bin\Resources.swf
abcsymbol bin\Resources.swf bin\Resources_new.swf
haxe -cp src -main Main -swf bin/$(OutputName) -swf-version 9 -debug -swf-lib bin/Resources_new.swf -resource src/Features.xml@features