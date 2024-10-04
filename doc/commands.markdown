The following commands are available in the mex file.

| Command | Description |
| --- | --- |
| [GetVersion](#getversion) | Returns the version of the MEXlibCZI mex file. |
| [Open](#open) | Open a CZI file and return a handle to the document. |
| [GetSubBlockBitmap](#getsubblockbitmap) | Read the specified subblock and get the (uncompressed) bitmap. |
| [GetInfo](#getinfo) | Get information about the specified CZI-document. |
| [GetScaling](#getscaling) | Retrieve scaling information from the specified document. |
| [GetMultiChannelScalingTileComposite](#getmultichannelscalingtilecomposite) | Get a multi-channel composite image from the specified axis-aligned ROI with the specified zoom. |
| [GetSingleChannelScalingTileComposite](#getsinglechannelscalingtilecomposite) | Get a tile-composite for the specified region-of-interest for the specified plane with the specified zoom-factor. |
| [GetMetadataXml](#getmetadataxml) | Get the metadata of the specified CZI-document as an XML string. |
| [GetDefaultDisplaySettings](#getdefaultdisplaysettings) | Get the display-settings from the CZI-document.  |
| [GetSubBlock](#getsubblock) | Get a handle to a subblock (and return a handle to it).  |
| [GetInfoFromSubBlock](#getinfofromsubblock) | Get information about a subblock. |
| [GetBitmapFromSubBlock](#getbitmapfromsubblock) | Get the bitmap of a subblock.  |
| [GetMetadataFromSubBlock](#getmetadatafromsubblock) | Get the metadata from a subblock. |
| [ReleaseSubBlock](#releasesubblock) | Releases a handle for a subblock. |
| [CreateCziWriter](#createcziwriter) | Creates a writer object (for writing a CZI document) and returns a handle to it. |
| [AddSubBlock](#addsubblock) | Adds a subblock to the writer object.|
| [CloseCziWriter](#closecziwriter) | Closes a writer object and finalizes the CZI-document. |

## GetVersion

The function returns a struct containing version information. It does not take any arguments.
![CZIGetInfo sample](pictures/getversion_sample.PNG)

## Open

The specified file (second argument) is opened, and a handle for the document is returned.

![CZIGetInfo sample](pictures/open_sample.PNG)

## GetSubBlockBitmap

| argument # | type    | comment                              |
| :--------- | :------ | :----------------------------------- |
| 1          | integer | The handle (as returned from "Open") |
| 2          | integer | The subblock number                  |

Read the specified subblock and get the (uncompressed) bitmap.

![CZIGetInfo sample](pictures/getsubblockbitmap_sample.PNG)

## GetInfo

Get information about the specified CZI-document. A struct is returned.

![CZIGetInfo sample](pictures/getinfo1_sample.PNG)
![CZIGetInfo sample](pictures/getinfo2_sample.PNG)
![CZIGetInfo sample](pictures/getinfo3__sample.PNG)

## GetScaling

Retrieve scaling information from the specified document. The scaling is given in units of µm. 
If no information is available, the property will have the value of NaN.

![CZIGetInfo sample](pictures/getscaling_sample.PNG)

## GetMultiChannelScalingTileComposite

Get a multi-channel composite image from the specified axis-aligned ROI with the specified zoom.

| argument # | type            | comment                              |
| :--------- | :-------------- | :----------------------------------- |
| 1          | integer         | The handle (as returned from "Open") |
| 2          | array of numbers| The coordinates of the ROI           |
| 3          | string          | The plane coordinate                 |
| 4          | number          | The zoom                             |

![CZIGetInfo sample](pictures/getmultichannelscalingtilecomposite_sample.PNG)


## GetSingleChannelScalingTileComposite

Get a tile-composite for the specified region-of-interest for the specified plane with the specified zoom-factor.
zoom is a number between 1 and 0, and the pixel-size of the resulting image will be zoom * w and zoom * h.

| argument # | type            | comment                              |
| :--------- | :-------------- | :----------------------------------- |
| 1          | integer         | The handle (as returned from "Open") |
| 2          | array of numbers| The coordinates of the ROI           |
| 3          | string          | The plane coordinate                 |
| 4          | number          | The zoom                             |

![CZIGetInfo sample](pictures/getsinglechannelscalingtilecomposite_sample_.PNG)

## GetMetadataXml

Get the metadata of the specified CZI-document as an XML string.

## Close

Close the document and free the resources.