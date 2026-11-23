#!/usr/bin/env python3
import argparse
import hashlib
import io
import subprocess
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

import cairosvg
import numpy as np
from PIL import Image, ImageChops


def rasterize_svg(svg_bytes: bytes, scale: float = 1.0) -> Image.Image:
    """Convertit un contenu SVG (bytes) en image RGBA via CairoSVG."""
    png_bytes = cairosvg.svg2png(bytestring=svg_bytes, scale=scale)
    return Image.open(io.BytesIO(png_bytes)).convert("RGBA")


def extract_last_stderr_line(stderr_bytes: bytes) -> str:
    """Extrait la dernière ligne non vide de stderr."""
    text = stderr_bytes.decode("utf-8", errors="replace")
    lines = [line.strip() for line in text.splitlines() if line.strip()]
    return lines[-1] if lines else ""


def _strip_ns(tag: str) -> str:
    """Retire le préfixe d'espace de noms Clark '{uri}localname' si présent."""
    if tag.startswith("{"):
        return tag.split("}", 1)[1]
    return tag


def _is_namespace_attr(attr_name: str) -> bool:
    """Identifie les attributs liés aux namespaces / schema instance."""
    local = _strip_ns(attr_name)
    return (
        attr_name == "xmlns"
        or attr_name.startswith("xmlns:")
        or local == "xmlns"
        or local.startswith("xmlns:")
        or local in {"schemaLocation", "noNamespaceSchemaLocation"}
    )


def _normalize_elem(elem: ET.Element) -> dict:
    """Convertit récursivement un élément XML en structure canonique sans xmlns."""
    clean_attribs = {
        _strip_ns(k): v
        for k, v in sorted(elem.attrib.items(), key=lambda item: _strip_ns(item[0]))
        if not _is_namespace_attr(k)
    }
    text = elem.text.strip() if elem.text and elem.text.strip() else None
    tail = elem.tail.strip() if elem.tail and elem.tail.strip() else None
    children = [_normalize_elem(child) for child in elem]

    return {
        "tag": _strip_ns(elem.tag),
        "attrib": clean_attribs,
        "text": text,
        "tail": tail,
        "children": children,
    }


def canonicalize_xml(xml_source: str | bytes) -> tuple[bool, str | dict]:
    """Parse et normalise un fragment/document XML en ignorant xmlns et espaces superflus."""
    if isinstance(xml_source, bytes):
        xml_text = xml_source.decode("utf-8", errors="replace").strip()
    else:
        xml_text = xml_source.strip()

    if not xml_text:
        return False, "Contenu XML vide"

    try:
        root = ET.fromstring(xml_text)
    except ET.ParseError as exc:
        return False, f"XML invalide ({exc}) : {xml_text!r}"

    return True, _normalize_elem(root)


def compare_svg(
    actual_svg_bytes: bytes,
    expected_svg_path: Path,
    script_path: Path,
    channel_tol: int,
    max_diff_ratio: float,
    scale: float,
    diff_dir_path: Path,
) -> tuple[bool, str]:
    """Compare le SVG produit (stdout) avec le SVG de référence."""
    if not actual_svg_bytes.strip():
        return False, "Sortie SVG (stdout) vide"

    try:
        actual_img = rasterize_svg(actual_svg_bytes, scale=scale)
        expected_img = rasterize_svg(expected_svg_path.read_bytes(), scale=scale)
    except Exception as exc:
        return False, f"Erreur lors de la rastérisation SVG : {exc}"

    if actual_img.size != expected_img.size:
        return (
            False,
            f"Dimensions SVG différentes : obtenu {actual_img.size}, "
            f"attendu {expected_img.size}",
        )

    arr_actual = np.asarray(actual_img, dtype=np.int16)
    arr_expected = np.asarray(expected_img, dtype=np.int16)

    diff = np.abs(arr_actual - arr_expected)
    bad_pixels_mask = np.any(diff > channel_tol, axis=-1)

    total_pixels = bad_pixels_mask.size
    bad_pixels_count = int(np.count_nonzero(bad_pixels_mask))
    diff_ratio = bad_pixels_count / total_pixels
    max_channel_diff = int(diff.max())

    if diff_ratio > max_diff_ratio:
        diff_dir_path.mkdir(parents=True, exist_ok=True)
        path_hash = hashlib.md5(str(script_path).encode("utf-8")).hexdigest()[:8]
        prefix = f"{script_path.stem}_{path_hash}"

        actual_path = diff_dir_path / f"{prefix}_actual.png"
        expected_path = diff_dir_path / f"{prefix}_expected.png"
        diff_path = diff_dir_path / f"{prefix}_diff.png"

        actual_img.save(actual_path)
        expected_img.save(expected_path)

        diff_img = ImageChops.difference(
            actual_img.convert("RGB"), expected_img.convert("RGB")
        )
        diff_img = diff_img.point(lambda p: min(255, p * 10))
        diff_img.save(diff_path)

        return (
            False,
            f"Régression visuelle SVG :\n"
            f"  - Pixels hors tolérance : {bad_pixels_count}/{total_pixels} "
            f"({diff_ratio:.4%}, max autorisé: {max_diff_ratio:.4%})\n"
            f"  - Écart max sur un canal : {max_channel_diff} (tolérance: {channel_tol})\n"
            f"  - Artefacts sauvegardés dans : {diff_dir_path}/ ({prefix}_*.png)",
        )

    return True, f"SVG OK (diff: {diff_ratio:.4%}, max_delta: {max_channel_diff})"


def compare_xml(actual_xml_str: str, expected_xml_path: Path) -> tuple[bool, str]:
    """Compare strictement la structure XML normalisée (hors xmlns)."""
    ok_actual, norm_actual = canonicalize_xml(actual_xml_str)
    if not ok_actual:
        return False, f"XML produit invalide : {norm_actual}"

    ok_expected, norm_expected = canonicalize_xml(expected_xml_path.read_bytes())
    if not ok_expected:
        return False, f"XML de référence invalide ({expected_xml_path.name}) : {norm_expected}"

    if norm_actual != norm_expected:
        return (
            False,
            f"Différence XML stricte (référence : {expected_xml_path.name}) :\n"
            f"  - Attendu : {norm_expected}\n"
            f"  - Obtenu  : {norm_actual}",
        )

    return True, f"XML OK ({expected_xml_path.name})"


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Test de non-régression fonctionnel (SVG + XML) pour CTest."
    )
    parser.add_argument("binary", type=Path, help="Chemin vers l'exécutable à tester")
    parser.add_argument(
        "input_script",
        type=Path,
        help="Fichier d'entrée (ex: test_foo.cairo, passé sur stdin)",
    )
    parser.add_argument(
        "--ref-dir",
        type=Path,
        default=None,
        help="Dossier des références (défaut: même dossier que input_script)",
    )
    parser.add_argument(
        "--channel-tol",
        type=int,
        default=2,
        help="Tolérance max d'écart par canal RGBA (0-255, défaut: 2)",
    )
    parser.add_argument(
        "--max-diff-ratio",
        type=float,
        default=0.001,
        help="Proportion max de pixels différents autorisée (défaut: 0.001 = 0.1%%)",
    )
    parser.add_argument(
        "--scale",
        type=float,
        default=1.0,
        help="Facteur d'échelle pour la rastérisation (défaut: 1.0)",
    )
    parser.add_argument(
        "--diff-dir",
        type=Path,
        default=Path("test_diffs"),
        help="Dossier où sauvegarder les images en cas d'échec SVG",
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=30.0,
        help="Timeout d'exécution du binaire en secondes (défaut: 30)",
    )
    args = parser.parse_args()

    binary_path = args.binary.resolve()
    script_path = args.input_script.resolve()
    ref_dir_path = args.ref_dir.resolve() if args.ref_dir else script_path.parent
    diff_dir_path = args.diff_dir.resolve()
    script_dir = script_path.parent

    if not binary_path.is_file():
        print(f"[ERROR] Binaire introuvable : {binary_path}", file=sys.stderr)
        return 1
    if not script_path.is_file():
        print(f"[ERROR] Script d'entrée introuvable : {script_path}", file=sys.stderr)
        return 1

    expected_svg_path = ref_dir_path / f"{script_path.stem}.svg"
    expected_xml_path = ref_dir_path / f"{script_path.stem}.xml"

    has_svg_ref = expected_svg_path.is_file()
    has_xml_ref = expected_xml_path.is_file()

    if not has_svg_ref and not has_xml_ref:
        print(
            f"[ERROR] Aucune référence trouvée pour '{script_path.name}' "
            f"(attendu au moins l'un de : {expected_svg_path} ou {expected_xml_path})",
            file=sys.stderr,
        )
        return 1

    try:
        proc = subprocess.run(
            [str(binary_path)],
            input=script_path.read_bytes(),
            cwd=script_dir,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=args.timeout,
        )
    except subprocess.TimeoutExpired:
        print(
            f"[FAIL] Timeout ({args.timeout}s) dépassé pour '{script_path.name}' "
            f"(cwd: {script_dir}).",
            file=sys.stderr,
        )
        return 1

    if proc.returncode != 0:
        print(
            f"[FAIL] Le binaire a quitté avec le code {proc.returncode} "
            f"(cwd: {script_dir}).\n"
            f"stderr:\n{proc.stderr.decode('utf-8', errors='replace')}",
            file=sys.stderr,
        )
        return 1

    checks: list[tuple[str, bool, str]] = []

    if has_svg_ref:
        ok_svg, msg_svg = compare_svg(
            actual_svg_bytes=proc.stdout,
            expected_svg_path=expected_svg_path,
            script_path=script_path,
            channel_tol=args.channel_tol,
            max_diff_ratio=args.max_diff_ratio,
            scale=args.scale,
            diff_dir_path=diff_dir_path,
        )
        checks.append(("SVG", ok_svg, msg_svg))

    if has_xml_ref:
        actual_xml_line = extract_last_stderr_line(proc.stderr)
        ok_xml, msg_xml = compare_xml(
            actual_xml_str=actual_xml_line,
            expected_xml_path=expected_xml_path,
        )
        checks.append(("XML", ok_xml, msg_xml))

    failures = [(kind, msg) for kind, ok, msg in checks if not ok]
    if failures:
        print(f"[FAIL] Échec pour '{script_path.name}' :", file=sys.stderr)
        for kind, msg in failures:
            print(f"  [{kind}] {msg}", file=sys.stderr)
        return 1

    summary = " | ".join(msg for _, _, msg in checks)
    print(f"[OK] {script_path.name} ({summary})")
    return 0


if __name__ == "__main__":
    sys.exit(main())
