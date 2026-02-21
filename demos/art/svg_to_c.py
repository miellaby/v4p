"""
SVG to C code generator for v4p_decodeSVGPath calls.
- One v4p_decodeSVGPath call per path element
- inkscape:label used as the variable name
- fill color mapped to V4P_* constants
Usage: python svg_to_c.py input.svg [scale]
"""

import re
import sys
from xml.etree import ElementTree as ET

INKSCAPE_NS = 'http://www.inkscape.org/namespaces/inkscape'

COLOR_MAP = {
    '#ff0':    'V4P_YELLOW',
    '#ffff00': 'V4P_YELLOW',
    '#0f0':    'V4P_GREEN',
    '#00ff00': 'V4P_GREEN',
    '#007f00': 'V4P_OLIVE',
    '#7f7f7f': 'V4P_GRAY',
    '#bfbfbf': 'V4P_SILVER',
    '#00f':    'V4P_BLUE',
    '#0000ff': 'V4P_BLUE',
    '#00007f': 'V4P_VIOLET',
    '#ff7f00': '120',
    '#7f0000': 'V4P_DARKRED',
    'red':     'V4P_RED',
    '#ff0000': 'V4P_RED',
    '#c3ff00': 'V4P_LIMEGREEN',
}

def get_fill(elem):
    fill = elem.get('fill')
    if not fill:
        style = elem.get('style', '')
        m = re.search(r'fill\s*:\s*([^;]+)', style)
        if m:
            fill = m.group(1).strip()
    return (fill or '').strip()

def color_const(fill):
    return COLOR_MAP.get(fill.lower(), f'/* unknown: {fill} */')

def format_path_d(d, indent=8):
    d = ' '.join(d.split())
    pad = ' ' * indent
    if len(d) <= 70:
        return f'"{d}"'
    lines = []
    while len(d) > 70:
        cut = d.rfind(' ', 0, 70)
        if cut == -1:
            cut = 70
        lines.append(d[:cut])
        d = d[cut+1:]
    if d:
        lines.append(d)
    result = ('\n' + pad).join(f'"{line} "' for line in lines[:-1])
    result += '\n' + pad + f'"{lines[-1]}"'
    return result

def process(svg_file, scale):
    ET.register_namespace('', 'http://www.w3.org/2000/svg')
    tree = ET.parse(svg_file)
    root = tree.getroot()

    output = []
    output.append(f"// Auto-generated from {svg_file}")
    output.append(f"// Scale: {scale}f\n")

    for elem in root.iter():
        tag = elem.tag.split('}')[-1] if '}' in elem.tag else elem.tag
        if tag not in ('path', 'rect', 'polygon'):
            continue
        d = elem.get('d', '').strip()
        if not d:
            continue
        label = elem.get(f'{{{INKSCAPE_NS}}}label', '') or elem.get('id', 'unnamed')
        fill  = get_fill(elem)
        color = color_const(fill) if fill else '/* no fill */'

        output.append(f"// {label}  (fill: {fill} -> {color})")
        output.append(f"v4p_decodeSVGPath({label},")
        output.append(f"        {format_path_d(d)},")
        output.append(f"        {scale}f);\n")

    return '\n'.join(output)

if __name__ == '__main__':
    svg_file = sys.argv[1] if len(sys.argv) > 1 else 'circuit2.svg'
    scale    = sys.argv[2] if len(sys.argv) > 2 else '16.0'
    print(process(svg_file, scale))
