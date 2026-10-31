# Release materials

The release workflow prepares licenses, source bundles and a CycloneDX 1.6 SBOM
for each MATLAB/Octave package. This is evidence for legal/security review, not
an assertion that the current licenses permit every host combination. Read
[the open legal review](legal-review.md) before making licensing claims.

The reviewed source inventory is `compliance/dependencies.json`. A changed
dependency commit, dirty dependency checkout or enabled uninventoried optional
feature fails packaging. Update the inventory and review its license evidence
when intentionally changing those inputs.

Each binary package keeps its binaries and a short README.txt at the root.
licenses/ contains just LICENSE and a consolidated THIRD_PARTY_NOTICES.txt.
Identical upstream texts are included once. Linux static-runtime evidence is
appended only to MATLAB notices. Original source-bundle notices remain intact.
Eigen license texts are retained conservatively pending file-level review.
Technical files (sbom.cdx.json, build-info.json, runtime-dependencies.txt and
package.json) are under metadata/. README.txt explains installation and identifies
the matching source archive and hash. Maintainer legal-review notes stay in the
repository. The SBOM hashes the root-level binaries; archive checksums are
published separately to avoid a self-referential ZIP hash.

The inventory includes header-only/static components from build inputs. Runtime
inspection alone cannot reliably identify them. Direct external host/system
libraries are recorded as non-redistributed prerequisites with unknown details
left unknown. The SBOM explicitly declares incomplete host/system coverage.

For each platform there is a matching source archive. The publication job checks
ten unique host/platform packages, a single commit, and matching source hashes;
it publishes ten ZIPs, ten SBOM sidecars, five source archives and SHA256SUMS.
Source archives and packages are also available as CI artifacts before release.
Checksums and build/run metadata are not cryptographic provenance attestations.

To inspect generated materials locally after a normal build, with all four
reviewed dependency checkouts present:

```text
python scripts/release_materials.py prepare --build build --output build/release-materials --platform ubuntu-24.04
python scripts/release_materials.py package --materials build/release-materials --package artifacts/MATLAB-example --host matlab
python -m pip install -r compliance/requirements-validation.txt
python scripts/validate_sbom.py artifacts/MATLAB-example/metadata/sbom.cdx.json
```

Use the actual package directory and platform label. Preparing evidence requires
committed project changes. Untracked build/test files are not included. This
does not reconstruct an inventory for an existing historical binary; that needs
separate source, toolchain and artifact evidence.
