"""
CMSIS device header -> MCORE typed IRQ map (nvic_def.hpp).

Usage:
  python nvic_gen.py                                  # STM32F767 (defaults)
  python nvic_gen.py --header stm32h755xx.h \
                     --out ../../mcore/chips/stm32h755/chip/nvic_def.hpp
"""
import argparse
import re
from pathlib import Path

HERE = Path(__file__).resolve().parent

# Cortex-M7 system exceptions: priorities live in SCB->SHPRx, not in NVIC.
SYSTEM_EXCEPTIONS = """    //SYSTEM EXCEPTIONS
    struct NonMaskableInt_IRQn{
        static constexpr uint32_t irq_number = -14;
    };
    struct MemoryManagement_IRQn { // < Window WatchDog Interrupt
        using Priority = SCB::_SHPR1::PRI_4;
    };
    struct BusFault_IRQn {// < Bus Fault Interrupt
        using Priority = SCB::_SHPR1::PRI_5;
    };
    struct UsageFault_IRQn {// < Usage Fault Interrupt
        using Priority = SCB::_SHPR1::PRI_6;
    };
    struct SVCall_IRQn {// < SV Call Interrupt
        using Priority = SCB::_SHPR2::PRI_11;
    };
    struct DebugMonitor_IRQn {// < Debug Monitor Interrupt
        using Priority = SCB::_SHPR3::PRI_12;
    };
    struct PendSV_IRQn {// < Pend SV Interrupt
        using Priority = SCB::_SHPR3::PRI_14;
    };
    struct SysTick_IRQn {// < SysTick Interrupt
        using Priority = SCB::_SHPR3::PRI_15;
    };"""

PATTERN = re.compile(r"^\s*(\w+_IRQn)\s*=\s*(\d+),\s*/\*\!<\s*(.*?)\s*\*/")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--header", default=str(HERE / "stm32f767xx.h"), help="CMSIS device header")
    ap.add_argument("--out", default=str(HERE / "irqn_type_autogen.hpp"))
    args = ap.parse_args()

    matches, seen = [], set()
    with open(args.header, encoding="utf-8", errors="ignore") as f:
        for line in f:
            m = PATTERN.match(line)
            if m and m.group(1) not in seen:
                seen.add(m.group(1))
                matches.append((m.group(1), int(m.group(2)), m.group(3)))

    out = ["// Auto-generated NVIC IRQ mapping", "#pragma once", '#include "core/regs.hpp"',
           "struct IRQn_Type {", SYSTEM_EXCEPTIONS]
    for name, num, comment in matches:
        out.append(f"    struct {name} {{ // < {comment}")
        out.append(f"        static constexpr uint32_t irq_number = {num};")
        out.append(f"        using Priority = NVIC::_NVIC_IPR{num // 4}::PRI_N{num % 4};")
        out.append(f"        using Enable   = NVIC::_NVIC_ISER{num // 32};")
        out.append(f"        using Disable  = NVIC::_NVIC_ICER{num // 32};")
        out.append(f"        using SetPend  = NVIC::_NVIC_ISPR{num // 32};")
        out.append(f"        using ClrPend  = NVIC::_NVIC_ICPR{num // 32};")
        out.append("    };")
    out.append("};")

    Path(args.out).write_text("\n".join(out), encoding="utf-8", newline="\n")
    print(f"Generated {args.out} with {len(matches)} IRQ entries.")


if __name__ == "__main__":
    main()
