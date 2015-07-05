@echo off

echo Splicing eternity defaults into Language file %1.lng

bin\qcnftool.exe /v bin\defaults\eternity.lng custom\%1.lng /o custom\%1.lng /w