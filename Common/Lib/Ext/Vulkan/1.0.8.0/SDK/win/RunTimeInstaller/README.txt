The file in this folder, VulkanRT-<version>-Installer.exe, is the installer
for the Vulkan Run Time Libaries.

A Vulkan Application Installer or Vulkan Driver Installer can include this
file in its install package, and run the Vulkan RT installer during the
app/driver install to install the Vulkan Run Time Libraries.

Some notes on the behavior of the Windows Vulkan Runtime Installer:

   o  When VulkanRT-<version>-Installer.exe is run on a 64-bit version
      of Windows, it will install both the 32 and 64 bit versions of
      the Vulkan runtime.  When it is run on a 32-bit version of
      Windows, it will install the 32 bit version of the Vulkan runtime.

   o  The VulkanRT-<version>-Installer.exe can be run in silent mode by
      using the /S command line option when invoking the installer:

           VulkanRT-<version>-Installer.exe /S

   o  If the Vulkan Runtime is already installed on the system,
      subsequent installs of the same version of the Vulkan Runtime
      will be installed to the same folder to which it is was
      previously installed.

   o  The Vulkan Runtime Installer uses "counted" installs. If the
      same version of the Vulkan Runtime is installed multiple times
      on a system, the Vulkan Runtime must be uninstalled the same
      number of times before it is completely removed from the system.

   o  If the Vulkan Runtime is already installed on a system and a
      different version is subsequently installed, both versions will
      then co-exist on the system. The Vulkan Runtime Installer does
      not remove prior versions of the Vulkan Runtime when a newer
      version is installed.

  o  The Vulkan Runtime can be uninstalled from Control Panel ->
     Programs and Features. It can also be uninstalled by running
     UninstallVulkanRT.exe in the install folder. The uninstall can
     be run in silent mode by using the /S command line option
     when invoking the uninstaller:

          UninstallVulkanRT.exe /S

      The location of the the UninstallVulkanRT.exe can be found
      in the registry value HKLM\Software\Microsoft\Windows\
      CurrentVersion\Uninstall\VulkanRT<version>\UninstallString.


   o  The Vulkan Runtime Installer installs the Vulkan loader as
      C:\Windows\System32\vulkan-<version>.dll. It then compares all
      versions of the loader in C:\Windows\System32 that have the
      same VERSION_ABI_MAJOR as the version being installed. It selects
      the most recent one of these loader files and copies it to
      C:\Windows\System32\vulkan-<VERSION_ABI_MAJOR>.dll.  For example,
      during the install of Vulkan Runtime version 2.0.1.1, the
      following files might be present in C:\Windows\System32:

           vulkan-1-1-0-2-3.dll
           vulkan-1-2-0-1-0.dll
           vulkan-1-2-0-1-1.dll

     [Note that the first "1" in the above files is VERSION_ABI_MAJOR.
     The other numbers identify the release version.] The installer
     will copy the most recent one of these files (in this case
     vulkan-1-2-0-1-1.dll) to vulkan-1.dll. This is repeated for
     C:\Windows\SYSWOW64 on 64-bit Windows systems to set up the
     32-bit loader.

   o The Vulkan Runtime Installer returns an exit code of 0-9
     to indicate success. All other exit codes indicate failure.
     If the Installer returns a failure exit code, the Installer
     will have attempted to uninstall whatever it installed before
     it detected an error.

   o The Vulkan Runtime Uninstaller returns an exit code of 0-9
     to indicate success. An exit code of 3 indicates success, but
     a reboot is required to complete the uninstall.  All other
     exit codes indicate failure.  If the Uninstaller returns a
     failure exit code, it will have simply exited when the failure
     was detected and will not have attempted to do further uninstall
     work.

   o The ProductVersion of the installer executable (right click on
     the executable, Properties, then the Details tab) can be used
     by other installers/uninstallers to find the path to the
     uninstaller for the Vulkan Runtime in the Windows registry.
     This ProductVersion should always be identical to <version> in:

       HKLM\Software\Microsoft\Windows\CurrentVersion\Uininstall\VulkanRT<version>\UinstalString
