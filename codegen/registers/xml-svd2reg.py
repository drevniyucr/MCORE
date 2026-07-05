import xml.etree.ElementTree as ET
from pathlib import Path
import svd2reg
import re
import os

NS = {"arm": "http://www.arm.com/core_reg"}  # пространство имён ARM

os.chdir(Path(__file__).resolve().parent)
# Генерация
hpp_lines = [
        "#pragma once",
        "#include <cstdint>",
        "#include <mcore_utils>",
        "",
    ]

def clean_description(text: str | None) -> str:
    if not text:
        return ""
    return re.sub(r"\s+", " ", text).strip()

def xml_to_hpp(xml_path, output_path):
    hpp_lines.append( f"""// Generated from xml file: {xml_path}
""")
    tree = ET.parse(xml_path)
    root = tree.getroot()

    peripherals = {}

    # Теперь ищем с учётом namespace
    for periph in root.findall("arm:peripheral", NS):
        periph_name = periph.get("name")
        periph_desc = clean_description(periph.findtext("arm:description", default="", namespaces=NS))
        regs = []

        for reg in periph.findall("arm:register", NS):
            reg_access = reg.get("access", "")
            reg_name = reg.get("name", "")
            reg_offset = reg.get("offset", "")
            reg_desc = clean_description(reg.findtext("arm:description", default="", namespaces=NS))
            reg_gui_name = reg.findtext("arm:gui_name", default="", namespaces=NS)

            bitfields = []
            for bf in reg.findall("arm:bitField", NS):
                bf_name = bf.get("name", "")
                bf_desc = clean_description(bf.findtext("arm:description", default="", namespaces=NS))
                bits = bf.findtext("arm:definition", default="", namespaces=NS).strip()

                if bits.startswith("[") and bits.endswith("]"):
                    bit_range = bits[1:-1]
                    if ":" in bit_range:
                        high, low = map(int, bit_range.split(":"))
                    else:
                        high = low = int(bit_range)
                    width = high - low + 1
                else:
                    continue

                bitfields.append({
                    "name": bf_name,
                    "desc": bf_desc,
                    "low": low,
                    "width": width,
                    "bits": bits
                })

            regs.append({
                "name": reg_name,
                "offset": reg_offset,
                "access": reg_access,
                "desc": reg_desc,
                "gui_name": reg_gui_name,
                "bitfields": bitfields
            })

        peripherals[periph_name] = {
            "desc": periph_desc,
            "registers": regs
        }

    for periph_name, periph_data in peripherals.items():
        hpp_lines.append(f"// --------------------------------------------")
        hpp_lines.append(f"// {periph_name}: {periph_data['desc']}")
        hpp_lines.append(f"// --------------------------------------------")
        hpp_lines.append(f"namespace {periph_name} {{")
        hpp_lines.append("")

        for reg in periph_data["registers"]:
            if reg["desc"]:
                hpp_lines.append(f"    // {reg['name']}: {reg['desc']}")
            hpp_lines.append(f"    struct _{reg['name']} : Register <{reg['offset']}, {access_to_cpp(reg['access'])}, _{reg['name']}> {{")

            for bf in reg["bitfields"]:
                if bf["desc"]:
                    hpp_lines.append(f"        using {bf['name']} = Field <_{reg['name']}, {bf['low']}, {bf['width']}>; // Bits {bf['bits']} : {bf['desc']}")
                else:
                    hpp_lines.append(f"        using {bf['name']} = Field <_{reg['name']}, {bf['low']}, {bf['width']}>; // Bits {bf['bits']}")

            hpp_lines.append("    };")
            hpp_lines.append("")

        hpp_lines.append(f"}} // namespace {periph_name}")
        hpp_lines.append("")

   # mode = "w" if not Path(output_path).exists() else "a"
    print(f"✅ Generated: {xml_path}")

def access_to_cpp(access):
    return {
        "RW": "ReadWrite",
        "RO": "ReadOnly",
        "WO": "WriteOnly"
    }.get(access, "ReadWrite")

if __name__ == "__main__":

    current_dir = Path(__file__).parent
    print(current_dir)
    
    out_file = (r"./mcore_regs.hpp")
    svd_file = (r"./xml-svd/STM32F767.svd")
    Path(out_file).write_text("\n".join(""), encoding="utf-8")

    modules = [
        (r"./xml-svd/system_registers_m7.xml"),
        (r"./xml-svd/nvic_registers_v7m.xml"),
        (r"./xml-svd/cache_registers_v7em.xml"),
        (r"./xml-svd/mpu_registers_v7m.xml")]
    
    hpp_lines.append(f"""
/*------------------------------------------------------------------------------
------------------------ Coretex-M7 Register Descripton ------------------------
------------------------------------------------------------------------------*/
"""
)

    for name in modules:
        xml_to_hpp(name, f"out_file")

    hpp_lines.append(f"""
/*-------------------------------------------------------------------------------
--------------------- End of Coretex-M7 Register Descripton ---------------------
-------------------------------------------------------------------------------*/
"""
    )
    with open(out_file, "a", encoding="utf-8") as f:
        f.write("\n".join(hpp_lines) + "\n")    

    svd2reg.main(svd_file,out_file)    
