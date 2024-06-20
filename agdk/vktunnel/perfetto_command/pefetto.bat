
adb root
adb push temp_perfetto_config.txt /data/local/tmp/trace_config.txt
del .temp_perfetto_config.txt
adb shell "cat /data/local/tmp/trace_config.txt | perfetto --txt -c - -o /data/misc/perfetto-traces/trace -d" > nul
echo Trace Started. Press any key to stop tracing...
pause > nul
echo Done. Pulling trace
adb shell killall perfetto
timeout /t 7
adb pull /data/misc/perfetto-traces/trace %USERPROFILE%\Downloads\trace