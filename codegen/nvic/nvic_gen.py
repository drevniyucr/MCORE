import re
from pathlib import Path

# === Настройки ===
CMSIS_FILE = Path("stm32f767xx.h")   # путь к CMSIS-заголовку
OUTPUT_FILE = Path("irqn_type_autogen.hpp")

# === Регистры NVIC (по 4 IRQ на 1 IPR, по 32 IRQ на ISER) ===
def ipr_struct(irq):
    reg_index = irq // 4
    field_index = irq % 4
    return f"NVIC::_NVIC_IPR{reg_index}::PRI_N{field_index}"

def iser_struct(irq):
    reg_index = irq // 32
    field_index = irq % 32
    return f"NVIC::_NVIC_ISER{reg_index}"

def icer_struct(irq):
    reg_index = irq // 32
    field_index = irq % 32
    return f"NVIC::_NVIC_ICER{reg_index}"

def ispr_struct(irq):
    reg_index = irq // 32
    field_index = irq % 32
    return f"NVIC::_NVIC_ISPR{reg_index}"

def icpr_struct(irq):
    reg_index = irq // 32
    field_index = irq % 32
    return f"NVIC::_NVIC_ICPR{reg_index}"

# === Парсинг CMSIS ===
pattern = re.compile(r"^\s*(\w+_IRQn)\s*=\s*(\d+),\s*/\*\!<\s*(.*?)\s*\*/")
matches = []

with CMSIS_FILE.open(encoding="utf-8", errors="ignore") as f:
    for line in f:
        m = pattern.match(line)
        if m:
            matches.append((m.group(1), int(m.group(2)), m.group(3)))

# === Генерация кода ===
out = []
out.append("// Auto-generated NVIC IRQ mapping")
out.append("struct IRQn_Type {")
for name, num, comment in matches:
    out.append(f"    struct {name} {{ // < {comment}")
    out.append(f"        static constexpr uint32_t irq_number = {num};")
    out.append(f"        using Priority = {ipr_struct(num)};")
    out.append(f"        using Enable   = {iser_struct(num)};")
    out.append(f"        using Disable  = {icer_struct(num)};")
    out.append(f"        using SetPend  = {ispr_struct(num)};")
    out.append(f"        using ClrPend  = {icpr_struct(num)};")
    out.append("    };")
out.append("};")

# === Сохранение ===
OUTPUT_FILE.write_text("\n".join(out), encoding="utf-8")
print(f"✅ Generated {OUTPUT_FILE} with {len(matches)} IRQ entries.")
