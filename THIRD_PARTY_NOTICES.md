# Third-party software in MEXlibCZI

This document describes the reviewed release configuration. It does not grant
additional rights or conclude that all combinations with proprietary hosts are
permitted. The project's existing GPL version 3 license remains unchanged.

`compliance/dependencies.json` records the source repositories, exact commits,
build paths, license evidence, excluded features and unresolved review items.
Generated binary packages contain the original texts under `licenses/` and the
project `LICENSE`. See `LEGAL-REVIEW.md` in a package or `doc/legal-review.md` here.

| Component | Release use | Upstream licensing evidence |
| --- | --- | --- |
| libCZI 0.71.0 | Static library | LGPL-3.0-or-later; COPYING and LICENSES |
| jxrlib, modified within libCZI | Embedded JPEG XR code | BSD-2-Clause; Microsoft copyright/source headers |
| pugixml 1.15, modified within libCZI | Embedded XML parser | MIT; embedded source version and license block |
| MD5 implementation, modified within libCZI | Embedded source | LGPL-3.0-or-later SPDX headers; original derivation described as Unlicense in MD5Sum.h |
| Eigen 3.4.0 | Header-only dependency of libCZI | Primarily MPL-2.0; mixed source-tree licenses in COPYING.README and COPYING.*; file-level conclusion pending |
| Zstandard 1.5.7 | Static library | BSD-3-Clause OR GPL-2.0-only; LICENSE and COPYING |
| xxHash 0.8.2, adapted within Zstandard | Embedded checksum code | Adapted header refers to Zstandard's BSD/GPL dual license |
| RapidJSON, pinned commit in manifest | Header-only dependency | MIT; original license.txt retained with its third-party disclosures |

libCZI's aggregate notice still lists pugixml 1.13. Its actual embedded headers
identify 1.15. Packages preserve the aggregate notice and also reproduce the
current embedded source's MIT notice and copyright.

The RapidJSON source bundle contains `include/` and `license.txt`. The unused
`bin/jsonchecker/` directory (JSON license) and test dependencies are excluded.
Its original license.txt is reproduced intact, including disclosures about
material not shipped in the binary. This is not an assertion that every license
mentioned in that file applies to the MEX binary.

Compiler runtimes require separate treatment. MATLAB Linux builds select static
libgcc/libstdc++; Windows builds select the static MSVC runtime. Packages retain
available runtime license evidence and flag unresolved toolchain licensing.
Host and OS libraries observed by ldd/otool/dumpbin are external prerequisites,
not files redistributed by this project. Their full transitive inventories are
not asserted by the generated SBOM.

CycloneDX schemas are validation-only inputs downloaded to an ignored build
cache from a fixed upstream commit with verified SHA-256 hashes. They are not
linked into or redistributed with the binaries.
