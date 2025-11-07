import re

# === 1. Читаем CMSIS IRQ-определения ===
with open("irqs.txt", "r", encoding="utf-8") as f:
    cmsis_data = f.read()

# Пример строки: WWDG_IRQn = 0, /*!< Window WatchDog Interrupt */
cmsis_irqs = re.findall(r"(\w+_IRQn)\s*=\s*(\d+),\s*/\*!\s*(.*?)\s*\*/", cmsis_data)

# === 2. Читаем NVIC IPR структуры ===
with open("nvic_ipr.txt", "r", encoding="utf-8") as f:
    nvic_data = f.read()

# Пример строки: using PRI_N3 = Field <_NVIC_IPR8, 24, 8>; // Bits [31:24] : Priority of interrupt 35
nvic_fields = re.findall(
    r"using\s+(PRI_N\d)\s*=\s*Field\s*<(_NVIC_IPR\d+),.*?Priority of interrupt\s+(\d+)",
    nvic_data
)

# === 3. Создаём словарь { interrupt_number: (register, field) } ===
nvic_map = {int(num): (reg, field) for field, reg, num in nvic_fields}

# === 4. Генерация итогового кода ===
output = []
for name, num, desc in cmsis_irqs:
    irq_num = int(num)
    if irq_num in nvic_map:
        reg, field = nvic_map[irq_num]
        output.append(f"using {name} = NVIC::{reg}::{field}; // {desc}")
    else:
        output.append(f"// {name} (IRQ {irq_num}) not found in NVIC_IPR map")

# === 5. Запись результата ===
with open("nvic_irq_map.hpp", "w", encoding="utf-8") as f:
    f.write("// Auto-generated NVIC IRQ mapping\n\n")
    f.write("struct mcore_nvic {\n")
    for line in output:
        f.write(f"    {line}\n")
    f.write("};\n")

print("✅ Файл nvic_irq_map.hpp создан.")
