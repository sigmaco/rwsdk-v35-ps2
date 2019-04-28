rem Batch script to convert .i fiel into a .h file.
rem Ensure that sed is included in your path.

del src\stddata.h
sed -e "s/vuSymb/pipeASymb/g" -f src\common.sed src\stddata.i > src\stddata.h
