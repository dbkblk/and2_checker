#Include, settings.ahk

; Routine
SetWorkingDir, ..
RunWait, %comspec% /c docker run -v "%A_WorkingDir%\translations":/app/translations/ -it rand_launcher_translations && timeout 5
FileMove, %A_WorkingDir%\translations\*.qm, %mod_dir%\checker\lang, 1