@REM This batch file has been generated by the IAR Embedded Workbench
@REM C-SPY Debugger, as an aid to preparing a command line for running
@REM the cspybat command line utility using the appropriate settings.
@REM
@REM You can launch cspybat by typing the name of this batch file followed
@REM by the name of the debug file (usually an ELF/DWARF or UBROF file).
@REM Note that this file is generated every time a new debug session
@REM is initialized, so you may want to move or rename the file before
@REM making changes.
@REM 


"D:\program files\EWARM\common\bin\cspybat" "D:\program files\EWARM\arm\bin\armproc.dll" "D:\program files\EWARM\arm\bin\armjlink.dll"  %1 --plugin "D:\program files\EWARM\arm\bin\armbat.dll" --backend -B "--endian=little" "--cpu=Cortex-M3" "--fpu=None" "-p" "D:\program files\EWARM\arm\CONFIG\debugger\ST\iostm32f10xxE.ddf" "--semihosting" "--device=STM32F10xxC" "--drv_communication=USB0" "--jlink_speed=auto" "--jlink_initial_speed=32" "--jlink_reset_strategy=0,0" "--drv_catch_exceptions=0x000" 


