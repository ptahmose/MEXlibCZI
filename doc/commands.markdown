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
| [QuerySubBlockDirectory](#querysubblockdirectory) | Get information about all (or a filtered subset of the) subblocks without reading them. |
| [CreateCziWriter](#createcziwriter) | Creates a writer object (for writing a CZI document) and returns a handle to it. |
| [AddSubBlock](#addsubblock) | Adds a subblock to the writer object.|
| [CloseCziWriter](#closecziwriter) | Closes a writer object and finalizes the CZI-document. |
| [Writer_GetPreparedMetadata](#writer_getpreparedmetadata) | Creates a mutable metadata builder from the writer's current image information. |
| [MetadataBuilder_Modify](#metadatabuilder_modify) | Applies a convenience operation, such as setting pixel sizes. |
| [MetadataBuilder_GetXml](#metadatabuilder_getxml) | Retrieves a metadata builder's XML document. |
| [MetadataBuilder_SetXml](#metadatabuilder_setxml) | Replaces a metadata builder's XML document. |
| [Writer_WriteMetadata](#writer_writemetadata) | Writes a complete metadata XML document to a CZI writer. |
| [MetadataBuilder_Release](#metadatabuilder_release) | Releases a metadata builder handle. |

Command names and metadata operation names are case-sensitive. Examples use
`MEXlibCZI`; for Octave, replace it with `octavelibczi`. For the metadata commands,
text arguments must be character arrays (single quotes), rather than MATLAB
string objects. Argument numbers in the tables exclude the command name.

## GetVersion

The function returns a struct containing version information. It does not take any arguments.
![CZIGetInfo sample](pictures/getversion_sample.PNG)

## Open

The specified file (second argument) is opened, and a handle for the document is returned.
An optional third argument supplies libCZI reader open options as a character string
of semicolon-separated `key=value` pairs:

```matlab
h = MEXlibCZI('Open', 'sample.czi', 'dimensionSwapWorkaround=XtSubblocks');
```

Omitting this argument or passing `''` uses libCZI's default open options.
Unspecified options retain their defaults. Keys and values are case-sensitive.
Unknown keys are ignored by libCZI; invalid syntax or values produce an error.
The same optional argument is supported by `octavelibczi`.

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

Retrieve scaling information from the specified document. The returned struct has
double fields `scaleX`, `scaleY`, and `scaleZ`, given in **metres**.
If no information is available, the property will have the value of NaN.

```matlab
scaling = MEXlibCZI('GetScaling', reader);
pixel_size_x_um = scaling.scaleX * 1e6;
```

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

```matlab
xml = MEXlibCZI('GetMetadataXml', reader);
```

This command accepts a reader handle returned by `Open`. To retrieve XML from a
mutable metadata builder, use [MetadataBuilder_GetXml](#metadatabuilder_getxml).

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

## QuerySubBlockDirectory

Queries the subblock directory of the document and returns information about the subblocks, optionally restricted to a subset
(like the "EnumSubset"-operation in libCZI). The subblock directory is held in memory, so this operation does not read any 
subblock from the file. It is the efficient way to find out which subblocks exist (e.g. for a given plane), and the returned
`Index` can then be used with "GetSubBlock" (or "GetSubBlockBitmap") to read the subblock.

| argument # | type              | comment                                              |
| :--------- | :---------------- | :--------------------------------------------------- |
| 1          | integer           | The handle (as returned from "Open")                 |
| 2          | struct (optional) | The filter, selecting the subblocks to be returned. If omitted, all subblocks are returned. |

The following (all optional) fields of the filter struct are available:

| Field          | type                        | Description |
| :-----         | :---                        | :--- |
| "Coordinate"   | struct                      | Restricts the result to subblocks with the specified coordinates. For each dimension (`Z`, `C`, `T`, `R`, `S`, `I`, `H`, `V`, `B`) a field can be given, which is either a single integer *n* (selects that value) or `[start size]` (selects the values *start* to *start+size-1*; *size* must be greater than 0). This is the same representation as used for dimension bounds elsewhere (e.g. in "GetInfo"). The subblock's coordinate must lie in the interval for each dimension given; a subblock without the queried dimension does not fail the filter; the dimension is simply ignored for that subblock. Dimensions not mentioned are not restricted. Values must be integral (fractional values are an error). Example: `struct('C', 0, 'Z', [2 2])` selects channel 0 and Z-planes 2 and 3. |
| "Roi"          | array of 4 numbers          | `[x y width height]`. Only subblocks whose logical rectangle intersects this region are returned. |
| "OnlyLayer0"   | logical                     | If true, only subblocks of pyramid-layer 0 are returned. Default is false. |

The function returns a struct of arrays. All arrays have one row per returned subblock (N rows), and row *i* of each field describes the same subblock.
**The order of the subblocks is not specified** - in particular, it is not guaranteed to be sorted by index or by coordinate, and it can differ between
different queries. Use `Index` to identify a subblock.

| Field                  | type / size    | Description |
| :-----                 | :---           | :--- |
| "Index"                | int32, Nx1     | The index of the subblock (the number to be used with "GetSubBlock" and "GetSubBlockBitmap"). |
| "Mode"                 | uint8, Nx1     | The compression mode (numeric code, see below). |
| "Pixeltype"            | uint8, Nx1     | The pixel type (numeric code, see below). |
| "CoordinateDimensions" | char, 1xD      | The dimensions present in at least one subblock in the document, in the order `ZCTRSIHVB`, e.g. `'ZCT'`. This is independent of the filter, including when no subblocks match. |
| "Coordinate"           | int32, NxD     | Column *j* contains the coordinate for the dimension given by the *j*-th character of "CoordinateDimensions". For a subblock which does not have this dimension, the value is `intmin('int32')`. |
| "LogicalRect"          | int32, Nx4     | `[x y width height]` of the subblock's logical rectangle. |
| "PhysicalSize"         | int32, Nx2     | `[width height]` of the subblock's bitmap. The zoom factor can be calculated from "LogicalRect" and "PhysicalSize". |
| "MIndex"               | int32, Nx1     | The M-index. If the subblock does not have a valid M-index, the value is `intmin('int32')`. |

The numeric codes for "Pixeltype" are:

| Code | Pixel type |
| :--- | :--- |
| 0 | Gray8 |
| 1 | Gray16 |
| 2 | Gray32Float |
| 3 | Bgr24 |
| 4 | Bgr48 |
| 8 | Bgr96Float |
| 9 | Bgra32 |
| 10 | Gray64ComplexFloat |
| 11 | Bgr192ComplexFloat |
| 12 | Gray32 |
| 13 | Gray64Float |
| 255 | Invalid |

The numeric codes for "Mode" are:

| Code | Compression |
| :--- | :--- |
| 0 | Uncompressed |
| 1 | JPG |
| 4 | JPG-XR |
| 5 | Zstd0 |
| 6 | Zstd1 |
| 7 | ChunkedExtensible |
| 255 | Invalid |

Example - read all subblocks of channel 0:

```matlab
d = MEXlibCZI('QuerySubBlockDirectory', handle, struct('Coordinate', struct('C', 0)));
for k = 1:numel(d.Index)
    bitmap = MEXlibCZI('GetSubBlockBitmap', handle, d.Index(k));
end
```

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

```matlab
MEXlibCZI('CloseCziWriter', writer);
```

The argument is a writer handle returned by `CreateCziWriter`. There is no output;
after closing, the handle is invalid. Closing is required to finalize the file.

If no explicit metadata write has succeeded, this command generates minimal
document metadata from the subblocks and default channel display settings, as
in existing workflows. After a successful [Writer_WriteMetadata](#writer_writemetadata)
call, it preserves the explicitly written metadata and skips that generation.
Preparing or editing a metadata builder alone does not disable the automatic path.
Closing a writer does not release independently held metadata builders.

## Writing document metadata

After adding **all** subblocks, prepare a mutable metadata builder, edit it,
and write its XML explicitly:

```matlab
writer = MEXlibCZI('CreateCziWriter', 'output.czi', 'x');
MEXlibCZI('AddSubBlock', writer, 'C0', [0 0 2 2], ...
    'gray8', uint8([1 2; 3 4]), struct('M', 0));
metadata_builder = MEXlibCZI('Writer_GetPreparedMetadata', writer);
MEXlibCZI('MetadataBuilder_Modify', metadata_builder, 'Scaling', ...
    struct('X', 0.25e-6, 'Y', 0.25e-6, 'Z', 1e-6));
xml = MEXlibCZI('MetadataBuilder_GetXml', metadata_builder);
% Optionally edit xml using MATLAB XML facilities.
MEXlibCZI('Writer_WriteMetadata', writer, xml);
MEXlibCZI('CloseCziWriter', writer);
MEXlibCZI('MetadataBuilder_Release', metadata_builder);
```

Document metadata describes the entire CZI file. It is separate from the
per-subblock `metadata_xml` option of [AddSubBlock](#addsubblock).

The workflow follows libCZI: prepare an `ICziMetadataBuilder`, edit it through
convenience operations or XML, then pass the final XML to the writer. All six
commands are described below. Builder handles are opaque scalar `int32` values;
retain the returned value and do not construct handles manually.

### Verify the result

After closing the writer, reopen the file to inspect the scaling or document XML:

```matlab
reader = MEXlibCZI('Open', 'output.czi');
scaling = MEXlibCZI('GetScaling', reader);
xml = MEXlibCZI('GetMetadataXml', reader);
MEXlibCZI('Close', reader);
% scaling.scaleX is 0.25e-6 metres (0.25 micrometres).
```

## Writer_GetPreparedMetadata

Creates a mutable metadata builder populated with information already known to
the writer, such as image dimensions derived from the written subblocks.

```matlab
metadata_builder = MEXlibCZI('Writer_GetPreparedMetadata', writer);
```

| argument # | type | Description |
| --- | --- | --- |
| 1 | scalar integer handle | An open writer returned by `CreateCziWriter`. |

**Output:** exactly one opaque `int32` metadata builder handle.

Add all subblocks before calling this command. The returned builder is an
independent snapshot; adding more subblocks does not update it. Prepare a new
builder if the image data changes, and release any builders you no longer need.
Calling this command again creates another independent builder.

Preparation does not write metadata to the file. It also does not add the legacy
default display settings produced by `CloseCziWriter`'s automatic path. An
explicitly written document must contain the display settings your reader needs.

The builder remains usable after its originating writer closes. Release it with
[MetadataBuilder_Release](#metadatabuilder_release).

## MetadataBuilder_Modify

Applies a convenience operation to a metadata builder. Supported operations are
`'Scaling'` (`MetadataUtils::WriteScalingInfoEx`) and `'GeneralDocumentInfo'`
(`MetadataUtils::WriteGeneralDocumentInfo`).
Further convenience operations can be added under the same command.

```matlab
MEXlibCZI('MetadataBuilder_Modify', metadata_builder, 'Scaling', values);
```

| argument # | type | Description |
| --- | --- | --- |
| 1 | scalar integer handle | A live metadata builder handle. |
| 2 | character array | `'Scaling'` or `'GeneralDocumentInfo'`. |
| 3 | scalar struct | The operation's parameters. |

**Outputs:** none. The builder is modified in place; the CZI file is unchanged.

### Scaling parameters

| Field | type | Description |
| --- | --- | --- |
| `X` | double or single scalar | Physical size of a pixel along X, in metres. |
| `Y` | double or single scalar | Physical size of a pixel along Y, in metres. |
| `Z` | double or single scalar | Z spacing, in metres. |
| `DefaultUnitFormatX` | character array | Suggested display unit format for X, e.g. `'um'`. |
| `DefaultUnitFormatY` | character array | Suggested display unit format for Y. |
| `DefaultUnitFormatZ` | character array | Suggested display unit format for Z. |

All fields are optional, but at least one recognized field must be present.
Numeric scaling values must be finite, positive, non-sparse scalars. Integer
numeric classes, vectors, zero, negative values, `NaN`, and `Inf` are rejected.
Display unit formats do not convert the numeric values: those are always metres.

Omitted fields retain their existing metadata values. Empty unit-format character
arrays do not clear an existing format. Unrecognized struct fields are ignored;
an unsupported operation name produces an error. Recognized fields are validated
before the convenience operation is applied, so an invalid field leaves the
builder unchanged.

```matlab
% Set 0.25 micrometres per pixel along X and Y.
values = struct('X', 0.25e-6, 'Y', 0.25e-6, ...
    'DefaultUnitFormatX', 'um', 'DefaultUnitFormatY', 'um');
MEXlibCZI('MetadataBuilder_Modify', metadata_builder, 'Scaling', values);

% Add Z spacing later without changing X or Y.
MEXlibCZI('MetadataBuilder_Modify', metadata_builder, 'Scaling', ...
    struct('Z', 1e-6));
```

Use [MetadataBuilder_GetXml](#metadatabuilder_getxml) to obtain the modified
document and [Writer_WriteMetadata](#writer_writemetadata) to write it.

### GeneralDocumentInfo parameters

Sets general information under `Metadata/Information/Document` in the builder.

```matlab
MEXlibCZI('MetadataBuilder_Modify', metadata_builder, 'GeneralDocumentInfo', info);
```

| Field | type | Description |
| --- | --- | --- |
| `Name` | character array | Document name. |
| `Title` | character array | Document title. |
| `UserName` | character array | Name of the user who created the document. |
| `Description` | character array | Description of the document. |
| `Comment` | character array | Additional comments. |
| `Keywords` | character array | Keywords separated by semicolons. |
| `CreationDateTime` | character array | Creation timestamp in XML `dateTime` format, e.g. `'2026-10-29T13:42:17.123Z'`. Written to the XML `CreationDate` element. |
| `Rating` | int32 scalar | Five-star rating from 0 to 5, inclusive. |

All fields are optional, but at least one recognized field must be supplied.
Omitted fields and empty character arrays leave existing values unchanged,
following libCZI's string-setter semantics. Use XML editing to clear a text field.
Unrecognized struct fields are ignored. Supplied fields are checked before the
update is applied. Creation timestamps are passed through as text without date
parsing or format validation; MATLAB `datetime` objects are not accepted directly.

```matlab
t = datetime('now', 'TimeZone', 'UTC');
t.Format = 'yyyy-MM-dd''T''HH:mm:ss.SSSXXX';
info = struct('Name', 'Processed image', ...
    'Description', 'Background subtraction applied', ...
    'Keywords', 'processed;fluorescence', ...
    'CreationDateTime', char(t), 'Rating', int32(4));
MEXlibCZI('MetadataBuilder_Modify', metadata_builder, 'GeneralDocumentInfo', info);
```

`XXX` includes a timezone offset, or `Z` for UTC. In Octave, supply the timestamp
as a character array directly or create it using its date-formatting facilities.
These changes affect the builder only: retrieve the XML and pass it to
`Writer_WriteMetadata` to persist them in the CZI file.

## MetadataBuilder_GetXml

Serializes a metadata builder's current document as formatted XML.

```matlab
xml = MEXlibCZI('MetadataBuilder_GetXml', metadata_builder);
```

| argument # | type | Description |
| --- | --- | --- |
| 1 | scalar integer handle | A live metadata builder handle. |

**Output:** exactly one character array containing the complete metadata XML.

Serialization does not change the builder or write to a file. The returned XML
can be inspected or edited in MATLAB, loaded back through
[MetadataBuilder_SetXml](#metadatabuilder_setxml), or passed directly to
[Writer_WriteMetadata](#writer_writemetadata). This command accepts builder
handles; use [GetMetadataXml](#getmetadataxml) for reader handles.

## MetadataBuilder_SetXml

Replaces the complete XML document behind an existing metadata builder handle.
This permits arbitrary XML edits followed by further convenience operations.

```matlab
MEXlibCZI('MetadataBuilder_SetXml', metadata_builder, xml);
```

| argument # | type | Description |
| --- | --- | --- |
| 1 | scalar integer handle | A live metadata builder handle. |
| 2 | character array | A complete metadata XML document with an `ImageDocument` root. |

**Outputs:** none. The existing handle remains valid and refers to the replacement
document. This replaces the whole document rather than merging individual nodes.

The XML is parsed into a replacement builder first. Invalid XML or a missing
`ImageDocument` root produces an error and leaves the existing builder unchanged.
Parsing does not validate the CZI metadata schema or consistency with image data.
No metadata is written to the CZI file by this command.

```matlab
% Example custom XML edit; the prepared document contains a Metadata element.
xml = MEXlibCZI('MetadataBuilder_GetXml', metadata_builder);
xml = strrep(xml, '</Metadata>', ...
    '<CustomAttributes><ProcessingNote>Example</ProcessingNote></CustomAttributes></Metadata>');
MEXlibCZI('MetadataBuilder_SetXml', metadata_builder, xml);

% Convenience operations can now be applied to the replacement document.
MEXlibCZI('MetadataBuilder_Modify', metadata_builder, 'Scaling', struct('X', 0.25e-6));
```

The custom element above illustrates XML editing; recognition of custom fields
depends on the consuming application. For more involved edits, use XML parsing
tools rather than string replacement.

## Writer_WriteMetadata

Writes a complete XML document as the CZI file's document metadata using
`ICziWriter::SyncWriteMetadata`.

```matlab
MEXlibCZI('Writer_WriteMetadata', writer, xml);
```

| argument # | type | Description |
| --- | --- | --- |
| 1 | scalar integer handle | An open writer returned by `CreateCziWriter`. |
| 2 | character array | A complete metadata XML document with an `ImageDocument` root. |

**Outputs:** none. Pass XML, not a builder handle. The XML can come from
`MetadataBuilder_GetXml` or be constructed separately in MATLAB.

XML parsing and the root check occur before writing. They do not validate the
CZI schema, image dimensions, channel descriptions, or other consistency with
the subblocks. The supplied XML is written without merging generated metadata
or adding default display settings.

Only after the write succeeds does the writer disable automatic metadata
generation on close. A failed write does not change that state: if no earlier
write succeeded, automatic metadata generation remains enabled. An earlier
successful write keeps it disabled.

```matlab
xml = MEXlibCZI('MetadataBuilder_GetXml', metadata_builder);
MEXlibCZI('Writer_WriteMetadata', writer, xml);
MEXlibCZI('CloseCziWriter', writer);
```

You must still call [CloseCziWriter](#closecziwriter) to finalize the file.
Subsequent builder edits are not automatically written; retrieve and write the
updated XML explicitly. Writing metadata does not release the builder.

## MetadataBuilder_Release

Releases a metadata builder and invalidates its handle.

```matlab
MEXlibCZI('MetadataBuilder_Release', metadata_builder);
```

| argument # | type | Description |
| --- | --- | --- |
| 1 | scalar integer handle | A live metadata builder handle. |

**Outputs:** none. An unknown or already released handle produces an error.
Release does not close a writer or write metadata. Character arrays previously
returned by `MetadataBuilder_GetXml` remain usable after the builder is released.

Release builders explicitly when finished, including builders superseded by
another preparation call. Any remaining registered builders are released when
the extension shuts down.
