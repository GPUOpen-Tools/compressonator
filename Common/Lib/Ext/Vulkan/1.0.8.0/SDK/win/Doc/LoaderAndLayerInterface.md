# Vulkan Loader Specification and Architecture Overview


Goals of this document
----------------------

Specify necessary functions and expected behavior of interface between the
loader library and ICDs and layers for Windows, Linux and Android based
systems. Also describe the application visible behaviors of the loader.

Audience
--------

Application, Vulkan driver and Vulkan layer developers.

Any developers interested in understanding more about loader and layer behavior
and architecture.


Loader goals
------------

-   Support multiple ICDs (Installable Client Drivers) to co-exist on a system
without interfering with each other.

-   Support optional modules (layers) that can be enabled by an application,
developer or the system and have no impact when not enabled.

-   Negligible performance cost for an application calling through the loader
to an ICD entry point.

Architectural overview of layers and loader
-------------------------------------------

Vulkan is a layered architecture. Layers can hook (intercept) Vulkan commands to
achieve various functionality that a Vulkan driver (aka ICD) or loader doesn’t
support. Functionality such as Vulkan API tracing and debugging, API usage
validation, and other tools such as framebuffer overlays are all natural
candidates for Vulkan layers. Layers are implemented as libraries that are
inserted between the application and the driver.

Not only is Vulkan a layered architecture but it also supports multiple GPUs
and their drivers. Vulkan commands called by an application may wind up calling
into a diverse set of modules: loader, layers, and ICDs. The loader is critical
to managing the proper dispatching of Vulkan commands to the appropriate set of
layers and ICDs. The Vulkan object model allows the loader to insert layers
into a call chain so the layers can process Vulkan commands prior to the
ICD being called.

Vulkan uses an object model to control the scope of a particular action /
operation.  The object to be acted on is always the first parameter of a Vulkan
call and is a dispatchable object (see Vulkan specification section 2.2 Object
Model).  Under the covers, the dispatchable object handle is a pointer to a
structure that contains a pointer to a dispatch table maintained by the loader.
This dispatch table contains pointers to the Vulkan functions appropriate to
that object. There are two types of dispatch tables the loader maintains,
Instance and Device. I.e. a VkInstance object’s dispatch table will point to Vulkan
functions such as vkEnumeratePhysicalDevices, vkDestroyInstance,
vkCreateInstance, etc. Instance functions take a VkInstance or VkPhysicalDevice as
their first argument.

Device objects have a separate dispatch table containing the appropriate
function pointers. The device dispatch table is used for all functions that
take a VkDevice, VkQueue or VkCommandBuffer as their first argument.

These instance and device dispatch tables are constructed when the application
calls vkCreateInstance and vkCreateDevice. At that time the application and/or
system can specify optional layers to be included. The loader will initialize
the specified layers to create a call chain for each Vulkan function and each
entry of the dispatch table will point to the first element of that chain.
Thus, the loader builds an instance call chain for each VkInstance that is
created and a device call chain for each VkDevice that is created.

For example, the diagram below represents what happens in the call chain for
vkCreateInstance. After initializing the chain, the loader will call into the
first layer’s vkCreateInstance which will call the next finally terminating in
the loader again where this function calls every ICD’s vkCreateInstance and
saves the results. This allows every enabled layer for this chain to set up
what it needs based on the VkInstanceCreateInfo structure from the application.
![Instance call chain](instance_call_chain.png)

This also highlights some of the complexity the loader must manage when using
instance chains. As shown here, the loader must aggregate information from
multiple devices when they are present. This means that the loader has to know
about instance level extensions to aggregate them correctly.

Device chains are created at vkCreateDevice and are generally simpler because
they deal with only a single device and the ICD can always be the terminator of
the chain. The below diagram also illustrates how layers (either device or
instance) can skip intercepting any given Vulkan entry point.
![Chain skipping layers](chain_skipping_layers.png)

Application interface to loader
-------------------------------

In this section we’ll discuss how an application interacts with the loader.

-   Linking to loader library for core and WSI extension symbols.

-   Dynamic Vulkan command lookup & application dispatch table.

-   Loader library filenames for linking to different Vulkan ABI versions.

-   Layers

-   Extensions

-   vkGetInstanceProcAddr, vkGetDeviceProcAddr

The loader library on Windows, Linux and Android will export all core Vulkan
and all appropriate Window System Interface (WSI) extensions. This is done to
make it simpler to get started with Vulkan development. When an application
links directly to the loader library in this way, the Vulkan calls are simple
trampoline functions that jump to the appropriate dispatch table entry for the
object they are given.

Applications are not required to link directly to the loader library, instead
they can use the appropriate platform specific dynamic symbol lookup on the
loader library to initialize the application’s own dispatch table. This allows
an application to fail gracefully if the loader cannot be found, and it
provides the fastest mechanism for the application to call Vulkan functions. An
application will only need to query (via system calls such as dlsym()) the
address of vkGetInstanceProcAddr from the loader library. Using
vkGetInstanceProcAddr the application can then discover the address of all
instance and global functions and extensions, such as vkCreateInstance,
vkEnumerateInstanceExtensionProperties and vkEnumerateInstanceLayerProperties
in a platform independent way.

The Vulkan loader library will be distributed in various ways including Vulkan
SDKs, OS package distributions and IHV driver packages. These details are
beyond the scope of this document. However, the name and versioning of the
Vulkan loader library is specified so an app can link to the correct Vulkan ABI
library version. Vulkan versioning is such that ABI backwards compatibility is
guaranteed for all versions with the same major number (e.g. 1.0 and 1.1). On
Windows, the loader library encodes the ABI version in its name such that
multiple ABI incompatible versions of the loader can peacefully coexist on a
given system. The Vulkan loader library file name is “vulkan-&lt;ABI
version&gt;.dll”. For example, for Vulkan version 1.X on Windows the library
filename is vulkan-1.dll. And this library file can typically be found in the
windows/system32 directory.

For Linux, shared libraries are versioned based on a suffix. Thus, the ABI
number is not encoded in the base of the library filename as on Windows. On
Linux an application wanting to link to the latest Vulkan ABI version would
just link to the name vulkan (libvulkan.so).  A specific Vulkan ABI version can
also be linked to by applications (e.g. libvulkan.so.1).

Applications desiring Vulkan functionality beyond what the core API offers may
use various layers or extensions. A layer cannot add new or modify existing
Vulkan commands, but may offer extensions that do. A common use of layers is
for API validation. A developer can use validation layers during application
development, but during production the layers can be disabled by the
application. Thus, eliminating the overhead of validating the application's
usage of the API. Layers discovered by the loader can be reported to the
application via vkEnumerateInstanceLayerProperties and
vkEnumerateDeviceLayerProperties, for instance and device layers respectively.
Instance layers are enabled at vkCreateInstance; device layers are enabled at
vkCreateDevice. For example, the ppEnabledLayerNames array in the
VkDeviceCreateInfo structure is used by the application to list the device
layer names to be enabled at vkCreateDevice. At vkCreateInstance and
vkCreateDevice, the loader will construct call chains that include the
application specified (enabled) layers. Order is important in the
ppEnabledLayerNames array; array element 0 is the topmost (closest to the
application) layer inserted in the chain and the last array element is closest
to the driver.

Developers may want to enable layers that are not enabled by the given
application they are using. On Linux and Windows, the environment variables
“VK\_INSTANCE\_LAYERS” and “VK\_DEVICE\_LAYERS” can be used to enable
additional layers which are not specified (enabled) by the application at
vkCreateInstance/vkCreateDevice. VK\_INSTANCE\_LAYERS is a colon
(Linux)/semi-colon (Windows) separated list of layer names to enable. Order is
relevant with the first layer in the list being the topmost layer (closest to
the application) and the last layer in the list being the bottommost layer
(closest to the driver).

Application specified layers and user specified layers (via environment
variables) are aggregated and duplicates removed by the loader when enabling
layers. Layers specified via environment variable are topmost (closest to the
application) while layers specified by the application are bottommost.

An example of using these environment variables to activate the validation
layer VK\_LAYER\_LUNARG\_param\_checker on Windows or Linux is as follows:

```
> $ export VK_INSTANCE_LAYERS=VK_LAYER_LUNARG_parameter_validation

> $ export VK_DEVICE_LAYERS=VK_LAYER_LUNARG_parameter_validation
```

**Note**: Many layers, including all LunarG validation layers are “global”
(i.e. both instance and device) layers and *must* be enabled on both the
instance and device chains to function properly. This is required for “global”
layers regardless of which method is used to enable the layer (application or
environment variable).

Some platforms, including Linux and Windows, support layers which are enabled
automatically by the loader rather than explicitly by the application (or via
environment variable). Explicit layers are those layers enabled by the
application (or environment variable) by providing the layer name. Implicit
layers are those layers enabled by the loader automatically. Any implicit
layers the loader discovers on the system in the appropriate location will be
enabled (subject to environment variable overrides described later). Discovery
of properly installed implicit and explicit layers is described later.
Explicitly enabling a layer that is implicitly enabled has no additional
effect: the layer will still be enabled implicitly by the loader.

Extensions are optional functionality provided by a layer, the loader or an
ICD. Extensions can modify the behavior of the Vulkan API and need to be
specified and registered with Khronos.

Instance extensions can be discovered via
vkEnumerateInstanceExtensionProperties. Device extensions can be discovered via
vkEnumerateDeviceExtensionProperties. The loader discovers and aggregates all
extensions from layers (both explicit and implicit), ICDs and the loader before
reporting them to the application in vkEnumerate\*ExtensionProperties. The
pLayerName parameter in these functions is used to select either a single layer
or the Vulkan platform implementation. If pLayerName is NULL, extensions from
Vulkan implementation components (including loader, implicit layers, and ICDs)
are enumerated. If pLayerName is equal to a discovered layer module name then
any extensions from that layer (which may be implicit or explicit) are
enumerated. Duplicate extensions (e.g. an implicit layer and ICD might report
support for the same extension) are eliminated by the loader. For duplicates, the
ICD version is reported and the layer version is culled. Extensions must
be enabled (in vkCreateInstance or vkCreateDevice) before they can be used.

Extension command entry points should be queried via vkGetInstanceProcAddr or
vkGetDeviceProcAddr. vkGetDeviceProcAddr can only be used to query for device
extension or core device entry points. Device entry points include any command
that uses a VkDevice as the first parameter or a dispatchable object that is a
child of a VkDevice (currently this includes VkQueue and VkCommandBuffer).
vkGetInstanceProcAddr can be used to query either device or instance extension
entry points in addition to all core entry points.

VkGetDeviceProcAddr is particularly interesting because it will provide the
most efficient way to call into the ICD. For example, the diagram below shows
what could happen if the application were to use vkGetDeviceProcAddr for the
function “vkGetDeviceQueue” and “vkDestroyDevice” but not “vkAllocateMemory”.
The resulting function pointer (fpGetDeviceQueue) would be the ICD’s entry
point if the loader and any enabled layers do not need to see that call. Even
if an enabled layer intercepts the call (e.g. vkDestroyDevice) the loader
trampoline code is skipped for function pointers obtained via
vkGetDeviceProcAddr. This also means that function pointers obtained via
vkGetDeviceProcAddr will only work with the specific VkDevice it was created
for, using it with another device has undefined results. For extensions,
Get\*ProcAddr will often be the only way to access extension API features.

![Get*ProcAddr efficiency](get_proc_addr.png)


Vulkan Installable Client Driver interface with the loader
----------------------------------------------------------

### ICD discovery

Vulkan allows multiple drivers each with one or more devices (represented by a
Vulkan VkPhysicalDevice object) to be used collectively. The loader is
responsible for discovering available Vulkan ICDs on the system. Given a list
of available ICDs, the loader can enumerate all the physical devices available
for an application and return this information to the application. The process
in which the loader discovers the available Installable Client Drivers (ICDs)
on a system is platform dependent. Windows, Linux and Android ICD discovery
details are listed below.

#### Windows

##### Properly-Installed ICDs

In order to find properly-installed ICDs, the Vulkan loader will scan the
values in the following Windows registry key:

HKEY\_LOCAL\_MACHINE\\SOFTWARE\\Khronos\\Vulkan\\Drivers

For each value in this key which has DWORD data set to 0, the loader opens the
JSON format text information file (a.k.a. "manifest file") specified by the
name of the value. Each name must be a full pathname to the text manifest file.
The Vulkan loader will open each manifest file to obtain the name or pathname
of an ICD shared library (".dll") file. For example:

 ```
 {
    "file_format_version": "1.0.0",
    "ICD": {
        "library_path": "path to ICD library",
        "api_version": "1.0.5"
    }
  }
  ```


The "library\_path" specifies either a filename, a relative pathname, or a full
pathname to an ICD shared library file, which the loader will attempt to load
using LoadLibrary(). If the ICD is specified via a filename, the shared library
lives in the system's DLL search path (e.g. in the "C:\\\\Windows\\\\System32"
folder). If the ICD is specified via a relative pathname, it is relative to the
path of the manifest file. Relative pathnames are those that do not start with
a drive specifier (e.g. "C:"), nor with a directory separator (i.e. the '\\'
character), but do contain at least one directory separator.

The "file\_format\_version" specifies a major.minor.patch version number in
case the format of the text information file changes in the future. If the same
ICD shared library supports multiple, incompatible versions of text manifest
file format versions, it must have multiple text info files (all of which may
point to the same shared library).

The “api\_version” specifies the major.minor.patch version number of the Vulkan
API that the shared library (referenced by "library\_path") was built with.

There are no rules about the name of the text information files (except the
.json suffix).

There are no rules about the name of the ICD shared library files. For example,
if the registry contains the following values,

```
[HKEY_LOCAL_MACHINE\SOFTWARE\Khronos\Vulkan\Drivers\]

"C:\vendor a\vk\_vendora.json"=dword:00000000

"C:\windows\system32\vendorb\_vk.json"=dword:00000000

"C:\windows\system32\vendorc\_icd.json"=dword:00000000
```
then the loader will open the following text information files, with the
specified contents:

| Text File Name | Text File Contents |
|----------------|--------------------|
|vk\_vendora.json  | "ICD": { "library\_path": "C:\\\\VENDORA\\\\vk\_vendora.dll", "api_version": "1.0.5" } |
| vendorb\_vk.json |  "ICD": { "library\_path": "vendorb\_vk.dll", "api_version": "1.0.5" } |
|vendorc\_icd.json  | "ICD": { "library\_path": "vedorc\_icd.dll", "api_version": "1.0.5" }|

Then the loader will open the three files mentioned in the "Text File Contents"
column, and then try to load and use the three shared libraries indicated by
the ICD.library\_path value.

##### Using Pre-Production ICDs

IHV developers (and sometimes other developers) need to use special,
pre-production ICDs. In some cases, a pre-production ICD may be in an
installable package. In other cases, a pre-production ICD may simply be a
shared library in the developer's build tree. In this latter case, we want to
allow developers to point to such an ICD without modifying the
properly-installed ICD(s) on their system.

This need is met with the use of the "VK\_ICD\_FILENAMES" environment variable,
which will override the mechanism used for finding properly-installed ICDs. In
other words, only the ICDs listed in "VK\_ICD\_FILENAMES" will be used. The
"VK\_ICD\_FILENAMES" environment variable is a semi-colon-separated list of ICD
text information files (aka manifest files), containing the following:

- A full pathname (e.g. "C:\\my\_build\\my\_icd.json")

Typically, "VK\_ICD\_FILENAMES" will only contain a full pathname to one info
file for a developer-built ICD. A semi-colon is only used if more than one ICD
is listed.

For example, if a developer wants to refer to one ICD that they built, they
could set the "VK\_ICD\_FILENAMES" environment variable to:

C:\\my\_build\\my\_icd.json

If a developer wants to refer to two ICDs, one of which is a properly-installed
ICD, they can use the full pathname of the text file:

C:\\Windows\\System32\\vendorc\_icd.json;C:\\my\_build\\my\_icd.json

Notice the semi-colon between "C:\\Windows\\System32\\vendorc\_icd.json" and
"C:\\my\_build\\my\_icd.json".

#### Linux

##### Properly-Installed ICDs

In order to find properly-installed ICDs, the Vulkan loader will scan the files
in the following Linux directories:

/usr/share/vulkan/icd.d
/etc/vulkan/icd.d
$HOME/.local/share/vulkan/icd.d

Where $HOME is the current home directory of the application's user id; this
path will be ignored for suid programs.

These directories will contain text information files (a.k.a. "manifest
files"), that use a JSON format.

The Vulkan loader will open each manifest file found to obtain the name or
pathname of an ICD shared library (".so") file. For example:

```
{
    "file_format_version": "1.0.0",
    "ICD": {
        "library_path": "path to ICD library",
        "api_version": "1.0.5"
    }
}
```
The "library\_path" specifies either a filename, a relative pathname, or a full
pathname to an ICD shared library file. If the ICD is specified via a filename,
the loader will attempt to open that file as a shared object using dlopen(),
and the file must be in a directory that dlopen is configured to look in (Note:
various distributions are configured differently). A distribution is free to
create Vulkan-specific system directories (e.g. ".../vulkan/icd"), but is not
required to do so. If the ICD is specified via a relative pathname, it is
relative to the path of the info file. Relative pathnames are those that do not
start with, but do contain at least one directory separator (i.e. the '/'
character). For example, "lib/vendora.so" and "./vendora.so" are examples of
relative pathnames.

The "file\_format\_version" provides a major.minor.patch version number in case
the format of the manifest file changes in the future. If the same ICD shared
library supports multiple, incompatible versions of manifest file format
versions, it must have multiple manifest files (all of which may point to the
same shared library).

The “api\_version” specifies the major.minor.patch version number of the Vulkan
API that the shared library (referenced by "library\_path") was built with.

The "/usr/share/vulkan/icd.d" directory is for ICDs that are installed from
Linux-distribution-provided packages. The "/etc/vulkan/icd.d" directory is for
ICDs that are installed from non-Linux-distribution-provided packages.

There are no rules about the name of the text files (except the .json suffix).

There are no rules about the name of the ICD shared library files. For example,
if the "/usr/share/vulkan/icd.d" directory contain the following files, with
the specified contents:

| Text File Name    | Text File Contents     |
|-------------------|------------------------|
| vk\_vendora.json | "ICD": { "library\_path": "vendora.so", "api_version": "1.0.5" } |
| vendorb\_vk.json | "ICD": { "library\_path": "vendorb\_vulkan\_icd.so", "api_version": "1.0.5" } |
| vendorc\_icd.json | "ICD": { "library\_path": "/usr/lib/VENDORC/icd.so", "api_version": "1.0.5" } |

then the loader will open the three files mentioned in the "Text File Contents"
column, and then try to load and use the three shared libraries indicated by
the ICD.library\_path value.

##### Using Pre-Production ICDs

IHV developers (and sometimes other developers) need to use special,
pre-production ICDs. In some cases, a pre-production ICD may be in an
installable package. In other cases, a pre-production ICD may simply be a
shared library in the developer's build tree. In this latter case, we want to
allow developers to point to such an ICD without modifying the
properly-installed ICD(s) on their system.

This need is met with the use of the "VK\_ICD\_FILENAMES" environment variable,
which will override the mechanism used for finding properly-installed ICDs. In
other words, only the ICDs listed in "VK\_ICD\_FILENAMES" will be used.

The "VK\_ICD\_FILENAMES" environment variable is a colon-separated list of ICD
manifest files, containing the following:

- A filename (e.g. "libvkicd.json") in the "/usr/share/vulkan/icd.d", "/etc/vulkan/icd.d" "$HOME/.local/share/vulkan/icd.d" directories

- A full pathname (e.g. "/my\_build/my\_icd.json")

Typically, "VK\_ICD\_FILENAMES" will only contain a full pathname to one info
file for a developer-built ICD. A colon is only used if more than one ICD is
listed.

For example, if a developer wants to refer to one ICD that they built, they
could set the "VK\_ICD\_FILENAMES" environment variable to:

/my\_build/my\_icd.json

If a developer wants to refer to two ICDs, one of which is a properly-installed
ICD, they can use the name of the text file in the system directory:

vendorc\_vulkan.json:/my\_build/my\_icd.json

Notice the colon between "vendorc\_vulkan.json" and "/my\_build/my\_icd.json".

NOTE: this environment variable will be ignored for suid programs.

#### Android

The Android loader lives in the system library folder. The location cannot be
changed. The loader will load the driver/ICD via hw_get_module with the ID
of "vulkan". Due to security policies in Android none of this can be modified
under normal use.


ICD interface requirements
----------------------------------------

Generally, for all Vulkan commands issued by an application, the loader can be
viewed as a pass through. That is, the loader generally doesn’t modified the
commands or their parameters but simply calls the ICDs entry point for that
command. Thus, the loader to ICD interface requirements will be specified by
covering two areas: 1) Obtaining ICD Vulkan entry points; 2) Specifying
requirements for a given Vulkan command(s) over and above the Vulkan
specification requirements.

#### Windows and Linux

##### Obtaining ICD entry points

Currently, two methods of the loader finding ICD entry points are supported on
Linux and Windows:

1) Recommended

- vk\_icdGetInstanceProcAddr is exported by the ICD library and it returns
  valid function pointers for all the global level and instance level Vulkan
  commands, and also for vkGetDeviceProcAddr. Global level commands are those
  which contain no dispatchable object as the first parameter, such as
  vkCreateInstance and vkEnumerateInstanceExtensionProperties. The ICD must
  support querying global level entry points by calling
  vk\_icdGetInstanceProcAddr with a NULL VkInstance parameter. Instance level
  commands are those that have either VkInstance, or VkPhysicalDevice as the
  first parameter dispatchable object. Both core entry points and any instance
  extension entry points the ICD supports should be available via
  vk\_icdGetInstanceProcAddr. Future Vulkan instance extensions may define and
  use new instance level dispatchable objects other than VkInstance and
  VkPhysicalDevice, in which case extension entry points using these newly
  defined dispatchable objects must be queryable via
  vk\_icdGetInstanceProcAddr.

- All other Vulkan entry points must either NOT be exported from the ICD
  library or else NOT use the official Vulkan function names if they are
  exported. This requirement is for ICD libraries that include other
  functionality (such as OpenGL library) and thus could be loaded by the
  application prior to when the Vulkan loader library is loaded by the
  application. In other words, the ICD library exported Vulkan symbols must not
  clash with the loader's exported Vulkan symbols.

- Beware of interposing by dynamic OS library loaders if the official Vulkan
  names are used. On Linux, if official names are used, the ICD library must be
  linked with -Bsymbolic.

2) Deprecated

- vkGetInstanceProcAddr exported in the ICD library and returns valid function
  pointers for all the Vulkan API entry points.

- vkCreateInstance exported in the ICD library;

- vkEnumerateInstanceExtensionProperties exported in the ICD library;

##### Loader specific requirements for Vulkan commands

Normally, ICDs handle object creation and destruction for various Vulkan
objects. The WSI surface extensions for Linux and Windows
(VK\_KHR\_win32\_surface, VK\_KHR\_xcb\_surface, VK\_KHR\_xlib\_surface,
VK\_KHR\_mir\_surface, VK\_KHR\_wayland\_surface, and VK\_KHR\_surface) are
handled differently. For these extensions, the VkSurfaceKHR object creation and
destruction is handled by the loader as follows:

1. Loader handles the vkCreate\*SurfaceKHR() and vkDestroySurfaceKHR()
   functions including creating/destroying the VkSurfaceKHR object.

2. VkSurfaceKHR objects have the underlying structure (VkIcdSurface\*) as
   defined in include/vulkan/vk\_icd.h.

3. ICDs can cast any VkSurfaceKHR object to a pointer to the appropriate
   VkIcdSurface\* structure.

4. VkIcdSurface\* structures include VkIcdSurfaceWin32, VkIcdSurfaceXcb,
   VkIcdSurfaceXlib, VkIcdSurfaceMir, and VkIcdSurfaceWayland. The first field
   in the structure is a VkIcdSurfaceBase enumerant that indicates whether the
   surface object is Win32, Xcb, Xlib, Mir, or Wayland.

As previously covered, the loader requires dispatch tables to be accessible
within Vulkan dispatchable objects, which include VkInstance, VkPhysicalDevice,
VkDevice, VkQueue, and VkCommandBuffer. The specific requirements on all
dispatchable objects created by ICDs are as follows:

- All dispatchable objects created by an ICD can be cast to void \*\*

- The loader will replace the first entry with a pointer to the dispatch table
  which is owned by the loader. This implies three things for ICD drivers:

1. The ICD must return a pointer for the opaque dispatchable object handle.

2. This pointer points to a regular C structure with the first entry being a
   pointer. Note: for any C\++ ICD's that implement VK objects directly as C\++
   classes. The C\++ compiler may put a vtable at offset zero if your class is
   non-POD due to the use of a virtual function. In this case use a regular C
   structure (see below).

3. The loader checks for a magic value (ICD\_LOADER\_MAGIC) in all the created
   dispatchable objects, as follows (see include/vulkan/vk\_icd.h):

```

#include "vk_icd.h"

union _VK_LOADER_DATA {
    uintptr loadermagic;
    void *loaderData;
} VK_LOADER_DATA;

vkObj alloc_icd_obj()
{
    vkObj *newObj = alloc_obj();
    ...
    // Initialize pointer to loader's dispatch table with ICD_LOADER_MAGIC

    set_loader_magic_value(newObj);
    ...
    return newObj;
}
```

Additional Notes:

- The loader will filter out extensions requested in vkCreateInstance and
vkCreateDevice before calling into the ICD; Filtering will be of extensions
advertised by entities (e.g. layers) different from the ICD in question.
- The loader will not call the ICD for vkEnumerate\*LayerProperties() as layer
properties are obtained from the layer libraries and layer JSON files.
- If an ICD library wants to implement a layer it can do so by having the
appropriate layer JSON manifest file refer to the ICD library file.
- The loader will not call the ICD for
  vkEnumerate\*ExtensionProperties(pLayerName != NULL).

#### Android

The Android loader uses the same protocol for initializing the dispatch
table as described above. The only difference is that the Android
loader queries layer and extension information directly from the
respective libraries and does not use the json manifest files used
by the Windows and Linux loaders.

Vulkan layer interface with the loader
--------------------------------------

### Layer discovery

#### Windows

##### Properly-Installed Layers

In order to find properly-installed layers, the Vulkan loader will use a
similar mechanism as used for ICDs. Text information files (aka manifest
files), that use a JSON format, are read in order to identify the names and
attributes of layers and their extensions. The use of manifest files allows the
loader to avoid loading any shared library files when the application does not
query nor request any extensions. Layers and extensions have additional
complexity, and so their manifest files contain more information than ICD info
files. For example, a layer shared library file may contain multiple
layers/extensions (perhaps even an ICD).

In order to find properly-installed layers, the Vulkan loader will scan the
values in the following Windows registry keys:

HKEY\_LOCAL\_MACHINE\\SOFTWARE\\Khronos\\Vulkan\\ExplicitLayers

HKEY\_LOCAL\_MACHINE\\SOFTWARE\\Khronos\\Vulkan\\ImplicitLayers

Explicit layers are those which are enabled by an application (e.g. with the
vkCreateInstance function), or by an environment variable (as mentioned
previously).

Implicit layers are those which are enabled by their existence. For example,
certain application environments (e.g. Steam or an automotive infotainment
system) may have layers which they always want enabled for all applications
that they start. Other implicit layers may be for all applications started on a
given system (e.g. layers that overlay frames-per-second). Implicit layers are
enabled automatically, whereas explicit layers must be enabled explicitly. What
distinguishes a layer as implicit or explicit is by which registry key its
layer information file is referenced by.

For each value in these keys which has DWORD data set to 0, the loader opens
the JSON manifest file specified by the name of the value. Each name must be a
full pathname to the manifest file.

The Vulkan loader will open each info file to obtain information about the
layer, including the name or pathname of a shared library (".dll") file.

This manifest file is in the JSON format and contains the following
information:

- (required) "file\_format\_version" - same as for ICDs, except that the format
version can vary independently for ICDs and layers.

- (required) "name" - layer name

- (required) "type" - which layer chains should the layer be activated on.
Allowable values are "INSTANCE", "DEVICE", "GLOBAL". Global means activate on
both device and instance chains.

- (required) "library\_path" - filename / full path / relative path to the
library file

- (required) "api\_version" - same as for ICDs.

- (required) "implementation\_version" - layer version, a single number
increasing with backward compatible changes.

- (required) "description" - informative description of the layer.

- (optional) "device\_extensions" or "instance\_extensions" - array of
extension information as follows

    - (required) extension "name" - Vulkan registered name

    - (required) extension "spec\_version" - extension specification version, a
single number, increasing with backward compatible changes.

    - (required for device\_extensions with entry points) extension
"entrypoints" - array of device extension entry points; not used for instance
extensions

- (sometimes required) "functions" - mapping list of function entry points. If
multiple layers exist within the same shared library (or if a layer is in the
same shared library as an ICD), this must be specified to allow each layer to
have its own vkGet\*ProcAddr entry points that can be found by the loader. At
this time, only the following two functions are required:

    - "vkGetInstanceProcAddr" name

    - "vkGetDeviceProcAddr" name

- (optional for implicit layers) "enable\_environment" requirement(s) -
environment variable and value required to enable an implicit layer. This
environment variable (which should vary with each "version" of the layer, as in
"ENABLE\_LAYER\_FOO\_1") must be set to the given value or else the implicit
layer is not loaded. This is for application environments (e.g. Steam) which
want to enable a layer(s) only for applications that they launch, and allows
for applications run outside of an application environment to not get that
implicit layer(s).

- (required for implicit layers) "disable\_environment" requirement(s) -
environment variable and value required to disable an implicit layer. Note: in
rare cases of an application not working with an implicit layer, the
application can set this environment variable (before calling Vulkan functions)
in order to "blacklist" the layer. This environment variable (which should vary
with each "version" of the layer, as in "DISABLE\_LAYER\_FOO\_1") must be set
(not particularly to any value). If both the "enable\_environment" and
"disable\_environment" variables are set, the implicit layer is disabled.

For example:

```
{
"file_format_version" : "1.0.0",
"layer": {
    "name": "VK_LAYER_LUNARG_OverlayLayer",
    "type": "DEVICE",
    "library_path": "vkOverlayLayer.dll"
    "api_version" : "1.0.5",
    "implementation_version" : "2",
    "description" : "LunarG HUD layer",
    "functions": {
        "vkGetInstanceProcAddr": "OverlayLayer_GetInstanceProcAddr",
        "vkGetDeviceProcAddr": "OverlayLayer_GetDeviceProcAddr"
    },
    "instance_extensions": [
        {
            "name": "VK_debug_report_EXT",
            "spec_version": "1"
        },
        {
            "name": "VK_VENDOR_DEBUG_X",
            "spec_version": "3"
         }
    ],
    "device_extensions": [
        {
            "name": "VK_DEBUG_MARKER_EXT",
            "spec_version": "1",
            "entrypoints": ["vkCmdDbgMarkerBegin", "vkCmdDbgMarkerEnd"]
        }
    ],
    "disable_environment": {
        "DISABLE_LAYER_OVERLAY_1": ""
    }
}
}
```

The "library\_path" specifies either a filename, a relative pathname, or a full
pathname to a layer shared library (".dll") file, which the loader will attempt
to load using LoadLibrary(). If the layer is specified via a relative pathname,
it is relative to the path of the info file (e.g. for cases when an application
provides a layer that is in the same folder hierarchy as the rest of the
application files). If the layer is specified via a filename, the shared
library lives in the system's DLL search path (e.g. in the
"C:\\Windows\\System32" folder).

There are no rules about the name of the text files (except the .json suffix).

There are no rules about the name of the layer shared library files.

##### Using Pre-Production Layers

As with ICDs, developers may need to use special, pre-production layers,
without modifying the properly-installed layers. This need is met with the use
of the "VK\_LAYER\_PATH" environment variable, which will override the
mechanism using for finding properly-installed layers. Because many layers may
exist on a system, this environment variable is a semi-colon-separated list of
folders that contain layer info files. Only the folder listed in
"VK\_LAYER\_PATH" will be scanned for info files. Each semi-colon-separated
entry is:

- The full pathname of a folder containing layer info files

#### Linux

##### Properly-Installed Layers

In order to find properly-installed layers, the Vulkan loader will use a
similar mechanism as used for ICDs. Text information files, that use a JSON
format, are read in order to identify the names and attributes of layers and
their extensions. The use of text info files allows the loader to avoid loading
any shared library files when the application does not query nor request any
extensions. Layers and extensions have additional complexity, and so their info
files contain more information than ICD info files. For example, a layer shared
library file may contain multiple layers/extensions (perhaps even an ICD).

The Vulkan loader will scan the files in the following Linux directories:

/usr/share/vulkan/explicit\_layer.d
/usr/share/vulkan/implicit\_layer.d
/etc/vulkan/explicit\_layer.d
/etc/vulkan/implicit\_layer.d
$HOME/.local/share/vulkan/explicit\_layer.d
$HOME/.local/share/vulkan/implicit\_layer.d

Where $HOME is the current home directory of the application's user id; this
path will be ignored for suid programs.

Explicit layers are those which are enabled by an application (e.g. with the
vkCreateInstance function), or by an environment variable (as mentioned
previously). Implicit layers are those which are enabled by their existence.
For example, certain application environments (e.g. Steam or an automotive
infotainment system) may have layers which they always want enabled for all
applications that they start. Other implicit layers may be for all applications
started on a given system (e.g. layers that overlay frames-per-second).
Implicit layers are enabled automatically, whereas explicit layers must be
enabled explicitly. What distinguishes a layer as implicit or explicit is by
which directory its layer information file exists in.

The "/usr/share/vulkan/\*\_layer.d" directories are for layers that are
installed from Linux-distribution-provided packages. The
"/etc/vulkan/\*\_layer.d" directories are for layers that are installed from
non-Linux-distribution-provided packages.

The information file is in the JSON format and contains the following
information:

- (required) "file\_format\_version" – same as for ICDs, except that the format
version can vary independently for ICDs and layers.

- (required) "name" - layer name

- (required) "type" - which layer chains should the layer be activated on.
Allowable values are "INSTANCE", "DEVICE", "GLOBAL". Global means activate on
both device and instance chains.

- (required) "library\_path" - filename / full path / relative path to the text
file

- (required) "api\_version" – same as for ICDs.

- (required) "implementation\_version" – layer version, a single number
increasing with backward compatible changes.

- (required) "description" – informative description of the layer.

- (optional) "device\_extensions" or "instance\_extensions" - array of
extension information as follows

    - (required) extension "name" - Vulkan registered name

    - (required) extension "spec\_version" - extension specification version, a
single number, increasing with backward compatible changes.

    - (required for device extensions with entry points) extension
"entrypoints" - array of device extension entry points; not used for instance
extensions

- (sometimes required) "functions" - mapping list of function entry points. If
multiple layers exist within the same shared library (or if a layer is in the
same shared library as an ICD), this must be specified to allow each layer to
have its own vkGet\*ProcAddr entry points that can be found by the loader. At
this time, only the following two functions are required:
    - "vkGetInstanceProcAddr" name
    - "vkGetDeviceProcAddr" name

- (optional for implicit layers) "enable\_environment" requirement(s) -
environment variable and value required to enable an implicit layer. This
environment variable (which should vary with each "version" of the layer, as in
"ENABLE\_LAYER\_FOO\_1") must be set to the given value or else the implicit
layer is not loaded. This is for application environments (e.g. Steam) which
want to enable a layer(s) only for applications that they launch, and allows
for applications run outside of an application environment to not get that
implicit layer(s).

- (required for implicit layers) "disable\_environment" requirement(s) -
environment variable and value required to disable an implicit layer. Note: in
rare cases of an application not working with an implicit layer, the
application can set this environment variable (before calling Vulkan functions)
in order to "blacklist" the layer. This environment variable (which should vary
with each "version" of the layer, as in "DISABLE\_LAYER\_FOO\_1") must be set
(not particularly to any value). If both the "enable\_environment" and
"disable\_environment" variables are set, the implicit layer is disabled.

For example:
```
{
"file_format_version" : "1.0.0",
"layer": {
    "name": "VK_LAYER_LUNARG_OverlayLayer",
    "type": "DEVICE",
    "library_path": "vkOverlayLayer.dll"
    "api_version" : "1.0.5",
    "implementation_version" : "2",
    "description" : "LunarG HUD layer",
    "functions": {
        "vkGetInstanceProcAddr": "OverlayLayer_GetInstanceProcAddr",
        "vkGetDeviceProcAddr": "OverlayLayer_GetDeviceProcAddr"
    },
    "instance_extensions": [
        {
            "name": "VK_debug_report_EXT",
            "spec_version": "1"
        },
        {
            "name": "VK_VENDOR_DEBUG_X",
            "spec_version": "3"
         }
    ],
    "device_extensions": [
        {
            "name": "VK_DEBUG_MARKER_EXT",
            "spec_version": "1",
            "entrypoints": ["vkCmdDbgMarkerBegin", "vkCmdDbgMarkerEnd"]
        }
    ],
    "disable_environment": {
        "DISABLE_LAYER_OVERLAY_1": ""
    }
}
}
```
The "library\_path" specifies either a filename, a relative pathname, or a full
pathname to a layer shared library (".so") file, which the loader will attempt
to load using dlopen(). If the layer is specified via a filename, the loader
will attempt to open that file as a shared object using dlopen(), and the file
must be in a directory that dlopen is configured to look in (Note: various
distributions are configured differently). A distribution is free to create
Vulkan-specific system directories (e.g. ".../vulkan/layers"), but is not
required to do so. If the layer is specified via a relative pathname, it is
relative to the path of the info file (e.g. for cases when an application
provides a layer that is in the same directory hierarchy as the rest of the
application files).

There are no rules about the name of the text files (except the .json suffix).

There are no rules about the name of the layer shared library files.

##### Using Pre-Production Layers

As with ICDs, developers may need to use special, pre-production layers,
without modifying the properly-installed layers.  This need is met with the use
of the "VK\_LAYER\_PATH" environment variable, which will override the
mechanism using for finding properly-installed layers. Because many layers may
exist on a system, this environment variable is a colon-separated list of
directories that contain layer info files. Only the directories listed in
"VK\_LAYER\_PATH" will be scanned for info files. Each colon-separated entry
is:

- The full pathname of a directory containing layer info files

NOTE: these environment variables will be ignored for suid programs.

#### Android

The recommended way to enable layers is for applications
to programatically enable them. The layers are provided by the application
and must live in the application's library folder. The application
enables the layers at vkCreateInstance and vkCreateDevice as any Vulkan
application would.
An application enabled for debug has more options. It can enumerate and enable
layers located in /data/local/vulkan/debug.

Layer interface requirements
------------------------------------------------------

#### Architectural interface overview

There are two key architectural features that drive the loader to layer library
interface: 1) separate and distinct instance and device call chains, and 2)
distributed dispatch. First these architectural features will be described and
then the detailed interface will be specified.

Call chains are the links of calls for a given Vulkan command from layer module
to layer module with the loader and or the ICD being the bottom most command.
Call chains are constructed at both the instance level and the device level by
the loader with cooperation from the layer libraries. Instance call chains are
constructed by the loader when layers are enabled at vkCreateInstance. Device
call chains are constructed by the loader when layers are enabled at
CreateDevice. A layer can intercept Vulkan instance commands, device commands
or both. For a layer to intercept instance commands, it must participate in the
instance call chain. For a layer to intercept device commands, it must
participate in the device chain. Layers which participate in intercepting calls
in both the instance and device chains are called global layers.

Normally, when a layer intercepts a given Vulkan command, it will call down the
instance or device chain as needed. The loader and all layer libraries that
participate in a call chain cooperate to ensure the correct sequencing of calls
from one entity to the next. This group effort for call chain sequencing is
hereinafter referred to as distributed dispatch. In distributed dispatch, since
each layer is responsible for properly calling the next entity in the device or
instance chain, a dispatch mechanism is required for all Vulkan commands a
layer intercepts. For Vulkan commands that are not intercepted by a layer, or
if the layer chooses to terminate a given Vulkan command by not calling down
the chain, then no dispatch mechanism is needed for that particular Vulkan
command.  Only for those Vulkan commands, which may be a subset of all Vulkan
commands, that a layer intercepts is a dispatching mechanism by the layer
needed. The loader is responsible for dispatching all core and instance
extension Vulkan commands to the first entity in the chain.

Instance level Vulkan commands are those that have the dispatchable objects
VkInstance, or VkPhysicalDevice as the first parameter and also includes
vkCreateInstance.

Device level Vulkan commands are those that use VkDevice, VkQueue or
VkCommandBuffer as the first parameter and also include vkCreateDevice. Future
extensions may introduce new instance or device level dispatchable objects, so
the above lists may be extended in the future.

#### Discovery of layer entry points

For the layer libraries that have been discovered by the loader, their
intercepting entry points that will participate in a device or instance call
chain need to be available to the loader or whatever layer is before them in
the chain.  Layers have the following requirements in this area.
- A layer intercepting instance level Vulkan commands (aka an instance level
layer) must implement a vkGetInstanceProcAddr type of function.
- This vkGetInstanceProcAddr type function must be exported by the layer
library. The name of this function is specified in various ways: 1) the layer
manifest JSON file in the "functions", "vkGetInstanceProcAddr" node
(Linux/Windows); 2) it is named "vkGetInstanceProcAddr"; 3) it is
"<layerName>GetInstanceProcAddr" (Android).
- A layer intercepting device level Vulkan commands (aka a device level layer)
must implement a vkGetDeviceProcAddr type of function.
- This vkGetDeviceProcAddr type function must be exported by the layer library.
The name of this function is specified in various ways: 1) the layer manifest
JSON file in the "functions", "vkGetDeviceProcAddr" node (Linux/Windows); 2) it
is named "vkGetDeviceProcAddr"; 3) it is "<layerName>GetDeviceProcAddr"
(Android).
- A layer's vkGetInstanceProcAddr function (regardless of its name) must return
the local entry points for all instance level Vulkan commands it intercepts. At
a minimum, this includes vkGetInstanceProcAddr and vkCreateInstance.
- A layer's vkGetDeviceProcAddr function (regardless of its name) must return
the entry points for all device level Vulkan commands it intercepts. At a
minimum, this includes vkGetDeviceProcAddr and vkCreateDevice.
- There are no requirements on the names of the intercepting functions a layer
implements except those listed above for vkGetInstanceProcAddr and
vkGetDeviceProcAddr.
- Currently a layer's VkGetInstanceProcAddr must be able to handle a VkInstance
parameter equal to NULL for
instance level commands it intercepts including vkCreateDevice.
- Currently a layer's VkGetDeviceProcAddr must be able to handle a VkDevice
parameter equal to NULL for device level commands it intercepts.

#### Layer intercept requirements

- Layers intercept a Vulkan command by defining a C/C++ function with signature
identical to the Vulkan API for that command.
- Other than the two vkGet*ProcAddr, all other functions intercepted by a layer
need NOT be exported by the layer.
- For any Vulkan command a layer intercepts which has a non-void return value,
an appropriate value must be returned by the layer intercept function.
- The layer intercept function must call down the chain to the corresponding
Vulkan command in the next entity. Undefined results will occur if a layer
doesn't propagate calls down the chain. The two exceptions to this requirement
are vkGetInstanceProcAddr and vkGetDeviceProcAddr which only call down the
chain for Vulkan commands that they do not intercept.
- Layer intercept functions may insert extra calls to Vulkan commands in
addition to the intercept. For example, a layer intercepting vkQueueSubmit may
want to add a call to vkQueueWaitIdle after calling down the chain for
vkQueueSubmit.  Any additional calls inserted by a layer must be on the same
chain. They should call down the chain.

#### Distributed dispatching requirements

- For each entry point a layer intercepts, it must keep track of the entry
point residing in the next entity in the chain it will call down into. In other
words, the layer must have a list of pointers to functions of the appropriate
type to call into the next entity. This can be implemented in various ways but
for clarity will be referred to as a dispatch table.
- A layer can use the VkLayerDispatchTable structure as a device dispatch table
(see include/vulkan/vk_layer.h).
- A layer can use the VkLayerInstanceDispatchTable structure as a instance
dispatch table (see include/vulkan/vk_layer.h).
- Layers vkGetInstanceProcAddr function uses the next entity's
vkGetInstanceProcAddr to call down the chain for unknown (i.e. non-intercepted)
functions.
- Layers vkGetDeviceProcAddr function uses the next entity's
vkGetDeviceProcAddr to call down the chain for unknown (i.e. non-intercepted)
functions.

#### Layer dispatch initialization

- A layer initializes its instance dispatch table within its vkCreateInstance
function.
- A layer initializes its device dispatch table within its vkCreateDevice
function.
- The loader passes a linked list of initialization structures to layers via
the "pNext" field in the VkInstanceCreateInfo and VkDeviceCreateInfo structures
for vkCreateInstance  and VkCreateDevice respectively.
- The head node in this linked list is of type VkLayerInstanceCreateInfo for
instance and VkLayerDeviceCreateInfo for device. See file
include/vulkan/vk_layer.h for details.
- A VK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO is used by the loader for the
"sType" field in VkLayerInstanceCreateInfo.
- A VK_STRUCTURE_TYPE_LOADER_DEVICE_CREATE_INFO is used by the loader for the
"sType" field in VkLayerDeviceCreateInfo.
- The "function" field indicates how the union field "u" should be interpreted
within VkLayer*CreateInfo. The loader will set the "function" field to
VK_LAYER_LINK_INFO. This indicates "u" field should be VkLayerInstanceLink or
VkLayerDeviceLink.
- The VkLayerInstanceLink and VkLayerDeviceLink structures are the list nodes.
- The VkLayerInstanceLink contains the next entity's vkGetInstanceProcAddr used
by a layer.
- The VkLayerDeviceLink contains the next entity's vkGetInstanceProcAddr and
vkGetDeviceProcAddr used by a layer.
- Given the above structures set up by the loader, layer must initialize their
dispatch table as follows:
  - Find the  VkLayerInstanceCreateInfo/VkLayerDeviceCreateInfo structure in
the VkInstanceCreateInfo/VkDeviceCreateInfo structure.
  - Get the next entity's vkGet*ProcAddr from the "pLayerInfo" field.
  - For CreateInstance get the next entity's vkCreateInstance by calling the
"pfnNextGetInstanceProcAddr":
     pfnNextGetInstanceProcAddr(NULL, "vkCreateInstance").
  - For CreateDevice get the next entity's vkCreateDevice by calling the
"pfnNextGetInstanceProcAddr":
     pfnNextGetInstanceProcAddr(NULL, "vkCreateDevice").
  - Advanced the linked list to the next node: pLayerInfo = pLayerInfo->pNext.
  - Call down the chain either CreateDevice or CreateInstance
  - Initialize your layer dispatch table by calling the next entity's
Get*ProcAddr function once for each Vulkan command needed in your dispatch
table

#### Example code for CreateInstance

```cpp
VkResult vkCreateInstance(
        const VkInstanceCreateInfo *pCreateInfo,
        const VkAllocationCallbacks *pAllocator,
        VkInstance *pInstance)
{
   VkLayerInstanceCreateInfo *chain_info =
        get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);

    assert(chain_info->u.pLayerInfo);
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr =
        chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkCreateInstance fpCreateInstance =
        (PFN_vkCreateInstance)fpGetInstanceProcAddr(NULL, "vkCreateInstance");
    if (fpCreateInstance == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // Advance the link info for the next element of the chain
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;

    // Continue call down the chain
    VkResult result = fpCreateInstance(pCreateInfo, pAllocator, pInstance);
    if (result != VK_SUCCESS)
        return result;

    // Allocate new structure to store peristent data
    layer_data *my_data = new layer_data;

    // Associate this instance with the newly allocated data
    // layer will store any persistent state it needs for
    // this instance in the my_data structure
    layer_data_map[get_dispatch_key(*pInstance)] = my_data;

    // Create layer's dispatch table using GetInstanceProcAddr of
    // next layer in the chain.
    my_data->instance_dispatch_table = new VkLayerInstanceDispatchTable;
    layer_init_instance_dispatch_table(
        *pInstance, my_data->instance_dispatch_table, fpGetInstanceProcAddr);

    // Keep track of any extensions that were enabled for this
    // instance. In this case check for VK_EXT_debug_report
    my_data->report_data = debug_report_create_instance(
        my_data->instance_dispatch_table, *pInstance,
        pCreateInfo->enabledExtensionCount,
        pCreateInfo->ppEnabledExtensionNames);

    // Other layer initialization
    ...

    return VK_SUCCESS;
}
```

#### Example code for CreateDevice

```cpp
VkResult 
vkCreateDevice(
        VkPhysicalDevice gpu,
        const VkDeviceCreateInfo *pCreateInfo,
        const VkAllocationCallbacks *pAllocator,
        VkDevice *pDevice)
{
    VkLayerDeviceCreateInfo *chain_info =
        get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);

    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr =
        chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr fpGetDeviceProcAddr =
        chain_info->u.pLayerInfo->pfnNextGetDeviceProcAddr;
    PFN_vkCreateDevice fpCreateDevice =
        (PFN_vkCreateDevice)fpGetInstanceProcAddr(NULL, "vkCreateDevice");
    if (fpCreateDevice == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // Advance the link info for the next element on the chain
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;

    VkResult result = fpCreateDevice(gpu, pCreateInfo, pAllocator, pDevice);
    if (result != VK_SUCCESS) {
        return result;
    }

    // Allocate new structure to store peristent data
    layer_data *my_data = new layer_data;

    // Associate this instance with the newly allocated data
    // layer will store any persistent state it needs for
    // this instance in the my_data structure
    layer_data_map[get_dispatch_key(*pDevice)] = my_data;

    my_device_data->device_dispatch_table = new VkLayerDispatchTable;
    layer_init_device_dispatch_table(
        *pDevice, my_device_data->device_dispatch_table, fpGetDeviceProcAddr);

    // Keep track of any extensions that were enabled for this
    // instance. In this case check for VK_EXT_debug_report
    my_data->report_data = debug_report_create_instance(
        my_instance_data->report_data, *pDevice);

    // Other layer initialization
    ...

    return VK_SUCCESS;
}
```

#### Special Considerations
A layer may want to associate it's own private data with one or more Vulkan
objects.
Two common methods to do this are hash maps  and object wrapping. The loader
supports layers wrapping any Vulkan object including dispatchable objects.
Layers which wrap objects should ensure they always unwrap objects before
passing them down the chain. This implies the layer must intercept every Vulkan
command which uses the object in question. Layers above the object wrapping
layer will see the wrapped object. Layers which wrap dispatchable objects must
ensure that the first field in the wrapping structure is a pointer to a dispatch table
as defined in vk_layer.h. Specifically, an instance wrapped dispatchable object
could be as follows:
```
struct my_wrapped_instance_obj_ {
    VkLayerInstanceDispatchTable *disp;
    // whatever data layer wants to add to this object
};
```
A device wrapped dispatchable object could be as follows:
```
struct my_wrapped_instance_obj_ {
    VkLayerDispatchTable *disp;
    // whatever data layer wants to add to this object
};
```

Alternatively, a layer may want to use a hash map to associate data with a
given object. The key to the map could be the object. Alternatively, for
dispatchable objects at a given level (eg device or instance) the layer may
want data associated with the VkDevice or VkInstance objects. Since
there are multiple dispatchable objects for a given VkInstance or VkDevice, the
VkDevice or VkInstance object is not a great map key. Instead the layer should
use the dispatch table pointer within the VkDevice or VkInstance since that
will be unique for a given VkInstance or VkDevice.

Layers which create dispatchable objects take special care. Remember that loader
trampoline code normally fills in the dispatch table pointer in the newly
created object. Thus, the layer must fill in the dispatch table pointer if the
loader trampoline will not do so.  Common cases where a layer (or ICD) may create a
dispatchable object without loader trampoline code is as follows:
- object wrapping layers that wrap dispatchable objects
- layers which add extensions that create dispatchable objects
- layers which insert extra Vulkan commands in the stream of commands they
intercept from the application
- ICDs which add extensions that create dispatchable objects

To fill in the dispatch table pointer in newly created dispatchable object,
the layer should copy the dispatch pointer, which is always the first entry in the structure, from an existing parent object of the same level (instance versus
device). For example, if there is a newly created VkCommandBuffer object, then the dispatch pointer from the VkDevice object, which is the parent of the VkCommandBuffer object, should be copied into the newly created object.

