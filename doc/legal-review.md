# Licensing review record

Status: **open; engineering evidence, not legal clearance**.

No linking exception, relicensing, copyright assignment or declaration of legal
compliance is made by the release-materials tooling. An SBOM is an inventory, not
an authorization to redistribute a particular combined work.

## Decisions requiring the maintainer and qualified legal review

1. **GPL/MATLAB combination.** The project carries a GPL version 3 text and links
   its MATLAB MEX to proprietary MATLAB libraries. No MATLAB linking exception
   was found in the reviewed repository. Determine the applicable combined-work
   treatment, host redistribution terms, and whether an appropriate exception or
   different licensing arrangement is necessary. The FSF discusses this class
   of question at https://www.gnu.org/licenses/gpl-faq.en.html#GPLPluginsInNF.
   Do not treat that discussion as an adjudication of this particular project.
2. **Rights and grant policy.** Confirm who owns the original and contributed
   code, including any employer rights, before changing licensing. Decide and
   document GPL-3.0-only versus GPL-3.0-or-later explicitly. Merely shipping the
   GPLv3 text does not resolve every question about the intended grant. Existing
   third-party code cannot be relicensed by changing this project's LICENSE.
3. **Bundled copyleft material and source arrangements.** Review the use of
   static libCZI, its LGPLv3 conditions, Eigen's per-file licenses, and the source,
   rebuild and installation information required for the distributions at hand.
   The source bundle contains the project and bundled library sources, not a
   certification of legally complete Corresponding Source in every context.
4. **Compiler runtimes and proprietary SDKs.** Verify exact runtime licenses,
   exceptions and redistribution terms, including static GCC and MSVC runtime
   use. The provided source bundle excludes general-purpose toolchains, MATLAB,
   Octave installations and OS libraries. It does not make an unsupported
   assertion that every exclusion qualifies as a legal System Library exception.
5. **Source/test data provenance.** Confirm rights to distribute all contributed
   source and test fixtures. Preserve upstream source copyright notices.
6. **Regulatory/contract scope.** Determine whether commercial distribution,
   customer contracts or other applicable rules impose additional SBOM/security
   obligations. A public repository alone does not settle that question.

## Existing releases

The release workflow through v0.6.4-alpha copied binaries into packages without
project license texts, dependency notices, source bundles or SBOMs. Review those
releases and provide missing materials after verifying their exact source and
runtime identities. Current dependency metadata must not be backdated or applied
to older binaries without reconstruction. Retain the original binary hashes;
do not silently replace historical ZIP files and call them the original build.

## Evidence now produced

- Exact resolved project/dependency commits and source identity checks.
- Project license, original dependency license texts and embedded pugixml notice.
- Per-package CycloneDX 1.6 inventory with binary hashes and dependency edges.
- Explicit incomplete-coverage and pending-review declarations.
- Project/dependency source archive and offline rebuild helper.
- Runtime linker output, compiler identity, run URL and checksum manifest.

These outputs make review possible. They do not close the decisions above.
