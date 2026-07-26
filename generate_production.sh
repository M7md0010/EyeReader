#!/usr/bin/env bash
# ============================================================================
# generate_production.sh — EyeReader KiCad CLI Production File Generator
# ============================================================================
# Exports schematic PDFs, Gerber files, and drill files from the KiCad
# project in /Hardware for PCB fabrication.
#
# Prerequisites:
#   - KiCad 8.x installed with `kicad-cli` on $PATH
#
# Usage:
#   chmod +x generate_production.sh
#   ./generate_production.sh
# ============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
HARDWARE_DIR="${SCRIPT_DIR}/Hardware"
DOCS_DIR="${SCRIPT_DIR}/Docs"
PRODUCTION_DIR="${SCRIPT_DIR}/Production"

SCHEMATIC="${HARDWARE_DIR}/v1.0.kicad_sch"
PCB="${HARDWARE_DIR}/v1.0.kicad_pcb"

# ---------------------------------------------------------------------------
# Preflight checks
# ---------------------------------------------------------------------------
KICAD_CLI="kicad-cli"
if ! command -v "${KICAD_CLI}" &>/dev/null; then
    # Check standard Windows install paths
    for ver in 10.0 9.0 8.0; do
        win_path="/c/Program Files/KiCad/${ver}/bin/kicad-cli.exe"
        if [[ -f "${win_path}" ]]; then
            KICAD_CLI="${win_path}"
            break
        fi
    done
    if ! command -v "${KICAD_CLI}" &>/dev/null && [[ ! -f "${KICAD_CLI}" ]]; then
        echo "ERROR: kicad-cli not found on PATH or in standard install directories."
        echo "       Please install KiCad 8.x+ and ensure kicad-cli is accessible."
        exit 1
    fi
fi
echo "Using kicad-cli: ${KICAD_CLI}"

if [[ ! -f "${SCHEMATIC}" ]]; then
    echo "ERROR: Schematic not found at ${SCHEMATIC}"
    exit 1
fi

if [[ ! -f "${PCB}" ]]; then
    echo "ERROR: PCB file not found at ${PCB}"
    exit 1
fi

# ---------------------------------------------------------------------------
# Ensure output directories exist
# ---------------------------------------------------------------------------
mkdir -p "${DOCS_DIR}"
mkdir -p "${PRODUCTION_DIR}"

# ---------------------------------------------------------------------------
# 1. Export Schematic PDF → /Docs
# ---------------------------------------------------------------------------
echo "==> Exporting schematic PDF..."
"${KICAD_CLI}" sch export pdf \
    --output "${DOCS_DIR}/EyeReader_Schematic.pdf" \
    "${SCHEMATIC}"
echo "    ✓ Schematic PDF saved to ${DOCS_DIR}/EyeReader_Schematic.pdf"

# ---------------------------------------------------------------------------
# 2. Export Gerber Files → /Production
# ---------------------------------------------------------------------------
echo "==> Exporting Gerber files..."
"${KICAD_CLI}" pcb export gerbers \
    --output "${PRODUCTION_DIR}/" \
    "${PCB}"
echo "    ✓ Gerber files saved to ${PRODUCTION_DIR}/"

# ---------------------------------------------------------------------------
# 3. Export Drill Files → /Production
# ---------------------------------------------------------------------------
echo "==> Exporting drill files..."
"${KICAD_CLI}" pcb export drill \
    --output "${PRODUCTION_DIR}/" \
    "${PCB}"
echo "    ✓ Drill files saved to ${PRODUCTION_DIR}/"

# ---------------------------------------------------------------------------
echo ""
echo "=========================================="
echo " Production export complete."
echo "   Docs:       ${DOCS_DIR}/"
echo "   Production: ${PRODUCTION_DIR}/"
echo "=========================================="
