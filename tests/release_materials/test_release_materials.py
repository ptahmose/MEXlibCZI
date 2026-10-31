import argparse
import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest
import zipfile
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "scripts"))
import release_materials as materials
import validate_sbom


class ReleaseMaterialsTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.manifest = json.loads((materials.ROOT / "compliance/dependencies.json").read_text())

    def make_package(self, host="matlab", runtime_notices=False):
        prepared = self.root / "prepared"
        (prepared / "common/licenses").mkdir(parents=True)
        (prepared / "common/licenses/LICENSE").write_text("test license")
        (prepared / "common/licenses/THIRD_PARTY_NOTICES.txt").write_text("test notices")
        # Maintainer-only material must never leak into the archive root.
        (prepared / "common/LEGAL-REVIEW.md").write_text("maintainer notes")
        info = {"projectVersion": "0.6.4-alpha", "projectCommit": "a" * 40,
                "platform": "ubuntu-24.04", "generatedAt": "2026-10-31T00:00:00Z",
                "workflowRun": "https://example.com/run", "reviewStatus": "review pending",
                "inventoryCoverage": "incomplete", "hostVersions": {"matlab": "26.2.0", "octave": "11.3.0"},
                "sources": self.manifest["sources"], "embedded": self.manifest["embedded"],
                "runtimes": [], "sourceArchive": {"name": "source.tar.gz", "sha256": "0" * 64}}
        if runtime_notices:
            (prepared / "runtime-notices").mkdir()
            for name in ("libgcc.a", "libstdc++.a"):
                filename = name + ".copyright"
                (prepared / "runtime-notices" / filename).write_text("Runtime copyright and exception")
                info["runtimes"].append({"name": name, "version": "test", "noticeFile": filename,
                                         "licenseName": "runtime exception", "notes": "fixture"})
        materials.write_json(prepared / "inventory.json", info)
        materials.write_json(prepared / "common/metadata/build-info.json", info)
        package = self.root / "MATLAB-test"
        package.mkdir()
        if host == "matlab":
            (package / "MEXlibCZI.mexa64").write_bytes(b"fixture binary")
        else:
            (package / "octavelibczi.mex").write_bytes(b"fixture wrapper")
            (package / "libmexlibczi.so").write_bytes(b"fixture companion")
        with patch.object(materials, "linked_libraries", return_value="libc.so.6 => /lib/libc.so.6"):
            materials.package(argparse.Namespace(materials=prepared, package=package, host=host))
        return package

    def test_matlab_package_has_clean_root_and_source_readme(self):
        package = self.make_package()
        self.assertEqual({p.name for p in package.iterdir()},
                         {"MEXlibCZI.mexa64", "README.txt", "licenses", "metadata"})
        self.assertTrue((package / "licenses/LICENSE").is_file())
        self.assertTrue((package / "licenses/THIRD_PARTY_NOTICES.txt").is_file())
        self.assertEqual({p.name for p in (package / "licenses").iterdir()},
                         {"LICENSE", "THIRD_PARTY_NOTICES.txt"})
        readme = (package / "README.txt").read_text()
        self.assertIn("MEXlibCZI('GetVersion')", readme)
        self.assertIn("source.tar.gz", readme)
        self.assertIn("SHA-256: " + "0" * 64, readme)

    def test_static_runtime_notices_are_consolidated_once(self):
        package = self.make_package(runtime_notices=True)
        text = (package / "licenses/THIRD_PARTY_NOTICES.txt").read_text()
        self.assertEqual(text.count("Runtime copyright and exception"), 1)
        self.assertIn("Same text as:", text)
        self.assertEqual(len(list((package / "licenses").iterdir())), 2)

    def test_octave_omits_unshipped_static_runtime_notices(self):
        package = self.make_package("octave", runtime_notices=True)
        text = (package / "licenses/THIRD_PARTY_NOTICES.txt").read_text()
        self.assertNotIn("Runtime copyright and exception", text)

    def test_consolidated_notices_preserve_terms_and_deduplicate(self):
        text = materials.consolidated_notices([
            ("First", "Copyright Example\nPermission and disclaimer"),
            ("Second", "Copyright Example\nPermission and disclaimer")])
        self.assertEqual(text.count("Copyright Example"), 1)
        self.assertIn("Same text as: First", text)
        with self.assertRaisesRegex(ValueError, "Empty license evidence"):
            materials.consolidated_notices([("Missing", " ")])

    def test_octave_binaries_stay_together_at_package_root(self):
        package = self.make_package("octave")
        self.assertEqual({p.name for p in package.iterdir()},
                         {"octavelibczi.mex", "libmexlibczi.so", "README.txt", "licenses", "metadata"})
        self.assertIn("octavelibczi('GetVersion')", (package / "README.txt").read_text())
        validate_sbom.validate(package / "metadata/sbom.cdx.json")

    def test_sbom_schema_graph_and_binary_hashes(self):
        package = self.make_package()
        validate_sbom.validate(package / "metadata/sbom.cdx.json")
        bom = json.loads((package / "metadata/sbom.cdx.json").read_text())
        host = next(item for item in bom["components"] if item["bom-ref"] == "host")
        self.assertEqual(host["version"], "26.2.0")
        self.assertEqual(host["scope"], "excluded")

    def test_tampered_binary_is_rejected(self):
        package = self.make_package()
        (package / "MEXlibCZI.mexa64").write_bytes(b"tampered")
        with self.assertRaisesRegex(ValueError, "hash mismatch"):
            validate_sbom.validate(package / "metadata/sbom.cdx.json")

    def test_missing_reviewed_component_is_rejected(self):
        package = self.make_package()
        path = package / "metadata/sbom.cdx.json"
        bom = json.loads(path.read_text())
        bom["components"] = [c for c in bom["components"] if c["bom-ref"] != "pugixml"]
        materials.write_json(path, bom)
        with self.assertRaisesRegex(ValueError, "Missing reviewed"):
            validate_sbom.validate(path)

    def test_false_complete_claim_is_rejected(self):
        package = self.make_package()
        path = package / "metadata/sbom.cdx.json"
        bom = json.loads(path.read_text())
        bom["compositions"][0]["aggregate"] = "complete"
        materials.write_json(path, bom)
        with self.assertRaisesRegex(ValueError, "complete host"):
            validate_sbom.validate(path)

    def test_source_archive_has_stable_metadata(self):
        entries = [("sources/a.txt", b"source", 0o644)]
        first, second = self.root / "first.tar.gz", self.root / "second.tar.gz"
        materials.source_archive(first, entries)
        materials.source_archive(second, entries)
        self.assertEqual(materials.sha256(first), materials.sha256(second))

    def test_unknown_binary_package_contents_are_rejected(self):
        prepared = self.root / "prepared"
        prepared.mkdir()
        materials.write_json(prepared / "inventory.json", {})
        package = self.root / "package"
        package.mkdir()
        (package / "unexpected.txt").write_text("not a binary")
        with self.assertRaisesRegex(ValueError, "Unexpected binary"):
            materials.package(argparse.Namespace(materials=prepared, package=package, host="matlab"))

    def test_release_requires_the_entire_matrix(self):
        artifacts = self.root / "artifacts"
        artifacts.mkdir()
        with self.assertRaisesRegex(ValueError, "ten binary"):
            materials.assemble(argparse.Namespace(artifacts=artifacts, output=self.root / "dist"))

    def test_source_identity_mismatch_is_rejected(self):
        build = self.root / "build"
        build.mkdir()
        (build / "CMakeCache.txt").write_text("")
        with patch.object(materials, "git_commit", return_value="b" * 40):
            with self.assertRaisesRegex(ValueError, "reviewed commit"):
                materials.verify_sources(self.manifest, build)

    def test_uninventoried_feature_is_rejected(self):
        build = self.root / "build"
        build.mkdir()
        (build / "CMakeCache.txt").write_text("LIBCZI_BUILD_CURL_BASED_STREAM:BOOL=ON\n")
        with self.assertRaisesRegex(ValueError, "Uninventoried"):
            materials.verify_sources(self.manifest, build)

    def test_visual_studio_compiler_need_not_be_in_cache(self):
        build = self.root / "build"
        directory = build / "CMakeFiles/4.4.0"
        directory.mkdir(parents=True)
        (directory / "CMakeCXXCompiler.cmake").write_text(
            'set(CMAKE_CXX_COMPILER "C:/VS/cl.exe")\n'
            'set(CMAKE_CXX_COMPILER_ID "MSVC")\n'
            'set(CMAKE_CXX_COMPILER_VERSION "19.51.0")\n')
        compiler, info = materials.compiler_metadata(build, {})
        self.assertEqual(compiler, "C:/VS/cl.exe")
        self.assertEqual(info["CMAKE_CXX_COMPILER_ID"], "MSVC")

    def make_release_set(self):
        artifacts = self.root / "artifacts"
        artifacts.mkdir()
        for platform in ("ubuntu-22.04", "ubuntu-24.04", "ubuntu-26.04", "windows-latest", "macos-15"):
            support = artifacts / ("source-" + platform)
            support.mkdir()
            archive = support / ("source-" + platform + ".tar.gz")
            archive.write_bytes(b"source bundle fixture")
            for host in ("matlab", "octave"):
                package = artifacts / (("MATLAB-" if host == "matlab" else "Octave-") + platform)
                package.mkdir()
                materials.write_json(package / "metadata/package.json", {
                    "host": host, "platform": platform, "projectCommit": "a" * 40,
                    "sourceArchive": {"name": archive.name, "sha256": materials.sha256(archive)}})
                materials.write_json(package / "metadata/sbom.cdx.json", {"fixture": True})
        return artifacts

    def test_release_assembles_checksums_sources_and_sidecars(self):
        artifacts = self.make_release_set()
        output = self.root / "dist"
        with patch.dict(materials.os.environ, {"GITHUB_SHA": "a" * 40}):
            materials.assemble(argparse.Namespace(artifacts=artifacts, output=output))
        self.assertEqual(len(list(output.glob("*.zip"))), 10)
        self.assertEqual(len(list(output.glob("*.cdx.json"))), 10)
        self.assertEqual(len(list(output.glob("*.tar.gz"))), 5)
        self.assertEqual(len((output / "SHA256SUMS").read_text().splitlines()), 25)
        with zipfile.ZipFile(next(output.glob("*.zip"))) as archive:
            self.assertTrue(any(name.endswith("/metadata/sbom.cdx.json") for name in archive.namelist()))

    def test_release_rejects_changed_source_archive(self):
        artifacts = self.make_release_set()
        next(artifacts.glob("source-*/*.tar.gz")).write_bytes(b"tampered")
        with patch.dict(materials.os.environ, {"GITHUB_SHA": "a" * 40}):
            with self.assertRaisesRegex(ValueError, "mismatched source"):
                materials.assemble(argparse.Namespace(artifacts=artifacts, output=self.root / "dist"))

    def test_release_rejects_wrong_commit(self):
        artifacts = self.make_release_set()
        with patch.dict(materials.os.environ, {"GITHUB_SHA": "b" * 40}):
            with self.assertRaisesRegex(ValueError, "release commit"):
                materials.assemble(argparse.Namespace(artifacts=artifacts, output=self.root / "dist"))


if __name__ == "__main__":
    unittest.main()
