# SVD fixups

Vendor SVD files contain errors (wrong bit positions, registers at wrong
offsets, off-by-one channel numbering). `svd2chip.py --fixups <chip>.json`
corrects them while generating, so the generated header stays reproducible.

Find errors with the cross-check against the vendor CMSIS device header (also
run by CTest as `regs_<chip>`):

```sh
python check_regs.py --regs ../../mcore/chips/stm32h755/chip/regs.hpp --cmsis ../nvic/stm32h755xx.h -v
```

Every reported difference is either fixed here or listed in `KNOWN` in
`check_regs.py` with the reason (CMSIS typos, generic CMSIS definitions).

Keys are `<base peripheral>/<register>` as named in the SVD (for derived
peripherals: the one they derive from, e.g. `ADC3` for ADC1/2/3).

| Section | Value | Effect |
|---|---|---|
| `aliases` | `{"ALIAS": "FIELD"}` | extra name for an existing field (driver compatibility) |
| `fields` | `{"FIELD": [offset, width]}` or `null` | replace / add a field, `null` removes it |
| `field_array` | `{"names": [...], "count": n, "first": k}` | replaces all fields with per-channel groups `NAME<k..k+n-1>` |
| `registers` | `{"offset": "0x..", "rename": "NEW", "_why": "..."}` | move or rename a register |

The STM32F767 header (`mcore/chips/stm32f767/chip/regs.hpp`) predates this flow and
was corrected by hand; its fixes are marked `(fixed: ...)` in the comments.
