# Licensing notes for review

The existing GPL version 3 license is unchanged. The generated notices, SBOMs
and source bundles describe the build; they do not certify legal compliance.

Before choosing a new project license, confirm copyright/contributor rights and
applicable employer policy. MIT or BSD-3-Clause are possible directions for code
whose rights holders agree. A change here would not remove libCZI's LGPL terms
or the other dependency obligations.

The current GPL/MATLAB linking arrangement has no explicit exception in this
repo and merits a focused review. The FSF discusses this class of question at
https://www.gnu.org/licenses/gpl-faq.en.html#GPLPluginsInNF. Also confirm Eigen's
file-level licensing and the static compiler-runtime redistribution terms.

The matching source bundle supplies project and bundled dependency sources plus
rebuild instructions. It excludes proprietary hosts and general-purpose tools;
no blanket legal System Library determination is made for those exclusions.

Releases through v0.6.4-alpha lack the new license/SBOM/source materials. Any
backfill needs its own verified build inventory. Existing binary archives must
not be silently replaced or assigned today's inventory as historical evidence.
