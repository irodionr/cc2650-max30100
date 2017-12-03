# invoke SourceDir generated makefile for MAX30100.pem3
MAX30100.pem3: .libraries,MAX30100.pem3
.libraries,MAX30100.pem3: package/cfg/MAX30100_pem3.xdl
	$(MAKE) -f D:\Repo\MAX\lib\Board/src/makefile.libs

clean::
	$(MAKE) -f D:\Repo\MAX\lib\Board/src/makefile.libs clean

