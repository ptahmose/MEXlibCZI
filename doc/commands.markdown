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
| [Close](#close) | Close the document and free the resources. |
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

![GetMetadataXML example](pictures/getmetadataxml_sample.png)

## Close

Close the document and free the resources. The handle is no longer valid.

![Close example](pictures/close_sample.png)

## GetDefaultDisplaySettings

Get the display-settings from the CZI-document. The function returns a struct.

![GetDefaultDisplaySettings example](pictures/getdefaultdisplaysettings_sample.png)

## GetSubBlock

Get a handle to a subblock (and return a handle to it).

| argument # | type            | comment                              |
| :--------- | :-------------- | :----------------------------------- |
| 1          | integer         | The handle (as returned from "Open") |
| 2          | integer         | The subblock number                  |

![GetSubBlock example](pictures/getsubblock_sample.png)

## GetInfoFromSubBlock

Get information about a subblock. The function returns a struct.

| argument # | type            | comment                                              |
| :--------- | :-------------- | :--------------------------------------------------- |
| 1          | integer         | The handle (as returned from "Open")                 |
| 2          | integer         | The subblock handle (as returned from "GetSubBlock"  |

![GetInfoFromSubBlock example](pictures/getinfofromsubblock_sample.png)

## GetBitmapFromSubBlock

Get the bitmap from the specified subblock. The subblock is decompressed (if necessary) and the bitmap is returned.

| argument # | type            | comment                                              |
| :--------- | :-------------- | :--------------------------------------------------- |
| 1          | integer         | The handle (as returned from "Open")                 |
| 2          | integer         | The subblock handle (as returned from "GetSubBlock"  |

![GetBitmapFromSubBlock example](pictures/getbitmapfromsubblock_sample.png)

## GetMetadataFromSubBlock

Get the XML-metadata from the specified subblock. 

| argument # | type            | comment                                              |
| :--------- | :-------------- | :--------------------------------------------------- |
| 1          | integer         | The handle (as returned from "Open")                 |
| 2          | integer         | The subblock handle (as returned from "GetSubBlock"  |


![GetMetadataFromSubBlock example](pictures/getmetadatafromsubblock_sample.png)

## ReleaseSubBlock

Releases a subblock handle. The handle is no longer valid, and the resources are freed.

## CreateCziWriter

Creates a writer object (for writing a CZI document) and returns a handle to it.

| argument # | type              | comment                                              |
| :--------- | :---------------- | :--------------------------------------------------- |
| 1          | string            | The filename.                                        |
| 2          | string (optional) | Options for creating the file.                       |

For the argument 2, the following options are available:

| Option | Description |
| --- | --- |
| "overwrite" or 'x' | Overwrite the file if it exists. |

The function returns a handle to the writer object.

![CreateCziWriter example](pictures/createcziwriter_sample.png)

## AddSubBlock

Adds a subblock to the writer object.

| argument # | type              | comment                                              |
| :--------- | :---------------- | :--------------------------------------------------- |
| 1          | integer           | The handle (as returned from "CreateCziWriter")      |
| 2          | string            | The coordinate of the subblock (in a string represenation).     |
| 3          | array of numbers  | The coordinates of the logical ROI |
| 4          | string            | The pixel type|
| 5          | 2d or 3d array of numbers  | The bitmap to be written into the subblock |
| 6          | struct (optional) | property bag with options for the operation |

The following pixel types are available (for argument #4):

| Pixel type | Description |
| --- | --- |
| "gray8" | 8-bit grayscale |
| "gray16" | 16-bit grayscale |
| "bgr24" | 8-bit BGR color |

The following options are available (for argument #6):

| Option         | type    | Description |
| :-----         | :---    | :--- |
| "M"            | integer | The value of the M-index |
| "metadata_xml" | string  | XML metadata to be written to the subblock |
| "compression"  | string  | A string describing the compression to be used for the bitmap. The syntax is as defined [here](https://zeiss.github.io/libczi/classlib_c_z_i_1_1_utils.html#a4cb9b660d182e59a218f58d42bd04025). |

![AddSubBlock example](pictures/addsubblock_sample.png)

![AddSubBlock example](pictures/addsubblock_sample_2.png)

## CloseCziWriter

Closes a writer object and finalizes the CZI-document.