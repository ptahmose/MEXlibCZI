#!/usr/bin/env python3
"""Validate CycloneDX syntax and the package-specific inventory invariants."""
import argparse
import hashlib
import json
from pathlib import Path

import jsonschema
from referencing import Registry, Resource

ROOT = Path(__file__).resolve().parents[1]


def validate(path):
    directory = ROOT / "compliance/schema"
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
    for component in components:
        if component["type"] == "file" and component.get("scope") != "excluded":
            if Path(component["name"]).name != component["name"]:
                raise ValueError("Unsafe binary path in SBOM")
            binary = path.parent / component["name"]
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
