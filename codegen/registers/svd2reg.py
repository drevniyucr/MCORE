#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import xml.etree.ElementTree as ET
from pathlib import Path
import sys
import re

FOOTER = """
\n /* stm32f767\n 

--------------------------------------------------------------------------------
--------------------- End of STM32F767 Register Descripton ---------------------
--------------------------------------------------------------------------------    
*/
"""

def sanitize_name(name: str) -> str:
    """Приводит имена к корректному C++ виду"""
    return name.replace("-", "_").replace(" ", "_").replace("/", "_")


def clean_description(text: str | None) -> str:
    """Удаляет лишние пробелы, переводы строк, табуляцию"""
    if not text:
        return ""
    return re.sub(r"\s+", " ", text).strip()


def parse_fields(register_elem):
    """Возвращает список полей (битов) регистра"""
    fields = []
    fields_elem = register_elem.find('fields')
    if fields_elem is None:
        return fields
    for f in fields_elem.findall('field'):
        name = sanitize_name(f.findtext('name'))
        bit_offset = f.findtext('bitOffset')
        bit_width = f.findtext('bitWidth')
        desc = clean_description(f.findtext('description'))
        if name and bit_offset and bit_width:
            fields.append((name, int(bit_offset), int(bit_width), desc))
    return fields


def gen_registers(periph_elem, base_name):
    """Создаёт код регистров и их полей"""
    result = []
    registers = periph_elem.find('registers')
    if registers is None:
        return result

    for reg in registers.findall('register'):
        reg_name = sanitize_name(reg.findtext('name'))
        offset = reg.findtext('addressOffset')
        access = reg.findtext('access') or "read-write"
        desc = clean_description(reg.findtext('description'))

        rw_type = "ReadWrite"
        if "read-only" in access:
            rw_type = "ReadOnly"
        elif "write-only" in access:
            rw_type = "WriteOnly"

        full_addr = f"{base_name}_BASE + {offset}"
        reg_alias = f"Register<{full_addr}, {rw_type}, _{reg_name}>"

        if desc:
            result.append(f"    // {desc}")

        fields = parse_fields(reg)
        if fields:
            result.append(f"    struct _{reg_name} : {reg_alias} {{")
            for name, off, width, fdesc in fields:
                comment = f" // {fdesc}" if fdesc else ""
                result.append(f"        using {name} = Field<_{reg_name}, {off}, {width}>;{comment}")
            result.append("    };")
        result.append("")
    return result


def main(svd_path,out_path):
    # if len(sys.argv) < 2:
    #     print("Usage: svd2registers.py STM32F767.svd [output.hpp]")
    #     sys.exit(1)
    print(f"Парсим {svd_path} → {out_path}")
    # svd_path = Path(sys.argv[1])
    # out_path = Path(sys.argv[2]) if len(sys.argv) > 2 else svd_path.with_suffix(".hpp")

    tree = ET.parse(svd_path)
    root = tree.getroot()

    HEADER = f"""
/*-----------------------------------------------------------------------------
------------------------ STM32F767 Register Descripton ------------------------
-------------------------------------------------------------------------------

Generating from file: {out_path}*/

"""
  
    out_lines = [HEADER]

    for periph in root.findall("peripherals/peripheral"):
        name = sanitize_name(periph.findtext("name"))
        print(f"{name}")
        base = periph.findtext("baseAddress")
        desc = clean_description(periph.findtext("description"))

        out_lines.append(f"// --------------------------------------------")
        out_lines.append(f"// {name}: {desc}")
        out_lines.append(f"// Base address: {base}")
        out_lines.append(f"// --------------------------------------------\n")

        out_lines.append(f"namespace {name} {{")
        out_lines.append(f"    static constexpr uint32_t {name}_BASE = {base};\n")
        out_lines += gen_registers(periph, name)
        out_lines.append(f"}} // namespace {name}\n")

    out_lines.append(FOOTER)

    # out_path.write_text("\n".join(out_lines), encoding="utf-8")

    with open(out_path, "a", encoding="utf-8") as f:
        f.write("\n".join(out_lines) + "\n")

    print(f"✅ Generated: {out_path}")


if __name__ == "__main__":
    main()
