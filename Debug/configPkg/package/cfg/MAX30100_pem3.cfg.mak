# invoke SourceDir generated makefile for MAX30100.pem3
MAX30100.pem3: .libraries,MAX30100.pem3
.libraries,MAX30100.pem3: package/cfg/MAX30100_pem3.xdl
	$(MAKE) -f D:\CCS\workspace_v7\i2ctmp007_CC2650STK_TI/src/makefile.libs

clean::
	$(MAKE) -f D:\CCS\workspace_v7\i2ctmp007_CC2650STK_TI/src/makefile.libs clean

