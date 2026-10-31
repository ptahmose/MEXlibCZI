#!/usr/bin/env python3
"""Prepare evidence-backed release materials; never infer legal clearance."""
import argparse
import datetime as dt
import gzip
import hashlib
import io
import json
import os
from pathlib import Path, PurePosixPath
import re
import shutil
import subprocess
import tarfile
import uuid
import zipfile
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]


def run(*args, cwd=None):
    return subprocess.check_output(args, cwd=cwd, text=True, encoding="utf-8", errors="replace").strip()


def write_json(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")


def sha256(path):
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def git_commit(path):
    return run("git", "rev-parse", "HEAD", cwd=path)


def read_cache(build):
    cache = {}
    for line in (build / "CMakeCache.txt").read_text(encoding="utf-8").splitlines():
        match = re.match(r"([^/#][^:]*):[^=]*=(.*)", line)
        if match:
            cache[match[1]] = match[2]
    return cache


def version():
    text = (ROOT / "CMakeLists.txt").read_text(encoding="utf-8-sig")
    values = {}
    for key in ("MAJOR", "MINOR", "PATCH", "EXT"):
        values[key] = re.search(r"set\(MEXLIBCZI_" + key + r'\s+"?([^\s"\)]+)', text)[1]
    return ".".join(values[key] for key in ("MAJOR", "MINOR", "PATCH")) + "-" + values["EXT"]


def property_(name, value):
    return {"name": "mexlibczi:" + name, "value": str(value)}


def verify_sources(manifest, build):
    cache = read_cache(build)
    if cache.get("BUILD_SHARED_LIBS", "OFF").upper() not in ("OFF", "FALSE", "0", ""):
        raise ValueError("Uninventoried shared-core configuration")
    for option in manifest["excludedFeatures"]:
        if cache.get(option, "OFF").upper() not in ("OFF", "FALSE", "0", ""):
            raise ValueError(f"Uninventoried configuration: {option}={cache[option]}")
    sources = {}
    for item in manifest["sources"]:
        path = build / item["buildPath"]
        if git_commit(path) != item["commit"]:
            raise ValueError(f"{item['id']}: resolved source does not match reviewed commit")
        # Tracked changes would invalidate both license evidence and source identity.
        if run("git", "status", "--porcelain", "--untracked-files=no", cwd=path):
            raise ValueError(f"{item['id']}: modified source requires an inventory review")
        sources[item["id"]] = path
    return sources, cache


def archive_entries(repo, ref, prefix, source_paths=None):
    command = ["git", "archive", "--format=tar", ref]
    if source_paths:
        command += ["--", *source_paths]
    data = subprocess.check_output(command, cwd=repo)
    with tarfile.open(fileobj=io.BytesIO(data)) as archive:
        for member in archive.getmembers():
            name = PurePosixPath(member.name)
            if name.is_absolute() or ".." in name.parts:
                raise ValueError(f"Unsafe source path: {name}")
            if not member.isfile():
                if member.isdir():
                    continue
                raise ValueError(f"Non-regular source entry requires review: {name}")
            yield str(PurePosixPath(prefix) / name), archive.extractfile(member).read(), member.mode


def source_hash(repo, item):
    command = ["git", "archive", "--format=tar", item["commit"]]
    if item.get("sourcePaths"):
        command += ["--", *item["sourcePaths"]]
    return hashlib.sha256(subprocess.check_output(command, cwd=repo)).hexdigest()


def source_archive(destination, entries):
    # Stable metadata makes hashes independent of the checkout path/mtime.
    with destination.open("wb") as raw:
        with gzip.GzipFile(fileobj=raw, mode="wb", filename="", mtime=0) as compressed:
            with tarfile.open(fileobj=compressed, mode="w") as archive:
                for name, data, mode in entries:
                    info = tarfile.TarInfo(name)
                    info.size, info.mode, info.mtime = len(data), mode, 0
                    archive.addfile(info, io.BytesIO(data))


def consolidated_notices(sections):
    """Keep complete evidence, but include identical texts only once."""
    seen, blocks = {}, []
    for label, text in sections:
        text = text.strip()
        if not text:
            raise ValueError(f"Empty license evidence: {label}")
        key = " ".join(text.split())
        if key in seen:
            blocks.append(f"{label}\nSame text as: {seen[key]}\n")
        else:
            seen[key] = label
            blocks.append(f"{label}\n{'=' * len(label)}\n{text}\n")
    return "\n".join(blocks)


def copy_notices(manifest, sources, output):
    licenses = output / "licenses"
    licenses.mkdir()
    shutil.copyfile(ROOT / "LICENSE", licenses / "LICENSE")
    sections = [("About these notices",
                 "Matching source and rebuild instructions are identified in README.txt. "
                 "Original upstream notices remain in the source bundle. "
                 "Eigen's mixed source-tree license texts are retained conservatively; "
                 "their inclusion does not mean every text applies to this binary. "
                 "Licensing review remains pending; see metadata/build-info.json.")]
    def read(component, path):
        return (sources[component] / path).read_text(encoding="utf-8-sig")
    sections.append(("libCZI and MD5-derived code: Carl Zeiss Microscopy GmbH; LGPL-3.0-or-later",
                     read("libczi", "LICENSES/LGPL-3.0-or-later.txt")))
    jxr = read("libczi", "Src/JxrDecode/jxrlib/image/sys/strcodec.c")
    sections.append(("jxrlib: Microsoft BSD-2-Clause notice", jxr[:jxr.index("//*@@@---")]))
    pugi = read("libczi", "Src/libCZI/pugixml.cpp")
    notice = pugi[pugi.rfind("/**"):].strip()
    if "2006-2025" not in notice or "Permission is hereby granted" not in notice:
        raise ValueError("pugixml license evidence changed; review it")
    sections.append(("pugixml 1.15: embedded source notice", notice))
    rapid = read("rapidjson", "license.txt")
    # JSON_checker is excluded from the source bundle and release build.
    # Retain msinttypes' BSD notice conservatively for the shipped headers.
    bsd = rapid[rapid.index("The msinttypes r29"):rapid.index("Open Source Software Licensed Under the JSON License:")]
    mit = rapid[rapid.index("Terms of the MIT License:"):]
    sections.append(("RapidJSON", "Copyright (C) 2015 THL A29 Limited, a Tencent company, and Milo Yip. All rights reserved.\n\n" + mit))
    sections.append(("RapidJSON msinttypes headers", bsd))
    sections.append(("Zstandard and its adapted xxHash: BSD license alternative", read("zstd", "LICENSE")))
    xxhash = read("zstd", "lib/common/xxhash.h")
    notice = xxhash[:xxhash.index("*/") + 2]
    if "Copyright (c) Yann Collet - Meta Platforms, Inc" not in notice:
        raise ValueError("xxHash copyright evidence changed; review it")
    sections.append(("xxHash: embedded source notice (BSD alternative selected)", notice))
    for path in sorted(sources["eigen"].glob("COPYING.*")):
        sections.append(("Eigen: " + path.name, path.read_text(encoding="utf-8-sig")))
    if not list(sources["eigen"].glob("COPYING.*")):
        raise ValueError("Missing Eigen license evidence")
    (licenses / "THIRD_PARTY_NOTICES.txt").write_text(consolidated_notices(sections), encoding="utf-8")


def runtime_inventory(compiler, platform, output):
    """Preserve runtime evidence, not an unsupported blanket license assertion."""
    result = []
    if platform.startswith("ubuntu-"):
        for library in ("libstdc++.a", "libgcc.a"):
            path = Path(run(compiler, "-print-file-name=" + library)).resolve()
            if not path.is_file():
                raise ValueError(f"Cannot identify static compiler runtime: {library}")
            owner = run("dpkg-query", "-S", str(path)).split(": ", 1)[0]
            package_version = run("dpkg-query", "-W", "-f=${Version}", owner)
            copyright_path = Path("/usr/share/doc") / owner.split(":", 1)[0] / "copyright"
            if not copyright_path.is_file():
                raise ValueError(f"Missing runtime copyright file: {copyright_path}")
            target = output / "runtime-notices" / (library + ".copyright")
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(copyright_path, target)
            result.append({"name": library, "version": package_version, "package": owner,
                           "noticeFile": target.name, "sha256": sha256(path), "licenseName": "GCC runtime; see packaged copyright and exceptions",
                           "notes": "Statically linked into MATLAB MEX; Octave companion normally uses shared runtime. Source/exception treatment requires review."})
    elif platform.startswith("windows"):
        result.append({"name": "Microsoft Visual C++ static runtime", "version": "toolchain version in metadata/build-info.json",
                       "licenseName": "Microsoft Visual Studio runtime redistribution terms (review pending)",
                       "notes": "Project selects /MT. No Microsoft SDK/runtime source or license grant is invented by this inventory."})
    return result


def compiler_metadata(build, cache):
    files = sorted((build / "CMakeFiles").glob("*/CMakeCXXCompiler.cmake"))
    if not files:
        raise ValueError("Missing resolved CMake compiler metadata")
    text = files[-1].read_text(encoding="utf-8")
    info = {}
    for key in ("CMAKE_CXX_COMPILER_ID", "CMAKE_CXX_COMPILER_VERSION"):
        info[key] = re.search(r'set\(' + key + r' "([^\"]*)"\)', text)[1]
    compiler = cache.get("CMAKE_CXX_COMPILER")
    if not compiler:
        compiler = re.search(r'set\(CMAKE_CXX_COMPILER "([^\"]*)"\)', text)[1]
    return compiler, info


def prepare(args):
    manifest = json.loads((ROOT / "compliance/dependencies.json").read_text(encoding="utf-8"))
    build, output = Path(args.build).resolve(), Path(args.output).resolve()
    if output.exists():
        raise ValueError("Use a fresh release-materials output directory")
    sources, cache = verify_sources(manifest, build)
    commit = git_commit(ROOT)
    if run("git", "status", "--porcelain", "--untracked-files=no", cwd=ROOT):
        raise ValueError("Commit tracked project changes before preparing release materials")
    output.mkdir(parents=True)
    common = output / "common"
    common.mkdir()
    copy_notices(manifest, sources, common)
    compiler, compiler_info = compiler_metadata(build, cache)
    info = {"projectVersion": version(), "projectCommit": commit, "platform": args.platform,
            "generatedAt": dt.datetime.now(dt.timezone.utc).isoformat().replace("+00:00", "Z"),
            "workflowRun": os.getenv("GITHUB_SERVER_URL", "https://github.com") + "/" + os.getenv("GITHUB_REPOSITORY", "ptahmose/MEXlibCZI") + "/actions/runs/" + os.getenv("GITHUB_RUN_ID", "local"),
            "compiler": compiler_info, "reviewStatus": manifest["reviewStatus"],
            "openReviewItems": manifest["openReviewItems"],
            "inventoryCoverage": "Reviewed bundled sources, packaged files, compiler-runtime evidence and observed runtime-linker output; transitive host/system inventories are incomplete.",
            "runtimes": runtime_inventory(compiler, args.platform, output)}
    matlab_xml = Path(cache["Matlab_ROOT_DIR"]) / "VersionInfo.xml"
    matlab_version = ET.parse(matlab_xml).findtext(".//version")
    octave_version = (build / "octave-version.txt").read_text(encoding="utf-8").strip()
    if not matlab_version or not octave_version:
        raise ValueError("Missing actual host versions")
    info["hostVersions"] = {"matlab": matlab_version, "octave": octave_version}
    archive_name = f"MEXlibCZI-source-{args.platform}-{commit[:7]}.tar.gz"
    prefix = archive_name[:-7]
    entries = list(archive_entries(ROOT, commit, prefix + "/project"))
    for item in manifest["sources"]:
        item["sourceHash"] = source_hash(sources[item["id"]], item)
        entries.extend(archive_entries(sources[item["id"]], item["commit"], prefix + "/deps/" + item["id"], item.get("sourcePaths")))
    # Include a configure helper at the bundle root; it performs no downloads.
    entries.append((prefix + "/rebuild.py", (ROOT / "scripts/rebuild_source_bundle.py").read_bytes(), 0o644))
    entries.append((prefix + "/README.md", (ROOT / "doc/source-bundle.md").read_bytes(), 0o644))
    entries.append((prefix + "/build-info.json", (json.dumps(info, indent=2) + "\n").encode(), 0o644))
    source_archive(output / archive_name, entries)
    info["sourceArchive"] = {"name": archive_name, "sha256": sha256(output / archive_name)}
    info["sources"] = manifest["sources"]
    info["embedded"] = manifest["embedded"]
    write_json(output / "inventory.json", info)
    write_json(common / "metadata/build-info.json", info)


def component(item):
    result = {"type": "library", "bom-ref": item["id"], "name": item["name"], "version": item["version"],
              "properties": [property_("relationship", item["relationship"])]}
    if "licenseExpression" in item:
        result["licenses"] = [{"expression": item["licenseExpression"]}]
    else:
        result["licenses"] = [{"license": {"name": item["licenseName"]}}]
    if "repository" in item:
        result["externalReferences"] = [{"type": "vcs", "url": item["repository"]}]
        result["properties"].append(property_("source-commit", item["commit"]))
    if "sourceHash" in item:
        result["hashes"] = [{"alg": "SHA-256", "content": item["sourceHash"]}]
        result["properties"].append(property_("hash-subject", "uncompressed git archive tar of reviewed source paths at the recorded commit"))
    for key in ("notes", "evidence", "path"):
        if key in item:
            result["properties"].append(property_(key, item[key]))
    return result


def linked_libraries(binary, info):
    if info["platform"].startswith("ubuntu-"):
        return run("readelf", "-d", str(binary))
    if info["platform"].startswith("macos-"):
        return run("otool", "-L", str(binary))
    # VS tools are not necessarily on PATH in GitHub's bash shell.
    vswhere = Path(os.environ.get("ProgramFiles(x86)", "C:/Program Files (x86)")) / "Microsoft Visual Studio/Installer/vswhere.exe"
    install = run(str(vswhere), "-latest", "-products", "*", "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-property", "installationPath")
    tools = sorted((Path(install) / "VC/Tools/MSVC").glob("*/bin/Hostx64/x64/dumpbin.exe"))
    if not tools:
        raise ValueError("Cannot inspect Windows runtime dependencies")
    return run(str(tools[-1]), "/DEPENDENTS", str(binary))


def package(args):
    materials, package_dir = Path(args.materials).resolve(), Path(args.package).resolve()
    info = json.loads((materials / "inventory.json").read_text(encoding="utf-8"))
    binaries = sorted(p for p in package_dir.iterdir() if p.is_file())
    expected = 1 if args.host == "matlab" else 2
    if len(binaries) != expected or any(p.suffix not in (".mexa64", ".mexw64", ".mexmaca64", ".mex", ".so", ".dll", ".dylib") for p in binaries):
        raise ValueError("Unexpected binary package contents")
    for directory in ("licenses", "metadata"):
        shutil.copytree(materials / "common" / directory, package_dir / directory)
    # Runtime evidence belongs only to packages that include that static runtime.
    runtime_sections = []
    if args.host == "matlab":
        for runtime in info["runtimes"]:
            if "noticeFile" in runtime:
                path = materials / "runtime-notices" / runtime["noticeFile"]
                runtime_sections.append((runtime["name"] + " runtime copyright and exceptions",
                                         path.read_text(encoding="utf-8")))
    if runtime_sections:
        with (package_dir / "licenses/THIRD_PARTY_NOTICES.txt").open("a", encoding="utf-8") as stream:
            stream.write("\n" + consolidated_notices(runtime_sections))
    metadata_dir = package_dir / "metadata"
    metadata_dir.mkdir(exist_ok=True)
    source = info["sourceArchive"]
    if args.host == "matlab":
        installation = "Add this folder to the MATLAB path.\nCheck installation: info = MEXlibCZI('GetVersion');"
    else:
        installation = ("Keep octavelibczi.mex and its companion library together in this folder.\n"
                        "Add this folder to the Octave path.\nCheck installation: info = octavelibczi('GetVersion');")
    (package_dir / "README.txt").write_text(
        f"MEXlibCZI {info['projectVersion']} for {args.host.upper()}\n\n"
        "INSTALLATION\nExtract the complete archive into a folder.\n" + installation + "\n\n"
        "LICENSES AND BUILD INFORMATION\n"
        "Project license: licenses/LICENSE\nThird-party notices: licenses/THIRD_PARTY_NOTICES.txt\n"
        "SBOM, build information and runtime dependencies: metadata/\n\n"
        f"MATCHING SOURCE\nArchive: {source['name']}\nSHA-256: {source['sha256']}\n"
        f"Release: https://github.com/ptahmose/MEXlibCZI/releases/tag/v{info['projectVersion']}\n"
        f"For CI builds, download the matching source artifact from: {info['workflowRun']}\n"
        "The source bundle includes project/dependency sources and rebuild instructions.\n", encoding="utf-8")
    evidence = "\n\n".join(p.name + "\n" + linked_libraries(p, info) for p in binaries)
    (metadata_dir / "runtime-dependencies.txt").write_text(evidence + "\n", encoding="utf-8")
    root_ref = "mexlibczi-package"
    root = {"type": "application", "bom-ref": root_ref, "name": package_dir.name,
            "version": info["projectVersion"], "licenses": [{"license": {"name": "GNU GPL version 3; see licenses/LICENSE"}}],
            "properties": [property_("project-commit", info["projectCommit"]), property_("platform", info["platform"]),
                           property_("host", args.host), property_("inventory-coverage", info["inventoryCoverage"])]}
    components = [component(item) for item in info["sources"] + info["embedded"]]
    dependencies = [{"ref": root_ref, "dependsOn": ["libczi", "rapidjson", "host"]},
                    {"ref": "libczi", "dependsOn": ["jxrlib", "pugixml", "md5-derived", "eigen", "zstd"]},
                    {"ref": "zstd", "dependsOn": ["xxhash"]}]
    host = {"type": "application", "bom-ref": "host", "name": "MATLAB" if args.host == "matlab" else "GNU Octave",
            "version": info["hostVersions"][args.host],
            "scope": "required", "description": "User-provided host; not distributed in this package. Exact installation evidence is recorded by the CI log and metadata/runtime-dependencies.txt."}
    components.append(host)
    for index, runtime in enumerate(info["runtimes"] if args.host == "matlab" or info["platform"].startswith("windows") else []):
        ref = f"compiler-runtime-{index}"
        comp = {"type": "library", "bom-ref": ref, "name": runtime["name"], "version": runtime["version"],
                "licenses": [{"license": {"name": runtime["licenseName"]}}],
                "properties": [property_("review-notes", runtime["notes"])]}
        if "sha256" in runtime:
            comp["hashes"] = [{"alg": "SHA-256", "content": runtime["sha256"]}]
            comp["properties"].append(property_("hash-subject", "toolchain archive, not the extracted linked subset"))
        components.append(comp)
        dependencies[0]["dependsOn"].append(ref)
    # Represent observed direct dynamic dependencies separately from shipped code.
    dynamic_names = sorted(set(re.findall(r"(?:[\w.+@/-]+\.so(?:\.[\d]+)*|[\w.+@/-]+\.dylib|[\w.+-]+\.dll)", evidence, re.I)))
    dynamic_names = [name for name in dynamic_names if PurePosixPath(name).name.lower() not in {p.name.lower() for p in binaries}]
    for index, name in enumerate(dynamic_names):
        ref = f"runtime-prerequisite-{index}"
        components.append({"type": "library", "bom-ref": ref, "name": name, "scope": "required",
                           "description": "Observed runtime prerequisite from linker inspection; not shipped here. Transitive relationships, versions and licenses are not inferred from a filename."})
        dependencies[0]["dependsOn"].append(ref)
    for index, binary in enumerate(binaries):
        ref = f"binary-{index}"
        components.append({"type": "file", "bom-ref": ref, "name": binary.name,
                           "hashes": [{"alg": "SHA-256", "content": sha256(binary)}]})
        dependencies[0]["dependsOn"].append(ref)
    source = info["sourceArchive"]
    components.append({"type": "file", "bom-ref": "source-archive", "name": source["name"],
                       "scope": "excluded", "hashes": [{"alg": "SHA-256", "content": source["sha256"]}],
                       "description": "Companion project/dependency source bundle, published separately from this binary package."})
    bom = {"bomFormat": "CycloneDX", "specVersion": "1.6", "serialNumber": "urn:uuid:" + str(uuid.uuid4()), "version": 1,
           "metadata": {"timestamp": info["generatedAt"], "component": root,
                        "tools": {"components": [{"type": "application", "name": "MEXlibCZI release_materials.py", "version": info["projectCommit"]}]},
                        "properties": [property_("workflow-run", info["workflowRun"]), property_("generation-context", "build inputs and packaged binaries"),
                                       property_("review-status", info["reviewStatus"])]},
           "components": components, "dependencies": dependencies,
           "compositions": [{"aggregate": "incomplete", "assemblies": [root_ref]}]}
    write_json(metadata_dir / "sbom.cdx.json", bom)
    write_json(metadata_dir / "package.json", {"host": args.host, "platform": info["platform"], "sourceArchive": source,
               "projectCommit": info["projectCommit"], "version": info["projectVersion"]})


def assemble(args):
    artifacts, output = Path(args.artifacts).resolve(), Path(args.output).resolve()
    if output.exists():
        raise ValueError("Use a fresh release output directory")
    packages = sorted(p for p in artifacts.iterdir() if p.is_dir() and p.name.startswith(("MATLAB-", "Octave-")))
    if len(packages) != 10:
        raise ValueError(f"Expected ten binary packages, found {len(packages)}")
    identities = set()
    sources_by_platform = {}
    commits = set()
    output.mkdir(parents=True)
    for package_dir in packages:
        metadata = json.loads((package_dir / "metadata/package.json").read_text(encoding="utf-8"))
        identity = metadata["host"], metadata["platform"]
        if identity in identities:
            raise ValueError("Duplicate host/platform package")
        identities.add(identity)
        commits.add(metadata["projectCommit"])
        if metadata["projectCommit"] != os.environ.get("GITHUB_SHA", metadata["projectCommit"]):
            raise ValueError("Package does not match release commit")
        source = metadata["sourceArchive"]
        previous = sources_by_platform.setdefault(metadata["platform"], source)
        if previous != source:
            raise ValueError("Hosts on one platform must reference the same source archive")
        matches = list(artifacts.glob("source-*/" + source["name"]))
        if len(matches) != 1 or sha256(matches[0]) != source["sha256"]:
            raise ValueError("Missing or mismatched source archive")
        shutil.copyfile(matches[0], output / source["name"])
        with zipfile.ZipFile(output / (package_dir.name + ".zip"), "w", zipfile.ZIP_DEFLATED) as archive:
            for path in sorted(package_dir.rglob("*")):
                if path.is_symlink():
                    raise ValueError("Unexpected package symlink")
                if path.is_file():
                    archive.write(path, path.relative_to(package_dir.parent).as_posix())
        shutil.copyfile(package_dir / "metadata/sbom.cdx.json", output / (package_dir.name + ".cdx.json"))
    expected = {(host, platform) for host in ("matlab", "octave") for platform in ("ubuntu-22.04", "ubuntu-24.04", "ubuntu-26.04", "windows-latest", "macos-15")}
    if identities != expected or len(commits) != 1:
        raise ValueError("Release package matrix or commit identity mismatch")
    if len({source["name"] for source in sources_by_platform.values()}) != 5:
        raise ValueError("Expected five distinct platform source archives")
    (output / "SHA256SUMS").write_text("".join(f"{sha256(path)}  {path.name}\n" for path in sorted(output.iterdir())), encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    prep = sub.add_parser("prepare")
    prep.add_argument("--build", required=True)
    prep.add_argument("--output", required=True)
    prep.add_argument("--platform", required=True)
    pkg = sub.add_parser("package")
    pkg.add_argument("--materials", required=True)
    pkg.add_argument("--package", required=True)
    pkg.add_argument("--host", choices=("matlab", "octave"), required=True)
    release = sub.add_parser("assemble")
    release.add_argument("--artifacts", required=True)
    release.add_argument("--output", required=True)
    args = parser.parse_args()
    {"prepare": prepare, "package": package, "assemble": assemble}[args.command](args)


if __name__ == "__main__":
    main()
