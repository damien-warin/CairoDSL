#!/usr/bin/env python3
import argparse
import io
import subprocess
import sys
from pathlib import Path

import cairosvg
import numpy as np
from PIL import Image, ImageChops


def rasterize_svg(svg_bytes: bytes, scale: float = 1.0) -> Image.Image:
    """Convertit un contenu SVG (bytes) en image RGBA via CairoSVG."""
    png_bytes = cairosvg.svg2png(bytestring=svg_bytes, scale=scale)
    return Image.open(io.BytesIO(png_bytes)).convert("RGBA")


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Test de non-régression graphique pour CTest."
    )
    parser.add_argument("binary", type=Path, help="Chemin vers l'exécutable à tester")
    parser.add_argument("input_script", type=Path, help="Script d'entrée (passé sur stdin)")
    parser.add_argument("expected_svg", type=Path, help="Fichier SVG de référence")
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
        help="Dossier où sauvegarder les images en cas d'échec",
    )
    args = parser.parse_args()

    # 1. Résolution des chemins avant de modifier le répertoire de travail (cwd)
    binary_path = args.binary.resolve()
    script_path = args.input_script.resolve()
    expected_svg_path = args.expected_svg.resolve()
    diff_dir_path = args.diff_dir.resolve()
    script_dir = script_path.parent

    if not binary_path.is_file():
        print(f"[ERROR] Binaire introuvable : {binary_path}", file=sys.stderr)
        return 1
    if not script_path.is_file():
        print(f"[ERROR] Script d'entrée introuvable : {script_path}", file=sys.stderr)
        return 1
    if not expected_svg_path.is_file():
        print(f"[ERROR] SVG de référence introuvable : {expected_svg_path}", file=sys.stderr)
        return 1

    # 2. Exécution du binaire depuis le dossier du script (stdin -> stdout)
    input_data = script_path.read_bytes()
    proc = subprocess.run(
        [str(binary_path)],
        input=input_data,
        cwd=script_dir,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    if proc.returncode != 0:
        print(
            f"[FAIL] Le binaire a quitté avec le code {proc.returncode} "
            f"(cwd: {script_dir}).\n"
            f"stderr:\n{proc.stderr.decode('utf-8', errors='replace')}",
            file=sys.stderr,
        )
        return 1

    actual_svg_bytes = proc.stdout
    expected_svg_bytes = expected_svg_path.read_bytes()

    # 3. Rastérisation des deux SVG
    try:
        actual_img = rasterize_svg(actual_svg_bytes, scale=args.scale)
        expected_img = rasterize_svg(expected_svg_bytes, scale=args.scale)
    except Exception as exc:
        print(f"[FAIL] Erreur lors de la rastérisation SVG : {exc}", file=sys.stderr)
        return 1

    # 4. Vérification des dimensions
    if actual_img.size != expected_img.size:
        print(
            f"[FAIL] Dimensions différentes : obtenu {actual_img.size}, "
            f"attendu {expected_img.size}",
            file=sys.stderr,
        )
        return 1

    # 5. Comparaison pixel par pixel avec tolérance
    arr_actual = np.asarray(actual_img, dtype=np.int16)
    arr_expected = np.asarray(expected_img, dtype=np.int16)

    diff = np.abs(arr_actual - arr_expected)
    bad_pixels_mask = np.any(diff > args.channel_tol, axis=-1)

    total_pixels = bad_pixels_mask.size
    bad_pixels_count = int(np.count_nonzero(bad_pixels_mask))
    diff_ratio = bad_pixels_count / total_pixels
    max_channel_diff = int(diff.max())

    if diff_ratio > args.max_diff_ratio:
        diff_dir_path.mkdir(parents=True, exist_ok=True)
        stem = script_path.stem

        actual_path = diff_dir_path / f"{stem}_actual.png"
        expected_path = diff_dir_path / f"{stem}_expected.png"
        diff_path = diff_dir_path / f"{stem}_diff.png"

        actual_img.save(actual_path)
        expected_img.save(expected_path)

        diff_img = ImageChops.difference(
            actual_img.convert("RGB"), expected_img.convert("RGB")
        )
        diff_img = diff_img.point(lambda p: min(255, p * 10))
        diff_img.save(diff_path)

        print(
            f"[FAIL] Régression visuelle détectée pour '{script_path.name}' :\n"
            f"  - Pixels hors tolérance : {bad_pixels_count}/{total_pixels} "
            f"({diff_ratio:.4%}, max autorisé: {args.max_diff_ratio:.4%})\n"
            f"  - Écart max sur un canal : {max_channel_diff} (tolérance: {args.channel_tol})\n"
            f"  - Artefacts sauvegardés dans : {diff_dir_path}/",
            file=sys.stderr,
        )
        return 1

    print(
        f"[OK] {script_path.name} "
        f"(diff: {diff_ratio:.4%}, max_delta: {max_channel_diff})"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
