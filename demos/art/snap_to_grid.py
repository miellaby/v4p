"""
SVG Snap-to-Grid
Converts all path commands to absolute coordinates, then snaps every vertex to the nearest grid unit.
Usage: python snap_to_grid.py input.svg output.svg [grid_size]
"""

import re
import sys
from xml.etree import ElementTree as ET

def snap(v, grid):
    return round(round(v / grid) * grid, 6)

def parse_numbers(s):
    return [float(x) for x in re.findall(r'[-+]?(?:\d+\.?\d*|\.\d+)(?:[eE][-+]?\d+)?', s)]

def tokenize_path(d):
    """Split path data into (command, [numbers]) tokens."""
    tokens = []
    for m in re.finditer(r'([MmZzLlHhVvCcSsQqTtAa])((?:[^MmZzLlHhVvCcSsQqTtAa])*)', d):
        cmd = m.group(1)
        nums = parse_numbers(m.group(2))
        tokens.append((cmd, nums))
    return tokens

def to_absolute(tokens):
    """Convert all relative commands to absolute, returning list of (CMD, [numbers])."""
    result = []
    cx, cy = 0.0, 0.0  # current point
    sx, sy = 0.0, 0.0  # start of current subpath (for Z)

    for cmd, nums in tokens:
        if cmd == 'M':
            pts = list(zip(nums[0::2], nums[1::2]))
            result.append(('M', list(pts[0])))
            cx, cy = pts[0]
            sx, sy = cx, cy
            for p in pts[1:]:
                result.append(('L', list(p)))
                cx, cy = p
        elif cmd == 'm':
            pts = list(zip(nums[0::2], nums[1::2]))
            cx, cy = cx + pts[0][0], cy + pts[0][1]
            result.append(('M', [cx, cy]))
            sx, sy = cx, cy
            for p in pts[1:]:
                cx, cy = cx + p[0], cy + p[1]
                result.append(('L', [cx, cy]))
        elif cmd == 'L':
            pts = list(zip(nums[0::2], nums[1::2]))
            for p in pts:
                result.append(('L', list(p)))
                cx, cy = p
        elif cmd == 'l':
            pts = list(zip(nums[0::2], nums[1::2]))
            for p in pts:
                cx, cy = cx + p[0], cy + p[1]
                result.append(('L', [cx, cy]))
        elif cmd == 'H':
            for x in nums:
                cx = x
                result.append(('L', [cx, cy]))
        elif cmd == 'h':
            for x in nums:
                cx += x
                result.append(('L', [cx, cy]))
        elif cmd == 'V':
            for y in nums:
                cy = y
                result.append(('L', [cx, cy]))
        elif cmd == 'v':
            for y in nums:
                cy += y
                result.append(('L', [cx, cy]))
        elif cmd == 'C':
            pts = list(zip(nums[0::2], nums[1::2]))
            for i in range(0, len(pts), 3):
                c1, c2, p = pts[i], pts[i+1], pts[i+2]
                result.append(('C', [c1[0], c1[1], c2[0], c2[1], p[0], p[1]]))
                cx, cy = p
        elif cmd == 'c':
            pts = list(zip(nums[0::2], nums[1::2]))
            for i in range(0, len(pts), 3):
                c1 = (cx + pts[i][0], cy + pts[i][1])
                c2 = (cx + pts[i+1][0], cy + pts[i+1][1])
                p  = (cx + pts[i+2][0], cy + pts[i+2][1])
                result.append(('C', [c1[0], c1[1], c2[0], c2[1], p[0], p[1]]))
                cx, cy = p
        elif cmd == 'S':
            pts = list(zip(nums[0::2], nums[1::2]))
            for i in range(0, len(pts), 2):
                c2, p = pts[i], pts[i+1]
                result.append(('S', [c2[0], c2[1], p[0], p[1]]))
                cx, cy = p
        elif cmd == 's':
            pts = list(zip(nums[0::2], nums[1::2]))
            for i in range(0, len(pts), 2):
                c2 = (cx + pts[i][0], cy + pts[i][1])
                p  = (cx + pts[i+1][0], cy + pts[i+1][1])
                result.append(('S', [c2[0], c2[1], p[0], p[1]]))
                cx, cy = p
        elif cmd == 'Q':
            pts = list(zip(nums[0::2], nums[1::2]))
            for i in range(0, len(pts), 2):
                c, p = pts[i], pts[i+1]
                result.append(('Q', [c[0], c[1], p[0], p[1]]))
                cx, cy = p
        elif cmd == 'q':
            pts = list(zip(nums[0::2], nums[1::2]))
            for i in range(0, len(pts), 2):
                c = (cx + pts[i][0], cy + pts[i][1])
                p = (cx + pts[i+1][0], cy + pts[i+1][1])
                result.append(('Q', [c[0], c[1], p[0], p[1]]))
                cx, cy = p
        elif cmd in ('Z', 'z'):
            result.append(('Z', []))
            cx, cy = sx, sy
        else:
            # Pass through anything else (A, T, etc.) as-is
            result.append((cmd.upper(), nums))

    return result

def snap_path(d, grid):
    tokens = tokenize_path(d)
    absolute = to_absolute(tokens)

    parts = []
    for cmd, nums in absolute:
        if cmd == 'Z':
            parts.append('Z')
        else:
            snapped = [snap(n, grid) for n in nums]
            # Format numbers: drop .0 if integer
            formatted = []
            for n in snapped:
                formatted.append(str(int(n)) if n == int(n) else str(n))
            parts.append(cmd + ' ' + ' '.join(formatted))

    return ' '.join(parts)

def process_svg(input_path, output_path, grid):
    ET.register_namespace('', 'http://www.w3.org/2000/svg')
    tree = ET.parse(input_path)
    root = tree.getroot()

    ns = {'svg': 'http://www.w3.org/2000/svg'}

    # Process all elements with a 'd' attribute
    for elem in root.iter():
        tag = elem.tag.split('}')[-1] if '}' in elem.tag else elem.tag
        if 'd' in elem.attrib:
            original = elem.attrib['d']
            try:
                elem.attrib['d'] = snap_path(original, grid)
            except Exception as e:
                print(f"Warning: could not process path: {e}")

    tree.write(output_path, xml_declaration=True, encoding='unicode')
    print(f"Done. Saved to {output_path}")

if __name__ == '__main__':
    input_svg  = sys.argv[1] if len(sys.argv) > 1 else 'input.svg'
    output_svg = sys.argv[2] if len(sys.argv) > 2 else 'output.svg'
    grid_size  = float(sys.argv[3]) if len(sys.argv) > 3 else 1.0

    process_svg(input_svg, output_svg, grid_size)
