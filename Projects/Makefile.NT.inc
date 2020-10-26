PLATFORM=Win32

OBJDIR=$(PROJECT_ROOT)\build\$(PROJECT_TARGET)\$(PLATFORM)\$(BUILD_FLAVOR)
OUTDIR=$(PROJECT_ROOT)\out\$(PROJECT_TARGET)\$(PLATFORM)\$(BUILD_FLAVOR)


!if "$(BUILD_FLAVOR)" == "debug"
BUILDFLAVOR_EXTENSION=_d 
CFLAGS=/Zi /EHsc /Od /D_DEBUG /DDEBUG /D_WIN32 /MTd /W3 /WX /fp:fast /Fd$(OBJDIR)\$(PROJECT_TARGET).pdb
LIBFLAGS=
!else
BUILDFLAVOR_EXTENSION=
CFLAGS=/Ox /MT /W3 /WX /fp:fast /D_WIN32 /Fd$(OBJDIR)\$(PROJECT_TARGET).pdb
LINKFLAGS=
!endif

!if "$(PROJECT_ROOT)" == ""
!error PROJECT_ROOT needs to be defined.
!endif

!if "$(BUILD_FLAVOR)" == ""
!error BUILD_FLAVOR needs to be defined.
!endif


all: dirs projectall

dirs:
        @!if not EXIST "$(PROJECT_ROOT)\build" mkdir "$(PROJECT_ROOT)\build"
        @!if not EXIST "$(PROJECT_ROOT)\build" mkdir "$(PROJECT_ROOT)\build"
        @!if not EXIST "$(PROJECT_ROOT)\build\$(PROJECT_TARGET)" mkdir "$(PROJECT_ROOT)\build\$(PROJECT_TARGET)"
        @!if not EXIST "$(PROJECT_ROOT)\build\$(PROJECT_TARGET)\$(PLATFORM)" mkdir "$(PROJECT_ROOT)\build\$(PROJECT_TARGET)\$(PLATFORM)"
        @!if not EXIST "$(PROJECT_ROOT)\build\$(PROJECT_TARGET)\$(PLATFORM)\$(BUILD_FLAVOR)" mkdir "$(PROJECT_ROOT)\build\$(PROJECT_TARGET)\$(PLATFORM)\$(BUILD_FLAVOR)"       
        @!if not EXIST "$(PROJECT_ROOT)\out" mkdir "$(PROJECT_ROOT)\out"
        @!if not EXIST "$(PROJECT_ROOT)\out\$(PROJECT_TARGET)" mkdir "$(PROJECT_ROOT)\out\$(PROJECT_TARGET)"
        @!if not EXIST "$(PROJECT_ROOT)\out\$(PROJECT_TARGET)\$(PLATFORM)" mkdir "$(PROJECT_ROOT)\out\$(PROJECT_TARGET)\$(PLATFORM)"
        @!if not EXIST "$(PROJECT_ROOT)\out\$(PROJECT_TARGET)\$(PLATFORM)\$(BUILD_FLAVOR)" mkdir "$(PROJECT_ROOT)\out\$(PROJECT_TARGET)\$(PLATFORM)\$(BUILD_FLAVOR)"

clean:
	-rmdir /s /Q $(PROJECT_ROOT)\build\$(PROJECT_TARGET)\$(PLATFORM)
	-rmdir /s /Q $(PROJECT_ROOT)\out\$(PROJECT_TARGET)\$(PLATFORM)

{..\src}.cpp{$(OBJDIR)}.obj:
	cl /c $(CFLAGS) $< /Fo$@

{$(OBJDIR)}.cpp{$(OBJDIR)}.obj:
	cl /c $(CFLAGS) $< /Fo$@                                                              
