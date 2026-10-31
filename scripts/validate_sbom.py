#!/usr/bin/env python3
"""Validate CycloneDX syntax and the package-specific inventory invariants."""
import argparse
import hashlib
import json
import urllib.request
from pathlib import Path

import jsonschema
from referencing import Registry, Resource

ROOT = Path(__file__).resolve().parents[1]
SCHEMA_COMMIT = "55343ba19dee1785acf1ce9191540d5fd7b590db"
SCHEMA_HASHES = {'bom-1.6.schema.json': '3e92dddbc30cf7f6a02b80f0942b1a4cfd4fb1c26f1dfc4310afa9d613cafb93', 'spdx.schema.json': 'baa9d3bd1ed57b6751b0887edead6b5063ff53ff7429cf85d476c6c94af0166e', 'jsf-0.82.schema.json': '8bae002c25e723db7ee1f26afde680ae1a2b1a8f6b4b4b0fd65dc3becb090aae'}


def validate(path):
    directory = ROOT / "build/release-schema"
    directory.mkdir(parents=True, exist_ok=True)
    for name, expected_hash in SCHEMA_HASHES.items():
        target = directory / name
        if not target.is_file() or hashlib.sha256(target.read_bytes()).hexdigest() != expected_hash:
            url = f"https://raw.githubusercontent.com/CycloneDX/specification/{SCHEMA_COMMIT}/schema/{name}"
            with urllib.request.urlopen(url, timeout=60) as response:
                data = response.read()
            if hashlib.sha256(data).hexdigest() != expected_hash:
                raise ValueError("SBOM validation schema hash mismatch")
            target.write_bytes(data)
    registry = Registry()
    for name in ("bom-1.6.schema.json", "spdx.schema.json", "jsf-0.82.schema.json"):
        schema = json.loads((directory / name).read_text(encoding="utf-8"))
        for uri in (schema["$id"], "http://cyclonedx.org/schema/" + name, "https://cyclonedx.org/schema/" + name):
            registry = registry.with_resource(uri, Resource.from_contents(schema))
    schema = json.loads((directory / "bom-1.6.schema.json").read_text(encoding="utf-8"))
    bom = json.loads(path.read_text(encoding="utf-8"))
    jsonschema.Draft7Validator(schema, registry=registry, format_checker=jsonschema.FormatChecker()).validate(bom)
    components = bom["components"]
    refs = [bom["metadata"]["component"]["bom-ref"]] + [c["bom-ref"] for c in components]
    if len(refs) != len(set(refs)):
        raise ValueError("Duplicate SBOM identities")
    if not {"libczi", "rapidjson", "eigen", "zstd", "jxrlib", "pugixml", "md5-derived", "xxhash", "host", "source-archive"} <= set(refs):
        raise ValueError("Missing reviewed components")
    for dependency in bom["dependencies"]:
        if dependency["ref"] not in refs or not set(dependency.get("dependsOn", [])) <= set(refs):
            raise ValueError("Dangling dependency relationship")
    package_root = path.parent.parent
    actual = {p.name for p in package_root.iterdir() if p.is_file() and p.name != "README.txt"}
    inventoried = [c["name"] for c in components
                   if c["type"] == "file" and c.get("scope") != "excluded"]
    if len(inventoried) != len(set(inventoried)) or set(inventoried) != actual:
        raise ValueError("SBOM binary inventory does not match package files")
    for component in components:
        if component["type"] == "file" and component.get("scope") != "excluded":
            if Path(component["name"]).name != component["name"]:
                raise ValueError("Unsafe binary path in SBOM")
            binary = path.parent.parent / component["name"]
            digest = hashlib.sha256(binary.read_bytes()).hexdigest()
            if component["hashes"] != [{"alg": "SHA-256", "content": digest}]:
                raise ValueError("SBOM binary hash mismatch")
    if bom["compositions"][0]["aggregate"] != "incomplete":
        raise ValueError("Do not claim complete host/system coverage")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("sbom", type=Path, nargs="+")
    for item in parser.parse_args().sbom:
        validate(item)
        print("Validated", item)
