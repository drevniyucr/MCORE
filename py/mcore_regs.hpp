#pragma once
#include "mcore_utils.hpp"  // Определи шаблоны Register и Field заранее



// --------------------------------------------
// RNG: Random number generator
// Base address: 0x50060800
// --------------------------------------------

namespace RNG {
    static constexpr uint32_t RNG_BASE = 0x50060800;

    // control register
    using CR_Reg = Register<RNG_BASE + 0x0, ReadWrite>;
    struct _CR {
        using IE = Field<CR_Reg, 3, 1>; // Interrupt enable
        using RNGEN = Field<CR_Reg, 2, 1>; // Random number generator enable
    };

    // status register
    using SR_Reg = Register<RNG_BASE + 0x4, ReadWrite>;
    struct _SR {
        using SEIS = Field<SR_Reg, 6, 1>; // Seed error interrupt status
        using CEIS = Field<SR_Reg, 5, 1>; // Clock error interrupt status
        using SECS = Field<SR_Reg, 2, 1>; // Seed error current status
        using CECS = Field<SR_Reg, 1, 1>; // Clock error current status
        using DRDY = Field<SR_Reg, 0, 1>; // Data ready
    };

    // data register
    using DR_Reg = Register<RNG_BASE + 0x8, ReadOnly>;
    struct _DR {
        using RNDATA = Field<DR_Reg, 0, 32>; // Random data
    };

} // namespace RNG

// --------------------------------------------
// HASH: Hash processor
// Base address: 0x50060400
// --------------------------------------------

namespace HASH {
    static constexpr uint32_t HASH_BASE = 0x50060400;

    // control register
    using CR_Reg = Register<HASH_BASE + 0x0, ReadWrite>;
    struct _CR {
        using INIT = Field<CR_Reg, 2, 1>; // Initialize message digest calculation
        using DMAE = Field<CR_Reg, 3, 1>; // DMA enable
        using DATATYPE = Field<CR_Reg, 4, 2>; // Data type selection
        using MODE = Field<CR_Reg, 6, 1>; // Mode selection
        using ALGO0 = Field<CR_Reg, 7, 1>; // Algorithm selection
        using NBW = Field<CR_Reg, 8, 4>; // Number of words already pushed
        using DINNE = Field<CR_Reg, 12, 1>; // DIN not empty
        using MDMAT = Field<CR_Reg, 13, 1>; // Multiple DMA Transfers
        using LKEY = Field<CR_Reg, 16, 1>; // Long key selection
        using ALGO1 = Field<CR_Reg, 18, 1>; // ALGO
    };

    // data input register
    using DIN_Reg = Register<HASH_BASE + 0x4, ReadWrite>;
    struct _DIN {
        using DATAIN = Field<DIN_Reg, 0, 32>; // Data input
    };

    // start register
    using STR_Reg = Register<HASH_BASE + 0x8, ReadWrite>;
    struct _STR {
        using DCAL = Field<STR_Reg, 8, 1>; // Digest calculation
        using NBLW = Field<STR_Reg, 0, 5>; // Number of valid bits in the last word of the message
    };

    // digest registers
    using HR0_Reg = Register<HASH_BASE + 0xC, ReadOnly>;
    struct _HR0 {
        using H0 = Field<HR0_Reg, 0, 32>; // H0
    };

    // digest registers
    using HR1_Reg = Register<HASH_BASE + 0x10, ReadOnly>;
    struct _HR1 {
        using H1 = Field<HR1_Reg, 0, 32>; // H1
    };

    // digest registers
    using HR2_Reg = Register<HASH_BASE + 0x14, ReadOnly>;
    struct _HR2 {
        using H2 = Field<HR2_Reg, 0, 32>; // H2
    };

    // digest registers
    using HR3_Reg = Register<HASH_BASE + 0x18, ReadOnly>;
    struct _HR3 {
        using H3 = Field<HR3_Reg, 0, 32>; // H3
    };

    // digest registers
    using HR4_Reg = Register<HASH_BASE + 0x1C, ReadOnly>;
    struct _HR4 {
        using H4 = Field<HR4_Reg, 0, 32>; // H4
    };

    // interrupt enable register
    using IMR_Reg = Register<HASH_BASE + 0x20, ReadWrite>;
    struct _IMR {
        using DCIE = Field<IMR_Reg, 1, 1>; // Digest calculation completion interrupt enable
        using DINIE = Field<IMR_Reg, 0, 1>; // Data input interrupt enable
    };

    // status register
    using SR_Reg = Register<HASH_BASE + 0x24, ReadWrite>;
    struct _SR {
        using BUSY = Field<SR_Reg, 3, 1>; // Busy bit
        using DMAS = Field<SR_Reg, 2, 1>; // DMA Status
        using DCIS = Field<SR_Reg, 1, 1>; // Digest calculation completion interrupt status
        using DINIS = Field<SR_Reg, 0, 1>; // Data input interrupt status
    };

    // context swap registers
    using CSR0_Reg = Register<HASH_BASE + 0xF8, ReadWrite>;
    struct _CSR0 {
        using CSR0 = Field<CSR0_Reg, 0, 32>; // CSR0
    };

    // context swap registers
    using CSR1_Reg = Register<HASH_BASE + 0xFC, ReadWrite>;
    struct _CSR1 {
        using CSR1 = Field<CSR1_Reg, 0, 32>; // CSR1
    };

    // context swap registers
    using CSR2_Reg = Register<HASH_BASE + 0x100, ReadWrite>;
    struct _CSR2 {
        using CSR2 = Field<CSR2_Reg, 0, 32>; // CSR2
    };

    // context swap registers
    using CSR3_Reg = Register<HASH_BASE + 0x104, ReadWrite>;
    struct _CSR3 {
        using CSR3 = Field<CSR3_Reg, 0, 32>; // CSR3
    };

    // context swap registers
    using CSR4_Reg = Register<HASH_BASE + 0x108, ReadWrite>;
    struct _CSR4 {
        using CSR4 = Field<CSR4_Reg, 0, 32>; // CSR4
    };

    // context swap registers
    using CSR5_Reg = Register<HASH_BASE + 0x10C, ReadWrite>;
    struct _CSR5 {
        using CSR5 = Field<CSR5_Reg, 0, 32>; // CSR5
    };

    // context swap registers
    using CSR6_Reg = Register<HASH_BASE + 0x110, ReadWrite>;
    struct _CSR6 {
        using CSR6 = Field<CSR6_Reg, 0, 32>; // CSR6
    };

    // context swap registers
    using CSR7_Reg = Register<HASH_BASE + 0x114, ReadWrite>;
    struct _CSR7 {
        using CSR7 = Field<CSR7_Reg, 0, 32>; // CSR7
    };

    // context swap registers
    using CSR8_Reg = Register<HASH_BASE + 0x118, ReadWrite>;
    struct _CSR8 {
        using CSR8 = Field<CSR8_Reg, 0, 32>; // CSR8
    };

    // context swap registers
    using CSR9_Reg = Register<HASH_BASE + 0x11C, ReadWrite>;
    struct _CSR9 {
        using CSR9 = Field<CSR9_Reg, 0, 32>; // CSR9
    };

    // context swap registers
    using CSR10_Reg = Register<HASH_BASE + 0x120, ReadWrite>;
    struct _CSR10 {
        using CSR10 = Field<CSR10_Reg, 0, 32>; // CSR10
    };

    // context swap registers
    using CSR11_Reg = Register<HASH_BASE + 0x124, ReadWrite>;
    struct _CSR11 {
        using CSR11 = Field<CSR11_Reg, 0, 32>; // CSR11
    };

    // context swap registers
    using CSR12_Reg = Register<HASH_BASE + 0x128, ReadWrite>;
    struct _CSR12 {
        using CSR12 = Field<CSR12_Reg, 0, 32>; // CSR12
    };

    // context swap registers
    using CSR13_Reg = Register<HASH_BASE + 0x12C, ReadWrite>;
    struct _CSR13 {
        using CSR13 = Field<CSR13_Reg, 0, 32>; // CSR13
    };

    // context swap registers
    using CSR14_Reg = Register<HASH_BASE + 0x130, ReadWrite>;
    struct _CSR14 {
        using CSR14 = Field<CSR14_Reg, 0, 32>; // CSR14
    };

    // context swap registers
    using CSR15_Reg = Register<HASH_BASE + 0x134, ReadWrite>;
    struct _CSR15 {
        using CSR15 = Field<CSR15_Reg, 0, 32>; // CSR15
    };

    // context swap registers
    using CSR16_Reg = Register<HASH_BASE + 0x138, ReadWrite>;
    struct _CSR16 {
        using CSR16 = Field<CSR16_Reg, 0, 32>; // CSR16
    };

    // context swap registers
    using CSR17_Reg = Register<HASH_BASE + 0x13C, ReadWrite>;
    struct _CSR17 {
        using CSR17 = Field<CSR17_Reg, 0, 32>; // CSR17
    };

    // context swap registers
    using CSR18_Reg = Register<HASH_BASE + 0x140, ReadWrite>;
    struct _CSR18 {
        using CSR18 = Field<CSR18_Reg, 0, 32>; // CSR18
    };

    // context swap registers
    using CSR19_Reg = Register<HASH_BASE + 0x144, ReadWrite>;
    struct _CSR19 {
        using CSR19 = Field<CSR19_Reg, 0, 32>; // CSR19
    };

    // context swap registers
    using CSR20_Reg = Register<HASH_BASE + 0x148, ReadWrite>;
    struct _CSR20 {
        using CSR20 = Field<CSR20_Reg, 0, 32>; // CSR20
    };

    // context swap registers
    using CSR21_Reg = Register<HASH_BASE + 0x14C, ReadWrite>;
    struct _CSR21 {
        using CSR21 = Field<CSR21_Reg, 0, 32>; // CSR21
    };

    // context swap registers
    using CSR22_Reg = Register<HASH_BASE + 0x150, ReadWrite>;
    struct _CSR22 {
        using CSR22 = Field<CSR22_Reg, 0, 32>; // CSR22
    };

    // context swap registers
    using CSR23_Reg = Register<HASH_BASE + 0x154, ReadWrite>;
    struct _CSR23 {
        using CSR23 = Field<CSR23_Reg, 0, 32>; // CSR23
    };

    // context swap registers
    using CSR24_Reg = Register<HASH_BASE + 0x158, ReadWrite>;
    struct _CSR24 {
        using CSR24 = Field<CSR24_Reg, 0, 32>; // CSR24
    };

    // context swap registers
    using CSR25_Reg = Register<HASH_BASE + 0x15C, ReadWrite>;
    struct _CSR25 {
        using CSR25 = Field<CSR25_Reg, 0, 32>; // CSR25
    };

    // context swap registers
    using CSR26_Reg = Register<HASH_BASE + 0x160, ReadWrite>;
    struct _CSR26 {
        using CSR26 = Field<CSR26_Reg, 0, 32>; // CSR26
    };

    // context swap registers
    using CSR27_Reg = Register<HASH_BASE + 0x164, ReadWrite>;
    struct _CSR27 {
        using CSR27 = Field<CSR27_Reg, 0, 32>; // CSR27
    };

    // context swap registers
    using CSR28_Reg = Register<HASH_BASE + 0x168, ReadWrite>;
    struct _CSR28 {
        using CSR28 = Field<CSR28_Reg, 0, 32>; // CSR28
    };

    // context swap registers
    using CSR29_Reg = Register<HASH_BASE + 0x16C, ReadWrite>;
    struct _CSR29 {
        using CSR29 = Field<CSR29_Reg, 0, 32>; // CSR29
    };

    // context swap registers
    using CSR30_Reg = Register<HASH_BASE + 0x170, ReadWrite>;
    struct _CSR30 {
        using CSR30 = Field<CSR30_Reg, 0, 32>; // CSR30
    };

    // context swap registers
    using CSR31_Reg = Register<HASH_BASE + 0x174, ReadWrite>;
    struct _CSR31 {
        using CSR31 = Field<CSR31_Reg, 0, 32>; // CSR31
    };

    // context swap registers
    using CSR32_Reg = Register<HASH_BASE + 0x178, ReadWrite>;
    struct _CSR32 {
        using CSR32 = Field<CSR32_Reg, 0, 32>; // CSR32
    };

    // context swap registers
    using CSR33_Reg = Register<HASH_BASE + 0x17C, ReadWrite>;
    struct _CSR33 {
        using CSR33 = Field<CSR33_Reg, 0, 32>; // CSR33
    };

    // context swap registers
    using CSR34_Reg = Register<HASH_BASE + 0x180, ReadWrite>;
    struct _CSR34 {
        using CSR34 = Field<CSR34_Reg, 0, 32>; // CSR34
    };

    // context swap registers
    using CSR35_Reg = Register<HASH_BASE + 0x184, ReadWrite>;
    struct _CSR35 {
        using CSR35 = Field<CSR35_Reg, 0, 32>; // CSR35
    };

    // context swap registers
    using CSR36_Reg = Register<HASH_BASE + 0x188, ReadWrite>;
    struct _CSR36 {
        using CSR36 = Field<CSR36_Reg, 0, 32>; // CSR36
    };

    // context swap registers
    using CSR37_Reg = Register<HASH_BASE + 0x18C, ReadWrite>;
    struct _CSR37 {
        using CSR37 = Field<CSR37_Reg, 0, 32>; // CSR37
    };

    // context swap registers
    using CSR38_Reg = Register<HASH_BASE + 0x190, ReadWrite>;
    struct _CSR38 {
        using CSR38 = Field<CSR38_Reg, 0, 32>; // CSR38
    };

    // context swap registers
    using CSR39_Reg = Register<HASH_BASE + 0x194, ReadWrite>;
    struct _CSR39 {
        using CSR39 = Field<CSR39_Reg, 0, 32>; // CSR39
    };

    // context swap registers
    using CSR40_Reg = Register<HASH_BASE + 0x198, ReadWrite>;
    struct _CSR40 {
        using CSR40 = Field<CSR40_Reg, 0, 32>; // CSR40
    };

    // context swap registers
    using CSR41_Reg = Register<HASH_BASE + 0x19C, ReadWrite>;
    struct _CSR41 {
        using CSR41 = Field<CSR41_Reg, 0, 32>; // CSR41
    };

    // context swap registers
    using CSR42_Reg = Register<HASH_BASE + 0x1A0, ReadWrite>;
    struct _CSR42 {
        using CSR42 = Field<CSR42_Reg, 0, 32>; // CSR42
    };

    // context swap registers
    using CSR43_Reg = Register<HASH_BASE + 0x1A4, ReadWrite>;
    struct _CSR43 {
        using CSR43 = Field<CSR43_Reg, 0, 32>; // CSR43
    };

    // context swap registers
    using CSR44_Reg = Register<HASH_BASE + 0x1A8, ReadWrite>;
    struct _CSR44 {
        using CSR44 = Field<CSR44_Reg, 0, 32>; // CSR44
    };

    // context swap registers
    using CSR45_Reg = Register<HASH_BASE + 0x1AC, ReadWrite>;
    struct _CSR45 {
        using CSR45 = Field<CSR45_Reg, 0, 32>; // CSR45
    };

    // context swap registers
    using CSR46_Reg = Register<HASH_BASE + 0x1B0, ReadWrite>;
    struct _CSR46 {
        using CSR46 = Field<CSR46_Reg, 0, 32>; // CSR46
    };

    // context swap registers
    using CSR47_Reg = Register<HASH_BASE + 0x1B4, ReadWrite>;
    struct _CSR47 {
        using CSR47 = Field<CSR47_Reg, 0, 32>; // CSR47
    };

    // context swap registers
    using CSR48_Reg = Register<HASH_BASE + 0x1B8, ReadWrite>;
    struct _CSR48 {
        using CSR48 = Field<CSR48_Reg, 0, 32>; // CSR48
    };

    // context swap registers
    using CSR49_Reg = Register<HASH_BASE + 0x1BC, ReadWrite>;
    struct _CSR49 {
        using CSR49 = Field<CSR49_Reg, 0, 32>; // CSR49
    };

    // context swap registers
    using CSR50_Reg = Register<HASH_BASE + 0x1C0, ReadWrite>;
    struct _CSR50 {
        using CSR50 = Field<CSR50_Reg, 0, 32>; // CSR50
    };

    // context swap registers
    using CSR51_Reg = Register<HASH_BASE + 0x1C4, ReadWrite>;
    struct _CSR51 {
        using CSR51 = Field<CSR51_Reg, 0, 32>; // CSR51
    };

    // context swap registers
    using CSR52_Reg = Register<HASH_BASE + 0x1C8, ReadWrite>;
    struct _CSR52 {
        using CSR52 = Field<CSR52_Reg, 0, 32>; // CSR52
    };

    // context swap registers
    using CSR53_Reg = Register<HASH_BASE + 0x1CC, ReadWrite>;
    struct _CSR53 {
        using CSR53 = Field<CSR53_Reg, 0, 32>; // CSR53
    };

    // HASH digest register
    using HASH_HR0_Reg = Register<HASH_BASE + 0x310, ReadOnly>;
    struct _HASH_HR0 {
        using H0 = Field<HASH_HR0_Reg, 0, 32>; // H0
    };

    // read-only
    using HASH_HR1_Reg = Register<HASH_BASE + 0x314, ReadOnly>;
    struct _HASH_HR1 {
        using H1 = Field<HASH_HR1_Reg, 0, 32>; // H1
    };

    // read-only
    using HASH_HR2_Reg = Register<HASH_BASE + 0x318, ReadOnly>;
    struct _HASH_HR2 {
        using H2 = Field<HASH_HR2_Reg, 0, 32>; // H2
    };

    // read-only
    using HASH_HR3_Reg = Register<HASH_BASE + 0x31C, ReadOnly>;
    struct _HASH_HR3 {
        using H3 = Field<HASH_HR3_Reg, 0, 32>; // H3
    };

    // read-only
    using HASH_HR4_Reg = Register<HASH_BASE + 0x320, ReadOnly>;
    struct _HASH_HR4 {
        using H4 = Field<HASH_HR4_Reg, 0, 32>; // H4
    };

    // read-only
    using HASH_HR5_Reg = Register<HASH_BASE + 0x324, ReadOnly>;
    struct _HASH_HR5 {
        using H5 = Field<HASH_HR5_Reg, 0, 32>; // H5
    };

    // read-only
    using HASH_HR6_Reg = Register<HASH_BASE + 0x328, ReadOnly>;
    struct _HASH_HR6 {
        using H6 = Field<HASH_HR6_Reg, 0, 32>; // H6
    };

    // read-only
    using HASH_HR7_Reg = Register<HASH_BASE + 0x32C, ReadOnly>;
    struct _HASH_HR7 {
        using H7 = Field<HASH_HR7_Reg, 0, 32>; // H7
    };

} // namespace HASH

// --------------------------------------------
// CRYP: Cryptographic processor
// Base address: 0x50060000
// --------------------------------------------

namespace CRYP {
    static constexpr uint32_t CRYP_BASE = 0x50060000;

    // control register
    using CR_Reg = Register<CRYP_BASE + 0x0, ReadWrite>;
    struct _CR {
        using ALGODIR = Field<CR_Reg, 2, 1>; // Algorithm direction
        using ALGOMODE0 = Field<CR_Reg, 3, 3>; // Algorithm mode
        using DATATYPE = Field<CR_Reg, 6, 2>; // Data type selection
        using KEYSIZE = Field<CR_Reg, 8, 2>; // Key size selection (AES mode only)
        using FFLUSH = Field<CR_Reg, 14, 1>; // FIFO flush
        using CRYPEN = Field<CR_Reg, 15, 1>; // Cryptographic processor enable
        using GCM_CCMPH = Field<CR_Reg, 16, 2>; // GCM_CCMPH
        using ALGOMODE3 = Field<CR_Reg, 19, 1>; // ALGOMODE
    };

    // status register
    using SR_Reg = Register<CRYP_BASE + 0x4, ReadOnly>;
    struct _SR {
        using BUSY = Field<SR_Reg, 4, 1>; // Busy bit
        using OFFU = Field<SR_Reg, 3, 1>; // Output FIFO full
        using OFNE = Field<SR_Reg, 2, 1>; // Output FIFO not empty
        using IFNF = Field<SR_Reg, 1, 1>; // Input FIFO not full
        using IFEM = Field<SR_Reg, 0, 1>; // Input FIFO empty
    };

    // data input register
    using DIN_Reg = Register<CRYP_BASE + 0x8, ReadWrite>;
    struct _DIN {
        using DATAIN = Field<DIN_Reg, 0, 32>; // Data input
    };

    // data output register
    using DOUT_Reg = Register<CRYP_BASE + 0xC, ReadOnly>;
    struct _DOUT {
        using DATAOUT = Field<DOUT_Reg, 0, 32>; // Data output
    };

    // DMA control register
    using DMACR_Reg = Register<CRYP_BASE + 0x10, ReadWrite>;
    struct _DMACR {
        using DOEN = Field<DMACR_Reg, 1, 1>; // DMA output enable
        using DIEN = Field<DMACR_Reg, 0, 1>; // DMA input enable
    };

    // interrupt mask set/clear register
    using IMSCR_Reg = Register<CRYP_BASE + 0x14, ReadWrite>;
    struct _IMSCR {
        using OUTIM = Field<IMSCR_Reg, 1, 1>; // Output FIFO service interrupt mask
        using INIM = Field<IMSCR_Reg, 0, 1>; // Input FIFO service interrupt mask
    };

    // raw interrupt status register
    using RISR_Reg = Register<CRYP_BASE + 0x18, ReadOnly>;
    struct _RISR {
        using OUTRIS = Field<RISR_Reg, 1, 1>; // Output FIFO service raw interrupt status
        using INRIS = Field<RISR_Reg, 0, 1>; // Input FIFO service raw interrupt status
    };

    // masked interrupt status register
    using MISR_Reg = Register<CRYP_BASE + 0x1C, ReadOnly>;
    struct _MISR {
        using OUTMIS = Field<MISR_Reg, 1, 1>; // Output FIFO service masked interrupt status
        using INMIS = Field<MISR_Reg, 0, 1>; // Input FIFO service masked interrupt status
    };

    // key registers
    using K0LR_Reg = Register<CRYP_BASE + 0x20, WriteOnly>;
    struct _K0LR {
        using b224 = Field<K0LR_Reg, 0, 1>; // b224
        using b225 = Field<K0LR_Reg, 1, 1>; // b225
        using b226 = Field<K0LR_Reg, 2, 1>; // b226
        using b227 = Field<K0LR_Reg, 3, 1>; // b227
        using b228 = Field<K0LR_Reg, 4, 1>; // b228
        using b229 = Field<K0LR_Reg, 5, 1>; // b229
        using b230 = Field<K0LR_Reg, 6, 1>; // b230
        using b231 = Field<K0LR_Reg, 7, 1>; // b231
        using b232 = Field<K0LR_Reg, 8, 1>; // b232
        using b233 = Field<K0LR_Reg, 9, 1>; // b233
        using b234 = Field<K0LR_Reg, 10, 1>; // b234
        using b235 = Field<K0LR_Reg, 11, 1>; // b235
        using b236 = Field<K0LR_Reg, 12, 1>; // b236
        using b237 = Field<K0LR_Reg, 13, 1>; // b237
        using b238 = Field<K0LR_Reg, 14, 1>; // b238
        using b239 = Field<K0LR_Reg, 15, 1>; // b239
        using b240 = Field<K0LR_Reg, 16, 1>; // b240
        using b241 = Field<K0LR_Reg, 17, 1>; // b241
        using b242 = Field<K0LR_Reg, 18, 1>; // b242
        using b243 = Field<K0LR_Reg, 19, 1>; // b243
        using b244 = Field<K0LR_Reg, 20, 1>; // b244
        using b245 = Field<K0LR_Reg, 21, 1>; // b245
        using b246 = Field<K0LR_Reg, 22, 1>; // b246
        using b247 = Field<K0LR_Reg, 23, 1>; // b247
        using b248 = Field<K0LR_Reg, 24, 1>; // b248
        using b249 = Field<K0LR_Reg, 25, 1>; // b249
        using b250 = Field<K0LR_Reg, 26, 1>; // b250
        using b251 = Field<K0LR_Reg, 27, 1>; // b251
        using b252 = Field<K0LR_Reg, 28, 1>; // b252
        using b253 = Field<K0LR_Reg, 29, 1>; // b253
        using b254 = Field<K0LR_Reg, 30, 1>; // b254
        using b255 = Field<K0LR_Reg, 31, 1>; // b255
    };

    // key registers
    using K0RR_Reg = Register<CRYP_BASE + 0x24, WriteOnly>;
    struct _K0RR {
        using b192 = Field<K0RR_Reg, 0, 1>; // b192
        using b193 = Field<K0RR_Reg, 1, 1>; // b193
        using b194 = Field<K0RR_Reg, 2, 1>; // b194
        using b195 = Field<K0RR_Reg, 3, 1>; // b195
        using b196 = Field<K0RR_Reg, 4, 1>; // b196
        using b197 = Field<K0RR_Reg, 5, 1>; // b197
        using b198 = Field<K0RR_Reg, 6, 1>; // b198
        using b199 = Field<K0RR_Reg, 7, 1>; // b199
        using b200 = Field<K0RR_Reg, 8, 1>; // b200
        using b201 = Field<K0RR_Reg, 9, 1>; // b201
        using b202 = Field<K0RR_Reg, 10, 1>; // b202
        using b203 = Field<K0RR_Reg, 11, 1>; // b203
        using b204 = Field<K0RR_Reg, 12, 1>; // b204
        using b205 = Field<K0RR_Reg, 13, 1>; // b205
        using b206 = Field<K0RR_Reg, 14, 1>; // b206
        using b207 = Field<K0RR_Reg, 15, 1>; // b207
        using b208 = Field<K0RR_Reg, 16, 1>; // b208
        using b209 = Field<K0RR_Reg, 17, 1>; // b209
        using b210 = Field<K0RR_Reg, 18, 1>; // b210
        using b211 = Field<K0RR_Reg, 19, 1>; // b211
        using b212 = Field<K0RR_Reg, 20, 1>; // b212
        using b213 = Field<K0RR_Reg, 21, 1>; // b213
        using b214 = Field<K0RR_Reg, 22, 1>; // b214
        using b215 = Field<K0RR_Reg, 23, 1>; // b215
        using b216 = Field<K0RR_Reg, 24, 1>; // b216
        using b217 = Field<K0RR_Reg, 25, 1>; // b217
        using b218 = Field<K0RR_Reg, 26, 1>; // b218
        using b219 = Field<K0RR_Reg, 27, 1>; // b219
        using b220 = Field<K0RR_Reg, 28, 1>; // b220
        using b221 = Field<K0RR_Reg, 29, 1>; // b221
        using b222 = Field<K0RR_Reg, 30, 1>; // b222
        using b223 = Field<K0RR_Reg, 31, 1>; // b223
    };

    // key registers
    using K1LR_Reg = Register<CRYP_BASE + 0x28, WriteOnly>;
    struct _K1LR {
        using b160 = Field<K1LR_Reg, 0, 1>; // b160
        using b161 = Field<K1LR_Reg, 1, 1>; // b161
        using b162 = Field<K1LR_Reg, 2, 1>; // b162
        using b163 = Field<K1LR_Reg, 3, 1>; // b163
        using b164 = Field<K1LR_Reg, 4, 1>; // b164
        using b165 = Field<K1LR_Reg, 5, 1>; // b165
        using b166 = Field<K1LR_Reg, 6, 1>; // b166
        using b167 = Field<K1LR_Reg, 7, 1>; // b167
        using b168 = Field<K1LR_Reg, 8, 1>; // b168
        using b169 = Field<K1LR_Reg, 9, 1>; // b169
        using b170 = Field<K1LR_Reg, 10, 1>; // b170
        using b171 = Field<K1LR_Reg, 11, 1>; // b171
        using b172 = Field<K1LR_Reg, 12, 1>; // b172
        using b173 = Field<K1LR_Reg, 13, 1>; // b173
        using b174 = Field<K1LR_Reg, 14, 1>; // b174
        using b175 = Field<K1LR_Reg, 15, 1>; // b175
        using b176 = Field<K1LR_Reg, 16, 1>; // b176
        using b177 = Field<K1LR_Reg, 17, 1>; // b177
        using b178 = Field<K1LR_Reg, 18, 1>; // b178
        using b179 = Field<K1LR_Reg, 19, 1>; // b179
        using b180 = Field<K1LR_Reg, 20, 1>; // b180
        using b181 = Field<K1LR_Reg, 21, 1>; // b181
        using b182 = Field<K1LR_Reg, 22, 1>; // b182
        using b183 = Field<K1LR_Reg, 23, 1>; // b183
        using b184 = Field<K1LR_Reg, 24, 1>; // b184
        using b185 = Field<K1LR_Reg, 25, 1>; // b185
        using b186 = Field<K1LR_Reg, 26, 1>; // b186
        using b187 = Field<K1LR_Reg, 27, 1>; // b187
        using b188 = Field<K1LR_Reg, 28, 1>; // b188
        using b189 = Field<K1LR_Reg, 29, 1>; // b189
        using b190 = Field<K1LR_Reg, 30, 1>; // b190
        using b191 = Field<K1LR_Reg, 31, 1>; // b191
    };

    // key registers
    using K1RR_Reg = Register<CRYP_BASE + 0x2C, WriteOnly>;
    struct _K1RR {
        using b128 = Field<K1RR_Reg, 0, 1>; // b128
        using b129 = Field<K1RR_Reg, 1, 1>; // b129
        using b130 = Field<K1RR_Reg, 2, 1>; // b130
        using b131 = Field<K1RR_Reg, 3, 1>; // b131
        using b132 = Field<K1RR_Reg, 4, 1>; // b132
        using b133 = Field<K1RR_Reg, 5, 1>; // b133
        using b134 = Field<K1RR_Reg, 6, 1>; // b134
        using b135 = Field<K1RR_Reg, 7, 1>; // b135
        using b136 = Field<K1RR_Reg, 8, 1>; // b136
        using b137 = Field<K1RR_Reg, 9, 1>; // b137
        using b138 = Field<K1RR_Reg, 10, 1>; // b138
        using b139 = Field<K1RR_Reg, 11, 1>; // b139
        using b140 = Field<K1RR_Reg, 12, 1>; // b140
        using b141 = Field<K1RR_Reg, 13, 1>; // b141
        using b142 = Field<K1RR_Reg, 14, 1>; // b142
        using b143 = Field<K1RR_Reg, 15, 1>; // b143
        using b144 = Field<K1RR_Reg, 16, 1>; // b144
        using b145 = Field<K1RR_Reg, 17, 1>; // b145
        using b146 = Field<K1RR_Reg, 18, 1>; // b146
        using b147 = Field<K1RR_Reg, 19, 1>; // b147
        using b148 = Field<K1RR_Reg, 20, 1>; // b148
        using b149 = Field<K1RR_Reg, 21, 1>; // b149
        using b150 = Field<K1RR_Reg, 22, 1>; // b150
        using b151 = Field<K1RR_Reg, 23, 1>; // b151
        using b152 = Field<K1RR_Reg, 24, 1>; // b152
        using b153 = Field<K1RR_Reg, 25, 1>; // b153
        using b154 = Field<K1RR_Reg, 26, 1>; // b154
        using b155 = Field<K1RR_Reg, 27, 1>; // b155
        using b156 = Field<K1RR_Reg, 28, 1>; // b156
        using b157 = Field<K1RR_Reg, 29, 1>; // b157
        using b158 = Field<K1RR_Reg, 30, 1>; // b158
        using b159 = Field<K1RR_Reg, 31, 1>; // b159
    };

    // key registers
    using K2LR_Reg = Register<CRYP_BASE + 0x30, WriteOnly>;
    struct _K2LR {
        using b96 = Field<K2LR_Reg, 0, 1>; // b96
        using b97 = Field<K2LR_Reg, 1, 1>; // b97
        using b98 = Field<K2LR_Reg, 2, 1>; // b98
        using b99 = Field<K2LR_Reg, 3, 1>; // b99
        using b100 = Field<K2LR_Reg, 4, 1>; // b100
        using b101 = Field<K2LR_Reg, 5, 1>; // b101
        using b102 = Field<K2LR_Reg, 6, 1>; // b102
        using b103 = Field<K2LR_Reg, 7, 1>; // b103
        using b104 = Field<K2LR_Reg, 8, 1>; // b104
        using b105 = Field<K2LR_Reg, 9, 1>; // b105
        using b106 = Field<K2LR_Reg, 10, 1>; // b106
        using b107 = Field<K2LR_Reg, 11, 1>; // b107
        using b108 = Field<K2LR_Reg, 12, 1>; // b108
        using b109 = Field<K2LR_Reg, 13, 1>; // b109
        using b110 = Field<K2LR_Reg, 14, 1>; // b110
        using b111 = Field<K2LR_Reg, 15, 1>; // b111
        using b112 = Field<K2LR_Reg, 16, 1>; // b112
        using b113 = Field<K2LR_Reg, 17, 1>; // b113
        using b114 = Field<K2LR_Reg, 18, 1>; // b114
        using b115 = Field<K2LR_Reg, 19, 1>; // b115
        using b116 = Field<K2LR_Reg, 20, 1>; // b116
        using b117 = Field<K2LR_Reg, 21, 1>; // b117
        using b118 = Field<K2LR_Reg, 22, 1>; // b118
        using b119 = Field<K2LR_Reg, 23, 1>; // b119
        using b120 = Field<K2LR_Reg, 24, 1>; // b120
        using b121 = Field<K2LR_Reg, 25, 1>; // b121
        using b122 = Field<K2LR_Reg, 26, 1>; // b122
        using b123 = Field<K2LR_Reg, 27, 1>; // b123
        using b124 = Field<K2LR_Reg, 28, 1>; // b124
        using b125 = Field<K2LR_Reg, 29, 1>; // b125
        using b126 = Field<K2LR_Reg, 30, 1>; // b126
        using b127 = Field<K2LR_Reg, 31, 1>; // b127
    };

    // key registers
    using K2RR_Reg = Register<CRYP_BASE + 0x34, WriteOnly>;
    struct _K2RR {
        using b64 = Field<K2RR_Reg, 0, 1>; // b64
        using b65 = Field<K2RR_Reg, 1, 1>; // b65
        using b66 = Field<K2RR_Reg, 2, 1>; // b66
        using b67 = Field<K2RR_Reg, 3, 1>; // b67
        using b68 = Field<K2RR_Reg, 4, 1>; // b68
        using b69 = Field<K2RR_Reg, 5, 1>; // b69
        using b70 = Field<K2RR_Reg, 6, 1>; // b70
        using b71 = Field<K2RR_Reg, 7, 1>; // b71
        using b72 = Field<K2RR_Reg, 8, 1>; // b72
        using b73 = Field<K2RR_Reg, 9, 1>; // b73
        using b74 = Field<K2RR_Reg, 10, 1>; // b74
        using b75 = Field<K2RR_Reg, 11, 1>; // b75
        using b76 = Field<K2RR_Reg, 12, 1>; // b76
        using b77 = Field<K2RR_Reg, 13, 1>; // b77
        using b78 = Field<K2RR_Reg, 14, 1>; // b78
        using b79 = Field<K2RR_Reg, 15, 1>; // b79
        using b80 = Field<K2RR_Reg, 16, 1>; // b80
        using b81 = Field<K2RR_Reg, 17, 1>; // b81
        using b82 = Field<K2RR_Reg, 18, 1>; // b82
        using b83 = Field<K2RR_Reg, 19, 1>; // b83
        using b84 = Field<K2RR_Reg, 20, 1>; // b84
        using b85 = Field<K2RR_Reg, 21, 1>; // b85
        using b86 = Field<K2RR_Reg, 22, 1>; // b86
        using b87 = Field<K2RR_Reg, 23, 1>; // b87
        using b88 = Field<K2RR_Reg, 24, 1>; // b88
        using b89 = Field<K2RR_Reg, 25, 1>; // b89
        using b90 = Field<K2RR_Reg, 26, 1>; // b90
        using b91 = Field<K2RR_Reg, 27, 1>; // b91
        using b92 = Field<K2RR_Reg, 28, 1>; // b92
        using b93 = Field<K2RR_Reg, 29, 1>; // b93
        using b94 = Field<K2RR_Reg, 30, 1>; // b94
        using b95 = Field<K2RR_Reg, 31, 1>; // b95
    };

    // key registers
    using K3LR_Reg = Register<CRYP_BASE + 0x38, WriteOnly>;
    struct _K3LR {
        using b32 = Field<K3LR_Reg, 0, 1>; // b32
        using b33 = Field<K3LR_Reg, 1, 1>; // b33
        using b34 = Field<K3LR_Reg, 2, 1>; // b34
        using b35 = Field<K3LR_Reg, 3, 1>; // b35
        using b36 = Field<K3LR_Reg, 4, 1>; // b36
        using b37 = Field<K3LR_Reg, 5, 1>; // b37
        using b38 = Field<K3LR_Reg, 6, 1>; // b38
        using b39 = Field<K3LR_Reg, 7, 1>; // b39
        using b40 = Field<K3LR_Reg, 8, 1>; // b40
        using b41 = Field<K3LR_Reg, 9, 1>; // b41
        using b42 = Field<K3LR_Reg, 10, 1>; // b42
        using b43 = Field<K3LR_Reg, 11, 1>; // b43
        using b44 = Field<K3LR_Reg, 12, 1>; // b44
        using b45 = Field<K3LR_Reg, 13, 1>; // b45
        using b46 = Field<K3LR_Reg, 14, 1>; // b46
        using b47 = Field<K3LR_Reg, 15, 1>; // b47
        using b48 = Field<K3LR_Reg, 16, 1>; // b48
        using b49 = Field<K3LR_Reg, 17, 1>; // b49
        using b50 = Field<K3LR_Reg, 18, 1>; // b50
        using b51 = Field<K3LR_Reg, 19, 1>; // b51
        using b52 = Field<K3LR_Reg, 20, 1>; // b52
        using b53 = Field<K3LR_Reg, 21, 1>; // b53
        using b54 = Field<K3LR_Reg, 22, 1>; // b54
        using b55 = Field<K3LR_Reg, 23, 1>; // b55
        using b56 = Field<K3LR_Reg, 24, 1>; // b56
        using b57 = Field<K3LR_Reg, 25, 1>; // b57
        using b58 = Field<K3LR_Reg, 26, 1>; // b58
        using b59 = Field<K3LR_Reg, 27, 1>; // b59
        using b60 = Field<K3LR_Reg, 28, 1>; // b60
        using b61 = Field<K3LR_Reg, 29, 1>; // b61
        using b62 = Field<K3LR_Reg, 30, 1>; // b62
        using b63 = Field<K3LR_Reg, 31, 1>; // b63
    };

    // key registers
    using K3RR_Reg = Register<CRYP_BASE + 0x3C, WriteOnly>;
    struct _K3RR {
        using b0 = Field<K3RR_Reg, 0, 1>; // b0
        using b1 = Field<K3RR_Reg, 1, 1>; // b1
        using b2 = Field<K3RR_Reg, 2, 1>; // b2
        using b3 = Field<K3RR_Reg, 3, 1>; // b3
        using b4 = Field<K3RR_Reg, 4, 1>; // b4
        using b5 = Field<K3RR_Reg, 5, 1>; // b5
        using b6 = Field<K3RR_Reg, 6, 1>; // b6
        using b7 = Field<K3RR_Reg, 7, 1>; // b7
        using b8 = Field<K3RR_Reg, 8, 1>; // b8
        using b9 = Field<K3RR_Reg, 9, 1>; // b9
        using b10 = Field<K3RR_Reg, 10, 1>; // b10
        using b11 = Field<K3RR_Reg, 11, 1>; // b11
        using b12 = Field<K3RR_Reg, 12, 1>; // b12
        using b13 = Field<K3RR_Reg, 13, 1>; // b13
        using b14 = Field<K3RR_Reg, 14, 1>; // b14
        using b15 = Field<K3RR_Reg, 15, 1>; // b15
        using b16 = Field<K3RR_Reg, 16, 1>; // b16
        using b17 = Field<K3RR_Reg, 17, 1>; // b17
        using b18 = Field<K3RR_Reg, 18, 1>; // b18
        using b19 = Field<K3RR_Reg, 19, 1>; // b19
        using b20 = Field<K3RR_Reg, 20, 1>; // b20
        using b21 = Field<K3RR_Reg, 21, 1>; // b21
        using b22 = Field<K3RR_Reg, 22, 1>; // b22
        using b23 = Field<K3RR_Reg, 23, 1>; // b23
        using b24 = Field<K3RR_Reg, 24, 1>; // b24
        using b25 = Field<K3RR_Reg, 25, 1>; // b25
        using b26 = Field<K3RR_Reg, 26, 1>; // b26
        using b27 = Field<K3RR_Reg, 27, 1>; // b27
        using b28 = Field<K3RR_Reg, 28, 1>; // b28
        using b29 = Field<K3RR_Reg, 29, 1>; // b29
        using b30 = Field<K3RR_Reg, 30, 1>; // b30
        using b31 = Field<K3RR_Reg, 31, 1>; // b31
    };

    // initialization vector registers
    using IV0LR_Reg = Register<CRYP_BASE + 0x40, ReadWrite>;
    struct _IV0LR {
        using IV31 = Field<IV0LR_Reg, 0, 1>; // IV31
        using IV30 = Field<IV0LR_Reg, 1, 1>; // IV30
        using IV29 = Field<IV0LR_Reg, 2, 1>; // IV29
        using IV28 = Field<IV0LR_Reg, 3, 1>; // IV28
        using IV27 = Field<IV0LR_Reg, 4, 1>; // IV27
        using IV26 = Field<IV0LR_Reg, 5, 1>; // IV26
        using IV25 = Field<IV0LR_Reg, 6, 1>; // IV25
        using IV24 = Field<IV0LR_Reg, 7, 1>; // IV24
        using IV23 = Field<IV0LR_Reg, 8, 1>; // IV23
        using IV22 = Field<IV0LR_Reg, 9, 1>; // IV22
        using IV21 = Field<IV0LR_Reg, 10, 1>; // IV21
        using IV20 = Field<IV0LR_Reg, 11, 1>; // IV20
        using IV19 = Field<IV0LR_Reg, 12, 1>; // IV19
        using IV18 = Field<IV0LR_Reg, 13, 1>; // IV18
        using IV17 = Field<IV0LR_Reg, 14, 1>; // IV17
        using IV16 = Field<IV0LR_Reg, 15, 1>; // IV16
        using IV15 = Field<IV0LR_Reg, 16, 1>; // IV15
        using IV14 = Field<IV0LR_Reg, 17, 1>; // IV14
        using IV13 = Field<IV0LR_Reg, 18, 1>; // IV13
        using IV12 = Field<IV0LR_Reg, 19, 1>; // IV12
        using IV11 = Field<IV0LR_Reg, 20, 1>; // IV11
        using IV10 = Field<IV0LR_Reg, 21, 1>; // IV10
        using IV9 = Field<IV0LR_Reg, 22, 1>; // IV9
        using IV8 = Field<IV0LR_Reg, 23, 1>; // IV8
        using IV7 = Field<IV0LR_Reg, 24, 1>; // IV7
        using IV6 = Field<IV0LR_Reg, 25, 1>; // IV6
        using IV5 = Field<IV0LR_Reg, 26, 1>; // IV5
        using IV4 = Field<IV0LR_Reg, 27, 1>; // IV4
        using IV3 = Field<IV0LR_Reg, 28, 1>; // IV3
        using IV2 = Field<IV0LR_Reg, 29, 1>; // IV2
        using IV1 = Field<IV0LR_Reg, 30, 1>; // IV1
        using IV0 = Field<IV0LR_Reg, 31, 1>; // IV0
    };

    // initialization vector registers
    using IV0RR_Reg = Register<CRYP_BASE + 0x44, ReadWrite>;
    struct _IV0RR {
        using IV63 = Field<IV0RR_Reg, 0, 1>; // IV63
        using IV62 = Field<IV0RR_Reg, 1, 1>; // IV62
        using IV61 = Field<IV0RR_Reg, 2, 1>; // IV61
        using IV60 = Field<IV0RR_Reg, 3, 1>; // IV60
        using IV59 = Field<IV0RR_Reg, 4, 1>; // IV59
        using IV58 = Field<IV0RR_Reg, 5, 1>; // IV58
        using IV57 = Field<IV0RR_Reg, 6, 1>; // IV57
        using IV56 = Field<IV0RR_Reg, 7, 1>; // IV56
        using IV55 = Field<IV0RR_Reg, 8, 1>; // IV55
        using IV54 = Field<IV0RR_Reg, 9, 1>; // IV54
        using IV53 = Field<IV0RR_Reg, 10, 1>; // IV53
        using IV52 = Field<IV0RR_Reg, 11, 1>; // IV52
        using IV51 = Field<IV0RR_Reg, 12, 1>; // IV51
        using IV50 = Field<IV0RR_Reg, 13, 1>; // IV50
        using IV49 = Field<IV0RR_Reg, 14, 1>; // IV49
        using IV48 = Field<IV0RR_Reg, 15, 1>; // IV48
        using IV47 = Field<IV0RR_Reg, 16, 1>; // IV47
        using IV46 = Field<IV0RR_Reg, 17, 1>; // IV46
        using IV45 = Field<IV0RR_Reg, 18, 1>; // IV45
        using IV44 = Field<IV0RR_Reg, 19, 1>; // IV44
        using IV43 = Field<IV0RR_Reg, 20, 1>; // IV43
        using IV42 = Field<IV0RR_Reg, 21, 1>; // IV42
        using IV41 = Field<IV0RR_Reg, 22, 1>; // IV41
        using IV40 = Field<IV0RR_Reg, 23, 1>; // IV40
        using IV39 = Field<IV0RR_Reg, 24, 1>; // IV39
        using IV38 = Field<IV0RR_Reg, 25, 1>; // IV38
        using IV37 = Field<IV0RR_Reg, 26, 1>; // IV37
        using IV36 = Field<IV0RR_Reg, 27, 1>; // IV36
        using IV35 = Field<IV0RR_Reg, 28, 1>; // IV35
        using IV34 = Field<IV0RR_Reg, 29, 1>; // IV34
        using IV33 = Field<IV0RR_Reg, 30, 1>; // IV33
        using IV32 = Field<IV0RR_Reg, 31, 1>; // IV32
    };

    // initialization vector registers
    using IV1LR_Reg = Register<CRYP_BASE + 0x48, ReadWrite>;
    struct _IV1LR {
        using IV95 = Field<IV1LR_Reg, 0, 1>; // IV95
        using IV94 = Field<IV1LR_Reg, 1, 1>; // IV94
        using IV93 = Field<IV1LR_Reg, 2, 1>; // IV93
        using IV92 = Field<IV1LR_Reg, 3, 1>; // IV92
        using IV91 = Field<IV1LR_Reg, 4, 1>; // IV91
        using IV90 = Field<IV1LR_Reg, 5, 1>; // IV90
        using IV89 = Field<IV1LR_Reg, 6, 1>; // IV89
        using IV88 = Field<IV1LR_Reg, 7, 1>; // IV88
        using IV87 = Field<IV1LR_Reg, 8, 1>; // IV87
        using IV86 = Field<IV1LR_Reg, 9, 1>; // IV86
        using IV85 = Field<IV1LR_Reg, 10, 1>; // IV85
        using IV84 = Field<IV1LR_Reg, 11, 1>; // IV84
        using IV83 = Field<IV1LR_Reg, 12, 1>; // IV83
        using IV82 = Field<IV1LR_Reg, 13, 1>; // IV82
        using IV81 = Field<IV1LR_Reg, 14, 1>; // IV81
        using IV80 = Field<IV1LR_Reg, 15, 1>; // IV80
        using IV79 = Field<IV1LR_Reg, 16, 1>; // IV79
        using IV78 = Field<IV1LR_Reg, 17, 1>; // IV78
        using IV77 = Field<IV1LR_Reg, 18, 1>; // IV77
        using IV76 = Field<IV1LR_Reg, 19, 1>; // IV76
        using IV75 = Field<IV1LR_Reg, 20, 1>; // IV75
        using IV74 = Field<IV1LR_Reg, 21, 1>; // IV74
        using IV73 = Field<IV1LR_Reg, 22, 1>; // IV73
        using IV72 = Field<IV1LR_Reg, 23, 1>; // IV72
        using IV71 = Field<IV1LR_Reg, 24, 1>; // IV71
        using IV70 = Field<IV1LR_Reg, 25, 1>; // IV70
        using IV69 = Field<IV1LR_Reg, 26, 1>; // IV69
        using IV68 = Field<IV1LR_Reg, 27, 1>; // IV68
        using IV67 = Field<IV1LR_Reg, 28, 1>; // IV67
        using IV66 = Field<IV1LR_Reg, 29, 1>; // IV66
        using IV65 = Field<IV1LR_Reg, 30, 1>; // IV65
        using IV64 = Field<IV1LR_Reg, 31, 1>; // IV64
    };

    // initialization vector registers
    using IV1RR_Reg = Register<CRYP_BASE + 0x4C, ReadWrite>;
    struct _IV1RR {
        using IV127 = Field<IV1RR_Reg, 0, 1>; // IV127
        using IV126 = Field<IV1RR_Reg, 1, 1>; // IV126
        using IV125 = Field<IV1RR_Reg, 2, 1>; // IV125
        using IV124 = Field<IV1RR_Reg, 3, 1>; // IV124
        using IV123 = Field<IV1RR_Reg, 4, 1>; // IV123
        using IV122 = Field<IV1RR_Reg, 5, 1>; // IV122
        using IV121 = Field<IV1RR_Reg, 6, 1>; // IV121
        using IV120 = Field<IV1RR_Reg, 7, 1>; // IV120
        using IV119 = Field<IV1RR_Reg, 8, 1>; // IV119
        using IV118 = Field<IV1RR_Reg, 9, 1>; // IV118
        using IV117 = Field<IV1RR_Reg, 10, 1>; // IV117
        using IV116 = Field<IV1RR_Reg, 11, 1>; // IV116
        using IV115 = Field<IV1RR_Reg, 12, 1>; // IV115
        using IV114 = Field<IV1RR_Reg, 13, 1>; // IV114
        using IV113 = Field<IV1RR_Reg, 14, 1>; // IV113
        using IV112 = Field<IV1RR_Reg, 15, 1>; // IV112
        using IV111 = Field<IV1RR_Reg, 16, 1>; // IV111
        using IV110 = Field<IV1RR_Reg, 17, 1>; // IV110
        using IV109 = Field<IV1RR_Reg, 18, 1>; // IV109
        using IV108 = Field<IV1RR_Reg, 19, 1>; // IV108
        using IV107 = Field<IV1RR_Reg, 20, 1>; // IV107
        using IV106 = Field<IV1RR_Reg, 21, 1>; // IV106
        using IV105 = Field<IV1RR_Reg, 22, 1>; // IV105
        using IV104 = Field<IV1RR_Reg, 23, 1>; // IV104
        using IV103 = Field<IV1RR_Reg, 24, 1>; // IV103
        using IV102 = Field<IV1RR_Reg, 25, 1>; // IV102
        using IV101 = Field<IV1RR_Reg, 26, 1>; // IV101
        using IV100 = Field<IV1RR_Reg, 27, 1>; // IV100
        using IV99 = Field<IV1RR_Reg, 28, 1>; // IV99
        using IV98 = Field<IV1RR_Reg, 29, 1>; // IV98
        using IV97 = Field<IV1RR_Reg, 30, 1>; // IV97
        using IV96 = Field<IV1RR_Reg, 31, 1>; // IV96
    };

    // context swap register
    using CSGCMCCM0R_Reg = Register<CRYP_BASE + 0x50, ReadWrite>;
    struct _CSGCMCCM0R {
        using CSGCMCCM0R = Field<CSGCMCCM0R_Reg, 0, 32>; // CSGCMCCM0R
    };

    // context swap register
    using CSGCMCCM1R_Reg = Register<CRYP_BASE + 0x54, ReadWrite>;
    struct _CSGCMCCM1R {
        using CSGCMCCM1R = Field<CSGCMCCM1R_Reg, 0, 32>; // CSGCMCCM1R
    };

    // context swap register
    using CSGCMCCM2R_Reg = Register<CRYP_BASE + 0x58, ReadWrite>;
    struct _CSGCMCCM2R {
        using CSGCMCCM2R = Field<CSGCMCCM2R_Reg, 0, 32>; // CSGCMCCM2R
    };

    // context swap register
    using CSGCMCCM3R_Reg = Register<CRYP_BASE + 0x5C, ReadWrite>;
    struct _CSGCMCCM3R {
        using CSGCMCCM3R = Field<CSGCMCCM3R_Reg, 0, 32>; // CSGCMCCM3R
    };

    // context swap register
    using CSGCMCCM4R_Reg = Register<CRYP_BASE + 0x60, ReadWrite>;
    struct _CSGCMCCM4R {
        using CSGCMCCM4R = Field<CSGCMCCM4R_Reg, 0, 32>; // CSGCMCCM4R
    };

    // context swap register
    using CSGCMCCM5R_Reg = Register<CRYP_BASE + 0x64, ReadWrite>;
    struct _CSGCMCCM5R {
        using CSGCMCCM5R = Field<CSGCMCCM5R_Reg, 0, 32>; // CSGCMCCM5R
    };

    // context swap register
    using CSGCMCCM6R_Reg = Register<CRYP_BASE + 0x68, ReadWrite>;
    struct _CSGCMCCM6R {
        using CSGCMCCM6R = Field<CSGCMCCM6R_Reg, 0, 32>; // CSGCMCCM6R
    };

    // context swap register
    using CSGCMCCM7R_Reg = Register<CRYP_BASE + 0x6C, ReadWrite>;
    struct _CSGCMCCM7R {
        using CSGCMCCM7R = Field<CSGCMCCM7R_Reg, 0, 32>; // CSGCMCCM7R
    };

    // context swap register
    using CSGCM0R_Reg = Register<CRYP_BASE + 0x70, ReadWrite>;
    struct _CSGCM0R {
        using CSGCM0R = Field<CSGCM0R_Reg, 0, 32>; // CSGCM0R
    };

    // context swap register
    using CSGCM1R_Reg = Register<CRYP_BASE + 0x74, ReadWrite>;
    struct _CSGCM1R {
        using CSGCM1R = Field<CSGCM1R_Reg, 0, 32>; // CSGCM1R
    };

    // context swap register
    using CSGCM2R_Reg = Register<CRYP_BASE + 0x78, ReadWrite>;
    struct _CSGCM2R {
        using CSGCM2R = Field<CSGCM2R_Reg, 0, 32>; // CSGCM2R
    };

    // context swap register
    using CSGCM3R_Reg = Register<CRYP_BASE + 0x7C, ReadWrite>;
    struct _CSGCM3R {
        using CSGCM3R = Field<CSGCM3R_Reg, 0, 32>; // CSGCM3R
    };

    // context swap register
    using CSGCM4R_Reg = Register<CRYP_BASE + 0x80, ReadWrite>;
    struct _CSGCM4R {
        using CSGCM4R = Field<CSGCM4R_Reg, 0, 32>; // CSGCM4R
    };

    // context swap register
    using CSGCM5R_Reg = Register<CRYP_BASE + 0x84, ReadWrite>;
    struct _CSGCM5R {
        using CSGCM5R = Field<CSGCM5R_Reg, 0, 32>; // CSGCM5R
    };

    // context swap register
    using CSGCM6R_Reg = Register<CRYP_BASE + 0x88, ReadWrite>;
    struct _CSGCM6R {
        using CSGCM6R = Field<CSGCM6R_Reg, 0, 32>; // CSGCM6R
    };

    // context swap register
    using CSGCM7R_Reg = Register<CRYP_BASE + 0x8C, ReadWrite>;
    struct _CSGCM7R {
        using CSGCM7R = Field<CSGCM7R_Reg, 0, 32>; // CSGCM7R
    };

} // namespace CRYP

// --------------------------------------------
// DCMI: Digital camera interface
// Base address: 0x50050000
// --------------------------------------------

namespace DCMI {
    static constexpr uint32_t DCMI_BASE = 0x50050000;

    // control register 1
    using CR_Reg = Register<DCMI_BASE + 0x0, ReadWrite>;
    struct _CR {
        using ENABLE = Field<CR_Reg, 14, 1>; // DCMI enable
        using EDM = Field<CR_Reg, 10, 2>; // Extended data mode
        using FCRC = Field<CR_Reg, 8, 2>; // Frame capture rate control
        using VSPOL = Field<CR_Reg, 7, 1>; // Vertical synchronization polarity
        using HSPOL = Field<CR_Reg, 6, 1>; // Horizontal synchronization polarity
        using PCKPOL = Field<CR_Reg, 5, 1>; // Pixel clock polarity
        using ESS = Field<CR_Reg, 4, 1>; // Embedded synchronization select
        using JPEG = Field<CR_Reg, 3, 1>; // JPEG format
        using CROP = Field<CR_Reg, 2, 1>; // Crop feature
        using CM = Field<CR_Reg, 1, 1>; // Capture mode
        using CAPTURE = Field<CR_Reg, 0, 1>; // Capture enable
    };

    // status register
    using SR_Reg = Register<DCMI_BASE + 0x4, ReadOnly>;
    struct _SR {
        using FNE = Field<SR_Reg, 2, 1>; // FIFO not empty
        using VSYNC = Field<SR_Reg, 1, 1>; // VSYNC
        using HSYNC = Field<SR_Reg, 0, 1>; // HSYNC
    };

    // raw interrupt status register
    using RIS_Reg = Register<DCMI_BASE + 0x8, ReadOnly>;
    struct _RIS {
        using LINE_RIS = Field<RIS_Reg, 4, 1>; // Line raw interrupt status
        using VSYNC_RIS = Field<RIS_Reg, 3, 1>; // VSYNC raw interrupt status
        using ERR_RIS = Field<RIS_Reg, 2, 1>; // Synchronization error raw interrupt status
        using OVR_RIS = Field<RIS_Reg, 1, 1>; // Overrun raw interrupt status
        using FRAME_RIS = Field<RIS_Reg, 0, 1>; // Capture complete raw interrupt status
    };

    // interrupt enable register
    using IER_Reg = Register<DCMI_BASE + 0xC, ReadWrite>;
    struct _IER {
        using LINE_IE = Field<IER_Reg, 4, 1>; // Line interrupt enable
        using VSYNC_IE = Field<IER_Reg, 3, 1>; // VSYNC interrupt enable
        using ERR_IE = Field<IER_Reg, 2, 1>; // Synchronization error interrupt enable
        using OVR_IE = Field<IER_Reg, 1, 1>; // Overrun interrupt enable
        using FRAME_IE = Field<IER_Reg, 0, 1>; // Capture complete interrupt enable
    };

    // masked interrupt status register
    using MIS_Reg = Register<DCMI_BASE + 0x10, ReadOnly>;
    struct _MIS {
        using LINE_MIS = Field<MIS_Reg, 4, 1>; // Line masked interrupt status
        using VSYNC_MIS = Field<MIS_Reg, 3, 1>; // VSYNC masked interrupt status
        using ERR_MIS = Field<MIS_Reg, 2, 1>; // Synchronization error masked interrupt status
        using OVR_MIS = Field<MIS_Reg, 1, 1>; // Overrun masked interrupt status
        using FRAME_MIS = Field<MIS_Reg, 0, 1>; // Capture complete masked interrupt status
    };

    // interrupt clear register
    using ICR_Reg = Register<DCMI_BASE + 0x14, WriteOnly>;
    struct _ICR {
        using LINE_ISC = Field<ICR_Reg, 4, 1>; // line interrupt status clear
        using VSYNC_ISC = Field<ICR_Reg, 3, 1>; // Vertical synch interrupt status clear
        using ERR_ISC = Field<ICR_Reg, 2, 1>; // Synchronization error interrupt status clear
        using OVR_ISC = Field<ICR_Reg, 1, 1>; // Overrun interrupt status clear
        using FRAME_ISC = Field<ICR_Reg, 0, 1>; // Capture complete interrupt status clear
    };

    // embedded synchronization code register
    using ESCR_Reg = Register<DCMI_BASE + 0x18, ReadWrite>;
    struct _ESCR {
        using FEC = Field<ESCR_Reg, 24, 8>; // Frame end delimiter code
        using LEC = Field<ESCR_Reg, 16, 8>; // Line end delimiter code
        using LSC = Field<ESCR_Reg, 8, 8>; // Line start delimiter code
        using FSC = Field<ESCR_Reg, 0, 8>; // Frame start delimiter code
    };

    // embedded synchronization unmask register
    using ESUR_Reg = Register<DCMI_BASE + 0x1C, ReadWrite>;
    struct _ESUR {
        using FEU = Field<ESUR_Reg, 24, 8>; // Frame end delimiter unmask
        using LEU = Field<ESUR_Reg, 16, 8>; // Line end delimiter unmask
        using LSU = Field<ESUR_Reg, 8, 8>; // Line start delimiter unmask
        using FSU = Field<ESUR_Reg, 0, 8>; // Frame start delimiter unmask
    };

    // crop window start
    using CWSTRT_Reg = Register<DCMI_BASE + 0x20, ReadWrite>;
    struct _CWSTRT {
        using VST = Field<CWSTRT_Reg, 16, 13>; // Vertical start line count
        using HOFFCNT = Field<CWSTRT_Reg, 0, 14>; // Horizontal offset count
    };

    // crop window size
    using CWSIZE_Reg = Register<DCMI_BASE + 0x24, ReadWrite>;
    struct _CWSIZE {
        using VLINE = Field<CWSIZE_Reg, 16, 14>; // Vertical line count
        using CAPCNT = Field<CWSIZE_Reg, 0, 14>; // Capture count
    };

    // data register
    using DR_Reg = Register<DCMI_BASE + 0x28, ReadOnly>;
    struct _DR {
        using Byte3 = Field<DR_Reg, 24, 8>; // Data byte 3
        using Byte2 = Field<DR_Reg, 16, 8>; // Data byte 2
        using Byte1 = Field<DR_Reg, 8, 8>; // Data byte 1
        using Byte0 = Field<DR_Reg, 0, 8>; // Data byte 0
    };

} // namespace DCMI

// --------------------------------------------
// FMC: Flexible memory controller
// Base address: 0xA0000000
// --------------------------------------------

namespace FMC {
    static constexpr uint32_t FMC_BASE = 0xA0000000;

    // SRAM/NOR-Flash chip-select control register 1
    using BCR1_Reg = Register<FMC_BASE + 0x0, ReadWrite>;
    struct _BCR1 {
        using CCLKEN = Field<BCR1_Reg, 20, 1>; // CCLKEN
        using CBURSTRW = Field<BCR1_Reg, 19, 1>; // CBURSTRW
        using ASYNCWAIT = Field<BCR1_Reg, 15, 1>; // ASYNCWAIT
        using EXTMOD = Field<BCR1_Reg, 14, 1>; // EXTMOD
        using WAITEN = Field<BCR1_Reg, 13, 1>; // WAITEN
        using WREN = Field<BCR1_Reg, 12, 1>; // WREN
        using WAITCFG = Field<BCR1_Reg, 11, 1>; // WAITCFG
        using WAITPOL = Field<BCR1_Reg, 9, 1>; // WAITPOL
        using BURSTEN = Field<BCR1_Reg, 8, 1>; // BURSTEN
        using FACCEN = Field<BCR1_Reg, 6, 1>; // FACCEN
        using MWID = Field<BCR1_Reg, 4, 2>; // MWID
        using MTYP = Field<BCR1_Reg, 2, 2>; // MTYP
        using MUXEN = Field<BCR1_Reg, 1, 1>; // MUXEN
        using MBKEN = Field<BCR1_Reg, 0, 1>; // MBKEN
    };

    // SRAM/NOR-Flash chip-select timing register 1
    using BTR1_Reg = Register<FMC_BASE + 0x4, ReadWrite>;
    struct _BTR1 {
        using ACCMOD = Field<BTR1_Reg, 28, 2>; // ACCMOD
        using DATLAT = Field<BTR1_Reg, 24, 4>; // DATLAT
        using CLKDIV = Field<BTR1_Reg, 20, 4>; // CLKDIV
        using BUSTURN = Field<BTR1_Reg, 16, 4>; // BUSTURN
        using DATAST = Field<BTR1_Reg, 8, 8>; // DATAST
        using ADDHLD = Field<BTR1_Reg, 4, 4>; // ADDHLD
        using ADDSET = Field<BTR1_Reg, 0, 4>; // ADDSET
    };

    // SRAM/NOR-Flash chip-select control register 2
    using BCR2_Reg = Register<FMC_BASE + 0x8, ReadWrite>;
    struct _BCR2 {
        using CBURSTRW = Field<BCR2_Reg, 19, 1>; // CBURSTRW
        using ASYNCWAIT = Field<BCR2_Reg, 15, 1>; // ASYNCWAIT
        using EXTMOD = Field<BCR2_Reg, 14, 1>; // EXTMOD
        using WAITEN = Field<BCR2_Reg, 13, 1>; // WAITEN
        using WREN = Field<BCR2_Reg, 12, 1>; // WREN
        using WAITCFG = Field<BCR2_Reg, 11, 1>; // WAITCFG
        using WRAPMOD = Field<BCR2_Reg, 10, 1>; // WRAPMOD
        using WAITPOL = Field<BCR2_Reg, 9, 1>; // WAITPOL
        using BURSTEN = Field<BCR2_Reg, 8, 1>; // BURSTEN
        using FACCEN = Field<BCR2_Reg, 6, 1>; // FACCEN
        using MWID = Field<BCR2_Reg, 4, 2>; // MWID
        using MTYP = Field<BCR2_Reg, 2, 2>; // MTYP
        using MUXEN = Field<BCR2_Reg, 1, 1>; // MUXEN
        using MBKEN = Field<BCR2_Reg, 0, 1>; // MBKEN
    };

    // SRAM/NOR-Flash chip-select timing register 2
    using BTR2_Reg = Register<FMC_BASE + 0xC, ReadWrite>;
    struct _BTR2 {
        using ACCMOD = Field<BTR2_Reg, 28, 2>; // ACCMOD
        using DATLAT = Field<BTR2_Reg, 24, 4>; // DATLAT
        using CLKDIV = Field<BTR2_Reg, 20, 4>; // CLKDIV
        using BUSTURN = Field<BTR2_Reg, 16, 4>; // BUSTURN
        using DATAST = Field<BTR2_Reg, 8, 8>; // DATAST
        using ADDHLD = Field<BTR2_Reg, 4, 4>; // ADDHLD
        using ADDSET = Field<BTR2_Reg, 0, 4>; // ADDSET
    };

    // SRAM/NOR-Flash chip-select control register 3
    using BCR3_Reg = Register<FMC_BASE + 0x10, ReadWrite>;
    struct _BCR3 {
        using CBURSTRW = Field<BCR3_Reg, 19, 1>; // CBURSTRW
        using ASYNCWAIT = Field<BCR3_Reg, 15, 1>; // ASYNCWAIT
        using EXTMOD = Field<BCR3_Reg, 14, 1>; // EXTMOD
        using WAITEN = Field<BCR3_Reg, 13, 1>; // WAITEN
        using WREN = Field<BCR3_Reg, 12, 1>; // WREN
        using WAITCFG = Field<BCR3_Reg, 11, 1>; // WAITCFG
        using WRAPMOD = Field<BCR3_Reg, 10, 1>; // WRAPMOD
        using WAITPOL = Field<BCR3_Reg, 9, 1>; // WAITPOL
        using BURSTEN = Field<BCR3_Reg, 8, 1>; // BURSTEN
        using FACCEN = Field<BCR3_Reg, 6, 1>; // FACCEN
        using MWID = Field<BCR3_Reg, 4, 2>; // MWID
        using MTYP = Field<BCR3_Reg, 2, 2>; // MTYP
        using MUXEN = Field<BCR3_Reg, 1, 1>; // MUXEN
        using MBKEN = Field<BCR3_Reg, 0, 1>; // MBKEN
    };

    // SRAM/NOR-Flash chip-select timing register 3
    using BTR3_Reg = Register<FMC_BASE + 0x14, ReadWrite>;
    struct _BTR3 {
        using ACCMOD = Field<BTR3_Reg, 28, 2>; // ACCMOD
        using DATLAT = Field<BTR3_Reg, 24, 4>; // DATLAT
        using CLKDIV = Field<BTR3_Reg, 20, 4>; // CLKDIV
        using BUSTURN = Field<BTR3_Reg, 16, 4>; // BUSTURN
        using DATAST = Field<BTR3_Reg, 8, 8>; // DATAST
        using ADDHLD = Field<BTR3_Reg, 4, 4>; // ADDHLD
        using ADDSET = Field<BTR3_Reg, 0, 4>; // ADDSET
    };

    // SRAM/NOR-Flash chip-select control register 4
    using BCR4_Reg = Register<FMC_BASE + 0x18, ReadWrite>;
    struct _BCR4 {
        using CBURSTRW = Field<BCR4_Reg, 19, 1>; // CBURSTRW
        using ASYNCWAIT = Field<BCR4_Reg, 15, 1>; // ASYNCWAIT
        using EXTMOD = Field<BCR4_Reg, 14, 1>; // EXTMOD
        using WAITEN = Field<BCR4_Reg, 13, 1>; // WAITEN
        using WREN = Field<BCR4_Reg, 12, 1>; // WREN
        using WAITCFG = Field<BCR4_Reg, 11, 1>; // WAITCFG
        using WRAPMOD = Field<BCR4_Reg, 10, 1>; // WRAPMOD
        using WAITPOL = Field<BCR4_Reg, 9, 1>; // WAITPOL
        using BURSTEN = Field<BCR4_Reg, 8, 1>; // BURSTEN
        using FACCEN = Field<BCR4_Reg, 6, 1>; // FACCEN
        using MWID = Field<BCR4_Reg, 4, 2>; // MWID
        using MTYP = Field<BCR4_Reg, 2, 2>; // MTYP
        using MUXEN = Field<BCR4_Reg, 1, 1>; // MUXEN
        using MBKEN = Field<BCR4_Reg, 0, 1>; // MBKEN
    };

    // SRAM/NOR-Flash chip-select timing register 4
    using BTR4_Reg = Register<FMC_BASE + 0x1C, ReadWrite>;
    struct _BTR4 {
        using ACCMOD = Field<BTR4_Reg, 28, 2>; // ACCMOD
        using DATLAT = Field<BTR4_Reg, 24, 4>; // DATLAT
        using CLKDIV = Field<BTR4_Reg, 20, 4>; // CLKDIV
        using BUSTURN = Field<BTR4_Reg, 16, 4>; // BUSTURN
        using DATAST = Field<BTR4_Reg, 8, 8>; // DATAST
        using ADDHLD = Field<BTR4_Reg, 4, 4>; // ADDHLD
        using ADDSET = Field<BTR4_Reg, 0, 4>; // ADDSET
    };

    // PC Card/NAND Flash control register
    using PCR_Reg = Register<FMC_BASE + 0x80, ReadWrite>;
    struct _PCR {
        using ECCPS = Field<PCR_Reg, 17, 3>; // ECCPS
        using TAR = Field<PCR_Reg, 13, 4>; // TAR
        using TCLR = Field<PCR_Reg, 9, 4>; // TCLR
        using ECCEN = Field<PCR_Reg, 6, 1>; // ECCEN
        using PWID = Field<PCR_Reg, 4, 2>; // PWID
        using PTYP = Field<PCR_Reg, 3, 1>; // PTYP
        using PBKEN = Field<PCR_Reg, 2, 1>; // PBKEN
        using PWAITEN = Field<PCR_Reg, 1, 1>; // PWAITEN
    };

    // FIFO status and interrupt register
    using SR_Reg = Register<FMC_BASE + 0x84, ReadWrite>;
    struct _SR {
        using FEMPT = Field<SR_Reg, 6, 1>; // FEMPT
        using IFEN = Field<SR_Reg, 5, 1>; // IFEN
        using ILEN = Field<SR_Reg, 4, 1>; // ILEN
        using IREN = Field<SR_Reg, 3, 1>; // IREN
        using IFS = Field<SR_Reg, 2, 1>; // IFS
        using ILS = Field<SR_Reg, 1, 1>; // ILS
        using IRS = Field<SR_Reg, 0, 1>; // IRS
    };

    // Common memory space timing register
    using PMEM_Reg = Register<FMC_BASE + 0x88, ReadWrite>;
    struct _PMEM {
        using MEMHIZx = Field<PMEM_Reg, 24, 8>; // MEMHIZx
        using MEMHOLDx = Field<PMEM_Reg, 16, 8>; // MEMHOLDx
        using MEMWAITx = Field<PMEM_Reg, 8, 8>; // MEMWAITx
        using MEMSETx = Field<PMEM_Reg, 0, 8>; // MEMSETx
    };

    // Attribute memory space timing register
    using PATT_Reg = Register<FMC_BASE + 0x8C, ReadWrite>;
    struct _PATT {
        using ATTHIZx = Field<PATT_Reg, 24, 8>; // ATTHIZx
        using ATTHOLDx = Field<PATT_Reg, 16, 8>; // ATTHOLDx
        using ATTWAITx = Field<PATT_Reg, 8, 8>; // ATTWAITx
        using ATTSETx = Field<PATT_Reg, 0, 8>; // ATTSETx
    };

    // ECC result register
    using ECCR_Reg = Register<FMC_BASE + 0x94, ReadOnly>;
    struct _ECCR {
        using ECCx = Field<ECCR_Reg, 0, 32>; // ECCx
    };

    // SRAM/NOR-Flash write timing registers 1
    using BWTR1_Reg = Register<FMC_BASE + 0x104, ReadWrite>;
    struct _BWTR1 {
        using ACCMOD = Field<BWTR1_Reg, 28, 2>; // ACCMOD
        using DATLAT = Field<BWTR1_Reg, 24, 4>; // DATLAT
        using CLKDIV = Field<BWTR1_Reg, 20, 4>; // CLKDIV
        using DATAST = Field<BWTR1_Reg, 8, 8>; // DATAST
        using ADDHLD = Field<BWTR1_Reg, 4, 4>; // ADDHLD
        using ADDSET = Field<BWTR1_Reg, 0, 4>; // ADDSET
    };

    // SRAM/NOR-Flash write timing registers 2
    using BWTR2_Reg = Register<FMC_BASE + 0x10C, ReadWrite>;
    struct _BWTR2 {
        using ACCMOD = Field<BWTR2_Reg, 28, 2>; // ACCMOD
        using DATLAT = Field<BWTR2_Reg, 24, 4>; // DATLAT
        using CLKDIV = Field<BWTR2_Reg, 20, 4>; // CLKDIV
        using DATAST = Field<BWTR2_Reg, 8, 8>; // DATAST
        using ADDHLD = Field<BWTR2_Reg, 4, 4>; // ADDHLD
        using ADDSET = Field<BWTR2_Reg, 0, 4>; // ADDSET
    };

    // SRAM/NOR-Flash write timing registers 3
    using BWTR3_Reg = Register<FMC_BASE + 0x114, ReadWrite>;
    struct _BWTR3 {
        using ACCMOD = Field<BWTR3_Reg, 28, 2>; // ACCMOD
        using DATLAT = Field<BWTR3_Reg, 24, 4>; // DATLAT
        using CLKDIV = Field<BWTR3_Reg, 20, 4>; // CLKDIV
        using DATAST = Field<BWTR3_Reg, 8, 8>; // DATAST
        using ADDHLD = Field<BWTR3_Reg, 4, 4>; // ADDHLD
        using ADDSET = Field<BWTR3_Reg, 0, 4>; // ADDSET
    };

    // SRAM/NOR-Flash write timing registers 4
    using BWTR4_Reg = Register<FMC_BASE + 0x11C, ReadWrite>;
    struct _BWTR4 {
        using ACCMOD = Field<BWTR4_Reg, 28, 2>; // ACCMOD
        using DATLAT = Field<BWTR4_Reg, 24, 4>; // DATLAT
        using CLKDIV = Field<BWTR4_Reg, 20, 4>; // CLKDIV
        using DATAST = Field<BWTR4_Reg, 8, 8>; // DATAST
        using ADDHLD = Field<BWTR4_Reg, 4, 4>; // ADDHLD
        using ADDSET = Field<BWTR4_Reg, 0, 4>; // ADDSET
    };

    // SDRAM Control Register 1
    using SDCR1_Reg = Register<FMC_BASE + 0x140, ReadWrite>;
    struct _SDCR1 {
        using NC = Field<SDCR1_Reg, 0, 2>; // Number of column address bits
        using NR = Field<SDCR1_Reg, 2, 2>; // Number of row address bits
        using MWID = Field<SDCR1_Reg, 4, 2>; // Memory data bus width
        using NB = Field<SDCR1_Reg, 6, 1>; // Number of internal banks
        using CAS = Field<SDCR1_Reg, 7, 2>; // CAS latency
        using WP = Field<SDCR1_Reg, 9, 1>; // Write protection
        using SDCLK = Field<SDCR1_Reg, 10, 2>; // SDRAM clock configuration
        using RBURST = Field<SDCR1_Reg, 12, 1>; // Burst read
        using RPIPE = Field<SDCR1_Reg, 13, 2>; // Read pipe
    };

    // SDRAM Control Register 2
    using SDCR2_Reg = Register<FMC_BASE + 0x144, ReadWrite>;
    struct _SDCR2 {
        using NC = Field<SDCR2_Reg, 0, 2>; // Number of column address bits
        using NR = Field<SDCR2_Reg, 2, 2>; // Number of row address bits
        using MWID = Field<SDCR2_Reg, 4, 2>; // Memory data bus width
        using NB = Field<SDCR2_Reg, 6, 1>; // Number of internal banks
        using CAS = Field<SDCR2_Reg, 7, 2>; // CAS latency
        using WP = Field<SDCR2_Reg, 9, 1>; // Write protection
        using SDCLK = Field<SDCR2_Reg, 10, 2>; // SDRAM clock configuration
        using RBURST = Field<SDCR2_Reg, 12, 1>; // Burst read
        using RPIPE = Field<SDCR2_Reg, 13, 2>; // Read pipe
    };

    // SDRAM Timing register 1
    using SDTR1_Reg = Register<FMC_BASE + 0x148, ReadWrite>;
    struct _SDTR1 {
        using TMRD = Field<SDTR1_Reg, 0, 4>; // Load Mode Register to Active
        using TXSR = Field<SDTR1_Reg, 4, 4>; // Exit self-refresh delay
        using TRAS = Field<SDTR1_Reg, 8, 4>; // Self refresh time
        using TRC = Field<SDTR1_Reg, 12, 4>; // Row cycle delay
        using TWR = Field<SDTR1_Reg, 16, 4>; // Recovery delay
        using TRP = Field<SDTR1_Reg, 20, 4>; // Row precharge delay
        using TRCD = Field<SDTR1_Reg, 24, 4>; // Row to column delay
    };

    // SDRAM Timing register 2
    using SDTR2_Reg = Register<FMC_BASE + 0x14C, ReadWrite>;
    struct _SDTR2 {
        using TMRD = Field<SDTR2_Reg, 0, 4>; // Load Mode Register to Active
        using TXSR = Field<SDTR2_Reg, 4, 4>; // Exit self-refresh delay
        using TRAS = Field<SDTR2_Reg, 8, 4>; // Self refresh time
        using TRC = Field<SDTR2_Reg, 12, 4>; // Row cycle delay
        using TWR = Field<SDTR2_Reg, 16, 4>; // Recovery delay
        using TRP = Field<SDTR2_Reg, 20, 4>; // Row precharge delay
        using TRCD = Field<SDTR2_Reg, 24, 4>; // Row to column delay
    };

    // SDRAM Command Mode register
    using SDCMR_Reg = Register<FMC_BASE + 0x150, ReadWrite>;
    struct _SDCMR {
        using MODE = Field<SDCMR_Reg, 0, 3>; // Command mode
        using CTB2 = Field<SDCMR_Reg, 3, 1>; // Command target bank 2
        using CTB1 = Field<SDCMR_Reg, 4, 1>; // Command target bank 1
        using NRFS = Field<SDCMR_Reg, 5, 4>; // Number of Auto-refresh
        using MRD = Field<SDCMR_Reg, 9, 13>; // Mode Register definition
    };

    // SDRAM Refresh Timer register
    using SDRTR_Reg = Register<FMC_BASE + 0x154, ReadWrite>;
    struct _SDRTR {
        using CRE = Field<SDRTR_Reg, 0, 1>; // Clear Refresh error flag
        using COUNT = Field<SDRTR_Reg, 1, 13>; // Refresh Timer Count
        using REIE = Field<SDRTR_Reg, 14, 1>; // RES Interrupt Enable
    };

    // SDRAM Status register
    using SDSR_Reg = Register<FMC_BASE + 0x158, ReadOnly>;
    struct _SDSR {
        using RE = Field<SDSR_Reg, 0, 1>; // Refresh error flag
        using MODES1 = Field<SDSR_Reg, 1, 2>; // Status Mode for Bank 1
        using MODES2 = Field<SDSR_Reg, 3, 2>; // Status Mode for Bank 2
        using BUSY = Field<SDSR_Reg, 5, 1>; // Busy status
    };

} // namespace FMC

// --------------------------------------------
// DMA2: DMA controller
// Base address: 0x40026400
// --------------------------------------------

namespace DMA2 {
    static constexpr uint32_t DMA2_BASE = 0x40026400;

    // low interrupt status register
    using LISR_Reg = Register<DMA2_BASE + 0x0, ReadOnly>;
    struct _LISR {
        using TCIF3 = Field<LISR_Reg, 27, 1>; // Stream x transfer complete interrupt flag (x = 3..0)
        using HTIF3 = Field<LISR_Reg, 26, 1>; // Stream x half transfer interrupt flag (x=3..0)
        using TEIF3 = Field<LISR_Reg, 25, 1>; // Stream x transfer error interrupt flag (x=3..0)
        using DMEIF3 = Field<LISR_Reg, 24, 1>; // Stream x direct mode error interrupt flag (x=3..0)
        using FEIF3 = Field<LISR_Reg, 22, 1>; // Stream x FIFO error interrupt flag (x=3..0)
        using TCIF2 = Field<LISR_Reg, 21, 1>; // Stream x transfer complete interrupt flag (x = 3..0)
        using HTIF2 = Field<LISR_Reg, 20, 1>; // Stream x half transfer interrupt flag (x=3..0)
        using TEIF2 = Field<LISR_Reg, 19, 1>; // Stream x transfer error interrupt flag (x=3..0)
        using DMEIF2 = Field<LISR_Reg, 18, 1>; // Stream x direct mode error interrupt flag (x=3..0)
        using FEIF2 = Field<LISR_Reg, 16, 1>; // Stream x FIFO error interrupt flag (x=3..0)
        using TCIF1 = Field<LISR_Reg, 11, 1>; // Stream x transfer complete interrupt flag (x = 3..0)
        using HTIF1 = Field<LISR_Reg, 10, 1>; // Stream x half transfer interrupt flag (x=3..0)
        using TEIF1 = Field<LISR_Reg, 9, 1>; // Stream x transfer error interrupt flag (x=3..0)
        using DMEIF1 = Field<LISR_Reg, 8, 1>; // Stream x direct mode error interrupt flag (x=3..0)
        using FEIF1 = Field<LISR_Reg, 6, 1>; // Stream x FIFO error interrupt flag (x=3..0)
        using TCIF0 = Field<LISR_Reg, 5, 1>; // Stream x transfer complete interrupt flag (x = 3..0)
        using HTIF0 = Field<LISR_Reg, 4, 1>; // Stream x half transfer interrupt flag (x=3..0)
        using TEIF0 = Field<LISR_Reg, 3, 1>; // Stream x transfer error interrupt flag (x=3..0)
        using DMEIF0 = Field<LISR_Reg, 2, 1>; // Stream x direct mode error interrupt flag (x=3..0)
        using FEIF0 = Field<LISR_Reg, 0, 1>; // Stream x FIFO error interrupt flag (x=3..0)
    };

    // high interrupt status register
    using HISR_Reg = Register<DMA2_BASE + 0x4, ReadOnly>;
    struct _HISR {
        using TCIF7 = Field<HISR_Reg, 27, 1>; // Stream x transfer complete interrupt flag (x=7..4)
        using HTIF7 = Field<HISR_Reg, 26, 1>; // Stream x half transfer interrupt flag (x=7..4)
        using TEIF7 = Field<HISR_Reg, 25, 1>; // Stream x transfer error interrupt flag (x=7..4)
        using DMEIF7 = Field<HISR_Reg, 24, 1>; // Stream x direct mode error interrupt flag (x=7..4)
        using FEIF7 = Field<HISR_Reg, 22, 1>; // Stream x FIFO error interrupt flag (x=7..4)
        using TCIF6 = Field<HISR_Reg, 21, 1>; // Stream x transfer complete interrupt flag (x=7..4)
        using HTIF6 = Field<HISR_Reg, 20, 1>; // Stream x half transfer interrupt flag (x=7..4)
        using TEIF6 = Field<HISR_Reg, 19, 1>; // Stream x transfer error interrupt flag (x=7..4)
        using DMEIF6 = Field<HISR_Reg, 18, 1>; // Stream x direct mode error interrupt flag (x=7..4)
        using FEIF6 = Field<HISR_Reg, 16, 1>; // Stream x FIFO error interrupt flag (x=7..4)
        using TCIF5 = Field<HISR_Reg, 11, 1>; // Stream x transfer complete interrupt flag (x=7..4)
        using HTIF5 = Field<HISR_Reg, 10, 1>; // Stream x half transfer interrupt flag (x=7..4)
        using TEIF5 = Field<HISR_Reg, 9, 1>; // Stream x transfer error interrupt flag (x=7..4)
        using DMEIF5 = Field<HISR_Reg, 8, 1>; // Stream x direct mode error interrupt flag (x=7..4)
        using FEIF5 = Field<HISR_Reg, 6, 1>; // Stream x FIFO error interrupt flag (x=7..4)
        using TCIF4 = Field<HISR_Reg, 5, 1>; // Stream x transfer complete interrupt flag (x=7..4)
        using HTIF4 = Field<HISR_Reg, 4, 1>; // Stream x half transfer interrupt flag (x=7..4)
        using TEIF4 = Field<HISR_Reg, 3, 1>; // Stream x transfer error interrupt flag (x=7..4)
        using DMEIF4 = Field<HISR_Reg, 2, 1>; // Stream x direct mode error interrupt flag (x=7..4)
        using FEIF4 = Field<HISR_Reg, 0, 1>; // Stream x FIFO error interrupt flag (x=7..4)
    };

    // low interrupt flag clear register
    using LIFCR_Reg = Register<DMA2_BASE + 0x8, ReadWrite>;
    struct _LIFCR {
        using CTCIF3 = Field<LIFCR_Reg, 27, 1>; // Stream x clear transfer complete interrupt flag (x = 3..0)
        using CHTIF3 = Field<LIFCR_Reg, 26, 1>; // Stream x clear half transfer interrupt flag (x = 3..0)
        using CTEIF3 = Field<LIFCR_Reg, 25, 1>; // Stream x clear transfer error interrupt flag (x = 3..0)
        using CDMEIF3 = Field<LIFCR_Reg, 24, 1>; // Stream x clear direct mode error interrupt flag (x = 3..0)
        using CFEIF3 = Field<LIFCR_Reg, 22, 1>; // Stream x clear FIFO error interrupt flag (x = 3..0)
        using CTCIF2 = Field<LIFCR_Reg, 21, 1>; // Stream x clear transfer complete interrupt flag (x = 3..0)
        using CHTIF2 = Field<LIFCR_Reg, 20, 1>; // Stream x clear half transfer interrupt flag (x = 3..0)
        using CTEIF2 = Field<LIFCR_Reg, 19, 1>; // Stream x clear transfer error interrupt flag (x = 3..0)
        using CDMEIF2 = Field<LIFCR_Reg, 18, 1>; // Stream x clear direct mode error interrupt flag (x = 3..0)
        using CFEIF2 = Field<LIFCR_Reg, 16, 1>; // Stream x clear FIFO error interrupt flag (x = 3..0)
        using CTCIF1 = Field<LIFCR_Reg, 11, 1>; // Stream x clear transfer complete interrupt flag (x = 3..0)
        using CHTIF1 = Field<LIFCR_Reg, 10, 1>; // Stream x clear half transfer interrupt flag (x = 3..0)
        using CTEIF1 = Field<LIFCR_Reg, 9, 1>; // Stream x clear transfer error interrupt flag (x = 3..0)
        using CDMEIF1 = Field<LIFCR_Reg, 8, 1>; // Stream x clear direct mode error interrupt flag (x = 3..0)
        using CFEIF1 = Field<LIFCR_Reg, 6, 1>; // Stream x clear FIFO error interrupt flag (x = 3..0)
        using CTCIF0 = Field<LIFCR_Reg, 5, 1>; // Stream x clear transfer complete interrupt flag (x = 3..0)
        using CHTIF0 = Field<LIFCR_Reg, 4, 1>; // Stream x clear half transfer interrupt flag (x = 3..0)
        using CTEIF0 = Field<LIFCR_Reg, 3, 1>; // Stream x clear transfer error interrupt flag (x = 3..0)
        using CDMEIF0 = Field<LIFCR_Reg, 2, 1>; // Stream x clear direct mode error interrupt flag (x = 3..0)
        using CFEIF0 = Field<LIFCR_Reg, 0, 1>; // Stream x clear FIFO error interrupt flag (x = 3..0)
    };

    // high interrupt flag clear register
    using HIFCR_Reg = Register<DMA2_BASE + 0xC, ReadWrite>;
    struct _HIFCR {
        using CTCIF7 = Field<HIFCR_Reg, 27, 1>; // Stream x clear transfer complete interrupt flag (x = 7..4)
        using CHTIF7 = Field<HIFCR_Reg, 26, 1>; // Stream x clear half transfer interrupt flag (x = 7..4)
        using CTEIF7 = Field<HIFCR_Reg, 25, 1>; // Stream x clear transfer error interrupt flag (x = 7..4)
        using CDMEIF7 = Field<HIFCR_Reg, 24, 1>; // Stream x clear direct mode error interrupt flag (x = 7..4)
        using CFEIF7 = Field<HIFCR_Reg, 22, 1>; // Stream x clear FIFO error interrupt flag (x = 7..4)
        using CTCIF6 = Field<HIFCR_Reg, 21, 1>; // Stream x clear transfer complete interrupt flag (x = 7..4)
        using CHTIF6 = Field<HIFCR_Reg, 20, 1>; // Stream x clear half transfer interrupt flag (x = 7..4)
        using CTEIF6 = Field<HIFCR_Reg, 19, 1>; // Stream x clear transfer error interrupt flag (x = 7..4)
        using CDMEIF6 = Field<HIFCR_Reg, 18, 1>; // Stream x clear direct mode error interrupt flag (x = 7..4)
        using CFEIF6 = Field<HIFCR_Reg, 16, 1>; // Stream x clear FIFO error interrupt flag (x = 7..4)
        using CTCIF5 = Field<HIFCR_Reg, 11, 1>; // Stream x clear transfer complete interrupt flag (x = 7..4)
        using CHTIF5 = Field<HIFCR_Reg, 10, 1>; // Stream x clear half transfer interrupt flag (x = 7..4)
        using CTEIF5 = Field<HIFCR_Reg, 9, 1>; // Stream x clear transfer error interrupt flag (x = 7..4)
        using CDMEIF5 = Field<HIFCR_Reg, 8, 1>; // Stream x clear direct mode error interrupt flag (x = 7..4)
        using CFEIF5 = Field<HIFCR_Reg, 6, 1>; // Stream x clear FIFO error interrupt flag (x = 7..4)
        using CTCIF4 = Field<HIFCR_Reg, 5, 1>; // Stream x clear transfer complete interrupt flag (x = 7..4)
        using CHTIF4 = Field<HIFCR_Reg, 4, 1>; // Stream x clear half transfer interrupt flag (x = 7..4)
        using CTEIF4 = Field<HIFCR_Reg, 3, 1>; // Stream x clear transfer error interrupt flag (x = 7..4)
        using CDMEIF4 = Field<HIFCR_Reg, 2, 1>; // Stream x clear direct mode error interrupt flag (x = 7..4)
        using CFEIF4 = Field<HIFCR_Reg, 0, 1>; // Stream x clear FIFO error interrupt flag (x = 7..4)
    };

    // stream x configuration register
    using S0CR_Reg = Register<DMA2_BASE + 0x10, ReadWrite>;
    struct _S0CR {
        using CHSEL = Field<S0CR_Reg, 25, 4>; // Channel selection
        using MBURST = Field<S0CR_Reg, 23, 2>; // Memory burst transfer configuration
        using PBURST = Field<S0CR_Reg, 21, 2>; // Peripheral burst transfer configuration
        using CT = Field<S0CR_Reg, 19, 1>; // Current target (only in double buffer mode)
        using DBM = Field<S0CR_Reg, 18, 1>; // Double buffer mode
        using PL = Field<S0CR_Reg, 16, 2>; // Priority level
        using PINCOS = Field<S0CR_Reg, 15, 1>; // Peripheral increment offset size
        using MSIZE = Field<S0CR_Reg, 13, 2>; // Memory data size
        using PSIZE = Field<S0CR_Reg, 11, 2>; // Peripheral data size
        using MINC = Field<S0CR_Reg, 10, 1>; // Memory increment mode
        using PINC = Field<S0CR_Reg, 9, 1>; // Peripheral increment mode
        using CIRC = Field<S0CR_Reg, 8, 1>; // Circular mode
        using DIR = Field<S0CR_Reg, 6, 2>; // Data transfer direction
        using PFCTRL = Field<S0CR_Reg, 5, 1>; // Peripheral flow controller
        using TCIE = Field<S0CR_Reg, 4, 1>; // Transfer complete interrupt enable
        using HTIE = Field<S0CR_Reg, 3, 1>; // Half transfer interrupt enable
        using TEIE = Field<S0CR_Reg, 2, 1>; // Transfer error interrupt enable
        using DMEIE = Field<S0CR_Reg, 1, 1>; // Direct mode error interrupt enable
        using EN = Field<S0CR_Reg, 0, 1>; // Stream enable / flag stream ready when read low
    };

    // stream x number of data register
    using S0NDTR_Reg = Register<DMA2_BASE + 0x14, ReadWrite>;
    struct _S0NDTR {
        using NDT = Field<S0NDTR_Reg, 0, 16>; // Number of data items to transfer
    };

    // stream x peripheral address register
    using S0PAR_Reg = Register<DMA2_BASE + 0x18, ReadWrite>;
    struct _S0PAR {
        using PA = Field<S0PAR_Reg, 0, 32>; // Peripheral address
    };

    // stream x memory 0 address register
    using S0M0AR_Reg = Register<DMA2_BASE + 0x1C, ReadWrite>;
    struct _S0M0AR {
        using M0A = Field<S0M0AR_Reg, 0, 32>; // Memory 0 address
    };

    // stream x memory 1 address register
    using S0M1AR_Reg = Register<DMA2_BASE + 0x20, ReadWrite>;
    struct _S0M1AR {
        using M1A = Field<S0M1AR_Reg, 0, 32>; // Memory 1 address (used in case of Double buffer mode)
    };

    // stream x FIFO control register
    using S0FCR_Reg = Register<DMA2_BASE + 0x24, ReadWrite>;
    struct _S0FCR {
        using FEIE = Field<S0FCR_Reg, 7, 1>; // FIFO error interrupt enable
        using FS = Field<S0FCR_Reg, 3, 3>; // FIFO status
        using DMDIS = Field<S0FCR_Reg, 2, 1>; // Direct mode disable
        using FTH = Field<S0FCR_Reg, 0, 2>; // FIFO threshold selection
    };

    // stream x configuration register
    using S1CR_Reg = Register<DMA2_BASE + 0x28, ReadWrite>;
    struct _S1CR {
        using CHSEL = Field<S1CR_Reg, 25, 4>; // Channel selection
        using MBURST = Field<S1CR_Reg, 23, 2>; // Memory burst transfer configuration
        using PBURST = Field<S1CR_Reg, 21, 2>; // Peripheral burst transfer configuration
        using ACK = Field<S1CR_Reg, 20, 1>; // ACK
        using CT = Field<S1CR_Reg, 19, 1>; // Current target (only in double buffer mode)
        using DBM = Field<S1CR_Reg, 18, 1>; // Double buffer mode
        using PL = Field<S1CR_Reg, 16, 2>; // Priority level
        using PINCOS = Field<S1CR_Reg, 15, 1>; // Peripheral increment offset size
        using MSIZE = Field<S1CR_Reg, 13, 2>; // Memory data size
        using PSIZE = Field<S1CR_Reg, 11, 2>; // Peripheral data size
        using MINC = Field<S1CR_Reg, 10, 1>; // Memory increment mode
        using PINC = Field<S1CR_Reg, 9, 1>; // Peripheral increment mode
        using CIRC = Field<S1CR_Reg, 8, 1>; // Circular mode
        using DIR = Field<S1CR_Reg, 6, 2>; // Data transfer direction
        using PFCTRL = Field<S1CR_Reg, 5, 1>; // Peripheral flow controller
        using TCIE = Field<S1CR_Reg, 4, 1>; // Transfer complete interrupt enable
        using HTIE = Field<S1CR_Reg, 3, 1>; // Half transfer interrupt enable
        using TEIE = Field<S1CR_Reg, 2, 1>; // Transfer error interrupt enable
        using DMEIE = Field<S1CR_Reg, 1, 1>; // Direct mode error interrupt enable
        using EN = Field<S1CR_Reg, 0, 1>; // Stream enable / flag stream ready when read low
    };

    // stream x number of data register
    using S1NDTR_Reg = Register<DMA2_BASE + 0x2C, ReadWrite>;
    struct _S1NDTR {
        using NDT = Field<S1NDTR_Reg, 0, 16>; // Number of data items to transfer
    };

    // stream x peripheral address register
    using S1PAR_Reg = Register<DMA2_BASE + 0x30, ReadWrite>;
    struct _S1PAR {
        using PA = Field<S1PAR_Reg, 0, 32>; // Peripheral address
    };

    // stream x memory 0 address register
    using S1M0AR_Reg = Register<DMA2_BASE + 0x34, ReadWrite>;
    struct _S1M0AR {
        using M0A = Field<S1M0AR_Reg, 0, 32>; // Memory 0 address
    };

    // stream x memory 1 address register
    using S1M1AR_Reg = Register<DMA2_BASE + 0x38, ReadWrite>;
    struct _S1M1AR {
        using M1A = Field<S1M1AR_Reg, 0, 32>; // Memory 1 address (used in case of Double buffer mode)
    };

    // stream x FIFO control register
    using S1FCR_Reg = Register<DMA2_BASE + 0x3C, ReadWrite>;
    struct _S1FCR {
        using FEIE = Field<S1FCR_Reg, 7, 1>; // FIFO error interrupt enable
        using FS = Field<S1FCR_Reg, 3, 3>; // FIFO status
        using DMDIS = Field<S1FCR_Reg, 2, 1>; // Direct mode disable
        using FTH = Field<S1FCR_Reg, 0, 2>; // FIFO threshold selection
    };

    // stream x configuration register
    using S2CR_Reg = Register<DMA2_BASE + 0x40, ReadWrite>;
    struct _S2CR {
        using CHSEL = Field<S2CR_Reg, 25, 4>; // Channel selection
        using MBURST = Field<S2CR_Reg, 23, 2>; // Memory burst transfer configuration
        using PBURST = Field<S2CR_Reg, 21, 2>; // Peripheral burst transfer configuration
        using ACK = Field<S2CR_Reg, 20, 1>; // ACK
        using CT = Field<S2CR_Reg, 19, 1>; // Current target (only in double buffer mode)
        using DBM = Field<S2CR_Reg, 18, 1>; // Double buffer mode
        using PL = Field<S2CR_Reg, 16, 2>; // Priority level
        using PINCOS = Field<S2CR_Reg, 15, 1>; // Peripheral increment offset size
        using MSIZE = Field<S2CR_Reg, 13, 2>; // Memory data size
        using PSIZE = Field<S2CR_Reg, 11, 2>; // Peripheral data size
        using MINC = Field<S2CR_Reg, 10, 1>; // Memory increment mode
        using PINC = Field<S2CR_Reg, 9, 1>; // Peripheral increment mode
        using CIRC = Field<S2CR_Reg, 8, 1>; // Circular mode
        using DIR = Field<S2CR_Reg, 6, 2>; // Data transfer direction
        using PFCTRL = Field<S2CR_Reg, 5, 1>; // Peripheral flow controller
        using TCIE = Field<S2CR_Reg, 4, 1>; // Transfer complete interrupt enable
        using HTIE = Field<S2CR_Reg, 3, 1>; // Half transfer interrupt enable
        using TEIE = Field<S2CR_Reg, 2, 1>; // Transfer error interrupt enable
        using DMEIE = Field<S2CR_Reg, 1, 1>; // Direct mode error interrupt enable
        using EN = Field<S2CR_Reg, 0, 1>; // Stream enable / flag stream ready when read low
    };

    // stream x number of data register
    using S2NDTR_Reg = Register<DMA2_BASE + 0x44, ReadWrite>;
    struct _S2NDTR {
        using NDT = Field<S2NDTR_Reg, 0, 16>; // Number of data items to transfer
    };

    // stream x peripheral address register
    using S2PAR_Reg = Register<DMA2_BASE + 0x48, ReadWrite>;
    struct _S2PAR {
        using PA = Field<S2PAR_Reg, 0, 32>; // Peripheral address
    };

    // stream x memory 0 address register
    using S2M0AR_Reg = Register<DMA2_BASE + 0x4C, ReadWrite>;
    struct _S2M0AR {
        using M0A = Field<S2M0AR_Reg, 0, 32>; // Memory 0 address
    };

    // stream x memory 1 address register
    using S2M1AR_Reg = Register<DMA2_BASE + 0x50, ReadWrite>;
    struct _S2M1AR {
        using M1A = Field<S2M1AR_Reg, 0, 32>; // Memory 1 address (used in case of Double buffer mode)
    };

    // stream x FIFO control register
    using S2FCR_Reg = Register<DMA2_BASE + 0x54, ReadWrite>;
    struct _S2FCR {
        using FEIE = Field<S2FCR_Reg, 7, 1>; // FIFO error interrupt enable
        using FS = Field<S2FCR_Reg, 3, 3>; // FIFO status
        using DMDIS = Field<S2FCR_Reg, 2, 1>; // Direct mode disable
        using FTH = Field<S2FCR_Reg, 0, 2>; // FIFO threshold selection
    };

    // stream x configuration register
    using S3CR_Reg = Register<DMA2_BASE + 0x58, ReadWrite>;
    struct _S3CR {
        using CHSEL = Field<S3CR_Reg, 25, 4>; // Channel selection
        using MBURST = Field<S3CR_Reg, 23, 2>; // Memory burst transfer configuration
        using PBURST = Field<S3CR_Reg, 21, 2>; // Peripheral burst transfer configuration
        using ACK = Field<S3CR_Reg, 20, 1>; // ACK
        using CT = Field<S3CR_Reg, 19, 1>; // Current target (only in double buffer mode)
        using DBM = Field<S3CR_Reg, 18, 1>; // Double buffer mode
        using PL = Field<S3CR_Reg, 16, 2>; // Priority level
        using PINCOS = Field<S3CR_Reg, 15, 1>; // Peripheral increment offset size
        using MSIZE = Field<S3CR_Reg, 13, 2>; // Memory data size
        using PSIZE = Field<S3CR_Reg, 11, 2>; // Peripheral data size
        using MINC = Field<S3CR_Reg, 10, 1>; // Memory increment mode
        using PINC = Field<S3CR_Reg, 9, 1>; // Peripheral increment mode
        using CIRC = Field<S3CR_Reg, 8, 1>; // Circular mode
        using DIR = Field<S3CR_Reg, 6, 2>; // Data transfer direction
        using PFCTRL = Field<S3CR_Reg, 5, 1>; // Peripheral flow controller
        using TCIE = Field<S3CR_Reg, 4, 1>; // Transfer complete interrupt enable
        using HTIE = Field<S3CR_Reg, 3, 1>; // Half transfer interrupt enable
        using TEIE = Field<S3CR_Reg, 2, 1>; // Transfer error interrupt enable
        using DMEIE = Field<S3CR_Reg, 1, 1>; // Direct mode error interrupt enable
        using EN = Field<S3CR_Reg, 0, 1>; // Stream enable / flag stream ready when read low
    };

    // stream x number of data register
    using S3NDTR_Reg = Register<DMA2_BASE + 0x5C, ReadWrite>;
    struct _S3NDTR {
        using NDT = Field<S3NDTR_Reg, 0, 16>; // Number of data items to transfer
    };

    // stream x peripheral address register
    using S3PAR_Reg = Register<DMA2_BASE + 0x60, ReadWrite>;
    struct _S3PAR {
        using PA = Field<S3PAR_Reg, 0, 32>; // Peripheral address
    };

    // stream x memory 0 address register
    using S3M0AR_Reg = Register<DMA2_BASE + 0x64, ReadWrite>;
    struct _S3M0AR {
        using M0A = Field<S3M0AR_Reg, 0, 32>; // Memory 0 address
    };

    // stream x memory 1 address register
    using S3M1AR_Reg = Register<DMA2_BASE + 0x68, ReadWrite>;
    struct _S3M1AR {
        using M1A = Field<S3M1AR_Reg, 0, 32>; // Memory 1 address (used in case of Double buffer mode)
    };

    // stream x FIFO control register
    using S3FCR_Reg = Register<DMA2_BASE + 0x6C, ReadWrite>;
    struct _S3FCR {
        using FEIE = Field<S3FCR_Reg, 7, 1>; // FIFO error interrupt enable
        using FS = Field<S3FCR_Reg, 3, 3>; // FIFO status
        using DMDIS = Field<S3FCR_Reg, 2, 1>; // Direct mode disable
        using FTH = Field<S3FCR_Reg, 0, 2>; // FIFO threshold selection
    };

    // stream x configuration register
    using S4CR_Reg = Register<DMA2_BASE + 0x70, ReadWrite>;
    struct _S4CR {
        using CHSEL = Field<S4CR_Reg, 25, 3>; // Channel selection
        using MBURST = Field<S4CR_Reg, 23, 2>; // Memory burst transfer configuration
        using PBURST = Field<S4CR_Reg, 21, 2>; // Peripheral burst transfer configuration
        using ACK = Field<S4CR_Reg, 20, 1>; // ACK
        using CT = Field<S4CR_Reg, 19, 1>; // Current target (only in double buffer mode)
        using DBM = Field<S4CR_Reg, 18, 1>; // Double buffer mode
        using PL = Field<S4CR_Reg, 16, 2>; // Priority level
        using PINCOS = Field<S4CR_Reg, 15, 1>; // Peripheral increment offset size
        using MSIZE = Field<S4CR_Reg, 13, 2>; // Memory data size
        using PSIZE = Field<S4CR_Reg, 11, 2>; // Peripheral data size
        using MINC = Field<S4CR_Reg, 10, 1>; // Memory increment mode
        using PINC = Field<S4CR_Reg, 9, 1>; // Peripheral increment mode
        using CIRC = Field<S4CR_Reg, 8, 1>; // Circular mode
        using DIR = Field<S4CR_Reg, 6, 2>; // Data transfer direction
        using PFCTRL = Field<S4CR_Reg, 5, 1>; // Peripheral flow controller
        using TCIE = Field<S4CR_Reg, 4, 1>; // Transfer complete interrupt enable
        using HTIE = Field<S4CR_Reg, 3, 1>; // Half transfer interrupt enable
        using TEIE = Field<S4CR_Reg, 2, 1>; // Transfer error interrupt enable
        using DMEIE = Field<S4CR_Reg, 1, 1>; // Direct mode error interrupt enable
        using EN = Field<S4CR_Reg, 0, 1>; // Stream enable / flag stream ready when read low
    };

    // stream x number of data register
    using S4NDTR_Reg = Register<DMA2_BASE + 0x74, ReadWrite>;
    struct _S4NDTR {
        using NDT = Field<S4NDTR_Reg, 0, 16>; // Number of data items to transfer
    };

    // stream x peripheral address register
    using S4PAR_Reg = Register<DMA2_BASE + 0x78, ReadWrite>;
    struct _S4PAR {
        using PA = Field<S4PAR_Reg, 0, 32>; // Peripheral address
    };

    // stream x memory 0 address register
    using S4M0AR_Reg = Register<DMA2_BASE + 0x7C, ReadWrite>;
    struct _S4M0AR {
        using M0A = Field<S4M0AR_Reg, 0, 32>; // Memory 0 address
    };

    // stream x memory 1 address register
    using S4M1AR_Reg = Register<DMA2_BASE + 0x80, ReadWrite>;
    struct _S4M1AR {
        using M1A = Field<S4M1AR_Reg, 0, 32>; // Memory 1 address (used in case of Double buffer mode)
    };

    // stream x FIFO control register
    using S4FCR_Reg = Register<DMA2_BASE + 0x84, ReadWrite>;
    struct _S4FCR {
        using FEIE = Field<S4FCR_Reg, 7, 1>; // FIFO error interrupt enable
        using FS = Field<S4FCR_Reg, 3, 3>; // FIFO status
        using DMDIS = Field<S4FCR_Reg, 2, 1>; // Direct mode disable
        using FTH = Field<S4FCR_Reg, 0, 2>; // FIFO threshold selection
    };

    // stream x configuration register
    using S5CR_Reg = Register<DMA2_BASE + 0x88, ReadWrite>;
    struct _S5CR {
        using CHSEL = Field<S5CR_Reg, 25, 4>; // Channel selection
        using MBURST = Field<S5CR_Reg, 23, 2>; // Memory burst transfer configuration
        using PBURST = Field<S5CR_Reg, 21, 2>; // Peripheral burst transfer configuration
        using ACK = Field<S5CR_Reg, 20, 1>; // ACK
        using CT = Field<S5CR_Reg, 19, 1>; // Current target (only in double buffer mode)
        using DBM = Field<S5CR_Reg, 18, 1>; // Double buffer mode
        using PL = Field<S5CR_Reg, 16, 2>; // Priority level
        using PINCOS = Field<S5CR_Reg, 15, 1>; // Peripheral increment offset size
        using MSIZE = Field<S5CR_Reg, 13, 2>; // Memory data size
        using PSIZE = Field<S5CR_Reg, 11, 2>; // Peripheral data size
        using MINC = Field<S5CR_Reg, 10, 1>; // Memory increment mode
        using PINC = Field<S5CR_Reg, 9, 1>; // Peripheral increment mode
        using CIRC = Field<S5CR_Reg, 8, 1>; // Circular mode
        using DIR = Field<S5CR_Reg, 6, 2>; // Data transfer direction
        using PFCTRL = Field<S5CR_Reg, 5, 1>; // Peripheral flow controller
        using TCIE = Field<S5CR_Reg, 4, 1>; // Transfer complete interrupt enable
        using HTIE = Field<S5CR_Reg, 3, 1>; // Half transfer interrupt enable
        using TEIE = Field<S5CR_Reg, 2, 1>; // Transfer error interrupt enable
        using DMEIE = Field<S5CR_Reg, 1, 1>; // Direct mode error interrupt enable
        using EN = Field<S5CR_Reg, 0, 1>; // Stream enable / flag stream ready when read low
    };

    // stream x number of data register
    using S5NDTR_Reg = Register<DMA2_BASE + 0x8C, ReadWrite>;
    struct _S5NDTR {
        using NDT = Field<S5NDTR_Reg, 0, 16>; // Number of data items to transfer
    };

    // stream x peripheral address register
    using S5PAR_Reg = Register<DMA2_BASE + 0x90, ReadWrite>;
    struct _S5PAR {
        using PA = Field<S5PAR_Reg, 0, 32>; // Peripheral address
    };

    // stream x memory 0 address register
    using S5M0AR_Reg = Register<DMA2_BASE + 0x94, ReadWrite>;
    struct _S5M0AR {
        using M0A = Field<S5M0AR_Reg, 0, 32>; // Memory 0 address
    };

    // stream x memory 1 address register
    using S5M1AR_Reg = Register<DMA2_BASE + 0x98, ReadWrite>;
    struct _S5M1AR {
        using M1A = Field<S5M1AR_Reg, 0, 32>; // Memory 1 address (used in case of Double buffer mode)
    };

    // stream x FIFO control register
    using S5FCR_Reg = Register<DMA2_BASE + 0x9C, ReadWrite>;
    struct _S5FCR {
        using FEIE = Field<S5FCR_Reg, 7, 1>; // FIFO error interrupt enable
        using FS = Field<S5FCR_Reg, 3, 3>; // FIFO status
        using DMDIS = Field<S5FCR_Reg, 2, 1>; // Direct mode disable
        using FTH = Field<S5FCR_Reg, 0, 2>; // FIFO threshold selection
    };

    // stream x configuration register
    using S6CR_Reg = Register<DMA2_BASE + 0xA0, ReadWrite>;
    struct _S6CR {
        using CHSEL = Field<S6CR_Reg, 25, 4>; // Channel selection
        using MBURST = Field<S6CR_Reg, 23, 2>; // Memory burst transfer configuration
        using PBURST = Field<S6CR_Reg, 21, 2>; // Peripheral burst transfer configuration
        using ACK = Field<S6CR_Reg, 20, 1>; // ACK
        using CT = Field<S6CR_Reg, 19, 1>; // Current target (only in double buffer mode)
        using DBM = Field<S6CR_Reg, 18, 1>; // Double buffer mode
        using PL = Field<S6CR_Reg, 16, 2>; // Priority level
        using PINCOS = Field<S6CR_Reg, 15, 1>; // Peripheral increment offset size
        using MSIZE = Field<S6CR_Reg, 13, 2>; // Memory data size
        using PSIZE = Field<S6CR_Reg, 11, 2>; // Peripheral data size
        using MINC = Field<S6CR_Reg, 10, 1>; // Memory increment mode
        using PINC = Field<S6CR_Reg, 9, 1>; // Peripheral increment mode
        using CIRC = Field<S6CR_Reg, 8, 1>; // Circular mode
        using DIR = Field<S6CR_Reg, 6, 2>; // Data transfer direction
        using PFCTRL = Field<S6CR_Reg, 5, 1>; // Peripheral flow controller
        using TCIE = Field<S6CR_Reg, 4, 1>; // Transfer complete interrupt enable
        using HTIE = Field<S6CR_Reg, 3, 1>; // Half transfer interrupt enable
        using TEIE = Field<S6CR_Reg, 2, 1>; // Transfer error interrupt enable
        using DMEIE = Field<S6CR_Reg, 1, 1>; // Direct mode error interrupt enable
        using EN = Field<S6CR_Reg, 0, 1>; // Stream enable / flag stream ready when read low
    };

    // stream x number of data register
    using S6NDTR_Reg = Register<DMA2_BASE + 0xA4, ReadWrite>;
    struct _S6NDTR {
        using NDT = Field<S6NDTR_Reg, 0, 16>; // Number of data items to transfer
    };

    // stream x peripheral address register
    using S6PAR_Reg = Register<DMA2_BASE + 0xA8, ReadWrite>;
    struct _S6PAR {
        using PA = Field<S6PAR_Reg, 0, 32>; // Peripheral address
    };

    // stream x memory 0 address register
    using S6M0AR_Reg = Register<DMA2_BASE + 0xAC, ReadWrite>;
    struct _S6M0AR {
        using M0A = Field<S6M0AR_Reg, 0, 32>; // Memory 0 address
    };

    // stream x memory 1 address register
    using S6M1AR_Reg = Register<DMA2_BASE + 0xB0, ReadWrite>;
    struct _S6M1AR {
        using M1A = Field<S6M1AR_Reg, 0, 32>; // Memory 1 address (used in case of Double buffer mode)
    };

    // stream x FIFO control register
    using S6FCR_Reg = Register<DMA2_BASE + 0xB4, ReadWrite>;
    struct _S6FCR {
        using FEIE = Field<S6FCR_Reg, 7, 1>; // FIFO error interrupt enable
        using FS = Field<S6FCR_Reg, 3, 3>; // FIFO status
        using DMDIS = Field<S6FCR_Reg, 2, 1>; // Direct mode disable
        using FTH = Field<S6FCR_Reg, 0, 2>; // FIFO threshold selection
    };

    // stream x configuration register
    using S7CR_Reg = Register<DMA2_BASE + 0xB8, ReadWrite>;
    struct _S7CR {
        using CHSEL = Field<S7CR_Reg, 25, 4>; // Channel selection
        using MBURST = Field<S7CR_Reg, 23, 2>; // Memory burst transfer configuration
        using PBURST = Field<S7CR_Reg, 21, 2>; // Peripheral burst transfer configuration
        using ACK = Field<S7CR_Reg, 20, 1>; // ACK
        using CT = Field<S7CR_Reg, 19, 1>; // Current target (only in double buffer mode)
        using DBM = Field<S7CR_Reg, 18, 1>; // Double buffer mode
        using PL = Field<S7CR_Reg, 16, 2>; // Priority level
        using PINCOS = Field<S7CR_Reg, 15, 1>; // Peripheral increment offset size
        using MSIZE = Field<S7CR_Reg, 13, 2>; // Memory data size
        using PSIZE = Field<S7CR_Reg, 11, 2>; // Peripheral data size
        using MINC = Field<S7CR_Reg, 10, 1>; // Memory increment mode
        using PINC = Field<S7CR_Reg, 9, 1>; // Peripheral increment mode
        using CIRC = Field<S7CR_Reg, 8, 1>; // Circular mode
        using DIR = Field<S7CR_Reg, 6, 2>; // Data transfer direction
        using PFCTRL = Field<S7CR_Reg, 5, 1>; // Peripheral flow controller
        using TCIE = Field<S7CR_Reg, 4, 1>; // Transfer complete interrupt enable
        using HTIE = Field<S7CR_Reg, 3, 1>; // Half transfer interrupt enable
        using TEIE = Field<S7CR_Reg, 2, 1>; // Transfer error interrupt enable
        using DMEIE = Field<S7CR_Reg, 1, 1>; // Direct mode error interrupt enable
        using EN = Field<S7CR_Reg, 0, 1>; // Stream enable / flag stream ready when read low
    };

    // stream x number of data register
    using S7NDTR_Reg = Register<DMA2_BASE + 0xBC, ReadWrite>;
    struct _S7NDTR {
        using NDT = Field<S7NDTR_Reg, 0, 16>; // Number of data items to transfer
    };

    // stream x peripheral address register
    using S7PAR_Reg = Register<DMA2_BASE + 0xC0, ReadWrite>;
    struct _S7PAR {
        using PA = Field<S7PAR_Reg, 0, 32>; // Peripheral address
    };

    // stream x memory 0 address register
    using S7M0AR_Reg = Register<DMA2_BASE + 0xC4, ReadWrite>;
    struct _S7M0AR {
        using M0A = Field<S7M0AR_Reg, 0, 32>; // Memory 0 address
    };

    // stream x memory 1 address register
    using S7M1AR_Reg = Register<DMA2_BASE + 0xC8, ReadWrite>;
    struct _S7M1AR {
        using M1A = Field<S7M1AR_Reg, 0, 32>; // Memory 1 address (used in case of Double buffer mode)
    };

    // stream x FIFO control register
    using S7FCR_Reg = Register<DMA2_BASE + 0xCC, ReadWrite>;
    struct _S7FCR {
        using FEIE = Field<S7FCR_Reg, 7, 1>; // FIFO error interrupt enable
        using FS = Field<S7FCR_Reg, 3, 3>; // FIFO status
        using DMDIS = Field<S7FCR_Reg, 2, 1>; // Direct mode disable
        using FTH = Field<S7FCR_Reg, 0, 2>; // FIFO threshold selection
    };

} // namespace DMA2

// --------------------------------------------
// DMA1: 
// Base address: 0x40026000
// --------------------------------------------

namespace DMA1 {
    static constexpr uint32_t DMA1_BASE = 0x40026000;

} // namespace DMA1

// --------------------------------------------
// RCC: Reset and clock control
// Base address: 0x40023800
// --------------------------------------------

namespace RCC {
    static constexpr uint32_t RCC_BASE = 0x40023800;

    // clock control register
    using CR_Reg = Register<RCC_BASE + 0x0, ReadWrite>;
    struct _CR {
        using PLLI2SRDY = Field<CR_Reg, 27, 1>; // PLLI2S clock ready flag
        using PLLI2SON = Field<CR_Reg, 26, 1>; // PLLI2S enable
        using PLLRDY = Field<CR_Reg, 25, 1>; // Main PLL (PLL) clock ready flag
        using PLLON = Field<CR_Reg, 24, 1>; // Main PLL (PLL) enable
        using CSSON = Field<CR_Reg, 19, 1>; // Clock security system enable
        using HSEBYP = Field<CR_Reg, 18, 1>; // HSE clock bypass
        using HSERDY = Field<CR_Reg, 17, 1>; // HSE clock ready flag
        using HSEON = Field<CR_Reg, 16, 1>; // HSE clock enable
        using HSICAL = Field<CR_Reg, 8, 8>; // Internal high-speed clock calibration
        using HSITRIM = Field<CR_Reg, 3, 5>; // Internal high-speed clock trimming
        using HSIRDY = Field<CR_Reg, 1, 1>; // Internal high-speed clock ready flag
        using HSION = Field<CR_Reg, 0, 1>; // Internal high-speed clock enable
    };

    // PLL configuration register
    using PLLCFGR_Reg = Register<RCC_BASE + 0x4, ReadWrite>;
    struct _PLLCFGR {
        using PLLQ3 = Field<PLLCFGR_Reg, 27, 1>; // Main PLL (PLL) division factor for USB OTG FS, SDIO and random number generator clocks
        using PLLQ2 = Field<PLLCFGR_Reg, 26, 1>; // Main PLL (PLL) division factor for USB OTG FS, SDIO and random number generator clocks
        using PLLQ1 = Field<PLLCFGR_Reg, 25, 1>; // Main PLL (PLL) division factor for USB OTG FS, SDIO and random number generator clocks
        using PLLQ0 = Field<PLLCFGR_Reg, 24, 1>; // Main PLL (PLL) division factor for USB OTG FS, SDIO and random number generator clocks
        using PLLSRC = Field<PLLCFGR_Reg, 22, 1>; // Main PLL(PLL) and audio PLL (PLLI2S) entry clock source
        using PLLP1 = Field<PLLCFGR_Reg, 17, 1>; // Main PLL (PLL) division factor for main system clock
        using PLLP0 = Field<PLLCFGR_Reg, 16, 1>; // Main PLL (PLL) division factor for main system clock
        using PLLN8 = Field<PLLCFGR_Reg, 14, 1>; // Main PLL (PLL) multiplication factor for VCO
        using PLLN7 = Field<PLLCFGR_Reg, 13, 1>; // Main PLL (PLL) multiplication factor for VCO
        using PLLN6 = Field<PLLCFGR_Reg, 12, 1>; // Main PLL (PLL) multiplication factor for VCO
        using PLLN5 = Field<PLLCFGR_Reg, 11, 1>; // Main PLL (PLL) multiplication factor for VCO
        using PLLN4 = Field<PLLCFGR_Reg, 10, 1>; // Main PLL (PLL) multiplication factor for VCO
        using PLLN3 = Field<PLLCFGR_Reg, 9, 1>; // Main PLL (PLL) multiplication factor for VCO
        using PLLN2 = Field<PLLCFGR_Reg, 8, 1>; // Main PLL (PLL) multiplication factor for VCO
        using PLLN1 = Field<PLLCFGR_Reg, 7, 1>; // Main PLL (PLL) multiplication factor for VCO
        using PLLN0 = Field<PLLCFGR_Reg, 6, 1>; // Main PLL (PLL) multiplication factor for VCO
        using PLLM5 = Field<PLLCFGR_Reg, 5, 1>; // Division factor for the main PLL (PLL) and audio PLL (PLLI2S) input clock
        using PLLM4 = Field<PLLCFGR_Reg, 4, 1>; // Division factor for the main PLL (PLL) and audio PLL (PLLI2S) input clock
        using PLLM3 = Field<PLLCFGR_Reg, 3, 1>; // Division factor for the main PLL (PLL) and audio PLL (PLLI2S) input clock
        using PLLM2 = Field<PLLCFGR_Reg, 2, 1>; // Division factor for the main PLL (PLL) and audio PLL (PLLI2S) input clock
        using PLLM1 = Field<PLLCFGR_Reg, 1, 1>; // Division factor for the main PLL (PLL) and audio PLL (PLLI2S) input clock
        using PLLM0 = Field<PLLCFGR_Reg, 0, 1>; // Division factor for the main PLL (PLL) and audio PLL (PLLI2S) input clock
    };

    // clock configuration register
    using CFGR_Reg = Register<RCC_BASE + 0x8, ReadWrite>;
    struct _CFGR {
        using MCO2 = Field<CFGR_Reg, 30, 2>; // Microcontroller clock output 2
        using MCO2PRE = Field<CFGR_Reg, 27, 3>; // MCO2 prescaler
        using MCO1PRE = Field<CFGR_Reg, 24, 3>; // MCO1 prescaler
        using I2SSRC = Field<CFGR_Reg, 23, 1>; // I2S clock selection
        using MCO1 = Field<CFGR_Reg, 21, 2>; // Microcontroller clock output 1
        using RTCPRE = Field<CFGR_Reg, 16, 5>; // HSE division factor for RTC clock
        using PPRE2 = Field<CFGR_Reg, 13, 3>; // APB high-speed prescaler (APB2)
        using PPRE1 = Field<CFGR_Reg, 10, 3>; // APB Low speed prescaler (APB1)
        using HPRE = Field<CFGR_Reg, 4, 4>; // AHB prescaler
        using SWS1 = Field<CFGR_Reg, 3, 1>; // System clock switch status
        using SWS0 = Field<CFGR_Reg, 2, 1>; // System clock switch status
        using SW1 = Field<CFGR_Reg, 1, 1>; // System clock switch
        using SW0 = Field<CFGR_Reg, 0, 1>; // System clock switch
    };

    // clock interrupt register
    using CIR_Reg = Register<RCC_BASE + 0xC, ReadWrite>;
    struct _CIR {
        using CSSC = Field<CIR_Reg, 23, 1>; // Clock security system interrupt clear
        using PLLSAIRDYC = Field<CIR_Reg, 22, 1>; // PLLSAI Ready Interrupt Clear
        using PLLI2SRDYC = Field<CIR_Reg, 21, 1>; // PLLI2S ready interrupt clear
        using PLLRDYC = Field<CIR_Reg, 20, 1>; // Main PLL(PLL) ready interrupt clear
        using HSERDYC = Field<CIR_Reg, 19, 1>; // HSE ready interrupt clear
        using HSIRDYC = Field<CIR_Reg, 18, 1>; // HSI ready interrupt clear
        using LSERDYC = Field<CIR_Reg, 17, 1>; // LSE ready interrupt clear
        using LSIRDYC = Field<CIR_Reg, 16, 1>; // LSI ready interrupt clear
        using PLLSAIRDYIE = Field<CIR_Reg, 14, 1>; // PLLSAI Ready Interrupt Enable
        using PLLI2SRDYIE = Field<CIR_Reg, 13, 1>; // PLLI2S ready interrupt enable
        using PLLRDYIE = Field<CIR_Reg, 12, 1>; // Main PLL (PLL) ready interrupt enable
        using HSERDYIE = Field<CIR_Reg, 11, 1>; // HSE ready interrupt enable
        using HSIRDYIE = Field<CIR_Reg, 10, 1>; // HSI ready interrupt enable
        using LSERDYIE = Field<CIR_Reg, 9, 1>; // LSE ready interrupt enable
        using LSIRDYIE = Field<CIR_Reg, 8, 1>; // LSI ready interrupt enable
        using CSSF = Field<CIR_Reg, 7, 1>; // Clock security system interrupt flag
        using PLLSAIRDYF = Field<CIR_Reg, 6, 1>; // PLLSAI ready interrupt flag
        using PLLI2SRDYF = Field<CIR_Reg, 5, 1>; // PLLI2S ready interrupt flag
        using PLLRDYF = Field<CIR_Reg, 4, 1>; // Main PLL (PLL) ready interrupt flag
        using HSERDYF = Field<CIR_Reg, 3, 1>; // HSE ready interrupt flag
        using HSIRDYF = Field<CIR_Reg, 2, 1>; // HSI ready interrupt flag
        using LSERDYF = Field<CIR_Reg, 1, 1>; // LSE ready interrupt flag
        using LSIRDYF = Field<CIR_Reg, 0, 1>; // LSI ready interrupt flag
    };

    // AHB1 peripheral reset register
    using AHB1RSTR_Reg = Register<RCC_BASE + 0x10, ReadWrite>;
    struct _AHB1RSTR {
        using OTGHSRST = Field<AHB1RSTR_Reg, 29, 1>; // USB OTG HS module reset
        using ETHMACRST = Field<AHB1RSTR_Reg, 25, 1>; // Ethernet MAC reset
        using DMA2DRST = Field<AHB1RSTR_Reg, 23, 1>; // DMA2D reset
        using DMA2RST = Field<AHB1RSTR_Reg, 22, 1>; // DMA2 reset
        using DMA1RST = Field<AHB1RSTR_Reg, 21, 1>; // DMA2 reset
        using CRCRST = Field<AHB1RSTR_Reg, 12, 1>; // CRC reset
        using GPIOKRST = Field<AHB1RSTR_Reg, 10, 1>; // IO port K reset
        using GPIOJRST = Field<AHB1RSTR_Reg, 9, 1>; // IO port J reset
        using GPIOIRST = Field<AHB1RSTR_Reg, 8, 1>; // IO port I reset
        using GPIOHRST = Field<AHB1RSTR_Reg, 7, 1>; // IO port H reset
        using GPIOGRST = Field<AHB1RSTR_Reg, 6, 1>; // IO port G reset
        using GPIOFRST = Field<AHB1RSTR_Reg, 5, 1>; // IO port F reset
        using GPIOERST = Field<AHB1RSTR_Reg, 4, 1>; // IO port E reset
        using GPIODRST = Field<AHB1RSTR_Reg, 3, 1>; // IO port D reset
        using GPIOCRST = Field<AHB1RSTR_Reg, 2, 1>; // IO port C reset
        using GPIOBRST = Field<AHB1RSTR_Reg, 1, 1>; // IO port B reset
        using GPIOARST = Field<AHB1RSTR_Reg, 0, 1>; // IO port A reset
    };

    // AHB2 peripheral reset register
    using AHB2RSTR_Reg = Register<RCC_BASE + 0x14, ReadWrite>;
    struct _AHB2RSTR {
        using OTGFSRST = Field<AHB2RSTR_Reg, 7, 1>; // USB OTG FS module reset
        using RNGRST = Field<AHB2RSTR_Reg, 6, 1>; // Random number generator module reset
        using HSAHRST = Field<AHB2RSTR_Reg, 5, 1>; // Hash module reset
        using CRYPRST = Field<AHB2RSTR_Reg, 4, 1>; // Cryptographic module reset
        using DCMIRST = Field<AHB2RSTR_Reg, 0, 1>; // Camera interface reset
    };

    // AHB3 peripheral reset register
    using AHB3RSTR_Reg = Register<RCC_BASE + 0x18, ReadWrite>;
    struct _AHB3RSTR {
        using FMCRST = Field<AHB3RSTR_Reg, 0, 1>; // Flexible memory controller module reset
        using QSPIRST = Field<AHB3RSTR_Reg, 1, 1>; // Quad SPI memory controller reset
    };

    // APB1 peripheral reset register
    using APB1RSTR_Reg = Register<RCC_BASE + 0x20, ReadWrite>;
    struct _APB1RSTR {
        using TIM2RST = Field<APB1RSTR_Reg, 0, 1>; // TIM2 reset
        using TIM3RST = Field<APB1RSTR_Reg, 1, 1>; // TIM3 reset
        using TIM4RST = Field<APB1RSTR_Reg, 2, 1>; // TIM4 reset
        using TIM5RST = Field<APB1RSTR_Reg, 3, 1>; // TIM5 reset
        using TIM6RST = Field<APB1RSTR_Reg, 4, 1>; // TIM6 reset
        using TIM7RST = Field<APB1RSTR_Reg, 5, 1>; // TIM7 reset
        using TIM12RST = Field<APB1RSTR_Reg, 6, 1>; // TIM12 reset
        using TIM13RST = Field<APB1RSTR_Reg, 7, 1>; // TIM13 reset
        using TIM14RST = Field<APB1RSTR_Reg, 8, 1>; // TIM14 reset
        using LPTIM1RST = Field<APB1RSTR_Reg, 9, 1>; // Low power timer 1 reset
        using WWDGRST = Field<APB1RSTR_Reg, 11, 1>; // Window watchdog reset
        using CAN3RST = Field<APB1RSTR_Reg, 13, 1>; // CAN 3 reset
        using SPI2RST = Field<APB1RSTR_Reg, 14, 1>; // SPI 2 reset
        using SPI3RST = Field<APB1RSTR_Reg, 15, 1>; // SPI 3 reset
        using SPDIFRXRST = Field<APB1RSTR_Reg, 16, 1>; // SPDIF-RX reset
        using UART2RST = Field<APB1RSTR_Reg, 17, 1>; // USART 2 reset
        using UART3RST = Field<APB1RSTR_Reg, 18, 1>; // USART 3 reset
        using UART4RST = Field<APB1RSTR_Reg, 19, 1>; // USART 4 reset
        using UART5RST = Field<APB1RSTR_Reg, 20, 1>; // USART 5 reset
        using I2C1RST = Field<APB1RSTR_Reg, 21, 1>; // I2C 1 reset
        using I2C2RST = Field<APB1RSTR_Reg, 22, 1>; // I2C 2 reset
        using I2C3RST = Field<APB1RSTR_Reg, 23, 1>; // I2C3 reset
        using I2C4RST = Field<APB1RSTR_Reg, 24, 1>; // I2C 4 reset
        using CAN1RST = Field<APB1RSTR_Reg, 25, 1>; // CAN1 reset
        using CAN2RST = Field<APB1RSTR_Reg, 26, 1>; // CAN2 reset
        using CECRST = Field<APB1RSTR_Reg, 27, 1>; // HDMI-CEC reset
        using PWRRST = Field<APB1RSTR_Reg, 28, 1>; // Power interface reset
        using DACRST = Field<APB1RSTR_Reg, 29, 1>; // DAC reset
        using UART7RST = Field<APB1RSTR_Reg, 30, 1>; // UART7 reset
        using UART8RST = Field<APB1RSTR_Reg, 31, 1>; // UART8 reset
    };

    // APB2 peripheral reset register
    using APB2RSTR_Reg = Register<RCC_BASE + 0x24, ReadWrite>;
    struct _APB2RSTR {
        using TIM1RST = Field<APB2RSTR_Reg, 0, 1>; // TIM1 reset
        using TIM8RST = Field<APB2RSTR_Reg, 1, 1>; // TIM8 reset
        using USART1RST = Field<APB2RSTR_Reg, 4, 1>; // USART1 reset
        using USART6RST = Field<APB2RSTR_Reg, 5, 1>; // USART6 reset
        using ADCRST = Field<APB2RSTR_Reg, 8, 1>; // ADC interface reset (common to all ADCs)
        using SPI1RST = Field<APB2RSTR_Reg, 12, 1>; // SPI 1 reset
        using SPI4RST = Field<APB2RSTR_Reg, 13, 1>; // SPI4 reset
        using SYSCFGRST = Field<APB2RSTR_Reg, 14, 1>; // System configuration controller reset
        using TIM9RST = Field<APB2RSTR_Reg, 16, 1>; // TIM9 reset
        using TIM10RST = Field<APB2RSTR_Reg, 17, 1>; // TIM10 reset
        using TIM11RST = Field<APB2RSTR_Reg, 18, 1>; // TIM11 reset
        using SPI5RST = Field<APB2RSTR_Reg, 20, 1>; // SPI5 reset
        using SPI6RST = Field<APB2RSTR_Reg, 21, 1>; // SPI6 reset
        using SAI1RST = Field<APB2RSTR_Reg, 22, 1>; // SAI1 reset
        using LTDCRST = Field<APB2RSTR_Reg, 26, 1>; // LTDC reset
        using SAI2RST = Field<APB2RSTR_Reg, 23, 1>; // SAI2 reset
        using SDMMC1RST = Field<APB2RSTR_Reg, 11, 1>; // SDMMC1 reset
    };

    // AHB1 peripheral clock register
    using AHB1ENR_Reg = Register<RCC_BASE + 0x30, ReadWrite>;
    struct _AHB1ENR {
        using OTGHSULPIEN = Field<AHB1ENR_Reg, 30, 1>; // USB OTG HSULPI clock enable
        using OTGHSEN = Field<AHB1ENR_Reg, 29, 1>; // USB OTG HS clock enable
        using ETHMACPTPEN = Field<AHB1ENR_Reg, 28, 1>; // Ethernet PTP clock enable
        using ETHMACRXEN = Field<AHB1ENR_Reg, 27, 1>; // Ethernet Reception clock enable
        using ETHMACTXEN = Field<AHB1ENR_Reg, 26, 1>; // Ethernet Transmission clock enable
        using ETHMACEN = Field<AHB1ENR_Reg, 25, 1>; // Ethernet MAC clock enable
        using DMA2DEN = Field<AHB1ENR_Reg, 23, 1>; // DMA2D clock enable
        using DMA2EN = Field<AHB1ENR_Reg, 22, 1>; // DMA2 clock enable
        using DMA1EN = Field<AHB1ENR_Reg, 21, 1>; // DMA1 clock enable
        using CCMDATARAMEN = Field<AHB1ENR_Reg, 20, 1>; // CCM data RAM clock enable
        using BKPSRAMEN = Field<AHB1ENR_Reg, 18, 1>; // Backup SRAM interface clock enable
        using CRCEN = Field<AHB1ENR_Reg, 12, 1>; // CRC clock enable
        using GPIOKEN = Field<AHB1ENR_Reg, 10, 1>; // IO port K clock enable
        using GPIOJEN = Field<AHB1ENR_Reg, 9, 1>; // IO port J clock enable
        using GPIOIEN = Field<AHB1ENR_Reg, 8, 1>; // IO port I clock enable
        using GPIOHEN = Field<AHB1ENR_Reg, 7, 1>; // IO port H clock enable
        using GPIOGEN = Field<AHB1ENR_Reg, 6, 1>; // IO port G clock enable
        using GPIOFEN = Field<AHB1ENR_Reg, 5, 1>; // IO port F clock enable
        using GPIOEEN = Field<AHB1ENR_Reg, 4, 1>; // IO port E clock enable
        using GPIODEN = Field<AHB1ENR_Reg, 3, 1>; // IO port D clock enable
        using GPIOCEN = Field<AHB1ENR_Reg, 2, 1>; // IO port C clock enable
        using GPIOBEN = Field<AHB1ENR_Reg, 1, 1>; // IO port B clock enable
        using GPIOAEN = Field<AHB1ENR_Reg, 0, 1>; // IO port A clock enable
    };

    // AHB2 peripheral clock enable register
    using AHB2ENR_Reg = Register<RCC_BASE + 0x34, ReadWrite>;
    struct _AHB2ENR {
        using OTGFSEN = Field<AHB2ENR_Reg, 7, 1>; // USB OTG FS clock enable
        using RNGEN = Field<AHB2ENR_Reg, 6, 1>; // Random number generator clock enable
        using HASHEN = Field<AHB2ENR_Reg, 5, 1>; // Hash modules clock enable
        using CRYPEN = Field<AHB2ENR_Reg, 4, 1>; // Cryptographic modules clock enable
        using DCMIEN = Field<AHB2ENR_Reg, 0, 1>; // Camera interface enable
    };

    // AHB3 peripheral clock enable register
    using AHB3ENR_Reg = Register<RCC_BASE + 0x38, ReadWrite>;
    struct _AHB3ENR {
        using FMCEN = Field<AHB3ENR_Reg, 0, 1>; // Flexible memory controller module clock enable
        using QSPIEN = Field<AHB3ENR_Reg, 1, 1>; // Quad SPI memory controller clock enable
    };

    // APB1 peripheral clock enable register
    using APB1ENR_Reg = Register<RCC_BASE + 0x40, ReadWrite>;
    struct _APB1ENR {
        using TIM2EN = Field<APB1ENR_Reg, 0, 1>; // TIM2 clock enable
        using TIM3EN = Field<APB1ENR_Reg, 1, 1>; // TIM3 clock enable
        using TIM4EN = Field<APB1ENR_Reg, 2, 1>; // TIM4 clock enable
        using TIM5EN = Field<APB1ENR_Reg, 3, 1>; // TIM5 clock enable
        using TIM6EN = Field<APB1ENR_Reg, 4, 1>; // TIM6 clock enable
        using TIM7EN = Field<APB1ENR_Reg, 5, 1>; // TIM7 clock enable
        using TIM12EN = Field<APB1ENR_Reg, 6, 1>; // TIM12 clock enable
        using TIM13EN = Field<APB1ENR_Reg, 7, 1>; // TIM13 clock enable
        using TIM14EN = Field<APB1ENR_Reg, 8, 1>; // TIM14 clock enable
        using LPTMI1EN = Field<APB1ENR_Reg, 9, 1>; // Low power timer 1 clock enable
        using RTCAPBEN = Field<APB1ENR_Reg, 10, 1>; // RTC register interface clock enable
        using WWDGEN = Field<APB1ENR_Reg, 11, 1>; // Window watchdog clock enable
        using CAN3EN = Field<APB1ENR_Reg, 13, 1>; // CAN 3 clock enable
        using SPI2EN = Field<APB1ENR_Reg, 14, 1>; // SPI2 clock enable
        using SPI3EN = Field<APB1ENR_Reg, 15, 1>; // SPI3 clock enable
        using SPDIFRXEN = Field<APB1ENR_Reg, 16, 1>; // SPDIF-RX clock enable
        using USART2EN = Field<APB1ENR_Reg, 17, 1>; // USART 2 clock enable
        using USART3EN = Field<APB1ENR_Reg, 18, 1>; // USART3 clock enable
        using UART4EN = Field<APB1ENR_Reg, 19, 1>; // UART4 clock enable
        using UART5EN = Field<APB1ENR_Reg, 20, 1>; // UART5 clock enable
        using I2C1EN = Field<APB1ENR_Reg, 21, 1>; // I2C1 clock enable
        using I2C2EN = Field<APB1ENR_Reg, 22, 1>; // I2C2 clock enable
        using I2C3EN = Field<APB1ENR_Reg, 23, 1>; // I2C3 clock enable
        using I2C4EN = Field<APB1ENR_Reg, 24, 1>; // I2C4 clock enable
        using CAN1EN = Field<APB1ENR_Reg, 25, 1>; // CAN 1 clock enable
        using CAN2EN = Field<APB1ENR_Reg, 26, 1>; // CAN 2 clock enable
        using CECEN = Field<APB1ENR_Reg, 27, 1>; // HDMI-CEN clock enable
        using PWREN = Field<APB1ENR_Reg, 28, 1>; // Power interface clock enable
        using DACEN = Field<APB1ENR_Reg, 29, 1>; // DAC interface clock enable
        using UART7ENR = Field<APB1ENR_Reg, 30, 1>; // UART7 clock enable
        using UART8ENR = Field<APB1ENR_Reg, 31, 1>; // UART8 clock enable
    };

    // APB2 peripheral clock enable register
    using APB2ENR_Reg = Register<RCC_BASE + 0x44, ReadWrite>;
    struct _APB2ENR {
        using TIM1EN = Field<APB2ENR_Reg, 0, 1>; // TIM1 clock enable
        using TIM8EN = Field<APB2ENR_Reg, 1, 1>; // TIM8 clock enable
        using USART1EN = Field<APB2ENR_Reg, 4, 1>; // USART1 clock enable
        using USART6EN = Field<APB2ENR_Reg, 5, 1>; // USART6 clock enable
        using ADC1EN = Field<APB2ENR_Reg, 8, 1>; // ADC1 clock enable
        using ADC2EN = Field<APB2ENR_Reg, 9, 1>; // ADC2 clock enable
        using ADC3EN = Field<APB2ENR_Reg, 10, 1>; // ADC3 clock enable
        using SPI1EN = Field<APB2ENR_Reg, 12, 1>; // SPI1 clock enable
        using SPI4ENR = Field<APB2ENR_Reg, 13, 1>; // SPI4 clock enable
        using SYSCFGEN = Field<APB2ENR_Reg, 14, 1>; // System configuration controller clock enable
        using TIM9EN = Field<APB2ENR_Reg, 16, 1>; // TIM9 clock enable
        using TIM10EN = Field<APB2ENR_Reg, 17, 1>; // TIM10 clock enable
        using TIM11EN = Field<APB2ENR_Reg, 18, 1>; // TIM11 clock enable
        using SPI5ENR = Field<APB2ENR_Reg, 20, 1>; // SPI5 clock enable
        using SPI6ENR = Field<APB2ENR_Reg, 21, 1>; // SPI6 clock enable
        using SAI1EN = Field<APB2ENR_Reg, 22, 1>; // SAI1 clock enable
        using LTDCEN = Field<APB2ENR_Reg, 26, 1>; // LTDC clock enable
        using SAI2EN = Field<APB2ENR_Reg, 23, 1>; // SAI2 clock enable
        using SDMMC1EN = Field<APB2ENR_Reg, 11, 1>; // SDMMC1 clock enable
    };

    // AHB1 peripheral clock enable in low power mode register
    using AHB1LPENR_Reg = Register<RCC_BASE + 0x50, ReadWrite>;
    struct _AHB1LPENR {
        using GPIOALPEN = Field<AHB1LPENR_Reg, 0, 1>; // IO port A clock enable during sleep mode
        using GPIOBLPEN = Field<AHB1LPENR_Reg, 1, 1>; // IO port B clock enable during Sleep mode
        using GPIOCLPEN = Field<AHB1LPENR_Reg, 2, 1>; // IO port C clock enable during Sleep mode
        using GPIODLPEN = Field<AHB1LPENR_Reg, 3, 1>; // IO port D clock enable during Sleep mode
        using GPIOELPEN = Field<AHB1LPENR_Reg, 4, 1>; // IO port E clock enable during Sleep mode
        using GPIOFLPEN = Field<AHB1LPENR_Reg, 5, 1>; // IO port F clock enable during Sleep mode
        using GPIOGLPEN = Field<AHB1LPENR_Reg, 6, 1>; // IO port G clock enable during Sleep mode
        using GPIOHLPEN = Field<AHB1LPENR_Reg, 7, 1>; // IO port H clock enable during Sleep mode
        using GPIOILPEN = Field<AHB1LPENR_Reg, 8, 1>; // IO port I clock enable during Sleep mode
        using GPIOJLPEN = Field<AHB1LPENR_Reg, 9, 1>; // IO port J clock enable during Sleep mode
        using GPIOKLPEN = Field<AHB1LPENR_Reg, 10, 1>; // IO port K clock enable during Sleep mode
        using CRCLPEN = Field<AHB1LPENR_Reg, 12, 1>; // CRC clock enable during Sleep mode
        using FLITFLPEN = Field<AHB1LPENR_Reg, 15, 1>; // Flash interface clock enable during Sleep mode
        using SRAM1LPEN = Field<AHB1LPENR_Reg, 16, 1>; // SRAM 1interface clock enable during Sleep mode
        using SRAM2LPEN = Field<AHB1LPENR_Reg, 17, 1>; // SRAM 2 interface clock enable during Sleep mode
        using BKPSRAMLPEN = Field<AHB1LPENR_Reg, 18, 1>; // Backup SRAM interface clock enable during Sleep mode
        using SRAM3LPEN = Field<AHB1LPENR_Reg, 19, 1>; // SRAM 3 interface clock enable during Sleep mode
        using DMA1LPEN = Field<AHB1LPENR_Reg, 21, 1>; // DMA1 clock enable during Sleep mode
        using DMA2LPEN = Field<AHB1LPENR_Reg, 22, 1>; // DMA2 clock enable during Sleep mode
        using DMA2DLPEN = Field<AHB1LPENR_Reg, 23, 1>; // DMA2D clock enable during Sleep mode
        using ETHMACLPEN = Field<AHB1LPENR_Reg, 25, 1>; // Ethernet MAC clock enable during Sleep mode
        using ETHMACTXLPEN = Field<AHB1LPENR_Reg, 26, 1>; // Ethernet transmission clock enable during Sleep mode
        using ETHMACRXLPEN = Field<AHB1LPENR_Reg, 27, 1>; // Ethernet reception clock enable during Sleep mode
        using ETHMACPTPLPEN = Field<AHB1LPENR_Reg, 28, 1>; // Ethernet PTP clock enable during Sleep mode
        using OTGHSLPEN = Field<AHB1LPENR_Reg, 29, 1>; // USB OTG HS clock enable during Sleep mode
        using OTGHSULPILPEN = Field<AHB1LPENR_Reg, 30, 1>; // USB OTG HS ULPI clock enable during Sleep mode
    };

    // AHB2 peripheral clock enable in low power mode register
    using AHB2LPENR_Reg = Register<RCC_BASE + 0x54, ReadWrite>;
    struct _AHB2LPENR {
        using OTGFSLPEN = Field<AHB2LPENR_Reg, 7, 1>; // USB OTG FS clock enable during Sleep mode
        using RNGLPEN = Field<AHB2LPENR_Reg, 6, 1>; // Random number generator clock enable during Sleep mode
        using HASHLPEN = Field<AHB2LPENR_Reg, 5, 1>; // Hash modules clock enable during Sleep mode
        using CRYPLPEN = Field<AHB2LPENR_Reg, 4, 1>; // Cryptography modules clock enable during Sleep mode
        using DCMILPEN = Field<AHB2LPENR_Reg, 0, 1>; // Camera interface enable during Sleep mode
    };

    // AHB3 peripheral clock enable in low power mode register
    using AHB3LPENR_Reg = Register<RCC_BASE + 0x58, ReadWrite>;
    struct _AHB3LPENR {
        using FMCLPEN = Field<AHB3LPENR_Reg, 0, 1>; // Flexible memory controller module clock enable during Sleep mode
        using QSPILPEN = Field<AHB3LPENR_Reg, 1, 1>; // Quand SPI memory controller clock enable during Sleep mode
    };

    // APB1 peripheral clock enable in low power mode register
    using APB1LPENR_Reg = Register<RCC_BASE + 0x60, ReadWrite>;
    struct _APB1LPENR {
        using TIM2LPEN = Field<APB1LPENR_Reg, 0, 1>; // TIM2 clock enable during Sleep mode
        using TIM3LPEN = Field<APB1LPENR_Reg, 1, 1>; // TIM3 clock enable during Sleep mode
        using TIM4LPEN = Field<APB1LPENR_Reg, 2, 1>; // TIM4 clock enable during Sleep mode
        using TIM5LPEN = Field<APB1LPENR_Reg, 3, 1>; // TIM5 clock enable during Sleep mode
        using TIM6LPEN = Field<APB1LPENR_Reg, 4, 1>; // TIM6 clock enable during Sleep mode
        using TIM7LPEN = Field<APB1LPENR_Reg, 5, 1>; // TIM7 clock enable during Sleep mode
        using TIM12LPEN = Field<APB1LPENR_Reg, 6, 1>; // TIM12 clock enable during Sleep mode
        using TIM13LPEN = Field<APB1LPENR_Reg, 7, 1>; // TIM13 clock enable during Sleep mode
        using TIM14LPEN = Field<APB1LPENR_Reg, 8, 1>; // TIM14 clock enable during Sleep mode
        using WWDGLPEN = Field<APB1LPENR_Reg, 11, 1>; // Window watchdog clock enable during Sleep mode
        using SPI2LPEN = Field<APB1LPENR_Reg, 14, 1>; // SPI2 clock enable during Sleep mode
        using SPI3LPEN = Field<APB1LPENR_Reg, 15, 1>; // SPI3 clock enable during Sleep mode
        using USART2LPEN = Field<APB1LPENR_Reg, 17, 1>; // USART2 clock enable during Sleep mode
        using USART3LPEN = Field<APB1LPENR_Reg, 18, 1>; // USART3 clock enable during Sleep mode
        using UART4LPEN = Field<APB1LPENR_Reg, 19, 1>; // UART4 clock enable during Sleep mode
        using UART5LPEN = Field<APB1LPENR_Reg, 20, 1>; // UART5 clock enable during Sleep mode
        using I2C1LPEN = Field<APB1LPENR_Reg, 21, 1>; // I2C1 clock enable during Sleep mode
        using I2C2LPEN = Field<APB1LPENR_Reg, 22, 1>; // I2C2 clock enable during Sleep mode
        using I2C3LPEN = Field<APB1LPENR_Reg, 23, 1>; // I2C3 clock enable during Sleep mode
        using CAN1LPEN = Field<APB1LPENR_Reg, 25, 1>; // CAN 1 clock enable during Sleep mode
        using CAN2LPEN = Field<APB1LPENR_Reg, 26, 1>; // CAN 2 clock enable during Sleep mode
        using PWRLPEN = Field<APB1LPENR_Reg, 28, 1>; // Power interface clock enable during Sleep mode
        using DACLPEN = Field<APB1LPENR_Reg, 29, 1>; // DAC interface clock enable during Sleep mode
        using UART7LPEN = Field<APB1LPENR_Reg, 30, 1>; // UART7 clock enable during Sleep mode
        using UART8LPEN = Field<APB1LPENR_Reg, 31, 1>; // UART8 clock enable during Sleep mode
        using SPDIFRXLPEN = Field<APB1LPENR_Reg, 16, 1>; // SPDIF-RX clock enable during sleep mode
        using CECLPEN = Field<APB1LPENR_Reg, 27, 1>; // HDMI-CEN clock enable during Sleep mode
        using LPTIM1LPEN = Field<APB1LPENR_Reg, 9, 1>; // low power timer 1 clock enable during Sleep mode
        using I2C4LPEN = Field<APB1LPENR_Reg, 24, 1>; // I2C4 clock enable during Sleep mode
    };

    // APB2 peripheral clock enabled in low power mode register
    using APB2LPENR_Reg = Register<RCC_BASE + 0x64, ReadWrite>;
    struct _APB2LPENR {
        using TIM1LPEN = Field<APB2LPENR_Reg, 0, 1>; // TIM1 clock enable during Sleep mode
        using TIM8LPEN = Field<APB2LPENR_Reg, 1, 1>; // TIM8 clock enable during Sleep mode
        using USART1LPEN = Field<APB2LPENR_Reg, 4, 1>; // USART1 clock enable during Sleep mode
        using USART6LPEN = Field<APB2LPENR_Reg, 5, 1>; // USART6 clock enable during Sleep mode
        using ADC1LPEN = Field<APB2LPENR_Reg, 8, 1>; // ADC1 clock enable during Sleep mode
        using ADC2LPEN = Field<APB2LPENR_Reg, 9, 1>; // ADC2 clock enable during Sleep mode
        using ADC3LPEN = Field<APB2LPENR_Reg, 10, 1>; // ADC 3 clock enable during Sleep mode
        using SPI1LPEN = Field<APB2LPENR_Reg, 12, 1>; // SPI 1 clock enable during Sleep mode
        using SPI4LPEN = Field<APB2LPENR_Reg, 13, 1>; // SPI 4 clock enable during Sleep mode
        using SYSCFGLPEN = Field<APB2LPENR_Reg, 14, 1>; // System configuration controller clock enable during Sleep mode
        using TIM9LPEN = Field<APB2LPENR_Reg, 16, 1>; // TIM9 clock enable during sleep mode
        using TIM10LPEN = Field<APB2LPENR_Reg, 17, 1>; // TIM10 clock enable during Sleep mode
        using TIM11LPEN = Field<APB2LPENR_Reg, 18, 1>; // TIM11 clock enable during Sleep mode
        using SPI5LPEN = Field<APB2LPENR_Reg, 20, 1>; // SPI 5 clock enable during Sleep mode
        using SPI6LPEN = Field<APB2LPENR_Reg, 21, 1>; // SPI 6 clock enable during Sleep mode
        using SAI1LPEN = Field<APB2LPENR_Reg, 22, 1>; // SAI1 clock enable during sleep mode
        using LTDCLPEN = Field<APB2LPENR_Reg, 26, 1>; // LTDC clock enable during sleep mode
        using SAI2LPEN = Field<APB2LPENR_Reg, 23, 1>; // SAI2 clock enable during sleep mode
        using SDMMC1LPEN = Field<APB2LPENR_Reg, 11, 1>; // SDMMC1 clock enable during Sleep mode
    };

    // Backup domain control register
    using BDCR_Reg = Register<RCC_BASE + 0x70, ReadWrite>;
    struct _BDCR {
        using BDRST = Field<BDCR_Reg, 16, 1>; // Backup domain software reset
        using RTCEN = Field<BDCR_Reg, 15, 1>; // RTC clock enable
        using RTCSEL1 = Field<BDCR_Reg, 9, 1>; // RTC clock source selection
        using RTCSEL0 = Field<BDCR_Reg, 8, 1>; // RTC clock source selection
        using LSEBYP = Field<BDCR_Reg, 2, 1>; // External low-speed oscillator bypass
        using LSERDY = Field<BDCR_Reg, 1, 1>; // External low-speed oscillator ready
        using LSEON = Field<BDCR_Reg, 0, 1>; // External low-speed oscillator enable
    };

    // clock control & status register
    using CSR_Reg = Register<RCC_BASE + 0x74, ReadWrite>;
    struct _CSR {
        using LPWRRSTF = Field<CSR_Reg, 31, 1>; // Low-power reset flag
        using WWDGRSTF = Field<CSR_Reg, 30, 1>; // Window watchdog reset flag
        using WDGRSTF = Field<CSR_Reg, 29, 1>; // Independent watchdog reset flag
        using SFTRSTF = Field<CSR_Reg, 28, 1>; // Software reset flag
        using PORRSTF = Field<CSR_Reg, 27, 1>; // POR/PDR reset flag
        using PADRSTF = Field<CSR_Reg, 26, 1>; // PIN reset flag
        using BORRSTF = Field<CSR_Reg, 25, 1>; // BOR reset flag
        using RMVF = Field<CSR_Reg, 24, 1>; // Remove reset flag
        using LSIRDY = Field<CSR_Reg, 1, 1>; // Internal low-speed oscillator ready
        using LSION = Field<CSR_Reg, 0, 1>; // Internal low-speed oscillator enable
    };

    // spread spectrum clock generation register
    using SSCGR_Reg = Register<RCC_BASE + 0x80, ReadWrite>;
    struct _SSCGR {
        using SSCGEN = Field<SSCGR_Reg, 31, 1>; // Spread spectrum modulation enable
        using SPREADSEL = Field<SSCGR_Reg, 30, 1>; // Spread Select
        using INCSTEP = Field<SSCGR_Reg, 13, 15>; // Incrementation step
        using MODPER = Field<SSCGR_Reg, 0, 13>; // Modulation period
    };

    // PLLI2S configuration register
    using PLLI2SCFGR_Reg = Register<RCC_BASE + 0x84, ReadWrite>;
    struct _PLLI2SCFGR {
        using PLLI2SR = Field<PLLI2SCFGR_Reg, 28, 3>; // PLLI2S division factor for I2S clocks
        using PLLI2SQ = Field<PLLI2SCFGR_Reg, 24, 4>; // PLLI2S division factor for SAI1 clock
        using PLLI2SN = Field<PLLI2SCFGR_Reg, 6, 9>; // PLLI2S multiplication factor for VCO
    };

    // PLL configuration register
    using PLLSAICFGR_Reg = Register<RCC_BASE + 0x88, ReadWrite>;
    struct _PLLSAICFGR {
        using PLLSAIN = Field<PLLSAICFGR_Reg, 6, 9>; // PLLSAI division factor for VCO
        using PLLSAIP = Field<PLLSAICFGR_Reg, 16, 2>; // PLLSAI division factor for 48MHz clock
        using PLLSAIQ = Field<PLLSAICFGR_Reg, 24, 4>; // PLLSAI division factor for SAI clock
        using PLLSAIR = Field<PLLSAICFGR_Reg, 28, 3>; // PLLSAI division factor for LCD clock
    };

    // dedicated clocks configuration register
    using DKCFGR1_Reg = Register<RCC_BASE + 0x8C, ReadWrite>;
    struct _DKCFGR1 {
        using PLLI2SDIV = Field<DKCFGR1_Reg, 0, 5>; // PLLI2S division factor for SAI1 clock
        using PLLSAIDIVQ = Field<DKCFGR1_Reg, 8, 5>; // PLLSAI division factor for SAI1 clock
        using PLLSAIDIVR = Field<DKCFGR1_Reg, 16, 2>; // division factor for LCD_CLK
        using SAI1SEL = Field<DKCFGR1_Reg, 20, 2>; // SAI1 clock source selection
        using SAI2SEL = Field<DKCFGR1_Reg, 22, 2>; // SAI2 clock source selection
        using TIMPRE = Field<DKCFGR1_Reg, 24, 1>; // Timers clocks prescalers selection
    };

    // dedicated clocks configuration register
    using DKCFGR2_Reg = Register<RCC_BASE + 0x90, ReadWrite>;
    struct _DKCFGR2 {
        using USART1SEL = Field<DKCFGR2_Reg, 0, 2>; // USART 1 clock source selection
        using USART2SEL = Field<DKCFGR2_Reg, 2, 2>; // USART 2 clock source selection
        using USART3SEL = Field<DKCFGR2_Reg, 4, 2>; // USART 3 clock source selection
        using UART4SEL = Field<DKCFGR2_Reg, 6, 2>; // UART 4 clock source selection
        using UART5SEL = Field<DKCFGR2_Reg, 8, 2>; // UART 5 clock source selection
        using USART6SEL = Field<DKCFGR2_Reg, 10, 2>; // USART 6 clock source selection
        using UART7SEL = Field<DKCFGR2_Reg, 12, 2>; // UART 7 clock source selection
        using UART8SEL = Field<DKCFGR2_Reg, 14, 2>; // UART 8 clock source selection
        using I2C1SEL = Field<DKCFGR2_Reg, 16, 2>; // I2C1 clock source selection
        using I2C2SEL = Field<DKCFGR2_Reg, 18, 2>; // I2C2 clock source selection
        using I2C3SEL = Field<DKCFGR2_Reg, 20, 2>; // I2C3 clock source selection
        using I2C4SEL = Field<DKCFGR2_Reg, 22, 2>; // I2C4 clock source selection
        using LPTIM1SEL = Field<DKCFGR2_Reg, 24, 2>; // Low power timer 1 clock source selection
        using CECSEL = Field<DKCFGR2_Reg, 26, 1>; // HDMI-CEC clock source selection
        using CK48MSEL = Field<DKCFGR2_Reg, 27, 1>; // 48MHz clock source selection
        using SDMMC1SEL = Field<DKCFGR2_Reg, 28, 1>; // SDMMC1 clock source selection
        using SDMMC2SEL = Field<DKCFGR2_Reg, 29, 1>; // SDMMC2 clock source selection
        using DSISEL = Field<DKCFGR2_Reg, 30, 1>; // DSI clock source selection
    };

} // namespace RCC

// --------------------------------------------
// GPIOD: General-purpose I/Os
// Base address: 0X40020C00
// --------------------------------------------

namespace GPIOD {
    static constexpr uint32_t GPIOD_BASE = 0X40020C00;

    // GPIO port mode register
    using MODER_Reg = Register<GPIOD_BASE + 0x0, ReadWrite>;
    struct _MODER {
        using MODER15 = Field<MODER_Reg, 30, 2>; // Port x configuration bits (y = 0..15)
        using MODER14 = Field<MODER_Reg, 28, 2>; // Port x configuration bits (y = 0..15)
        using MODER13 = Field<MODER_Reg, 26, 2>; // Port x configuration bits (y = 0..15)
        using MODER12 = Field<MODER_Reg, 24, 2>; // Port x configuration bits (y = 0..15)
        using MODER11 = Field<MODER_Reg, 22, 2>; // Port x configuration bits (y = 0..15)
        using MODER10 = Field<MODER_Reg, 20, 2>; // Port x configuration bits (y = 0..15)
        using MODER9 = Field<MODER_Reg, 18, 2>; // Port x configuration bits (y = 0..15)
        using MODER8 = Field<MODER_Reg, 16, 2>; // Port x configuration bits (y = 0..15)
        using MODER7 = Field<MODER_Reg, 14, 2>; // Port x configuration bits (y = 0..15)
        using MODER6 = Field<MODER_Reg, 12, 2>; // Port x configuration bits (y = 0..15)
        using MODER5 = Field<MODER_Reg, 10, 2>; // Port x configuration bits (y = 0..15)
        using MODER4 = Field<MODER_Reg, 8, 2>; // Port x configuration bits (y = 0..15)
        using MODER3 = Field<MODER_Reg, 6, 2>; // Port x configuration bits (y = 0..15)
        using MODER2 = Field<MODER_Reg, 4, 2>; // Port x configuration bits (y = 0..15)
        using MODER1 = Field<MODER_Reg, 2, 2>; // Port x configuration bits (y = 0..15)
        using MODER0 = Field<MODER_Reg, 0, 2>; // Port x configuration bits (y = 0..15)
    };

    // GPIO port output type register
    using OTYPER_Reg = Register<GPIOD_BASE + 0x4, ReadWrite>;
    struct _OTYPER {
        using OT15 = Field<OTYPER_Reg, 15, 1>; // Port x configuration bits (y = 0..15)
        using OT14 = Field<OTYPER_Reg, 14, 1>; // Port x configuration bits (y = 0..15)
        using OT13 = Field<OTYPER_Reg, 13, 1>; // Port x configuration bits (y = 0..15)
        using OT12 = Field<OTYPER_Reg, 12, 1>; // Port x configuration bits (y = 0..15)
        using OT11 = Field<OTYPER_Reg, 11, 1>; // Port x configuration bits (y = 0..15)
        using OT10 = Field<OTYPER_Reg, 10, 1>; // Port x configuration bits (y = 0..15)
        using OT9 = Field<OTYPER_Reg, 9, 1>; // Port x configuration bits (y = 0..15)
        using OT8 = Field<OTYPER_Reg, 8, 1>; // Port x configuration bits (y = 0..15)
        using OT7 = Field<OTYPER_Reg, 7, 1>; // Port x configuration bits (y = 0..15)
        using OT6 = Field<OTYPER_Reg, 6, 1>; // Port x configuration bits (y = 0..15)
        using OT5 = Field<OTYPER_Reg, 5, 1>; // Port x configuration bits (y = 0..15)
        using OT4 = Field<OTYPER_Reg, 4, 1>; // Port x configuration bits (y = 0..15)
        using OT3 = Field<OTYPER_Reg, 3, 1>; // Port x configuration bits (y = 0..15)
        using OT2 = Field<OTYPER_Reg, 2, 1>; // Port x configuration bits (y = 0..15)
        using OT1 = Field<OTYPER_Reg, 1, 1>; // Port x configuration bits (y = 0..15)
        using OT0 = Field<OTYPER_Reg, 0, 1>; // Port x configuration bits (y = 0..15)
    };

    // GPIO port output speed register
    using OSPEEDR_Reg = Register<GPIOD_BASE + 0x8, ReadWrite>;
    struct _OSPEEDR {
        using OSPEEDR15 = Field<OSPEEDR_Reg, 30, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR14 = Field<OSPEEDR_Reg, 28, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR13 = Field<OSPEEDR_Reg, 26, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR12 = Field<OSPEEDR_Reg, 24, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR11 = Field<OSPEEDR_Reg, 22, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR10 = Field<OSPEEDR_Reg, 20, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR9 = Field<OSPEEDR_Reg, 18, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR8 = Field<OSPEEDR_Reg, 16, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR7 = Field<OSPEEDR_Reg, 14, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR6 = Field<OSPEEDR_Reg, 12, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR5 = Field<OSPEEDR_Reg, 10, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR4 = Field<OSPEEDR_Reg, 8, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR3 = Field<OSPEEDR_Reg, 6, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR2 = Field<OSPEEDR_Reg, 4, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR1 = Field<OSPEEDR_Reg, 2, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR0 = Field<OSPEEDR_Reg, 0, 2>; // Port x configuration bits (y = 0..15)
    };

    // GPIO port pull-up/pull-down register
    using PUPDR_Reg = Register<GPIOD_BASE + 0xC, ReadWrite>;
    struct _PUPDR {
        using PUPDR15 = Field<PUPDR_Reg, 30, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR14 = Field<PUPDR_Reg, 28, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR13 = Field<PUPDR_Reg, 26, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR12 = Field<PUPDR_Reg, 24, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR11 = Field<PUPDR_Reg, 22, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR10 = Field<PUPDR_Reg, 20, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR9 = Field<PUPDR_Reg, 18, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR8 = Field<PUPDR_Reg, 16, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR7 = Field<PUPDR_Reg, 14, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR6 = Field<PUPDR_Reg, 12, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR5 = Field<PUPDR_Reg, 10, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR4 = Field<PUPDR_Reg, 8, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR3 = Field<PUPDR_Reg, 6, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR2 = Field<PUPDR_Reg, 4, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR1 = Field<PUPDR_Reg, 2, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR0 = Field<PUPDR_Reg, 0, 2>; // Port x configuration bits (y = 0..15)
    };

    // GPIO port input data register
    using IDR_Reg = Register<GPIOD_BASE + 0x10, ReadOnly>;
    struct _IDR {
        using IDR15 = Field<IDR_Reg, 15, 1>; // Port input data (y = 0..15)
        using IDR14 = Field<IDR_Reg, 14, 1>; // Port input data (y = 0..15)
        using IDR13 = Field<IDR_Reg, 13, 1>; // Port input data (y = 0..15)
        using IDR12 = Field<IDR_Reg, 12, 1>; // Port input data (y = 0..15)
        using IDR11 = Field<IDR_Reg, 11, 1>; // Port input data (y = 0..15)
        using IDR10 = Field<IDR_Reg, 10, 1>; // Port input data (y = 0..15)
        using IDR9 = Field<IDR_Reg, 9, 1>; // Port input data (y = 0..15)
        using IDR8 = Field<IDR_Reg, 8, 1>; // Port input data (y = 0..15)
        using IDR7 = Field<IDR_Reg, 7, 1>; // Port input data (y = 0..15)
        using IDR6 = Field<IDR_Reg, 6, 1>; // Port input data (y = 0..15)
        using IDR5 = Field<IDR_Reg, 5, 1>; // Port input data (y = 0..15)
        using IDR4 = Field<IDR_Reg, 4, 1>; // Port input data (y = 0..15)
        using IDR3 = Field<IDR_Reg, 3, 1>; // Port input data (y = 0..15)
        using IDR2 = Field<IDR_Reg, 2, 1>; // Port input data (y = 0..15)
        using IDR1 = Field<IDR_Reg, 1, 1>; // Port input data (y = 0..15)
        using IDR0 = Field<IDR_Reg, 0, 1>; // Port input data (y = 0..15)
    };

    // GPIO port output data register
    using ODR_Reg = Register<GPIOD_BASE + 0x14, ReadWrite>;
    struct _ODR {
        using ODR15 = Field<ODR_Reg, 15, 1>; // Port output data (y = 0..15)
        using ODR14 = Field<ODR_Reg, 14, 1>; // Port output data (y = 0..15)
        using ODR13 = Field<ODR_Reg, 13, 1>; // Port output data (y = 0..15)
        using ODR12 = Field<ODR_Reg, 12, 1>; // Port output data (y = 0..15)
        using ODR11 = Field<ODR_Reg, 11, 1>; // Port output data (y = 0..15)
        using ODR10 = Field<ODR_Reg, 10, 1>; // Port output data (y = 0..15)
        using ODR9 = Field<ODR_Reg, 9, 1>; // Port output data (y = 0..15)
        using ODR8 = Field<ODR_Reg, 8, 1>; // Port output data (y = 0..15)
        using ODR7 = Field<ODR_Reg, 7, 1>; // Port output data (y = 0..15)
        using ODR6 = Field<ODR_Reg, 6, 1>; // Port output data (y = 0..15)
        using ODR5 = Field<ODR_Reg, 5, 1>; // Port output data (y = 0..15)
        using ODR4 = Field<ODR_Reg, 4, 1>; // Port output data (y = 0..15)
        using ODR3 = Field<ODR_Reg, 3, 1>; // Port output data (y = 0..15)
        using ODR2 = Field<ODR_Reg, 2, 1>; // Port output data (y = 0..15)
        using ODR1 = Field<ODR_Reg, 1, 1>; // Port output data (y = 0..15)
        using ODR0 = Field<ODR_Reg, 0, 1>; // Port output data (y = 0..15)
    };

    // GPIO port bit set/reset register
    using BSRR_Reg = Register<GPIOD_BASE + 0x18, WriteOnly>;
    struct _BSRR {
        using BR15 = Field<BSRR_Reg, 31, 1>; // Port x reset bit y (y = 0..15)
        using BR14 = Field<BSRR_Reg, 30, 1>; // Port x reset bit y (y = 0..15)
        using BR13 = Field<BSRR_Reg, 29, 1>; // Port x reset bit y (y = 0..15)
        using BR12 = Field<BSRR_Reg, 28, 1>; // Port x reset bit y (y = 0..15)
        using BR11 = Field<BSRR_Reg, 27, 1>; // Port x reset bit y (y = 0..15)
        using BR10 = Field<BSRR_Reg, 26, 1>; // Port x reset bit y (y = 0..15)
        using BR9 = Field<BSRR_Reg, 25, 1>; // Port x reset bit y (y = 0..15)
        using BR8 = Field<BSRR_Reg, 24, 1>; // Port x reset bit y (y = 0..15)
        using BR7 = Field<BSRR_Reg, 23, 1>; // Port x reset bit y (y = 0..15)
        using BR6 = Field<BSRR_Reg, 22, 1>; // Port x reset bit y (y = 0..15)
        using BR5 = Field<BSRR_Reg, 21, 1>; // Port x reset bit y (y = 0..15)
        using BR4 = Field<BSRR_Reg, 20, 1>; // Port x reset bit y (y = 0..15)
        using BR3 = Field<BSRR_Reg, 19, 1>; // Port x reset bit y (y = 0..15)
        using BR2 = Field<BSRR_Reg, 18, 1>; // Port x reset bit y (y = 0..15)
        using BR1 = Field<BSRR_Reg, 17, 1>; // Port x reset bit y (y = 0..15)
        using BR0 = Field<BSRR_Reg, 16, 1>; // Port x set bit y (y= 0..15)
        using BS15 = Field<BSRR_Reg, 15, 1>; // Port x set bit y (y= 0..15)
        using BS14 = Field<BSRR_Reg, 14, 1>; // Port x set bit y (y= 0..15)
        using BS13 = Field<BSRR_Reg, 13, 1>; // Port x set bit y (y= 0..15)
        using BS12 = Field<BSRR_Reg, 12, 1>; // Port x set bit y (y= 0..15)
        using BS11 = Field<BSRR_Reg, 11, 1>; // Port x set bit y (y= 0..15)
        using BS10 = Field<BSRR_Reg, 10, 1>; // Port x set bit y (y= 0..15)
        using BS9 = Field<BSRR_Reg, 9, 1>; // Port x set bit y (y= 0..15)
        using BS8 = Field<BSRR_Reg, 8, 1>; // Port x set bit y (y= 0..15)
        using BS7 = Field<BSRR_Reg, 7, 1>; // Port x set bit y (y= 0..15)
        using BS6 = Field<BSRR_Reg, 6, 1>; // Port x set bit y (y= 0..15)
        using BS5 = Field<BSRR_Reg, 5, 1>; // Port x set bit y (y= 0..15)
        using BS4 = Field<BSRR_Reg, 4, 1>; // Port x set bit y (y= 0..15)
        using BS3 = Field<BSRR_Reg, 3, 1>; // Port x set bit y (y= 0..15)
        using BS2 = Field<BSRR_Reg, 2, 1>; // Port x set bit y (y= 0..15)
        using BS1 = Field<BSRR_Reg, 1, 1>; // Port x set bit y (y= 0..15)
        using BS0 = Field<BSRR_Reg, 0, 1>; // Port x set bit y (y= 0..15)
    };

    // GPIO port configuration lock register
    using LCKR_Reg = Register<GPIOD_BASE + 0x1C, ReadWrite>;
    struct _LCKR {
        using LCKK = Field<LCKR_Reg, 16, 1>; // Port x lock bit y (y= 0..15)
        using LCK15 = Field<LCKR_Reg, 15, 1>; // Port x lock bit y (y= 0..15)
        using LCK14 = Field<LCKR_Reg, 14, 1>; // Port x lock bit y (y= 0..15)
        using LCK13 = Field<LCKR_Reg, 13, 1>; // Port x lock bit y (y= 0..15)
        using LCK12 = Field<LCKR_Reg, 12, 1>; // Port x lock bit y (y= 0..15)
        using LCK11 = Field<LCKR_Reg, 11, 1>; // Port x lock bit y (y= 0..15)
        using LCK10 = Field<LCKR_Reg, 10, 1>; // Port x lock bit y (y= 0..15)
        using LCK9 = Field<LCKR_Reg, 9, 1>; // Port x lock bit y (y= 0..15)
        using LCK8 = Field<LCKR_Reg, 8, 1>; // Port x lock bit y (y= 0..15)
        using LCK7 = Field<LCKR_Reg, 7, 1>; // Port x lock bit y (y= 0..15)
        using LCK6 = Field<LCKR_Reg, 6, 1>; // Port x lock bit y (y= 0..15)
        using LCK5 = Field<LCKR_Reg, 5, 1>; // Port x lock bit y (y= 0..15)
        using LCK4 = Field<LCKR_Reg, 4, 1>; // Port x lock bit y (y= 0..15)
        using LCK3 = Field<LCKR_Reg, 3, 1>; // Port x lock bit y (y= 0..15)
        using LCK2 = Field<LCKR_Reg, 2, 1>; // Port x lock bit y (y= 0..15)
        using LCK1 = Field<LCKR_Reg, 1, 1>; // Port x lock bit y (y= 0..15)
        using LCK0 = Field<LCKR_Reg, 0, 1>; // Port x lock bit y (y= 0..15)
    };

    // GPIO alternate function lowregister
    using AFRL_Reg = Register<GPIOD_BASE + 0x20, ReadWrite>;
    struct _AFRL {
        using AFRL7 = Field<AFRL_Reg, 28, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL6 = Field<AFRL_Reg, 24, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL5 = Field<AFRL_Reg, 20, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL4 = Field<AFRL_Reg, 16, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL3 = Field<AFRL_Reg, 12, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL2 = Field<AFRL_Reg, 8, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL1 = Field<AFRL_Reg, 4, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL0 = Field<AFRL_Reg, 0, 4>; // Alternate function selection for port x bit y (y = 0..7)
    };

    // GPIO alternate function high register
    using AFRH_Reg = Register<GPIOD_BASE + 0x24, ReadWrite>;
    struct _AFRH {
        using AFRH15 = Field<AFRH_Reg, 28, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH14 = Field<AFRH_Reg, 24, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH13 = Field<AFRH_Reg, 20, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH12 = Field<AFRH_Reg, 16, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH11 = Field<AFRH_Reg, 12, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH10 = Field<AFRH_Reg, 8, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH9 = Field<AFRH_Reg, 4, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH8 = Field<AFRH_Reg, 0, 4>; // Alternate function selection for port x bit y (y = 8..15)
    };

    // GPIO port bit reset register
    using BRR_Reg = Register<GPIOD_BASE + 0x28, ReadWrite>;
    struct _BRR {
        using BR0 = Field<BRR_Reg, 0, 1>; // Port D Reset bit 0
        using BR1 = Field<BRR_Reg, 1, 1>; // Port D Reset bit 1
        using BR2 = Field<BRR_Reg, 2, 1>; // Port D Reset bit 2
        using BR3 = Field<BRR_Reg, 3, 1>; // Port D Reset bit 3
        using BR4 = Field<BRR_Reg, 4, 1>; // Port D Reset bit 4
        using BR5 = Field<BRR_Reg, 5, 1>; // Port D Reset bit 5
        using BR6 = Field<BRR_Reg, 6, 1>; // Port D Reset bit 6
        using BR7 = Field<BRR_Reg, 7, 1>; // Port D Reset bit 7
        using BR8 = Field<BRR_Reg, 8, 1>; // Port D Reset bit 8
        using BR9 = Field<BRR_Reg, 9, 1>; // Port D Reset bit 9
        using BR10 = Field<BRR_Reg, 10, 1>; // Port D Reset bit 10
        using BR11 = Field<BRR_Reg, 11, 1>; // Port D Reset bit 11
        using BR12 = Field<BRR_Reg, 12, 1>; // Port D Reset bit 12
        using BR13 = Field<BRR_Reg, 13, 1>; // Port D Reset bit 13
        using BR14 = Field<BRR_Reg, 14, 1>; // Port D Reset bit 14
        using BR15 = Field<BRR_Reg, 15, 1>; // Port D Reset bit 15
    };

} // namespace GPIOD

// --------------------------------------------
// GPIOC: 
// Base address: 0x40020800
// --------------------------------------------

namespace GPIOC {
    static constexpr uint32_t GPIOC_BASE = 0x40020800;

} // namespace GPIOC

// --------------------------------------------
// GPIOK: 
// Base address: 0X40022800
// --------------------------------------------

namespace GPIOK {
    static constexpr uint32_t GPIOK_BASE = 0X40022800;

} // namespace GPIOK

// --------------------------------------------
// GPIOJ: 
// Base address: 0X40022400
// --------------------------------------------

namespace GPIOJ {
    static constexpr uint32_t GPIOJ_BASE = 0X40022400;

} // namespace GPIOJ

// --------------------------------------------
// GPIOI: 
// Base address: 0X40022000
// --------------------------------------------

namespace GPIOI {
    static constexpr uint32_t GPIOI_BASE = 0X40022000;

} // namespace GPIOI

// --------------------------------------------
// GPIOH: 
// Base address: 0X40021C00
// --------------------------------------------

namespace GPIOH {
    static constexpr uint32_t GPIOH_BASE = 0X40021C00;

} // namespace GPIOH

// --------------------------------------------
// GPIOG: 
// Base address: 0X40021800
// --------------------------------------------

namespace GPIOG {
    static constexpr uint32_t GPIOG_BASE = 0X40021800;

} // namespace GPIOG

// --------------------------------------------
// GPIOF: 
// Base address: 0X40021400
// --------------------------------------------

namespace GPIOF {
    static constexpr uint32_t GPIOF_BASE = 0X40021400;

} // namespace GPIOF

// --------------------------------------------
// GPIOE: 
// Base address: 0X40021000
// --------------------------------------------

namespace GPIOE {
    static constexpr uint32_t GPIOE_BASE = 0X40021000;

} // namespace GPIOE

// --------------------------------------------
// GPIOB: General-purpose I/Os
// Base address: 0x40020400
// --------------------------------------------

namespace GPIOB {
    static constexpr uint32_t GPIOB_BASE = 0x40020400;

    // GPIO port mode register
    using MODER_Reg = Register<GPIOB_BASE + 0x0, ReadWrite>;
    struct _MODER {
        using MODER15 = Field<MODER_Reg, 30, 2>; // Port x configuration bits (y = 0..15)
        using MODER14 = Field<MODER_Reg, 28, 2>; // Port x configuration bits (y = 0..15)
        using MODER13 = Field<MODER_Reg, 26, 2>; // Port x configuration bits (y = 0..15)
        using MODER12 = Field<MODER_Reg, 24, 2>; // Port x configuration bits (y = 0..15)
        using MODER11 = Field<MODER_Reg, 22, 2>; // Port x configuration bits (y = 0..15)
        using MODER10 = Field<MODER_Reg, 20, 2>; // Port x configuration bits (y = 0..15)
        using MODER9 = Field<MODER_Reg, 18, 2>; // Port x configuration bits (y = 0..15)
        using MODER8 = Field<MODER_Reg, 16, 2>; // Port x configuration bits (y = 0..15)
        using MODER7 = Field<MODER_Reg, 14, 2>; // Port x configuration bits (y = 0..15)
        using MODER6 = Field<MODER_Reg, 12, 2>; // Port x configuration bits (y = 0..15)
        using MODER5 = Field<MODER_Reg, 10, 2>; // Port x configuration bits (y = 0..15)
        using MODER4 = Field<MODER_Reg, 8, 2>; // Port x configuration bits (y = 0..15)
        using MODER3 = Field<MODER_Reg, 6, 2>; // Port x configuration bits (y = 0..15)
        using MODER2 = Field<MODER_Reg, 4, 2>; // Port x configuration bits (y = 0..15)
        using MODER1 = Field<MODER_Reg, 2, 2>; // Port x configuration bits (y = 0..15)
        using MODER0 = Field<MODER_Reg, 0, 2>; // Port x configuration bits (y = 0..15)
    };

    // GPIO port output type register
    using OTYPER_Reg = Register<GPIOB_BASE + 0x4, ReadWrite>;
    struct _OTYPER {
        using OT15 = Field<OTYPER_Reg, 15, 1>; // Port x configuration bits (y = 0..15)
        using OT14 = Field<OTYPER_Reg, 14, 1>; // Port x configuration bits (y = 0..15)
        using OT13 = Field<OTYPER_Reg, 13, 1>; // Port x configuration bits (y = 0..15)
        using OT12 = Field<OTYPER_Reg, 12, 1>; // Port x configuration bits (y = 0..15)
        using OT11 = Field<OTYPER_Reg, 11, 1>; // Port x configuration bits (y = 0..15)
        using OT10 = Field<OTYPER_Reg, 10, 1>; // Port x configuration bits (y = 0..15)
        using OT9 = Field<OTYPER_Reg, 9, 1>; // Port x configuration bits (y = 0..15)
        using OT8 = Field<OTYPER_Reg, 8, 1>; // Port x configuration bits (y = 0..15)
        using OT7 = Field<OTYPER_Reg, 7, 1>; // Port x configuration bits (y = 0..15)
        using OT6 = Field<OTYPER_Reg, 6, 1>; // Port x configuration bits (y = 0..15)
        using OT5 = Field<OTYPER_Reg, 5, 1>; // Port x configuration bits (y = 0..15)
        using OT4 = Field<OTYPER_Reg, 4, 1>; // Port x configuration bits (y = 0..15)
        using OT3 = Field<OTYPER_Reg, 3, 1>; // Port x configuration bits (y = 0..15)
        using OT2 = Field<OTYPER_Reg, 2, 1>; // Port x configuration bits (y = 0..15)
        using OT1 = Field<OTYPER_Reg, 1, 1>; // Port x configuration bits (y = 0..15)
        using OT0 = Field<OTYPER_Reg, 0, 1>; // Port x configuration bits (y = 0..15)
    };

    // GPIO port output speed register
    using OSPEEDR_Reg = Register<GPIOB_BASE + 0x8, ReadWrite>;
    struct _OSPEEDR {
        using OSPEEDR15 = Field<OSPEEDR_Reg, 30, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR14 = Field<OSPEEDR_Reg, 28, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR13 = Field<OSPEEDR_Reg, 26, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR12 = Field<OSPEEDR_Reg, 24, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR11 = Field<OSPEEDR_Reg, 22, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR10 = Field<OSPEEDR_Reg, 20, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR9 = Field<OSPEEDR_Reg, 18, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR8 = Field<OSPEEDR_Reg, 16, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR7 = Field<OSPEEDR_Reg, 14, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR6 = Field<OSPEEDR_Reg, 12, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR5 = Field<OSPEEDR_Reg, 10, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR4 = Field<OSPEEDR_Reg, 8, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR3 = Field<OSPEEDR_Reg, 6, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR2 = Field<OSPEEDR_Reg, 4, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR1 = Field<OSPEEDR_Reg, 2, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR0 = Field<OSPEEDR_Reg, 0, 2>; // Port x configuration bits (y = 0..15)
    };

    // GPIO port pull-up/pull-down register
    using PUPDR_Reg = Register<GPIOB_BASE + 0xC, ReadWrite>;
    struct _PUPDR {
        using PUPDR15 = Field<PUPDR_Reg, 30, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR14 = Field<PUPDR_Reg, 28, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR13 = Field<PUPDR_Reg, 26, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR12 = Field<PUPDR_Reg, 24, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR11 = Field<PUPDR_Reg, 22, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR10 = Field<PUPDR_Reg, 20, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR9 = Field<PUPDR_Reg, 18, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR8 = Field<PUPDR_Reg, 16, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR7 = Field<PUPDR_Reg, 14, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR6 = Field<PUPDR_Reg, 12, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR5 = Field<PUPDR_Reg, 10, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR4 = Field<PUPDR_Reg, 8, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR3 = Field<PUPDR_Reg, 6, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR2 = Field<PUPDR_Reg, 4, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR1 = Field<PUPDR_Reg, 2, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR0 = Field<PUPDR_Reg, 0, 2>; // Port x configuration bits (y = 0..15)
    };

    // GPIO port input data register
    using IDR_Reg = Register<GPIOB_BASE + 0x10, ReadOnly>;
    struct _IDR {
        using IDR15 = Field<IDR_Reg, 15, 1>; // Port input data (y = 0..15)
        using IDR14 = Field<IDR_Reg, 14, 1>; // Port input data (y = 0..15)
        using IDR13 = Field<IDR_Reg, 13, 1>; // Port input data (y = 0..15)
        using IDR12 = Field<IDR_Reg, 12, 1>; // Port input data (y = 0..15)
        using IDR11 = Field<IDR_Reg, 11, 1>; // Port input data (y = 0..15)
        using IDR10 = Field<IDR_Reg, 10, 1>; // Port input data (y = 0..15)
        using IDR9 = Field<IDR_Reg, 9, 1>; // Port input data (y = 0..15)
        using IDR8 = Field<IDR_Reg, 8, 1>; // Port input data (y = 0..15)
        using IDR7 = Field<IDR_Reg, 7, 1>; // Port input data (y = 0..15)
        using IDR6 = Field<IDR_Reg, 6, 1>; // Port input data (y = 0..15)
        using IDR5 = Field<IDR_Reg, 5, 1>; // Port input data (y = 0..15)
        using IDR4 = Field<IDR_Reg, 4, 1>; // Port input data (y = 0..15)
        using IDR3 = Field<IDR_Reg, 3, 1>; // Port input data (y = 0..15)
        using IDR2 = Field<IDR_Reg, 2, 1>; // Port input data (y = 0..15)
        using IDR1 = Field<IDR_Reg, 1, 1>; // Port input data (y = 0..15)
        using IDR0 = Field<IDR_Reg, 0, 1>; // Port input data (y = 0..15)
    };

    // GPIO port output data register
    using ODR_Reg = Register<GPIOB_BASE + 0x14, ReadWrite>;
    struct _ODR {
        using ODR15 = Field<ODR_Reg, 15, 1>; // Port output data (y = 0..15)
        using ODR14 = Field<ODR_Reg, 14, 1>; // Port output data (y = 0..15)
        using ODR13 = Field<ODR_Reg, 13, 1>; // Port output data (y = 0..15)
        using ODR12 = Field<ODR_Reg, 12, 1>; // Port output data (y = 0..15)
        using ODR11 = Field<ODR_Reg, 11, 1>; // Port output data (y = 0..15)
        using ODR10 = Field<ODR_Reg, 10, 1>; // Port output data (y = 0..15)
        using ODR9 = Field<ODR_Reg, 9, 1>; // Port output data (y = 0..15)
        using ODR8 = Field<ODR_Reg, 8, 1>; // Port output data (y = 0..15)
        using ODR7 = Field<ODR_Reg, 7, 1>; // Port output data (y = 0..15)
        using ODR6 = Field<ODR_Reg, 6, 1>; // Port output data (y = 0..15)
        using ODR5 = Field<ODR_Reg, 5, 1>; // Port output data (y = 0..15)
        using ODR4 = Field<ODR_Reg, 4, 1>; // Port output data (y = 0..15)
        using ODR3 = Field<ODR_Reg, 3, 1>; // Port output data (y = 0..15)
        using ODR2 = Field<ODR_Reg, 2, 1>; // Port output data (y = 0..15)
        using ODR1 = Field<ODR_Reg, 1, 1>; // Port output data (y = 0..15)
        using ODR0 = Field<ODR_Reg, 0, 1>; // Port output data (y = 0..15)
    };

    // GPIO port bit set/reset register
    using BSRR_Reg = Register<GPIOB_BASE + 0x18, WriteOnly>;
    struct _BSRR {
        using BR15 = Field<BSRR_Reg, 31, 1>; // Port x reset bit y (y = 0..15)
        using BR14 = Field<BSRR_Reg, 30, 1>; // Port x reset bit y (y = 0..15)
        using BR13 = Field<BSRR_Reg, 29, 1>; // Port x reset bit y (y = 0..15)
        using BR12 = Field<BSRR_Reg, 28, 1>; // Port x reset bit y (y = 0..15)
        using BR11 = Field<BSRR_Reg, 27, 1>; // Port x reset bit y (y = 0..15)
        using BR10 = Field<BSRR_Reg, 26, 1>; // Port x reset bit y (y = 0..15)
        using BR9 = Field<BSRR_Reg, 25, 1>; // Port x reset bit y (y = 0..15)
        using BR8 = Field<BSRR_Reg, 24, 1>; // Port x reset bit y (y = 0..15)
        using BR7 = Field<BSRR_Reg, 23, 1>; // Port x reset bit y (y = 0..15)
        using BR6 = Field<BSRR_Reg, 22, 1>; // Port x reset bit y (y = 0..15)
        using BR5 = Field<BSRR_Reg, 21, 1>; // Port x reset bit y (y = 0..15)
        using BR4 = Field<BSRR_Reg, 20, 1>; // Port x reset bit y (y = 0..15)
        using BR3 = Field<BSRR_Reg, 19, 1>; // Port x reset bit y (y = 0..15)
        using BR2 = Field<BSRR_Reg, 18, 1>; // Port x reset bit y (y = 0..15)
        using BR1 = Field<BSRR_Reg, 17, 1>; // Port x reset bit y (y = 0..15)
        using BR0 = Field<BSRR_Reg, 16, 1>; // Port x set bit y (y= 0..15)
        using BS15 = Field<BSRR_Reg, 15, 1>; // Port x set bit y (y= 0..15)
        using BS14 = Field<BSRR_Reg, 14, 1>; // Port x set bit y (y= 0..15)
        using BS13 = Field<BSRR_Reg, 13, 1>; // Port x set bit y (y= 0..15)
        using BS12 = Field<BSRR_Reg, 12, 1>; // Port x set bit y (y= 0..15)
        using BS11 = Field<BSRR_Reg, 11, 1>; // Port x set bit y (y= 0..15)
        using BS10 = Field<BSRR_Reg, 10, 1>; // Port x set bit y (y= 0..15)
        using BS9 = Field<BSRR_Reg, 9, 1>; // Port x set bit y (y= 0..15)
        using BS8 = Field<BSRR_Reg, 8, 1>; // Port x set bit y (y= 0..15)
        using BS7 = Field<BSRR_Reg, 7, 1>; // Port x set bit y (y= 0..15)
        using BS6 = Field<BSRR_Reg, 6, 1>; // Port x set bit y (y= 0..15)
        using BS5 = Field<BSRR_Reg, 5, 1>; // Port x set bit y (y= 0..15)
        using BS4 = Field<BSRR_Reg, 4, 1>; // Port x set bit y (y= 0..15)
        using BS3 = Field<BSRR_Reg, 3, 1>; // Port x set bit y (y= 0..15)
        using BS2 = Field<BSRR_Reg, 2, 1>; // Port x set bit y (y= 0..15)
        using BS1 = Field<BSRR_Reg, 1, 1>; // Port x set bit y (y= 0..15)
        using BS0 = Field<BSRR_Reg, 0, 1>; // Port x set bit y (y= 0..15)
    };

    // GPIO port configuration lock register
    using LCKR_Reg = Register<GPIOB_BASE + 0x1C, ReadWrite>;
    struct _LCKR {
        using LCKK = Field<LCKR_Reg, 16, 1>; // Port x lock bit y (y= 0..15)
        using LCK15 = Field<LCKR_Reg, 15, 1>; // Port x lock bit y (y= 0..15)
        using LCK14 = Field<LCKR_Reg, 14, 1>; // Port x lock bit y (y= 0..15)
        using LCK13 = Field<LCKR_Reg, 13, 1>; // Port x lock bit y (y= 0..15)
        using LCK12 = Field<LCKR_Reg, 12, 1>; // Port x lock bit y (y= 0..15)
        using LCK11 = Field<LCKR_Reg, 11, 1>; // Port x lock bit y (y= 0..15)
        using LCK10 = Field<LCKR_Reg, 10, 1>; // Port x lock bit y (y= 0..15)
        using LCK9 = Field<LCKR_Reg, 9, 1>; // Port x lock bit y (y= 0..15)
        using LCK8 = Field<LCKR_Reg, 8, 1>; // Port x lock bit y (y= 0..15)
        using LCK7 = Field<LCKR_Reg, 7, 1>; // Port x lock bit y (y= 0..15)
        using LCK6 = Field<LCKR_Reg, 6, 1>; // Port x lock bit y (y= 0..15)
        using LCK5 = Field<LCKR_Reg, 5, 1>; // Port x lock bit y (y= 0..15)
        using LCK4 = Field<LCKR_Reg, 4, 1>; // Port x lock bit y (y= 0..15)
        using LCK3 = Field<LCKR_Reg, 3, 1>; // Port x lock bit y (y= 0..15)
        using LCK2 = Field<LCKR_Reg, 2, 1>; // Port x lock bit y (y= 0..15)
        using LCK1 = Field<LCKR_Reg, 1, 1>; // Port x lock bit y (y= 0..15)
        using LCK0 = Field<LCKR_Reg, 0, 1>; // Port x lock bit y (y= 0..15)
    };

    // GPIO alternate function low register
    using AFRL_Reg = Register<GPIOB_BASE + 0x20, ReadWrite>;
    struct _AFRL {
        using AFRL7 = Field<AFRL_Reg, 28, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL6 = Field<AFRL_Reg, 24, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL5 = Field<AFRL_Reg, 20, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL4 = Field<AFRL_Reg, 16, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL3 = Field<AFRL_Reg, 12, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL2 = Field<AFRL_Reg, 8, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL1 = Field<AFRL_Reg, 4, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL0 = Field<AFRL_Reg, 0, 4>; // Alternate function selection for port x bit y (y = 0..7)
    };

    // GPIO alternate function high register
    using AFRH_Reg = Register<GPIOB_BASE + 0x24, ReadWrite>;
    struct _AFRH {
        using AFRH15 = Field<AFRH_Reg, 28, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH14 = Field<AFRH_Reg, 24, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH13 = Field<AFRH_Reg, 20, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH12 = Field<AFRH_Reg, 16, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH11 = Field<AFRH_Reg, 12, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH10 = Field<AFRH_Reg, 8, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH9 = Field<AFRH_Reg, 4, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH8 = Field<AFRH_Reg, 0, 4>; // Alternate function selection for port x bit y (y = 8..15)
    };

    // GPIO port bit reset register
    using BRR_Reg = Register<GPIOB_BASE + 0x28, ReadWrite>;
    struct _BRR {
        using BR0 = Field<BRR_Reg, 0, 1>; // Port B Reset bit 0
        using BR1 = Field<BRR_Reg, 1, 1>; // Port B Reset bit 1
        using BR2 = Field<BRR_Reg, 2, 1>; // Port B Reset bit 2
        using BR3 = Field<BRR_Reg, 3, 1>; // Port B Reset bit 3
        using BR4 = Field<BRR_Reg, 4, 1>; // Port B Reset bit 4
        using BR5 = Field<BRR_Reg, 5, 1>; // Port B Reset bit 5
        using BR6 = Field<BRR_Reg, 6, 1>; // Port B Reset bit 6
        using BR7 = Field<BRR_Reg, 7, 1>; // Port B Reset bit 7
        using BR8 = Field<BRR_Reg, 8, 1>; // Port B Reset bit 8
        using BR9 = Field<BRR_Reg, 9, 1>; // Port B Reset bit 9
        using BR10 = Field<BRR_Reg, 10, 1>; // Port B Reset bit 10
        using BR11 = Field<BRR_Reg, 11, 1>; // Port B Reset bit 11
        using BR12 = Field<BRR_Reg, 12, 1>; // Port B Reset bit 12
        using BR13 = Field<BRR_Reg, 13, 1>; // Port B Reset bit 13
        using BR14 = Field<BRR_Reg, 14, 1>; // Port B Reset bit 14
        using BR15 = Field<BRR_Reg, 15, 1>; // Port B Reset bit 15
    };

} // namespace GPIOB

// --------------------------------------------
// GPIOA: General-purpose I/Os
// Base address: 0x40020000
// --------------------------------------------

namespace GPIOA {
    static constexpr uint32_t GPIOA_BASE = 0x40020000;

    // GPIO port mode register
    using MODER_Reg = Register<GPIOA_BASE + 0x0, ReadWrite>;
    struct _MODER {
        using MODER15 = Field<MODER_Reg, 30, 2>; // Port x configuration bits (y = 0..15)
        using MODER14 = Field<MODER_Reg, 28, 2>; // Port x configuration bits (y = 0..15)
        using MODER13 = Field<MODER_Reg, 26, 2>; // Port x configuration bits (y = 0..15)
        using MODER12 = Field<MODER_Reg, 24, 2>; // Port x configuration bits (y = 0..15)
        using MODER11 = Field<MODER_Reg, 22, 2>; // Port x configuration bits (y = 0..15)
        using MODER10 = Field<MODER_Reg, 20, 2>; // Port x configuration bits (y = 0..15)
        using MODER9 = Field<MODER_Reg, 18, 2>; // Port x configuration bits (y = 0..15)
        using MODER8 = Field<MODER_Reg, 16, 2>; // Port x configuration bits (y = 0..15)
        using MODER7 = Field<MODER_Reg, 14, 2>; // Port x configuration bits (y = 0..15)
        using MODER6 = Field<MODER_Reg, 12, 2>; // Port x configuration bits (y = 0..15)
        using MODER5 = Field<MODER_Reg, 10, 2>; // Port x configuration bits (y = 0..15)
        using MODER4 = Field<MODER_Reg, 8, 2>; // Port x configuration bits (y = 0..15)
        using MODER3 = Field<MODER_Reg, 6, 2>; // Port x configuration bits (y = 0..15)
        using MODER2 = Field<MODER_Reg, 4, 2>; // Port x configuration bits (y = 0..15)
        using MODER1 = Field<MODER_Reg, 2, 2>; // Port x configuration bits (y = 0..15)
        using MODER0 = Field<MODER_Reg, 0, 2>; // Port x configuration bits (y = 0..15)
    };

    // GPIO port output type register
    using OTYPER_Reg = Register<GPIOA_BASE + 0x4, ReadWrite>;
    struct _OTYPER {
        using OT15 = Field<OTYPER_Reg, 15, 1>; // Port x configuration bits (y = 0..15)
        using OT14 = Field<OTYPER_Reg, 14, 1>; // Port x configuration bits (y = 0..15)
        using OT13 = Field<OTYPER_Reg, 13, 1>; // Port x configuration bits (y = 0..15)
        using OT12 = Field<OTYPER_Reg, 12, 1>; // Port x configuration bits (y = 0..15)
        using OT11 = Field<OTYPER_Reg, 11, 1>; // Port x configuration bits (y = 0..15)
        using OT10 = Field<OTYPER_Reg, 10, 1>; // Port x configuration bits (y = 0..15)
        using OT9 = Field<OTYPER_Reg, 9, 1>; // Port x configuration bits (y = 0..15)
        using OT8 = Field<OTYPER_Reg, 8, 1>; // Port x configuration bits (y = 0..15)
        using OT7 = Field<OTYPER_Reg, 7, 1>; // Port x configuration bits (y = 0..15)
        using OT6 = Field<OTYPER_Reg, 6, 1>; // Port x configuration bits (y = 0..15)
        using OT5 = Field<OTYPER_Reg, 5, 1>; // Port x configuration bits (y = 0..15)
        using OT4 = Field<OTYPER_Reg, 4, 1>; // Port x configuration bits (y = 0..15)
        using OT3 = Field<OTYPER_Reg, 3, 1>; // Port x configuration bits (y = 0..15)
        using OT2 = Field<OTYPER_Reg, 2, 1>; // Port x configuration bits (y = 0..15)
        using OT1 = Field<OTYPER_Reg, 1, 1>; // Port x configuration bits (y = 0..15)
        using OT0 = Field<OTYPER_Reg, 0, 1>; // Port x configuration bits (y = 0..15)
    };

    // GPIO port output speed register
    using OSPEEDR_Reg = Register<GPIOA_BASE + 0x8, ReadWrite>;
    struct _OSPEEDR {
        using OSPEEDR15 = Field<OSPEEDR_Reg, 30, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR14 = Field<OSPEEDR_Reg, 28, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR13 = Field<OSPEEDR_Reg, 26, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR12 = Field<OSPEEDR_Reg, 24, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR11 = Field<OSPEEDR_Reg, 22, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR10 = Field<OSPEEDR_Reg, 20, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR9 = Field<OSPEEDR_Reg, 18, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR8 = Field<OSPEEDR_Reg, 16, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR7 = Field<OSPEEDR_Reg, 14, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR6 = Field<OSPEEDR_Reg, 12, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR5 = Field<OSPEEDR_Reg, 10, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR4 = Field<OSPEEDR_Reg, 8, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR3 = Field<OSPEEDR_Reg, 6, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR2 = Field<OSPEEDR_Reg, 4, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR1 = Field<OSPEEDR_Reg, 2, 2>; // Port x configuration bits (y = 0..15)
        using OSPEEDR0 = Field<OSPEEDR_Reg, 0, 2>; // Port x configuration bits (y = 0..15)
    };

    // GPIO port pull-up/pull-down register
    using PUPDR_Reg = Register<GPIOA_BASE + 0xC, ReadWrite>;
    struct _PUPDR {
        using PUPDR15 = Field<PUPDR_Reg, 30, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR14 = Field<PUPDR_Reg, 28, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR13 = Field<PUPDR_Reg, 26, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR12 = Field<PUPDR_Reg, 24, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR11 = Field<PUPDR_Reg, 22, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR10 = Field<PUPDR_Reg, 20, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR9 = Field<PUPDR_Reg, 18, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR8 = Field<PUPDR_Reg, 16, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR7 = Field<PUPDR_Reg, 14, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR6 = Field<PUPDR_Reg, 12, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR5 = Field<PUPDR_Reg, 10, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR4 = Field<PUPDR_Reg, 8, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR3 = Field<PUPDR_Reg, 6, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR2 = Field<PUPDR_Reg, 4, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR1 = Field<PUPDR_Reg, 2, 2>; // Port x configuration bits (y = 0..15)
        using PUPDR0 = Field<PUPDR_Reg, 0, 2>; // Port x configuration bits (y = 0..15)
    };

    // GPIO port input data register
    using IDR_Reg = Register<GPIOA_BASE + 0x10, ReadOnly>;
    struct _IDR {
        using IDR15 = Field<IDR_Reg, 15, 1>; // Port input data (y = 0..15)
        using IDR14 = Field<IDR_Reg, 14, 1>; // Port input data (y = 0..15)
        using IDR13 = Field<IDR_Reg, 13, 1>; // Port input data (y = 0..15)
        using IDR12 = Field<IDR_Reg, 12, 1>; // Port input data (y = 0..15)
        using IDR11 = Field<IDR_Reg, 11, 1>; // Port input data (y = 0..15)
        using IDR10 = Field<IDR_Reg, 10, 1>; // Port input data (y = 0..15)
        using IDR9 = Field<IDR_Reg, 9, 1>; // Port input data (y = 0..15)
        using IDR8 = Field<IDR_Reg, 8, 1>; // Port input data (y = 0..15)
        using IDR7 = Field<IDR_Reg, 7, 1>; // Port input data (y = 0..15)
        using IDR6 = Field<IDR_Reg, 6, 1>; // Port input data (y = 0..15)
        using IDR5 = Field<IDR_Reg, 5, 1>; // Port input data (y = 0..15)
        using IDR4 = Field<IDR_Reg, 4, 1>; // Port input data (y = 0..15)
        using IDR3 = Field<IDR_Reg, 3, 1>; // Port input data (y = 0..15)
        using IDR2 = Field<IDR_Reg, 2, 1>; // Port input data (y = 0..15)
        using IDR1 = Field<IDR_Reg, 1, 1>; // Port input data (y = 0..15)
        using IDR0 = Field<IDR_Reg, 0, 1>; // Port input data (y = 0..15)
    };

    // GPIO port output data register
    using ODR_Reg = Register<GPIOA_BASE + 0x14, ReadWrite>;
    struct _ODR {
        using ODR15 = Field<ODR_Reg, 15, 1>; // Port output data (y = 0..15)
        using ODR14 = Field<ODR_Reg, 14, 1>; // Port output data (y = 0..15)
        using ODR13 = Field<ODR_Reg, 13, 1>; // Port output data (y = 0..15)
        using ODR12 = Field<ODR_Reg, 12, 1>; // Port output data (y = 0..15)
        using ODR11 = Field<ODR_Reg, 11, 1>; // Port output data (y = 0..15)
        using ODR10 = Field<ODR_Reg, 10, 1>; // Port output data (y = 0..15)
        using ODR9 = Field<ODR_Reg, 9, 1>; // Port output data (y = 0..15)
        using ODR8 = Field<ODR_Reg, 8, 1>; // Port output data (y = 0..15)
        using ODR7 = Field<ODR_Reg, 7, 1>; // Port output data (y = 0..15)
        using ODR6 = Field<ODR_Reg, 6, 1>; // Port output data (y = 0..15)
        using ODR5 = Field<ODR_Reg, 5, 1>; // Port output data (y = 0..15)
        using ODR4 = Field<ODR_Reg, 4, 1>; // Port output data (y = 0..15)
        using ODR3 = Field<ODR_Reg, 3, 1>; // Port output data (y = 0..15)
        using ODR2 = Field<ODR_Reg, 2, 1>; // Port output data (y = 0..15)
        using ODR1 = Field<ODR_Reg, 1, 1>; // Port output data (y = 0..15)
        using ODR0 = Field<ODR_Reg, 0, 1>; // Port output data (y = 0..15)
    };

    // GPIO port bit set/reset register
    using BSRR_Reg = Register<GPIOA_BASE + 0x18, WriteOnly>;
    struct _BSRR {
        using BR15 = Field<BSRR_Reg, 31, 1>; // Port x reset bit y (y = 0..15)
        using BR14 = Field<BSRR_Reg, 30, 1>; // Port x reset bit y (y = 0..15)
        using BR13 = Field<BSRR_Reg, 29, 1>; // Port x reset bit y (y = 0..15)
        using BR12 = Field<BSRR_Reg, 28, 1>; // Port x reset bit y (y = 0..15)
        using BR11 = Field<BSRR_Reg, 27, 1>; // Port x reset bit y (y = 0..15)
        using BR10 = Field<BSRR_Reg, 26, 1>; // Port x reset bit y (y = 0..15)
        using BR9 = Field<BSRR_Reg, 25, 1>; // Port x reset bit y (y = 0..15)
        using BR8 = Field<BSRR_Reg, 24, 1>; // Port x reset bit y (y = 0..15)
        using BR7 = Field<BSRR_Reg, 23, 1>; // Port x reset bit y (y = 0..15)
        using BR6 = Field<BSRR_Reg, 22, 1>; // Port x reset bit y (y = 0..15)
        using BR5 = Field<BSRR_Reg, 21, 1>; // Port x reset bit y (y = 0..15)
        using BR4 = Field<BSRR_Reg, 20, 1>; // Port x reset bit y (y = 0..15)
        using BR3 = Field<BSRR_Reg, 19, 1>; // Port x reset bit y (y = 0..15)
        using BR2 = Field<BSRR_Reg, 18, 1>; // Port x reset bit y (y = 0..15)
        using BR1 = Field<BSRR_Reg, 17, 1>; // Port x reset bit y (y = 0..15)
        using BR0 = Field<BSRR_Reg, 16, 1>; // Port x set bit y (y= 0..15)
        using BS15 = Field<BSRR_Reg, 15, 1>; // Port x set bit y (y= 0..15)
        using BS14 = Field<BSRR_Reg, 14, 1>; // Port x set bit y (y= 0..15)
        using BS13 = Field<BSRR_Reg, 13, 1>; // Port x set bit y (y= 0..15)
        using BS12 = Field<BSRR_Reg, 12, 1>; // Port x set bit y (y= 0..15)
        using BS11 = Field<BSRR_Reg, 11, 1>; // Port x set bit y (y= 0..15)
        using BS10 = Field<BSRR_Reg, 10, 1>; // Port x set bit y (y= 0..15)
        using BS9 = Field<BSRR_Reg, 9, 1>; // Port x set bit y (y= 0..15)
        using BS8 = Field<BSRR_Reg, 8, 1>; // Port x set bit y (y= 0..15)
        using BS7 = Field<BSRR_Reg, 7, 1>; // Port x set bit y (y= 0..15)
        using BS6 = Field<BSRR_Reg, 6, 1>; // Port x set bit y (y= 0..15)
        using BS5 = Field<BSRR_Reg, 5, 1>; // Port x set bit y (y= 0..15)
        using BS4 = Field<BSRR_Reg, 4, 1>; // Port x set bit y (y= 0..15)
        using BS3 = Field<BSRR_Reg, 3, 1>; // Port x set bit y (y= 0..15)
        using BS2 = Field<BSRR_Reg, 2, 1>; // Port x set bit y (y= 0..15)
        using BS1 = Field<BSRR_Reg, 1, 1>; // Port x set bit y (y= 0..15)
        using BS0 = Field<BSRR_Reg, 0, 1>; // Port x set bit y (y= 0..15)
    };

    // GPIO port configuration lock register
    using LCKR_Reg = Register<GPIOA_BASE + 0x1C, ReadWrite>;
    struct _LCKR {
        using LCKK = Field<LCKR_Reg, 16, 1>; // Port x lock bit y (y= 0..15)
        using LCK15 = Field<LCKR_Reg, 15, 1>; // Port x lock bit y (y= 0..15)
        using LCK14 = Field<LCKR_Reg, 14, 1>; // Port x lock bit y (y= 0..15)
        using LCK13 = Field<LCKR_Reg, 13, 1>; // Port x lock bit y (y= 0..15)
        using LCK12 = Field<LCKR_Reg, 12, 1>; // Port x lock bit y (y= 0..15)
        using LCK11 = Field<LCKR_Reg, 11, 1>; // Port x lock bit y (y= 0..15)
        using LCK10 = Field<LCKR_Reg, 10, 1>; // Port x lock bit y (y= 0..15)
        using LCK9 = Field<LCKR_Reg, 9, 1>; // Port x lock bit y (y= 0..15)
        using LCK8 = Field<LCKR_Reg, 8, 1>; // Port x lock bit y (y= 0..15)
        using LCK7 = Field<LCKR_Reg, 7, 1>; // Port x lock bit y (y= 0..15)
        using LCK6 = Field<LCKR_Reg, 6, 1>; // Port x lock bit y (y= 0..15)
        using LCK5 = Field<LCKR_Reg, 5, 1>; // Port x lock bit y (y= 0..15)
        using LCK4 = Field<LCKR_Reg, 4, 1>; // Port x lock bit y (y= 0..15)
        using LCK3 = Field<LCKR_Reg, 3, 1>; // Port x lock bit y (y= 0..15)
        using LCK2 = Field<LCKR_Reg, 2, 1>; // Port x lock bit y (y= 0..15)
        using LCK1 = Field<LCKR_Reg, 1, 1>; // Port x lock bit y (y= 0..15)
        using LCK0 = Field<LCKR_Reg, 0, 1>; // Port x lock bit y (y= 0..15)
    };

    // GPIO alternate function low register
    using AFRL_Reg = Register<GPIOA_BASE + 0x20, ReadWrite>;
    struct _AFRL {
        using AFRL7 = Field<AFRL_Reg, 28, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL6 = Field<AFRL_Reg, 24, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL5 = Field<AFRL_Reg, 20, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL4 = Field<AFRL_Reg, 16, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL3 = Field<AFRL_Reg, 12, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL2 = Field<AFRL_Reg, 8, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL1 = Field<AFRL_Reg, 4, 4>; // Alternate function selection for port x bit y (y = 0..7)
        using AFRL0 = Field<AFRL_Reg, 0, 4>; // Alternate function selection for port x bit y (y = 0..7)
    };

    // GPIO alternate function high register
    using AFRH_Reg = Register<GPIOA_BASE + 0x24, ReadWrite>;
    struct _AFRH {
        using AFRH15 = Field<AFRH_Reg, 28, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH14 = Field<AFRH_Reg, 24, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH13 = Field<AFRH_Reg, 20, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH12 = Field<AFRH_Reg, 16, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH11 = Field<AFRH_Reg, 12, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH10 = Field<AFRH_Reg, 8, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH9 = Field<AFRH_Reg, 4, 4>; // Alternate function selection for port x bit y (y = 8..15)
        using AFRH8 = Field<AFRH_Reg, 0, 4>; // Alternate function selection for port x bit y (y = 8..15)
    };

    // GPIO port bit reset register
    using BRR_Reg = Register<GPIOA_BASE + 0x28, ReadWrite>;
    struct _BRR {
        using BR0 = Field<BRR_Reg, 0, 1>; // Port A Reset bit 0
        using BR1 = Field<BRR_Reg, 1, 1>; // Port A Reset bit 1
        using BR2 = Field<BRR_Reg, 2, 1>; // Port A Reset bit 2
        using BR3 = Field<BRR_Reg, 3, 1>; // Port A Reset bit 3
        using BR4 = Field<BRR_Reg, 4, 1>; // Port A Reset bit 4
        using BR5 = Field<BRR_Reg, 5, 1>; // Port A Reset bit 5
        using BR6 = Field<BRR_Reg, 6, 1>; // Port A Reset bit 6
        using BR7 = Field<BRR_Reg, 7, 1>; // Port A Reset bit 7
        using BR8 = Field<BRR_Reg, 8, 1>; // Port A Reset bit 8
        using BR9 = Field<BRR_Reg, 9, 1>; // Port A Reset bit 9
        using BR10 = Field<BRR_Reg, 10, 1>; // Port A Reset bit 10
        using BR11 = Field<BRR_Reg, 11, 1>; // Port A Reset bit 11
        using BR12 = Field<BRR_Reg, 12, 1>; // Port A Reset bit 12
        using BR13 = Field<BRR_Reg, 13, 1>; // Port A Reset bit 13
        using BR14 = Field<BRR_Reg, 14, 1>; // Port A Reset bit 14
        using BR15 = Field<BRR_Reg, 15, 1>; // Port A Reset bit 15
    };

} // namespace GPIOA

// --------------------------------------------
// SYSCFG: System configuration controller
// Base address: 0x40013800
// --------------------------------------------

namespace SYSCFG {
    static constexpr uint32_t SYSCFG_BASE = 0x40013800;

    // memory remap register
    using MEMRM_Reg = Register<SYSCFG_BASE + 0x0, ReadWrite>;
    struct _MEMRM {
        using MEM_MODE = Field<MEMRM_Reg, 0, 3>; // Memory mapping selection
        using FB_MODE = Field<MEMRM_Reg, 8, 1>; // Flash bank mode selection
        using SWP_FMC = Field<MEMRM_Reg, 10, 2>; // FMC memory mapping swap
    };

    // peripheral mode configuration register
    using PMC_Reg = Register<SYSCFG_BASE + 0x4, ReadWrite>;
    struct _PMC {
        using MII_RMII_SEL = Field<PMC_Reg, 23, 1>; // Ethernet PHY interface selection
        using ADC1DC2 = Field<PMC_Reg, 16, 1>; // ADC1DC2
        using ADC2DC2 = Field<PMC_Reg, 17, 1>; // ADC2DC2
        using ADC3DC2 = Field<PMC_Reg, 18, 1>; // ADC3DC2
    };

    // external interrupt configuration register 1
    using EXTICR1_Reg = Register<SYSCFG_BASE + 0x8, ReadWrite>;
    struct _EXTICR1 {
        using EXTI3 = Field<EXTICR1_Reg, 12, 4>; // EXTI x configuration (x = 0 to 3)
        using EXTI2 = Field<EXTICR1_Reg, 8, 4>; // EXTI x configuration (x = 0 to 3)
        using EXTI1 = Field<EXTICR1_Reg, 4, 4>; // EXTI x configuration (x = 0 to 3)
        using EXTI0 = Field<EXTICR1_Reg, 0, 4>; // EXTI x configuration (x = 0 to 3)
    };

    // external interrupt configuration register 2
    using EXTICR2_Reg = Register<SYSCFG_BASE + 0xC, ReadWrite>;
    struct _EXTICR2 {
        using EXTI7 = Field<EXTICR2_Reg, 12, 4>; // EXTI x configuration (x = 4 to 7)
        using EXTI6 = Field<EXTICR2_Reg, 8, 4>; // EXTI x configuration (x = 4 to 7)
        using EXTI5 = Field<EXTICR2_Reg, 4, 4>; // EXTI x configuration (x = 4 to 7)
        using EXTI4 = Field<EXTICR2_Reg, 0, 4>; // EXTI x configuration (x = 4 to 7)
    };

    // external interrupt configuration register 3
    using EXTICR3_Reg = Register<SYSCFG_BASE + 0x10, ReadWrite>;
    struct _EXTICR3 {
        using EXTI11 = Field<EXTICR3_Reg, 12, 4>; // EXTI x configuration (x = 8 to 11)
        using EXTI10 = Field<EXTICR3_Reg, 8, 4>; // EXTI10
        using EXTI9 = Field<EXTICR3_Reg, 4, 4>; // EXTI x configuration (x = 8 to 11)
        using EXTI8 = Field<EXTICR3_Reg, 0, 4>; // EXTI x configuration (x = 8 to 11)
    };

    // external interrupt configuration register 4
    using EXTICR4_Reg = Register<SYSCFG_BASE + 0x14, ReadWrite>;
    struct _EXTICR4 {
        using EXTI15 = Field<EXTICR4_Reg, 12, 4>; // EXTI x configuration (x = 12 to 15)
        using EXTI14 = Field<EXTICR4_Reg, 8, 4>; // EXTI x configuration (x = 12 to 15)
        using EXTI13 = Field<EXTICR4_Reg, 4, 4>; // EXTI x configuration (x = 12 to 15)
        using EXTI12 = Field<EXTICR4_Reg, 0, 4>; // EXTI x configuration (x = 12 to 15)
    };

    // Compensation cell control register
    using CMPCR_Reg = Register<SYSCFG_BASE + 0x20, ReadOnly>;
    struct _CMPCR {
        using READY = Field<CMPCR_Reg, 8, 1>; // READY
        using CMP_PD = Field<CMPCR_Reg, 0, 1>; // Compensation cell power-down
    };

} // namespace SYSCFG

// --------------------------------------------
// SPI1: Serial peripheral interface
// Base address: 0x40013000
// --------------------------------------------

namespace SPI1 {
    static constexpr uint32_t SPI1_BASE = 0x40013000;

    // control register 1
    using CR1_Reg = Register<SPI1_BASE + 0x0, ReadWrite>;
    struct _CR1 {
        using BIDIMODE = Field<CR1_Reg, 15, 1>; // Bidirectional data mode enable
        using BIDIOE = Field<CR1_Reg, 14, 1>; // Output enable in bidirectional mode
        using CRCEN = Field<CR1_Reg, 13, 1>; // Hardware CRC calculation enable
        using CRCNEXT = Field<CR1_Reg, 12, 1>; // CRC transfer next
        using CRCL = Field<CR1_Reg, 11, 1>; // CRC length
        using RXONLY = Field<CR1_Reg, 10, 1>; // Receive only
        using SSM = Field<CR1_Reg, 9, 1>; // Software slave management
        using SSI = Field<CR1_Reg, 8, 1>; // Internal slave select
        using LSBFIRST = Field<CR1_Reg, 7, 1>; // Frame format
        using SPE = Field<CR1_Reg, 6, 1>; // SPI enable
        using BR = Field<CR1_Reg, 3, 3>; // Baud rate control
        using MSTR = Field<CR1_Reg, 2, 1>; // Master selection
        using CPOL = Field<CR1_Reg, 1, 1>; // Clock polarity
        using CPHA = Field<CR1_Reg, 0, 1>; // Clock phase
    };

    // control register 2
    using CR2_Reg = Register<SPI1_BASE + 0x4, ReadWrite>;
    struct _CR2 {
        using RXDMAEN = Field<CR2_Reg, 0, 1>; // Rx buffer DMA enable
        using TXDMAEN = Field<CR2_Reg, 1, 1>; // Tx buffer DMA enable
        using SSOE = Field<CR2_Reg, 2, 1>; // SS output enable
        using NSSP = Field<CR2_Reg, 3, 1>; // NSS pulse management
        using FRF = Field<CR2_Reg, 4, 1>; // Frame format
        using ERRIE = Field<CR2_Reg, 5, 1>; // Error interrupt enable
        using RXNEIE = Field<CR2_Reg, 6, 1>; // RX buffer not empty interrupt enable
        using TXEIE = Field<CR2_Reg, 7, 1>; // Tx buffer empty interrupt enable
        using DS = Field<CR2_Reg, 8, 4>; // Data size
        using FRXTH = Field<CR2_Reg, 12, 1>; // FIFO reception threshold
        using LDMA_RX = Field<CR2_Reg, 13, 1>; // Last DMA transfer for reception
        using LDMA_TX = Field<CR2_Reg, 14, 1>; // Last DMA transfer for transmission
    };

    // status register
    using SR_Reg = Register<SPI1_BASE + 0x8, ReadWrite>;
    struct _SR {
        using FRE = Field<SR_Reg, 8, 1>; // Frame format error
        using BSY = Field<SR_Reg, 7, 1>; // Busy flag
        using OVR = Field<SR_Reg, 6, 1>; // Overrun flag
        using MODF = Field<SR_Reg, 5, 1>; // Mode fault
        using CRCERR = Field<SR_Reg, 4, 1>; // CRC error flag
        using UDR = Field<SR_Reg, 3, 1>; // Underrun flag
        using CHSIDE = Field<SR_Reg, 2, 1>; // Channel side
        using TXE = Field<SR_Reg, 1, 1>; // Transmit buffer empty
        using RXNE = Field<SR_Reg, 0, 1>; // Receive buffer not empty
        using FRLVL = Field<SR_Reg, 9, 2>; // FIFO reception level
        using FTLVL = Field<SR_Reg, 11, 2>; // FIFO Transmission Level
    };

    // data register
    using DR_Reg = Register<SPI1_BASE + 0xC, ReadWrite>;
    struct _DR {
        using DR = Field<DR_Reg, 0, 16>; // Data register
    };

    // CRC polynomial register
    using CRCPR_Reg = Register<SPI1_BASE + 0x10, ReadWrite>;
    struct _CRCPR {
        using CRCPOLY = Field<CRCPR_Reg, 0, 16>; // CRC polynomial register
    };

    // RX CRC register
    using RXCRCR_Reg = Register<SPI1_BASE + 0x14, ReadOnly>;
    struct _RXCRCR {
        using RxCRC = Field<RXCRCR_Reg, 0, 16>; // Rx CRC register
    };

    // TX CRC register
    using TXCRCR_Reg = Register<SPI1_BASE + 0x18, ReadOnly>;
    struct _TXCRCR {
        using TxCRC = Field<TXCRCR_Reg, 0, 16>; // Tx CRC register
    };

    // I2S configuration register
    using I2SCFGR_Reg = Register<SPI1_BASE + 0x1C, ReadWrite>;
    struct _I2SCFGR {
        using I2SMOD = Field<I2SCFGR_Reg, 11, 1>; // I2S mode selection
        using I2SE = Field<I2SCFGR_Reg, 10, 1>; // I2S Enable
        using I2SCFG = Field<I2SCFGR_Reg, 8, 2>; // I2S configuration mode
        using PCMSYNC = Field<I2SCFGR_Reg, 7, 1>; // PCM frame synchronization
        using I2SSTD = Field<I2SCFGR_Reg, 4, 2>; // I2S standard selection
        using CKPOL = Field<I2SCFGR_Reg, 3, 1>; // Steady state clock polarity
        using DATLEN = Field<I2SCFGR_Reg, 1, 2>; // Data length to be transferred
        using CHLEN = Field<I2SCFGR_Reg, 0, 1>; // Channel length (number of bits per audio channel)
        using ASTRTEN = Field<I2SCFGR_Reg, 12, 1>; // Asynchronous start enable
    };

    // I2S prescaler register
    using I2SPR_Reg = Register<SPI1_BASE + 0x20, ReadWrite>;
    struct _I2SPR {
        using MCKOE = Field<I2SPR_Reg, 9, 1>; // Master clock output enable
        using ODD = Field<I2SPR_Reg, 8, 1>; // Odd factor for the prescaler
        using I2SDIV = Field<I2SPR_Reg, 0, 8>; // I2S Linear prescaler
    };

} // namespace SPI1

// --------------------------------------------
// SPI2: 
// Base address: 0x40003800
// --------------------------------------------

namespace SPI2 {
    static constexpr uint32_t SPI2_BASE = 0x40003800;

} // namespace SPI2

// --------------------------------------------
// SPI4: 
// Base address: 0x40013400
// --------------------------------------------

namespace SPI4 {
    static constexpr uint32_t SPI4_BASE = 0x40013400;

} // namespace SPI4

// --------------------------------------------
// SPI5: 
// Base address: 0x40015000
// --------------------------------------------

namespace SPI5 {
    static constexpr uint32_t SPI5_BASE = 0x40015000;

} // namespace SPI5

// --------------------------------------------
// SPI3: 
// Base address: 0x40003C00
// --------------------------------------------

namespace SPI3 {
    static constexpr uint32_t SPI3_BASE = 0x40003C00;

} // namespace SPI3

// --------------------------------------------
// SPI6: 
// Base address: 0x40015400
// --------------------------------------------

namespace SPI6 {
    static constexpr uint32_t SPI6_BASE = 0x40015400;

} // namespace SPI6

// --------------------------------------------
// ADC1: Analog-to-digital converter
// Base address: 0x40012000
// --------------------------------------------

namespace ADC1 {
    static constexpr uint32_t ADC1_BASE = 0x40012000;

    // status register
    using SR_Reg = Register<ADC1_BASE + 0x0, ReadWrite>;
    struct _SR {
        using OVR = Field<SR_Reg, 5, 1>; // Overrun
        using STRT = Field<SR_Reg, 4, 1>; // Regular channel start flag
        using JSTRT = Field<SR_Reg, 3, 1>; // Injected channel start flag
        using JEOC = Field<SR_Reg, 2, 1>; // Injected channel end of conversion
        using EOC = Field<SR_Reg, 1, 1>; // Regular channel end of conversion
        using AWD = Field<SR_Reg, 0, 1>; // Analog watchdog flag
    };

    // control register 1
    using CR1_Reg = Register<ADC1_BASE + 0x4, ReadWrite>;
    struct _CR1 {
        using OVRIE = Field<CR1_Reg, 26, 1>; // Overrun interrupt enable
        using RES = Field<CR1_Reg, 24, 2>; // Resolution
        using AWDEN = Field<CR1_Reg, 23, 1>; // Analog watchdog enable on regular channels
        using JAWDEN = Field<CR1_Reg, 22, 1>; // Analog watchdog enable on injected channels
        using DISCNUM = Field<CR1_Reg, 13, 3>; // Discontinuous mode channel count
        using JDISCEN = Field<CR1_Reg, 12, 1>; // Discontinuous mode on injected channels
        using DISCEN = Field<CR1_Reg, 11, 1>; // Discontinuous mode on regular channels
        using JAUTO = Field<CR1_Reg, 10, 1>; // Automatic injected group conversion
        using AWDSGL = Field<CR1_Reg, 9, 1>; // Enable the watchdog on a single channel in scan mode
        using SCAN = Field<CR1_Reg, 8, 1>; // Scan mode
        using JEOCIE = Field<CR1_Reg, 7, 1>; // Interrupt enable for injected channels
        using AWDIE = Field<CR1_Reg, 6, 1>; // Analog watchdog interrupt enable
        using EOCIE = Field<CR1_Reg, 5, 1>; // Interrupt enable for EOC
        using AWDCH = Field<CR1_Reg, 0, 5>; // Analog watchdog channel select bits
    };

    // control register 2
    using CR2_Reg = Register<ADC1_BASE + 0x8, ReadWrite>;
    struct _CR2 {
        using SWSTART = Field<CR2_Reg, 30, 1>; // Start conversion of regular channels
        using EXTEN = Field<CR2_Reg, 28, 2>; // External trigger enable for regular channels
        using EXTSEL = Field<CR2_Reg, 24, 4>; // External event select for regular group
        using JSWSTART = Field<CR2_Reg, 22, 1>; // Start conversion of injected channels
        using JEXTEN = Field<CR2_Reg, 20, 2>; // External trigger enable for injected channels
        using JEXTSEL = Field<CR2_Reg, 16, 4>; // External event select for injected group
        using ALIGN = Field<CR2_Reg, 11, 1>; // Data alignment
        using EOCS = Field<CR2_Reg, 10, 1>; // End of conversion selection
        using DDS = Field<CR2_Reg, 9, 1>; // DMA disable selection (for single ADC mode)
        using DMA = Field<CR2_Reg, 8, 1>; // Direct memory access mode (for single ADC mode)
        using CONT = Field<CR2_Reg, 1, 1>; // Continuous conversion
        using ADON = Field<CR2_Reg, 0, 1>; // A/D Converter ON / OFF
    };

    // sample time register 1
    using SMPR1_Reg = Register<ADC1_BASE + 0xC, ReadWrite>;
    struct _SMPR1 {
        using SMPx_x = Field<SMPR1_Reg, 0, 32>; // Sample time bits
    };

    // sample time register 2
    using SMPR2_Reg = Register<ADC1_BASE + 0x10, ReadWrite>;
    struct _SMPR2 {
        using SMPx_x = Field<SMPR2_Reg, 0, 32>; // Sample time bits
    };

    // injected channel data offset register x
    using JOFR1_Reg = Register<ADC1_BASE + 0x14, ReadWrite>;
    struct _JOFR1 {
        using JOFFSET1 = Field<JOFR1_Reg, 0, 12>; // Data offset for injected channel x
    };

    // injected channel data offset register x
    using JOFR2_Reg = Register<ADC1_BASE + 0x18, ReadWrite>;
    struct _JOFR2 {
        using JOFFSET2 = Field<JOFR2_Reg, 0, 12>; // Data offset for injected channel x
    };

    // injected channel data offset register x
    using JOFR3_Reg = Register<ADC1_BASE + 0x1C, ReadWrite>;
    struct _JOFR3 {
        using JOFFSET3 = Field<JOFR3_Reg, 0, 12>; // Data offset for injected channel x
    };

    // injected channel data offset register x
    using JOFR4_Reg = Register<ADC1_BASE + 0x20, ReadWrite>;
    struct _JOFR4 {
        using JOFFSET4 = Field<JOFR4_Reg, 0, 12>; // Data offset for injected channel x
    };

    // watchdog higher threshold register
    using HTR_Reg = Register<ADC1_BASE + 0x24, ReadWrite>;
    struct _HTR {
        using HT = Field<HTR_Reg, 0, 12>; // Analog watchdog higher threshold
    };

    // watchdog lower threshold register
    using LTR_Reg = Register<ADC1_BASE + 0x28, ReadWrite>;
    struct _LTR {
        using LT = Field<LTR_Reg, 0, 12>; // Analog watchdog lower threshold
    };

    // regular sequence register 1
    using SQR1_Reg = Register<ADC1_BASE + 0x2C, ReadWrite>;
    struct _SQR1 {
        using L = Field<SQR1_Reg, 20, 4>; // Regular channel sequence length
        using SQ16 = Field<SQR1_Reg, 15, 5>; // 16th conversion in regular sequence
        using SQ15 = Field<SQR1_Reg, 10, 5>; // 15th conversion in regular sequence
        using SQ14 = Field<SQR1_Reg, 5, 5>; // 14th conversion in regular sequence
        using SQ13 = Field<SQR1_Reg, 0, 5>; // 13th conversion in regular sequence
    };

    // regular sequence register 2
    using SQR2_Reg = Register<ADC1_BASE + 0x30, ReadWrite>;
    struct _SQR2 {
        using SQ12 = Field<SQR2_Reg, 25, 5>; // 12th conversion in regular sequence
        using SQ11 = Field<SQR2_Reg, 20, 5>; // 11th conversion in regular sequence
        using SQ10 = Field<SQR2_Reg, 15, 5>; // 10th conversion in regular sequence
        using SQ9 = Field<SQR2_Reg, 10, 5>; // 9th conversion in regular sequence
        using SQ8 = Field<SQR2_Reg, 5, 5>; // 8th conversion in regular sequence
        using SQ7 = Field<SQR2_Reg, 0, 5>; // 7th conversion in regular sequence
    };

    // regular sequence register 3
    using SQR3_Reg = Register<ADC1_BASE + 0x34, ReadWrite>;
    struct _SQR3 {
        using SQ6 = Field<SQR3_Reg, 25, 5>; // 6th conversion in regular sequence
        using SQ5 = Field<SQR3_Reg, 20, 5>; // 5th conversion in regular sequence
        using SQ4 = Field<SQR3_Reg, 15, 5>; // 4th conversion in regular sequence
        using SQ3 = Field<SQR3_Reg, 10, 5>; // 3rd conversion in regular sequence
        using SQ2 = Field<SQR3_Reg, 5, 5>; // 2nd conversion in regular sequence
        using SQ1 = Field<SQR3_Reg, 0, 5>; // 1st conversion in regular sequence
    };

    // injected sequence register
    using JSQR_Reg = Register<ADC1_BASE + 0x38, ReadWrite>;
    struct _JSQR {
        using JL = Field<JSQR_Reg, 20, 2>; // Injected sequence length
        using JSQ4 = Field<JSQR_Reg, 15, 5>; // 4th conversion in injected sequence
        using JSQ3 = Field<JSQR_Reg, 10, 5>; // 3rd conversion in injected sequence
        using JSQ2 = Field<JSQR_Reg, 5, 5>; // 2nd conversion in injected sequence
        using JSQ1 = Field<JSQR_Reg, 0, 5>; // 1st conversion in injected sequence
    };

    // injected data register x
    using JDR1_Reg = Register<ADC1_BASE + 0x3C, ReadOnly>;
    struct _JDR1 {
        using JDATA = Field<JDR1_Reg, 0, 16>; // Injected data
    };

    // injected data register x
    using JDR2_Reg = Register<ADC1_BASE + 0x40, ReadOnly>;
    struct _JDR2 {
        using JDATA = Field<JDR2_Reg, 0, 16>; // Injected data
    };

    // injected data register x
    using JDR3_Reg = Register<ADC1_BASE + 0x44, ReadOnly>;
    struct _JDR3 {
        using JDATA = Field<JDR3_Reg, 0, 16>; // Injected data
    };

    // injected data register x
    using JDR4_Reg = Register<ADC1_BASE + 0x48, ReadOnly>;
    struct _JDR4 {
        using JDATA = Field<JDR4_Reg, 0, 16>; // Injected data
    };

    // regular data register
    using DR_Reg = Register<ADC1_BASE + 0x4C, ReadOnly>;
    struct _DR {
        using DATA = Field<DR_Reg, 0, 16>; // Regular data
    };

} // namespace ADC1

// --------------------------------------------
// ADC2: 
// Base address: 0x40012100
// --------------------------------------------

namespace ADC2 {
    static constexpr uint32_t ADC2_BASE = 0x40012100;

} // namespace ADC2

// --------------------------------------------
// ADC3: 
// Base address: 0x40012200
// --------------------------------------------

namespace ADC3 {
    static constexpr uint32_t ADC3_BASE = 0x40012200;

} // namespace ADC3

// --------------------------------------------
// ADC_Common: Common ADC registers
// Base address: 0x40012300
// --------------------------------------------

namespace ADC_Common {
    static constexpr uint32_t ADC_Common_BASE = 0x40012300;

    // ADC Common status register
    using CSR_Reg = Register<ADC_Common_BASE + 0x0, ReadOnly>;
    struct _CSR {
        using AWD1 = Field<CSR_Reg, 0, 1>; // AWD1
        using EOC1 = Field<CSR_Reg, 1, 1>; // EOC1
        using JEOC1 = Field<CSR_Reg, 2, 1>; // JEOC1
        using JSTRT1 = Field<CSR_Reg, 3, 1>; // JSTRT1
        using STRT1 = Field<CSR_Reg, 4, 1>; // STRT1
        using OVR1 = Field<CSR_Reg, 5, 1>; // OVR1
        using AWD2 = Field<CSR_Reg, 8, 1>; // AWD2
        using EOC2 = Field<CSR_Reg, 9, 1>; // EOC2
        using JEOC2 = Field<CSR_Reg, 10, 1>; // JEOC2
        using JSTRT2 = Field<CSR_Reg, 11, 1>; // JSTRT2
        using STRT2 = Field<CSR_Reg, 12, 1>; // STRT2
        using OVR2 = Field<CSR_Reg, 13, 1>; // OVR2
        using AWD3 = Field<CSR_Reg, 16, 1>; // AWD3
        using EOC3 = Field<CSR_Reg, 17, 1>; // EOC3
        using JEOC3 = Field<CSR_Reg, 18, 1>; // JEOC3
        using JSTRT3 = Field<CSR_Reg, 19, 1>; // JSTRT3
        using STRT3 = Field<CSR_Reg, 20, 1>; // STRT3
        using OVR3 = Field<CSR_Reg, 21, 1>; // OVR3
    };

    // ADC common control register
    using CCR_Reg = Register<ADC_Common_BASE + 0x4, ReadWrite>;
    struct _CCR {
        using MULTI = Field<CCR_Reg, 0, 5>; // MULTI
        using DELAY = Field<CCR_Reg, 8, 4>; // DELAY
        using DDS = Field<CCR_Reg, 13, 1>; // DDS
        using DMA = Field<CCR_Reg, 14, 2>; // DMA
        using ADCPRE = Field<CCR_Reg, 16, 2>; // ADCPRE
        using VBATE = Field<CCR_Reg, 22, 1>; // VBATE
        using TSVREFE = Field<CCR_Reg, 23, 1>; // TSVREFE
    };

    // ADC common regular data register for dual and triple modes
    using CDR_Reg = Register<ADC_Common_BASE + 0x8, ReadOnly>;
    struct _CDR {
        using DATA1 = Field<CDR_Reg, 0, 16>; // DATA1
        using DATA2 = Field<CDR_Reg, 16, 16>; // DATA2
    };

} // namespace ADC_Common

// --------------------------------------------
// DAC: Digital-to-analog converter
// Base address: 0x40007400
// --------------------------------------------

namespace DAC {
    static constexpr uint32_t DAC_BASE = 0x40007400;

    // control register
    using CR_Reg = Register<DAC_BASE + 0x0, ReadWrite>;
    struct _CR {
        using DMAUDRIE2 = Field<CR_Reg, 29, 1>; // DAC channel2 DMA underrun interrupt enable
        using DMAEN2 = Field<CR_Reg, 28, 1>; // DAC channel2 DMA enable
        using MAMP2 = Field<CR_Reg, 24, 4>; // DAC channel2 mask/amplitude selector
        using WAVE2 = Field<CR_Reg, 22, 2>; // DAC channel2 noise/triangle wave generation enable
        using TSEL2 = Field<CR_Reg, 19, 3>; // DAC channel2 trigger selection
        using TEN2 = Field<CR_Reg, 18, 1>; // DAC channel2 trigger enable
        using BOFF2 = Field<CR_Reg, 17, 1>; // DAC channel2 output buffer disable
        using EN2 = Field<CR_Reg, 16, 1>; // DAC channel2 enable
        using DMAUDRIE1 = Field<CR_Reg, 13, 1>; // DAC channel1 DMA Underrun Interrupt enable
        using DMAEN1 = Field<CR_Reg, 12, 1>; // DAC channel1 DMA enable
        using MAMP1 = Field<CR_Reg, 8, 4>; // DAC channel1 mask/amplitude selector
        using WAVE1 = Field<CR_Reg, 6, 2>; // DAC channel1 noise/triangle wave generation enable
        using TSEL1 = Field<CR_Reg, 3, 3>; // DAC channel1 trigger selection
        using TEN1 = Field<CR_Reg, 2, 1>; // DAC channel1 trigger enable
        using BOFF1 = Field<CR_Reg, 1, 1>; // DAC channel1 output buffer disable
        using EN1 = Field<CR_Reg, 0, 1>; // DAC channel1 enable
    };

    // software trigger register
    using SWTRIGR_Reg = Register<DAC_BASE + 0x4, WriteOnly>;
    struct _SWTRIGR {
        using SWTRIG2 = Field<SWTRIGR_Reg, 1, 1>; // DAC channel2 software trigger
        using SWTRIG1 = Field<SWTRIGR_Reg, 0, 1>; // DAC channel1 software trigger
    };

    // channel1 12-bit right-aligned data holding register
    using DHR12R1_Reg = Register<DAC_BASE + 0x8, ReadWrite>;
    struct _DHR12R1 {
        using DACC1DHR = Field<DHR12R1_Reg, 0, 12>; // DAC channel1 12-bit right-aligned data
    };

    // channel1 12-bit left aligned data holding register
    using DHR12L1_Reg = Register<DAC_BASE + 0xC, ReadWrite>;
    struct _DHR12L1 {
        using DACC1DHR = Field<DHR12L1_Reg, 4, 12>; // DAC channel1 12-bit left-aligned data
    };

    // channel1 8-bit right aligned data holding register
    using DHR8R1_Reg = Register<DAC_BASE + 0x10, ReadWrite>;
    struct _DHR8R1 {
        using DACC1DHR = Field<DHR8R1_Reg, 0, 8>; // DAC channel1 8-bit right-aligned data
    };

    // channel2 12-bit right aligned data holding register
    using DHR12R2_Reg = Register<DAC_BASE + 0x14, ReadWrite>;
    struct _DHR12R2 {
        using DACC2DHR = Field<DHR12R2_Reg, 0, 12>; // DAC channel2 12-bit right-aligned data
    };

    // channel2 12-bit left aligned data holding register
    using DHR12L2_Reg = Register<DAC_BASE + 0x18, ReadWrite>;
    struct _DHR12L2 {
        using DACC2DHR = Field<DHR12L2_Reg, 4, 12>; // DAC channel2 12-bit left-aligned data
    };

    // channel2 8-bit right-aligned data holding register
    using DHR8R2_Reg = Register<DAC_BASE + 0x1C, ReadWrite>;
    struct _DHR8R2 {
        using DACC2DHR = Field<DHR8R2_Reg, 0, 8>; // DAC channel2 8-bit right-aligned data
    };

    // Dual DAC 12-bit right-aligned data holding register
    using DHR12RD_Reg = Register<DAC_BASE + 0x20, ReadWrite>;
    struct _DHR12RD {
        using DACC2DHR = Field<DHR12RD_Reg, 16, 12>; // DAC channel2 12-bit right-aligned data
        using DACC1DHR = Field<DHR12RD_Reg, 0, 12>; // DAC channel1 12-bit right-aligned data
    };

    // DUAL DAC 12-bit left aligned data holding register
    using DHR12LD_Reg = Register<DAC_BASE + 0x24, ReadWrite>;
    struct _DHR12LD {
        using DACC2DHR = Field<DHR12LD_Reg, 20, 12>; // DAC channel2 12-bit left-aligned data
        using DACC1DHR = Field<DHR12LD_Reg, 4, 12>; // DAC channel1 12-bit left-aligned data
    };

    // DUAL DAC 8-bit right aligned data holding register
    using DHR8RD_Reg = Register<DAC_BASE + 0x28, ReadWrite>;
    struct _DHR8RD {
        using DACC2DHR = Field<DHR8RD_Reg, 8, 8>; // DAC channel2 8-bit right-aligned data
        using DACC1DHR = Field<DHR8RD_Reg, 0, 8>; // DAC channel1 8-bit right-aligned data
    };

    // channel1 data output register
    using DOR1_Reg = Register<DAC_BASE + 0x2C, ReadOnly>;
    struct _DOR1 {
        using DACC1DOR = Field<DOR1_Reg, 0, 12>; // DAC channel1 data output
    };

    // channel2 data output register
    using DOR2_Reg = Register<DAC_BASE + 0x30, ReadOnly>;
    struct _DOR2 {
        using DACC2DOR = Field<DOR2_Reg, 0, 12>; // DAC channel2 data output
    };

    // status register
    using SR_Reg = Register<DAC_BASE + 0x34, ReadWrite>;
    struct _SR {
        using DMAUDR2 = Field<SR_Reg, 29, 1>; // DAC channel2 DMA underrun flag
        using DMAUDR1 = Field<SR_Reg, 13, 1>; // DAC channel1 DMA underrun flag
    };

} // namespace DAC

// --------------------------------------------
// PWR: Power control
// Base address: 0x40007000
// --------------------------------------------

namespace PWR {
    static constexpr uint32_t PWR_BASE = 0x40007000;

    // power control register
    using CR1_Reg = Register<PWR_BASE + 0x0, ReadWrite>;
    struct _CR1 {
        using LPDS = Field<CR1_Reg, 0, 1>; // Low-power deep sleep
        using PDDS = Field<CR1_Reg, 1, 1>; // Power down deepsleep
        using CSBF = Field<CR1_Reg, 3, 1>; // Clear standby flag
        using PVDE = Field<CR1_Reg, 4, 1>; // Power voltage detector enable
        using PLS = Field<CR1_Reg, 5, 3>; // PVD level selection
        using DBP = Field<CR1_Reg, 8, 1>; // Disable backup domain write protection
        using FPDS = Field<CR1_Reg, 9, 1>; // Flash power down in Stop mode
        using LPUDS = Field<CR1_Reg, 10, 1>; // Low-power regulator in deepsleep under-drive mode
        using MRUDS = Field<CR1_Reg, 11, 1>; // Main regulator in deepsleep under-drive mode
        using ADCDC1 = Field<CR1_Reg, 13, 1>; // ADCDC1
        using VOS = Field<CR1_Reg, 14, 2>; // Regulator voltage scaling output selection
        using ODEN = Field<CR1_Reg, 16, 1>; // Over-drive enable
        using ODSWEN = Field<CR1_Reg, 17, 1>; // Over-drive switching enabled
        using UDEN = Field<CR1_Reg, 18, 2>; // Under-drive enable in stop mode
    };

    // power control/status register
    using CSR1_Reg = Register<PWR_BASE + 0x4, ReadWrite>;
    struct _CSR1 {
        using WUIF = Field<CSR1_Reg, 0, 1>; // Wakeup internal flag
        using SBF = Field<CSR1_Reg, 1, 1>; // Standby flag
        using PVDO = Field<CSR1_Reg, 2, 1>; // PVD output
        using BRR = Field<CSR1_Reg, 3, 1>; // Backup regulator ready
        using BRE = Field<CSR1_Reg, 9, 1>; // Backup regulator enable
        using VOSRDY = Field<CSR1_Reg, 14, 1>; // Regulator voltage scaling output selection ready bit
        using ODRDY = Field<CSR1_Reg, 16, 1>; // Over-drive mode ready
        using ODSWRDY = Field<CSR1_Reg, 17, 1>; // Over-drive mode switching ready
        using UDRDY = Field<CSR1_Reg, 18, 2>; // Under-drive ready flag
    };

    // power control register
    using CR2_Reg = Register<PWR_BASE + 0x8, ReadWrite>;
    struct _CR2 {
        using CWUPF1 = Field<CR2_Reg, 0, 1>; // Clear Wakeup Pin flag for PA0
        using CWUPF2 = Field<CR2_Reg, 1, 1>; // Clear Wakeup Pin flag for PA2
        using CWUPF3 = Field<CR2_Reg, 2, 1>; // Clear Wakeup Pin flag for PC1
        using CWUPF4 = Field<CR2_Reg, 3, 1>; // Clear Wakeup Pin flag for PC13
        using CWUPF5 = Field<CR2_Reg, 4, 1>; // Clear Wakeup Pin flag for PI8
        using CWUPF6 = Field<CR2_Reg, 5, 1>; // Clear Wakeup Pin flag for PI11
        using WUPP1 = Field<CR2_Reg, 8, 1>; // Wakeup pin polarity bit for PA0
        using WUPP2 = Field<CR2_Reg, 9, 1>; // Wakeup pin polarity bit for PA2
        using WUPP3 = Field<CR2_Reg, 10, 1>; // Wakeup pin polarity bit for PC1
        using WUPP4 = Field<CR2_Reg, 11, 1>; // Wakeup pin polarity bit for PC13
        using WUPP5 = Field<CR2_Reg, 12, 1>; // Wakeup pin polarity bit for PI8
        using WUPP6 = Field<CR2_Reg, 13, 1>; // Wakeup pin polarity bit for PI11
    };

    // power control/status register
    using CSR2_Reg = Register<PWR_BASE + 0xC, ReadWrite>;
    struct _CSR2 {
        using WUPF1 = Field<CSR2_Reg, 0, 1>; // Wakeup Pin flag for PA0
        using WUPF2 = Field<CSR2_Reg, 1, 1>; // Wakeup Pin flag for PA2
        using WUPF3 = Field<CSR2_Reg, 2, 1>; // Wakeup Pin flag for PC1
        using WUPF4 = Field<CSR2_Reg, 3, 1>; // Wakeup Pin flag for PC13
        using WUPF5 = Field<CSR2_Reg, 4, 1>; // Wakeup Pin flag for PI8
        using WUPF6 = Field<CSR2_Reg, 5, 1>; // Wakeup Pin flag for PI11
        using EWUP1 = Field<CSR2_Reg, 8, 1>; // Enable Wakeup pin for PA0
        using EWUP2 = Field<CSR2_Reg, 9, 1>; // Enable Wakeup pin for PA2
        using EWUP3 = Field<CSR2_Reg, 10, 1>; // Enable Wakeup pin for PC1
        using EWUP4 = Field<CSR2_Reg, 11, 1>; // Enable Wakeup pin for PC13
        using EWUP5 = Field<CSR2_Reg, 12, 1>; // Enable Wakeup pin for PI8
        using EWUP6 = Field<CSR2_Reg, 13, 1>; // Enable Wakeup pin for PI11
    };

} // namespace PWR

// --------------------------------------------
// IWDG: Independent watchdog
// Base address: 0x40003000
// --------------------------------------------

namespace IWDG {
    static constexpr uint32_t IWDG_BASE = 0x40003000;

    // Key register
    using KR_Reg = Register<IWDG_BASE + 0x0, WriteOnly>;
    struct _KR {
        using KEY = Field<KR_Reg, 0, 16>; // Key value (write only, read 0000h)
    };

    // Prescaler register
    using PR_Reg = Register<IWDG_BASE + 0x4, ReadWrite>;
    struct _PR {
        using PR = Field<PR_Reg, 0, 3>; // Prescaler divider
    };

    // Reload register
    using RLR_Reg = Register<IWDG_BASE + 0x8, ReadWrite>;
    struct _RLR {
        using RL = Field<RLR_Reg, 0, 12>; // Watchdog counter reload value
    };

    // Status register
    using SR_Reg = Register<IWDG_BASE + 0xC, ReadOnly>;
    struct _SR {
        using RVU = Field<SR_Reg, 1, 1>; // Watchdog counter reload value update
        using PVU = Field<SR_Reg, 0, 1>; // Watchdog prescaler value update
    };

    // Window register
    using WINR_Reg = Register<IWDG_BASE + 0x10, ReadWrite>;
    struct _WINR {
        using WIN = Field<WINR_Reg, 0, 12>; // Watchdog counter window value
    };

} // namespace IWDG

// --------------------------------------------
// WWDG: Window watchdog
// Base address: 0x40002C00
// --------------------------------------------

namespace WWDG {
    static constexpr uint32_t WWDG_BASE = 0x40002C00;

    // Control register
    using CR_Reg = Register<WWDG_BASE + 0x0, ReadWrite>;
    struct _CR {
        using WDGA = Field<CR_Reg, 7, 1>; // Activation bit
        using T = Field<CR_Reg, 0, 7>; // 7-bit counter (MSB to LSB)
    };

    // Configuration register
    using CFR_Reg = Register<WWDG_BASE + 0x4, ReadWrite>;
    struct _CFR {
        using EWI = Field<CFR_Reg, 9, 1>; // Early wakeup interrupt
        using WDGTB1 = Field<CFR_Reg, 8, 1>; // Timer base
        using WDGTB0 = Field<CFR_Reg, 7, 1>; // Timer base
        using W = Field<CFR_Reg, 0, 7>; // 7-bit window value
    };

    // Status register
    using SR_Reg = Register<WWDG_BASE + 0x8, ReadWrite>;
    struct _SR {
        using EWIF = Field<SR_Reg, 0, 1>; // Early wakeup interrupt flag
    };

} // namespace WWDG

// --------------------------------------------
// TIM1: Advanced-timers
// Base address: 0x40010000
// --------------------------------------------

namespace TIM1 {
    static constexpr uint32_t TIM1_BASE = 0x40010000;

    // control register 1
    using CR1_Reg = Register<TIM1_BASE + 0x0, ReadWrite>;
    struct _CR1 {
        using CKD = Field<CR1_Reg, 8, 2>; // Clock division
        using ARPE = Field<CR1_Reg, 7, 1>; // Auto-reload preload enable
        using CMS = Field<CR1_Reg, 5, 2>; // Center-aligned mode selection
        using DIR = Field<CR1_Reg, 4, 1>; // Direction
        using OPM = Field<CR1_Reg, 3, 1>; // One-pulse mode
        using URS = Field<CR1_Reg, 2, 1>; // Update request source
        using UDIS = Field<CR1_Reg, 1, 1>; // Update disable
        using CEN = Field<CR1_Reg, 0, 1>; // Counter enable
    };

    // control register 2
    using CR2_Reg = Register<TIM1_BASE + 0x4, ReadWrite>;
    struct _CR2 {
        using MMS2 = Field<CR2_Reg, 20, 4>; // Master mode selection 2
        using OIS6 = Field<CR2_Reg, 18, 1>; // Output Idle state 6
        using OIS5 = Field<CR2_Reg, 16, 1>; // Output Idle state 5
        using OIS4 = Field<CR2_Reg, 14, 1>; // Output Idle state 4
        using OIS3N = Field<CR2_Reg, 13, 1>; // Output Idle state 3
        using OIS3 = Field<CR2_Reg, 12, 1>; // Output Idle state 3
        using OIS2N = Field<CR2_Reg, 11, 1>; // Output Idle state 2
        using OIS2 = Field<CR2_Reg, 10, 1>; // Output Idle state 2
        using OIS1N = Field<CR2_Reg, 9, 1>; // Output Idle state 1
        using OIS1 = Field<CR2_Reg, 8, 1>; // Output Idle state 1
        using TI1S = Field<CR2_Reg, 7, 1>; // TI1 selection
        using MMS = Field<CR2_Reg, 4, 3>; // Master mode selection
        using CCDS = Field<CR2_Reg, 3, 1>; // Capture/compare DMA selection
        using CCUS = Field<CR2_Reg, 2, 1>; // Capture/compare control update selection
        using CCPC = Field<CR2_Reg, 0, 1>; // Capture/compare preloaded control
    };

    // slave mode control register
    using SMCR_Reg = Register<TIM1_BASE + 0x8, ReadWrite>;
    struct _SMCR {
        using ETP = Field<SMCR_Reg, 15, 1>; // External trigger polarity
        using ECE = Field<SMCR_Reg, 14, 1>; // External clock enable
        using ETPS = Field<SMCR_Reg, 12, 2>; // External trigger prescaler
        using ETF = Field<SMCR_Reg, 8, 4>; // External trigger filter
        using MSM = Field<SMCR_Reg, 7, 1>; // Master/Slave mode
        using TS = Field<SMCR_Reg, 4, 3>; // Trigger selection
        using SMS = Field<SMCR_Reg, 0, 3>; // Slave mode selection
    };

    // DMA/Interrupt enable register
    using DIER_Reg = Register<TIM1_BASE + 0xC, ReadWrite>;
    struct _DIER {
        using TDE = Field<DIER_Reg, 14, 1>; // Trigger DMA request enable
        using COMDE = Field<DIER_Reg, 13, 1>; // COM DMA request enable
        using CC4DE = Field<DIER_Reg, 12, 1>; // Capture/Compare 4 DMA request enable
        using CC3DE = Field<DIER_Reg, 11, 1>; // Capture/Compare 3 DMA request enable
        using CC2DE = Field<DIER_Reg, 10, 1>; // Capture/Compare 2 DMA request enable
        using CC1DE = Field<DIER_Reg, 9, 1>; // Capture/Compare 1 DMA request enable
        using UDE = Field<DIER_Reg, 8, 1>; // Update DMA request enable
        using TIE = Field<DIER_Reg, 6, 1>; // Trigger interrupt enable
        using CC4IE = Field<DIER_Reg, 4, 1>; // Capture/Compare 4 interrupt enable
        using CC3IE = Field<DIER_Reg, 3, 1>; // Capture/Compare 3 interrupt enable
        using CC2IE = Field<DIER_Reg, 2, 1>; // Capture/Compare 2 interrupt enable
        using CC1IE = Field<DIER_Reg, 1, 1>; // Capture/Compare 1 interrupt enable
        using UIE = Field<DIER_Reg, 0, 1>; // Update interrupt enable
        using BIE = Field<DIER_Reg, 7, 1>; // Break interrupt enable
        using COMIE = Field<DIER_Reg, 5, 1>; // COM interrupt enable
    };

    // status register
    using SR_Reg = Register<TIM1_BASE + 0x10, ReadWrite>;
    struct _SR {
        using CC4OF = Field<SR_Reg, 12, 1>; // Capture/Compare 4 overcapture flag
        using CC3OF = Field<SR_Reg, 11, 1>; // Capture/Compare 3 overcapture flag
        using CC2OF = Field<SR_Reg, 10, 1>; // Capture/compare 2 overcapture flag
        using CC1OF = Field<SR_Reg, 9, 1>; // Capture/Compare 1 overcapture flag
        using BIF = Field<SR_Reg, 7, 1>; // Break interrupt flag
        using TIF = Field<SR_Reg, 6, 1>; // Trigger interrupt flag
        using COMIF = Field<SR_Reg, 5, 1>; // COM interrupt flag
        using CC4IF = Field<SR_Reg, 4, 1>; // Capture/Compare 4 interrupt flag
        using CC3IF = Field<SR_Reg, 3, 1>; // Capture/Compare 3 interrupt flag
        using CC2IF = Field<SR_Reg, 2, 1>; // Capture/Compare 2 interrupt flag
        using CC1IF = Field<SR_Reg, 1, 1>; // Capture/compare 1 interrupt flag
        using UIF = Field<SR_Reg, 0, 1>; // Update interrupt flag
    };

    // event generation register
    using EGR_Reg = Register<TIM1_BASE + 0x14, WriteOnly>;
    struct _EGR {
        using BG = Field<EGR_Reg, 7, 1>; // Break generation
        using TG = Field<EGR_Reg, 6, 1>; // Trigger generation
        using COMG = Field<EGR_Reg, 5, 1>; // Capture/Compare control update generation
        using CC4G = Field<EGR_Reg, 4, 1>; // Capture/compare 4 generation
        using CC3G = Field<EGR_Reg, 3, 1>; // Capture/compare 3 generation
        using CC2G = Field<EGR_Reg, 2, 1>; // Capture/compare 2 generation
        using CC1G = Field<EGR_Reg, 1, 1>; // Capture/compare 1 generation
        using UG = Field<EGR_Reg, 0, 1>; // Update generation
    };

    // capture/compare mode register 1 (output mode)
    using CCMR1_Output_Reg = Register<TIM1_BASE + 0x18, ReadWrite>;
    struct _CCMR1_Output {
        using OC2CE = Field<CCMR1_Output_Reg, 15, 1>; // Output Compare 2 clear enable
        using OC2M = Field<CCMR1_Output_Reg, 12, 3>; // Output Compare 2 mode
        using OC2PE = Field<CCMR1_Output_Reg, 11, 1>; // Output Compare 2 preload enable
        using OC2FE = Field<CCMR1_Output_Reg, 10, 1>; // Output Compare 2 fast enable
        using CC2S = Field<CCMR1_Output_Reg, 8, 2>; // Capture/Compare 2 selection
        using OC1CE = Field<CCMR1_Output_Reg, 7, 1>; // Output Compare 1 clear enable
        using OC1M = Field<CCMR1_Output_Reg, 4, 3>; // Output Compare 1 mode
        using OC1PE = Field<CCMR1_Output_Reg, 3, 1>; // Output Compare 1 preload enable
        using OC1FE = Field<CCMR1_Output_Reg, 2, 1>; // Output Compare 1 fast enable
        using CC1S = Field<CCMR1_Output_Reg, 0, 2>; // Capture/Compare 1 selection
    };

    // capture/compare mode register 1 (input mode)
    using CCMR1_Input_Reg = Register<TIM1_BASE + 0x18, ReadWrite>;
    struct _CCMR1_Input {
        using IC2F = Field<CCMR1_Input_Reg, 12, 4>; // Input capture 2 filter
        using IC2PSC = Field<CCMR1_Input_Reg, 10, 2>; // Input capture 2 prescaler
        using CC2S = Field<CCMR1_Input_Reg, 8, 2>; // Capture/Compare 2 selection
        using IC1F = Field<CCMR1_Input_Reg, 4, 4>; // Input capture 1 filter
        using IC1PSC = Field<CCMR1_Input_Reg, 2, 2>; // Input capture 1 prescaler
        using CC1S = Field<CCMR1_Input_Reg, 0, 2>; // Capture/Compare 1 selection
    };

    // capture/compare mode register 2 (output mode)
    using CCMR2_Output_Reg = Register<TIM1_BASE + 0x1C, ReadWrite>;
    struct _CCMR2_Output {
        using OC4CE = Field<CCMR2_Output_Reg, 15, 1>; // Output compare 4 clear enable
        using OC4M = Field<CCMR2_Output_Reg, 12, 3>; // Output compare 4 mode
        using OC4PE = Field<CCMR2_Output_Reg, 11, 1>; // Output compare 4 preload enable
        using OC4FE = Field<CCMR2_Output_Reg, 10, 1>; // Output compare 4 fast enable
        using CC4S = Field<CCMR2_Output_Reg, 8, 2>; // Capture/Compare 4 selection
        using OC3CE = Field<CCMR2_Output_Reg, 7, 1>; // Output compare 3 clear enable
        using OC3M = Field<CCMR2_Output_Reg, 4, 3>; // Output compare 3 mode
        using OC3PE = Field<CCMR2_Output_Reg, 3, 1>; // Output compare 3 preload enable
        using OC3FE = Field<CCMR2_Output_Reg, 2, 1>; // Output compare 3 fast enable
        using CC3S = Field<CCMR2_Output_Reg, 0, 2>; // Capture/Compare 3 selection
    };

    // capture/compare mode register 2 (input mode)
    using CCMR2_Input_Reg = Register<TIM1_BASE + 0x1C, ReadWrite>;
    struct _CCMR2_Input {
        using IC4F = Field<CCMR2_Input_Reg, 12, 4>; // Input capture 4 filter
        using IC4PSC = Field<CCMR2_Input_Reg, 10, 2>; // Input capture 4 prescaler
        using CC4S = Field<CCMR2_Input_Reg, 8, 2>; // Capture/Compare 4 selection
        using IC3F = Field<CCMR2_Input_Reg, 4, 4>; // Input capture 3 filter
        using IC3PSC = Field<CCMR2_Input_Reg, 2, 2>; // Input capture 3 prescaler
        using CC3S = Field<CCMR2_Input_Reg, 0, 2>; // Capture/compare 3 selection
    };

    // capture/compare enable register
    using CCER_Reg = Register<TIM1_BASE + 0x20, ReadWrite>;
    struct _CCER {
        using CC4P = Field<CCER_Reg, 13, 1>; // Capture/Compare 3 output Polarity
        using CC4E = Field<CCER_Reg, 12, 1>; // Capture/Compare 4 output enable
        using CC3NP = Field<CCER_Reg, 11, 1>; // Capture/Compare 3 output Polarity
        using CC3NE = Field<CCER_Reg, 10, 1>; // Capture/Compare 3 complementary output enable
        using CC3P = Field<CCER_Reg, 9, 1>; // Capture/Compare 3 output Polarity
        using CC3E = Field<CCER_Reg, 8, 1>; // Capture/Compare 3 output enable
        using CC2NP = Field<CCER_Reg, 7, 1>; // Capture/Compare 2 output Polarity
        using CC2NE = Field<CCER_Reg, 6, 1>; // Capture/Compare 2 complementary output enable
        using CC2P = Field<CCER_Reg, 5, 1>; // Capture/Compare 2 output Polarity
        using CC2E = Field<CCER_Reg, 4, 1>; // Capture/Compare 2 output enable
        using CC1NP = Field<CCER_Reg, 3, 1>; // Capture/Compare 1 output Polarity
        using CC1NE = Field<CCER_Reg, 2, 1>; // Capture/Compare 1 complementary output enable
        using CC1P = Field<CCER_Reg, 1, 1>; // Capture/Compare 1 output Polarity
        using CC1E = Field<CCER_Reg, 0, 1>; // Capture/Compare 1 output enable
    };

    // counter
    using CNT_Reg = Register<TIM1_BASE + 0x24, ReadWrite>;
    struct _CNT {
        using CNT = Field<CNT_Reg, 0, 16>; // counter value
    };

    // prescaler
    using PSC_Reg = Register<TIM1_BASE + 0x28, ReadWrite>;
    struct _PSC {
        using PSC = Field<PSC_Reg, 0, 16>; // Prescaler value
    };

    // auto-reload register
    using ARR_Reg = Register<TIM1_BASE + 0x2C, ReadWrite>;
    struct _ARR {
        using ARR = Field<ARR_Reg, 0, 16>; // Auto-reload value
    };

    // capture/compare register 1
    using CCR1_Reg = Register<TIM1_BASE + 0x34, ReadWrite>;
    struct _CCR1 {
        using CCR1 = Field<CCR1_Reg, 0, 16>; // Capture/Compare 1 value
    };

    // capture/compare register 2
    using CCR2_Reg = Register<TIM1_BASE + 0x38, ReadWrite>;
    struct _CCR2 {
        using CCR2 = Field<CCR2_Reg, 0, 16>; // Capture/Compare 2 value
    };

    // capture/compare register 3
    using CCR3_Reg = Register<TIM1_BASE + 0x3C, ReadWrite>;
    struct _CCR3 {
        using CCR3 = Field<CCR3_Reg, 0, 16>; // Capture/Compare value
    };

    // capture/compare register 4
    using CCR4_Reg = Register<TIM1_BASE + 0x40, ReadWrite>;
    struct _CCR4 {
        using CCR4 = Field<CCR4_Reg, 0, 16>; // Capture/Compare value
    };

    // DMA control register
    using DCR_Reg = Register<TIM1_BASE + 0x48, ReadWrite>;
    struct _DCR {
        using DBL = Field<DCR_Reg, 8, 5>; // DMA burst length
        using DBA = Field<DCR_Reg, 0, 5>; // DMA base address
    };

    // DMA address for full transfer
    using DMAR_Reg = Register<TIM1_BASE + 0x4C, ReadWrite>;
    struct _DMAR {
        using DMAB = Field<DMAR_Reg, 0, 16>; // DMA register for burst accesses
    };

    // repetition counter register
    using RCR_Reg = Register<TIM1_BASE + 0x30, ReadWrite>;
    struct _RCR {
        using REP = Field<RCR_Reg, 0, 8>; // Repetition counter value
    };

    // break and dead-time register
    using BDTR_Reg = Register<TIM1_BASE + 0x44, ReadWrite>;
    struct _BDTR {
        using DTG = Field<BDTR_Reg, 0, 8>; // Dead-time generator setup
        using LOCK = Field<BDTR_Reg, 8, 2>; // Lock configuration
        using OSSI = Field<BDTR_Reg, 10, 1>; // Off-state selection for Idle mode
        using OSSR = Field<BDTR_Reg, 11, 1>; // Off-state selection for Run mode
        using BKE = Field<BDTR_Reg, 12, 1>; // Break enable
        using BKP = Field<BDTR_Reg, 13, 1>; // Break polarity
        using AOE = Field<BDTR_Reg, 14, 1>; // Automatic output enable
        using MOE = Field<BDTR_Reg, 15, 1>; // Main output enable
        using BKF = Field<BDTR_Reg, 16, 4>; // Break filter
        using BK2F = Field<BDTR_Reg, 20, 4>; // Break 2 filter
        using BK2E = Field<BDTR_Reg, 24, 1>; // Break 2 enable
        using BK2P = Field<BDTR_Reg, 25, 1>; // Break 2 polarity
    };

    // capture/compare mode register 3 (output mode)
    using CCMR3_Output_Reg = Register<TIM1_BASE + 0x54, ReadWrite>;
    struct _CCMR3_Output {
        using OC5FE = Field<CCMR3_Output_Reg, 2, 1>; // Output compare 5 fast enable
        using OC5PE = Field<CCMR3_Output_Reg, 3, 1>; // Output compare 5 preload enable
        using OC5M = Field<CCMR3_Output_Reg, 4, 3>; // Output compare 5 mode
        using OC5CE = Field<CCMR3_Output_Reg, 7, 1>; // Output compare 5 clear enable
        using OC6FE = Field<CCMR3_Output_Reg, 10, 1>; // Output compare 6 fast enable
        using OC6PE = Field<CCMR3_Output_Reg, 11, 1>; // Output compare 6 preload enable
        using OC6M = Field<CCMR3_Output_Reg, 12, 3>; // Output compare 6 mode
        using OC6CE = Field<CCMR3_Output_Reg, 15, 1>; // Output compare 6 clear enable
        using OC5M3 = Field<CCMR3_Output_Reg, 16, 1>; // Output Compare 5 mode
        using OC6M3 = Field<CCMR3_Output_Reg, 24, 1>; // Output Compare 6 mode
    };

    // capture/compare register 5
    using CCR5_Reg = Register<TIM1_BASE + 0x58, ReadWrite>;
    struct _CCR5 {
        using CCR5 = Field<CCR5_Reg, 0, 16>; // Capture/Compare 5 value
        using GC5C1 = Field<CCR5_Reg, 29, 1>; // Group Channel 5 and Channel 1
        using GC5C2 = Field<CCR5_Reg, 30, 1>; // Group Channel 5 and Channel 2
        using GC5C3 = Field<CCR5_Reg, 31, 1>; // Group Channel 5 and Channel 3
    };

    // capture/compare register 6
    using CRR6_Reg = Register<TIM1_BASE + 0x5C, ReadWrite>;
    struct _CRR6 {
        using CCR6 = Field<CRR6_Reg, 0, 16>; // Capture/Compare 6 value
    };

    // alternate function option register 1
    using AF1_Reg = Register<TIM1_BASE + 0x60, ReadWrite>;
    struct _AF1 {
        using BKINE = Field<AF1_Reg, 0, 1>; // BRK BKIN input enable
        using BKDFBKE = Field<AF1_Reg, 8, 1>; // BRK DFSDM_BREAK[0] enable
        using BKINP = Field<AF1_Reg, 9, 1>; // BRK BKIN input polarity
    };

    // alternate function option register 2
    using AF2_Reg = Register<TIM1_BASE + 0x64, ReadWrite>;
    struct _AF2 {
        using BK2INE = Field<AF2_Reg, 0, 1>; // BRK2 BKIN input enable
        using BK2DFBKE = Field<AF2_Reg, 8, 1>; // BRK2 DFSDM_BREAK enable
        using BK2INP = Field<AF2_Reg, 9, 1>; // BRK2 BKIN2 input polarity
    };

} // namespace TIM1

// --------------------------------------------
// TIM8: 
// Base address: 0x40010400
// --------------------------------------------

namespace TIM8 {
    static constexpr uint32_t TIM8_BASE = 0x40010400;

} // namespace TIM8

// --------------------------------------------
// TIM2: General purpose timers
// Base address: 0x40000000
// --------------------------------------------

namespace TIM2 {
    static constexpr uint32_t TIM2_BASE = 0x40000000;

    // control register 1
    using CR1_Reg = Register<TIM2_BASE + 0x0, ReadWrite>;
    struct _CR1 {
        using CKD = Field<CR1_Reg, 8, 2>; // Clock division
        using ARPE = Field<CR1_Reg, 7, 1>; // Auto-reload preload enable
        using CMS = Field<CR1_Reg, 5, 2>; // Center-aligned mode selection
        using DIR = Field<CR1_Reg, 4, 1>; // Direction
        using OPM = Field<CR1_Reg, 3, 1>; // One-pulse mode
        using URS = Field<CR1_Reg, 2, 1>; // Update request source
        using UDIS = Field<CR1_Reg, 1, 1>; // Update disable
        using CEN = Field<CR1_Reg, 0, 1>; // Counter enable
    };

    // control register 2
    using CR2_Reg = Register<TIM2_BASE + 0x4, ReadWrite>;
    struct _CR2 {
        using TI1S = Field<CR2_Reg, 7, 1>; // TI1 selection
        using MMS = Field<CR2_Reg, 4, 3>; // Master mode selection
        using CCDS = Field<CR2_Reg, 3, 1>; // Capture/compare DMA selection
    };

    // slave mode control register
    using SMCR_Reg = Register<TIM2_BASE + 0x8, ReadWrite>;
    struct _SMCR {
        using ETP = Field<SMCR_Reg, 15, 1>; // External trigger polarity
        using ECE = Field<SMCR_Reg, 14, 1>; // External clock enable
        using ETPS = Field<SMCR_Reg, 12, 2>; // External trigger prescaler
        using ETF = Field<SMCR_Reg, 8, 4>; // External trigger filter
        using MSM = Field<SMCR_Reg, 7, 1>; // Master/Slave mode
        using TS = Field<SMCR_Reg, 4, 3>; // Trigger selection
        using SMS = Field<SMCR_Reg, 0, 3>; // Slave mode selection
    };

    // DMA/Interrupt enable register
    using DIER_Reg = Register<TIM2_BASE + 0xC, ReadWrite>;
    struct _DIER {
        using TDE = Field<DIER_Reg, 14, 1>; // Trigger DMA request enable
        using CC4DE = Field<DIER_Reg, 12, 1>; // Capture/Compare 4 DMA request enable
        using CC3DE = Field<DIER_Reg, 11, 1>; // Capture/Compare 3 DMA request enable
        using CC2DE = Field<DIER_Reg, 10, 1>; // Capture/Compare 2 DMA request enable
        using CC1DE = Field<DIER_Reg, 9, 1>; // Capture/Compare 1 DMA request enable
        using UDE = Field<DIER_Reg, 8, 1>; // Update DMA request enable
        using TIE = Field<DIER_Reg, 6, 1>; // Trigger interrupt enable
        using CC4IE = Field<DIER_Reg, 4, 1>; // Capture/Compare 4 interrupt enable
        using CC3IE = Field<DIER_Reg, 3, 1>; // Capture/Compare 3 interrupt enable
        using CC2IE = Field<DIER_Reg, 2, 1>; // Capture/Compare 2 interrupt enable
        using CC1IE = Field<DIER_Reg, 1, 1>; // Capture/Compare 1 interrupt enable
        using UIE = Field<DIER_Reg, 0, 1>; // Update interrupt enable
    };

    // status register
    using SR_Reg = Register<TIM2_BASE + 0x10, ReadWrite>;
    struct _SR {
        using CC4OF = Field<SR_Reg, 12, 1>; // Capture/Compare 4 overcapture flag
        using CC3OF = Field<SR_Reg, 11, 1>; // Capture/Compare 3 overcapture flag
        using CC2OF = Field<SR_Reg, 10, 1>; // Capture/compare 2 overcapture flag
        using CC1OF = Field<SR_Reg, 9, 1>; // Capture/Compare 1 overcapture flag
        using TIF = Field<SR_Reg, 6, 1>; // Trigger interrupt flag
        using CC4IF = Field<SR_Reg, 4, 1>; // Capture/Compare 4 interrupt flag
        using CC3IF = Field<SR_Reg, 3, 1>; // Capture/Compare 3 interrupt flag
        using CC2IF = Field<SR_Reg, 2, 1>; // Capture/Compare 2 interrupt flag
        using CC1IF = Field<SR_Reg, 1, 1>; // Capture/compare 1 interrupt flag
        using UIF = Field<SR_Reg, 0, 1>; // Update interrupt flag
    };

    // event generation register
    using EGR_Reg = Register<TIM2_BASE + 0x14, WriteOnly>;
    struct _EGR {
        using TG = Field<EGR_Reg, 6, 1>; // Trigger generation
        using CC4G = Field<EGR_Reg, 4, 1>; // Capture/compare 4 generation
        using CC3G = Field<EGR_Reg, 3, 1>; // Capture/compare 3 generation
        using CC2G = Field<EGR_Reg, 2, 1>; // Capture/compare 2 generation
        using CC1G = Field<EGR_Reg, 1, 1>; // Capture/compare 1 generation
        using UG = Field<EGR_Reg, 0, 1>; // Update generation
    };

    // capture/compare mode register 1 (output mode)
    using CCMR1_Output_Reg = Register<TIM2_BASE + 0x18, ReadWrite>;
    struct _CCMR1_Output {
        using OC2CE = Field<CCMR1_Output_Reg, 15, 1>; // OC2CE
        using OC2M = Field<CCMR1_Output_Reg, 12, 3>; // OC2M
        using OC2PE = Field<CCMR1_Output_Reg, 11, 1>; // OC2PE
        using OC2FE = Field<CCMR1_Output_Reg, 10, 1>; // OC2FE
        using CC2S = Field<CCMR1_Output_Reg, 8, 2>; // CC2S
        using OC1CE = Field<CCMR1_Output_Reg, 7, 1>; // OC1CE
        using OC1M = Field<CCMR1_Output_Reg, 4, 3>; // OC1M
        using OC1PE = Field<CCMR1_Output_Reg, 3, 1>; // OC1PE
        using OC1FE = Field<CCMR1_Output_Reg, 2, 1>; // OC1FE
        using CC1S = Field<CCMR1_Output_Reg, 0, 2>; // CC1S
    };

    // capture/compare mode register 1 (input mode)
    using CCMR1_Input_Reg = Register<TIM2_BASE + 0x18, ReadWrite>;
    struct _CCMR1_Input {
        using IC2F = Field<CCMR1_Input_Reg, 12, 4>; // Input capture 2 filter
        using IC2PSC = Field<CCMR1_Input_Reg, 10, 2>; // Input capture 2 prescaler
        using CC2S = Field<CCMR1_Input_Reg, 8, 2>; // Capture/Compare 2 selection
        using IC1F = Field<CCMR1_Input_Reg, 4, 4>; // Input capture 1 filter
        using IC1PSC = Field<CCMR1_Input_Reg, 2, 2>; // Input capture 1 prescaler
        using CC1S = Field<CCMR1_Input_Reg, 0, 2>; // Capture/Compare 1 selection
    };

    // capture/compare mode register 2 (output mode)
    using CCMR2_Output_Reg = Register<TIM2_BASE + 0x1C, ReadWrite>;
    struct _CCMR2_Output {
        using O24CE = Field<CCMR2_Output_Reg, 15, 1>; // O24CE
        using OC4M = Field<CCMR2_Output_Reg, 12, 3>; // OC4M
        using OC4PE = Field<CCMR2_Output_Reg, 11, 1>; // OC4PE
        using OC4FE = Field<CCMR2_Output_Reg, 10, 1>; // OC4FE
        using CC4S = Field<CCMR2_Output_Reg, 8, 2>; // CC4S
        using OC3CE = Field<CCMR2_Output_Reg, 7, 1>; // OC3CE
        using OC3M = Field<CCMR2_Output_Reg, 4, 3>; // OC3M
        using OC3PE = Field<CCMR2_Output_Reg, 3, 1>; // OC3PE
        using OC3FE = Field<CCMR2_Output_Reg, 2, 1>; // OC3FE
        using CC3S = Field<CCMR2_Output_Reg, 0, 2>; // CC3S
    };

    // capture/compare mode register 2 (input mode)
    using CCMR2_Input_Reg = Register<TIM2_BASE + 0x1C, ReadWrite>;
    struct _CCMR2_Input {
        using IC4F = Field<CCMR2_Input_Reg, 12, 4>; // Input capture 4 filter
        using IC4PSC = Field<CCMR2_Input_Reg, 10, 2>; // Input capture 4 prescaler
        using CC4S = Field<CCMR2_Input_Reg, 8, 2>; // Capture/Compare 4 selection
        using IC3F = Field<CCMR2_Input_Reg, 4, 4>; // Input capture 3 filter
        using IC3PSC = Field<CCMR2_Input_Reg, 2, 2>; // Input capture 3 prescaler
        using CC3S = Field<CCMR2_Input_Reg, 0, 2>; // Capture/compare 3 selection
    };

    // capture/compare enable register
    using CCER_Reg = Register<TIM2_BASE + 0x20, ReadWrite>;
    struct _CCER {
        using CC4NP = Field<CCER_Reg, 15, 1>; // Capture/Compare 4 output Polarity
        using CC4P = Field<CCER_Reg, 13, 1>; // Capture/Compare 3 output Polarity
        using CC4E = Field<CCER_Reg, 12, 1>; // Capture/Compare 4 output enable
        using CC3NP = Field<CCER_Reg, 11, 1>; // Capture/Compare 3 output Polarity
        using CC3P = Field<CCER_Reg, 9, 1>; // Capture/Compare 3 output Polarity
        using CC3E = Field<CCER_Reg, 8, 1>; // Capture/Compare 3 output enable
        using CC2NP = Field<CCER_Reg, 7, 1>; // Capture/Compare 2 output Polarity
        using CC2P = Field<CCER_Reg, 5, 1>; // Capture/Compare 2 output Polarity
        using CC2E = Field<CCER_Reg, 4, 1>; // Capture/Compare 2 output enable
        using CC1NP = Field<CCER_Reg, 3, 1>; // Capture/Compare 1 output Polarity
        using CC1P = Field<CCER_Reg, 1, 1>; // Capture/Compare 1 output Polarity
        using CC1E = Field<CCER_Reg, 0, 1>; // Capture/Compare 1 output enable
    };

    // counter
    using CNT_Reg = Register<TIM2_BASE + 0x24, ReadWrite>;
    struct _CNT {
        using CNT_H = Field<CNT_Reg, 16, 16>; // High counter value
        using CNT_L = Field<CNT_Reg, 0, 16>; // Low counter value
    };

    // prescaler
    using PSC_Reg = Register<TIM2_BASE + 0x28, ReadWrite>;
    struct _PSC {
        using PSC = Field<PSC_Reg, 0, 16>; // Prescaler value
    };

    // auto-reload register
    using ARR_Reg = Register<TIM2_BASE + 0x2C, ReadWrite>;
    struct _ARR {
        using ARR_H = Field<ARR_Reg, 16, 16>; // High Auto-reload value
        using ARR_L = Field<ARR_Reg, 0, 16>; // Low Auto-reload value
    };

    // capture/compare register 1
    using CCR1_Reg = Register<TIM2_BASE + 0x34, ReadWrite>;
    struct _CCR1 {
        using CCR1_H = Field<CCR1_Reg, 16, 16>; // High Capture/Compare 1 value
        using CCR1_L = Field<CCR1_Reg, 0, 16>; // Low Capture/Compare 1 value
    };

    // capture/compare register 2
    using CCR2_Reg = Register<TIM2_BASE + 0x38, ReadWrite>;
    struct _CCR2 {
        using CCR2_H = Field<CCR2_Reg, 16, 16>; // High Capture/Compare 2 value
        using CCR2_L = Field<CCR2_Reg, 0, 16>; // Low Capture/Compare 2 value
    };

    // capture/compare register 3
    using CCR3_Reg = Register<TIM2_BASE + 0x3C, ReadWrite>;
    struct _CCR3 {
        using CCR3_H = Field<CCR3_Reg, 16, 16>; // High Capture/Compare value
        using CCR3_L = Field<CCR3_Reg, 0, 16>; // Low Capture/Compare value
    };

    // capture/compare register 4
    using CCR4_Reg = Register<TIM2_BASE + 0x40, ReadWrite>;
    struct _CCR4 {
        using CCR4_H = Field<CCR4_Reg, 16, 16>; // High Capture/Compare value
        using CCR4_L = Field<CCR4_Reg, 0, 16>; // Low Capture/Compare value
    };

    // DMA control register
    using DCR_Reg = Register<TIM2_BASE + 0x48, ReadWrite>;
    struct _DCR {
        using DBL = Field<DCR_Reg, 8, 5>; // DMA burst length
        using DBA = Field<DCR_Reg, 0, 5>; // DMA base address
    };

    // DMA address for full transfer
    using DMAR_Reg = Register<TIM2_BASE + 0x4C, ReadWrite>;
    struct _DMAR {
        using DMAB = Field<DMAR_Reg, 0, 16>; // DMA register for burst accesses
    };

    // TIM2 option register 1
    using OR1_Reg = Register<TIM2_BASE + 0x50, ReadWrite>;
    struct _OR1 {
        using TI4_RMP = Field<OR1_Reg, 2, 2>; // Input Capture 4 remap
        using ETR1_RMP = Field<OR1_Reg, 1, 1>; // External trigger remap
        using ITR1_RMP = Field<OR1_Reg, 0, 1>; // Internal trigger 1 remap
    };

    // TIM2 option register 2
    using OR2_Reg = Register<TIM2_BASE + 0x60, ReadWrite>;
    struct _OR2 {
        using ETRSEL = Field<OR2_Reg, 14, 3>; // ETR source selection
    };

} // namespace TIM2

// --------------------------------------------
// TIM3: General purpose timers
// Base address: 0x40000400
// --------------------------------------------

namespace TIM3 {
    static constexpr uint32_t TIM3_BASE = 0x40000400;

    // control register 1
    using CR1_Reg = Register<TIM3_BASE + 0x0, ReadWrite>;
    struct _CR1 {
        using CKD = Field<CR1_Reg, 8, 2>; // Clock division
        using ARPE = Field<CR1_Reg, 7, 1>; // Auto-reload preload enable
        using CMS = Field<CR1_Reg, 5, 2>; // Center-aligned mode selection
        using DIR = Field<CR1_Reg, 4, 1>; // Direction
        using OPM = Field<CR1_Reg, 3, 1>; // One-pulse mode
        using URS = Field<CR1_Reg, 2, 1>; // Update request source
        using UDIS = Field<CR1_Reg, 1, 1>; // Update disable
        using CEN = Field<CR1_Reg, 0, 1>; // Counter enable
    };

    // control register 2
    using CR2_Reg = Register<TIM3_BASE + 0x4, ReadWrite>;
    struct _CR2 {
        using TI1S = Field<CR2_Reg, 7, 1>; // TI1 selection
        using MMS = Field<CR2_Reg, 4, 3>; // Master mode selection
        using CCDS = Field<CR2_Reg, 3, 1>; // Capture/compare DMA selection
    };

    // slave mode control register
    using SMCR_Reg = Register<TIM3_BASE + 0x8, ReadWrite>;
    struct _SMCR {
        using ETP = Field<SMCR_Reg, 15, 1>; // External trigger polarity
        using ECE = Field<SMCR_Reg, 14, 1>; // External clock enable
        using ETPS = Field<SMCR_Reg, 12, 2>; // External trigger prescaler
        using ETF = Field<SMCR_Reg, 8, 4>; // External trigger filter
        using MSM = Field<SMCR_Reg, 7, 1>; // Master/Slave mode
        using TS = Field<SMCR_Reg, 4, 3>; // Trigger selection
        using SMS = Field<SMCR_Reg, 0, 3>; // Slave mode selection
    };

    // DMA/Interrupt enable register
    using DIER_Reg = Register<TIM3_BASE + 0xC, ReadWrite>;
    struct _DIER {
        using TDE = Field<DIER_Reg, 14, 1>; // Trigger DMA request enable
        using CC4DE = Field<DIER_Reg, 12, 1>; // Capture/Compare 4 DMA request enable
        using CC3DE = Field<DIER_Reg, 11, 1>; // Capture/Compare 3 DMA request enable
        using CC2DE = Field<DIER_Reg, 10, 1>; // Capture/Compare 2 DMA request enable
        using CC1DE = Field<DIER_Reg, 9, 1>; // Capture/Compare 1 DMA request enable
        using UDE = Field<DIER_Reg, 8, 1>; // Update DMA request enable
        using TIE = Field<DIER_Reg, 6, 1>; // Trigger interrupt enable
        using CC4IE = Field<DIER_Reg, 4, 1>; // Capture/Compare 4 interrupt enable
        using CC3IE = Field<DIER_Reg, 3, 1>; // Capture/Compare 3 interrupt enable
        using CC2IE = Field<DIER_Reg, 2, 1>; // Capture/Compare 2 interrupt enable
        using CC1IE = Field<DIER_Reg, 1, 1>; // Capture/Compare 1 interrupt enable
        using UIE = Field<DIER_Reg, 0, 1>; // Update interrupt enable
    };

    // status register
    using SR_Reg = Register<TIM3_BASE + 0x10, ReadWrite>;
    struct _SR {
        using CC4OF = Field<SR_Reg, 12, 1>; // Capture/Compare 4 overcapture flag
        using CC3OF = Field<SR_Reg, 11, 1>; // Capture/Compare 3 overcapture flag
        using CC2OF = Field<SR_Reg, 10, 1>; // Capture/compare 2 overcapture flag
        using CC1OF = Field<SR_Reg, 9, 1>; // Capture/Compare 1 overcapture flag
        using TIF = Field<SR_Reg, 6, 1>; // Trigger interrupt flag
        using CC4IF = Field<SR_Reg, 4, 1>; // Capture/Compare 4 interrupt flag
        using CC3IF = Field<SR_Reg, 3, 1>; // Capture/Compare 3 interrupt flag
        using CC2IF = Field<SR_Reg, 2, 1>; // Capture/Compare 2 interrupt flag
        using CC1IF = Field<SR_Reg, 1, 1>; // Capture/compare 1 interrupt flag
        using UIF = Field<SR_Reg, 0, 1>; // Update interrupt flag
    };

    // event generation register
    using EGR_Reg = Register<TIM3_BASE + 0x14, WriteOnly>;
    struct _EGR {
        using TG = Field<EGR_Reg, 6, 1>; // Trigger generation
        using CC4G = Field<EGR_Reg, 4, 1>; // Capture/compare 4 generation
        using CC3G = Field<EGR_Reg, 3, 1>; // Capture/compare 3 generation
        using CC2G = Field<EGR_Reg, 2, 1>; // Capture/compare 2 generation
        using CC1G = Field<EGR_Reg, 1, 1>; // Capture/compare 1 generation
        using UG = Field<EGR_Reg, 0, 1>; // Update generation
    };

    // capture/compare mode register 1 (output mode)
    using CCMR1_Output_Reg = Register<TIM3_BASE + 0x18, ReadWrite>;
    struct _CCMR1_Output {
        using OC2CE = Field<CCMR1_Output_Reg, 15, 1>; // OC2CE
        using OC2M = Field<CCMR1_Output_Reg, 12, 3>; // OC2M
        using OC2PE = Field<CCMR1_Output_Reg, 11, 1>; // OC2PE
        using OC2FE = Field<CCMR1_Output_Reg, 10, 1>; // OC2FE
        using CC2S = Field<CCMR1_Output_Reg, 8, 2>; // CC2S
        using OC1CE = Field<CCMR1_Output_Reg, 7, 1>; // OC1CE
        using OC1M = Field<CCMR1_Output_Reg, 4, 3>; // OC1M
        using OC1PE = Field<CCMR1_Output_Reg, 3, 1>; // OC1PE
        using OC1FE = Field<CCMR1_Output_Reg, 2, 1>; // OC1FE
        using CC1S = Field<CCMR1_Output_Reg, 0, 2>; // CC1S
    };

    // capture/compare mode register 1 (input mode)
    using CCMR1_Input_Reg = Register<TIM3_BASE + 0x18, ReadWrite>;
    struct _CCMR1_Input {
        using IC2F = Field<CCMR1_Input_Reg, 12, 4>; // Input capture 2 filter
        using IC2PSC = Field<CCMR1_Input_Reg, 10, 2>; // Input capture 2 prescaler
        using CC2S = Field<CCMR1_Input_Reg, 8, 2>; // Capture/Compare 2 selection
        using IC1F = Field<CCMR1_Input_Reg, 4, 4>; // Input capture 1 filter
        using IC1PSC = Field<CCMR1_Input_Reg, 2, 2>; // Input capture 1 prescaler
        using CC1S = Field<CCMR1_Input_Reg, 0, 2>; // Capture/Compare 1 selection
    };

    // capture/compare mode register 2 (output mode)
    using CCMR2_Output_Reg = Register<TIM3_BASE + 0x1C, ReadWrite>;
    struct _CCMR2_Output {
        using O24CE = Field<CCMR2_Output_Reg, 15, 1>; // O24CE
        using OC4M = Field<CCMR2_Output_Reg, 12, 3>; // OC4M
        using OC4PE = Field<CCMR2_Output_Reg, 11, 1>; // OC4PE
        using OC4FE = Field<CCMR2_Output_Reg, 10, 1>; // OC4FE
        using CC4S = Field<CCMR2_Output_Reg, 8, 2>; // CC4S
        using OC3CE = Field<CCMR2_Output_Reg, 7, 1>; // OC3CE
        using OC3M = Field<CCMR2_Output_Reg, 4, 3>; // OC3M
        using OC3PE = Field<CCMR2_Output_Reg, 3, 1>; // OC3PE
        using OC3FE = Field<CCMR2_Output_Reg, 2, 1>; // OC3FE
        using CC3S = Field<CCMR2_Output_Reg, 0, 2>; // CC3S
    };

    // capture/compare mode register 2 (input mode)
    using CCMR2_Input_Reg = Register<TIM3_BASE + 0x1C, ReadWrite>;
    struct _CCMR2_Input {
        using IC4F = Field<CCMR2_Input_Reg, 12, 4>; // Input capture 4 filter
        using IC4PSC = Field<CCMR2_Input_Reg, 10, 2>; // Input capture 4 prescaler
        using CC4S = Field<CCMR2_Input_Reg, 8, 2>; // Capture/Compare 4 selection
        using IC3F = Field<CCMR2_Input_Reg, 4, 4>; // Input capture 3 filter
        using IC3PSC = Field<CCMR2_Input_Reg, 2, 2>; // Input capture 3 prescaler
        using CC3S = Field<CCMR2_Input_Reg, 0, 2>; // Capture/compare 3 selection
    };

    // capture/compare enable register
    using CCER_Reg = Register<TIM3_BASE + 0x20, ReadWrite>;
    struct _CCER {
        using CC4NP = Field<CCER_Reg, 15, 1>; // Capture/Compare 4 output Polarity
        using CC4P = Field<CCER_Reg, 13, 1>; // Capture/Compare 3 output Polarity
        using CC4E = Field<CCER_Reg, 12, 1>; // Capture/Compare 4 output enable
        using CC3NP = Field<CCER_Reg, 11, 1>; // Capture/Compare 3 output Polarity
        using CC3P = Field<CCER_Reg, 9, 1>; // Capture/Compare 3 output Polarity
        using CC3E = Field<CCER_Reg, 8, 1>; // Capture/Compare 3 output enable
        using CC2NP = Field<CCER_Reg, 7, 1>; // Capture/Compare 2 output Polarity
        using CC2P = Field<CCER_Reg, 5, 1>; // Capture/Compare 2 output Polarity
        using CC2E = Field<CCER_Reg, 4, 1>; // Capture/Compare 2 output enable
        using CC1NP = Field<CCER_Reg, 3, 1>; // Capture/Compare 1 output Polarity
        using CC1P = Field<CCER_Reg, 1, 1>; // Capture/Compare 1 output Polarity
        using CC1E = Field<CCER_Reg, 0, 1>; // Capture/Compare 1 output enable
    };

    // counter
    using CNT_Reg = Register<TIM3_BASE + 0x24, ReadWrite>;
    struct _CNT {
        using CNT_H = Field<CNT_Reg, 16, 16>; // High counter value
        using CNT_L = Field<CNT_Reg, 0, 16>; // Low counter value
    };

    // prescaler
    using PSC_Reg = Register<TIM3_BASE + 0x28, ReadWrite>;
    struct _PSC {
        using PSC = Field<PSC_Reg, 0, 16>; // Prescaler value
    };

    // auto-reload register
    using ARR_Reg = Register<TIM3_BASE + 0x2C, ReadWrite>;
    struct _ARR {
        using ARR_H = Field<ARR_Reg, 16, 16>; // High Auto-reload value
        using ARR_L = Field<ARR_Reg, 0, 16>; // Low Auto-reload value
    };

    // capture/compare register 1
    using CCR1_Reg = Register<TIM3_BASE + 0x34, ReadWrite>;
    struct _CCR1 {
        using CCR1_H = Field<CCR1_Reg, 16, 16>; // High Capture/Compare 1 value
        using CCR1_L = Field<CCR1_Reg, 0, 16>; // Low Capture/Compare 1 value
    };

    // capture/compare register 2
    using CCR2_Reg = Register<TIM3_BASE + 0x38, ReadWrite>;
    struct _CCR2 {
        using CCR2_H = Field<CCR2_Reg, 16, 16>; // High Capture/Compare 2 value
        using CCR2_L = Field<CCR2_Reg, 0, 16>; // Low Capture/Compare 2 value
    };

    // capture/compare register 3
    using CCR3_Reg = Register<TIM3_BASE + 0x3C, ReadWrite>;
    struct _CCR3 {
        using CCR3_H = Field<CCR3_Reg, 16, 16>; // High Capture/Compare value
        using CCR3_L = Field<CCR3_Reg, 0, 16>; // Low Capture/Compare value
    };

    // capture/compare register 4
    using CCR4_Reg = Register<TIM3_BASE + 0x40, ReadWrite>;
    struct _CCR4 {
        using CCR4_H = Field<CCR4_Reg, 16, 16>; // High Capture/Compare value
        using CCR4_L = Field<CCR4_Reg, 0, 16>; // Low Capture/Compare value
    };

    // DMA control register
    using DCR_Reg = Register<TIM3_BASE + 0x48, ReadWrite>;
    struct _DCR {
        using DBL = Field<DCR_Reg, 8, 5>; // DMA burst length
        using DBA = Field<DCR_Reg, 0, 5>; // DMA base address
    };

    // DMA address for full transfer
    using DMAR_Reg = Register<TIM3_BASE + 0x4C, ReadWrite>;
    struct _DMAR {
        using DMAB = Field<DMAR_Reg, 0, 16>; // DMA register for burst accesses
    };

    // TIM3 option register 1
    using OR1_Reg = Register<TIM3_BASE + 0x50, ReadWrite>;
    struct _OR1 {
        using TI1_RMP = Field<OR1_Reg, 0, 2>; // Input Capture 1 remap
    };

    // TIM3 option register 2
    using OR2_Reg = Register<TIM3_BASE + 0x60, ReadWrite>;
    struct _OR2 {
        using ETRSEL = Field<OR2_Reg, 14, 3>; // ETR source selection
    };

} // namespace TIM3

// --------------------------------------------
// TIM4: General purpose timers
// Base address: 0x40000800
// --------------------------------------------

namespace TIM4 {
    static constexpr uint32_t TIM4_BASE = 0x40000800;

    // control register 1
    using CR1_Reg = Register<TIM4_BASE + 0x0, ReadWrite>;
    struct _CR1 {
        using CKD = Field<CR1_Reg, 8, 2>; // Clock division
        using ARPE = Field<CR1_Reg, 7, 1>; // Auto-reload preload enable
        using CMS = Field<CR1_Reg, 5, 2>; // Center-aligned mode selection
        using DIR = Field<CR1_Reg, 4, 1>; // Direction
        using OPM = Field<CR1_Reg, 3, 1>; // One-pulse mode
        using URS = Field<CR1_Reg, 2, 1>; // Update request source
        using UDIS = Field<CR1_Reg, 1, 1>; // Update disable
        using CEN = Field<CR1_Reg, 0, 1>; // Counter enable
    };

    // control register 2
    using CR2_Reg = Register<TIM4_BASE + 0x4, ReadWrite>;
    struct _CR2 {
        using TI1S = Field<CR2_Reg, 7, 1>; // TI1 selection
        using MMS = Field<CR2_Reg, 4, 3>; // Master mode selection
        using CCDS = Field<CR2_Reg, 3, 1>; // Capture/compare DMA selection
    };

    // slave mode control register
    using SMCR_Reg = Register<TIM4_BASE + 0x8, ReadWrite>;
    struct _SMCR {
        using ETP = Field<SMCR_Reg, 15, 1>; // External trigger polarity
        using ECE = Field<SMCR_Reg, 14, 1>; // External clock enable
        using ETPS = Field<SMCR_Reg, 12, 2>; // External trigger prescaler
        using ETF = Field<SMCR_Reg, 8, 4>; // External trigger filter
        using MSM = Field<SMCR_Reg, 7, 1>; // Master/Slave mode
        using TS = Field<SMCR_Reg, 4, 3>; // Trigger selection
        using SMS = Field<SMCR_Reg, 0, 3>; // Slave mode selection
    };

    // DMA/Interrupt enable register
    using DIER_Reg = Register<TIM4_BASE + 0xC, ReadWrite>;
    struct _DIER {
        using TDE = Field<DIER_Reg, 14, 1>; // Trigger DMA request enable
        using CC4DE = Field<DIER_Reg, 12, 1>; // Capture/Compare 4 DMA request enable
        using CC3DE = Field<DIER_Reg, 11, 1>; // Capture/Compare 3 DMA request enable
        using CC2DE = Field<DIER_Reg, 10, 1>; // Capture/Compare 2 DMA request enable
        using CC1DE = Field<DIER_Reg, 9, 1>; // Capture/Compare 1 DMA request enable
        using UDE = Field<DIER_Reg, 8, 1>; // Update DMA request enable
        using TIE = Field<DIER_Reg, 6, 1>; // Trigger interrupt enable
        using CC4IE = Field<DIER_Reg, 4, 1>; // Capture/Compare 4 interrupt enable
        using CC3IE = Field<DIER_Reg, 3, 1>; // Capture/Compare 3 interrupt enable
        using CC2IE = Field<DIER_Reg, 2, 1>; // Capture/Compare 2 interrupt enable
        using CC1IE = Field<DIER_Reg, 1, 1>; // Capture/Compare 1 interrupt enable
        using UIE = Field<DIER_Reg, 0, 1>; // Update interrupt enable
    };

    // status register
    using SR_Reg = Register<TIM4_BASE + 0x10, ReadWrite>;
    struct _SR {
        using CC4OF = Field<SR_Reg, 12, 1>; // Capture/Compare 4 overcapture flag
        using CC3OF = Field<SR_Reg, 11, 1>; // Capture/Compare 3 overcapture flag
        using CC2OF = Field<SR_Reg, 10, 1>; // Capture/compare 2 overcapture flag
        using CC1OF = Field<SR_Reg, 9, 1>; // Capture/Compare 1 overcapture flag
        using TIF = Field<SR_Reg, 6, 1>; // Trigger interrupt flag
        using CC4IF = Field<SR_Reg, 4, 1>; // Capture/Compare 4 interrupt flag
        using CC3IF = Field<SR_Reg, 3, 1>; // Capture/Compare 3 interrupt flag
        using CC2IF = Field<SR_Reg, 2, 1>; // Capture/Compare 2 interrupt flag
        using CC1IF = Field<SR_Reg, 1, 1>; // Capture/compare 1 interrupt flag
        using UIF = Field<SR_Reg, 0, 1>; // Update interrupt flag
    };

    // event generation register
    using EGR_Reg = Register<TIM4_BASE + 0x14, WriteOnly>;
    struct _EGR {
        using TG = Field<EGR_Reg, 6, 1>; // Trigger generation
        using CC4G = Field<EGR_Reg, 4, 1>; // Capture/compare 4 generation
        using CC3G = Field<EGR_Reg, 3, 1>; // Capture/compare 3 generation
        using CC2G = Field<EGR_Reg, 2, 1>; // Capture/compare 2 generation
        using CC1G = Field<EGR_Reg, 1, 1>; // Capture/compare 1 generation
        using UG = Field<EGR_Reg, 0, 1>; // Update generation
    };

    // capture/compare mode register 1 (output mode)
    using CCMR1_Output_Reg = Register<TIM4_BASE + 0x18, ReadWrite>;
    struct _CCMR1_Output {
        using OC2CE = Field<CCMR1_Output_Reg, 15, 1>; // OC2CE
        using OC2M = Field<CCMR1_Output_Reg, 12, 3>; // OC2M
        using OC2PE = Field<CCMR1_Output_Reg, 11, 1>; // OC2PE
        using OC2FE = Field<CCMR1_Output_Reg, 10, 1>; // OC2FE
        using CC2S = Field<CCMR1_Output_Reg, 8, 2>; // CC2S
        using OC1CE = Field<CCMR1_Output_Reg, 7, 1>; // OC1CE
        using OC1M = Field<CCMR1_Output_Reg, 4, 3>; // OC1M
        using OC1PE = Field<CCMR1_Output_Reg, 3, 1>; // OC1PE
        using OC1FE = Field<CCMR1_Output_Reg, 2, 1>; // OC1FE
        using CC1S = Field<CCMR1_Output_Reg, 0, 2>; // CC1S
    };

    // capture/compare mode register 1 (input mode)
    using CCMR1_Input_Reg = Register<TIM4_BASE + 0x18, ReadWrite>;
    struct _CCMR1_Input {
        using IC2F = Field<CCMR1_Input_Reg, 12, 4>; // Input capture 2 filter
        using IC2PSC = Field<CCMR1_Input_Reg, 10, 2>; // Input capture 2 prescaler
        using CC2S = Field<CCMR1_Input_Reg, 8, 2>; // Capture/Compare 2 selection
        using IC1F = Field<CCMR1_Input_Reg, 4, 4>; // Input capture 1 filter
        using IC1PSC = Field<CCMR1_Input_Reg, 2, 2>; // Input capture 1 prescaler
        using CC1S = Field<CCMR1_Input_Reg, 0, 2>; // Capture/Compare 1 selection
    };

    // capture/compare mode register 2 (output mode)
    using CCMR2_Output_Reg = Register<TIM4_BASE + 0x1C, ReadWrite>;
    struct _CCMR2_Output {
        using O24CE = Field<CCMR2_Output_Reg, 15, 1>; // O24CE
        using OC4M = Field<CCMR2_Output_Reg, 12, 3>; // OC4M
        using OC4PE = Field<CCMR2_Output_Reg, 11, 1>; // OC4PE
        using OC4FE = Field<CCMR2_Output_Reg, 10, 1>; // OC4FE
        using CC4S = Field<CCMR2_Output_Reg, 8, 2>; // CC4S
        using OC3CE = Field<CCMR2_Output_Reg, 7, 1>; // OC3CE
        using OC3M = Field<CCMR2_Output_Reg, 4, 3>; // OC3M
        using OC3PE = Field<CCMR2_Output_Reg, 3, 1>; // OC3PE
        using OC3FE = Field<CCMR2_Output_Reg, 2, 1>; // OC3FE
        using CC3S = Field<CCMR2_Output_Reg, 0, 2>; // CC3S
    };

    // capture/compare mode register 2 (input mode)
    using CCMR2_Input_Reg = Register<TIM4_BASE + 0x1C, ReadWrite>;
    struct _CCMR2_Input {
        using IC4F = Field<CCMR2_Input_Reg, 12, 4>; // Input capture 4 filter
        using IC4PSC = Field<CCMR2_Input_Reg, 10, 2>; // Input capture 4 prescaler
        using CC4S = Field<CCMR2_Input_Reg, 8, 2>; // Capture/Compare 4 selection
        using IC3F = Field<CCMR2_Input_Reg, 4, 4>; // Input capture 3 filter
        using IC3PSC = Field<CCMR2_Input_Reg, 2, 2>; // Input capture 3 prescaler
        using CC3S = Field<CCMR2_Input_Reg, 0, 2>; // Capture/compare 3 selection
    };

    // capture/compare enable register
    using CCER_Reg = Register<TIM4_BASE + 0x20, ReadWrite>;
    struct _CCER {
        using CC4NP = Field<CCER_Reg, 15, 1>; // Capture/Compare 4 output Polarity
        using CC4P = Field<CCER_Reg, 13, 1>; // Capture/Compare 3 output Polarity
        using CC4E = Field<CCER_Reg, 12, 1>; // Capture/Compare 4 output enable
        using CC3NP = Field<CCER_Reg, 11, 1>; // Capture/Compare 3 output Polarity
        using CC3P = Field<CCER_Reg, 9, 1>; // Capture/Compare 3 output Polarity
        using CC3E = Field<CCER_Reg, 8, 1>; // Capture/Compare 3 output enable
        using CC2NP = Field<CCER_Reg, 7, 1>; // Capture/Compare 2 output Polarity
        using CC2P = Field<CCER_Reg, 5, 1>; // Capture/Compare 2 output Polarity
        using CC2E = Field<CCER_Reg, 4, 1>; // Capture/Compare 2 output enable
        using CC1NP = Field<CCER_Reg, 3, 1>; // Capture/Compare 1 output Polarity
        using CC1P = Field<CCER_Reg, 1, 1>; // Capture/Compare 1 output Polarity
        using CC1E = Field<CCER_Reg, 0, 1>; // Capture/Compare 1 output enable
    };

    // counter
    using CNT_Reg = Register<TIM4_BASE + 0x24, ReadWrite>;
    struct _CNT {
        using CNT_H = Field<CNT_Reg, 16, 16>; // High counter value
        using CNT_L = Field<CNT_Reg, 0, 16>; // Low counter value
    };

    // prescaler
    using PSC_Reg = Register<TIM4_BASE + 0x28, ReadWrite>;
    struct _PSC {
        using PSC = Field<PSC_Reg, 0, 16>; // Prescaler value
    };

    // auto-reload register
    using ARR_Reg = Register<TIM4_BASE + 0x2C, ReadWrite>;
    struct _ARR {
        using ARR_H = Field<ARR_Reg, 16, 16>; // High Auto-reload value
        using ARR_L = Field<ARR_Reg, 0, 16>; // Low Auto-reload value
    };

    // capture/compare register 1
    using CCR1_Reg = Register<TIM4_BASE + 0x34, ReadWrite>;
    struct _CCR1 {
        using CCR1_H = Field<CCR1_Reg, 16, 16>; // High Capture/Compare 1 value
        using CCR1_L = Field<CCR1_Reg, 0, 16>; // Low Capture/Compare 1 value
    };

    // capture/compare register 2
    using CCR2_Reg = Register<TIM4_BASE + 0x38, ReadWrite>;
    struct _CCR2 {
        using CCR2_H = Field<CCR2_Reg, 16, 16>; // High Capture/Compare 2 value
        using CCR2_L = Field<CCR2_Reg, 0, 16>; // Low Capture/Compare 2 value
    };

    // capture/compare register 3
    using CCR3_Reg = Register<TIM4_BASE + 0x3C, ReadWrite>;
    struct _CCR3 {
        using CCR3_H = Field<CCR3_Reg, 16, 16>; // High Capture/Compare value
        using CCR3_L = Field<CCR3_Reg, 0, 16>; // Low Capture/Compare value
    };

    // capture/compare register 4
    using CCR4_Reg = Register<TIM4_BASE + 0x40, ReadWrite>;
    struct _CCR4 {
        using CCR4_H = Field<CCR4_Reg, 16, 16>; // High Capture/Compare value
        using CCR4_L = Field<CCR4_Reg, 0, 16>; // Low Capture/Compare value
    };

    // DMA control register
    using DCR_Reg = Register<TIM4_BASE + 0x48, ReadWrite>;
    struct _DCR {
        using DBL = Field<DCR_Reg, 8, 5>; // DMA burst length
        using DBA = Field<DCR_Reg, 0, 5>; // DMA base address
    };

    // DMA address for full transfer
    using DMAR_Reg = Register<TIM4_BASE + 0x4C, ReadWrite>;
    struct _DMAR {
        using DMAB = Field<DMAR_Reg, 0, 16>; // DMA register for burst accesses
    };

} // namespace TIM4

// --------------------------------------------
// TIM5: 
// Base address: 0x40000C00
// --------------------------------------------

namespace TIM5 {
    static constexpr uint32_t TIM5_BASE = 0x40000C00;

} // namespace TIM5

// --------------------------------------------
// TIM9: General purpose timers
// Base address: 0x40014000
// --------------------------------------------

namespace TIM9 {
    static constexpr uint32_t TIM9_BASE = 0x40014000;

    // control register 1
    using CR1_Reg = Register<TIM9_BASE + 0x0, ReadWrite>;
    struct _CR1 {
        using CKD = Field<CR1_Reg, 8, 2>; // Clock division
        using ARPE = Field<CR1_Reg, 7, 1>; // Auto-reload preload enable
        using OPM = Field<CR1_Reg, 3, 1>; // One-pulse mode
        using URS = Field<CR1_Reg, 2, 1>; // Update request source
        using UDIS = Field<CR1_Reg, 1, 1>; // Update disable
        using CEN = Field<CR1_Reg, 0, 1>; // Counter enable
    };

    // slave mode control register
    using SMCR_Reg = Register<TIM9_BASE + 0x8, ReadWrite>;
    struct _SMCR {
        using MSM = Field<SMCR_Reg, 7, 1>; // Master/Slave mode
        using TS = Field<SMCR_Reg, 4, 3>; // Trigger selection
        using SMS = Field<SMCR_Reg, 0, 3>; // Slave mode selection
    };

    // DMA/Interrupt enable register
    using DIER_Reg = Register<TIM9_BASE + 0xC, ReadWrite>;
    struct _DIER {
        using TIE = Field<DIER_Reg, 6, 1>; // Trigger interrupt enable
        using CC2IE = Field<DIER_Reg, 2, 1>; // Capture/Compare 2 interrupt enable
        using CC1IE = Field<DIER_Reg, 1, 1>; // Capture/Compare 1 interrupt enable
        using UIE = Field<DIER_Reg, 0, 1>; // Update interrupt enable
    };

    // status register
    using SR_Reg = Register<TIM9_BASE + 0x10, ReadWrite>;
    struct _SR {
        using CC2OF = Field<SR_Reg, 10, 1>; // Capture/compare 2 overcapture flag
        using CC1OF = Field<SR_Reg, 9, 1>; // Capture/Compare 1 overcapture flag
        using TIF = Field<SR_Reg, 6, 1>; // Trigger interrupt flag
        using CC2IF = Field<SR_Reg, 2, 1>; // Capture/Compare 2 interrupt flag
        using CC1IF = Field<SR_Reg, 1, 1>; // Capture/compare 1 interrupt flag
        using UIF = Field<SR_Reg, 0, 1>; // Update interrupt flag
    };

    // event generation register
    using EGR_Reg = Register<TIM9_BASE + 0x14, WriteOnly>;
    struct _EGR {
        using TG = Field<EGR_Reg, 6, 1>; // Trigger generation
        using CC2G = Field<EGR_Reg, 2, 1>; // Capture/compare 2 generation
        using CC1G = Field<EGR_Reg, 1, 1>; // Capture/compare 1 generation
        using UG = Field<EGR_Reg, 0, 1>; // Update generation
    };

    // capture/compare mode register 1 (output mode)
    using CCMR1_Output_Reg = Register<TIM9_BASE + 0x18, ReadWrite>;
    struct _CCMR1_Output {
        using OC2M = Field<CCMR1_Output_Reg, 12, 3>; // Output Compare 2 mode
        using OC2PE = Field<CCMR1_Output_Reg, 11, 1>; // Output Compare 2 preload enable
        using OC2FE = Field<CCMR1_Output_Reg, 10, 1>; // Output Compare 2 fast enable
        using CC2S = Field<CCMR1_Output_Reg, 8, 2>; // Capture/Compare 2 selection
        using OC1M = Field<CCMR1_Output_Reg, 4, 3>; // Output Compare 1 mode
        using OC1PE = Field<CCMR1_Output_Reg, 3, 1>; // Output Compare 1 preload enable
        using OC1FE = Field<CCMR1_Output_Reg, 2, 1>; // Output Compare 1 fast enable
        using CC1S = Field<CCMR1_Output_Reg, 0, 2>; // Capture/Compare 1 selection
    };

    // capture/compare mode register 1 (input mode)
    using CCMR1_Input_Reg = Register<TIM9_BASE + 0x18, ReadWrite>;
    struct _CCMR1_Input {
        using IC2F = Field<CCMR1_Input_Reg, 12, 3>; // Input capture 2 filter
        using IC2PSC = Field<CCMR1_Input_Reg, 10, 2>; // Input capture 2 prescaler
        using CC2S = Field<CCMR1_Input_Reg, 8, 2>; // Capture/Compare 2 selection
        using IC1F = Field<CCMR1_Input_Reg, 4, 3>; // Input capture 1 filter
        using IC1PSC = Field<CCMR1_Input_Reg, 2, 2>; // Input capture 1 prescaler
        using CC1S = Field<CCMR1_Input_Reg, 0, 2>; // Capture/Compare 1 selection
    };

    // capture/compare enable register
    using CCER_Reg = Register<TIM9_BASE + 0x20, ReadWrite>;
    struct _CCER {
        using CC2NP = Field<CCER_Reg, 7, 1>; // Capture/Compare 2 output Polarity
        using CC2P = Field<CCER_Reg, 5, 1>; // Capture/Compare 2 output Polarity
        using CC2E = Field<CCER_Reg, 4, 1>; // Capture/Compare 2 output enable
        using CC1NP = Field<CCER_Reg, 3, 1>; // Capture/Compare 1 output Polarity
        using CC1P = Field<CCER_Reg, 1, 1>; // Capture/Compare 1 output Polarity
        using CC1E = Field<CCER_Reg, 0, 1>; // Capture/Compare 1 output enable
    };

    // counter
    using CNT_Reg = Register<TIM9_BASE + 0x24, ReadWrite>;
    struct _CNT {
        using CNT = Field<CNT_Reg, 0, 16>; // counter value
    };

    // prescaler
    using PSC_Reg = Register<TIM9_BASE + 0x28, ReadWrite>;
    struct _PSC {
        using PSC = Field<PSC_Reg, 0, 16>; // Prescaler value
    };

    // auto-reload register
    using ARR_Reg = Register<TIM9_BASE + 0x2C, ReadWrite>;
    struct _ARR {
        using ARR = Field<ARR_Reg, 0, 16>; // Auto-reload value
    };

    // capture/compare register 1
    using CCR1_Reg = Register<TIM9_BASE + 0x34, ReadWrite>;
    struct _CCR1 {
        using CCR1 = Field<CCR1_Reg, 0, 16>; // Capture/Compare 1 value
    };

    // capture/compare register 2
    using CCR2_Reg = Register<TIM9_BASE + 0x38, ReadWrite>;
    struct _CCR2 {
        using CCR2 = Field<CCR2_Reg, 0, 16>; // Capture/Compare 2 value
    };

} // namespace TIM9

// --------------------------------------------
// TIM12: 
// Base address: 0x40001800
// --------------------------------------------

namespace TIM12 {
    static constexpr uint32_t TIM12_BASE = 0x40001800;

} // namespace TIM12

// --------------------------------------------
// TIM10: General-purpose-timers
// Base address: 0x40014400
// --------------------------------------------

namespace TIM10 {
    static constexpr uint32_t TIM10_BASE = 0x40014400;

    // control register 1
    using CR1_Reg = Register<TIM10_BASE + 0x0, ReadWrite>;
    struct _CR1 {
        using CKD = Field<CR1_Reg, 8, 2>; // Clock division
        using ARPE = Field<CR1_Reg, 7, 1>; // Auto-reload preload enable
        using URS = Field<CR1_Reg, 2, 1>; // Update request source
        using UDIS = Field<CR1_Reg, 1, 1>; // Update disable
        using CEN = Field<CR1_Reg, 0, 1>; // Counter enable
    };

    // DMA/Interrupt enable register
    using DIER_Reg = Register<TIM10_BASE + 0xC, ReadWrite>;
    struct _DIER {
        using CC1IE = Field<DIER_Reg, 1, 1>; // Capture/Compare 1 interrupt enable
        using UIE = Field<DIER_Reg, 0, 1>; // Update interrupt enable
    };

    // status register
    using SR_Reg = Register<TIM10_BASE + 0x10, ReadWrite>;
    struct _SR {
        using CC1OF = Field<SR_Reg, 9, 1>; // Capture/Compare 1 overcapture flag
        using CC1IF = Field<SR_Reg, 1, 1>; // Capture/compare 1 interrupt flag
        using UIF = Field<SR_Reg, 0, 1>; // Update interrupt flag
    };

    // event generation register
    using EGR_Reg = Register<TIM10_BASE + 0x14, WriteOnly>;
    struct _EGR {
        using CC1G = Field<EGR_Reg, 1, 1>; // Capture/compare 1 generation
        using UG = Field<EGR_Reg, 0, 1>; // Update generation
    };

    // capture/compare mode register 1 (output mode)
    using CCMR1_Output_Reg = Register<TIM10_BASE + 0x18, ReadWrite>;
    struct _CCMR1_Output {
        using OC1M = Field<CCMR1_Output_Reg, 4, 3>; // Output Compare 1 mode
        using OC1PE = Field<CCMR1_Output_Reg, 3, 1>; // Output Compare 1 preload enable
        using OC1FE = Field<CCMR1_Output_Reg, 2, 1>; // Output Compare 1 fast enable
        using CC1S = Field<CCMR1_Output_Reg, 0, 2>; // Capture/Compare 1 selection
    };

    // capture/compare mode register 1 (input mode)
    using CCMR1_Input_Reg = Register<TIM10_BASE + 0x18, ReadWrite>;
    struct _CCMR1_Input {
        using IC1F = Field<CCMR1_Input_Reg, 4, 4>; // Input capture 1 filter
        using IC1PSC = Field<CCMR1_Input_Reg, 2, 2>; // Input capture 1 prescaler
        using CC1S = Field<CCMR1_Input_Reg, 0, 2>; // Capture/Compare 1 selection
    };

    // capture/compare enable register
    using CCER_Reg = Register<TIM10_BASE + 0x20, ReadWrite>;
    struct _CCER {
        using CC1NP = Field<CCER_Reg, 3, 1>; // Capture/Compare 1 output Polarity
        using CC1P = Field<CCER_Reg, 1, 1>; // Capture/Compare 1 output Polarity
        using CC1E = Field<CCER_Reg, 0, 1>; // Capture/Compare 1 output enable
    };

    // counter
    using CNT_Reg = Register<TIM10_BASE + 0x24, ReadWrite>;
    struct _CNT {
        using CNT = Field<CNT_Reg, 0, 16>; // counter value
    };

    // prescaler
    using PSC_Reg = Register<TIM10_BASE + 0x28, ReadWrite>;
    struct _PSC {
        using PSC = Field<PSC_Reg, 0, 16>; // Prescaler value
    };

    // auto-reload register
    using ARR_Reg = Register<TIM10_BASE + 0x2C, ReadWrite>;
    struct _ARR {
        using ARR = Field<ARR_Reg, 0, 16>; // Auto-reload value
    };

    // capture/compare register 1
    using CCR1_Reg = Register<TIM10_BASE + 0x34, ReadWrite>;
    struct _CCR1 {
        using CCR1 = Field<CCR1_Reg, 0, 16>; // Capture/Compare 1 value
    };

    // slave mode control register
    using SMCR_Reg = Register<TIM10_BASE + 0x8, ReadWrite>;
    struct _SMCR {
        using SMS3 = Field<SMCR_Reg, 16, 1>; // Slave mode selection
        using ETP = Field<SMCR_Reg, 15, 1>; // External trigger polarity
        using ECE = Field<SMCR_Reg, 14, 1>; // External clock enable
        using ETPS = Field<SMCR_Reg, 12, 2>; // External trigger prescaler
        using ETF = Field<SMCR_Reg, 8, 4>; // External trigger filter
        using MSM = Field<SMCR_Reg, 7, 1>; // Master/slave mode
        using TS = Field<SMCR_Reg, 4, 3>; // Trigger selection
        using SMS = Field<SMCR_Reg, 0, 3>; // Slave mode selection
    };

    // option register
    using OR_Reg = Register<TIM10_BASE + 0x50, ReadWrite>;
    struct _OR {
        using TI1_RMP = Field<OR_Reg, 0, 2>; // TIM11 Input 1 remapping capability
    };

} // namespace TIM10

// --------------------------------------------
// TIM11: 
// Base address: 0x40014800
// --------------------------------------------

namespace TIM11 {
    static constexpr uint32_t TIM11_BASE = 0x40014800;

} // namespace TIM11

// --------------------------------------------
// TIM13: 
// Base address: 0x40001C00
// --------------------------------------------

namespace TIM13 {
    static constexpr uint32_t TIM13_BASE = 0x40001C00;

} // namespace TIM13

// --------------------------------------------
// TIM14: 
// Base address: 0x40002000
// --------------------------------------------

namespace TIM14 {
    static constexpr uint32_t TIM14_BASE = 0x40002000;

} // namespace TIM14

// --------------------------------------------
// TIM6: Basic timers
// Base address: 0x40001000
// --------------------------------------------

namespace TIM6 {
    static constexpr uint32_t TIM6_BASE = 0x40001000;

    // control register 1
    using CR1_Reg = Register<TIM6_BASE + 0x0, ReadWrite>;
    struct _CR1 {
        using ARPE = Field<CR1_Reg, 7, 1>; // Auto-reload preload enable
        using OPM = Field<CR1_Reg, 3, 1>; // One-pulse mode
        using URS = Field<CR1_Reg, 2, 1>; // Update request source
        using UDIS = Field<CR1_Reg, 1, 1>; // Update disable
        using CEN = Field<CR1_Reg, 0, 1>; // Counter enable
    };

    // control register 2
    using CR2_Reg = Register<TIM6_BASE + 0x4, ReadWrite>;
    struct _CR2 {
        using MMS = Field<CR2_Reg, 4, 3>; // Master mode selection
    };

    // DMA/Interrupt enable register
    using DIER_Reg = Register<TIM6_BASE + 0xC, ReadWrite>;
    struct _DIER {
        using UDE = Field<DIER_Reg, 8, 1>; // Update DMA request enable
        using UIE = Field<DIER_Reg, 0, 1>; // Update interrupt enable
    };

    // status register
    using SR_Reg = Register<TIM6_BASE + 0x10, ReadWrite>;
    struct _SR {
        using UIF = Field<SR_Reg, 0, 1>; // Update interrupt flag
    };

    // event generation register
    using EGR_Reg = Register<TIM6_BASE + 0x14, WriteOnly>;
    struct _EGR {
        using UG = Field<EGR_Reg, 0, 1>; // Update generation
    };

    // counter
    using CNT_Reg = Register<TIM6_BASE + 0x24, ReadWrite>;
    struct _CNT {
        using CNT = Field<CNT_Reg, 0, 16>; // Low counter value
    };

    // prescaler
    using PSC_Reg = Register<TIM6_BASE + 0x28, ReadWrite>;
    struct _PSC {
        using PSC = Field<PSC_Reg, 0, 16>; // Prescaler value
    };

    // auto-reload register
    using ARR_Reg = Register<TIM6_BASE + 0x2C, ReadWrite>;
    struct _ARR {
        using ARR = Field<ARR_Reg, 0, 16>; // Low Auto-reload value
    };

} // namespace TIM6

// --------------------------------------------
// TIM7: 
// Base address: 0x40001400
// --------------------------------------------

namespace TIM7 {
    static constexpr uint32_t TIM7_BASE = 0x40001400;

} // namespace TIM7

// --------------------------------------------
// Ethernet_MAC: Ethernet: media access control (MAC)
// Base address: 0x40028000
// --------------------------------------------

namespace Ethernet_MAC {
    static constexpr uint32_t Ethernet_MAC_BASE = 0x40028000;

    // Ethernet MAC configuration register
    using MACCR_Reg = Register<Ethernet_MAC_BASE + 0x0, ReadWrite>;
    struct _MACCR {
        using RE = Field<MACCR_Reg, 2, 1>; // RE
        using TE = Field<MACCR_Reg, 3, 1>; // TE
        using DC = Field<MACCR_Reg, 4, 1>; // DC
        using BL = Field<MACCR_Reg, 5, 2>; // BL
        using APCS = Field<MACCR_Reg, 7, 1>; // APCS
        using RD = Field<MACCR_Reg, 9, 1>; // RD
        using IPCO = Field<MACCR_Reg, 10, 1>; // IPCO
        using DM = Field<MACCR_Reg, 11, 1>; // DM
        using LM = Field<MACCR_Reg, 12, 1>; // LM
        using ROD = Field<MACCR_Reg, 13, 1>; // ROD
        using FES = Field<MACCR_Reg, 14, 1>; // FES
        using CSD = Field<MACCR_Reg, 16, 1>; // CSD
        using IFG = Field<MACCR_Reg, 17, 3>; // IFG
        using JD = Field<MACCR_Reg, 22, 1>; // JD
        using WD = Field<MACCR_Reg, 23, 1>; // WD
        using CSTF = Field<MACCR_Reg, 25, 1>; // CSTF
    };

    // Ethernet MAC frame filter register
    using MACFFR_Reg = Register<Ethernet_MAC_BASE + 0x4, ReadWrite>;
    struct _MACFFR {
        using PM = Field<MACFFR_Reg, 0, 1>; // PM
        using HU = Field<MACFFR_Reg, 1, 1>; // HU
        using HM = Field<MACFFR_Reg, 2, 1>; // HM
        using DAIF = Field<MACFFR_Reg, 3, 1>; // DAIF
        using RAM = Field<MACFFR_Reg, 4, 1>; // RAM
        using BFD = Field<MACFFR_Reg, 5, 1>; // BFD
        using PCF = Field<MACFFR_Reg, 6, 1>; // PCF
        using SAIF = Field<MACFFR_Reg, 7, 1>; // SAIF
        using SAF = Field<MACFFR_Reg, 8, 1>; // SAF
        using HPF = Field<MACFFR_Reg, 9, 1>; // HPF
        using RA = Field<MACFFR_Reg, 31, 1>; // RA
    };

    // Ethernet MAC hash table high register
    using MACHTHR_Reg = Register<Ethernet_MAC_BASE + 0x8, ReadWrite>;
    struct _MACHTHR {
        using HTH = Field<MACHTHR_Reg, 0, 32>; // HTH
    };

    // Ethernet MAC hash table low register
    using MACHTLR_Reg = Register<Ethernet_MAC_BASE + 0xC, ReadWrite>;
    struct _MACHTLR {
        using HTL = Field<MACHTLR_Reg, 0, 32>; // HTL
    };

    // Ethernet MAC MII address register
    using MACMIIAR_Reg = Register<Ethernet_MAC_BASE + 0x10, ReadWrite>;
    struct _MACMIIAR {
        using MB = Field<MACMIIAR_Reg, 0, 1>; // MB
        using MW = Field<MACMIIAR_Reg, 1, 1>; // MW
        using CR = Field<MACMIIAR_Reg, 2, 3>; // CR
        using MR = Field<MACMIIAR_Reg, 6, 5>; // MR
        using PA = Field<MACMIIAR_Reg, 11, 5>; // PA
    };

    // Ethernet MAC MII data register
    using MACMIIDR_Reg = Register<Ethernet_MAC_BASE + 0x14, ReadWrite>;
    struct _MACMIIDR {
        using TD = Field<MACMIIDR_Reg, 0, 16>; // TD
    };

    // Ethernet MAC flow control register
    using MACFCR_Reg = Register<Ethernet_MAC_BASE + 0x18, ReadWrite>;
    struct _MACFCR {
        using FCB = Field<MACFCR_Reg, 0, 1>; // FCB
        using TFCE = Field<MACFCR_Reg, 1, 1>; // TFCE
        using RFCE = Field<MACFCR_Reg, 2, 1>; // RFCE
        using UPFD = Field<MACFCR_Reg, 3, 1>; // UPFD
        using PLT = Field<MACFCR_Reg, 4, 2>; // PLT
        using ZQPD = Field<MACFCR_Reg, 7, 1>; // ZQPD
        using PT = Field<MACFCR_Reg, 16, 16>; // PT
    };

    // Ethernet MAC VLAN tag register
    using MACVLANTR_Reg = Register<Ethernet_MAC_BASE + 0x1C, ReadWrite>;
    struct _MACVLANTR {
        using VLANTI = Field<MACVLANTR_Reg, 0, 16>; // VLANTI
        using VLANTC = Field<MACVLANTR_Reg, 16, 1>; // VLANTC
    };

    // Ethernet MAC PMT control and status register
    using MACPMTCSR_Reg = Register<Ethernet_MAC_BASE + 0x2C, ReadWrite>;
    struct _MACPMTCSR {
        using PD = Field<MACPMTCSR_Reg, 0, 1>; // PD
        using MPE = Field<MACPMTCSR_Reg, 1, 1>; // MPE
        using WFE = Field<MACPMTCSR_Reg, 2, 1>; // WFE
        using MPR = Field<MACPMTCSR_Reg, 5, 1>; // MPR
        using WFR = Field<MACPMTCSR_Reg, 6, 1>; // WFR
        using GU = Field<MACPMTCSR_Reg, 9, 1>; // GU
        using WFFRPR = Field<MACPMTCSR_Reg, 31, 1>; // WFFRPR
    };

    // Ethernet MAC debug register
    using MACDBGR_Reg = Register<Ethernet_MAC_BASE + 0x34, ReadOnly>;
    struct _MACDBGR {
        using CR = Field<MACDBGR_Reg, 0, 1>; // CR
        using CSR = Field<MACDBGR_Reg, 1, 1>; // CSR
        using ROR = Field<MACDBGR_Reg, 2, 1>; // ROR
        using MCF = Field<MACDBGR_Reg, 3, 1>; // MCF
        using MCP = Field<MACDBGR_Reg, 4, 1>; // MCP
        using MCFHP = Field<MACDBGR_Reg, 5, 1>; // MCFHP
    };

    // Ethernet MAC interrupt status register
    using MACSR_Reg = Register<Ethernet_MAC_BASE + 0x38, ReadWrite>;
    struct _MACSR {
        using PMTS = Field<MACSR_Reg, 3, 1>; // PMTS
        using MMCS = Field<MACSR_Reg, 4, 1>; // MMCS
        using MMCRS = Field<MACSR_Reg, 5, 1>; // MMCRS
        using MMCTS = Field<MACSR_Reg, 6, 1>; // MMCTS
        using TSTS = Field<MACSR_Reg, 9, 1>; // TSTS
    };

    // Ethernet MAC interrupt mask register
    using MACIMR_Reg = Register<Ethernet_MAC_BASE + 0x3C, ReadWrite>;
    struct _MACIMR {
        using PMTIM = Field<MACIMR_Reg, 3, 1>; // PMTIM
        using TSTIM = Field<MACIMR_Reg, 9, 1>; // TSTIM
    };

    // Ethernet MAC address 0 high register
    using MACA0HR_Reg = Register<Ethernet_MAC_BASE + 0x40, ReadWrite>;
    struct _MACA0HR {
        using MACA0H = Field<MACA0HR_Reg, 0, 16>; // MAC address0 high
        using MO = Field<MACA0HR_Reg, 31, 1>; // Always 1
    };

    // Ethernet MAC address 0 low register
    using MACA0LR_Reg = Register<Ethernet_MAC_BASE + 0x44, ReadWrite>;
    struct _MACA0LR {
        using MACA0L = Field<MACA0LR_Reg, 0, 32>; // 0
    };

    // Ethernet MAC address 1 high register
    using MACA1HR_Reg = Register<Ethernet_MAC_BASE + 0x48, ReadWrite>;
    struct _MACA1HR {
        using MACA1H = Field<MACA1HR_Reg, 0, 16>; // MACA1H
        using MBC = Field<MACA1HR_Reg, 24, 6>; // MBC
        using SA = Field<MACA1HR_Reg, 30, 1>; // SA
        using AE = Field<MACA1HR_Reg, 31, 1>; // AE
    };

    // Ethernet MAC address1 low register
    using MACA1LR_Reg = Register<Ethernet_MAC_BASE + 0x4C, ReadWrite>;
    struct _MACA1LR {
        using MACA1LR = Field<MACA1LR_Reg, 0, 32>; // MACA1LR
    };

    // Ethernet MAC address 2 high register
    using MACA2HR_Reg = Register<Ethernet_MAC_BASE + 0x50, ReadWrite>;
    struct _MACA2HR {
        using MAC2AH = Field<MACA2HR_Reg, 0, 16>; // MAC2AH
        using MBC = Field<MACA2HR_Reg, 24, 6>; // MBC
        using SA = Field<MACA2HR_Reg, 30, 1>; // SA
        using AE = Field<MACA2HR_Reg, 31, 1>; // AE
    };

    // Ethernet MAC address 2 low register
    using MACA2LR_Reg = Register<Ethernet_MAC_BASE + 0x54, ReadWrite>;
    struct _MACA2LR {
        using MACA2L = Field<MACA2LR_Reg, 0, 31>; // MACA2L
    };

    // Ethernet MAC address 3 high register
    using MACA3HR_Reg = Register<Ethernet_MAC_BASE + 0x58, ReadWrite>;
    struct _MACA3HR {
        using MACA3H = Field<MACA3HR_Reg, 0, 16>; // MACA3H
        using MBC = Field<MACA3HR_Reg, 24, 6>; // MBC
        using SA = Field<MACA3HR_Reg, 30, 1>; // SA
        using AE = Field<MACA3HR_Reg, 31, 1>; // AE
    };

    // Ethernet MAC address 3 low register
    using MACA3LR_Reg = Register<Ethernet_MAC_BASE + 0x5C, ReadWrite>;
    struct _MACA3LR {
        using MBCA3L = Field<MACA3LR_Reg, 0, 32>; // MBCA3L
    };

    // Ethernet MAC remote wakeup frame filter register
    using MACRWUFFER_Reg = Register<Ethernet_MAC_BASE + 0x60, ReadWrite>;

} // namespace Ethernet_MAC

// --------------------------------------------
// CRC: Cryptographic processor
// Base address: 0x40023000
// --------------------------------------------

namespace CRC {
    static constexpr uint32_t CRC_BASE = 0x40023000;

    // Data register
    using DR_Reg = Register<CRC_BASE + 0x0, ReadWrite>;
    struct _DR {
        using DR = Field<DR_Reg, 0, 32>; // Data Register
    };

    // Independent Data register
    using IDR_Reg = Register<CRC_BASE + 0x4, ReadWrite>;
    struct _IDR {
        using IDR = Field<IDR_Reg, 0, 8>; // Independent Data register
    };

    // Control register
    using CR_Reg = Register<CRC_BASE + 0x8, WriteOnly>;
    struct _CR {
        using CR = Field<CR_Reg, 0, 1>; // Control regidter
    };

    // Initial CRC value
    using INIT_Reg = Register<CRC_BASE + 0x10, ReadWrite>;
    struct _INIT {
        using CRC_INIT = Field<INIT_Reg, 0, 32>; // Programmable initial CRC value
    };

    // CRC polynomial
    using POL_Reg = Register<CRC_BASE + 0x14, ReadWrite>;
    struct _POL {
        using POL = Field<POL_Reg, 0, 32>; // Programmable polynomial
    };

} // namespace CRC

// --------------------------------------------
// CAN1: Controller area network
// Base address: 0x40006400
// --------------------------------------------

namespace CAN1 {
    static constexpr uint32_t CAN1_BASE = 0x40006400;

    // master control register
    using MCR_Reg = Register<CAN1_BASE + 0x0, ReadWrite>;
    struct _MCR {
        using DBF = Field<MCR_Reg, 16, 1>; // DBF
        using RESET = Field<MCR_Reg, 15, 1>; // RESET
        using TTCM = Field<MCR_Reg, 7, 1>; // TTCM
        using ABOM = Field<MCR_Reg, 6, 1>; // ABOM
        using AWUM = Field<MCR_Reg, 5, 1>; // AWUM
        using NART = Field<MCR_Reg, 4, 1>; // NART
        using RFLM = Field<MCR_Reg, 3, 1>; // RFLM
        using TXFP = Field<MCR_Reg, 2, 1>; // TXFP
        using SLEEP = Field<MCR_Reg, 1, 1>; // SLEEP
        using INRQ = Field<MCR_Reg, 0, 1>; // INRQ
    };

    // master status register
    using MSR_Reg = Register<CAN1_BASE + 0x4, ReadWrite>;
    struct _MSR {
        using RX = Field<MSR_Reg, 11, 1>; // RX
        using SAMP = Field<MSR_Reg, 10, 1>; // SAMP
        using RXM = Field<MSR_Reg, 9, 1>; // RXM
        using TXM = Field<MSR_Reg, 8, 1>; // TXM
        using SLAKI = Field<MSR_Reg, 4, 1>; // SLAKI
        using WKUI = Field<MSR_Reg, 3, 1>; // WKUI
        using ERRI = Field<MSR_Reg, 2, 1>; // ERRI
        using SLAK = Field<MSR_Reg, 1, 1>; // SLAK
        using INAK = Field<MSR_Reg, 0, 1>; // INAK
    };

    // transmit status register
    using TSR_Reg = Register<CAN1_BASE + 0x8, ReadWrite>;
    struct _TSR {
        using LOW2 = Field<TSR_Reg, 31, 1>; // Lowest priority flag for mailbox 2
        using LOW1 = Field<TSR_Reg, 30, 1>; // Lowest priority flag for mailbox 1
        using LOW0 = Field<TSR_Reg, 29, 1>; // Lowest priority flag for mailbox 0
        using TME2 = Field<TSR_Reg, 28, 1>; // Lowest priority flag for mailbox 2
        using TME1 = Field<TSR_Reg, 27, 1>; // Lowest priority flag for mailbox 1
        using TME0 = Field<TSR_Reg, 26, 1>; // Lowest priority flag for mailbox 0
        using CODE = Field<TSR_Reg, 24, 2>; // CODE
        using ABRQ2 = Field<TSR_Reg, 23, 1>; // ABRQ2
        using TERR2 = Field<TSR_Reg, 19, 1>; // TERR2
        using ALST2 = Field<TSR_Reg, 18, 1>; // ALST2
        using TXOK2 = Field<TSR_Reg, 17, 1>; // TXOK2
        using RQCP2 = Field<TSR_Reg, 16, 1>; // RQCP2
        using ABRQ1 = Field<TSR_Reg, 15, 1>; // ABRQ1
        using TERR1 = Field<TSR_Reg, 11, 1>; // TERR1
        using ALST1 = Field<TSR_Reg, 10, 1>; // ALST1
        using TXOK1 = Field<TSR_Reg, 9, 1>; // TXOK1
        using RQCP1 = Field<TSR_Reg, 8, 1>; // RQCP1
        using ABRQ0 = Field<TSR_Reg, 7, 1>; // ABRQ0
        using TERR0 = Field<TSR_Reg, 3, 1>; // TERR0
        using ALST0 = Field<TSR_Reg, 2, 1>; // ALST0
        using TXOK0 = Field<TSR_Reg, 1, 1>; // TXOK0
        using RQCP0 = Field<TSR_Reg, 0, 1>; // RQCP0
    };

    // receive FIFO 0 register
    using RF0R_Reg = Register<CAN1_BASE + 0xC, ReadWrite>;
    struct _RF0R {
        using RFOM0 = Field<RF0R_Reg, 5, 1>; // RFOM0
        using FOVR0 = Field<RF0R_Reg, 4, 1>; // FOVR0
        using FULL0 = Field<RF0R_Reg, 3, 1>; // FULL0
        using FMP0 = Field<RF0R_Reg, 0, 2>; // FMP0
    };

    // receive FIFO 1 register
    using RF1R_Reg = Register<CAN1_BASE + 0x10, ReadWrite>;
    struct _RF1R {
        using RFOM1 = Field<RF1R_Reg, 5, 1>; // RFOM1
        using FOVR1 = Field<RF1R_Reg, 4, 1>; // FOVR1
        using FULL1 = Field<RF1R_Reg, 3, 1>; // FULL1
        using FMP1 = Field<RF1R_Reg, 0, 2>; // FMP1
    };

    // interrupt enable register
    using IER_Reg = Register<CAN1_BASE + 0x14, ReadWrite>;
    struct _IER {
        using SLKIE = Field<IER_Reg, 17, 1>; // SLKIE
        using WKUIE = Field<IER_Reg, 16, 1>; // WKUIE
        using ERRIE = Field<IER_Reg, 15, 1>; // ERRIE
        using LECIE = Field<IER_Reg, 11, 1>; // LECIE
        using BOFIE = Field<IER_Reg, 10, 1>; // BOFIE
        using EPVIE = Field<IER_Reg, 9, 1>; // EPVIE
        using EWGIE = Field<IER_Reg, 8, 1>; // EWGIE
        using FOVIE1 = Field<IER_Reg, 6, 1>; // FOVIE1
        using FFIE1 = Field<IER_Reg, 5, 1>; // FFIE1
        using FMPIE1 = Field<IER_Reg, 4, 1>; // FMPIE1
        using FOVIE0 = Field<IER_Reg, 3, 1>; // FOVIE0
        using FFIE0 = Field<IER_Reg, 2, 1>; // FFIE0
        using FMPIE0 = Field<IER_Reg, 1, 1>; // FMPIE0
        using TMEIE = Field<IER_Reg, 0, 1>; // TMEIE
    };

    // interrupt enable register
    using ESR_Reg = Register<CAN1_BASE + 0x18, ReadWrite>;
    struct _ESR {
        using REC = Field<ESR_Reg, 24, 8>; // REC
        using TEC = Field<ESR_Reg, 16, 8>; // TEC
        using LEC = Field<ESR_Reg, 4, 3>; // LEC
        using BOFF = Field<ESR_Reg, 2, 1>; // BOFF
        using EPVF = Field<ESR_Reg, 1, 1>; // EPVF
        using EWGF = Field<ESR_Reg, 0, 1>; // EWGF
    };

    // bit timing register
    using BTR_Reg = Register<CAN1_BASE + 0x1C, ReadWrite>;
    struct _BTR {
        using SILM = Field<BTR_Reg, 31, 1>; // SILM
        using LBKM = Field<BTR_Reg, 30, 1>; // LBKM
        using SJW = Field<BTR_Reg, 24, 2>; // SJW
        using TS2 = Field<BTR_Reg, 20, 3>; // TS2
        using TS1 = Field<BTR_Reg, 16, 4>; // TS1
        using BRP = Field<BTR_Reg, 0, 10>; // BRP
    };

    // TX mailbox identifier register
    using TI0R_Reg = Register<CAN1_BASE + 0x180, ReadWrite>;
    struct _TI0R {
        using STID = Field<TI0R_Reg, 21, 11>; // STID
        using EXID = Field<TI0R_Reg, 3, 18>; // EXID
        using IDE = Field<TI0R_Reg, 2, 1>; // IDE
        using RTR = Field<TI0R_Reg, 1, 1>; // RTR
        using TXRQ = Field<TI0R_Reg, 0, 1>; // TXRQ
    };

    // mailbox data length control and time stamp register
    using TDT0R_Reg = Register<CAN1_BASE + 0x184, ReadWrite>;
    struct _TDT0R {
        using TIME = Field<TDT0R_Reg, 16, 16>; // TIME
        using TGT = Field<TDT0R_Reg, 8, 1>; // TGT
        using DLC = Field<TDT0R_Reg, 0, 4>; // DLC
    };

    // mailbox data low register
    using TDL0R_Reg = Register<CAN1_BASE + 0x188, ReadWrite>;
    struct _TDL0R {
        using DATA3 = Field<TDL0R_Reg, 24, 8>; // DATA3
        using DATA2 = Field<TDL0R_Reg, 16, 8>; // DATA2
        using DATA1 = Field<TDL0R_Reg, 8, 8>; // DATA1
        using DATA0 = Field<TDL0R_Reg, 0, 8>; // DATA0
    };

    // mailbox data high register
    using TDH0R_Reg = Register<CAN1_BASE + 0x18C, ReadWrite>;
    struct _TDH0R {
        using DATA7 = Field<TDH0R_Reg, 24, 8>; // DATA7
        using DATA6 = Field<TDH0R_Reg, 16, 8>; // DATA6
        using DATA5 = Field<TDH0R_Reg, 8, 8>; // DATA5
        using DATA4 = Field<TDH0R_Reg, 0, 8>; // DATA4
    };

    // mailbox identifier register
    using TI1R_Reg = Register<CAN1_BASE + 0x190, ReadWrite>;
    struct _TI1R {
        using STID = Field<TI1R_Reg, 21, 11>; // STID
        using EXID = Field<TI1R_Reg, 3, 18>; // EXID
        using IDE = Field<TI1R_Reg, 2, 1>; // IDE
        using RTR = Field<TI1R_Reg, 1, 1>; // RTR
        using TXRQ = Field<TI1R_Reg, 0, 1>; // TXRQ
    };

    // mailbox data length control and time stamp register
    using TDT1R_Reg = Register<CAN1_BASE + 0x194, ReadWrite>;
    struct _TDT1R {
        using TIME = Field<TDT1R_Reg, 16, 16>; // TIME
        using TGT = Field<TDT1R_Reg, 8, 1>; // TGT
        using DLC = Field<TDT1R_Reg, 0, 4>; // DLC
    };

    // mailbox data low register
    using TDL1R_Reg = Register<CAN1_BASE + 0x198, ReadWrite>;
    struct _TDL1R {
        using DATA3 = Field<TDL1R_Reg, 24, 8>; // DATA3
        using DATA2 = Field<TDL1R_Reg, 16, 8>; // DATA2
        using DATA1 = Field<TDL1R_Reg, 8, 8>; // DATA1
        using DATA0 = Field<TDL1R_Reg, 0, 8>; // DATA0
    };

    // mailbox data high register
    using TDH1R_Reg = Register<CAN1_BASE + 0x19C, ReadWrite>;
    struct _TDH1R {
        using DATA7 = Field<TDH1R_Reg, 24, 8>; // DATA7
        using DATA6 = Field<TDH1R_Reg, 16, 8>; // DATA6
        using DATA5 = Field<TDH1R_Reg, 8, 8>; // DATA5
        using DATA4 = Field<TDH1R_Reg, 0, 8>; // DATA4
    };

    // mailbox identifier register
    using TI2R_Reg = Register<CAN1_BASE + 0x1A0, ReadWrite>;
    struct _TI2R {
        using STID = Field<TI2R_Reg, 21, 11>; // STID
        using EXID = Field<TI2R_Reg, 3, 18>; // EXID
        using IDE = Field<TI2R_Reg, 2, 1>; // IDE
        using RTR = Field<TI2R_Reg, 1, 1>; // RTR
        using TXRQ = Field<TI2R_Reg, 0, 1>; // TXRQ
    };

    // mailbox data length control and time stamp register
    using TDT2R_Reg = Register<CAN1_BASE + 0x1A4, ReadWrite>;
    struct _TDT2R {
        using TIME = Field<TDT2R_Reg, 16, 16>; // TIME
        using TGT = Field<TDT2R_Reg, 8, 1>; // TGT
        using DLC = Field<TDT2R_Reg, 0, 4>; // DLC
    };

    // mailbox data low register
    using TDL2R_Reg = Register<CAN1_BASE + 0x1A8, ReadWrite>;
    struct _TDL2R {
        using DATA3 = Field<TDL2R_Reg, 24, 8>; // DATA3
        using DATA2 = Field<TDL2R_Reg, 16, 8>; // DATA2
        using DATA1 = Field<TDL2R_Reg, 8, 8>; // DATA1
        using DATA0 = Field<TDL2R_Reg, 0, 8>; // DATA0
    };

    // mailbox data high register
    using TDH2R_Reg = Register<CAN1_BASE + 0x1AC, ReadWrite>;
    struct _TDH2R {
        using DATA7 = Field<TDH2R_Reg, 24, 8>; // DATA7
        using DATA6 = Field<TDH2R_Reg, 16, 8>; // DATA6
        using DATA5 = Field<TDH2R_Reg, 8, 8>; // DATA5
        using DATA4 = Field<TDH2R_Reg, 0, 8>; // DATA4
    };

    // receive FIFO mailbox identifier register
    using RI0R_Reg = Register<CAN1_BASE + 0x1B0, ReadOnly>;
    struct _RI0R {
        using STID = Field<RI0R_Reg, 21, 11>; // STID
        using EXID = Field<RI0R_Reg, 3, 18>; // EXID
        using IDE = Field<RI0R_Reg, 2, 1>; // IDE
        using RTR = Field<RI0R_Reg, 1, 1>; // RTR
    };

    // mailbox data high register
    using RDT0R_Reg = Register<CAN1_BASE + 0x1B4, ReadOnly>;
    struct _RDT0R {
        using TIME = Field<RDT0R_Reg, 16, 16>; // TIME
        using FMI = Field<RDT0R_Reg, 8, 8>; // FMI
        using DLC = Field<RDT0R_Reg, 0, 4>; // DLC
    };

    // mailbox data high register
    using RDL0R_Reg = Register<CAN1_BASE + 0x1B8, ReadOnly>;
    struct _RDL0R {
        using DATA3 = Field<RDL0R_Reg, 24, 8>; // DATA3
        using DATA2 = Field<RDL0R_Reg, 16, 8>; // DATA2
        using DATA1 = Field<RDL0R_Reg, 8, 8>; // DATA1
        using DATA0 = Field<RDL0R_Reg, 0, 8>; // DATA0
    };

    // receive FIFO mailbox data high register
    using RDH0R_Reg = Register<CAN1_BASE + 0x1BC, ReadOnly>;
    struct _RDH0R {
        using DATA7 = Field<RDH0R_Reg, 24, 8>; // DATA7
        using DATA6 = Field<RDH0R_Reg, 16, 8>; // DATA6
        using DATA5 = Field<RDH0R_Reg, 8, 8>; // DATA5
        using DATA4 = Field<RDH0R_Reg, 0, 8>; // DATA4
    };

    // mailbox data high register
    using RI1R_Reg = Register<CAN1_BASE + 0x1C0, ReadOnly>;
    struct _RI1R {
        using STID = Field<RI1R_Reg, 21, 11>; // STID
        using EXID = Field<RI1R_Reg, 3, 18>; // EXID
        using IDE = Field<RI1R_Reg, 2, 1>; // IDE
        using RTR = Field<RI1R_Reg, 1, 1>; // RTR
    };

    // mailbox data high register
    using RDT1R_Reg = Register<CAN1_BASE + 0x1C4, ReadOnly>;
    struct _RDT1R {
        using TIME = Field<RDT1R_Reg, 16, 16>; // TIME
        using FMI = Field<RDT1R_Reg, 8, 8>; // FMI
        using DLC = Field<RDT1R_Reg, 0, 4>; // DLC
    };

    // mailbox data high register
    using RDL1R_Reg = Register<CAN1_BASE + 0x1C8, ReadOnly>;
    struct _RDL1R {
        using DATA3 = Field<RDL1R_Reg, 24, 8>; // DATA3
        using DATA2 = Field<RDL1R_Reg, 16, 8>; // DATA2
        using DATA1 = Field<RDL1R_Reg, 8, 8>; // DATA1
        using DATA0 = Field<RDL1R_Reg, 0, 8>; // DATA0
    };

    // mailbox data high register
    using RDH1R_Reg = Register<CAN1_BASE + 0x1CC, ReadOnly>;
    struct _RDH1R {
        using DATA7 = Field<RDH1R_Reg, 24, 8>; // DATA7
        using DATA6 = Field<RDH1R_Reg, 16, 8>; // DATA6
        using DATA5 = Field<RDH1R_Reg, 8, 8>; // DATA5
        using DATA4 = Field<RDH1R_Reg, 0, 8>; // DATA4
    };

    // filter master register
    using FMR_Reg = Register<CAN1_BASE + 0x200, ReadWrite>;
    struct _FMR {
        using CAN2SB = Field<FMR_Reg, 8, 6>; // CAN2SB
        using FINIT = Field<FMR_Reg, 0, 1>; // FINIT
    };

    // filter mode register
    using FM1R_Reg = Register<CAN1_BASE + 0x204, ReadWrite>;
    struct _FM1R {
        using FBM0 = Field<FM1R_Reg, 0, 1>; // Filter mode
        using FBM1 = Field<FM1R_Reg, 1, 1>; // Filter mode
        using FBM2 = Field<FM1R_Reg, 2, 1>; // Filter mode
        using FBM3 = Field<FM1R_Reg, 3, 1>; // Filter mode
        using FBM4 = Field<FM1R_Reg, 4, 1>; // Filter mode
        using FBM5 = Field<FM1R_Reg, 5, 1>; // Filter mode
        using FBM6 = Field<FM1R_Reg, 6, 1>; // Filter mode
        using FBM7 = Field<FM1R_Reg, 7, 1>; // Filter mode
        using FBM8 = Field<FM1R_Reg, 8, 1>; // Filter mode
        using FBM9 = Field<FM1R_Reg, 9, 1>; // Filter mode
        using FBM10 = Field<FM1R_Reg, 10, 1>; // Filter mode
        using FBM11 = Field<FM1R_Reg, 11, 1>; // Filter mode
        using FBM12 = Field<FM1R_Reg, 12, 1>; // Filter mode
        using FBM13 = Field<FM1R_Reg, 13, 1>; // Filter mode
        using FBM14 = Field<FM1R_Reg, 14, 1>; // Filter mode
        using FBM15 = Field<FM1R_Reg, 15, 1>; // Filter mode
        using FBM16 = Field<FM1R_Reg, 16, 1>; // Filter mode
        using FBM17 = Field<FM1R_Reg, 17, 1>; // Filter mode
        using FBM18 = Field<FM1R_Reg, 18, 1>; // Filter mode
        using FBM19 = Field<FM1R_Reg, 19, 1>; // Filter mode
        using FBM20 = Field<FM1R_Reg, 20, 1>; // Filter mode
        using FBM21 = Field<FM1R_Reg, 21, 1>; // Filter mode
        using FBM22 = Field<FM1R_Reg, 22, 1>; // Filter mode
        using FBM23 = Field<FM1R_Reg, 23, 1>; // Filter mode
        using FBM24 = Field<FM1R_Reg, 24, 1>; // Filter mode
        using FBM25 = Field<FM1R_Reg, 25, 1>; // Filter mode
        using FBM26 = Field<FM1R_Reg, 26, 1>; // Filter mode
        using FBM27 = Field<FM1R_Reg, 27, 1>; // Filter mode
    };

    // filter scale register
    using FS1R_Reg = Register<CAN1_BASE + 0x20C, ReadWrite>;
    struct _FS1R {
        using FSC0 = Field<FS1R_Reg, 0, 1>; // Filter scale configuration
        using FSC1 = Field<FS1R_Reg, 1, 1>; // Filter scale configuration
        using FSC2 = Field<FS1R_Reg, 2, 1>; // Filter scale configuration
        using FSC3 = Field<FS1R_Reg, 3, 1>; // Filter scale configuration
        using FSC4 = Field<FS1R_Reg, 4, 1>; // Filter scale configuration
        using FSC5 = Field<FS1R_Reg, 5, 1>; // Filter scale configuration
        using FSC6 = Field<FS1R_Reg, 6, 1>; // Filter scale configuration
        using FSC7 = Field<FS1R_Reg, 7, 1>; // Filter scale configuration
        using FSC8 = Field<FS1R_Reg, 8, 1>; // Filter scale configuration
        using FSC9 = Field<FS1R_Reg, 9, 1>; // Filter scale configuration
        using FSC10 = Field<FS1R_Reg, 10, 1>; // Filter scale configuration
        using FSC11 = Field<FS1R_Reg, 11, 1>; // Filter scale configuration
        using FSC12 = Field<FS1R_Reg, 12, 1>; // Filter scale configuration
        using FSC13 = Field<FS1R_Reg, 13, 1>; // Filter scale configuration
        using FSC14 = Field<FS1R_Reg, 14, 1>; // Filter scale configuration
        using FSC15 = Field<FS1R_Reg, 15, 1>; // Filter scale configuration
        using FSC16 = Field<FS1R_Reg, 16, 1>; // Filter scale configuration
        using FSC17 = Field<FS1R_Reg, 17, 1>; // Filter scale configuration
        using FSC18 = Field<FS1R_Reg, 18, 1>; // Filter scale configuration
        using FSC19 = Field<FS1R_Reg, 19, 1>; // Filter scale configuration
        using FSC20 = Field<FS1R_Reg, 20, 1>; // Filter scale configuration
        using FSC21 = Field<FS1R_Reg, 21, 1>; // Filter scale configuration
        using FSC22 = Field<FS1R_Reg, 22, 1>; // Filter scale configuration
        using FSC23 = Field<FS1R_Reg, 23, 1>; // Filter scale configuration
        using FSC24 = Field<FS1R_Reg, 24, 1>; // Filter scale configuration
        using FSC25 = Field<FS1R_Reg, 25, 1>; // Filter scale configuration
        using FSC26 = Field<FS1R_Reg, 26, 1>; // Filter scale configuration
        using FSC27 = Field<FS1R_Reg, 27, 1>; // Filter scale configuration
    };

    // filter FIFO assignment register
    using FFA1R_Reg = Register<CAN1_BASE + 0x214, ReadWrite>;
    struct _FFA1R {
        using FFA0 = Field<FFA1R_Reg, 0, 1>; // Filter FIFO assignment for filter 0
        using FFA1 = Field<FFA1R_Reg, 1, 1>; // Filter FIFO assignment for filter 1
        using FFA2 = Field<FFA1R_Reg, 2, 1>; // Filter FIFO assignment for filter 2
        using FFA3 = Field<FFA1R_Reg, 3, 1>; // Filter FIFO assignment for filter 3
        using FFA4 = Field<FFA1R_Reg, 4, 1>; // Filter FIFO assignment for filter 4
        using FFA5 = Field<FFA1R_Reg, 5, 1>; // Filter FIFO assignment for filter 5
        using FFA6 = Field<FFA1R_Reg, 6, 1>; // Filter FIFO assignment for filter 6
        using FFA7 = Field<FFA1R_Reg, 7, 1>; // Filter FIFO assignment for filter 7
        using FFA8 = Field<FFA1R_Reg, 8, 1>; // Filter FIFO assignment for filter 8
        using FFA9 = Field<FFA1R_Reg, 9, 1>; // Filter FIFO assignment for filter 9
        using FFA10 = Field<FFA1R_Reg, 10, 1>; // Filter FIFO assignment for filter 10
        using FFA11 = Field<FFA1R_Reg, 11, 1>; // Filter FIFO assignment for filter 11
        using FFA12 = Field<FFA1R_Reg, 12, 1>; // Filter FIFO assignment for filter 12
        using FFA13 = Field<FFA1R_Reg, 13, 1>; // Filter FIFO assignment for filter 13
        using FFA14 = Field<FFA1R_Reg, 14, 1>; // Filter FIFO assignment for filter 14
        using FFA15 = Field<FFA1R_Reg, 15, 1>; // Filter FIFO assignment for filter 15
        using FFA16 = Field<FFA1R_Reg, 16, 1>; // Filter FIFO assignment for filter 16
        using FFA17 = Field<FFA1R_Reg, 17, 1>; // Filter FIFO assignment for filter 17
        using FFA18 = Field<FFA1R_Reg, 18, 1>; // Filter FIFO assignment for filter 18
        using FFA19 = Field<FFA1R_Reg, 19, 1>; // Filter FIFO assignment for filter 19
        using FFA20 = Field<FFA1R_Reg, 20, 1>; // Filter FIFO assignment for filter 20
        using FFA21 = Field<FFA1R_Reg, 21, 1>; // Filter FIFO assignment for filter 21
        using FFA22 = Field<FFA1R_Reg, 22, 1>; // Filter FIFO assignment for filter 22
        using FFA23 = Field<FFA1R_Reg, 23, 1>; // Filter FIFO assignment for filter 23
        using FFA24 = Field<FFA1R_Reg, 24, 1>; // Filter FIFO assignment for filter 24
        using FFA25 = Field<FFA1R_Reg, 25, 1>; // Filter FIFO assignment for filter 25
        using FFA26 = Field<FFA1R_Reg, 26, 1>; // Filter FIFO assignment for filter 26
        using FFA27 = Field<FFA1R_Reg, 27, 1>; // Filter FIFO assignment for filter 27
    };

    // filter activation register
    using FA1R_Reg = Register<CAN1_BASE + 0x21C, ReadWrite>;
    struct _FA1R {
        using FACT0 = Field<FA1R_Reg, 0, 1>; // Filter active
        using FACT1 = Field<FA1R_Reg, 1, 1>; // Filter active
        using FACT2 = Field<FA1R_Reg, 2, 1>; // Filter active
        using FACT3 = Field<FA1R_Reg, 3, 1>; // Filter active
        using FACT4 = Field<FA1R_Reg, 4, 1>; // Filter active
        using FACT5 = Field<FA1R_Reg, 5, 1>; // Filter active
        using FACT6 = Field<FA1R_Reg, 6, 1>; // Filter active
        using FACT7 = Field<FA1R_Reg, 7, 1>; // Filter active
        using FACT8 = Field<FA1R_Reg, 8, 1>; // Filter active
        using FACT9 = Field<FA1R_Reg, 9, 1>; // Filter active
        using FACT10 = Field<FA1R_Reg, 10, 1>; // Filter active
        using FACT11 = Field<FA1R_Reg, 11, 1>; // Filter active
        using FACT12 = Field<FA1R_Reg, 12, 1>; // Filter active
        using FACT13 = Field<FA1R_Reg, 13, 1>; // Filter active
        using FACT14 = Field<FA1R_Reg, 14, 1>; // Filter active
        using FACT15 = Field<FA1R_Reg, 15, 1>; // Filter active
        using FACT16 = Field<FA1R_Reg, 16, 1>; // Filter active
        using FACT17 = Field<FA1R_Reg, 17, 1>; // Filter active
        using FACT18 = Field<FA1R_Reg, 18, 1>; // Filter active
        using FACT19 = Field<FA1R_Reg, 19, 1>; // Filter active
        using FACT20 = Field<FA1R_Reg, 20, 1>; // Filter active
        using FACT21 = Field<FA1R_Reg, 21, 1>; // Filter active
        using FACT22 = Field<FA1R_Reg, 22, 1>; // Filter active
        using FACT23 = Field<FA1R_Reg, 23, 1>; // Filter active
        using FACT24 = Field<FA1R_Reg, 24, 1>; // Filter active
        using FACT25 = Field<FA1R_Reg, 25, 1>; // Filter active
        using FACT26 = Field<FA1R_Reg, 26, 1>; // Filter active
        using FACT27 = Field<FA1R_Reg, 27, 1>; // Filter active
    };

    // Filter bank 0 register 1
    using F0R1_Reg = Register<CAN1_BASE + 0x240, ReadWrite>;
    struct _F0R1 {
        using FB0 = Field<F0R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F0R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F0R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F0R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F0R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F0R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F0R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F0R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F0R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F0R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F0R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F0R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F0R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F0R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F0R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F0R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F0R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F0R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F0R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F0R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F0R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F0R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F0R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F0R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F0R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F0R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F0R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F0R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F0R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F0R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F0R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F0R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 0 register 2
    using F0R2_Reg = Register<CAN1_BASE + 0x244, ReadWrite>;
    struct _F0R2 {
        using FB0 = Field<F0R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F0R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F0R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F0R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F0R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F0R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F0R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F0R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F0R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F0R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F0R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F0R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F0R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F0R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F0R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F0R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F0R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F0R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F0R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F0R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F0R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F0R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F0R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F0R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F0R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F0R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F0R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F0R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F0R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F0R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F0R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F0R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 1 register 1
    using F1R1_Reg = Register<CAN1_BASE + 0x248, ReadWrite>;
    struct _F1R1 {
        using FB0 = Field<F1R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F1R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F1R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F1R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F1R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F1R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F1R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F1R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F1R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F1R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F1R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F1R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F1R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F1R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F1R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F1R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F1R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F1R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F1R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F1R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F1R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F1R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F1R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F1R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F1R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F1R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F1R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F1R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F1R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F1R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F1R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F1R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 1 register 2
    using F1R2_Reg = Register<CAN1_BASE + 0x24C, ReadWrite>;
    struct _F1R2 {
        using FB0 = Field<F1R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F1R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F1R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F1R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F1R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F1R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F1R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F1R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F1R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F1R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F1R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F1R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F1R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F1R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F1R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F1R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F1R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F1R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F1R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F1R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F1R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F1R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F1R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F1R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F1R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F1R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F1R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F1R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F1R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F1R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F1R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F1R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 2 register 1
    using F2R1_Reg = Register<CAN1_BASE + 0x250, ReadWrite>;
    struct _F2R1 {
        using FB0 = Field<F2R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F2R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F2R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F2R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F2R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F2R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F2R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F2R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F2R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F2R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F2R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F2R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F2R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F2R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F2R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F2R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F2R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F2R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F2R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F2R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F2R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F2R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F2R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F2R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F2R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F2R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F2R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F2R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F2R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F2R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F2R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F2R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 2 register 2
    using F2R2_Reg = Register<CAN1_BASE + 0x254, ReadWrite>;
    struct _F2R2 {
        using FB0 = Field<F2R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F2R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F2R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F2R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F2R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F2R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F2R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F2R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F2R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F2R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F2R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F2R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F2R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F2R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F2R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F2R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F2R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F2R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F2R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F2R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F2R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F2R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F2R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F2R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F2R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F2R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F2R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F2R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F2R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F2R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F2R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F2R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 3 register 1
    using F3R1_Reg = Register<CAN1_BASE + 0x258, ReadWrite>;
    struct _F3R1 {
        using FB0 = Field<F3R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F3R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F3R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F3R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F3R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F3R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F3R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F3R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F3R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F3R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F3R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F3R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F3R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F3R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F3R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F3R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F3R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F3R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F3R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F3R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F3R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F3R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F3R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F3R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F3R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F3R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F3R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F3R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F3R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F3R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F3R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F3R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 3 register 2
    using F3R2_Reg = Register<CAN1_BASE + 0x25C, ReadWrite>;
    struct _F3R2 {
        using FB0 = Field<F3R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F3R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F3R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F3R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F3R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F3R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F3R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F3R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F3R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F3R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F3R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F3R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F3R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F3R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F3R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F3R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F3R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F3R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F3R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F3R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F3R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F3R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F3R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F3R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F3R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F3R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F3R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F3R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F3R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F3R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F3R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F3R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 4 register 1
    using F4R1_Reg = Register<CAN1_BASE + 0x260, ReadWrite>;
    struct _F4R1 {
        using FB0 = Field<F4R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F4R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F4R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F4R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F4R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F4R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F4R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F4R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F4R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F4R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F4R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F4R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F4R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F4R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F4R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F4R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F4R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F4R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F4R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F4R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F4R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F4R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F4R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F4R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F4R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F4R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F4R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F4R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F4R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F4R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F4R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F4R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 4 register 2
    using F4R2_Reg = Register<CAN1_BASE + 0x264, ReadWrite>;
    struct _F4R2 {
        using FB0 = Field<F4R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F4R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F4R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F4R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F4R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F4R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F4R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F4R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F4R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F4R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F4R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F4R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F4R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F4R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F4R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F4R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F4R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F4R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F4R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F4R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F4R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F4R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F4R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F4R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F4R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F4R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F4R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F4R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F4R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F4R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F4R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F4R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 5 register 1
    using F5R1_Reg = Register<CAN1_BASE + 0x268, ReadWrite>;
    struct _F5R1 {
        using FB0 = Field<F5R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F5R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F5R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F5R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F5R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F5R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F5R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F5R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F5R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F5R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F5R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F5R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F5R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F5R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F5R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F5R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F5R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F5R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F5R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F5R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F5R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F5R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F5R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F5R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F5R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F5R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F5R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F5R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F5R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F5R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F5R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F5R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 5 register 2
    using F5R2_Reg = Register<CAN1_BASE + 0x26C, ReadWrite>;
    struct _F5R2 {
        using FB0 = Field<F5R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F5R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F5R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F5R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F5R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F5R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F5R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F5R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F5R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F5R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F5R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F5R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F5R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F5R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F5R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F5R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F5R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F5R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F5R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F5R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F5R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F5R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F5R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F5R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F5R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F5R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F5R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F5R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F5R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F5R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F5R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F5R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 6 register 1
    using F6R1_Reg = Register<CAN1_BASE + 0x270, ReadWrite>;
    struct _F6R1 {
        using FB0 = Field<F6R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F6R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F6R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F6R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F6R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F6R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F6R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F6R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F6R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F6R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F6R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F6R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F6R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F6R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F6R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F6R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F6R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F6R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F6R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F6R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F6R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F6R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F6R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F6R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F6R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F6R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F6R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F6R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F6R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F6R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F6R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F6R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 6 register 2
    using F6R2_Reg = Register<CAN1_BASE + 0x274, ReadWrite>;
    struct _F6R2 {
        using FB0 = Field<F6R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F6R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F6R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F6R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F6R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F6R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F6R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F6R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F6R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F6R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F6R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F6R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F6R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F6R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F6R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F6R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F6R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F6R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F6R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F6R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F6R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F6R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F6R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F6R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F6R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F6R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F6R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F6R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F6R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F6R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F6R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F6R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 7 register 1
    using F7R1_Reg = Register<CAN1_BASE + 0x278, ReadWrite>;
    struct _F7R1 {
        using FB0 = Field<F7R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F7R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F7R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F7R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F7R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F7R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F7R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F7R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F7R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F7R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F7R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F7R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F7R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F7R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F7R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F7R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F7R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F7R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F7R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F7R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F7R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F7R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F7R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F7R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F7R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F7R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F7R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F7R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F7R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F7R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F7R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F7R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 7 register 2
    using F7R2_Reg = Register<CAN1_BASE + 0x27C, ReadWrite>;
    struct _F7R2 {
        using FB0 = Field<F7R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F7R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F7R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F7R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F7R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F7R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F7R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F7R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F7R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F7R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F7R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F7R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F7R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F7R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F7R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F7R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F7R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F7R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F7R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F7R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F7R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F7R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F7R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F7R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F7R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F7R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F7R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F7R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F7R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F7R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F7R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F7R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 8 register 1
    using F8R1_Reg = Register<CAN1_BASE + 0x280, ReadWrite>;
    struct _F8R1 {
        using FB0 = Field<F8R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F8R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F8R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F8R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F8R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F8R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F8R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F8R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F8R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F8R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F8R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F8R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F8R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F8R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F8R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F8R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F8R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F8R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F8R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F8R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F8R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F8R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F8R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F8R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F8R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F8R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F8R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F8R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F8R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F8R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F8R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F8R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 8 register 2
    using F8R2_Reg = Register<CAN1_BASE + 0x284, ReadWrite>;
    struct _F8R2 {
        using FB0 = Field<F8R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F8R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F8R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F8R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F8R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F8R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F8R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F8R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F8R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F8R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F8R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F8R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F8R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F8R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F8R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F8R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F8R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F8R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F8R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F8R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F8R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F8R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F8R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F8R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F8R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F8R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F8R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F8R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F8R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F8R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F8R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F8R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 9 register 1
    using F9R1_Reg = Register<CAN1_BASE + 0x288, ReadWrite>;
    struct _F9R1 {
        using FB0 = Field<F9R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F9R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F9R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F9R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F9R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F9R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F9R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F9R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F9R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F9R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F9R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F9R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F9R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F9R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F9R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F9R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F9R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F9R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F9R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F9R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F9R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F9R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F9R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F9R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F9R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F9R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F9R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F9R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F9R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F9R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F9R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F9R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 9 register 2
    using F9R2_Reg = Register<CAN1_BASE + 0x28C, ReadWrite>;
    struct _F9R2 {
        using FB0 = Field<F9R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F9R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F9R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F9R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F9R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F9R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F9R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F9R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F9R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F9R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F9R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F9R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F9R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F9R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F9R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F9R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F9R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F9R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F9R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F9R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F9R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F9R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F9R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F9R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F9R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F9R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F9R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F9R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F9R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F9R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F9R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F9R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 10 register 1
    using F10R1_Reg = Register<CAN1_BASE + 0x290, ReadWrite>;
    struct _F10R1 {
        using FB0 = Field<F10R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F10R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F10R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F10R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F10R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F10R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F10R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F10R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F10R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F10R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F10R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F10R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F10R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F10R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F10R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F10R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F10R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F10R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F10R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F10R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F10R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F10R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F10R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F10R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F10R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F10R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F10R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F10R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F10R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F10R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F10R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F10R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 10 register 2
    using F10R2_Reg = Register<CAN1_BASE + 0x294, ReadWrite>;
    struct _F10R2 {
        using FB0 = Field<F10R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F10R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F10R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F10R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F10R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F10R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F10R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F10R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F10R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F10R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F10R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F10R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F10R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F10R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F10R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F10R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F10R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F10R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F10R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F10R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F10R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F10R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F10R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F10R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F10R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F10R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F10R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F10R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F10R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F10R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F10R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F10R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 11 register 1
    using F11R1_Reg = Register<CAN1_BASE + 0x298, ReadWrite>;
    struct _F11R1 {
        using FB0 = Field<F11R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F11R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F11R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F11R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F11R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F11R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F11R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F11R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F11R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F11R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F11R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F11R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F11R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F11R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F11R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F11R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F11R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F11R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F11R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F11R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F11R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F11R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F11R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F11R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F11R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F11R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F11R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F11R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F11R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F11R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F11R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F11R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 11 register 2
    using F11R2_Reg = Register<CAN1_BASE + 0x29C, ReadWrite>;
    struct _F11R2 {
        using FB0 = Field<F11R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F11R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F11R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F11R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F11R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F11R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F11R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F11R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F11R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F11R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F11R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F11R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F11R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F11R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F11R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F11R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F11R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F11R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F11R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F11R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F11R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F11R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F11R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F11R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F11R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F11R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F11R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F11R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F11R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F11R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F11R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F11R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 4 register 1
    using F12R1_Reg = Register<CAN1_BASE + 0x2A0, ReadWrite>;
    struct _F12R1 {
        using FB0 = Field<F12R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F12R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F12R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F12R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F12R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F12R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F12R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F12R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F12R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F12R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F12R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F12R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F12R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F12R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F12R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F12R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F12R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F12R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F12R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F12R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F12R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F12R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F12R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F12R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F12R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F12R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F12R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F12R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F12R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F12R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F12R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F12R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 12 register 2
    using F12R2_Reg = Register<CAN1_BASE + 0x2A4, ReadWrite>;
    struct _F12R2 {
        using FB0 = Field<F12R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F12R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F12R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F12R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F12R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F12R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F12R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F12R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F12R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F12R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F12R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F12R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F12R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F12R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F12R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F12R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F12R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F12R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F12R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F12R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F12R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F12R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F12R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F12R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F12R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F12R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F12R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F12R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F12R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F12R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F12R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F12R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 13 register 1
    using F13R1_Reg = Register<CAN1_BASE + 0x2A8, ReadWrite>;
    struct _F13R1 {
        using FB0 = Field<F13R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F13R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F13R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F13R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F13R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F13R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F13R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F13R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F13R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F13R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F13R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F13R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F13R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F13R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F13R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F13R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F13R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F13R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F13R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F13R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F13R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F13R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F13R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F13R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F13R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F13R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F13R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F13R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F13R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F13R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F13R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F13R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 13 register 2
    using F13R2_Reg = Register<CAN1_BASE + 0x2AC, ReadWrite>;
    struct _F13R2 {
        using FB0 = Field<F13R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F13R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F13R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F13R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F13R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F13R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F13R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F13R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F13R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F13R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F13R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F13R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F13R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F13R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F13R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F13R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F13R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F13R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F13R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F13R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F13R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F13R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F13R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F13R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F13R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F13R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F13R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F13R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F13R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F13R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F13R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F13R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 14 register 1
    using F14R1_Reg = Register<CAN1_BASE + 0x2B0, ReadWrite>;
    struct _F14R1 {
        using FB0 = Field<F14R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F14R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F14R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F14R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F14R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F14R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F14R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F14R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F14R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F14R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F14R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F14R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F14R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F14R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F14R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F14R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F14R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F14R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F14R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F14R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F14R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F14R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F14R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F14R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F14R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F14R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F14R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F14R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F14R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F14R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F14R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F14R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 14 register 2
    using F14R2_Reg = Register<CAN1_BASE + 0x2B4, ReadWrite>;
    struct _F14R2 {
        using FB0 = Field<F14R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F14R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F14R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F14R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F14R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F14R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F14R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F14R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F14R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F14R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F14R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F14R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F14R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F14R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F14R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F14R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F14R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F14R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F14R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F14R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F14R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F14R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F14R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F14R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F14R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F14R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F14R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F14R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F14R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F14R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F14R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F14R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 15 register 1
    using F15R1_Reg = Register<CAN1_BASE + 0x2B8, ReadWrite>;
    struct _F15R1 {
        using FB0 = Field<F15R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F15R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F15R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F15R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F15R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F15R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F15R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F15R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F15R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F15R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F15R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F15R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F15R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F15R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F15R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F15R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F15R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F15R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F15R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F15R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F15R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F15R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F15R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F15R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F15R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F15R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F15R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F15R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F15R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F15R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F15R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F15R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 15 register 2
    using F15R2_Reg = Register<CAN1_BASE + 0x2BC, ReadWrite>;
    struct _F15R2 {
        using FB0 = Field<F15R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F15R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F15R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F15R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F15R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F15R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F15R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F15R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F15R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F15R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F15R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F15R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F15R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F15R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F15R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F15R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F15R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F15R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F15R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F15R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F15R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F15R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F15R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F15R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F15R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F15R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F15R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F15R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F15R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F15R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F15R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F15R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 16 register 1
    using F16R1_Reg = Register<CAN1_BASE + 0x2C0, ReadWrite>;
    struct _F16R1 {
        using FB0 = Field<F16R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F16R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F16R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F16R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F16R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F16R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F16R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F16R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F16R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F16R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F16R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F16R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F16R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F16R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F16R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F16R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F16R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F16R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F16R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F16R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F16R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F16R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F16R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F16R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F16R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F16R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F16R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F16R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F16R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F16R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F16R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F16R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 16 register 2
    using F16R2_Reg = Register<CAN1_BASE + 0x2C4, ReadWrite>;
    struct _F16R2 {
        using FB0 = Field<F16R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F16R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F16R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F16R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F16R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F16R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F16R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F16R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F16R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F16R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F16R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F16R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F16R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F16R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F16R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F16R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F16R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F16R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F16R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F16R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F16R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F16R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F16R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F16R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F16R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F16R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F16R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F16R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F16R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F16R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F16R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F16R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 17 register 1
    using F17R1_Reg = Register<CAN1_BASE + 0x2C8, ReadWrite>;
    struct _F17R1 {
        using FB0 = Field<F17R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F17R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F17R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F17R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F17R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F17R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F17R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F17R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F17R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F17R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F17R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F17R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F17R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F17R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F17R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F17R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F17R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F17R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F17R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F17R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F17R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F17R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F17R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F17R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F17R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F17R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F17R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F17R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F17R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F17R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F17R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F17R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 17 register 2
    using F17R2_Reg = Register<CAN1_BASE + 0x2CC, ReadWrite>;
    struct _F17R2 {
        using FB0 = Field<F17R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F17R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F17R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F17R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F17R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F17R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F17R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F17R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F17R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F17R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F17R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F17R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F17R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F17R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F17R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F17R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F17R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F17R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F17R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F17R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F17R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F17R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F17R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F17R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F17R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F17R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F17R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F17R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F17R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F17R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F17R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F17R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 18 register 1
    using F18R1_Reg = Register<CAN1_BASE + 0x2D0, ReadWrite>;
    struct _F18R1 {
        using FB0 = Field<F18R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F18R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F18R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F18R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F18R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F18R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F18R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F18R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F18R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F18R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F18R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F18R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F18R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F18R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F18R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F18R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F18R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F18R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F18R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F18R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F18R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F18R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F18R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F18R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F18R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F18R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F18R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F18R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F18R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F18R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F18R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F18R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 18 register 2
    using F18R2_Reg = Register<CAN1_BASE + 0x2D4, ReadWrite>;
    struct _F18R2 {
        using FB0 = Field<F18R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F18R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F18R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F18R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F18R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F18R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F18R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F18R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F18R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F18R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F18R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F18R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F18R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F18R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F18R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F18R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F18R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F18R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F18R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F18R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F18R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F18R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F18R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F18R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F18R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F18R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F18R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F18R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F18R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F18R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F18R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F18R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 19 register 1
    using F19R1_Reg = Register<CAN1_BASE + 0x2D8, ReadWrite>;
    struct _F19R1 {
        using FB0 = Field<F19R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F19R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F19R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F19R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F19R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F19R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F19R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F19R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F19R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F19R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F19R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F19R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F19R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F19R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F19R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F19R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F19R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F19R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F19R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F19R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F19R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F19R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F19R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F19R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F19R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F19R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F19R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F19R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F19R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F19R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F19R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F19R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 19 register 2
    using F19R2_Reg = Register<CAN1_BASE + 0x2DC, ReadWrite>;
    struct _F19R2 {
        using FB0 = Field<F19R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F19R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F19R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F19R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F19R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F19R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F19R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F19R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F19R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F19R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F19R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F19R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F19R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F19R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F19R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F19R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F19R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F19R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F19R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F19R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F19R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F19R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F19R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F19R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F19R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F19R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F19R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F19R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F19R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F19R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F19R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F19R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 20 register 1
    using F20R1_Reg = Register<CAN1_BASE + 0x2E0, ReadWrite>;
    struct _F20R1 {
        using FB0 = Field<F20R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F20R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F20R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F20R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F20R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F20R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F20R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F20R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F20R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F20R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F20R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F20R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F20R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F20R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F20R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F20R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F20R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F20R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F20R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F20R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F20R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F20R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F20R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F20R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F20R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F20R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F20R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F20R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F20R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F20R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F20R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F20R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 20 register 2
    using F20R2_Reg = Register<CAN1_BASE + 0x2E4, ReadWrite>;
    struct _F20R2 {
        using FB0 = Field<F20R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F20R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F20R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F20R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F20R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F20R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F20R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F20R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F20R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F20R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F20R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F20R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F20R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F20R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F20R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F20R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F20R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F20R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F20R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F20R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F20R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F20R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F20R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F20R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F20R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F20R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F20R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F20R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F20R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F20R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F20R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F20R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 21 register 1
    using F21R1_Reg = Register<CAN1_BASE + 0x2E8, ReadWrite>;
    struct _F21R1 {
        using FB0 = Field<F21R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F21R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F21R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F21R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F21R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F21R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F21R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F21R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F21R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F21R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F21R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F21R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F21R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F21R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F21R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F21R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F21R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F21R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F21R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F21R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F21R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F21R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F21R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F21R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F21R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F21R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F21R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F21R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F21R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F21R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F21R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F21R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 21 register 2
    using F21R2_Reg = Register<CAN1_BASE + 0x2EC, ReadWrite>;
    struct _F21R2 {
        using FB0 = Field<F21R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F21R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F21R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F21R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F21R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F21R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F21R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F21R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F21R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F21R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F21R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F21R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F21R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F21R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F21R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F21R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F21R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F21R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F21R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F21R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F21R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F21R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F21R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F21R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F21R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F21R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F21R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F21R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F21R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F21R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F21R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F21R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 22 register 1
    using F22R1_Reg = Register<CAN1_BASE + 0x2F0, ReadWrite>;
    struct _F22R1 {
        using FB0 = Field<F22R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F22R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F22R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F22R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F22R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F22R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F22R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F22R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F22R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F22R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F22R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F22R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F22R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F22R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F22R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F22R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F22R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F22R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F22R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F22R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F22R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F22R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F22R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F22R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F22R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F22R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F22R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F22R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F22R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F22R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F22R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F22R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 22 register 2
    using F22R2_Reg = Register<CAN1_BASE + 0x2F4, ReadWrite>;
    struct _F22R2 {
        using FB0 = Field<F22R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F22R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F22R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F22R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F22R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F22R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F22R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F22R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F22R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F22R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F22R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F22R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F22R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F22R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F22R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F22R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F22R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F22R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F22R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F22R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F22R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F22R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F22R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F22R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F22R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F22R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F22R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F22R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F22R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F22R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F22R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F22R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 23 register 1
    using F23R1_Reg = Register<CAN1_BASE + 0x2F8, ReadWrite>;
    struct _F23R1 {
        using FB0 = Field<F23R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F23R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F23R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F23R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F23R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F23R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F23R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F23R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F23R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F23R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F23R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F23R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F23R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F23R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F23R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F23R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F23R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F23R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F23R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F23R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F23R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F23R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F23R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F23R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F23R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F23R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F23R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F23R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F23R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F23R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F23R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F23R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 23 register 2
    using F23R2_Reg = Register<CAN1_BASE + 0x2FC, ReadWrite>;
    struct _F23R2 {
        using FB0 = Field<F23R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F23R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F23R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F23R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F23R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F23R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F23R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F23R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F23R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F23R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F23R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F23R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F23R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F23R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F23R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F23R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F23R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F23R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F23R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F23R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F23R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F23R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F23R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F23R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F23R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F23R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F23R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F23R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F23R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F23R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F23R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F23R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 24 register 1
    using F24R1_Reg = Register<CAN1_BASE + 0x300, ReadWrite>;
    struct _F24R1 {
        using FB0 = Field<F24R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F24R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F24R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F24R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F24R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F24R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F24R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F24R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F24R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F24R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F24R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F24R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F24R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F24R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F24R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F24R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F24R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F24R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F24R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F24R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F24R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F24R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F24R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F24R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F24R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F24R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F24R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F24R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F24R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F24R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F24R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F24R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 24 register 2
    using F24R2_Reg = Register<CAN1_BASE + 0x304, ReadWrite>;
    struct _F24R2 {
        using FB0 = Field<F24R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F24R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F24R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F24R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F24R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F24R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F24R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F24R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F24R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F24R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F24R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F24R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F24R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F24R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F24R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F24R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F24R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F24R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F24R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F24R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F24R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F24R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F24R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F24R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F24R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F24R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F24R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F24R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F24R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F24R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F24R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F24R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 25 register 1
    using F25R1_Reg = Register<CAN1_BASE + 0x308, ReadWrite>;
    struct _F25R1 {
        using FB0 = Field<F25R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F25R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F25R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F25R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F25R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F25R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F25R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F25R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F25R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F25R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F25R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F25R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F25R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F25R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F25R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F25R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F25R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F25R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F25R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F25R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F25R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F25R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F25R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F25R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F25R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F25R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F25R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F25R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F25R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F25R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F25R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F25R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 25 register 2
    using F25R2_Reg = Register<CAN1_BASE + 0x30C, ReadWrite>;
    struct _F25R2 {
        using FB0 = Field<F25R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F25R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F25R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F25R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F25R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F25R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F25R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F25R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F25R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F25R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F25R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F25R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F25R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F25R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F25R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F25R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F25R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F25R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F25R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F25R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F25R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F25R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F25R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F25R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F25R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F25R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F25R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F25R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F25R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F25R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F25R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F25R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 26 register 1
    using F26R1_Reg = Register<CAN1_BASE + 0x310, ReadWrite>;
    struct _F26R1 {
        using FB0 = Field<F26R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F26R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F26R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F26R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F26R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F26R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F26R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F26R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F26R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F26R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F26R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F26R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F26R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F26R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F26R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F26R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F26R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F26R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F26R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F26R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F26R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F26R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F26R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F26R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F26R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F26R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F26R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F26R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F26R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F26R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F26R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F26R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 26 register 2
    using F26R2_Reg = Register<CAN1_BASE + 0x314, ReadWrite>;
    struct _F26R2 {
        using FB0 = Field<F26R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F26R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F26R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F26R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F26R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F26R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F26R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F26R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F26R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F26R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F26R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F26R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F26R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F26R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F26R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F26R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F26R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F26R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F26R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F26R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F26R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F26R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F26R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F26R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F26R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F26R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F26R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F26R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F26R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F26R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F26R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F26R2_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 27 register 1
    using F27R1_Reg = Register<CAN1_BASE + 0x318, ReadWrite>;
    struct _F27R1 {
        using FB0 = Field<F27R1_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F27R1_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F27R1_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F27R1_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F27R1_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F27R1_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F27R1_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F27R1_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F27R1_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F27R1_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F27R1_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F27R1_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F27R1_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F27R1_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F27R1_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F27R1_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F27R1_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F27R1_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F27R1_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F27R1_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F27R1_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F27R1_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F27R1_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F27R1_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F27R1_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F27R1_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F27R1_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F27R1_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F27R1_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F27R1_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F27R1_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F27R1_Reg, 31, 1>; // Filter bits
    };

    // Filter bank 27 register 2
    using F27R2_Reg = Register<CAN1_BASE + 0x31C, ReadWrite>;
    struct _F27R2 {
        using FB0 = Field<F27R2_Reg, 0, 1>; // Filter bits
        using FB1 = Field<F27R2_Reg, 1, 1>; // Filter bits
        using FB2 = Field<F27R2_Reg, 2, 1>; // Filter bits
        using FB3 = Field<F27R2_Reg, 3, 1>; // Filter bits
        using FB4 = Field<F27R2_Reg, 4, 1>; // Filter bits
        using FB5 = Field<F27R2_Reg, 5, 1>; // Filter bits
        using FB6 = Field<F27R2_Reg, 6, 1>; // Filter bits
        using FB7 = Field<F27R2_Reg, 7, 1>; // Filter bits
        using FB8 = Field<F27R2_Reg, 8, 1>; // Filter bits
        using FB9 = Field<F27R2_Reg, 9, 1>; // Filter bits
        using FB10 = Field<F27R2_Reg, 10, 1>; // Filter bits
        using FB11 = Field<F27R2_Reg, 11, 1>; // Filter bits
        using FB12 = Field<F27R2_Reg, 12, 1>; // Filter bits
        using FB13 = Field<F27R2_Reg, 13, 1>; // Filter bits
        using FB14 = Field<F27R2_Reg, 14, 1>; // Filter bits
        using FB15 = Field<F27R2_Reg, 15, 1>; // Filter bits
        using FB16 = Field<F27R2_Reg, 16, 1>; // Filter bits
        using FB17 = Field<F27R2_Reg, 17, 1>; // Filter bits
        using FB18 = Field<F27R2_Reg, 18, 1>; // Filter bits
        using FB19 = Field<F27R2_Reg, 19, 1>; // Filter bits
        using FB20 = Field<F27R2_Reg, 20, 1>; // Filter bits
        using FB21 = Field<F27R2_Reg, 21, 1>; // Filter bits
        using FB22 = Field<F27R2_Reg, 22, 1>; // Filter bits
        using FB23 = Field<F27R2_Reg, 23, 1>; // Filter bits
        using FB24 = Field<F27R2_Reg, 24, 1>; // Filter bits
        using FB25 = Field<F27R2_Reg, 25, 1>; // Filter bits
        using FB26 = Field<F27R2_Reg, 26, 1>; // Filter bits
        using FB27 = Field<F27R2_Reg, 27, 1>; // Filter bits
        using FB28 = Field<F27R2_Reg, 28, 1>; // Filter bits
        using FB29 = Field<F27R2_Reg, 29, 1>; // Filter bits
        using FB30 = Field<F27R2_Reg, 30, 1>; // Filter bits
        using FB31 = Field<F27R2_Reg, 31, 1>; // Filter bits
    };

} // namespace CAN1

// --------------------------------------------
// CAN2: 
// Base address: 0x40006800
// --------------------------------------------

namespace CAN2 {
    static constexpr uint32_t CAN2_BASE = 0x40006800;

} // namespace CAN2

// --------------------------------------------
// CAN3: 
// Base address: 0x40003400
// --------------------------------------------

namespace CAN3 {
    static constexpr uint32_t CAN3_BASE = 0x40003400;

} // namespace CAN3

// --------------------------------------------
// Flash: FLASH
// Base address: 0x40023C00
// --------------------------------------------

namespace Flash {
    static constexpr uint32_t Flash_BASE = 0x40023C00;

    // Flash access control register
    using ACR_Reg = Register<Flash_BASE + 0x0, ReadWrite>;
    struct _ACR {
        using LATENCY = Field<ACR_Reg, 0, 4>; // Latency
        using PRFTEN = Field<ACR_Reg, 8, 1>; // Prefetch enable
        using ARTEN = Field<ACR_Reg, 9, 1>; // ART Accelerator Enable
        using ARTRST = Field<ACR_Reg, 11, 1>; // ART Accelerator reset
    };

    // Flash key register
    using KEYR_Reg = Register<Flash_BASE + 0x4, WriteOnly>;
    struct _KEYR {
        using KEY = Field<KEYR_Reg, 0, 32>; // FPEC key
    };

    // Flash option key register
    using OPTKEYR_Reg = Register<Flash_BASE + 0x8, WriteOnly>;
    struct _OPTKEYR {
        using OPTKEY = Field<OPTKEYR_Reg, 0, 32>; // Option byte key
    };

    // Status register
    using SR_Reg = Register<Flash_BASE + 0xC, ReadWrite>;
    struct _SR {
        using EOP = Field<SR_Reg, 0, 1>; // End of operation
        using OPERR = Field<SR_Reg, 1, 1>; // Operation error
        using WRPERR = Field<SR_Reg, 4, 1>; // Write protection error
        using PGAERR = Field<SR_Reg, 5, 1>; // Programming alignment error
        using PGPERR = Field<SR_Reg, 6, 1>; // Programming parallelism error
        using PGSERR = Field<SR_Reg, 7, 1>; // Programming sequence error
        using BSY = Field<SR_Reg, 16, 1>; // Busy
    };

    // Control register
    using CR_Reg = Register<Flash_BASE + 0x10, ReadWrite>;
    struct _CR {
        using PG = Field<CR_Reg, 0, 1>; // Programming
        using SER = Field<CR_Reg, 1, 1>; // Sector Erase
        using MER = Field<CR_Reg, 2, 1>; // Mass Erase of sectors 0 to 11
        using SNB = Field<CR_Reg, 3, 5>; // Sector number
        using PSIZE = Field<CR_Reg, 8, 2>; // Program size
        using MER1 = Field<CR_Reg, 15, 1>; // Mass Erase of sectors 12 to 23
        using STRT = Field<CR_Reg, 16, 1>; // Start
        using EOPIE = Field<CR_Reg, 24, 1>; // End of operation interrupt enable
        using ERRIE = Field<CR_Reg, 25, 1>; // Error interrupt enable
        using LOCK = Field<CR_Reg, 31, 1>; // Lock
    };

    // Flash option control register
    using OPTCR_Reg = Register<Flash_BASE + 0x14, ReadWrite>;
    struct _OPTCR {
        using OPTLOCK = Field<OPTCR_Reg, 0, 1>; // Option lock
        using OPTSTRT = Field<OPTCR_Reg, 1, 1>; // Option start
        using BOR_LEV = Field<OPTCR_Reg, 2, 2>; // BOR reset Level
        using WWDG_SW = Field<OPTCR_Reg, 4, 1>; // User option bytes
        using IWDG_SW = Field<OPTCR_Reg, 5, 1>; // User option bytes
        using nRST_STOP = Field<OPTCR_Reg, 6, 1>; // User option bytes
        using nRST_STDBY = Field<OPTCR_Reg, 7, 1>; // User option bytes
        using RDP = Field<OPTCR_Reg, 8, 8>; // Read protect
        using nWRP = Field<OPTCR_Reg, 16, 12>; // Not write protect
        using nDBOOT = Field<OPTCR_Reg, 28, 1>; // Dual Boot mode (valid only when nDBANK=0)
        using nDBANK = Field<OPTCR_Reg, 29, 1>; // Not dual bank mode
        using IWDG_STDBY = Field<OPTCR_Reg, 30, 1>; // Independent watchdog counter freeze in standby mode
        using IWDG_STOP = Field<OPTCR_Reg, 31, 1>; // Independent watchdog counter freeze in Stop mode
    };

    // Flash option control register 1
    using OPTCR1_Reg = Register<Flash_BASE + 0x18, ReadWrite>;
    struct _OPTCR1 {
        using BOOT_ADD0 = Field<OPTCR1_Reg, 0, 16>; // Boot base address when Boot pin =0
        using BOOT_ADD1 = Field<OPTCR1_Reg, 16, 16>; // Boot base address when Boot pin =1
    };

} // namespace Flash

// --------------------------------------------
// EXTI: External interrupt/event controller
// Base address: 0x40013C00
// --------------------------------------------

namespace EXTI {
    static constexpr uint32_t EXTI_BASE = 0x40013C00;

    // Interrupt mask register (EXTI_IMR)
    using IMR_Reg = Register<EXTI_BASE + 0x0, ReadWrite>;
    struct _IMR {
        using MR0 = Field<IMR_Reg, 0, 1>; // Interrupt Mask on line 0
        using MR1 = Field<IMR_Reg, 1, 1>; // Interrupt Mask on line 1
        using MR2 = Field<IMR_Reg, 2, 1>; // Interrupt Mask on line 2
        using MR3 = Field<IMR_Reg, 3, 1>; // Interrupt Mask on line 3
        using MR4 = Field<IMR_Reg, 4, 1>; // Interrupt Mask on line 4
        using MR5 = Field<IMR_Reg, 5, 1>; // Interrupt Mask on line 5
        using MR6 = Field<IMR_Reg, 6, 1>; // Interrupt Mask on line 6
        using MR7 = Field<IMR_Reg, 7, 1>; // Interrupt Mask on line 7
        using MR8 = Field<IMR_Reg, 8, 1>; // Interrupt Mask on line 8
        using MR9 = Field<IMR_Reg, 9, 1>; // Interrupt Mask on line 9
        using MR10 = Field<IMR_Reg, 10, 1>; // Interrupt Mask on line 10
        using MR11 = Field<IMR_Reg, 11, 1>; // Interrupt Mask on line 11
        using MR12 = Field<IMR_Reg, 12, 1>; // Interrupt Mask on line 12
        using MR13 = Field<IMR_Reg, 13, 1>; // Interrupt Mask on line 13
        using MR14 = Field<IMR_Reg, 14, 1>; // Interrupt Mask on line 14
        using MR15 = Field<IMR_Reg, 15, 1>; // Interrupt Mask on line 15
        using MR16 = Field<IMR_Reg, 16, 1>; // Interrupt Mask on line 16
        using MR17 = Field<IMR_Reg, 17, 1>; // Interrupt Mask on line 17
        using MR18 = Field<IMR_Reg, 18, 1>; // Interrupt Mask on line 18
        using MR19 = Field<IMR_Reg, 19, 1>; // Interrupt Mask on line 19
        using MR20 = Field<IMR_Reg, 20, 1>; // Interrupt Mask on line 20
        using MR21 = Field<IMR_Reg, 21, 1>; // Interrupt Mask on line 21
        using MR22 = Field<IMR_Reg, 22, 1>; // Interrupt Mask on line 22
    };

    // Event mask register (EXTI_EMR)
    using EMR_Reg = Register<EXTI_BASE + 0x4, ReadWrite>;
    struct _EMR {
        using MR0 = Field<EMR_Reg, 0, 1>; // Event Mask on line 0
        using MR1 = Field<EMR_Reg, 1, 1>; // Event Mask on line 1
        using MR2 = Field<EMR_Reg, 2, 1>; // Event Mask on line 2
        using MR3 = Field<EMR_Reg, 3, 1>; // Event Mask on line 3
        using MR4 = Field<EMR_Reg, 4, 1>; // Event Mask on line 4
        using MR5 = Field<EMR_Reg, 5, 1>; // Event Mask on line 5
        using MR6 = Field<EMR_Reg, 6, 1>; // Event Mask on line 6
        using MR7 = Field<EMR_Reg, 7, 1>; // Event Mask on line 7
        using MR8 = Field<EMR_Reg, 8, 1>; // Event Mask on line 8
        using MR9 = Field<EMR_Reg, 9, 1>; // Event Mask on line 9
        using MR10 = Field<EMR_Reg, 10, 1>; // Event Mask on line 10
        using MR11 = Field<EMR_Reg, 11, 1>; // Event Mask on line 11
        using MR12 = Field<EMR_Reg, 12, 1>; // Event Mask on line 12
        using MR13 = Field<EMR_Reg, 13, 1>; // Event Mask on line 13
        using MR14 = Field<EMR_Reg, 14, 1>; // Event Mask on line 14
        using MR15 = Field<EMR_Reg, 15, 1>; // Event Mask on line 15
        using MR16 = Field<EMR_Reg, 16, 1>; // Event Mask on line 16
        using MR17 = Field<EMR_Reg, 17, 1>; // Event Mask on line 17
        using MR18 = Field<EMR_Reg, 18, 1>; // Event Mask on line 18
        using MR19 = Field<EMR_Reg, 19, 1>; // Event Mask on line 19
        using MR20 = Field<EMR_Reg, 20, 1>; // Event Mask on line 20
        using MR21 = Field<EMR_Reg, 21, 1>; // Event Mask on line 21
        using MR22 = Field<EMR_Reg, 22, 1>; // Event Mask on line 22
    };

    // Rising Trigger selection register (EXTI_RTSR)
    using RTSR_Reg = Register<EXTI_BASE + 0x8, ReadWrite>;
    struct _RTSR {
        using TR0 = Field<RTSR_Reg, 0, 1>; // Rising trigger event configuration of line 0
        using TR1 = Field<RTSR_Reg, 1, 1>; // Rising trigger event configuration of line 1
        using TR2 = Field<RTSR_Reg, 2, 1>; // Rising trigger event configuration of line 2
        using TR3 = Field<RTSR_Reg, 3, 1>; // Rising trigger event configuration of line 3
        using TR4 = Field<RTSR_Reg, 4, 1>; // Rising trigger event configuration of line 4
        using TR5 = Field<RTSR_Reg, 5, 1>; // Rising trigger event configuration of line 5
        using TR6 = Field<RTSR_Reg, 6, 1>; // Rising trigger event configuration of line 6
        using TR7 = Field<RTSR_Reg, 7, 1>; // Rising trigger event configuration of line 7
        using TR8 = Field<RTSR_Reg, 8, 1>; // Rising trigger event configuration of line 8
        using TR9 = Field<RTSR_Reg, 9, 1>; // Rising trigger event configuration of line 9
        using TR10 = Field<RTSR_Reg, 10, 1>; // Rising trigger event configuration of line 10
        using TR11 = Field<RTSR_Reg, 11, 1>; // Rising trigger event configuration of line 11
        using TR12 = Field<RTSR_Reg, 12, 1>; // Rising trigger event configuration of line 12
        using TR13 = Field<RTSR_Reg, 13, 1>; // Rising trigger event configuration of line 13
        using TR14 = Field<RTSR_Reg, 14, 1>; // Rising trigger event configuration of line 14
        using TR15 = Field<RTSR_Reg, 15, 1>; // Rising trigger event configuration of line 15
        using TR16 = Field<RTSR_Reg, 16, 1>; // Rising trigger event configuration of line 16
        using TR17 = Field<RTSR_Reg, 17, 1>; // Rising trigger event configuration of line 17
        using TR18 = Field<RTSR_Reg, 18, 1>; // Rising trigger event configuration of line 18
        using TR19 = Field<RTSR_Reg, 19, 1>; // Rising trigger event configuration of line 19
        using TR20 = Field<RTSR_Reg, 20, 1>; // Rising trigger event configuration of line 20
        using TR21 = Field<RTSR_Reg, 21, 1>; // Rising trigger event configuration of line 21
        using TR22 = Field<RTSR_Reg, 22, 1>; // Rising trigger event configuration of line 22
    };

    // Falling Trigger selection register (EXTI_FTSR)
    using FTSR_Reg = Register<EXTI_BASE + 0xC, ReadWrite>;
    struct _FTSR {
        using TR0 = Field<FTSR_Reg, 0, 1>; // Falling trigger event configuration of line 0
        using TR1 = Field<FTSR_Reg, 1, 1>; // Falling trigger event configuration of line 1
        using TR2 = Field<FTSR_Reg, 2, 1>; // Falling trigger event configuration of line 2
        using TR3 = Field<FTSR_Reg, 3, 1>; // Falling trigger event configuration of line 3
        using TR4 = Field<FTSR_Reg, 4, 1>; // Falling trigger event configuration of line 4
        using TR5 = Field<FTSR_Reg, 5, 1>; // Falling trigger event configuration of line 5
        using TR6 = Field<FTSR_Reg, 6, 1>; // Falling trigger event configuration of line 6
        using TR7 = Field<FTSR_Reg, 7, 1>; // Falling trigger event configuration of line 7
        using TR8 = Field<FTSR_Reg, 8, 1>; // Falling trigger event configuration of line 8
        using TR9 = Field<FTSR_Reg, 9, 1>; // Falling trigger event configuration of line 9
        using TR10 = Field<FTSR_Reg, 10, 1>; // Falling trigger event configuration of line 10
        using TR11 = Field<FTSR_Reg, 11, 1>; // Falling trigger event configuration of line 11
        using TR12 = Field<FTSR_Reg, 12, 1>; // Falling trigger event configuration of line 12
        using TR13 = Field<FTSR_Reg, 13, 1>; // Falling trigger event configuration of line 13
        using TR14 = Field<FTSR_Reg, 14, 1>; // Falling trigger event configuration of line 14
        using TR15 = Field<FTSR_Reg, 15, 1>; // Falling trigger event configuration of line 15
        using TR16 = Field<FTSR_Reg, 16, 1>; // Falling trigger event configuration of line 16
        using TR17 = Field<FTSR_Reg, 17, 1>; // Falling trigger event configuration of line 17
        using TR18 = Field<FTSR_Reg, 18, 1>; // Falling trigger event configuration of line 18
        using TR19 = Field<FTSR_Reg, 19, 1>; // Falling trigger event configuration of line 19
        using TR20 = Field<FTSR_Reg, 20, 1>; // Falling trigger event configuration of line 20
        using TR21 = Field<FTSR_Reg, 21, 1>; // Falling trigger event configuration of line 21
        using TR22 = Field<FTSR_Reg, 22, 1>; // Falling trigger event configuration of line 22
    };

    // Software interrupt event register (EXTI_SWIER)
    using SWIER_Reg = Register<EXTI_BASE + 0x10, ReadWrite>;
    struct _SWIER {
        using SWIER0 = Field<SWIER_Reg, 0, 1>; // Software Interrupt on line 0
        using SWIER1 = Field<SWIER_Reg, 1, 1>; // Software Interrupt on line 1
        using SWIER2 = Field<SWIER_Reg, 2, 1>; // Software Interrupt on line 2
        using SWIER3 = Field<SWIER_Reg, 3, 1>; // Software Interrupt on line 3
        using SWIER4 = Field<SWIER_Reg, 4, 1>; // Software Interrupt on line 4
        using SWIER5 = Field<SWIER_Reg, 5, 1>; // Software Interrupt on line 5
        using SWIER6 = Field<SWIER_Reg, 6, 1>; // Software Interrupt on line 6
        using SWIER7 = Field<SWIER_Reg, 7, 1>; // Software Interrupt on line 7
        using SWIER8 = Field<SWIER_Reg, 8, 1>; // Software Interrupt on line 8
        using SWIER9 = Field<SWIER_Reg, 9, 1>; // Software Interrupt on line 9
        using SWIER10 = Field<SWIER_Reg, 10, 1>; // Software Interrupt on line 10
        using SWIER11 = Field<SWIER_Reg, 11, 1>; // Software Interrupt on line 11
        using SWIER12 = Field<SWIER_Reg, 12, 1>; // Software Interrupt on line 12
        using SWIER13 = Field<SWIER_Reg, 13, 1>; // Software Interrupt on line 13
        using SWIER14 = Field<SWIER_Reg, 14, 1>; // Software Interrupt on line 14
        using SWIER15 = Field<SWIER_Reg, 15, 1>; // Software Interrupt on line 15
        using SWIER16 = Field<SWIER_Reg, 16, 1>; // Software Interrupt on line 16
        using SWIER17 = Field<SWIER_Reg, 17, 1>; // Software Interrupt on line 17
        using SWIER18 = Field<SWIER_Reg, 18, 1>; // Software Interrupt on line 18
        using SWIER19 = Field<SWIER_Reg, 19, 1>; // Software Interrupt on line 19
        using SWIER20 = Field<SWIER_Reg, 20, 1>; // Software Interrupt on line 20
        using SWIER21 = Field<SWIER_Reg, 21, 1>; // Software Interrupt on line 21
        using SWIER22 = Field<SWIER_Reg, 22, 1>; // Software Interrupt on line 22
    };

    // Pending register (EXTI_PR)
    using PR_Reg = Register<EXTI_BASE + 0x14, ReadWrite>;
    struct _PR {
        using PR0 = Field<PR_Reg, 0, 1>; // Pending bit 0
        using PR1 = Field<PR_Reg, 1, 1>; // Pending bit 1
        using PR2 = Field<PR_Reg, 2, 1>; // Pending bit 2
        using PR3 = Field<PR_Reg, 3, 1>; // Pending bit 3
        using PR4 = Field<PR_Reg, 4, 1>; // Pending bit 4
        using PR5 = Field<PR_Reg, 5, 1>; // Pending bit 5
        using PR6 = Field<PR_Reg, 6, 1>; // Pending bit 6
        using PR7 = Field<PR_Reg, 7, 1>; // Pending bit 7
        using PR8 = Field<PR_Reg, 8, 1>; // Pending bit 8
        using PR9 = Field<PR_Reg, 9, 1>; // Pending bit 9
        using PR10 = Field<PR_Reg, 10, 1>; // Pending bit 10
        using PR11 = Field<PR_Reg, 11, 1>; // Pending bit 11
        using PR12 = Field<PR_Reg, 12, 1>; // Pending bit 12
        using PR13 = Field<PR_Reg, 13, 1>; // Pending bit 13
        using PR14 = Field<PR_Reg, 14, 1>; // Pending bit 14
        using PR15 = Field<PR_Reg, 15, 1>; // Pending bit 15
        using PR16 = Field<PR_Reg, 16, 1>; // Pending bit 16
        using PR17 = Field<PR_Reg, 17, 1>; // Pending bit 17
        using PR18 = Field<PR_Reg, 18, 1>; // Pending bit 18
        using PR19 = Field<PR_Reg, 19, 1>; // Pending bit 19
        using PR20 = Field<PR_Reg, 20, 1>; // Pending bit 20
        using PR21 = Field<PR_Reg, 21, 1>; // Pending bit 21
        using PR22 = Field<PR_Reg, 22, 1>; // Pending bit 22
    };

} // namespace EXTI

// --------------------------------------------
// LTCD: LCD-TFT Controller
// Base address: 0x40016800
// --------------------------------------------

namespace LTCD {
    static constexpr uint32_t LTCD_BASE = 0x40016800;

    // Synchronization Size Configuration Register
    using SSCR_Reg = Register<LTCD_BASE + 0x8, ReadWrite>;
    struct _SSCR {
        using HSW = Field<SSCR_Reg, 16, 10>; // Horizontal Synchronization Width (in units of pixel clock period)
        using VSH = Field<SSCR_Reg, 0, 11>; // Vertical Synchronization Height (in units of horizontal scan line)
    };

    // Back Porch Configuration Register
    using BPCR_Reg = Register<LTCD_BASE + 0xC, ReadWrite>;
    struct _BPCR {
        using AHBP = Field<BPCR_Reg, 16, 10>; // Accumulated Horizontal back porch (in units of pixel clock period)
        using AVBP = Field<BPCR_Reg, 0, 11>; // Accumulated Vertical back porch (in units of horizontal scan line)
    };

    // Active Width Configuration Register
    using AWCR_Reg = Register<LTCD_BASE + 0x10, ReadWrite>;
    struct _AWCR {
        using AAV = Field<AWCR_Reg, 16, 10>; // AAV
        using AAH = Field<AWCR_Reg, 0, 11>; // Accumulated Active Height (in units of horizontal scan line)
    };

    // Total Width Configuration Register
    using TWCR_Reg = Register<LTCD_BASE + 0x14, ReadWrite>;
    struct _TWCR {
        using TOTALW = Field<TWCR_Reg, 16, 10>; // Total Width (in units of pixel clock period)
        using TOTALH = Field<TWCR_Reg, 0, 11>; // Total Height (in units of horizontal scan line)
    };

    // Global Control Register
    using GCR_Reg = Register<LTCD_BASE + 0x18, ReadWrite>;
    struct _GCR {
        using HSPOL = Field<GCR_Reg, 31, 1>; // Horizontal Synchronization Polarity
        using VSPOL = Field<GCR_Reg, 30, 1>; // Vertical Synchronization Polarity
        using DEPOL = Field<GCR_Reg, 29, 1>; // Data Enable Polarity
        using PCPOL = Field<GCR_Reg, 28, 1>; // Pixel Clock Polarity
        using DEN = Field<GCR_Reg, 16, 1>; // Dither Enable
        using DRW = Field<GCR_Reg, 12, 3>; // Dither Red Width
        using DGW = Field<GCR_Reg, 8, 3>; // Dither Green Width
        using DBW = Field<GCR_Reg, 4, 3>; // Dither Blue Width
        using LTDCEN = Field<GCR_Reg, 0, 1>; // LCD-TFT controller enable bit
    };

    // Shadow Reload Configuration Register
    using SRCR_Reg = Register<LTCD_BASE + 0x24, ReadWrite>;
    struct _SRCR {
        using VBR = Field<SRCR_Reg, 1, 1>; // Vertical Blanking Reload
        using IMR = Field<SRCR_Reg, 0, 1>; // Immediate Reload
    };

    // Background Color Configuration Register
    using BCCR_Reg = Register<LTCD_BASE + 0x2C, ReadWrite>;
    struct _BCCR {
        using BC = Field<BCCR_Reg, 0, 24>; // Background Color Red value
    };

    // Interrupt Enable Register
    using IER_Reg = Register<LTCD_BASE + 0x34, ReadWrite>;
    struct _IER {
        using RRIE = Field<IER_Reg, 3, 1>; // Register Reload interrupt enable
        using TERRIE = Field<IER_Reg, 2, 1>; // Transfer Error Interrupt Enable
        using FUIE = Field<IER_Reg, 1, 1>; // FIFO Underrun Interrupt Enable
        using LIE = Field<IER_Reg, 0, 1>; // Line Interrupt Enable
    };

    // Interrupt Status Register
    using ISR_Reg = Register<LTCD_BASE + 0x38, ReadOnly>;
    struct _ISR {
        using RRIF = Field<ISR_Reg, 3, 1>; // Register Reload Interrupt Flag
        using TERRIF = Field<ISR_Reg, 2, 1>; // Transfer Error interrupt flag
        using FUIF = Field<ISR_Reg, 1, 1>; // FIFO Underrun Interrupt flag
        using LIF = Field<ISR_Reg, 0, 1>; // Line Interrupt flag
    };

    // Interrupt Clear Register
    using ICR_Reg = Register<LTCD_BASE + 0x3C, WriteOnly>;
    struct _ICR {
        using CRRIF = Field<ICR_Reg, 3, 1>; // Clears Register Reload Interrupt Flag
        using CTERRIF = Field<ICR_Reg, 2, 1>; // Clears the Transfer Error Interrupt Flag
        using CFUIF = Field<ICR_Reg, 1, 1>; // Clears the FIFO Underrun Interrupt flag
        using CLIF = Field<ICR_Reg, 0, 1>; // Clears the Line Interrupt Flag
    };

    // Line Interrupt Position Configuration Register
    using LIPCR_Reg = Register<LTCD_BASE + 0x40, ReadWrite>;
    struct _LIPCR {
        using LIPOS = Field<LIPCR_Reg, 0, 11>; // Line Interrupt Position
    };

    // Current Position Status Register
    using CPSR_Reg = Register<LTCD_BASE + 0x44, ReadOnly>;
    struct _CPSR {
        using CXPOS = Field<CPSR_Reg, 16, 16>; // Current X Position
        using CYPOS = Field<CPSR_Reg, 0, 16>; // Current Y Position
    };

    // Current Display Status Register
    using CDSR_Reg = Register<LTCD_BASE + 0x48, ReadOnly>;
    struct _CDSR {
        using HSYNCS = Field<CDSR_Reg, 3, 1>; // Horizontal Synchronization display Status
        using VSYNCS = Field<CDSR_Reg, 2, 1>; // Vertical Synchronization display Status
        using HDES = Field<CDSR_Reg, 1, 1>; // Horizontal Data Enable display Status
        using VDES = Field<CDSR_Reg, 0, 1>; // Vertical Data Enable display Status
    };

    // Layerx Control Register
    using L1CR_Reg = Register<LTCD_BASE + 0x84, ReadWrite>;
    struct _L1CR {
        using CLUTEN = Field<L1CR_Reg, 4, 1>; // Color Look-Up Table Enable
        using COLKEN = Field<L1CR_Reg, 1, 1>; // Color Keying Enable
        using LEN = Field<L1CR_Reg, 0, 1>; // Layer Enable
    };

    // Layerx Window Horizontal Position Configuration Register
    using L1WHPCR_Reg = Register<LTCD_BASE + 0x88, ReadWrite>;
    struct _L1WHPCR {
        using WHSPPOS = Field<L1WHPCR_Reg, 16, 12>; // Window Horizontal Stop Position
        using WHSTPOS = Field<L1WHPCR_Reg, 0, 12>; // Window Horizontal Start Position
    };

    // Layerx Window Vertical Position Configuration Register
    using L1WVPCR_Reg = Register<LTCD_BASE + 0x8C, ReadWrite>;
    struct _L1WVPCR {
        using WVSPPOS = Field<L1WVPCR_Reg, 16, 11>; // Window Vertical Stop Position
        using WVSTPOS = Field<L1WVPCR_Reg, 0, 11>; // Window Vertical Start Position
    };

    // Layerx Color Keying Configuration Register
    using L1CKCR_Reg = Register<LTCD_BASE + 0x90, ReadWrite>;
    struct _L1CKCR {
        using CKRED = Field<L1CKCR_Reg, 16, 8>; // Color Key Red value
        using CKGREEN = Field<L1CKCR_Reg, 8, 8>; // Color Key Green value
        using CKBLUE = Field<L1CKCR_Reg, 0, 8>; // Color Key Blue value
    };

    // Layerx Pixel Format Configuration Register
    using L1PFCR_Reg = Register<LTCD_BASE + 0x94, ReadWrite>;
    struct _L1PFCR {
        using PF = Field<L1PFCR_Reg, 0, 3>; // Pixel Format
    };

    // Layerx Constant Alpha Configuration Register
    using L1CACR_Reg = Register<LTCD_BASE + 0x98, ReadWrite>;
    struct _L1CACR {
        using CONSTA = Field<L1CACR_Reg, 0, 8>; // Constant Alpha
    };

    // Layerx Default Color Configuration Register
    using L1DCCR_Reg = Register<LTCD_BASE + 0x9C, ReadWrite>;
    struct _L1DCCR {
        using DCALPHA = Field<L1DCCR_Reg, 24, 8>; // Default Color Alpha
        using DCRED = Field<L1DCCR_Reg, 16, 8>; // Default Color Red
        using DCGREEN = Field<L1DCCR_Reg, 8, 8>; // Default Color Green
        using DCBLUE = Field<L1DCCR_Reg, 0, 8>; // Default Color Blue
    };

    // Layerx Blending Factors Configuration Register
    using L1BFCR_Reg = Register<LTCD_BASE + 0xA0, ReadWrite>;
    struct _L1BFCR {
        using BF1 = Field<L1BFCR_Reg, 8, 3>; // Blending Factor 1
        using BF2 = Field<L1BFCR_Reg, 0, 3>; // Blending Factor 2
    };

    // Layerx Color Frame Buffer Address Register
    using L1CFBAR_Reg = Register<LTCD_BASE + 0xAC, ReadWrite>;
    struct _L1CFBAR {
        using CFBADD = Field<L1CFBAR_Reg, 0, 32>; // Color Frame Buffer Start Address
    };

    // Layerx Color Frame Buffer Length Register
    using L1CFBLR_Reg = Register<LTCD_BASE + 0xB0, ReadWrite>;
    struct _L1CFBLR {
        using CFBP = Field<L1CFBLR_Reg, 16, 13>; // Color Frame Buffer Pitch in bytes
        using CFBLL = Field<L1CFBLR_Reg, 0, 13>; // Color Frame Buffer Line Length
    };

    // Layerx ColorFrame Buffer Line Number Register
    using L1CFBLNR_Reg = Register<LTCD_BASE + 0xB4, ReadWrite>;
    struct _L1CFBLNR {
        using CFBLNBR = Field<L1CFBLNR_Reg, 0, 11>; // Frame Buffer Line Number
    };

    // Layerx CLUT Write Register
    using L1CLUTWR_Reg = Register<LTCD_BASE + 0xC4, WriteOnly>;
    struct _L1CLUTWR {
        using CLUTADD = Field<L1CLUTWR_Reg, 24, 8>; // CLUT Address
        using RED = Field<L1CLUTWR_Reg, 16, 8>; // Red value
        using GREEN = Field<L1CLUTWR_Reg, 8, 8>; // Green value
        using BLUE = Field<L1CLUTWR_Reg, 0, 8>; // Blue value
    };

    // Layerx Control Register
    using L2CR_Reg = Register<LTCD_BASE + 0x104, ReadWrite>;
    struct _L2CR {
        using CLUTEN = Field<L2CR_Reg, 4, 1>; // Color Look-Up Table Enable
        using COLKEN = Field<L2CR_Reg, 1, 1>; // Color Keying Enable
        using LEN = Field<L2CR_Reg, 0, 1>; // Layer Enable
    };

    // Layerx Window Horizontal Position Configuration Register
    using L2WHPCR_Reg = Register<LTCD_BASE + 0x108, ReadWrite>;
    struct _L2WHPCR {
        using WHSPPOS = Field<L2WHPCR_Reg, 16, 12>; // Window Horizontal Stop Position
        using WHSTPOS = Field<L2WHPCR_Reg, 0, 12>; // Window Horizontal Start Position
    };

    // Layerx Window Vertical Position Configuration Register
    using L2WVPCR_Reg = Register<LTCD_BASE + 0x10C, ReadWrite>;
    struct _L2WVPCR {
        using WVSPPOS = Field<L2WVPCR_Reg, 16, 11>; // Window Vertical Stop Position
        using WVSTPOS = Field<L2WVPCR_Reg, 0, 11>; // Window Vertical Start Position
    };

    // Layerx Color Keying Configuration Register
    using L2CKCR_Reg = Register<LTCD_BASE + 0x110, ReadWrite>;
    struct _L2CKCR {
        using CKRED = Field<L2CKCR_Reg, 15, 9>; // Color Key Red value
        using CKGREEN = Field<L2CKCR_Reg, 8, 7>; // Color Key Green value
        using CKBLUE = Field<L2CKCR_Reg, 0, 8>; // Color Key Blue value
    };

    // Layerx Pixel Format Configuration Register
    using L2PFCR_Reg = Register<LTCD_BASE + 0x114, ReadWrite>;
    struct _L2PFCR {
        using PF = Field<L2PFCR_Reg, 0, 3>; // Pixel Format
    };

    // Layerx Constant Alpha Configuration Register
    using L2CACR_Reg = Register<LTCD_BASE + 0x118, ReadWrite>;
    struct _L2CACR {
        using CONSTA = Field<L2CACR_Reg, 0, 8>; // Constant Alpha
    };

    // Layerx Default Color Configuration Register
    using L2DCCR_Reg = Register<LTCD_BASE + 0x11C, ReadWrite>;
    struct _L2DCCR {
        using DCALPHA = Field<L2DCCR_Reg, 24, 8>; // Default Color Alpha
        using DCRED = Field<L2DCCR_Reg, 16, 8>; // Default Color Red
        using DCGREEN = Field<L2DCCR_Reg, 8, 8>; // Default Color Green
        using DCBLUE = Field<L2DCCR_Reg, 0, 8>; // Default Color Blue
    };

    // Layerx Blending Factors Configuration Register
    using L2BFCR_Reg = Register<LTCD_BASE + 0x120, ReadWrite>;
    struct _L2BFCR {
        using BF1 = Field<L2BFCR_Reg, 8, 3>; // Blending Factor 1
        using BF2 = Field<L2BFCR_Reg, 0, 3>; // Blending Factor 2
    };

    // Layerx Color Frame Buffer Address Register
    using L2CFBAR_Reg = Register<LTCD_BASE + 0x12C, ReadWrite>;
    struct _L2CFBAR {
        using CFBADD = Field<L2CFBAR_Reg, 0, 32>; // Color Frame Buffer Start Address
    };

    // Layerx Color Frame Buffer Length Register
    using L2CFBLR_Reg = Register<LTCD_BASE + 0x130, ReadWrite>;
    struct _L2CFBLR {
        using CFBP = Field<L2CFBLR_Reg, 16, 13>; // Color Frame Buffer Pitch in bytes
        using CFBLL = Field<L2CFBLR_Reg, 0, 13>; // Color Frame Buffer Line Length
    };

    // Layerx ColorFrame Buffer Line Number Register
    using L2CFBLNR_Reg = Register<LTCD_BASE + 0x134, ReadWrite>;
    struct _L2CFBLNR {
        using CFBLNBR = Field<L2CFBLNR_Reg, 0, 11>; // Frame Buffer Line Number
    };

    // Layerx CLUT Write Register
    using L2CLUTWR_Reg = Register<LTCD_BASE + 0x144, WriteOnly>;
    struct _L2CLUTWR {
        using CLUTADD = Field<L2CLUTWR_Reg, 24, 8>; // CLUT Address
        using RED = Field<L2CLUTWR_Reg, 16, 8>; // Red value
        using GREEN = Field<L2CLUTWR_Reg, 8, 8>; // Green value
        using BLUE = Field<L2CLUTWR_Reg, 0, 8>; // Blue value
    };

} // namespace LTCD

// --------------------------------------------
// SAI1: Serial audio interface
// Base address: 0x40015800
// --------------------------------------------

namespace SAI1 {
    static constexpr uint32_t SAI1_BASE = 0x40015800;

    // BConfiguration register 1
    using BCR1_Reg = Register<SAI1_BASE + 0x24, ReadWrite>;
    struct _BCR1 {
        using MCKDIV = Field<BCR1_Reg, 20, 4>; // Master clock divider
        using NODIV = Field<BCR1_Reg, 19, 1>; // No divider
        using DMAEN = Field<BCR1_Reg, 17, 1>; // DMA enable
        using SAIBEN = Field<BCR1_Reg, 16, 1>; // Audio block B enable
        using OutDri = Field<BCR1_Reg, 13, 1>; // Output drive
        using MONO = Field<BCR1_Reg, 12, 1>; // Mono mode
        using SYNCEN = Field<BCR1_Reg, 10, 2>; // Synchronization enable
        using CKSTR = Field<BCR1_Reg, 9, 1>; // Clock strobing edge
        using LSBFIRST = Field<BCR1_Reg, 8, 1>; // Least significant bit first
        using DS = Field<BCR1_Reg, 5, 3>; // Data size
        using PRTCFG = Field<BCR1_Reg, 2, 2>; // Protocol configuration
        using MODE = Field<BCR1_Reg, 0, 2>; // Audio block mode
    };

    // BConfiguration register 2
    using BCR2_Reg = Register<SAI1_BASE + 0x28, ReadWrite>;
    struct _BCR2 {
        using COMP = Field<BCR2_Reg, 14, 2>; // Companding mode
        using CPL = Field<BCR2_Reg, 13, 1>; // Complement bit
        using MUTECN = Field<BCR2_Reg, 7, 6>; // Mute counter
        using MUTEVAL = Field<BCR2_Reg, 6, 1>; // Mute value
        using MUTE = Field<BCR2_Reg, 5, 1>; // Mute
        using TRIS = Field<BCR2_Reg, 4, 1>; // Tristate management on data line
        using FFLUS = Field<BCR2_Reg, 3, 1>; // FIFO flush
        using FTH = Field<BCR2_Reg, 0, 3>; // FIFO threshold
    };

    // BFRCR
    using BFRCR_Reg = Register<SAI1_BASE + 0x2C, ReadWrite>;
    struct _BFRCR {
        using FSOFF = Field<BFRCR_Reg, 18, 1>; // Frame synchronization offset
        using FSPOL = Field<BFRCR_Reg, 17, 1>; // Frame synchronization polarity
        using FSDEF = Field<BFRCR_Reg, 16, 1>; // Frame synchronization definition
        using FSALL = Field<BFRCR_Reg, 8, 7>; // Frame synchronization active level length
        using FRL = Field<BFRCR_Reg, 0, 8>; // Frame length
    };

    // BSlot register
    using BSLOTR_Reg = Register<SAI1_BASE + 0x30, ReadWrite>;
    struct _BSLOTR {
        using SLOTEN = Field<BSLOTR_Reg, 16, 16>; // Slot enable
        using NBSLOT = Field<BSLOTR_Reg, 8, 4>; // Number of slots in an audio frame
        using SLOTSZ = Field<BSLOTR_Reg, 6, 2>; // Slot size
        using FBOFF = Field<BSLOTR_Reg, 0, 5>; // First bit offset
    };

    // BInterrupt mask register2
    using BIM_Reg = Register<SAI1_BASE + 0x34, ReadWrite>;
    struct _BIM {
        using LFSDETIE = Field<BIM_Reg, 6, 1>; // Late frame synchronization detection interrupt enable
        using AFSDETIE = Field<BIM_Reg, 5, 1>; // Anticipated frame synchronization detection interrupt enable
        using CNRDYIE = Field<BIM_Reg, 4, 1>; // Codec not ready interrupt enable
        using FREQIE = Field<BIM_Reg, 3, 1>; // FIFO request interrupt enable
        using WCKCFG = Field<BIM_Reg, 2, 1>; // Wrong clock configuration interrupt enable
        using MUTEDET = Field<BIM_Reg, 1, 1>; // Mute detection interrupt enable
        using OVRUDRIE = Field<BIM_Reg, 0, 1>; // Overrun/underrun interrupt enable
    };

    // BStatus register
    using BSR_Reg = Register<SAI1_BASE + 0x38, ReadOnly>;
    struct _BSR {
        using FLVL = Field<BSR_Reg, 16, 3>; // FIFO level threshold
        using LFSDET = Field<BSR_Reg, 6, 1>; // Late frame synchronization detection
        using AFSDET = Field<BSR_Reg, 5, 1>; // Anticipated frame synchronization detection
        using CNRDY = Field<BSR_Reg, 4, 1>; // Codec not ready
        using FREQ = Field<BSR_Reg, 3, 1>; // FIFO request
        using WCKCFG = Field<BSR_Reg, 2, 1>; // Wrong clock configuration flag
        using MUTEDET = Field<BSR_Reg, 1, 1>; // Mute detection
        using OVRUDR = Field<BSR_Reg, 0, 1>; // Overrun / underrun
    };

    // BClear flag register
    using BCLRFR_Reg = Register<SAI1_BASE + 0x3C, WriteOnly>;
    struct _BCLRFR {
        using LFSDET = Field<BCLRFR_Reg, 6, 1>; // Clear late frame synchronization detection flag
        using CAFSDET = Field<BCLRFR_Reg, 5, 1>; // Clear anticipated frame synchronization detection flag
        using CNRDY = Field<BCLRFR_Reg, 4, 1>; // Clear codec not ready flag
        using WCKCFG = Field<BCLRFR_Reg, 2, 1>; // Clear wrong clock configuration flag
        using MUTEDET = Field<BCLRFR_Reg, 1, 1>; // Mute detection flag
        using OVRUDR = Field<BCLRFR_Reg, 0, 1>; // Clear overrun / underrun
    };

    // BData register
    using BDR_Reg = Register<SAI1_BASE + 0x40, ReadWrite>;
    struct _BDR {
        using DATA = Field<BDR_Reg, 0, 32>; // Data
    };

    // AConfiguration register 1
    using ACR1_Reg = Register<SAI1_BASE + 0x4, ReadWrite>;
    struct _ACR1 {
        using MCKDIV = Field<ACR1_Reg, 20, 4>; // Master clock divider
        using NODIV = Field<ACR1_Reg, 19, 1>; // No divider
        using DMAEN = Field<ACR1_Reg, 17, 1>; // DMA enable
        using SAIAEN = Field<ACR1_Reg, 16, 1>; // Audio block A enable
        using OutDri = Field<ACR1_Reg, 13, 1>; // Output drive
        using MONO = Field<ACR1_Reg, 12, 1>; // Mono mode
        using SYNCEN = Field<ACR1_Reg, 10, 2>; // Synchronization enable
        using CKSTR = Field<ACR1_Reg, 9, 1>; // Clock strobing edge
        using LSBFIRST = Field<ACR1_Reg, 8, 1>; // Least significant bit first
        using DS = Field<ACR1_Reg, 5, 3>; // Data size
        using PRTCFG = Field<ACR1_Reg, 2, 2>; // Protocol configuration
        using MODE = Field<ACR1_Reg, 0, 2>; // Audio block mode
    };

    // AConfiguration register 2
    using ACR2_Reg = Register<SAI1_BASE + 0x8, ReadWrite>;
    struct _ACR2 {
        using COMP = Field<ACR2_Reg, 14, 2>; // Companding mode
        using CPL = Field<ACR2_Reg, 13, 1>; // Complement bit
        using MUTECN = Field<ACR2_Reg, 7, 6>; // Mute counter
        using MUTEVAL = Field<ACR2_Reg, 6, 1>; // Mute value
        using MUTE = Field<ACR2_Reg, 5, 1>; // Mute
        using TRIS = Field<ACR2_Reg, 4, 1>; // Tristate management on data line
        using FFLUS = Field<ACR2_Reg, 3, 1>; // FIFO flush
        using FTH = Field<ACR2_Reg, 0, 3>; // FIFO threshold
    };

    // AFRCR
    using AFRCR_Reg = Register<SAI1_BASE + 0xC, ReadWrite>;
    struct _AFRCR {
        using FSOFF = Field<AFRCR_Reg, 18, 1>; // Frame synchronization offset
        using FSPOL = Field<AFRCR_Reg, 17, 1>; // Frame synchronization polarity
        using FSDEF = Field<AFRCR_Reg, 16, 1>; // Frame synchronization definition
        using FSALL = Field<AFRCR_Reg, 8, 7>; // Frame synchronization active level length
        using FRL = Field<AFRCR_Reg, 0, 8>; // Frame length
    };

    // ASlot register
    using ASLOTR_Reg = Register<SAI1_BASE + 0x10, ReadWrite>;
    struct _ASLOTR {
        using SLOTEN = Field<ASLOTR_Reg, 16, 16>; // Slot enable
        using NBSLOT = Field<ASLOTR_Reg, 8, 4>; // Number of slots in an audio frame
        using SLOTSZ = Field<ASLOTR_Reg, 6, 2>; // Slot size
        using FBOFF = Field<ASLOTR_Reg, 0, 5>; // First bit offset
    };

    // AInterrupt mask register2
    using AIM_Reg = Register<SAI1_BASE + 0x14, ReadWrite>;
    struct _AIM {
        using LFSDET = Field<AIM_Reg, 6, 1>; // Late frame synchronization detection interrupt enable
        using AFSDETIE = Field<AIM_Reg, 5, 1>; // Anticipated frame synchronization detection interrupt enable
        using CNRDYIE = Field<AIM_Reg, 4, 1>; // Codec not ready interrupt enable
        using FREQIE = Field<AIM_Reg, 3, 1>; // FIFO request interrupt enable
        using WCKCFG = Field<AIM_Reg, 2, 1>; // Wrong clock configuration interrupt enable
        using MUTEDET = Field<AIM_Reg, 1, 1>; // Mute detection interrupt enable
        using OVRUDRIE = Field<AIM_Reg, 0, 1>; // Overrun/underrun interrupt enable
    };

    // AStatus register
    using ASR_Reg = Register<SAI1_BASE + 0x18, ReadWrite>;
    struct _ASR {
        using FLVL = Field<ASR_Reg, 16, 3>; // FIFO level threshold
        using LFSDET = Field<ASR_Reg, 6, 1>; // Late frame synchronization detection
        using AFSDET = Field<ASR_Reg, 5, 1>; // Anticipated frame synchronization detection
        using CNRDY = Field<ASR_Reg, 4, 1>; // Codec not ready
        using FREQ = Field<ASR_Reg, 3, 1>; // FIFO request
        using WCKCFG = Field<ASR_Reg, 2, 1>; // Wrong clock configuration flag. This bit is read only.
        using MUTEDET = Field<ASR_Reg, 1, 1>; // Mute detection
        using OVRUDR = Field<ASR_Reg, 0, 1>; // Overrun / underrun
    };

    // AClear flag register
    using ACLRFR_Reg = Register<SAI1_BASE + 0x1C, ReadWrite>;
    struct _ACLRFR {
        using LFSDET = Field<ACLRFR_Reg, 6, 1>; // Clear late frame synchronization detection flag
        using CAFSDET = Field<ACLRFR_Reg, 5, 1>; // Clear anticipated frame synchronization detection flag.
        using CNRDY = Field<ACLRFR_Reg, 4, 1>; // Clear codec not ready flag
        using WCKCFG = Field<ACLRFR_Reg, 2, 1>; // Clear wrong clock configuration flag
        using MUTEDET = Field<ACLRFR_Reg, 1, 1>; // Mute detection flag
        using OVRUDR = Field<ACLRFR_Reg, 0, 1>; // Clear overrun / underrun
    };

    // AData register
    using ADR_Reg = Register<SAI1_BASE + 0x20, ReadWrite>;
    struct _ADR {
        using DATA = Field<ADR_Reg, 0, 32>; // Data
    };

    // Global configuration register
    using GCR_Reg = Register<SAI1_BASE + 0x0, ReadWrite>;
    struct _GCR {
        using SYNCIN = Field<GCR_Reg, 0, 2>; // Synchronization inputs
        using SYNCOUT = Field<GCR_Reg, 4, 2>; // Synchronization outputs
    };

} // namespace SAI1

// --------------------------------------------
// SAI2: 
// Base address: 0x40015C00
// --------------------------------------------

namespace SAI2 {
    static constexpr uint32_t SAI2_BASE = 0x40015C00;

} // namespace SAI2

// --------------------------------------------
// DMA2D: DMA2D controller
// Base address: 0x4002B000
// --------------------------------------------

namespace DMA2D {
    static constexpr uint32_t DMA2D_BASE = 0x4002B000;

    // control register
    using CR_Reg = Register<DMA2D_BASE + 0x0, ReadWrite>;
    struct _CR {
        using MODE = Field<CR_Reg, 16, 2>; // DMA2D mode
        using CEIE = Field<CR_Reg, 13, 1>; // Configuration Error Interrupt Enable
        using CTCIE = Field<CR_Reg, 12, 1>; // CLUT transfer complete interrupt enable
        using CAEIE = Field<CR_Reg, 11, 1>; // CLUT access error interrupt enable
        using TWIE = Field<CR_Reg, 10, 1>; // Transfer watermark interrupt enable
        using TCIE = Field<CR_Reg, 9, 1>; // Transfer complete interrupt enable
        using TEIE = Field<CR_Reg, 8, 1>; // Transfer error interrupt enable
        using ABORT = Field<CR_Reg, 2, 1>; // Abort
        using SUSP = Field<CR_Reg, 1, 1>; // Suspend
        using START = Field<CR_Reg, 0, 1>; // Start
    };

    // Interrupt Status Register
    using ISR_Reg = Register<DMA2D_BASE + 0x4, ReadOnly>;
    struct _ISR {
        using CEIF = Field<ISR_Reg, 5, 1>; // Configuration error interrupt flag
        using CTCIF = Field<ISR_Reg, 4, 1>; // CLUT transfer complete interrupt flag
        using CAEIF = Field<ISR_Reg, 3, 1>; // CLUT access error interrupt flag
        using TWIF = Field<ISR_Reg, 2, 1>; // Transfer watermark interrupt flag
        using TCIF = Field<ISR_Reg, 1, 1>; // Transfer complete interrupt flag
        using TEIF = Field<ISR_Reg, 0, 1>; // Transfer error interrupt flag
    };

    // interrupt flag clear register
    using IFCR_Reg = Register<DMA2D_BASE + 0x8, ReadWrite>;
    struct _IFCR {
        using CCEIF = Field<IFCR_Reg, 5, 1>; // Clear configuration error interrupt flag
        using CCTCIF = Field<IFCR_Reg, 4, 1>; // Clear CLUT transfer complete interrupt flag
        using CAECIF = Field<IFCR_Reg, 3, 1>; // Clear CLUT access error interrupt flag
        using CTWIF = Field<IFCR_Reg, 2, 1>; // Clear transfer watermark interrupt flag
        using CTCIF = Field<IFCR_Reg, 1, 1>; // Clear transfer complete interrupt flag
        using CTEIF = Field<IFCR_Reg, 0, 1>; // Clear Transfer error interrupt flag
    };

    // foreground memory address register
    using FGMAR_Reg = Register<DMA2D_BASE + 0xC, ReadWrite>;
    struct _FGMAR {
        using MA = Field<FGMAR_Reg, 0, 32>; // Memory address
    };

    // foreground offset register
    using FGOR_Reg = Register<DMA2D_BASE + 0x10, ReadWrite>;
    struct _FGOR {
        using LO = Field<FGOR_Reg, 0, 14>; // Line offset
    };

    // background memory address register
    using BGMAR_Reg = Register<DMA2D_BASE + 0x14, ReadWrite>;
    struct _BGMAR {
        using MA = Field<BGMAR_Reg, 0, 32>; // Memory address
    };

    // background offset register
    using BGOR_Reg = Register<DMA2D_BASE + 0x18, ReadWrite>;
    struct _BGOR {
        using LO = Field<BGOR_Reg, 0, 14>; // Line offset
    };

    // foreground PFC control register
    using FGPFCCR_Reg = Register<DMA2D_BASE + 0x1C, ReadWrite>;
    struct _FGPFCCR {
        using ALPHA = Field<FGPFCCR_Reg, 24, 8>; // Alpha value
        using AM = Field<FGPFCCR_Reg, 16, 2>; // Alpha mode
        using CS = Field<FGPFCCR_Reg, 8, 8>; // CLUT size
        using START = Field<FGPFCCR_Reg, 5, 1>; // Start
        using CCM = Field<FGPFCCR_Reg, 4, 1>; // CLUT color mode
        using CM = Field<FGPFCCR_Reg, 0, 4>; // Color mode
    };

    // foreground color register
    using FGCOLR_Reg = Register<DMA2D_BASE + 0x20, ReadWrite>;
    struct _FGCOLR {
        using RED = Field<FGCOLR_Reg, 16, 8>; // Red Value
        using GREEN = Field<FGCOLR_Reg, 8, 8>; // Green Value
        using BLUE = Field<FGCOLR_Reg, 0, 8>; // Blue Value
    };

    // background PFC control register
    using BGPFCCR_Reg = Register<DMA2D_BASE + 0x24, ReadWrite>;
    struct _BGPFCCR {
        using ALPHA = Field<BGPFCCR_Reg, 24, 8>; // Alpha value
        using AM = Field<BGPFCCR_Reg, 16, 2>; // Alpha mode
        using CS = Field<BGPFCCR_Reg, 8, 8>; // CLUT size
        using START = Field<BGPFCCR_Reg, 5, 1>; // Start
        using CCM = Field<BGPFCCR_Reg, 4, 1>; // CLUT Color mode
        using CM = Field<BGPFCCR_Reg, 0, 4>; // Color mode
    };

    // background color register
    using BGCOLR_Reg = Register<DMA2D_BASE + 0x28, ReadWrite>;
    struct _BGCOLR {
        using RED = Field<BGCOLR_Reg, 16, 8>; // Red Value
        using GREEN = Field<BGCOLR_Reg, 8, 8>; // Green Value
        using BLUE = Field<BGCOLR_Reg, 0, 8>; // Blue Value
    };

    // foreground CLUT memory address register
    using FGCMAR_Reg = Register<DMA2D_BASE + 0x2C, ReadWrite>;
    struct _FGCMAR {
        using MA = Field<FGCMAR_Reg, 0, 32>; // Memory Address
    };

    // background CLUT memory address register
    using BGCMAR_Reg = Register<DMA2D_BASE + 0x30, ReadWrite>;
    struct _BGCMAR {
        using MA = Field<BGCMAR_Reg, 0, 32>; // Memory address
    };

    // output PFC control register
    using OPFCCR_Reg = Register<DMA2D_BASE + 0x34, ReadWrite>;
    struct _OPFCCR {
        using CM = Field<OPFCCR_Reg, 0, 3>; // Color mode
    };

    // output color register
    using OCOLR_Reg = Register<DMA2D_BASE + 0x38, ReadWrite>;
    struct _OCOLR {
        using APLHA = Field<OCOLR_Reg, 24, 8>; // Alpha Channel Value
        using RED = Field<OCOLR_Reg, 16, 8>; // Red Value
        using GREEN = Field<OCOLR_Reg, 8, 8>; // Green Value
        using BLUE = Field<OCOLR_Reg, 0, 8>; // Blue Value
    };

    // output memory address register
    using OMAR_Reg = Register<DMA2D_BASE + 0x3C, ReadWrite>;
    struct _OMAR {
        using MA = Field<OMAR_Reg, 0, 32>; // Memory Address
    };

    // output offset register
    using OOR_Reg = Register<DMA2D_BASE + 0x40, ReadWrite>;
    struct _OOR {
        using LO = Field<OOR_Reg, 0, 14>; // Line Offset
    };

    // number of line register
    using NLR_Reg = Register<DMA2D_BASE + 0x44, ReadWrite>;
    struct _NLR {
        using PL = Field<NLR_Reg, 16, 14>; // Pixel per lines
        using NL = Field<NLR_Reg, 0, 16>; // Number of lines
    };

    // line watermark register
    using LWR_Reg = Register<DMA2D_BASE + 0x48, ReadWrite>;
    struct _LWR {
        using LW = Field<LWR_Reg, 0, 16>; // Line watermark
    };

    // AHB master timer configuration register
    using AMTCR_Reg = Register<DMA2D_BASE + 0x4C, ReadWrite>;
    struct _AMTCR {
        using DT = Field<AMTCR_Reg, 8, 8>; // Dead Time
        using EN = Field<AMTCR_Reg, 0, 1>; // Enable
    };

    // FGCLUT
    using FGCLUT_Reg = Register<DMA2D_BASE + 0x400, ReadWrite>;
    struct _FGCLUT {
        using APLHA = Field<FGCLUT_Reg, 24, 8>; // APLHA
        using RED = Field<FGCLUT_Reg, 16, 8>; // RED
        using GREEN = Field<FGCLUT_Reg, 8, 8>; // GREEN
        using BLUE = Field<FGCLUT_Reg, 0, 8>; // BLUE
    };

    // BGCLUT
    using BGCLUT_Reg = Register<DMA2D_BASE + 0x800, ReadWrite>;
    struct _BGCLUT {
        using APLHA = Field<BGCLUT_Reg, 24, 8>; // APLHA
        using RED = Field<BGCLUT_Reg, 16, 8>; // RED
        using GREEN = Field<BGCLUT_Reg, 8, 8>; // GREEN
        using BLUE = Field<BGCLUT_Reg, 0, 8>; // BLUE
    };

} // namespace DMA2D

// --------------------------------------------
// QUADSPI: QuadSPI interface
// Base address: 0xA0001000
// --------------------------------------------

namespace QUADSPI {
    static constexpr uint32_t QUADSPI_BASE = 0xA0001000;

    // control register
    using CR_Reg = Register<QUADSPI_BASE + 0x0, ReadWrite>;
    struct _CR {
        using PRESCALER = Field<CR_Reg, 24, 8>; // Clock prescaler
        using PMM = Field<CR_Reg, 23, 1>; // Polling match mode
        using APMS = Field<CR_Reg, 22, 1>; // Automatic poll mode stop
        using TOIE = Field<CR_Reg, 20, 1>; // TimeOut interrupt enable
        using SMIE = Field<CR_Reg, 19, 1>; // Status match interrupt enable
        using FTIE = Field<CR_Reg, 18, 1>; // FIFO threshold interrupt enable
        using TCIE = Field<CR_Reg, 17, 1>; // Transfer complete interrupt enable
        using TEIE = Field<CR_Reg, 16, 1>; // Transfer error interrupt enable
        using FTHRES = Field<CR_Reg, 8, 5>; // IFO threshold level
        using FSEL = Field<CR_Reg, 7, 1>; // FLASH memory selection
        using DFM = Field<CR_Reg, 6, 1>; // Dual-flash mode
        using SSHIFT = Field<CR_Reg, 4, 1>; // Sample shift
        using TCEN = Field<CR_Reg, 3, 1>; // Timeout counter enable
        using DMAEN = Field<CR_Reg, 2, 1>; // DMA enable
        using ABORT = Field<CR_Reg, 1, 1>; // Abort request
        using EN = Field<CR_Reg, 0, 1>; // Enable
    };

    // device configuration register
    using DCR_Reg = Register<QUADSPI_BASE + 0x4, ReadWrite>;
    struct _DCR {
        using FSIZE = Field<DCR_Reg, 16, 5>; // FLASH memory size
        using CSHT = Field<DCR_Reg, 8, 3>; // Chip select high time
        using CKMODE = Field<DCR_Reg, 0, 1>; // Mode 0 / mode 3
    };

    // status register
    using SR_Reg = Register<QUADSPI_BASE + 0x8, ReadOnly>;
    struct _SR {
        using FLEVEL = Field<SR_Reg, 8, 7>; // FIFO level
        using BUSY = Field<SR_Reg, 5, 1>; // Busy
        using TOF = Field<SR_Reg, 4, 1>; // Timeout flag
        using SMF = Field<SR_Reg, 3, 1>; // Status match flag
        using FTF = Field<SR_Reg, 2, 1>; // FIFO threshold flag
        using TCF = Field<SR_Reg, 1, 1>; // Transfer complete flag
        using TEF = Field<SR_Reg, 0, 1>; // Transfer error flag
    };

    // flag clear register
    using FCR_Reg = Register<QUADSPI_BASE + 0xC, ReadWrite>;
    struct _FCR {
        using CTOF = Field<FCR_Reg, 4, 1>; // Clear timeout flag
        using CSMF = Field<FCR_Reg, 3, 1>; // Clear status match flag
        using CTCF = Field<FCR_Reg, 1, 1>; // Clear transfer complete flag
        using CTEF = Field<FCR_Reg, 0, 1>; // Clear transfer error flag
    };

    // data length register
    using DLR_Reg = Register<QUADSPI_BASE + 0x10, ReadWrite>;
    struct _DLR {
        using DL = Field<DLR_Reg, 0, 32>; // Data length
    };

    // communication configuration register
    using CCR_Reg = Register<QUADSPI_BASE + 0x14, ReadWrite>;
    struct _CCR {
        using DDRM = Field<CCR_Reg, 31, 1>; // Double data rate mode
        using DHHC = Field<CCR_Reg, 30, 1>; // DDR hold half cycle
        using SIOO = Field<CCR_Reg, 28, 1>; // Send instruction only once mode
        using FMODE = Field<CCR_Reg, 26, 2>; // Functional mode
        using DMODE = Field<CCR_Reg, 24, 2>; // Data mode
        using DCYC = Field<CCR_Reg, 18, 5>; // Number of dummy cycles
        using ABSIZE = Field<CCR_Reg, 16, 2>; // Alternate bytes size
        using ABMODE = Field<CCR_Reg, 14, 2>; // Alternate bytes mode
        using ADSIZE = Field<CCR_Reg, 12, 2>; // Address size
        using ADMODE = Field<CCR_Reg, 10, 2>; // Address mode
        using IMODE = Field<CCR_Reg, 8, 2>; // Instruction mode
        using INSTRUCTION = Field<CCR_Reg, 0, 8>; // Instruction
    };

    // address register
    using AR_Reg = Register<QUADSPI_BASE + 0x18, ReadWrite>;
    struct _AR {
        using ADDRESS = Field<AR_Reg, 0, 32>; // Address
    };

    // ABR
    using ABR_Reg = Register<QUADSPI_BASE + 0x1C, ReadWrite>;
    struct _ABR {
        using ALTERNATE = Field<ABR_Reg, 0, 32>; // ALTERNATE
    };

    // data register
    using DR_Reg = Register<QUADSPI_BASE + 0x20, ReadWrite>;
    struct _DR {
        using DATA = Field<DR_Reg, 0, 32>; // Data
    };

    // polling status mask register
    using PSMKR_Reg = Register<QUADSPI_BASE + 0x24, ReadWrite>;
    struct _PSMKR {
        using MASK = Field<PSMKR_Reg, 0, 32>; // Status mask
    };

    // polling status match register
    using PSMAR_Reg = Register<QUADSPI_BASE + 0x28, ReadWrite>;
    struct _PSMAR {
        using MATCH = Field<PSMAR_Reg, 0, 32>; // Status match
    };

    // polling interval register
    using PIR_Reg = Register<QUADSPI_BASE + 0x2C, ReadWrite>;
    struct _PIR {
        using INTERVAL = Field<PIR_Reg, 0, 16>; // Polling interval
    };

    // low-power timeout register
    using LPTR_Reg = Register<QUADSPI_BASE + 0x30, ReadWrite>;
    struct _LPTR {
        using TIMEOUT = Field<LPTR_Reg, 0, 16>; // Timeout period
    };

} // namespace QUADSPI

// --------------------------------------------
// CEC: HDMI-CEC controller
// Base address: 0x40006C00
// --------------------------------------------

namespace CEC {
    static constexpr uint32_t CEC_BASE = 0x40006C00;

    // control register
    using CR_Reg = Register<CEC_BASE + 0x0, ReadWrite>;
    struct _CR {
        using TXEOM = Field<CR_Reg, 2, 1>; // Tx End Of Message
        using TXSOM = Field<CR_Reg, 1, 1>; // Tx start of message
        using CECEN = Field<CR_Reg, 0, 1>; // CEC Enable
    };

    // configuration register
    using CFGR_Reg = Register<CEC_BASE + 0x4, ReadWrite>;
    struct _CFGR {
        using SFT = Field<CFGR_Reg, 0, 3>; // Signal Free Time
        using RXTOL = Field<CFGR_Reg, 3, 1>; // Rx-Tolerance
        using BRESTP = Field<CFGR_Reg, 4, 1>; // Rx-stop on bit rising error
        using BREGEN = Field<CFGR_Reg, 5, 1>; // Generate error-bit on bit rising error
        using LBPEGEN = Field<CFGR_Reg, 6, 1>; // Generate Error-Bit on Long Bit Period Error
        using BRDNOGEN = Field<CFGR_Reg, 7, 1>; // Avoid Error-Bit Generation in Broadcast
        using SFTOP = Field<CFGR_Reg, 8, 1>; // SFT Option Bit
        using OAR = Field<CFGR_Reg, 16, 15>; // Own addresses configuration
        using LSTN = Field<CFGR_Reg, 31, 1>; // Listen mode
    };

    // Tx data register
    using TXDR_Reg = Register<CEC_BASE + 0x8, WriteOnly>;
    struct _TXDR {
        using TXD = Field<TXDR_Reg, 0, 8>; // Tx Data register
    };

    // Rx Data Register
    using RXDR_Reg = Register<CEC_BASE + 0xC, ReadOnly>;
    struct _RXDR {
        using RXDR = Field<RXDR_Reg, 0, 8>; // CEC Rx Data Register
    };

    // Interrupt and Status Register
    using ISR_Reg = Register<CEC_BASE + 0x10, ReadWrite>;
    struct _ISR {
        using TXACKE = Field<ISR_Reg, 12, 1>; // Tx-Missing acknowledge error
        using TXERR = Field<ISR_Reg, 11, 1>; // Tx-Error
        using TXUDR = Field<ISR_Reg, 10, 1>; // Tx-Buffer Underrun
        using TXEND = Field<ISR_Reg, 9, 1>; // End of Transmission
        using TXBR = Field<ISR_Reg, 8, 1>; // Tx-Byte Request
        using ARBLST = Field<ISR_Reg, 7, 1>; // Arbitration Lost
        using RXACKE = Field<ISR_Reg, 6, 1>; // Rx-Missing Acknowledge
        using LBPE = Field<ISR_Reg, 5, 1>; // Rx-Long Bit Period Error
        using SBPE = Field<ISR_Reg, 4, 1>; // Rx-Short Bit period error
        using BRE = Field<ISR_Reg, 3, 1>; // Rx-Bit rising error
        using RXOVR = Field<ISR_Reg, 2, 1>; // Rx-Overrun
        using RXEND = Field<ISR_Reg, 1, 1>; // End Of Reception
        using RXBR = Field<ISR_Reg, 0, 1>; // Rx-Byte Received
    };

    // interrupt enable register
    using IER_Reg = Register<CEC_BASE + 0x14, ReadWrite>;
    struct _IER {
        using TXACKIE = Field<IER_Reg, 12, 1>; // Tx-Missing Acknowledge Error Interrupt Enable
        using TXERRIE = Field<IER_Reg, 11, 1>; // Tx-Error Interrupt Enable
        using TXUDRIE = Field<IER_Reg, 10, 1>; // Tx-Underrun interrupt enable
        using TXENDIE = Field<IER_Reg, 9, 1>; // Tx-End of message interrupt enable
        using TXBRIE = Field<IER_Reg, 8, 1>; // Tx-Byte Request Interrupt Enable
        using ARBLSTIE = Field<IER_Reg, 7, 1>; // Arbitration Lost Interrupt Enable
        using RXACKIE = Field<IER_Reg, 6, 1>; // Rx-Missing Acknowledge Error Interrupt Enable
        using LBPEIE = Field<IER_Reg, 5, 1>; // Long Bit Period Error Interrupt Enable
        using SBPEIE = Field<IER_Reg, 4, 1>; // Short Bit Period Error Interrupt Enable
        using BREIE = Field<IER_Reg, 3, 1>; // Bit Rising Error Interrupt Enable
        using RXOVRIE = Field<IER_Reg, 2, 1>; // Rx-Buffer Overrun Interrupt Enable
        using RXENDIE = Field<IER_Reg, 1, 1>; // End Of Reception Interrupt Enable
        using RXBRIE = Field<IER_Reg, 0, 1>; // Rx-Byte Received Interrupt Enable
    };

} // namespace CEC

// --------------------------------------------
// SPDIFRX: Receiver Interface
// Base address: 0x40004000
// --------------------------------------------

namespace SPDIFRX {
    static constexpr uint32_t SPDIFRX_BASE = 0x40004000;

    // Control register
    using CR_Reg = Register<SPDIFRX_BASE + 0x0, ReadWrite>;
    struct _CR {
        using SPDIFEN = Field<CR_Reg, 0, 2>; // Peripheral Block Enable
        using RXDMAEN = Field<CR_Reg, 2, 1>; // Receiver DMA ENable for data flow
        using RXSTEO = Field<CR_Reg, 3, 1>; // STerEO Mode
        using DRFMT = Field<CR_Reg, 4, 2>; // RX Data format
        using PMSK = Field<CR_Reg, 6, 1>; // Mask Parity error bit
        using VMSK = Field<CR_Reg, 7, 1>; // Mask of Validity bit
        using CUMSK = Field<CR_Reg, 8, 1>; // Mask of channel status and user bits
        using PTMSK = Field<CR_Reg, 9, 1>; // Mask of Preamble Type bits
        using CBDMAEN = Field<CR_Reg, 10, 1>; // Control Buffer DMA ENable for control flow
        using CHSEL = Field<CR_Reg, 11, 1>; // Channel Selection
        using NBTR = Field<CR_Reg, 12, 2>; // Maximum allowed re-tries during synchronization phase
        using WFA = Field<CR_Reg, 14, 1>; // Wait For Activity
        using INSEL = Field<CR_Reg, 16, 3>; // input selection
    };

    // Interrupt mask register
    using IMR_Reg = Register<SPDIFRX_BASE + 0x4, ReadWrite>;
    struct _IMR {
        using RXNEIE = Field<IMR_Reg, 0, 1>; // RXNE interrupt enable
        using CSRNEIE = Field<IMR_Reg, 1, 1>; // Control Buffer Ready Interrupt Enable
        using PERRIE = Field<IMR_Reg, 2, 1>; // Parity error interrupt enable
        using OVRIE = Field<IMR_Reg, 3, 1>; // Overrun error Interrupt Enable
        using SBLKIE = Field<IMR_Reg, 4, 1>; // Synchronization Block Detected Interrupt Enable
        using SYNCDIE = Field<IMR_Reg, 5, 1>; // Synchronization Done
        using IFEIE = Field<IMR_Reg, 6, 1>; // Serial Interface Error Interrupt Enable
    };

    // Status register
    using SR_Reg = Register<SPDIFRX_BASE + 0x8, ReadOnly>;
    struct _SR {
        using RXNE = Field<SR_Reg, 0, 1>; // Read data register not empty
        using CSRNE = Field<SR_Reg, 1, 1>; // Control Buffer register is not empty
        using PERR = Field<SR_Reg, 2, 1>; // Parity error
        using OVR = Field<SR_Reg, 3, 1>; // Overrun error
        using SBD = Field<SR_Reg, 4, 1>; // Synchronization Block Detected
        using SYNCD = Field<SR_Reg, 5, 1>; // Synchronization Done
        using FERR = Field<SR_Reg, 6, 1>; // Framing error
        using SERR = Field<SR_Reg, 7, 1>; // Synchronization error
        using TERR = Field<SR_Reg, 8, 1>; // Time-out error
        using WIDTH5 = Field<SR_Reg, 16, 15>; // Duration of 5 symbols counted with SPDIF_CLK
    };

    // Interrupt Flag Clear register
    using IFCR_Reg = Register<SPDIFRX_BASE + 0xC, WriteOnly>;
    struct _IFCR {
        using PERRCF = Field<IFCR_Reg, 2, 1>; // Clears the Parity error flag
        using OVRCF = Field<IFCR_Reg, 3, 1>; // Clears the Overrun error flag
        using SBDCF = Field<IFCR_Reg, 4, 1>; // Clears the Synchronization Block Detected flag
        using SYNCDCF = Field<IFCR_Reg, 5, 1>; // Clears the Synchronization Done flag
    };

    // Data input register
    using DR_Reg = Register<SPDIFRX_BASE + 0x10, ReadOnly>;
    struct _DR {
        using DR = Field<DR_Reg, 0, 24>; // Parity Error bit
        using PE = Field<DR_Reg, 24, 1>; // Parity Error bit
        using V = Field<DR_Reg, 25, 1>; // Validity bit
        using U = Field<DR_Reg, 26, 1>; // User bit
        using C = Field<DR_Reg, 27, 1>; // Channel Status bit
        using PT = Field<DR_Reg, 28, 2>; // Preamble Type
    };

    // Channel Status register
    using CSR_Reg = Register<SPDIFRX_BASE + 0x14, ReadOnly>;
    struct _CSR {
        using USR = Field<CSR_Reg, 0, 16>; // User data information
        using CS = Field<CSR_Reg, 16, 8>; // Channel A status information
        using SOB = Field<CSR_Reg, 24, 1>; // Start Of Block
    };

    // Debug Information register
    using DIR_Reg = Register<SPDIFRX_BASE + 0x18, ReadOnly>;
    struct _DIR {
        using THI = Field<DIR_Reg, 0, 13>; // Threshold HIGH
        using TLO = Field<DIR_Reg, 16, 13>; // Threshold LOW
    };

} // namespace SPDIFRX

// --------------------------------------------
// SDMMC1: Secure digital input/output interface
// Base address: 0x40012C00
// --------------------------------------------

namespace SDMMC1 {
    static constexpr uint32_t SDMMC1_BASE = 0x40012C00;

    // power control register
    using POWER_Reg = Register<SDMMC1_BASE + 0x0, ReadWrite>;
    struct _POWER {
        using PWRCTRL = Field<POWER_Reg, 0, 2>; // PWRCTRL
    };

    // SDI clock control register
    using CLKCR_Reg = Register<SDMMC1_BASE + 0x4, ReadWrite>;
    struct _CLKCR {
        using HWFC_EN = Field<CLKCR_Reg, 14, 1>; // HW Flow Control enable
        using NEGEDGE = Field<CLKCR_Reg, 13, 1>; // SDIO_CK dephasing selection bit
        using WIDBUS = Field<CLKCR_Reg, 11, 2>; // Wide bus mode enable bit
        using BYPASS = Field<CLKCR_Reg, 10, 1>; // Clock divider bypass enable bit
        using PWRSAV = Field<CLKCR_Reg, 9, 1>; // Power saving configuration bit
        using CLKEN = Field<CLKCR_Reg, 8, 1>; // Clock enable bit
        using CLKDIV = Field<CLKCR_Reg, 0, 8>; // Clock divide factor
    };

    // argument register
    using ARG_Reg = Register<SDMMC1_BASE + 0x8, ReadWrite>;
    struct _ARG {
        using CMDARG = Field<ARG_Reg, 0, 32>; // Command argument
    };

    // command register
    using CMD_Reg = Register<SDMMC1_BASE + 0xC, ReadWrite>;
    struct _CMD {
        using CE_ATACMD = Field<CMD_Reg, 14, 1>; // CE-ATA command
        using nIEN = Field<CMD_Reg, 13, 1>; // not Interrupt Enable
        using ENCMDcompl = Field<CMD_Reg, 12, 1>; // Enable CMD completion
        using SDIOSuspend = Field<CMD_Reg, 11, 1>; // SD I/O suspend command
        using CPSMEN = Field<CMD_Reg, 10, 1>; // Command path state machine (CPSM) Enable bit
        using WAITPEND = Field<CMD_Reg, 9, 1>; // CPSM Waits for ends of data transfer (CmdPend internal signal)
        using WAITINT = Field<CMD_Reg, 8, 1>; // CPSM waits for interrupt request
        using WAITRESP = Field<CMD_Reg, 6, 2>; // Wait for response bits
        using CMDINDEX = Field<CMD_Reg, 0, 6>; // Command index
    };

    // command response register
    using RESPCMD_Reg = Register<SDMMC1_BASE + 0x10, ReadOnly>;
    struct _RESPCMD {
        using RESPCMD = Field<RESPCMD_Reg, 0, 6>; // Response command index
    };

    // response 1..4 register
    using RESP1_Reg = Register<SDMMC1_BASE + 0x14, ReadOnly>;
    struct _RESP1 {
        using CARDSTATUS1 = Field<RESP1_Reg, 0, 32>; // see Table 132
    };

    // response 1..4 register
    using RESP2_Reg = Register<SDMMC1_BASE + 0x18, ReadOnly>;
    struct _RESP2 {
        using CARDSTATUS2 = Field<RESP2_Reg, 0, 32>; // see Table 132
    };

    // response 1..4 register
    using RESP3_Reg = Register<SDMMC1_BASE + 0x1C, ReadOnly>;
    struct _RESP3 {
        using CARDSTATUS3 = Field<RESP3_Reg, 0, 32>; // see Table 132
    };

    // response 1..4 register
    using RESP4_Reg = Register<SDMMC1_BASE + 0x20, ReadOnly>;
    struct _RESP4 {
        using CARDSTATUS4 = Field<RESP4_Reg, 0, 32>; // see Table 132
    };

    // data timer register
    using DTIMER_Reg = Register<SDMMC1_BASE + 0x24, ReadWrite>;
    struct _DTIMER {
        using DATATIME = Field<DTIMER_Reg, 0, 32>; // Data timeout period
    };

    // data length register
    using DLEN_Reg = Register<SDMMC1_BASE + 0x28, ReadWrite>;
    struct _DLEN {
        using DATALENGTH = Field<DLEN_Reg, 0, 25>; // Data length value
    };

    // data control register
    using DCTRL_Reg = Register<SDMMC1_BASE + 0x2C, ReadWrite>;
    struct _DCTRL {
        using SDIOEN = Field<DCTRL_Reg, 11, 1>; // SD I/O enable functions
        using RWMOD = Field<DCTRL_Reg, 10, 1>; // Read wait mode
        using RWSTOP = Field<DCTRL_Reg, 9, 1>; // Read wait stop
        using RWSTART = Field<DCTRL_Reg, 8, 1>; // Read wait start
        using DBLOCKSIZE = Field<DCTRL_Reg, 4, 4>; // Data block size
        using DMAEN = Field<DCTRL_Reg, 3, 1>; // DMA enable bit
        using DTMODE = Field<DCTRL_Reg, 2, 1>; // Data transfer mode selection 1: Stream or SDIO multibyte data transfer
        using DTDIR = Field<DCTRL_Reg, 1, 1>; // Data transfer direction selection
        using DTEN = Field<DCTRL_Reg, 0, 1>; // DTEN
    };

    // data counter register
    using DCOUNT_Reg = Register<SDMMC1_BASE + 0x30, ReadOnly>;
    struct _DCOUNT {
        using DATACOUNT = Field<DCOUNT_Reg, 0, 25>; // Data count value
    };

    // status register
    using STA_Reg = Register<SDMMC1_BASE + 0x34, ReadOnly>;
    struct _STA {
        using CEATAEND = Field<STA_Reg, 23, 1>; // CE-ATA command completion signal received for CMD61
        using SDIOIT = Field<STA_Reg, 22, 1>; // SDIO interrupt received
        using RXDAVL = Field<STA_Reg, 21, 1>; // Data available in receive FIFO
        using TXDAVL = Field<STA_Reg, 20, 1>; // Data available in transmit FIFO
        using RXFIFOE = Field<STA_Reg, 19, 1>; // Receive FIFO empty
        using TXFIFOE = Field<STA_Reg, 18, 1>; // Transmit FIFO empty
        using RXFIFOF = Field<STA_Reg, 17, 1>; // Receive FIFO full
        using TXFIFOF = Field<STA_Reg, 16, 1>; // Transmit FIFO full
        using RXFIFOHF = Field<STA_Reg, 15, 1>; // Receive FIFO half full: there are at least 8 words in the FIFO
        using TXFIFOHE = Field<STA_Reg, 14, 1>; // Transmit FIFO half empty: at least 8 words can be written into the FIFO
        using RXACT = Field<STA_Reg, 13, 1>; // Data receive in progress
        using TXACT = Field<STA_Reg, 12, 1>; // Data transmit in progress
        using CMDACT = Field<STA_Reg, 11, 1>; // Command transfer in progress
        using DBCKEND = Field<STA_Reg, 10, 1>; // Data block sent/received (CRC check passed)
        using STBITERR = Field<STA_Reg, 9, 1>; // Start bit not detected on all data signals in wide bus mode
        using DATAEND = Field<STA_Reg, 8, 1>; // Data end (data counter, SDIDCOUNT, is zero)
        using CMDSENT = Field<STA_Reg, 7, 1>; // Command sent (no response required)
        using CMDREND = Field<STA_Reg, 6, 1>; // Command response received (CRC check passed)
        using RXOVERR = Field<STA_Reg, 5, 1>; // Received FIFO overrun error
        using TXUNDERR = Field<STA_Reg, 4, 1>; // Transmit FIFO underrun error
        using DTIMEOUT = Field<STA_Reg, 3, 1>; // Data timeout
        using CTIMEOUT = Field<STA_Reg, 2, 1>; // Command response timeout
        using DCRCFAIL = Field<STA_Reg, 1, 1>; // Data block sent/received (CRC check failed)
        using CCRCFAIL = Field<STA_Reg, 0, 1>; // Command response received (CRC check failed)
    };

    // interrupt clear register
    using ICR_Reg = Register<SDMMC1_BASE + 0x38, ReadWrite>;
    struct _ICR {
        using CEATAENDC = Field<ICR_Reg, 23, 1>; // CEATAEND flag clear bit
        using SDIOITC = Field<ICR_Reg, 22, 1>; // SDIOIT flag clear bit
        using DBCKENDC = Field<ICR_Reg, 10, 1>; // DBCKEND flag clear bit
        using STBITERRC = Field<ICR_Reg, 9, 1>; // STBITERR flag clear bit
        using DATAENDC = Field<ICR_Reg, 8, 1>; // DATAEND flag clear bit
        using CMDSENTC = Field<ICR_Reg, 7, 1>; // CMDSENT flag clear bit
        using CMDRENDC = Field<ICR_Reg, 6, 1>; // CMDREND flag clear bit
        using RXOVERRC = Field<ICR_Reg, 5, 1>; // RXOVERR flag clear bit
        using TXUNDERRC = Field<ICR_Reg, 4, 1>; // TXUNDERR flag clear bit
        using DTIMEOUTC = Field<ICR_Reg, 3, 1>; // DTIMEOUT flag clear bit
        using CTIMEOUTC = Field<ICR_Reg, 2, 1>; // CTIMEOUT flag clear bit
        using DCRCFAILC = Field<ICR_Reg, 1, 1>; // DCRCFAIL flag clear bit
        using CCRCFAILC = Field<ICR_Reg, 0, 1>; // CCRCFAIL flag clear bit
    };

    // mask register
    using MASK_Reg = Register<SDMMC1_BASE + 0x3C, ReadWrite>;
    struct _MASK {
        using CEATAENDIE = Field<MASK_Reg, 23, 1>; // CE-ATA command completion signal received interrupt enable
        using SDIOITIE = Field<MASK_Reg, 22, 1>; // SDIO mode interrupt received interrupt enable
        using RXDAVLIE = Field<MASK_Reg, 21, 1>; // Data available in Rx FIFO interrupt enable
        using TXDAVLIE = Field<MASK_Reg, 20, 1>; // Data available in Tx FIFO interrupt enable
        using RXFIFOEIE = Field<MASK_Reg, 19, 1>; // Rx FIFO empty interrupt enable
        using TXFIFOEIE = Field<MASK_Reg, 18, 1>; // Tx FIFO empty interrupt enable
        using RXFIFOFIE = Field<MASK_Reg, 17, 1>; // Rx FIFO full interrupt enable
        using TXFIFOFIE = Field<MASK_Reg, 16, 1>; // Tx FIFO full interrupt enable
        using RXFIFOHFIE = Field<MASK_Reg, 15, 1>; // Rx FIFO half full interrupt enable
        using TXFIFOHEIE = Field<MASK_Reg, 14, 1>; // Tx FIFO half empty interrupt enable
        using RXACTIE = Field<MASK_Reg, 13, 1>; // Data receive acting interrupt enable
        using TXACTIE = Field<MASK_Reg, 12, 1>; // Data transmit acting interrupt enable
        using CMDACTIE = Field<MASK_Reg, 11, 1>; // Command acting interrupt enable
        using DBCKENDIE = Field<MASK_Reg, 10, 1>; // Data block end interrupt enable
        using STBITERRIE = Field<MASK_Reg, 9, 1>; // Start bit error interrupt enable
        using DATAENDIE = Field<MASK_Reg, 8, 1>; // Data end interrupt enable
        using CMDSENTIE = Field<MASK_Reg, 7, 1>; // Command sent interrupt enable
        using CMDRENDIE = Field<MASK_Reg, 6, 1>; // Command response received interrupt enable
        using RXOVERRIE = Field<MASK_Reg, 5, 1>; // Rx FIFO overrun error interrupt enable
        using TXUNDERRIE = Field<MASK_Reg, 4, 1>; // Tx FIFO underrun error interrupt enable
        using DTIMEOUTIE = Field<MASK_Reg, 3, 1>; // Data timeout interrupt enable
        using CTIMEOUTIE = Field<MASK_Reg, 2, 1>; // Command timeout interrupt enable
        using DCRCFAILIE = Field<MASK_Reg, 1, 1>; // Data CRC fail interrupt enable
        using CCRCFAILIE = Field<MASK_Reg, 0, 1>; // Command CRC fail interrupt enable
    };

    // FIFO counter register
    using FIFOCNT_Reg = Register<SDMMC1_BASE + 0x48, ReadOnly>;
    struct _FIFOCNT {
        using FIFOCOUNT = Field<FIFOCNT_Reg, 0, 24>; // Remaining number of words to be written to or read from the FIFO
    };

    // data FIFO register
    using FIFO_Reg = Register<SDMMC1_BASE + 0x80, ReadWrite>;
    struct _FIFO {
        using FIFOData = Field<FIFO_Reg, 0, 32>; // Receive and transmit FIFO data
    };

} // namespace SDMMC1

// --------------------------------------------
// SDMMC2: 
// Base address: 0x40011C00
// --------------------------------------------

namespace SDMMC2 {
    static constexpr uint32_t SDMMC2_BASE = 0x40011C00;

} // namespace SDMMC2

// --------------------------------------------
// LPTIM1: Low power timer
// Base address: 0x40002400
// --------------------------------------------

namespace LPTIM1 {
    static constexpr uint32_t LPTIM1_BASE = 0x40002400;

    // Interrupt and Status Register
    using ISR_Reg = Register<LPTIM1_BASE + 0x0, ReadOnly>;
    struct _ISR {
        using DOWN = Field<ISR_Reg, 6, 1>; // Counter direction change up to down
        using UP = Field<ISR_Reg, 5, 1>; // Counter direction change down to up
        using ARROK = Field<ISR_Reg, 4, 1>; // Autoreload register update OK
        using CMPOK = Field<ISR_Reg, 3, 1>; // Compare register update OK
        using EXTTRIG = Field<ISR_Reg, 2, 1>; // External trigger edge event
        using ARRM = Field<ISR_Reg, 1, 1>; // Autoreload match
        using CMPM = Field<ISR_Reg, 0, 1>; // Compare match
    };

    // Interrupt Clear Register
    using ICR_Reg = Register<LPTIM1_BASE + 0x4, WriteOnly>;
    struct _ICR {
        using DOWNCF = Field<ICR_Reg, 6, 1>; // Direction change to down Clear Flag
        using UPCF = Field<ICR_Reg, 5, 1>; // Direction change to UP Clear Flag
        using ARROKCF = Field<ICR_Reg, 4, 1>; // Autoreload register update OK Clear Flag
        using CMPOKCF = Field<ICR_Reg, 3, 1>; // Compare register update OK Clear Flag
        using EXTTRIGCF = Field<ICR_Reg, 2, 1>; // External trigger valid edge Clear Flag
        using ARRMCF = Field<ICR_Reg, 1, 1>; // Autoreload match Clear Flag
        using CMPMCF = Field<ICR_Reg, 0, 1>; // compare match Clear Flag
    };

    // Interrupt Enable Register
    using IER_Reg = Register<LPTIM1_BASE + 0x8, ReadWrite>;
    struct _IER {
        using DOWNIE = Field<IER_Reg, 6, 1>; // Direction change to down Interrupt Enable
        using UPIE = Field<IER_Reg, 5, 1>; // Direction change to UP Interrupt Enable
        using ARROKIE = Field<IER_Reg, 4, 1>; // Autoreload register update OK Interrupt Enable
        using CMPOKIE = Field<IER_Reg, 3, 1>; // Compare register update OK Interrupt Enable
        using EXTTRIGIE = Field<IER_Reg, 2, 1>; // External trigger valid edge Interrupt Enable
        using ARRMIE = Field<IER_Reg, 1, 1>; // Autoreload match Interrupt Enable
        using CMPMIE = Field<IER_Reg, 0, 1>; // Compare match Interrupt Enable
    };

    // Configuration Register
    using CFGR_Reg = Register<LPTIM1_BASE + 0xC, ReadWrite>;
    struct _CFGR {
        using ENC = Field<CFGR_Reg, 24, 1>; // Encoder mode enable
        using COUNTMODE = Field<CFGR_Reg, 23, 1>; // counter mode enabled
        using PRELOAD = Field<CFGR_Reg, 22, 1>; // Registers update mode
        using WAVPOL = Field<CFGR_Reg, 21, 1>; // Waveform shape polarity
        using WAVE = Field<CFGR_Reg, 20, 1>; // Waveform shape
        using TIMOUT = Field<CFGR_Reg, 19, 1>; // Timeout enable
        using TRIGEN = Field<CFGR_Reg, 17, 2>; // Trigger enable and polarity
        using TRIGSEL = Field<CFGR_Reg, 13, 3>; // Trigger selector
        using PRESC = Field<CFGR_Reg, 9, 3>; // Clock prescaler
        using TRGFLT = Field<CFGR_Reg, 6, 2>; // Configurable digital filter for trigger
        using CKFLT = Field<CFGR_Reg, 3, 2>; // Configurable digital filter for external clock
        using CKPOL = Field<CFGR_Reg, 1, 2>; // Clock Polarity
        using CKSEL = Field<CFGR_Reg, 0, 1>; // Clock selector
    };

    // Control Register
    using CR_Reg = Register<LPTIM1_BASE + 0x10, ReadWrite>;
    struct _CR {
        using CNTSTRT = Field<CR_Reg, 2, 1>; // Timer start in continuous mode
        using SNGSTRT = Field<CR_Reg, 1, 1>; // LPTIM start in single mode
        using ENABLE = Field<CR_Reg, 0, 1>; // LPTIM Enable
    };

    // Compare Register
    using CMP_Reg = Register<LPTIM1_BASE + 0x14, ReadWrite>;
    struct _CMP {
        using CMP = Field<CMP_Reg, 0, 16>; // Compare value
    };

    // Autoreload Register
    using ARR_Reg = Register<LPTIM1_BASE + 0x18, ReadWrite>;
    struct _ARR {
        using ARR = Field<ARR_Reg, 0, 16>; // Auto reload value
    };

    // Counter Register
    using CNT_Reg = Register<LPTIM1_BASE + 0x1C, ReadOnly>;
    struct _CNT {
        using CNT = Field<CNT_Reg, 0, 16>; // Counter value
    };

} // namespace LPTIM1

// --------------------------------------------
// I2C1: Inter-integrated circuit
// Base address: 0x40005400
// --------------------------------------------

namespace I2C1 {
    static constexpr uint32_t I2C1_BASE = 0x40005400;

    // Control register 1
    using CR1_Reg = Register<I2C1_BASE + 0x0, ReadWrite>;
    struct _CR1 {
        using PE = Field<CR1_Reg, 0, 1>; // Peripheral enable
        using TXIE = Field<CR1_Reg, 1, 1>; // TX Interrupt enable
        using RXIE = Field<CR1_Reg, 2, 1>; // RX Interrupt enable
        using ADDRIE = Field<CR1_Reg, 3, 1>; // Address match interrupt enable (slave only)
        using NACKIE = Field<CR1_Reg, 4, 1>; // Not acknowledge received interrupt enable
        using STOPIE = Field<CR1_Reg, 5, 1>; // STOP detection Interrupt enable
        using TCIE = Field<CR1_Reg, 6, 1>; // Transfer Complete interrupt enable
        using ERRIE = Field<CR1_Reg, 7, 1>; // Error interrupts enable
        using DNF = Field<CR1_Reg, 8, 4>; // Digital noise filter
        using ANFOFF = Field<CR1_Reg, 12, 1>; // Analog noise filter OFF
        using TXDMAEN = Field<CR1_Reg, 14, 1>; // DMA transmission requests enable
        using RXDMAEN = Field<CR1_Reg, 15, 1>; // DMA reception requests enable
        using SBC = Field<CR1_Reg, 16, 1>; // Slave byte control
        using NOSTRETCH = Field<CR1_Reg, 17, 1>; // Clock stretching disable
        using WUPEN = Field<CR1_Reg, 18, 1>; // Wakeup from STOP enable
        using GCEN = Field<CR1_Reg, 19, 1>; // General call enable
        using SMBHEN = Field<CR1_Reg, 20, 1>; // SMBus Host address enable
        using SMBDEN = Field<CR1_Reg, 21, 1>; // SMBus Device Default address enable
        using ALERTEN = Field<CR1_Reg, 22, 1>; // SMBUS alert enable
        using PECEN = Field<CR1_Reg, 23, 1>; // PEC enable
    };

    // Control register 2
    using CR2_Reg = Register<I2C1_BASE + 0x4, ReadWrite>;
    struct _CR2 {
        using PECBYTE = Field<CR2_Reg, 26, 1>; // Packet error checking byte
        using AUTOEND = Field<CR2_Reg, 25, 1>; // Automatic end mode (master mode)
        using RELOAD = Field<CR2_Reg, 24, 1>; // NBYTES reload mode
        using NBYTES = Field<CR2_Reg, 16, 8>; // Number of bytes
        using NACK = Field<CR2_Reg, 15, 1>; // NACK generation (slave mode)
        using STOP = Field<CR2_Reg, 14, 1>; // Stop generation (master mode)
        using START = Field<CR2_Reg, 13, 1>; // Start generation
        using HEAD10R = Field<CR2_Reg, 12, 1>; // 10-bit address header only read direction (master receiver mode)
        using ADD10 = Field<CR2_Reg, 11, 1>; // 10-bit addressing mode (master mode)
        using RD_WRN = Field<CR2_Reg, 10, 1>; // Transfer direction (master mode)
        using SADD = Field<CR2_Reg, 0, 10>; // Slave address bit (master mode)
    };

    // Own address register 1
    using OAR1_Reg = Register<I2C1_BASE + 0x8, ReadWrite>;
    struct _OAR1 {
        using OA1 = Field<OAR1_Reg, 0, 10>; // Interface address
        using OA1MODE = Field<OAR1_Reg, 10, 1>; // Own Address 1 10-bit mode
        using OA1EN = Field<OAR1_Reg, 15, 1>; // Own Address 1 enable
    };

    // Own address register 2
    using OAR2_Reg = Register<I2C1_BASE + 0xC, ReadWrite>;
    struct _OAR2 {
        using OA2 = Field<OAR2_Reg, 1, 7>; // Interface address
        using OA2MSK = Field<OAR2_Reg, 8, 3>; // Own Address 2 masks
        using OA2EN = Field<OAR2_Reg, 15, 1>; // Own Address 2 enable
    };

    // Timing register
    using TIMINGR_Reg = Register<I2C1_BASE + 0x10, ReadWrite>;
    struct _TIMINGR {
        using SCLL = Field<TIMINGR_Reg, 0, 8>; // SCL low period (master mode)
        using SCLH = Field<TIMINGR_Reg, 8, 8>; // SCL high period (master mode)
        using SDADEL = Field<TIMINGR_Reg, 16, 4>; // Data hold time
        using SCLDEL = Field<TIMINGR_Reg, 20, 4>; // Data setup time
        using PRESC = Field<TIMINGR_Reg, 28, 4>; // Timing prescaler
    };

    // Status register 1
    using TIMEOUTR_Reg = Register<I2C1_BASE + 0x14, ReadWrite>;
    struct _TIMEOUTR {
        using TIMEOUTA = Field<TIMEOUTR_Reg, 0, 12>; // Bus timeout A
        using TIDLE = Field<TIMEOUTR_Reg, 12, 1>; // Idle clock timeout detection
        using TIMOUTEN = Field<TIMEOUTR_Reg, 15, 1>; // Clock timeout enable
        using TIMEOUTB = Field<TIMEOUTR_Reg, 16, 12>; // Bus timeout B
        using TEXTEN = Field<TIMEOUTR_Reg, 31, 1>; // Extended clock timeout enable
    };

    // Interrupt and Status register
    using ISR_Reg = Register<I2C1_BASE + 0x18, ReadWrite>;
    struct _ISR {
        using ADDCODE = Field<ISR_Reg, 17, 7>; // Address match code (Slave mode)
        using DIR = Field<ISR_Reg, 16, 1>; // Transfer direction (Slave mode)
        using BUSY = Field<ISR_Reg, 15, 1>; // Bus busy
        using ALERT = Field<ISR_Reg, 13, 1>; // SMBus alert
        using TIMEOUT = Field<ISR_Reg, 12, 1>; // Timeout or t_low detection flag
        using PECERR = Field<ISR_Reg, 11, 1>; // PEC Error in reception
        using OVR = Field<ISR_Reg, 10, 1>; // Overrun/Underrun (slave mode)
        using ARLO = Field<ISR_Reg, 9, 1>; // Arbitration lost
        using BERR = Field<ISR_Reg, 8, 1>; // Bus error
        using TCR = Field<ISR_Reg, 7, 1>; // Transfer Complete Reload
        using TC = Field<ISR_Reg, 6, 1>; // Transfer Complete (master mode)
        using STOPF = Field<ISR_Reg, 5, 1>; // Stop detection flag
        using NACKF = Field<ISR_Reg, 4, 1>; // Not acknowledge received flag
        using ADDR = Field<ISR_Reg, 3, 1>; // Address matched (slave mode)
        using RXNE = Field<ISR_Reg, 2, 1>; // Receive data register not empty (receivers)
        using TXIS = Field<ISR_Reg, 1, 1>; // Transmit interrupt status (transmitters)
        using TXE = Field<ISR_Reg, 0, 1>; // Transmit data register empty (transmitters)
    };

    // Interrupt clear register
    using ICR_Reg = Register<I2C1_BASE + 0x1C, WriteOnly>;
    struct _ICR {
        using ALERTCF = Field<ICR_Reg, 13, 1>; // Alert flag clear
        using TIMOUTCF = Field<ICR_Reg, 12, 1>; // Timeout detection flag clear
        using PECCF = Field<ICR_Reg, 11, 1>; // PEC Error flag clear
        using OVRCF = Field<ICR_Reg, 10, 1>; // Overrun/Underrun flag clear
        using ARLOCF = Field<ICR_Reg, 9, 1>; // Arbitration lost flag clear
        using BERRCF = Field<ICR_Reg, 8, 1>; // Bus error flag clear
        using STOPCF = Field<ICR_Reg, 5, 1>; // Stop detection flag clear
        using NACKCF = Field<ICR_Reg, 4, 1>; // Not Acknowledge flag clear
        using ADDRCF = Field<ICR_Reg, 3, 1>; // Address Matched flag clear
    };

    // PEC register
    using PECR_Reg = Register<I2C1_BASE + 0x20, ReadOnly>;
    struct _PECR {
        using PEC = Field<PECR_Reg, 0, 8>; // Packet error checking register
    };

    // Receive data register
    using RXDR_Reg = Register<I2C1_BASE + 0x24, ReadOnly>;
    struct _RXDR {
        using RXDATA = Field<RXDR_Reg, 0, 8>; // 8-bit receive data
    };

    // Transmit data register
    using TXDR_Reg = Register<I2C1_BASE + 0x28, ReadWrite>;
    struct _TXDR {
        using TXDATA = Field<TXDR_Reg, 0, 8>; // 8-bit transmit data
    };

} // namespace I2C1

// --------------------------------------------
// I2C2: 
// Base address: 0x40005800
// --------------------------------------------

namespace I2C2 {
    static constexpr uint32_t I2C2_BASE = 0x40005800;

} // namespace I2C2

// --------------------------------------------
// I2C3: 
// Base address: 0x40005C00
// --------------------------------------------

namespace I2C3 {
    static constexpr uint32_t I2C3_BASE = 0x40005C00;

} // namespace I2C3

// --------------------------------------------
// I2C4: 
// Base address: 0x40006000
// --------------------------------------------

namespace I2C4 {
    static constexpr uint32_t I2C4_BASE = 0x40006000;

} // namespace I2C4

// --------------------------------------------
// RTC: Real-time clock
// Base address: 0x40002800
// --------------------------------------------

namespace RTC {
    static constexpr uint32_t RTC_BASE = 0x40002800;

    // time register
    using TR_Reg = Register<RTC_BASE + 0x0, ReadWrite>;
    struct _TR {
        using PM = Field<TR_Reg, 22, 1>; // AM/PM notation
        using HT = Field<TR_Reg, 20, 2>; // Hour tens in BCD format
        using HU = Field<TR_Reg, 16, 4>; // Hour units in BCD format
        using MNT = Field<TR_Reg, 12, 3>; // Minute tens in BCD format
        using MNU = Field<TR_Reg, 8, 4>; // Minute units in BCD format
        using ST = Field<TR_Reg, 4, 3>; // Second tens in BCD format
        using SU = Field<TR_Reg, 0, 4>; // Second units in BCD format
    };

    // date register
    using DR_Reg = Register<RTC_BASE + 0x4, ReadWrite>;
    struct _DR {
        using YT = Field<DR_Reg, 20, 4>; // Year tens in BCD format
        using YU = Field<DR_Reg, 16, 4>; // Year units in BCD format
        using WDU = Field<DR_Reg, 13, 3>; // Week day units
        using MT = Field<DR_Reg, 12, 1>; // Month tens in BCD format
        using MU = Field<DR_Reg, 8, 4>; // Month units in BCD format
        using DT = Field<DR_Reg, 4, 2>; // Date tens in BCD format
        using DU = Field<DR_Reg, 0, 4>; // Date units in BCD format
    };

    // control register
    using CR_Reg = Register<RTC_BASE + 0x8, ReadWrite>;
    struct _CR {
        using WCKSEL = Field<CR_Reg, 0, 3>; // Wakeup clock selection
        using TSEDGE = Field<CR_Reg, 3, 1>; // Time-stamp event active edge
        using REFCKON = Field<CR_Reg, 4, 1>; // Reference clock detection enable (50 or 60 Hz)
        using BYPSHAD = Field<CR_Reg, 5, 1>; // Bypass the shadow registers
        using FMT = Field<CR_Reg, 6, 1>; // Hour format
        using ALRAE = Field<CR_Reg, 8, 1>; // Alarm A enable
        using ALRBE = Field<CR_Reg, 9, 1>; // Alarm B enable
        using WUTE = Field<CR_Reg, 10, 1>; // Wakeup timer enable
        using TSE = Field<CR_Reg, 11, 1>; // Time stamp enable
        using ALRAIE = Field<CR_Reg, 12, 1>; // Alarm A interrupt enable
        using ALRBIE = Field<CR_Reg, 13, 1>; // Alarm B interrupt enable
        using WUTIE = Field<CR_Reg, 14, 1>; // Wakeup timer interrupt enable
        using TSIE = Field<CR_Reg, 15, 1>; // Time-stamp interrupt enable
        using ADD1H = Field<CR_Reg, 16, 1>; // Add 1 hour (summer time change)
        using SUB1H = Field<CR_Reg, 17, 1>; // Subtract 1 hour (winter time change)
        using BKP = Field<CR_Reg, 18, 1>; // Backup
        using COSEL = Field<CR_Reg, 19, 1>; // Calibration output selection
        using POL = Field<CR_Reg, 20, 1>; // Output polarity
        using OSEL = Field<CR_Reg, 21, 2>; // Output selection
        using COE = Field<CR_Reg, 23, 1>; // Calibration output enable
        using ITSE = Field<CR_Reg, 24, 1>; // timestamp on internal event enable
    };

    // initialization and status register
    using ISR_Reg = Register<RTC_BASE + 0xC, ReadWrite>;
    struct _ISR {
        using ALRAWF = Field<ISR_Reg, 0, 1>; // Alarm A write flag
        using ALRBWF = Field<ISR_Reg, 1, 1>; // Alarm B write flag
        using WUTWF = Field<ISR_Reg, 2, 1>; // Wakeup timer write flag
        using SHPF = Field<ISR_Reg, 3, 1>; // Shift operation pending
        using INITS = Field<ISR_Reg, 4, 1>; // Initialization status flag
        using RSF = Field<ISR_Reg, 5, 1>; // Registers synchronization flag
        using INITF = Field<ISR_Reg, 6, 1>; // Initialization flag
        using INIT = Field<ISR_Reg, 7, 1>; // Initialization mode
        using ALRAF = Field<ISR_Reg, 8, 1>; // Alarm A flag
        using ALRBF = Field<ISR_Reg, 9, 1>; // Alarm B flag
        using WUTF = Field<ISR_Reg, 10, 1>; // Wakeup timer flag
        using TSF = Field<ISR_Reg, 11, 1>; // Time-stamp flag
        using TSOVF = Field<ISR_Reg, 12, 1>; // Time-stamp overflow flag
        using TAMP1F = Field<ISR_Reg, 13, 1>; // Tamper detection flag
        using TAMP2F = Field<ISR_Reg, 14, 1>; // RTC_TAMP2 detection flag
        using TAMP3F = Field<ISR_Reg, 15, 1>; // RTC_TAMP3 detection flag
        using RECALPF = Field<ISR_Reg, 16, 1>; // Recalibration pending Flag
    };

    // prescaler register
    using PRER_Reg = Register<RTC_BASE + 0x10, ReadWrite>;
    struct _PRER {
        using PREDIV_A = Field<PRER_Reg, 16, 7>; // Asynchronous prescaler factor
        using PREDIV_S = Field<PRER_Reg, 0, 15>; // Synchronous prescaler factor
    };

    // wakeup timer register
    using WUTR_Reg = Register<RTC_BASE + 0x14, ReadWrite>;
    struct _WUTR {
        using WUT = Field<WUTR_Reg, 0, 16>; // Wakeup auto-reload value bits
    };

    // alarm A register
    using ALRMAR_Reg = Register<RTC_BASE + 0x1C, ReadWrite>;
    struct _ALRMAR {
        using MSK4 = Field<ALRMAR_Reg, 31, 1>; // Alarm A date mask
        using WDSEL = Field<ALRMAR_Reg, 30, 1>; // Week day selection
        using DT = Field<ALRMAR_Reg, 28, 2>; // Date tens in BCD format
        using DU = Field<ALRMAR_Reg, 24, 4>; // Date units or day in BCD format
        using MSK3 = Field<ALRMAR_Reg, 23, 1>; // Alarm A hours mask
        using PM = Field<ALRMAR_Reg, 22, 1>; // AM/PM notation
        using HT = Field<ALRMAR_Reg, 20, 2>; // Hour tens in BCD format
        using HU = Field<ALRMAR_Reg, 16, 4>; // Hour units in BCD format
        using MSK2 = Field<ALRMAR_Reg, 15, 1>; // Alarm A minutes mask
        using MNT = Field<ALRMAR_Reg, 12, 3>; // Minute tens in BCD format
        using MNU = Field<ALRMAR_Reg, 8, 4>; // Minute units in BCD format
        using MSK1 = Field<ALRMAR_Reg, 7, 1>; // Alarm A seconds mask
        using ST = Field<ALRMAR_Reg, 4, 3>; // Second tens in BCD format
        using SU = Field<ALRMAR_Reg, 0, 4>; // Second units in BCD format
    };

    // alarm B register
    using ALRMBR_Reg = Register<RTC_BASE + 0x20, ReadWrite>;
    struct _ALRMBR {
        using MSK4 = Field<ALRMBR_Reg, 31, 1>; // Alarm B date mask
        using WDSEL = Field<ALRMBR_Reg, 30, 1>; // Week day selection
        using DT = Field<ALRMBR_Reg, 28, 2>; // Date tens in BCD format
        using DU = Field<ALRMBR_Reg, 24, 4>; // Date units or day in BCD format
        using MSK3 = Field<ALRMBR_Reg, 23, 1>; // Alarm B hours mask
        using PM = Field<ALRMBR_Reg, 22, 1>; // AM/PM notation
        using HT = Field<ALRMBR_Reg, 20, 2>; // Hour tens in BCD format
        using HU = Field<ALRMBR_Reg, 16, 4>; // Hour units in BCD format
        using MSK2 = Field<ALRMBR_Reg, 15, 1>; // Alarm B minutes mask
        using MNT = Field<ALRMBR_Reg, 12, 3>; // Minute tens in BCD format
        using MNU = Field<ALRMBR_Reg, 8, 4>; // Minute units in BCD format
        using MSK1 = Field<ALRMBR_Reg, 7, 1>; // Alarm B seconds mask
        using ST = Field<ALRMBR_Reg, 4, 3>; // Second tens in BCD format
        using SU = Field<ALRMBR_Reg, 0, 4>; // Second units in BCD format
    };

    // write protection register
    using WPR_Reg = Register<RTC_BASE + 0x24, WriteOnly>;
    struct _WPR {
        using KEY = Field<WPR_Reg, 0, 8>; // Write protection key
    };

    // sub second register
    using SSR_Reg = Register<RTC_BASE + 0x28, ReadOnly>;
    struct _SSR {
        using SS = Field<SSR_Reg, 0, 16>; // Sub second value
    };

    // shift control register
    using SHIFTR_Reg = Register<RTC_BASE + 0x2C, WriteOnly>;
    struct _SHIFTR {
        using ADD1S = Field<SHIFTR_Reg, 31, 1>; // Add one second
        using SUBFS = Field<SHIFTR_Reg, 0, 15>; // Subtract a fraction of a second
    };

    // time stamp time register
    using TSTR_Reg = Register<RTC_BASE + 0x30, ReadOnly>;
    struct _TSTR {
        using SU = Field<TSTR_Reg, 0, 4>; // Second units in BCD format
        using ST = Field<TSTR_Reg, 4, 3>; // Second tens in BCD format
        using MNU = Field<TSTR_Reg, 8, 4>; // Minute units in BCD format
        using MNT = Field<TSTR_Reg, 12, 3>; // Minute tens in BCD format
        using HU = Field<TSTR_Reg, 16, 4>; // Hour units in BCD format
        using HT = Field<TSTR_Reg, 20, 2>; // Hour tens in BCD format
        using PM = Field<TSTR_Reg, 22, 1>; // AM/PM notation
    };

    // time stamp date register
    using TSDR_Reg = Register<RTC_BASE + 0x34, ReadOnly>;
    struct _TSDR {
        using WDU = Field<TSDR_Reg, 13, 3>; // Week day units
        using MT = Field<TSDR_Reg, 12, 1>; // Month tens in BCD format
        using MU = Field<TSDR_Reg, 8, 4>; // Month units in BCD format
        using DT = Field<TSDR_Reg, 4, 2>; // Date tens in BCD format
        using DU = Field<TSDR_Reg, 0, 4>; // Date units in BCD format
    };

    // timestamp sub second register
    using TSSSR_Reg = Register<RTC_BASE + 0x38, ReadOnly>;
    struct _TSSSR {
        using SS = Field<TSSSR_Reg, 0, 16>; // Sub second value
    };

    // calibration register
    using CALR_Reg = Register<RTC_BASE + 0x3C, ReadWrite>;
    struct _CALR {
        using CALP = Field<CALR_Reg, 15, 1>; // Increase frequency of RTC by 488.5 ppm
        using CALW8 = Field<CALR_Reg, 14, 1>; // Use an 8-second calibration cycle period
        using CALW16 = Field<CALR_Reg, 13, 1>; // Use a 16-second calibration cycle period
        using CALM = Field<CALR_Reg, 0, 9>; // Calibration minus
    };

    // tamper configuration register
    using TAMPCR_Reg = Register<RTC_BASE + 0x40, ReadWrite>;
    struct _TAMPCR {
        using TAMP1E = Field<TAMPCR_Reg, 0, 1>; // Tamper 1 detection enable
        using TAMP1TRG = Field<TAMPCR_Reg, 1, 1>; // Active level for tamper 1
        using TAMPIE = Field<TAMPCR_Reg, 2, 1>; // Tamper interrupt enable
        using TAMP2E = Field<TAMPCR_Reg, 3, 1>; // Tamper 2 detection enable
        using TAMP2TRG = Field<TAMPCR_Reg, 4, 1>; // Active level for tamper 2
        using TAMP3E = Field<TAMPCR_Reg, 5, 1>; // Tamper 3 detection enable
        using TAMP3TRG = Field<TAMPCR_Reg, 6, 1>; // Active level for tamper 3
        using TAMPTS = Field<TAMPCR_Reg, 7, 1>; // Activate timestamp on tamper detection event
        using TAMPFREQ = Field<TAMPCR_Reg, 8, 3>; // Tamper sampling frequency
        using TAMPFLT = Field<TAMPCR_Reg, 11, 2>; // Tamper filter count
        using TAMPPRCH = Field<TAMPCR_Reg, 13, 2>; // Tamper precharge duration
        using TAMPPUDIS = Field<TAMPCR_Reg, 15, 1>; // TAMPER pull-up disable
        using TAMP1IE = Field<TAMPCR_Reg, 16, 1>; // Tamper 1 interrupt enable
        using TAMP1NOERASE = Field<TAMPCR_Reg, 17, 1>; // Tamper 1 no erase
        using TAMP1MF = Field<TAMPCR_Reg, 18, 1>; // Tamper 1 mask flag
        using TAMP2IE = Field<TAMPCR_Reg, 19, 1>; // Tamper 2 interrupt enable
        using TAMP2NOERASE = Field<TAMPCR_Reg, 20, 1>; // Tamper 2 no erase
        using TAMP2MF = Field<TAMPCR_Reg, 21, 1>; // Tamper 2 mask flag
        using TAMP3IE = Field<TAMPCR_Reg, 22, 1>; // Tamper 3 interrupt enable
        using TAMP3NOERASE = Field<TAMPCR_Reg, 23, 1>; // Tamper 3 no erase
        using TAMP3MF = Field<TAMPCR_Reg, 24, 1>; // Tamper 3 mask flag
    };

    // alarm A sub second register
    using ALRMASSR_Reg = Register<RTC_BASE + 0x44, ReadWrite>;
    struct _ALRMASSR {
        using MASKSS = Field<ALRMASSR_Reg, 24, 4>; // Mask the most-significant bits starting at this bit
        using SS = Field<ALRMASSR_Reg, 0, 15>; // Sub seconds value
    };

    // alarm B sub second register
    using ALRMBSSR_Reg = Register<RTC_BASE + 0x48, ReadWrite>;
    struct _ALRMBSSR {
        using MASKSS = Field<ALRMBSSR_Reg, 24, 4>; // Mask the most-significant bits starting at this bit
        using SS = Field<ALRMBSSR_Reg, 0, 15>; // Sub seconds value
    };

    // option register
    using OR_Reg = Register<RTC_BASE + 0x4C, ReadWrite>;
    struct _OR {
        using RTC_ALARM_TYPE = Field<OR_Reg, 0, 1>; // RTC_ALARM on PC13 output type
        using RTC_OUT_RMP = Field<OR_Reg, 1, 1>; // RTC_OUT remap
    };

    // backup register
    using BKP0R_Reg = Register<RTC_BASE + 0x50, ReadWrite>;
    struct _BKP0R {
        using BKP = Field<BKP0R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP1R_Reg = Register<RTC_BASE + 0x54, ReadWrite>;
    struct _BKP1R {
        using BKP = Field<BKP1R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP2R_Reg = Register<RTC_BASE + 0x58, ReadWrite>;
    struct _BKP2R {
        using BKP = Field<BKP2R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP3R_Reg = Register<RTC_BASE + 0x5C, ReadWrite>;
    struct _BKP3R {
        using BKP = Field<BKP3R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP4R_Reg = Register<RTC_BASE + 0x60, ReadWrite>;
    struct _BKP4R {
        using BKP = Field<BKP4R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP5R_Reg = Register<RTC_BASE + 0x64, ReadWrite>;
    struct _BKP5R {
        using BKP = Field<BKP5R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP6R_Reg = Register<RTC_BASE + 0x68, ReadWrite>;
    struct _BKP6R {
        using BKP = Field<BKP6R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP7R_Reg = Register<RTC_BASE + 0x6C, ReadWrite>;
    struct _BKP7R {
        using BKP = Field<BKP7R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP8R_Reg = Register<RTC_BASE + 0x70, ReadWrite>;
    struct _BKP8R {
        using BKP = Field<BKP8R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP9R_Reg = Register<RTC_BASE + 0x74, ReadWrite>;
    struct _BKP9R {
        using BKP = Field<BKP9R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP10R_Reg = Register<RTC_BASE + 0x78, ReadWrite>;
    struct _BKP10R {
        using BKP = Field<BKP10R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP11R_Reg = Register<RTC_BASE + 0x7C, ReadWrite>;
    struct _BKP11R {
        using BKP = Field<BKP11R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP12R_Reg = Register<RTC_BASE + 0x80, ReadWrite>;
    struct _BKP12R {
        using BKP = Field<BKP12R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP13R_Reg = Register<RTC_BASE + 0x84, ReadWrite>;
    struct _BKP13R {
        using BKP = Field<BKP13R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP14R_Reg = Register<RTC_BASE + 0x88, ReadWrite>;
    struct _BKP14R {
        using BKP = Field<BKP14R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP15R_Reg = Register<RTC_BASE + 0x8C, ReadWrite>;
    struct _BKP15R {
        using BKP = Field<BKP15R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP16R_Reg = Register<RTC_BASE + 0x90, ReadWrite>;
    struct _BKP16R {
        using BKP = Field<BKP16R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP17R_Reg = Register<RTC_BASE + 0x94, ReadWrite>;
    struct _BKP17R {
        using BKP = Field<BKP17R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP18R_Reg = Register<RTC_BASE + 0x98, ReadWrite>;
    struct _BKP18R {
        using BKP = Field<BKP18R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP19R_Reg = Register<RTC_BASE + 0x9C, ReadWrite>;
    struct _BKP19R {
        using BKP = Field<BKP19R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP20R_Reg = Register<RTC_BASE + 0xA0, ReadWrite>;
    struct _BKP20R {
        using BKP = Field<BKP20R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP21R_Reg = Register<RTC_BASE + 0xA4, ReadWrite>;
    struct _BKP21R {
        using BKP = Field<BKP21R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP22R_Reg = Register<RTC_BASE + 0xA8, ReadWrite>;
    struct _BKP22R {
        using BKP = Field<BKP22R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP23R_Reg = Register<RTC_BASE + 0xAC, ReadWrite>;
    struct _BKP23R {
        using BKP = Field<BKP23R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP24R_Reg = Register<RTC_BASE + 0xB0, ReadWrite>;
    struct _BKP24R {
        using BKP = Field<BKP24R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP25R_Reg = Register<RTC_BASE + 0xB4, ReadWrite>;
    struct _BKP25R {
        using BKP = Field<BKP25R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP26R_Reg = Register<RTC_BASE + 0xB8, ReadWrite>;
    struct _BKP26R {
        using BKP = Field<BKP26R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP27R_Reg = Register<RTC_BASE + 0xBC, ReadWrite>;
    struct _BKP27R {
        using BKP = Field<BKP27R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP28R_Reg = Register<RTC_BASE + 0xC0, ReadWrite>;
    struct _BKP28R {
        using BKP = Field<BKP28R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP29R_Reg = Register<RTC_BASE + 0xC4, ReadWrite>;
    struct _BKP29R {
        using BKP = Field<BKP29R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP30R_Reg = Register<RTC_BASE + 0xC8, ReadWrite>;
    struct _BKP30R {
        using BKP = Field<BKP30R_Reg, 0, 32>; // BKP
    };

    // backup register
    using BKP31R_Reg = Register<RTC_BASE + 0xCC, ReadWrite>;
    struct _BKP31R {
        using BKP = Field<BKP31R_Reg, 0, 32>; // BKP
    };

} // namespace RTC

// --------------------------------------------
// USART6: Universal synchronous asynchronous receiver transmitter
// Base address: 0x40011400
// --------------------------------------------

namespace USART6 {
    static constexpr uint32_t USART6_BASE = 0x40011400;

    // Control register 1
    using CR1_Reg = Register<USART6_BASE + 0x0, ReadWrite>;
    struct _CR1 {
        using M1 = Field<CR1_Reg, 28, 1>; // Word length
        using EOBIE = Field<CR1_Reg, 27, 1>; // End of Block interrupt enable
        using RTOIE = Field<CR1_Reg, 26, 1>; // Receiver timeout interrupt enable
        using DEAT4 = Field<CR1_Reg, 25, 1>; // Driver Enable assertion time
        using DEAT3 = Field<CR1_Reg, 24, 1>; // DEAT3
        using DEAT2 = Field<CR1_Reg, 23, 1>; // DEAT2
        using DEAT1 = Field<CR1_Reg, 22, 1>; // DEAT1
        using DEAT0 = Field<CR1_Reg, 21, 1>; // DEAT0
        using DEDT4 = Field<CR1_Reg, 20, 1>; // Driver Enable de-assertion time
        using DEDT3 = Field<CR1_Reg, 19, 1>; // DEDT3
        using DEDT2 = Field<CR1_Reg, 18, 1>; // DEDT2
        using DEDT1 = Field<CR1_Reg, 17, 1>; // DEDT1
        using DEDT0 = Field<CR1_Reg, 16, 1>; // DEDT0
        using OVER8 = Field<CR1_Reg, 15, 1>; // Oversampling mode
        using CMIE = Field<CR1_Reg, 14, 1>; // Character match interrupt enable
        using MME = Field<CR1_Reg, 13, 1>; // Mute mode enable
        using M0 = Field<CR1_Reg, 12, 1>; // Word length
        using WAKE = Field<CR1_Reg, 11, 1>; // Receiver wakeup method
        using PCE = Field<CR1_Reg, 10, 1>; // Parity control enable
        using PS = Field<CR1_Reg, 9, 1>; // Parity selection
        using PEIE = Field<CR1_Reg, 8, 1>; // PE interrupt enable
        using TXEIE = Field<CR1_Reg, 7, 1>; // interrupt enable
        using TCIE = Field<CR1_Reg, 6, 1>; // Transmission complete interrupt enable
        using RXNEIE = Field<CR1_Reg, 5, 1>; // RXNE interrupt enable
        using IDLEIE = Field<CR1_Reg, 4, 1>; // IDLE interrupt enable
        using TE = Field<CR1_Reg, 3, 1>; // Transmitter enable
        using RE = Field<CR1_Reg, 2, 1>; // Receiver enable
        using UESM = Field<CR1_Reg, 1, 1>; // USART enable in Stop mode
        using UE = Field<CR1_Reg, 0, 1>; // USART enable
    };

    // Control register 2
    using CR2_Reg = Register<USART6_BASE + 0x4, ReadWrite>;
    struct _CR2 {
        using ADD4_7 = Field<CR2_Reg, 28, 4>; // Address of the USART node
        using ADD0_3 = Field<CR2_Reg, 24, 4>; // Address of the USART node
        using RTOEN = Field<CR2_Reg, 23, 1>; // Receiver timeout enable
        using ABRMOD1 = Field<CR2_Reg, 22, 1>; // Auto baud rate mode
        using ABRMOD0 = Field<CR2_Reg, 21, 1>; // ABRMOD0
        using ABREN = Field<CR2_Reg, 20, 1>; // Auto baud rate enable
        using MSBFIRST = Field<CR2_Reg, 19, 1>; // Most significant bit first
        using TAINV = Field<CR2_Reg, 18, 1>; // Binary data inversion
        using TXINV = Field<CR2_Reg, 17, 1>; // TX pin active level inversion
        using RXINV = Field<CR2_Reg, 16, 1>; // RX pin active level inversion
        using SWAP = Field<CR2_Reg, 15, 1>; // Swap TX/RX pins
        using LINEN = Field<CR2_Reg, 14, 1>; // LIN mode enable
        using STOP = Field<CR2_Reg, 12, 2>; // STOP bits
        using CLKEN = Field<CR2_Reg, 11, 1>; // Clock enable
        using CPOL = Field<CR2_Reg, 10, 1>; // Clock polarity
        using CPHA = Field<CR2_Reg, 9, 1>; // Clock phase
        using LBCL = Field<CR2_Reg, 8, 1>; // Last bit clock pulse
        using LBDIE = Field<CR2_Reg, 6, 1>; // LIN break detection interrupt enable
        using LBDL = Field<CR2_Reg, 5, 1>; // LIN break detection length
        using ADDM7 = Field<CR2_Reg, 4, 1>; // 7-bit Address Detection/4-bit Address Detection
    };

    // Control register 3
    using CR3_Reg = Register<USART6_BASE + 0x8, ReadWrite>;
    struct _CR3 {
        using WUFIE = Field<CR3_Reg, 22, 1>; // Wakeup from Stop mode interrupt enable
        using WUS = Field<CR3_Reg, 20, 2>; // Wakeup from Stop mode interrupt flag selection
        using SCARCNT = Field<CR3_Reg, 17, 3>; // Smartcard auto-retry count
        using DEP = Field<CR3_Reg, 15, 1>; // Driver enable polarity selection
        using DEM = Field<CR3_Reg, 14, 1>; // Driver enable mode
        using DDRE = Field<CR3_Reg, 13, 1>; // DMA Disable on Reception Error
        using OVRDIS = Field<CR3_Reg, 12, 1>; // Overrun Disable
        using ONEBIT = Field<CR3_Reg, 11, 1>; // One sample bit method enable
        using CTSIE = Field<CR3_Reg, 10, 1>; // CTS interrupt enable
        using CTSE = Field<CR3_Reg, 9, 1>; // CTS enable
        using RTSE = Field<CR3_Reg, 8, 1>; // RTS enable
        using DMAT = Field<CR3_Reg, 7, 1>; // DMA enable transmitter
        using DMAR = Field<CR3_Reg, 6, 1>; // DMA enable receiver
        using SCEN = Field<CR3_Reg, 5, 1>; // Smartcard mode enable
        using NACK = Field<CR3_Reg, 4, 1>; // Smartcard NACK enable
        using HDSEL = Field<CR3_Reg, 3, 1>; // Half-duplex selection
        using IRLP = Field<CR3_Reg, 2, 1>; // Ir low-power
        using IREN = Field<CR3_Reg, 1, 1>; // Ir mode enable
        using EIE = Field<CR3_Reg, 0, 1>; // Error interrupt enable
    };

    // Baud rate register
    using BRR_Reg = Register<USART6_BASE + 0xC, ReadWrite>;
    struct _BRR {
        using DIV_Mantissa = Field<BRR_Reg, 4, 12>; // DIV_Mantissa
        using DIV_Fraction = Field<BRR_Reg, 0, 4>; // DIV_Fraction
    };

    // Guard time and prescaler register
    using GTPR_Reg = Register<USART6_BASE + 0x10, ReadWrite>;
    struct _GTPR {
        using GT = Field<GTPR_Reg, 8, 8>; // Guard time value
        using PSC = Field<GTPR_Reg, 0, 8>; // Prescaler value
    };

    // Receiver timeout register
    using RTOR_Reg = Register<USART6_BASE + 0x14, ReadWrite>;
    struct _RTOR {
        using BLEN = Field<RTOR_Reg, 24, 8>; // Block Length
        using RTO = Field<RTOR_Reg, 0, 24>; // Receiver timeout value
    };

    // Request register
    using RQR_Reg = Register<USART6_BASE + 0x18, WriteOnly>;
    struct _RQR {
        using TXFRQ = Field<RQR_Reg, 4, 1>; // Transmit data flush request
        using RXFRQ = Field<RQR_Reg, 3, 1>; // Receive data flush request
        using MMRQ = Field<RQR_Reg, 2, 1>; // Mute mode request
        using SBKRQ = Field<RQR_Reg, 1, 1>; // Send break request
        using ABRRQ = Field<RQR_Reg, 0, 1>; // Auto baud rate request
    };

    // Interrupt & status register
    using ISR_Reg = Register<USART6_BASE + 0x1C, ReadOnly>;
    struct _ISR {
        using REACK = Field<ISR_Reg, 22, 1>; // REACK
        using TEACK = Field<ISR_Reg, 21, 1>; // TEACK
        using WUF = Field<ISR_Reg, 20, 1>; // WUF
        using RWU = Field<ISR_Reg, 19, 1>; // RWU
        using SBKF = Field<ISR_Reg, 18, 1>; // SBKF
        using CMF = Field<ISR_Reg, 17, 1>; // CMF
        using BUSY = Field<ISR_Reg, 16, 1>; // BUSY
        using ABRF = Field<ISR_Reg, 15, 1>; // ABRF
        using ABRE = Field<ISR_Reg, 14, 1>; // ABRE
        using EOBF = Field<ISR_Reg, 12, 1>; // EOBF
        using RTOF = Field<ISR_Reg, 11, 1>; // RTOF
        using CTS = Field<ISR_Reg, 10, 1>; // CTS
        using CTSIF = Field<ISR_Reg, 9, 1>; // CTSIF
        using LBDF = Field<ISR_Reg, 8, 1>; // LBDF
        using TXE = Field<ISR_Reg, 7, 1>; // TXE
        using TC = Field<ISR_Reg, 6, 1>; // TC
        using RXNE = Field<ISR_Reg, 5, 1>; // RXNE
        using IDLE = Field<ISR_Reg, 4, 1>; // IDLE
        using ORE = Field<ISR_Reg, 3, 1>; // ORE
        using NF = Field<ISR_Reg, 2, 1>; // NF
        using FE = Field<ISR_Reg, 1, 1>; // FE
        using PE = Field<ISR_Reg, 0, 1>; // PE
    };

    // Interrupt flag clear register
    using ICR_Reg = Register<USART6_BASE + 0x20, WriteOnly>;
    struct _ICR {
        using WUCF = Field<ICR_Reg, 20, 1>; // Wakeup from Stop mode clear flag
        using CMCF = Field<ICR_Reg, 17, 1>; // Character match clear flag
        using EOBCF = Field<ICR_Reg, 12, 1>; // End of block clear flag
        using RTOCF = Field<ICR_Reg, 11, 1>; // Receiver timeout clear flag
        using CTSCF = Field<ICR_Reg, 9, 1>; // CTS clear flag
        using LBDCF = Field<ICR_Reg, 8, 1>; // LIN break detection clear flag
        using TCCF = Field<ICR_Reg, 6, 1>; // Transmission complete clear flag
        using IDLECF = Field<ICR_Reg, 4, 1>; // Idle line detected clear flag
        using ORECF = Field<ICR_Reg, 3, 1>; // Overrun error clear flag
        using NCF = Field<ICR_Reg, 2, 1>; // Noise detected clear flag
        using FECF = Field<ICR_Reg, 1, 1>; // Framing error clear flag
        using PECF = Field<ICR_Reg, 0, 1>; // Parity error clear flag
    };

    // Receive data register
    using RDR_Reg = Register<USART6_BASE + 0x24, ReadOnly>;
    struct _RDR {
        using RDR = Field<RDR_Reg, 0, 9>; // Receive data value
    };

    // Transmit data register
    using TDR_Reg = Register<USART6_BASE + 0x28, ReadWrite>;
    struct _TDR {
        using TDR = Field<TDR_Reg, 0, 9>; // Transmit data value
    };

} // namespace USART6

// --------------------------------------------
// USART1: 
// Base address: 0x40011000
// --------------------------------------------

namespace USART1 {
    static constexpr uint32_t USART1_BASE = 0x40011000;

} // namespace USART1

// --------------------------------------------
// USART3: 
// Base address: 0x40004800
// --------------------------------------------

namespace USART3 {
    static constexpr uint32_t USART3_BASE = 0x40004800;

} // namespace USART3

// --------------------------------------------
// USART2: 
// Base address: 0x40004400
// --------------------------------------------

namespace USART2 {
    static constexpr uint32_t USART2_BASE = 0x40004400;

} // namespace USART2

// --------------------------------------------
// UART5: 
// Base address: 0x40005000
// --------------------------------------------

namespace UART5 {
    static constexpr uint32_t UART5_BASE = 0x40005000;

} // namespace UART5

// --------------------------------------------
// UART4: 
// Base address: 0x40004C00
// --------------------------------------------

namespace UART4 {
    static constexpr uint32_t UART4_BASE = 0x40004C00;

} // namespace UART4

// --------------------------------------------
// UART8: 
// Base address: 0x40007C00
// --------------------------------------------

namespace UART8 {
    static constexpr uint32_t UART8_BASE = 0x40007C00;

} // namespace UART8

// --------------------------------------------
// UART7: 
// Base address: 0x40007800
// --------------------------------------------

namespace UART7 {
    static constexpr uint32_t UART7_BASE = 0x40007800;

} // namespace UART7

// --------------------------------------------
// OTG_FS_GLOBAL: USB on the go full speed
// Base address: 0x50000000
// --------------------------------------------

namespace OTG_FS_GLOBAL {
    static constexpr uint32_t OTG_FS_GLOBAL_BASE = 0x50000000;

    // OTG_FS control and status register (OTG_FS_GOTGCTL)
    using OTG_FS_GOTGCTL_Reg = Register<OTG_FS_GLOBAL_BASE + 0x0, ReadWrite>;
    struct _OTG_FS_GOTGCTL {
        using SRQSCS = Field<OTG_FS_GOTGCTL_Reg, 0, 1>; // Session request success
        using SRQ = Field<OTG_FS_GOTGCTL_Reg, 1, 1>; // Session request
        using HNGSCS = Field<OTG_FS_GOTGCTL_Reg, 8, 1>; // Host negotiation success
        using HNPRQ = Field<OTG_FS_GOTGCTL_Reg, 9, 1>; // HNP request
        using HSHNPEN = Field<OTG_FS_GOTGCTL_Reg, 10, 1>; // Host set HNP enable
        using DHNPEN = Field<OTG_FS_GOTGCTL_Reg, 11, 1>; // Device HNP enabled
        using CIDSTS = Field<OTG_FS_GOTGCTL_Reg, 16, 1>; // Connector ID status
        using DBCT = Field<OTG_FS_GOTGCTL_Reg, 17, 1>; // Long/short debounce time
        using ASVLD = Field<OTG_FS_GOTGCTL_Reg, 18, 1>; // A-session valid
        using BSVLD = Field<OTG_FS_GOTGCTL_Reg, 19, 1>; // B-session valid
        using VBVALOEN = Field<OTG_FS_GOTGCTL_Reg, 2, 1>; // VBUS valid override enable
        using VBVALOVAL = Field<OTG_FS_GOTGCTL_Reg, 3, 1>; // VBUS valid override value
        using AVALOEN = Field<OTG_FS_GOTGCTL_Reg, 4, 1>; // A-peripheral session valid override enable
        using AVALOVAL = Field<OTG_FS_GOTGCTL_Reg, 5, 1>; // A-peripheral session valid override value
        using BVALOEN = Field<OTG_FS_GOTGCTL_Reg, 6, 1>; // B-peripheral session valid override enable
        using BVALOVAL = Field<OTG_FS_GOTGCTL_Reg, 7, 1>; // B-peripheral session valid override value
        using EHEN = Field<OTG_FS_GOTGCTL_Reg, 12, 1>; // Embedded host enable
        using OTGVER = Field<OTG_FS_GOTGCTL_Reg, 20, 1>; // OTG version
    };

    // OTG_FS interrupt register (OTG_FS_GOTGINT)
    using OTG_FS_GOTGINT_Reg = Register<OTG_FS_GLOBAL_BASE + 0x4, ReadWrite>;
    struct _OTG_FS_GOTGINT {
        using SEDET = Field<OTG_FS_GOTGINT_Reg, 2, 1>; // Session end detected
        using SRSSCHG = Field<OTG_FS_GOTGINT_Reg, 8, 1>; // Session request success status change
        using HNSSCHG = Field<OTG_FS_GOTGINT_Reg, 9, 1>; // Host negotiation success status change
        using HNGDET = Field<OTG_FS_GOTGINT_Reg, 17, 1>; // Host negotiation detected
        using ADTOCHG = Field<OTG_FS_GOTGINT_Reg, 18, 1>; // A-device timeout change
        using DBCDNE = Field<OTG_FS_GOTGINT_Reg, 19, 1>; // Debounce done
        using IDCHNG = Field<OTG_FS_GOTGINT_Reg, 20, 1>; // ID input pin changed
    };

    // OTG_FS AHB configuration register (OTG_FS_GAHBCFG)
    using OTG_FS_GAHBCFG_Reg = Register<OTG_FS_GLOBAL_BASE + 0x8, ReadWrite>;
    struct _OTG_FS_GAHBCFG {
        using GINT = Field<OTG_FS_GAHBCFG_Reg, 0, 1>; // Global interrupt mask
        using TXFELVL = Field<OTG_FS_GAHBCFG_Reg, 7, 1>; // TxFIFO empty level
        using PTXFELVL = Field<OTG_FS_GAHBCFG_Reg, 8, 1>; // Periodic TxFIFO empty level
    };

    // OTG_FS USB configuration register (OTG_FS_GUSBCFG)
    using OTG_FS_GUSBCFG_Reg = Register<OTG_FS_GLOBAL_BASE + 0xC, ReadWrite>;
    struct _OTG_FS_GUSBCFG {
        using TOCAL = Field<OTG_FS_GUSBCFG_Reg, 0, 3>; // FS timeout calibration
        using PHYSEL = Field<OTG_FS_GUSBCFG_Reg, 6, 1>; // Full Speed serial transceiver select
        using SRPCAP = Field<OTG_FS_GUSBCFG_Reg, 8, 1>; // SRP-capable
        using HNPCAP = Field<OTG_FS_GUSBCFG_Reg, 9, 1>; // HNP-capable
        using TRDT = Field<OTG_FS_GUSBCFG_Reg, 10, 4>; // USB turnaround time
        using FHMOD = Field<OTG_FS_GUSBCFG_Reg, 29, 1>; // Force host mode
        using FDMOD = Field<OTG_FS_GUSBCFG_Reg, 30, 1>; // Force device mode
    };

    // OTG_FS reset register (OTG_FS_GRSTCTL)
    using OTG_FS_GRSTCTL_Reg = Register<OTG_FS_GLOBAL_BASE + 0x10, ReadWrite>;
    struct _OTG_FS_GRSTCTL {
        using CSRST = Field<OTG_FS_GRSTCTL_Reg, 0, 1>; // Core soft reset
        using HSRST = Field<OTG_FS_GRSTCTL_Reg, 1, 1>; // HCLK soft reset
        using FCRST = Field<OTG_FS_GRSTCTL_Reg, 2, 1>; // Host frame counter reset
        using RXFFLSH = Field<OTG_FS_GRSTCTL_Reg, 4, 1>; // RxFIFO flush
        using TXFFLSH = Field<OTG_FS_GRSTCTL_Reg, 5, 1>; // TxFIFO flush
        using TXFNUM = Field<OTG_FS_GRSTCTL_Reg, 6, 5>; // TxFIFO number
        using AHBIDL = Field<OTG_FS_GRSTCTL_Reg, 31, 1>; // AHB master idle
    };

    // OTG_FS core interrupt register (OTG_FS_GINTSTS)
    using OTG_FS_GINTSTS_Reg = Register<OTG_FS_GLOBAL_BASE + 0x14, ReadWrite>;
    struct _OTG_FS_GINTSTS {
        using CMOD = Field<OTG_FS_GINTSTS_Reg, 0, 1>; // Current mode of operation
        using MMIS = Field<OTG_FS_GINTSTS_Reg, 1, 1>; // Mode mismatch interrupt
        using OTGINT = Field<OTG_FS_GINTSTS_Reg, 2, 1>; // OTG interrupt
        using SOF = Field<OTG_FS_GINTSTS_Reg, 3, 1>; // Start of frame
        using RXFLVL = Field<OTG_FS_GINTSTS_Reg, 4, 1>; // RxFIFO non-empty
        using NPTXFE = Field<OTG_FS_GINTSTS_Reg, 5, 1>; // Non-periodic TxFIFO empty
        using GINAKEFF = Field<OTG_FS_GINTSTS_Reg, 6, 1>; // Global IN non-periodic NAK effective
        using GOUTNAKEFF = Field<OTG_FS_GINTSTS_Reg, 7, 1>; // Global OUT NAK effective
        using ESUSP = Field<OTG_FS_GINTSTS_Reg, 10, 1>; // Early suspend
        using USBSUSP = Field<OTG_FS_GINTSTS_Reg, 11, 1>; // USB suspend
        using USBRST = Field<OTG_FS_GINTSTS_Reg, 12, 1>; // USB reset
        using ENUMDNE = Field<OTG_FS_GINTSTS_Reg, 13, 1>; // Enumeration done
        using ISOODRP = Field<OTG_FS_GINTSTS_Reg, 14, 1>; // Isochronous OUT packet dropped interrupt
        using EOPF = Field<OTG_FS_GINTSTS_Reg, 15, 1>; // End of periodic frame interrupt
        using IEPINT = Field<OTG_FS_GINTSTS_Reg, 18, 1>; // IN endpoint interrupt
        using OEPINT = Field<OTG_FS_GINTSTS_Reg, 19, 1>; // OUT endpoint interrupt
        using IISOIXFR = Field<OTG_FS_GINTSTS_Reg, 20, 1>; // Incomplete isochronous IN transfer
        using IPXFR_INCOMPISOOUT = Field<OTG_FS_GINTSTS_Reg, 21, 1>; // Incomplete periodic transfer(Host mode)/Incomplete isochronous OUT transfer(Device mode)
        using HPRTINT = Field<OTG_FS_GINTSTS_Reg, 24, 1>; // Host port interrupt
        using HCINT = Field<OTG_FS_GINTSTS_Reg, 25, 1>; // Host channels interrupt
        using PTXFE = Field<OTG_FS_GINTSTS_Reg, 26, 1>; // Periodic TxFIFO empty
        using CIDSCHG = Field<OTG_FS_GINTSTS_Reg, 28, 1>; // Connector ID status change
        using DISCINT = Field<OTG_FS_GINTSTS_Reg, 29, 1>; // Disconnect detected interrupt
        using SRQINT = Field<OTG_FS_GINTSTS_Reg, 30, 1>; // Session request/new session detected interrupt
        using WKUPINT = Field<OTG_FS_GINTSTS_Reg, 31, 1>; // Resume/remote wakeup detected interrupt
        using RSTDET = Field<OTG_FS_GINTSTS_Reg, 23, 1>; // Reset detected interrupt
    };

    // OTG_FS interrupt mask register (OTG_FS_GINTMSK)
    using OTG_FS_GINTMSK_Reg = Register<OTG_FS_GLOBAL_BASE + 0x18, ReadWrite>;
    struct _OTG_FS_GINTMSK {
        using MMISM = Field<OTG_FS_GINTMSK_Reg, 1, 1>; // Mode mismatch interrupt mask
        using OTGINT = Field<OTG_FS_GINTMSK_Reg, 2, 1>; // OTG interrupt mask
        using SOFM = Field<OTG_FS_GINTMSK_Reg, 3, 1>; // Start of frame mask
        using RXFLVLM = Field<OTG_FS_GINTMSK_Reg, 4, 1>; // Receive FIFO non-empty mask
        using NPTXFEM = Field<OTG_FS_GINTMSK_Reg, 5, 1>; // Non-periodic TxFIFO empty mask
        using GINAKEFFM = Field<OTG_FS_GINTMSK_Reg, 6, 1>; // Global non-periodic IN NAK effective mask
        using GONAKEFFM = Field<OTG_FS_GINTMSK_Reg, 7, 1>; // Global OUT NAK effective mask
        using ESUSPM = Field<OTG_FS_GINTMSK_Reg, 10, 1>; // Early suspend mask
        using USBSUSPM = Field<OTG_FS_GINTMSK_Reg, 11, 1>; // USB suspend mask
        using USBRST = Field<OTG_FS_GINTMSK_Reg, 12, 1>; // USB reset mask
        using ENUMDNEM = Field<OTG_FS_GINTMSK_Reg, 13, 1>; // Enumeration done mask
        using ISOODRPM = Field<OTG_FS_GINTMSK_Reg, 14, 1>; // Isochronous OUT packet dropped interrupt mask
        using EOPFM = Field<OTG_FS_GINTMSK_Reg, 15, 1>; // End of periodic frame interrupt mask
        using IEPINT = Field<OTG_FS_GINTMSK_Reg, 18, 1>; // IN endpoints interrupt mask
        using OEPINT = Field<OTG_FS_GINTMSK_Reg, 19, 1>; // OUT endpoints interrupt mask
        using IISOIXFRM = Field<OTG_FS_GINTMSK_Reg, 20, 1>; // Incomplete isochronous IN transfer mask
        using IPXFRM_IISOOXFRM = Field<OTG_FS_GINTMSK_Reg, 21, 1>; // Incomplete periodic transfer mask(Host mode)/Incomplete isochronous OUT transfer mask(Device mode)
        using PRTIM = Field<OTG_FS_GINTMSK_Reg, 24, 1>; // Host port interrupt mask
        using HCIM = Field<OTG_FS_GINTMSK_Reg, 25, 1>; // Host channels interrupt mask
        using PTXFEM = Field<OTG_FS_GINTMSK_Reg, 26, 1>; // Periodic TxFIFO empty mask
        using CIDSCHGM = Field<OTG_FS_GINTMSK_Reg, 28, 1>; // Connector ID status change mask
        using DISCINT = Field<OTG_FS_GINTMSK_Reg, 29, 1>; // Disconnect detected interrupt mask
        using SRQIM = Field<OTG_FS_GINTMSK_Reg, 30, 1>; // Session request/new session detected interrupt mask
        using WUIM = Field<OTG_FS_GINTMSK_Reg, 31, 1>; // Resume/remote wakeup detected interrupt mask
        using RSTDETM = Field<OTG_FS_GINTMSK_Reg, 23, 1>; // Reset detected interrupt mask
        using LPMIN = Field<OTG_FS_GINTMSK_Reg, 27, 1>; // LPM interrupt mask
    };

    // OTG_FS Receive status debug read(Device mode)
    using OTG_FS_GRXSTSR_Device_Reg = Register<OTG_FS_GLOBAL_BASE + 0x1C, ReadOnly>;
    struct _OTG_FS_GRXSTSR_Device {
        using EPNUM = Field<OTG_FS_GRXSTSR_Device_Reg, 0, 4>; // Endpoint number
        using BCNT = Field<OTG_FS_GRXSTSR_Device_Reg, 4, 11>; // Byte count
        using DPID = Field<OTG_FS_GRXSTSR_Device_Reg, 15, 2>; // Data PID
        using PKTSTS = Field<OTG_FS_GRXSTSR_Device_Reg, 17, 4>; // Packet status
        using FRMNUM = Field<OTG_FS_GRXSTSR_Device_Reg, 21, 4>; // Frame number
    };

    // OTG_FS Receive status debug read(Host mode)
    using OTG_FS_GRXSTSR_Host_Reg = Register<OTG_FS_GLOBAL_BASE + 0x1C, ReadOnly>;
    struct _OTG_FS_GRXSTSR_Host {
        using CHNUM = Field<OTG_FS_GRXSTSR_Host_Reg, 0, 4>; // Endpoint number
        using BCNT = Field<OTG_FS_GRXSTSR_Host_Reg, 4, 11>; // Byte count
        using DPID = Field<OTG_FS_GRXSTSR_Host_Reg, 15, 2>; // Data PID
        using PKTSTS = Field<OTG_FS_GRXSTSR_Host_Reg, 17, 4>; // Packet status
    };

    // OTG_FS Receive FIFO size register (OTG_FS_GRXFSIZ)
    using OTG_FS_GRXFSIZ_Reg = Register<OTG_FS_GLOBAL_BASE + 0x24, ReadWrite>;
    struct _OTG_FS_GRXFSIZ {
        using RXFD = Field<OTG_FS_GRXFSIZ_Reg, 0, 16>; // RxFIFO depth
    };

    // OTG_FS Endpoint 0 Transmit FIFO size
    using OTG_FS_DIEPTXF0_Device_Reg = Register<OTG_FS_GLOBAL_BASE + 0x28, ReadWrite>;
    struct _OTG_FS_DIEPTXF0_Device {
        using TX0FSA = Field<OTG_FS_DIEPTXF0_Device_Reg, 0, 16>; // Endpoint 0 transmit RAM start address
        using TX0FD = Field<OTG_FS_DIEPTXF0_Device_Reg, 16, 16>; // Endpoint 0 TxFIFO depth
    };

    // OTG_FS Host non-periodic transmit FIFO size register
    using OTG_FS_HNPTXFSIZ_Host_Reg = Register<OTG_FS_GLOBAL_BASE + 0x28, ReadWrite>;
    struct _OTG_FS_HNPTXFSIZ_Host {
        using NPTXFSA = Field<OTG_FS_HNPTXFSIZ_Host_Reg, 0, 16>; // Non-periodic transmit RAM start address
        using NPTXFD = Field<OTG_FS_HNPTXFSIZ_Host_Reg, 16, 16>; // Non-periodic TxFIFO depth
    };

    // OTG_FS non-periodic transmit FIFO/queue status register (OTG_FS_GNPTXSTS)
    using OTG_FS_HNPTXSTS_Reg = Register<OTG_FS_GLOBAL_BASE + 0x2C, ReadOnly>;
    struct _OTG_FS_HNPTXSTS {
        using NPTXFSAV = Field<OTG_FS_HNPTXSTS_Reg, 0, 16>; // Non-periodic TxFIFO space available
        using NPTQXSAV = Field<OTG_FS_HNPTXSTS_Reg, 16, 8>; // Non-periodic transmit request queue space available
        using NPTXQTOP = Field<OTG_FS_HNPTXSTS_Reg, 24, 7>; // Top of the non-periodic transmit request queue
    };

    // OTG_FS general core configuration register (OTG_FS_GCCFG)
    using OTG_FS_GCCFG_Reg = Register<OTG_FS_GLOBAL_BASE + 0x38, ReadWrite>;
    struct _OTG_FS_GCCFG {
        using PWRDWN = Field<OTG_FS_GCCFG_Reg, 16, 1>; // Power down
        using BCDEN = Field<OTG_FS_GCCFG_Reg, 17, 1>; // Battery charging detector (BCD) enable
        using DCDEN = Field<OTG_FS_GCCFG_Reg, 18, 1>; // Data contact detection (DCD) mode enable
        using PDEN = Field<OTG_FS_GCCFG_Reg, 19, 1>; // Primary detection (PD) mode enable
        using SDEN = Field<OTG_FS_GCCFG_Reg, 20, 1>; // Secondary detection (SD) mode enable
        using VBDEN = Field<OTG_FS_GCCFG_Reg, 21, 1>; // USB VBUS detection enable
        using DCDET = Field<OTG_FS_GCCFG_Reg, 0, 1>; // Data contact detection (DCD) status
        using PDET = Field<OTG_FS_GCCFG_Reg, 1, 1>; // Primary detection (PD) status
        using SDET = Field<OTG_FS_GCCFG_Reg, 2, 1>; // Secondary detection (SD) status
        using PS2DET = Field<OTG_FS_GCCFG_Reg, 3, 1>; // DM pull-up detection status
    };

    // core ID register
    using OTG_FS_CID_Reg = Register<OTG_FS_GLOBAL_BASE + 0x3C, ReadWrite>;
    struct _OTG_FS_CID {
        using PRODUCT_ID = Field<OTG_FS_CID_Reg, 0, 32>; // Product ID field
    };

    // OTG_FS Host periodic transmit FIFO size register (OTG_FS_HPTXFSIZ)
    using OTG_FS_HPTXFSIZ_Reg = Register<OTG_FS_GLOBAL_BASE + 0x100, ReadWrite>;
    struct _OTG_FS_HPTXFSIZ {
        using PTXSA = Field<OTG_FS_HPTXFSIZ_Reg, 0, 16>; // Host periodic TxFIFO start address
        using PTXFSIZ = Field<OTG_FS_HPTXFSIZ_Reg, 16, 16>; // Host periodic TxFIFO depth
    };

    // OTG_FS device IN endpoint transmit FIFO size register (OTG_FS_DIEPTXF1)
    using OTG_FS_DIEPTXF1_Reg = Register<OTG_FS_GLOBAL_BASE + 0x104, ReadWrite>;
    struct _OTG_FS_DIEPTXF1 {
        using INEPTXSA = Field<OTG_FS_DIEPTXF1_Reg, 0, 16>; // IN endpoint FIFO2 transmit RAM start address
        using INEPTXFD = Field<OTG_FS_DIEPTXF1_Reg, 16, 16>; // IN endpoint TxFIFO depth
    };

    // OTG_FS device IN endpoint transmit FIFO size register (OTG_FS_DIEPTXF2)
    using OTG_FS_DIEPTXF2_Reg = Register<OTG_FS_GLOBAL_BASE + 0x108, ReadWrite>;
    struct _OTG_FS_DIEPTXF2 {
        using INEPTXSA = Field<OTG_FS_DIEPTXF2_Reg, 0, 16>; // IN endpoint FIFO3 transmit RAM start address
        using INEPTXFD = Field<OTG_FS_DIEPTXF2_Reg, 16, 16>; // IN endpoint TxFIFO depth
    };

    // OTG_FS device IN endpoint transmit FIFO size register (OTG_FS_DIEPTXF3)
    using OTG_FS_DIEPTXF3_Reg = Register<OTG_FS_GLOBAL_BASE + 0x10C, ReadWrite>;
    struct _OTG_FS_DIEPTXF3 {
        using INEPTXSA = Field<OTG_FS_DIEPTXF3_Reg, 0, 16>; // IN endpoint FIFO4 transmit RAM start address
        using INEPTXFD = Field<OTG_FS_DIEPTXF3_Reg, 16, 16>; // IN endpoint TxFIFO depth
    };

    // OTG status read and pop register (Device mode)
    using OTG_FS_GRXSTSP_Device_Reg = Register<OTG_FS_GLOBAL_BASE + 0x20, ReadOnly>;
    struct _OTG_FS_GRXSTSP_Device {
        using EPNUM = Field<OTG_FS_GRXSTSP_Device_Reg, 0, 4>; // Endpoint number
        using BCNT = Field<OTG_FS_GRXSTSP_Device_Reg, 4, 11>; // Byte count
        using DPID = Field<OTG_FS_GRXSTSP_Device_Reg, 15, 2>; // Data PID
        using PKTSTS = Field<OTG_FS_GRXSTSP_Device_Reg, 17, 4>; // Packet status
        using FRMNUM = Field<OTG_FS_GRXSTSP_Device_Reg, 21, 4>; // Frame number
    };

    // OTG status read and pop register (Host mode)
    using OTG_FS_GRXSTSP_Host_Reg = Register<OTG_FS_GLOBAL_BASE + 0x20, ReadOnly>;
    struct _OTG_FS_GRXSTSP_Host {
        using CHNUM = Field<OTG_FS_GRXSTSP_Host_Reg, 0, 4>; // Channel number
        using BCNT = Field<OTG_FS_GRXSTSP_Host_Reg, 4, 11>; // Byte count
        using DPID = Field<OTG_FS_GRXSTSP_Host_Reg, 15, 2>; // Data PID
        using PKTSTS = Field<OTG_FS_GRXSTSP_Host_Reg, 17, 4>; // Packet status
    };

    // OTG I2C access register
    using OTG_FS_GI2CCTL_Reg = Register<OTG_FS_GLOBAL_BASE + 0x30, ReadWrite>;
    struct _OTG_FS_GI2CCTL {
        using RWDATA = Field<OTG_FS_GI2CCTL_Reg, 0, 8>; // I2C Read/Write Data
        using REGADDR = Field<OTG_FS_GI2CCTL_Reg, 8, 8>; // I2C Register Address
        using ADDR = Field<OTG_FS_GI2CCTL_Reg, 16, 7>; // I2C Address
        using I2CEN = Field<OTG_FS_GI2CCTL_Reg, 23, 1>; // I2C Enable
        using ACK = Field<OTG_FS_GI2CCTL_Reg, 24, 1>; // I2C ACK
        using I2CDEVADR = Field<OTG_FS_GI2CCTL_Reg, 26, 2>; // I2C Device Address
        using I2CDATSE0 = Field<OTG_FS_GI2CCTL_Reg, 28, 1>; // I2C DatSe0 USB mode
        using RW = Field<OTG_FS_GI2CCTL_Reg, 30, 1>; // Read/Write Indicator
        using BSYDNE = Field<OTG_FS_GI2CCTL_Reg, 31, 1>; // I2C Busy/Done
    };

    // OTG power down register
    using OTG_FS_GPWRDN_Reg = Register<OTG_FS_GLOBAL_BASE + 0x58, ReadWrite>;
    struct _OTG_FS_GPWRDN {
        using ADPMEN = Field<OTG_FS_GPWRDN_Reg, 0, 1>; // ADP module enable
        using ADPIF = Field<OTG_FS_GPWRDN_Reg, 23, 1>; // ADP interrupt flag
    };

    // OTG ADP timer, control and status register
    using OTG_FS_GADPCTL_Reg = Register<OTG_FS_GLOBAL_BASE + 0x60, ReadWrite>;
    struct _OTG_FS_GADPCTL {
        using PRBDSCHG = Field<OTG_FS_GADPCTL_Reg, 0, 2>; // Probe discharge
        using PRBDELTA = Field<OTG_FS_GADPCTL_Reg, 2, 2>; // Probe delta
        using PRBPER = Field<OTG_FS_GADPCTL_Reg, 4, 2>; // Probe period
        using RTIM = Field<OTG_FS_GADPCTL_Reg, 6, 11>; // Ramp time
        using ENAPRB = Field<OTG_FS_GADPCTL_Reg, 17, 1>; // Enable probe
        using ENASNS = Field<OTG_FS_GADPCTL_Reg, 18, 1>; // Enable sense
        using ADPRST = Field<OTG_FS_GADPCTL_Reg, 19, 1>; // ADP reset
        using ADPEN = Field<OTG_FS_GADPCTL_Reg, 20, 1>; // ADP enable
        using ADPPRBIF = Field<OTG_FS_GADPCTL_Reg, 21, 1>; // ADP probe interrupt flag
        using ADPSNSIF = Field<OTG_FS_GADPCTL_Reg, 22, 1>; // ADP sense interrupt flag
        using ADPTOIF = Field<OTG_FS_GADPCTL_Reg, 23, 1>; // ADP timeout interrupt flag
        using ADPPRBIM = Field<OTG_FS_GADPCTL_Reg, 24, 1>; // ADP probe interrupt mask
        using ADPSNSIM = Field<OTG_FS_GADPCTL_Reg, 25, 1>; // ADP sense interrupt mask
        using ADPTOIM = Field<OTG_FS_GADPCTL_Reg, 26, 1>; // ADP timeout interrupt mask
        using AR = Field<OTG_FS_GADPCTL_Reg, 27, 2>; // Access request
    };

    // OTG_FS device IN endpoint transmit FIFO size register (OTG_FS_DIEPTXF4)
    using OTG_FS_DIEPTXF4_Reg = Register<OTG_FS_GLOBAL_BASE + 0x110, ReadWrite>;
    struct _OTG_FS_DIEPTXF4 {
        using INEPTXSA = Field<OTG_FS_DIEPTXF4_Reg, 0, 16>; // IN endpoint FIFOx transmit RAM start address
        using INEPTXFD = Field<OTG_FS_DIEPTXF4_Reg, 16, 16>; // IN endpoint Tx FIFO depth
    };

    // OTG_FS device IN endpoint transmit FIFO size register (OTG_FS_DIEPTXF5)
    using OTG_FS_DIEPTXF5_Reg = Register<OTG_FS_GLOBAL_BASE + 0x114, ReadWrite>;
    struct _OTG_FS_DIEPTXF5 {
        using INEPTXSA = Field<OTG_FS_DIEPTXF5_Reg, 0, 16>; // IN endpoint FIFOx transmit RAM start address
        using INEPTXFD = Field<OTG_FS_DIEPTXF5_Reg, 16, 16>; // IN endpoint Tx FIFO depth
    };

    // OTG core LPM configuration register
    using OTG_FS_GLPMCFG_Reg = Register<OTG_FS_GLOBAL_BASE + 0x54, ReadWrite>;
    struct _OTG_FS_GLPMCFG {
        using LPMEN = Field<OTG_FS_GLPMCFG_Reg, 0, 1>; // LPM support enable
        using LPMACK = Field<OTG_FS_GLPMCFG_Reg, 1, 1>; // LPM token acknowledge enable
        using BESL = Field<OTG_FS_GLPMCFG_Reg, 2, 4>; // Best effort service latency
        using REMWAKE = Field<OTG_FS_GLPMCFG_Reg, 6, 1>; // bRemoteWake value
        using L1SSEN = Field<OTG_FS_GLPMCFG_Reg, 7, 1>; // L1 Shallow Sleep enable
        using BESLTHRS = Field<OTG_FS_GLPMCFG_Reg, 8, 4>; // BESL threshold
        using L1DSEN = Field<OTG_FS_GLPMCFG_Reg, 12, 1>; // L1 deep sleep enable
        using LPMRST = Field<OTG_FS_GLPMCFG_Reg, 13, 2>; // LPM response
        using SLPSTS = Field<OTG_FS_GLPMCFG_Reg, 15, 1>; // Port sleep status
        using L1RSMOK = Field<OTG_FS_GLPMCFG_Reg, 16, 1>; // Sleep State Resume OK
        using LPMCHIDX = Field<OTG_FS_GLPMCFG_Reg, 17, 4>; // LPM Channel Index
        using LPMRCNT = Field<OTG_FS_GLPMCFG_Reg, 21, 3>; // LPM retry count
        using SNDLPM = Field<OTG_FS_GLPMCFG_Reg, 24, 1>; // Send LPM transaction
        using LPMRCNTSTS = Field<OTG_FS_GLPMCFG_Reg, 25, 3>; // LPM retry count status
        using ENBESL = Field<OTG_FS_GLPMCFG_Reg, 28, 1>; // Enable best effort service latency
    };

} // namespace OTG_FS_GLOBAL

// --------------------------------------------
// OTG_HS_GLOBAL: USB on the go high speed
// Base address: 0x40040000
// --------------------------------------------

namespace OTG_HS_GLOBAL {
    static constexpr uint32_t OTG_HS_GLOBAL_BASE = 0x40040000;

    // OTG_HS control and status register
    using OTG_HS_GOTGCTL_Reg = Register<OTG_HS_GLOBAL_BASE + 0x0, ReadWrite>;
    struct _OTG_HS_GOTGCTL {
        using SRQSCS = Field<OTG_HS_GOTGCTL_Reg, 0, 1>; // Session request success
        using SRQ = Field<OTG_HS_GOTGCTL_Reg, 1, 1>; // Session request
        using HNGSCS = Field<OTG_HS_GOTGCTL_Reg, 8, 1>; // Host negotiation success
        using HNPRQ = Field<OTG_HS_GOTGCTL_Reg, 9, 1>; // HNP request
        using HSHNPEN = Field<OTG_HS_GOTGCTL_Reg, 10, 1>; // Host set HNP enable
        using DHNPEN = Field<OTG_HS_GOTGCTL_Reg, 11, 1>; // Device HNP enabled
        using CIDSTS = Field<OTG_HS_GOTGCTL_Reg, 16, 1>; // Connector ID status
        using DBCT = Field<OTG_HS_GOTGCTL_Reg, 17, 1>; // Long/short debounce time
        using ASVLD = Field<OTG_HS_GOTGCTL_Reg, 18, 1>; // A-session valid
        using BSVLD = Field<OTG_HS_GOTGCTL_Reg, 19, 1>; // B-session valid
        using EHEN = Field<OTG_HS_GOTGCTL_Reg, 12, 1>; // Embedded host enable
    };

    // OTG_HS interrupt register
    using OTG_HS_GOTGINT_Reg = Register<OTG_HS_GLOBAL_BASE + 0x4, ReadWrite>;
    struct _OTG_HS_GOTGINT {
        using SEDET = Field<OTG_HS_GOTGINT_Reg, 2, 1>; // Session end detected
        using SRSSCHG = Field<OTG_HS_GOTGINT_Reg, 8, 1>; // Session request success status change
        using HNSSCHG = Field<OTG_HS_GOTGINT_Reg, 9, 1>; // Host negotiation success status change
        using HNGDET = Field<OTG_HS_GOTGINT_Reg, 17, 1>; // Host negotiation detected
        using ADTOCHG = Field<OTG_HS_GOTGINT_Reg, 18, 1>; // A-device timeout change
        using DBCDNE = Field<OTG_HS_GOTGINT_Reg, 19, 1>; // Debounce done
        using IDCHNG = Field<OTG_HS_GOTGINT_Reg, 20, 1>; // ID input pin changed
    };

    // OTG_HS AHB configuration register
    using OTG_HS_GAHBCFG_Reg = Register<OTG_HS_GLOBAL_BASE + 0x8, ReadWrite>;
    struct _OTG_HS_GAHBCFG {
        using GINT = Field<OTG_HS_GAHBCFG_Reg, 0, 1>; // Global interrupt mask
        using HBSTLEN = Field<OTG_HS_GAHBCFG_Reg, 1, 4>; // Burst length/type
        using DMAEN = Field<OTG_HS_GAHBCFG_Reg, 5, 1>; // DMA enable
        using TXFELVL = Field<OTG_HS_GAHBCFG_Reg, 7, 1>; // TxFIFO empty level
        using PTXFELVL = Field<OTG_HS_GAHBCFG_Reg, 8, 1>; // Periodic TxFIFO empty level
    };

    // OTG_HS USB configuration register
    using OTG_HS_GUSBCFG_Reg = Register<OTG_HS_GLOBAL_BASE + 0xC, ReadWrite>;
    struct _OTG_HS_GUSBCFG {
        using TOCAL = Field<OTG_HS_GUSBCFG_Reg, 0, 3>; // FS timeout calibration
        using PHYSEL = Field<OTG_HS_GUSBCFG_Reg, 6, 1>; // USB 2.0 high-speed ULPI PHY or USB 1.1 full-speed serial transceiver select
        using SRPCAP = Field<OTG_HS_GUSBCFG_Reg, 8, 1>; // SRP-capable
        using HNPCAP = Field<OTG_HS_GUSBCFG_Reg, 9, 1>; // HNP-capable
        using TRDT = Field<OTG_HS_GUSBCFG_Reg, 10, 4>; // USB turnaround time
        using PHYLPCS = Field<OTG_HS_GUSBCFG_Reg, 15, 1>; // PHY Low-power clock select
        using ULPIFSLS = Field<OTG_HS_GUSBCFG_Reg, 17, 1>; // ULPI FS/LS select
        using ULPIAR = Field<OTG_HS_GUSBCFG_Reg, 18, 1>; // ULPI Auto-resume
        using ULPICSM = Field<OTG_HS_GUSBCFG_Reg, 19, 1>; // ULPI Clock SuspendM
        using ULPIEVBUSD = Field<OTG_HS_GUSBCFG_Reg, 20, 1>; // ULPI External VBUS Drive
        using ULPIEVBUSI = Field<OTG_HS_GUSBCFG_Reg, 21, 1>; // ULPI external VBUS indicator
        using TSDPS = Field<OTG_HS_GUSBCFG_Reg, 22, 1>; // TermSel DLine pulsing selection
        using PCCI = Field<OTG_HS_GUSBCFG_Reg, 23, 1>; // Indicator complement
        using PTCI = Field<OTG_HS_GUSBCFG_Reg, 24, 1>; // Indicator pass through
        using ULPIIPD = Field<OTG_HS_GUSBCFG_Reg, 25, 1>; // ULPI interface protect disable
        using FHMOD = Field<OTG_HS_GUSBCFG_Reg, 29, 1>; // Forced host mode
        using FDMOD = Field<OTG_HS_GUSBCFG_Reg, 30, 1>; // Forced peripheral mode
    };

    // OTG_HS reset register
    using OTG_HS_GRSTCTL_Reg = Register<OTG_HS_GLOBAL_BASE + 0x10, ReadWrite>;
    struct _OTG_HS_GRSTCTL {
        using CSRST = Field<OTG_HS_GRSTCTL_Reg, 0, 1>; // Core soft reset
        using HSRST = Field<OTG_HS_GRSTCTL_Reg, 1, 1>; // HCLK soft reset
        using FCRST = Field<OTG_HS_GRSTCTL_Reg, 2, 1>; // Host frame counter reset
        using RXFFLSH = Field<OTG_HS_GRSTCTL_Reg, 4, 1>; // RxFIFO flush
        using TXFFLSH = Field<OTG_HS_GRSTCTL_Reg, 5, 1>; // TxFIFO flush
        using TXFNUM = Field<OTG_HS_GRSTCTL_Reg, 6, 5>; // TxFIFO number
        using AHBIDL = Field<OTG_HS_GRSTCTL_Reg, 31, 1>; // AHB master idle
        using DMAREQ = Field<OTG_HS_GRSTCTL_Reg, 30, 1>; // DMA request signal enabled for USB OTG HS
    };

    // OTG_HS core interrupt register
    using OTG_HS_GINTSTS_Reg = Register<OTG_HS_GLOBAL_BASE + 0x14, ReadWrite>;
    struct _OTG_HS_GINTSTS {
        using CMOD = Field<OTG_HS_GINTSTS_Reg, 0, 1>; // Current mode of operation
        using MMIS = Field<OTG_HS_GINTSTS_Reg, 1, 1>; // Mode mismatch interrupt
        using OTGINT = Field<OTG_HS_GINTSTS_Reg, 2, 1>; // OTG interrupt
        using SOF = Field<OTG_HS_GINTSTS_Reg, 3, 1>; // Start of frame
        using RXFLVL = Field<OTG_HS_GINTSTS_Reg, 4, 1>; // RxFIFO nonempty
        using NPTXFE = Field<OTG_HS_GINTSTS_Reg, 5, 1>; // Nonperiodic TxFIFO empty
        using GINAKEFF = Field<OTG_HS_GINTSTS_Reg, 6, 1>; // Global IN nonperiodic NAK effective
        using BOUTNAKEFF = Field<OTG_HS_GINTSTS_Reg, 7, 1>; // Global OUT NAK effective
        using ESUSP = Field<OTG_HS_GINTSTS_Reg, 10, 1>; // Early suspend
        using USBSUSP = Field<OTG_HS_GINTSTS_Reg, 11, 1>; // USB suspend
        using USBRST = Field<OTG_HS_GINTSTS_Reg, 12, 1>; // USB reset
        using ENUMDNE = Field<OTG_HS_GINTSTS_Reg, 13, 1>; // Enumeration done
        using ISOODRP = Field<OTG_HS_GINTSTS_Reg, 14, 1>; // Isochronous OUT packet dropped interrupt
        using EOPF = Field<OTG_HS_GINTSTS_Reg, 15, 1>; // End of periodic frame interrupt
        using IEPINT = Field<OTG_HS_GINTSTS_Reg, 18, 1>; // IN endpoint interrupt
        using OEPINT = Field<OTG_HS_GINTSTS_Reg, 19, 1>; // OUT endpoint interrupt
        using IISOIXFR = Field<OTG_HS_GINTSTS_Reg, 20, 1>; // Incomplete isochronous IN transfer
        using PXFR_INCOMPISOOUT = Field<OTG_HS_GINTSTS_Reg, 21, 1>; // Incomplete periodic transfer
        using DATAFSUSP = Field<OTG_HS_GINTSTS_Reg, 22, 1>; // Data fetch suspended
        using HPRTINT = Field<OTG_HS_GINTSTS_Reg, 24, 1>; // Host port interrupt
        using HCINT = Field<OTG_HS_GINTSTS_Reg, 25, 1>; // Host channels interrupt
        using PTXFE = Field<OTG_HS_GINTSTS_Reg, 26, 1>; // Periodic TxFIFO empty
        using CIDSCHG = Field<OTG_HS_GINTSTS_Reg, 28, 1>; // Connector ID status change
        using DISCINT = Field<OTG_HS_GINTSTS_Reg, 29, 1>; // Disconnect detected interrupt
        using SRQINT = Field<OTG_HS_GINTSTS_Reg, 30, 1>; // Session request/new session detected interrupt
        using WKUINT = Field<OTG_HS_GINTSTS_Reg, 31, 1>; // Resume/remote wakeup detected interrupt
    };

    // OTG_HS interrupt mask register
    using OTG_HS_GINTMSK_Reg = Register<OTG_HS_GLOBAL_BASE + 0x18, ReadWrite>;
    struct _OTG_HS_GINTMSK {
        using MMISM = Field<OTG_HS_GINTMSK_Reg, 1, 1>; // Mode mismatch interrupt mask
        using OTGINT = Field<OTG_HS_GINTMSK_Reg, 2, 1>; // OTG interrupt mask
        using SOFM = Field<OTG_HS_GINTMSK_Reg, 3, 1>; // Start of frame mask
        using RXFLVLM = Field<OTG_HS_GINTMSK_Reg, 4, 1>; // Receive FIFO nonempty mask
        using NPTXFEM = Field<OTG_HS_GINTMSK_Reg, 5, 1>; // Nonperiodic TxFIFO empty mask
        using GINAKEFFM = Field<OTG_HS_GINTMSK_Reg, 6, 1>; // Global nonperiodic IN NAK effective mask
        using GONAKEFFM = Field<OTG_HS_GINTMSK_Reg, 7, 1>; // Global OUT NAK effective mask
        using ESUSPM = Field<OTG_HS_GINTMSK_Reg, 10, 1>; // Early suspend mask
        using USBSUSPM = Field<OTG_HS_GINTMSK_Reg, 11, 1>; // USB suspend mask
        using USBRST = Field<OTG_HS_GINTMSK_Reg, 12, 1>; // USB reset mask
        using ENUMDNEM = Field<OTG_HS_GINTMSK_Reg, 13, 1>; // Enumeration done mask
        using ISOODRPM = Field<OTG_HS_GINTMSK_Reg, 14, 1>; // Isochronous OUT packet dropped interrupt mask
        using EOPFM = Field<OTG_HS_GINTMSK_Reg, 15, 1>; // End of periodic frame interrupt mask
        using IEPINT = Field<OTG_HS_GINTMSK_Reg, 18, 1>; // IN endpoints interrupt mask
        using OEPINT = Field<OTG_HS_GINTMSK_Reg, 19, 1>; // OUT endpoints interrupt mask
        using IISOIXFRM = Field<OTG_HS_GINTMSK_Reg, 20, 1>; // Incomplete isochronous IN transfer mask
        using PXFRM_IISOOXFRM = Field<OTG_HS_GINTMSK_Reg, 21, 1>; // Incomplete periodic transfer mask
        using FSUSPM = Field<OTG_HS_GINTMSK_Reg, 22, 1>; // Data fetch suspended mask
        using PRTIM = Field<OTG_HS_GINTMSK_Reg, 24, 1>; // Host port interrupt mask
        using HCIM = Field<OTG_HS_GINTMSK_Reg, 25, 1>; // Host channels interrupt mask
        using PTXFEM = Field<OTG_HS_GINTMSK_Reg, 26, 1>; // Periodic TxFIFO empty mask
        using CIDSCHGM = Field<OTG_HS_GINTMSK_Reg, 28, 1>; // Connector ID status change mask
        using DISCINT = Field<OTG_HS_GINTMSK_Reg, 29, 1>; // Disconnect detected interrupt mask
        using SRQIM = Field<OTG_HS_GINTMSK_Reg, 30, 1>; // Session request/new session detected interrupt mask
        using WUIM = Field<OTG_HS_GINTMSK_Reg, 31, 1>; // Resume/remote wakeup detected interrupt mask
        using RSTDE = Field<OTG_HS_GINTMSK_Reg, 23, 1>; // Reset detected interrupt mask
        using LPMINTM = Field<OTG_HS_GINTMSK_Reg, 27, 1>; // LPM interrupt mask
    };

    // OTG_HS Receive status debug read register (host mode)
    using OTG_HS_GRXSTSR_Host_Reg = Register<OTG_HS_GLOBAL_BASE + 0x1C, ReadOnly>;
    struct _OTG_HS_GRXSTSR_Host {
        using CHNUM = Field<OTG_HS_GRXSTSR_Host_Reg, 0, 4>; // Channel number
        using BCNT = Field<OTG_HS_GRXSTSR_Host_Reg, 4, 11>; // Byte count
        using DPID = Field<OTG_HS_GRXSTSR_Host_Reg, 15, 2>; // Data PID
        using PKTSTS = Field<OTG_HS_GRXSTSR_Host_Reg, 17, 4>; // Packet status
    };

    // OTG_HS status read and pop register (host mode)
    using OTG_HS_GRXSTSP_Host_Reg = Register<OTG_HS_GLOBAL_BASE + 0x20, ReadOnly>;
    struct _OTG_HS_GRXSTSP_Host {
        using CHNUM = Field<OTG_HS_GRXSTSP_Host_Reg, 0, 4>; // Channel number
        using BCNT = Field<OTG_HS_GRXSTSP_Host_Reg, 4, 11>; // Byte count
        using DPID = Field<OTG_HS_GRXSTSP_Host_Reg, 15, 2>; // Data PID
        using PKTSTS = Field<OTG_HS_GRXSTSP_Host_Reg, 17, 4>; // Packet status
    };

    // OTG_HS Receive FIFO size register
    using OTG_HS_GRXFSIZ_Reg = Register<OTG_HS_GLOBAL_BASE + 0x24, ReadWrite>;
    struct _OTG_HS_GRXFSIZ {
        using RXFD = Field<OTG_HS_GRXFSIZ_Reg, 0, 16>; // RxFIFO depth
    };

    // OTG_HS nonperiodic transmit FIFO size register (host mode)
    using OTG_HS_HNPTXFSIZ_Host_Reg = Register<OTG_HS_GLOBAL_BASE + 0x28, ReadWrite>;
    struct _OTG_HS_HNPTXFSIZ_Host {
        using NPTXFSA = Field<OTG_HS_HNPTXFSIZ_Host_Reg, 0, 16>; // Nonperiodic transmit RAM start address
        using NPTXFD = Field<OTG_HS_HNPTXFSIZ_Host_Reg, 16, 16>; // Nonperiodic TxFIFO depth
    };

    // Endpoint 0 transmit FIFO size (peripheral mode)
    using OTG_HS_DIEPTXF0_Device_Reg = Register<OTG_HS_GLOBAL_BASE + 0x28, ReadWrite>;
    struct _OTG_HS_DIEPTXF0_Device {
        using TX0FSA = Field<OTG_HS_DIEPTXF0_Device_Reg, 0, 16>; // Endpoint 0 transmit RAM start address
        using TX0FD = Field<OTG_HS_DIEPTXF0_Device_Reg, 16, 16>; // Endpoint 0 TxFIFO depth
    };

    // OTG_HS nonperiodic transmit FIFO/queue status register
    using OTG_HS_GNPTXSTS_Reg = Register<OTG_HS_GLOBAL_BASE + 0x2C, ReadOnly>;
    struct _OTG_HS_GNPTXSTS {
        using NPTXFSAV = Field<OTG_HS_GNPTXSTS_Reg, 0, 16>; // Nonperiodic TxFIFO space available
        using NPTQXSAV = Field<OTG_HS_GNPTXSTS_Reg, 16, 8>; // Nonperiodic transmit request queue space available
        using NPTXQTOP = Field<OTG_HS_GNPTXSTS_Reg, 24, 7>; // Top of the nonperiodic transmit request queue
    };

    // OTG_HS general core configuration register
    using OTG_HS_GCCFG_Reg = Register<OTG_HS_GLOBAL_BASE + 0x38, ReadWrite>;
    struct _OTG_HS_GCCFG {
        using PWRDWN = Field<OTG_HS_GCCFG_Reg, 16, 1>; // Power down
        using BCDEN = Field<OTG_HS_GCCFG_Reg, 17, 1>; // Battery charging detector (BCD) enable
        using DCDEN = Field<OTG_HS_GCCFG_Reg, 18, 1>; // Data contact detection (DCD) mode enable
        using PDEN = Field<OTG_HS_GCCFG_Reg, 19, 1>; // Primary detection (PD) mode enable
        using SDEN = Field<OTG_HS_GCCFG_Reg, 20, 1>; // Secondary detection (SD) mode enable
        using VBDEN = Field<OTG_HS_GCCFG_Reg, 21, 1>; // USB VBUS detection enable
        using DCDET = Field<OTG_HS_GCCFG_Reg, 0, 1>; // Data contact detection (DCD) status
        using PDET = Field<OTG_HS_GCCFG_Reg, 1, 1>; // Primary detection (PD) status
        using SDET = Field<OTG_HS_GCCFG_Reg, 2, 1>; // Secondary detection (SD) status
        using PS2DET = Field<OTG_HS_GCCFG_Reg, 3, 1>; // DM pull-up detection status
    };

    // OTG_HS core ID register
    using OTG_HS_CID_Reg = Register<OTG_HS_GLOBAL_BASE + 0x3C, ReadWrite>;
    struct _OTG_HS_CID {
        using PRODUCT_ID = Field<OTG_HS_CID_Reg, 0, 32>; // Product ID field
    };

    // OTG_HS Host periodic transmit FIFO size register
    using OTG_HS_HPTXFSIZ_Reg = Register<OTG_HS_GLOBAL_BASE + 0x100, ReadWrite>;
    struct _OTG_HS_HPTXFSIZ {
        using PTXSA = Field<OTG_HS_HPTXFSIZ_Reg, 0, 16>; // Host periodic TxFIFO start address
        using PTXFD = Field<OTG_HS_HPTXFSIZ_Reg, 16, 16>; // Host periodic TxFIFO depth
    };

    // OTG_HS device IN endpoint transmit FIFO size register
    using OTG_HS_DIEPTXF1_Reg = Register<OTG_HS_GLOBAL_BASE + 0x104, ReadWrite>;
    struct _OTG_HS_DIEPTXF1 {
        using INEPTXSA = Field<OTG_HS_DIEPTXF1_Reg, 0, 16>; // IN endpoint FIFOx transmit RAM start address
        using INEPTXFD = Field<OTG_HS_DIEPTXF1_Reg, 16, 16>; // IN endpoint TxFIFO depth
    };

    // OTG_HS device IN endpoint transmit FIFO size register
    using OTG_HS_DIEPTXF2_Reg = Register<OTG_HS_GLOBAL_BASE + 0x108, ReadWrite>;
    struct _OTG_HS_DIEPTXF2 {
        using INEPTXSA = Field<OTG_HS_DIEPTXF2_Reg, 0, 16>; // IN endpoint FIFOx transmit RAM start address
        using INEPTXFD = Field<OTG_HS_DIEPTXF2_Reg, 16, 16>; // IN endpoint TxFIFO depth
    };

    // OTG_HS device IN endpoint transmit FIFO size register
    using OTG_HS_DIEPTXF3_Reg = Register<OTG_HS_GLOBAL_BASE + 0x10C, ReadWrite>;
    struct _OTG_HS_DIEPTXF3 {
        using INEPTXSA = Field<OTG_HS_DIEPTXF3_Reg, 0, 16>; // IN endpoint FIFOx transmit RAM start address
        using INEPTXFD = Field<OTG_HS_DIEPTXF3_Reg, 16, 16>; // IN endpoint TxFIFO depth
    };

    // OTG_HS device IN endpoint transmit FIFO size register
    using OTG_HS_DIEPTXF4_Reg = Register<OTG_HS_GLOBAL_BASE + 0x110, ReadWrite>;
    struct _OTG_HS_DIEPTXF4 {
        using INEPTXSA = Field<OTG_HS_DIEPTXF4_Reg, 0, 16>; // IN endpoint FIFOx transmit RAM start address
        using INEPTXFD = Field<OTG_HS_DIEPTXF4_Reg, 16, 16>; // IN endpoint TxFIFO depth
    };

    // OTG_HS device IN endpoint transmit FIFO size register
    using OTG_HS_DIEPTXF5_Reg = Register<OTG_HS_GLOBAL_BASE + 0x114, ReadWrite>;
    struct _OTG_HS_DIEPTXF5 {
        using INEPTXSA = Field<OTG_HS_DIEPTXF5_Reg, 0, 16>; // IN endpoint FIFOx transmit RAM start address
        using INEPTXFD = Field<OTG_HS_DIEPTXF5_Reg, 16, 16>; // IN endpoint TxFIFO depth
    };

    // OTG_HS device IN endpoint transmit FIFO size register
    using OTG_HS_DIEPTXF6_Reg = Register<OTG_HS_GLOBAL_BASE + 0x118, ReadWrite>;
    struct _OTG_HS_DIEPTXF6 {
        using INEPTXSA = Field<OTG_HS_DIEPTXF6_Reg, 0, 16>; // IN endpoint FIFOx transmit RAM start address
        using INEPTXFD = Field<OTG_HS_DIEPTXF6_Reg, 16, 16>; // IN endpoint TxFIFO depth
    };

    // OTG_HS device IN endpoint transmit FIFO size register
    using OTG_HS_DIEPTXF7_Reg = Register<OTG_HS_GLOBAL_BASE + 0x11C, ReadWrite>;
    struct _OTG_HS_DIEPTXF7 {
        using INEPTXSA = Field<OTG_HS_DIEPTXF7_Reg, 0, 16>; // IN endpoint FIFOx transmit RAM start address
        using INEPTXFD = Field<OTG_HS_DIEPTXF7_Reg, 16, 16>; // IN endpoint TxFIFO depth
    };

    using OTG_DIEPTXF8_Reg = Register<OTG_HS_GLOBAL_BASE + 0x120, ReadWrite>;
    struct _OTG_DIEPTXF8 {
        using INEPTXSA = Field<OTG_DIEPTXF8_Reg, 0, 16>; // IN endpoint FIFOx transmit RAM start address This field contains the memory start address for IN endpoint transmit FIFOx. The address must be aligned with a 32-bit memory location.
        using INEPTXFD = Field<OTG_DIEPTXF8_Reg, 16, 16>; // IN endpoint Tx FIFO depth This value is in terms of 32-bit words. Minimum value is 16
    };

    // OTG_HS Receive status debug read register (peripheral mode mode)
    using OTG_HS_GRXSTSR_Device_Reg = Register<OTG_HS_GLOBAL_BASE + 0x1C, ReadOnly>;
    struct _OTG_HS_GRXSTSR_Device {
        using EPNUM = Field<OTG_HS_GRXSTSR_Device_Reg, 0, 4>; // Endpoint number
        using BCNT = Field<OTG_HS_GRXSTSR_Device_Reg, 4, 11>; // Byte count
        using DPID = Field<OTG_HS_GRXSTSR_Device_Reg, 15, 2>; // Data PID
        using PKTSTS = Field<OTG_HS_GRXSTSR_Device_Reg, 17, 4>; // Packet status
        using FRMNUM = Field<OTG_HS_GRXSTSR_Device_Reg, 21, 4>; // Frame number
    };

    // OTG_HS status read and pop register (peripheral mode)
    using OTG_HS_GRXSTSP_Device_Reg = Register<OTG_HS_GLOBAL_BASE + 0x20, ReadOnly>;
    struct _OTG_HS_GRXSTSP_Device {
        using EPNUM = Field<OTG_HS_GRXSTSP_Device_Reg, 0, 4>; // Endpoint number
        using BCNT = Field<OTG_HS_GRXSTSP_Device_Reg, 4, 11>; // Byte count
        using DPID = Field<OTG_HS_GRXSTSP_Device_Reg, 15, 2>; // Data PID
        using PKTSTS = Field<OTG_HS_GRXSTSP_Device_Reg, 17, 4>; // Packet status
        using FRMNUM = Field<OTG_HS_GRXSTSP_Device_Reg, 21, 4>; // Frame number
    };

    // OTG core LPM configuration register
    using OTG_HS_GLPMCFG_Reg = Register<OTG_HS_GLOBAL_BASE + 0x54, ReadWrite>;
    struct _OTG_HS_GLPMCFG {
        using LPMEN = Field<OTG_HS_GLPMCFG_Reg, 0, 1>; // LPM support enable
        using LPMACK = Field<OTG_HS_GLPMCFG_Reg, 1, 1>; // LPM token acknowledge enable
        using BESL = Field<OTG_HS_GLPMCFG_Reg, 2, 4>; // Best effort service latency
        using REMWAKE = Field<OTG_HS_GLPMCFG_Reg, 6, 1>; // bRemoteWake value
        using L1SSEN = Field<OTG_HS_GLPMCFG_Reg, 7, 1>; // L1 Shallow Sleep enable
        using BESLTHRS = Field<OTG_HS_GLPMCFG_Reg, 8, 4>; // BESL threshold
        using L1DSEN = Field<OTG_HS_GLPMCFG_Reg, 12, 1>; // L1 deep sleep enable
        using LPMRST = Field<OTG_HS_GLPMCFG_Reg, 13, 2>; // LPM response
        using SLPSTS = Field<OTG_HS_GLPMCFG_Reg, 15, 1>; // Port sleep status
        using L1RSMOK = Field<OTG_HS_GLPMCFG_Reg, 16, 1>; // Sleep State Resume OK
        using LPMCHIDX = Field<OTG_HS_GLPMCFG_Reg, 17, 4>; // LPM Channel Index
        using LPMRCNT = Field<OTG_HS_GLPMCFG_Reg, 21, 3>; // LPM retry count
        using SNDLPM = Field<OTG_HS_GLPMCFG_Reg, 24, 1>; // Send LPM transaction
        using LPMRCNTSTS = Field<OTG_HS_GLPMCFG_Reg, 25, 3>; // LPM retry count status
        using ENBESL = Field<OTG_HS_GLPMCFG_Reg, 28, 1>; // Enable best effort service latency
    };

} // namespace OTG_HS_GLOBAL

// --------------------------------------------
// MDIOS: Management data input/output slave
// Base address: 0x40017800
// --------------------------------------------

namespace MDIOS {
    static constexpr uint32_t MDIOS_BASE = 0x40017800;

    // MDIOS configuration register
    using MDIOS_CR_Reg = Register<MDIOS_BASE + 0x0, ReadWrite>;
    struct _MDIOS_CR {
        using EN = Field<MDIOS_CR_Reg, 0, 1>; // Peripheral enable
        using WRIE = Field<MDIOS_CR_Reg, 1, 1>; // Register write interrupt enable
        using RDIE = Field<MDIOS_CR_Reg, 2, 1>; // Register Read Interrupt Enable
        using EIE = Field<MDIOS_CR_Reg, 3, 1>; // Error interrupt enable
        using DPC = Field<MDIOS_CR_Reg, 7, 1>; // Disable Preamble Check
        using PORT_ADDRESS = Field<MDIOS_CR_Reg, 8, 5>; // Slaves's address
    };

    // MDIOS write flag register
    using MDIOS_WRFR_Reg = Register<MDIOS_BASE + 0x4, ReadOnly>;
    struct _MDIOS_WRFR {
        using WRF = Field<MDIOS_WRFR_Reg, 0, 32>; // Write flags for MDIO registers 0 to 31
    };

    // MDIOS clear write flag register
    using MDIOS_CWRFR_Reg = Register<MDIOS_BASE + 0x8, ReadWrite>;
    struct _MDIOS_CWRFR {
        using CWRF = Field<MDIOS_CWRFR_Reg, 0, 32>; // Clear the write flag
    };

    // MDIOS read flag register
    using MDIOS_RDFR_Reg = Register<MDIOS_BASE + 0xC, ReadOnly>;
    struct _MDIOS_RDFR {
        using RDF = Field<MDIOS_RDFR_Reg, 0, 32>; // Read flags for MDIO registers 0 to 31
    };

    // MDIOS clear read flag register
    using MDIOS_CRDFR_Reg = Register<MDIOS_BASE + 0x10, ReadWrite>;
    struct _MDIOS_CRDFR {
        using CRDF = Field<MDIOS_CRDFR_Reg, 0, 32>; // Clear the read flag
    };

    // MDIOS status register
    using MDIOS_SR_Reg = Register<MDIOS_BASE + 0x14, ReadOnly>;
    struct _MDIOS_SR {
        using PERF = Field<MDIOS_SR_Reg, 0, 1>; // Preamble error flag
        using SERF = Field<MDIOS_SR_Reg, 1, 1>; // Start error flag
        using TERF = Field<MDIOS_SR_Reg, 2, 1>; // Turnaround error flag
    };

    // MDIOS clear flag register
    using MDIOS_CLRFR_Reg = Register<MDIOS_BASE + 0x18, ReadWrite>;
    struct _MDIOS_CLRFR {
        using CPERF = Field<MDIOS_CLRFR_Reg, 0, 1>; // Clear the preamble error flag
        using CSERF = Field<MDIOS_CLRFR_Reg, 1, 1>; // Clear the start error flag
        using CTERF = Field<MDIOS_CLRFR_Reg, 2, 1>; // Clear the turnaround error flag
    };

    // MDIOS input data register 0
    using MDIOS_DINR0_Reg = Register<MDIOS_BASE + 0x1C, ReadOnly>;
    struct _MDIOS_DINR0 {
        using DIN0 = Field<MDIOS_DINR0_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 1
    using MDIOS_DINR1_Reg = Register<MDIOS_BASE + 0x20, ReadOnly>;
    struct _MDIOS_DINR1 {
        using DIN1 = Field<MDIOS_DINR1_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 2
    using MDIOS_DINR2_Reg = Register<MDIOS_BASE + 0x24, ReadOnly>;
    struct _MDIOS_DINR2 {
        using DIN2 = Field<MDIOS_DINR2_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 3
    using MDIOS_DINR3_Reg = Register<MDIOS_BASE + 0x28, ReadOnly>;
    struct _MDIOS_DINR3 {
        using DIN3 = Field<MDIOS_DINR3_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 4
    using MDIOS_DINR4_Reg = Register<MDIOS_BASE + 0x2C, ReadOnly>;
    struct _MDIOS_DINR4 {
        using DIN4 = Field<MDIOS_DINR4_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 5
    using MDIOS_DINR5_Reg = Register<MDIOS_BASE + 0x30, ReadOnly>;
    struct _MDIOS_DINR5 {
        using DIN5 = Field<MDIOS_DINR5_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 6
    using MDIOS_DINR6_Reg = Register<MDIOS_BASE + 0x34, ReadOnly>;
    struct _MDIOS_DINR6 {
        using DIN6 = Field<MDIOS_DINR6_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 7
    using MDIOS_DINR7_Reg = Register<MDIOS_BASE + 0x38, ReadOnly>;
    struct _MDIOS_DINR7 {
        using DIN7 = Field<MDIOS_DINR7_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 8
    using MDIOS_DINR8_Reg = Register<MDIOS_BASE + 0x3C, ReadOnly>;
    struct _MDIOS_DINR8 {
        using DIN8 = Field<MDIOS_DINR8_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 9
    using MDIOS_DINR9_Reg = Register<MDIOS_BASE + 0x40, ReadOnly>;
    struct _MDIOS_DINR9 {
        using DIN9 = Field<MDIOS_DINR9_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 10
    using MDIOS_DINR10_Reg = Register<MDIOS_BASE + 0x44, ReadOnly>;
    struct _MDIOS_DINR10 {
        using DIN10 = Field<MDIOS_DINR10_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 11
    using MDIOS_DINR11_Reg = Register<MDIOS_BASE + 0x48, ReadOnly>;
    struct _MDIOS_DINR11 {
        using DIN11 = Field<MDIOS_DINR11_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 12
    using MDIOS_DINR12_Reg = Register<MDIOS_BASE + 0x4C, ReadOnly>;
    struct _MDIOS_DINR12 {
        using DIN12 = Field<MDIOS_DINR12_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 13
    using MDIOS_DINR13_Reg = Register<MDIOS_BASE + 0x50, ReadOnly>;
    struct _MDIOS_DINR13 {
        using DIN13 = Field<MDIOS_DINR13_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 14
    using MDIOS_DINR14_Reg = Register<MDIOS_BASE + 0x54, ReadOnly>;
    struct _MDIOS_DINR14 {
        using DIN14 = Field<MDIOS_DINR14_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 15
    using MDIOS_DINR15_Reg = Register<MDIOS_BASE + 0x58, ReadOnly>;
    struct _MDIOS_DINR15 {
        using DIN15 = Field<MDIOS_DINR15_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 16
    using MDIOS_DINR16_Reg = Register<MDIOS_BASE + 0x5C, ReadOnly>;
    struct _MDIOS_DINR16 {
        using DIN16 = Field<MDIOS_DINR16_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 17
    using MDIOS_DINR17_Reg = Register<MDIOS_BASE + 0x60, ReadOnly>;
    struct _MDIOS_DINR17 {
        using DIN17 = Field<MDIOS_DINR17_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 18
    using MDIOS_DINR18_Reg = Register<MDIOS_BASE + 0x64, ReadOnly>;
    struct _MDIOS_DINR18 {
        using DIN18 = Field<MDIOS_DINR18_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 19
    using MDIOS_DINR19_Reg = Register<MDIOS_BASE + 0x68, ReadOnly>;
    struct _MDIOS_DINR19 {
        using DIN19 = Field<MDIOS_DINR19_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 20
    using MDIOS_DINR20_Reg = Register<MDIOS_BASE + 0x6C, ReadOnly>;
    struct _MDIOS_DINR20 {
        using DIN20 = Field<MDIOS_DINR20_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 21
    using MDIOS_DINR21_Reg = Register<MDIOS_BASE + 0x70, ReadOnly>;
    struct _MDIOS_DINR21 {
        using DIN21 = Field<MDIOS_DINR21_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 22
    using MDIOS_DINR22_Reg = Register<MDIOS_BASE + 0x74, ReadOnly>;
    struct _MDIOS_DINR22 {
        using DIN22 = Field<MDIOS_DINR22_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 23
    using MDIOS_DINR23_Reg = Register<MDIOS_BASE + 0x78, ReadOnly>;
    struct _MDIOS_DINR23 {
        using DIN23 = Field<MDIOS_DINR23_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 24
    using MDIOS_DINR24_Reg = Register<MDIOS_BASE + 0x7C, ReadOnly>;
    struct _MDIOS_DINR24 {
        using DIN24 = Field<MDIOS_DINR24_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 25
    using MDIOS_DINR25_Reg = Register<MDIOS_BASE + 0x80, ReadOnly>;
    struct _MDIOS_DINR25 {
        using DIN25 = Field<MDIOS_DINR25_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 26
    using MDIOS_DINR26_Reg = Register<MDIOS_BASE + 0x84, ReadOnly>;
    struct _MDIOS_DINR26 {
        using DIN26 = Field<MDIOS_DINR26_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 27
    using MDIOS_DINR27_Reg = Register<MDIOS_BASE + 0x88, ReadOnly>;
    struct _MDIOS_DINR27 {
        using DIN27 = Field<MDIOS_DINR27_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 28
    using MDIOS_DINR28_Reg = Register<MDIOS_BASE + 0x8C, ReadOnly>;
    struct _MDIOS_DINR28 {
        using DIN28 = Field<MDIOS_DINR28_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 29
    using MDIOS_DINR29_Reg = Register<MDIOS_BASE + 0x90, ReadOnly>;
    struct _MDIOS_DINR29 {
        using DIN29 = Field<MDIOS_DINR29_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 30
    using MDIOS_DINR30_Reg = Register<MDIOS_BASE + 0x94, ReadOnly>;
    struct _MDIOS_DINR30 {
        using DIN30 = Field<MDIOS_DINR30_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS input data register 31
    using MDIOS_DINR31_Reg = Register<MDIOS_BASE + 0x98, ReadOnly>;
    struct _MDIOS_DINR31 {
        using DIN31 = Field<MDIOS_DINR31_Reg, 0, 16>; // Input data received from MDIO Master during write frames
    };

    // MDIOS output data register 0
    using MDIOS_DOUTR0_Reg = Register<MDIOS_BASE + 0x9C, ReadWrite>;
    struct _MDIOS_DOUTR0 {
        using DOUT0 = Field<MDIOS_DOUTR0_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 1
    using MDIOS_DOUTR1_Reg = Register<MDIOS_BASE + 0xA0, ReadWrite>;
    struct _MDIOS_DOUTR1 {
        using DOUT1 = Field<MDIOS_DOUTR1_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 2
    using MDIOS_DOUTR2_Reg = Register<MDIOS_BASE + 0xA4, ReadWrite>;
    struct _MDIOS_DOUTR2 {
        using DOUT2 = Field<MDIOS_DOUTR2_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 3
    using MDIOS_DOUTR3_Reg = Register<MDIOS_BASE + 0xA8, ReadWrite>;
    struct _MDIOS_DOUTR3 {
        using DOUT3 = Field<MDIOS_DOUTR3_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 4
    using MDIOS_DOUTR4_Reg = Register<MDIOS_BASE + 0xAC, ReadWrite>;
    struct _MDIOS_DOUTR4 {
        using DOUT4 = Field<MDIOS_DOUTR4_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 5
    using MDIOS_DOUTR5_Reg = Register<MDIOS_BASE + 0xB0, ReadWrite>;
    struct _MDIOS_DOUTR5 {
        using DOUT5 = Field<MDIOS_DOUTR5_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 6
    using MDIOS_DOUTR6_Reg = Register<MDIOS_BASE + 0xB4, ReadWrite>;
    struct _MDIOS_DOUTR6 {
        using DOUT6 = Field<MDIOS_DOUTR6_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 7
    using MDIOS_DOUTR7_Reg = Register<MDIOS_BASE + 0xB8, ReadWrite>;
    struct _MDIOS_DOUTR7 {
        using DOUT7 = Field<MDIOS_DOUTR7_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 8
    using MDIOS_DOUTR8_Reg = Register<MDIOS_BASE + 0xBC, ReadWrite>;
    struct _MDIOS_DOUTR8 {
        using DOUT8 = Field<MDIOS_DOUTR8_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 9
    using MDIOS_DOUTR9_Reg = Register<MDIOS_BASE + 0xC0, ReadWrite>;
    struct _MDIOS_DOUTR9 {
        using DOUT9 = Field<MDIOS_DOUTR9_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 10
    using MDIOS_DOUTR10_Reg = Register<MDIOS_BASE + 0xC4, ReadWrite>;
    struct _MDIOS_DOUTR10 {
        using DOUT10 = Field<MDIOS_DOUTR10_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 11
    using MDIOS_DOUTR11_Reg = Register<MDIOS_BASE + 0xC8, ReadWrite>;
    struct _MDIOS_DOUTR11 {
        using DOUT11 = Field<MDIOS_DOUTR11_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 12
    using MDIOS_DOUTR12_Reg = Register<MDIOS_BASE + 0xCC, ReadWrite>;
    struct _MDIOS_DOUTR12 {
        using DOUT12 = Field<MDIOS_DOUTR12_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 13
    using MDIOS_DOUTR13_Reg = Register<MDIOS_BASE + 0xD0, ReadWrite>;
    struct _MDIOS_DOUTR13 {
        using DOUT13 = Field<MDIOS_DOUTR13_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 14
    using MDIOS_DOUTR14_Reg = Register<MDIOS_BASE + 0xD4, ReadWrite>;
    struct _MDIOS_DOUTR14 {
        using DOUT14 = Field<MDIOS_DOUTR14_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 15
    using MDIOS_DOUTR15_Reg = Register<MDIOS_BASE + 0xD8, ReadWrite>;
    struct _MDIOS_DOUTR15 {
        using DOUT15 = Field<MDIOS_DOUTR15_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 16
    using MDIOS_DOUTR16_Reg = Register<MDIOS_BASE + 0xDC, ReadWrite>;
    struct _MDIOS_DOUTR16 {
        using DOUT16 = Field<MDIOS_DOUTR16_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 17
    using MDIOS_DOUTR17_Reg = Register<MDIOS_BASE + 0xE0, ReadWrite>;
    struct _MDIOS_DOUTR17 {
        using DOUT17 = Field<MDIOS_DOUTR17_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 18
    using MDIOS_DOUTR18_Reg = Register<MDIOS_BASE + 0xE4, ReadWrite>;
    struct _MDIOS_DOUTR18 {
        using DOUT18 = Field<MDIOS_DOUTR18_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 19
    using MDIOS_DOUTR19_Reg = Register<MDIOS_BASE + 0xE8, ReadWrite>;
    struct _MDIOS_DOUTR19 {
        using DOUT19 = Field<MDIOS_DOUTR19_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 20
    using MDIOS_DOUTR20_Reg = Register<MDIOS_BASE + 0xEC, ReadWrite>;
    struct _MDIOS_DOUTR20 {
        using DOUT20 = Field<MDIOS_DOUTR20_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 21
    using MDIOS_DOUTR21_Reg = Register<MDIOS_BASE + 0xF0, ReadWrite>;
    struct _MDIOS_DOUTR21 {
        using DOUT21 = Field<MDIOS_DOUTR21_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 22
    using MDIOS_DOUTR22_Reg = Register<MDIOS_BASE + 0xF4, ReadWrite>;
    struct _MDIOS_DOUTR22 {
        using DOUT22 = Field<MDIOS_DOUTR22_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 23
    using MDIOS_DOUTR23_Reg = Register<MDIOS_BASE + 0xF8, ReadWrite>;
    struct _MDIOS_DOUTR23 {
        using DOUT23 = Field<MDIOS_DOUTR23_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 24
    using MDIOS_DOUTR24_Reg = Register<MDIOS_BASE + 0xFC, ReadWrite>;
    struct _MDIOS_DOUTR24 {
        using DOUT24 = Field<MDIOS_DOUTR24_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 25
    using MDIOS_DOUTR25_Reg = Register<MDIOS_BASE + 0x100, ReadWrite>;
    struct _MDIOS_DOUTR25 {
        using DOUT25 = Field<MDIOS_DOUTR25_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 26
    using MDIOS_DOUTR26_Reg = Register<MDIOS_BASE + 0x104, ReadWrite>;
    struct _MDIOS_DOUTR26 {
        using DOUT26 = Field<MDIOS_DOUTR26_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 27
    using MDIOS_DOUTR27_Reg = Register<MDIOS_BASE + 0x108, ReadWrite>;
    struct _MDIOS_DOUTR27 {
        using DOUT27 = Field<MDIOS_DOUTR27_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 28
    using MDIOS_DOUTR28_Reg = Register<MDIOS_BASE + 0x10C, ReadWrite>;
    struct _MDIOS_DOUTR28 {
        using DOUT28 = Field<MDIOS_DOUTR28_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 29
    using MDIOS_DOUTR29_Reg = Register<MDIOS_BASE + 0x110, ReadWrite>;
    struct _MDIOS_DOUTR29 {
        using DOUT29 = Field<MDIOS_DOUTR29_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 30
    using MDIOS_DOUTR30_Reg = Register<MDIOS_BASE + 0x114, ReadWrite>;
    struct _MDIOS_DOUTR30 {
        using DOUT30 = Field<MDIOS_DOUTR30_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

    // MDIOS output data register 31
    using MDIOS_DOUTR31_Reg = Register<MDIOS_BASE + 0x118, ReadWrite>;
    struct _MDIOS_DOUTR31 {
        using DOUT31 = Field<MDIOS_DOUTR31_Reg, 0, 16>; // Output data sent to MDIO Master during read frames
    };

} // namespace MDIOS

// --------------------------------------------
// DFSDM: Digital filter for sigma delta modulators
// Base address: 0x40017400
// --------------------------------------------

namespace DFSDM {
    static constexpr uint32_t DFSDM_BASE = 0x40017400;

    // DFSDM channel configuration 0 register 1
    using DFSDM_CHCFG0R1_Reg = Register<DFSDM_BASE + 0x0, ReadWrite>;
    struct _DFSDM_CHCFG0R1 {
        using SITP = Field<DFSDM_CHCFG0R1_Reg, 0, 2>; // Serial interface type for channel 0
        using SPICKSEL = Field<DFSDM_CHCFG0R1_Reg, 2, 2>; // SPI clock select for channel 0
        using SCDEN = Field<DFSDM_CHCFG0R1_Reg, 5, 1>; // Short-circuit detector enable on channel 0
        using CKABEN = Field<DFSDM_CHCFG0R1_Reg, 6, 1>; // Clock absence detector enable on channel 0
        using CHEN = Field<DFSDM_CHCFG0R1_Reg, 7, 1>; // Channel 0 enable
        using CHINSEL = Field<DFSDM_CHCFG0R1_Reg, 8, 1>; // Channel inputs selection
        using DATMPX = Field<DFSDM_CHCFG0R1_Reg, 12, 2>; // Input data multiplexer for channel 0
        using DATPACK = Field<DFSDM_CHCFG0R1_Reg, 14, 2>; // Data packing mode in DFSDM_CHDATINyR register
        using CKOUTDIV = Field<DFSDM_CHCFG0R1_Reg, 16, 8>; // Output serial clock divider
        using CKOUTSRC = Field<DFSDM_CHCFG0R1_Reg, 30, 1>; // Output serial clock source selection
        using DFSDMEN = Field<DFSDM_CHCFG0R1_Reg, 31, 1>; // Global enable for DFSDM interface
    };

    // DFSDM channel configuration 1 register 1
    using DFSDM_CHCFG1R1_Reg = Register<DFSDM_BASE + 0x20, ReadWrite>;
    struct _DFSDM_CHCFG1R1 {
        using SITP = Field<DFSDM_CHCFG1R1_Reg, 0, 2>; // Serial interface type for channel 1
        using SPICKSEL = Field<DFSDM_CHCFG1R1_Reg, 2, 2>; // SPI clock select for channel 1
        using SCDEN = Field<DFSDM_CHCFG1R1_Reg, 5, 1>; // Short-circuit detector enable on channel 1
        using CKABEN = Field<DFSDM_CHCFG1R1_Reg, 6, 1>; // Clock absence detector enable on channel 1
        using CHEN = Field<DFSDM_CHCFG1R1_Reg, 7, 1>; // Channel 1 enable
        using CHINSEL = Field<DFSDM_CHCFG1R1_Reg, 8, 1>; // Channel inputs selection
        using DATMPX = Field<DFSDM_CHCFG1R1_Reg, 12, 2>; // Input data multiplexer for channel 1
        using DATPACK = Field<DFSDM_CHCFG1R1_Reg, 14, 2>; // Data packing mode in DFSDM_CHDATINyR register
        using CKOUTDIV = Field<DFSDM_CHCFG1R1_Reg, 16, 8>; // Output serial clock divider
        using CKOUTSRC = Field<DFSDM_CHCFG1R1_Reg, 30, 1>; // Output serial clock source selection
        using DFSDMEN = Field<DFSDM_CHCFG1R1_Reg, 31, 1>; // Global enable for DFSDM interface
    };

    // DFSDM channel configuration 2 register 1
    using DFSDM_CHCFG2R1_Reg = Register<DFSDM_BASE + 0x40, ReadWrite>;
    struct _DFSDM_CHCFG2R1 {
        using SITP = Field<DFSDM_CHCFG2R1_Reg, 0, 2>; // Serial interface type for channel 2
        using SPICKSEL = Field<DFSDM_CHCFG2R1_Reg, 2, 2>; // SPI clock select for channel 2
        using SCDEN = Field<DFSDM_CHCFG2R1_Reg, 5, 1>; // Short-circuit detector enable on channel 2
        using CKABEN = Field<DFSDM_CHCFG2R1_Reg, 6, 1>; // Clock absence detector enable on channel 2
        using CHEN = Field<DFSDM_CHCFG2R1_Reg, 7, 1>; // Channel 2 enable
        using CHINSEL = Field<DFSDM_CHCFG2R1_Reg, 8, 1>; // Channel inputs selection
        using DATMPX = Field<DFSDM_CHCFG2R1_Reg, 12, 2>; // Input data multiplexer for channel 2
        using DATPACK = Field<DFSDM_CHCFG2R1_Reg, 14, 2>; // Data packing mode in DFSDM_CHDATINyR register
        using CKOUTDIV = Field<DFSDM_CHCFG2R1_Reg, 16, 8>; // Output serial clock divider
        using CKOUTSRC = Field<DFSDM_CHCFG2R1_Reg, 30, 1>; // Output serial clock source selection
        using DFSDMEN = Field<DFSDM_CHCFG2R1_Reg, 31, 1>; // Global enable for DFSDM interface
    };

    // DFSDM channel configuration 3 register 1
    using DFSDM_CHCFG3R1_Reg = Register<DFSDM_BASE + 0x60, ReadWrite>;
    struct _DFSDM_CHCFG3R1 {
        using SITP = Field<DFSDM_CHCFG3R1_Reg, 0, 2>; // Serial interface type for channel 3
        using SPICKSEL = Field<DFSDM_CHCFG3R1_Reg, 2, 2>; // SPI clock select for channel 3
        using SCDEN = Field<DFSDM_CHCFG3R1_Reg, 5, 1>; // Short-circuit detector enable on channel 3
        using CKABEN = Field<DFSDM_CHCFG3R1_Reg, 6, 1>; // Clock absence detector enable on channel 3
        using CHEN = Field<DFSDM_CHCFG3R1_Reg, 7, 1>; // Channel 3 enable
        using CHINSEL = Field<DFSDM_CHCFG3R1_Reg, 8, 1>; // Channel inputs selection
        using DATMPX = Field<DFSDM_CHCFG3R1_Reg, 12, 2>; // Input data multiplexer for channel 3
        using DATPACK = Field<DFSDM_CHCFG3R1_Reg, 14, 2>; // Data packing mode in DFSDM_CHDATINyR register
        using CKOUTDIV = Field<DFSDM_CHCFG3R1_Reg, 16, 8>; // Output serial clock divider
        using CKOUTSRC = Field<DFSDM_CHCFG3R1_Reg, 30, 1>; // Output serial clock source selection
        using DFSDMEN = Field<DFSDM_CHCFG3R1_Reg, 31, 1>; // Global enable for DFSDM interface
    };

    // DFSDM channel configuration 4 register 1
    using DFSDM_CHCFG4R1_Reg = Register<DFSDM_BASE + 0x80, ReadWrite>;
    struct _DFSDM_CHCFG4R1 {
        using SITP = Field<DFSDM_CHCFG4R1_Reg, 0, 2>; // Serial interface type for channel 4
        using SPICKSEL = Field<DFSDM_CHCFG4R1_Reg, 2, 2>; // SPI clock select for channel 4
        using SCDEN = Field<DFSDM_CHCFG4R1_Reg, 5, 1>; // Short-circuit detector enable on channel 4
        using CKABEN = Field<DFSDM_CHCFG4R1_Reg, 6, 1>; // Clock absence detector enable on channel 4
        using CHEN = Field<DFSDM_CHCFG4R1_Reg, 7, 1>; // Channel 4 enable
        using CHINSEL = Field<DFSDM_CHCFG4R1_Reg, 8, 1>; // Channel inputs selection
        using DATMPX = Field<DFSDM_CHCFG4R1_Reg, 12, 2>; // Input data multiplexer for channel 4
        using DATPACK = Field<DFSDM_CHCFG4R1_Reg, 14, 2>; // Data packing mode in DFSDM_CHDATINyR register
        using CKOUTDIV = Field<DFSDM_CHCFG4R1_Reg, 16, 8>; // Output serial clock divider
        using CKOUTSRC = Field<DFSDM_CHCFG4R1_Reg, 30, 1>; // Output serial clock source selection
        using DFSDMEN = Field<DFSDM_CHCFG4R1_Reg, 31, 1>; // Global enable for DFSDM interface
    };

    // DFSDM channel configuration 5 register 1
    using DFSDM_CHCFG5R1_Reg = Register<DFSDM_BASE + 0xA0, ReadWrite>;
    struct _DFSDM_CHCFG5R1 {
        using SITP = Field<DFSDM_CHCFG5R1_Reg, 0, 2>; // Serial interface type for channel 5
        using SPICKSEL = Field<DFSDM_CHCFG5R1_Reg, 2, 2>; // SPI clock select for channel 5
        using SCDEN = Field<DFSDM_CHCFG5R1_Reg, 5, 1>; // Short-circuit detector enable on channel 5
        using CKABEN = Field<DFSDM_CHCFG5R1_Reg, 6, 1>; // Clock absence detector enable on channel 5
        using CHEN = Field<DFSDM_CHCFG5R1_Reg, 7, 1>; // Channel 5 enable
        using CHINSEL = Field<DFSDM_CHCFG5R1_Reg, 8, 1>; // Channel inputs selection
        using DATMPX = Field<DFSDM_CHCFG5R1_Reg, 12, 2>; // Input data multiplexer for channel 5
        using DATPACK = Field<DFSDM_CHCFG5R1_Reg, 14, 2>; // Data packing mode in DFSDM_CHDATINyR register
        using CKOUTDIV = Field<DFSDM_CHCFG5R1_Reg, 16, 8>; // Output serial clock divider
        using CKOUTSRC = Field<DFSDM_CHCFG5R1_Reg, 30, 1>; // Output serial clock source selection
        using DFSDMEN = Field<DFSDM_CHCFG5R1_Reg, 31, 1>; // Global enable for DFSDM interface
    };

    // DFSDM channel configuration 6 register 1
    using DFSDM_CHCFG6R1_Reg = Register<DFSDM_BASE + 0xC0, ReadWrite>;
    struct _DFSDM_CHCFG6R1 {
        using SITP = Field<DFSDM_CHCFG6R1_Reg, 0, 2>; // Serial interface type for channel 6
        using SPICKSEL = Field<DFSDM_CHCFG6R1_Reg, 2, 2>; // SPI clock select for channel 6
        using SCDEN = Field<DFSDM_CHCFG6R1_Reg, 5, 1>; // Short-circuit detector enable on channel 6
        using CKABEN = Field<DFSDM_CHCFG6R1_Reg, 6, 1>; // Clock absence detector enable on channel 6
        using CHEN = Field<DFSDM_CHCFG6R1_Reg, 7, 1>; // Channel 6 enable
        using CHINSEL = Field<DFSDM_CHCFG6R1_Reg, 8, 1>; // Channel inputs selection
        using DATMPX = Field<DFSDM_CHCFG6R1_Reg, 12, 2>; // Input data multiplexer for channel 6
        using DATPACK = Field<DFSDM_CHCFG6R1_Reg, 14, 2>; // Data packing mode in DFSDM_CHDATINyR register
        using CKOUTDIV = Field<DFSDM_CHCFG6R1_Reg, 16, 8>; // Output serial clock divider
        using CKOUTSRC = Field<DFSDM_CHCFG6R1_Reg, 30, 1>; // Output serial clock source selection
        using DFSDMEN = Field<DFSDM_CHCFG6R1_Reg, 31, 1>; // Global enable for DFSDM interface
    };

    // DFSDM channel configuration 7 register 1
    using DFSDM_CHCFG7R1_Reg = Register<DFSDM_BASE + 0xE0, ReadWrite>;
    struct _DFSDM_CHCFG7R1 {
        using SITP = Field<DFSDM_CHCFG7R1_Reg, 0, 2>; // Serial interface type for channel 7
        using SPICKSEL = Field<DFSDM_CHCFG7R1_Reg, 2, 2>; // SPI clock select for channel 7
        using SCDEN = Field<DFSDM_CHCFG7R1_Reg, 5, 1>; // Short-circuit detector enable on channel 7
        using CKABEN = Field<DFSDM_CHCFG7R1_Reg, 6, 1>; // Clock absence detector enable on channel 7
        using CHEN = Field<DFSDM_CHCFG7R1_Reg, 7, 1>; // Channel 7 enable
        using CHINSEL = Field<DFSDM_CHCFG7R1_Reg, 8, 1>; // Channel inputs selection
        using DATMPX = Field<DFSDM_CHCFG7R1_Reg, 12, 2>; // Input data multiplexer for channel 7
        using DATPACK = Field<DFSDM_CHCFG7R1_Reg, 14, 2>; // Data packing mode in DFSDM_CHDATINyR register
        using CKOUTDIV = Field<DFSDM_CHCFG7R1_Reg, 16, 8>; // Output serial clock divider
        using CKOUTSRC = Field<DFSDM_CHCFG7R1_Reg, 30, 1>; // Output serial clock source selection
        using DFSDMEN = Field<DFSDM_CHCFG7R1_Reg, 31, 1>; // Global enable for DFSDM interface
    };

    // DFSDM channel configuration 0 register 2
    using DFSDM_CHCFG0R2_Reg = Register<DFSDM_BASE + 0x4, ReadWrite>;
    struct _DFSDM_CHCFG0R2 {
        using DTRBS = Field<DFSDM_CHCFG0R2_Reg, 3, 5>; // Data right bit-shift for channel 0
        using OFFSET = Field<DFSDM_CHCFG0R2_Reg, 8, 24>; // 24-bit calibration offset for channel 0
    };

    // DFSDM channel configuration 1 register 2
    using DFSDM_CHCFG1R2_Reg = Register<DFSDM_BASE + 0x24, ReadWrite>;
    struct _DFSDM_CHCFG1R2 {
        using DTRBS = Field<DFSDM_CHCFG1R2_Reg, 3, 5>; // Data right bit-shift for channel 1
        using OFFSET = Field<DFSDM_CHCFG1R2_Reg, 8, 24>; // 24-bit calibration offset for channel 1
    };

    // DFSDM channel configuration 2 register 2
    using DFSDM_CHCFG2R2_Reg = Register<DFSDM_BASE + 0x44, ReadWrite>;
    struct _DFSDM_CHCFG2R2 {
        using DTRBS = Field<DFSDM_CHCFG2R2_Reg, 3, 5>; // Data right bit-shift for channel 2
        using OFFSET = Field<DFSDM_CHCFG2R2_Reg, 8, 24>; // 24-bit calibration offset for channel 2
    };

    // DFSDM channel configuration 3 register 2
    using DFSDM_CHCFG3R2_Reg = Register<DFSDM_BASE + 0x64, ReadWrite>;
    struct _DFSDM_CHCFG3R2 {
        using DTRBS = Field<DFSDM_CHCFG3R2_Reg, 3, 5>; // Data right bit-shift for channel 3
        using OFFSET = Field<DFSDM_CHCFG3R2_Reg, 8, 24>; // 24-bit calibration offset for channel 3
    };

    // DFSDM channel configuration 4 register 2
    using DFSDM_CHCFG4R2_Reg = Register<DFSDM_BASE + 0x84, ReadWrite>;
    struct _DFSDM_CHCFG4R2 {
        using DTRBS = Field<DFSDM_CHCFG4R2_Reg, 3, 5>; // Data right bit-shift for channel 4
        using OFFSET = Field<DFSDM_CHCFG4R2_Reg, 8, 24>; // 24-bit calibration offset for channel 4
    };

    // DFSDM channel configuration 5 register 2
    using DFSDM_CHCFG5R2_Reg = Register<DFSDM_BASE + 0xA4, ReadWrite>;
    struct _DFSDM_CHCFG5R2 {
        using DTRBS = Field<DFSDM_CHCFG5R2_Reg, 3, 5>; // Data right bit-shift for channel 5
        using OFFSET = Field<DFSDM_CHCFG5R2_Reg, 8, 24>; // 24-bit calibration offset for channel 5
    };

    // DFSDM channel configuration 6 register 2
    using DFSDM_CHCFG6R2_Reg = Register<DFSDM_BASE + 0xC4, ReadWrite>;
    struct _DFSDM_CHCFG6R2 {
        using DTRBS = Field<DFSDM_CHCFG6R2_Reg, 3, 5>; // Data right bit-shift for channel 6
        using OFFSET = Field<DFSDM_CHCFG6R2_Reg, 8, 24>; // 24-bit calibration offset for channel 6
    };

    // DFSDM channel configuration 7 register 2
    using DFSDM_CHCFG7R2_Reg = Register<DFSDM_BASE + 0xE4, ReadWrite>;
    struct _DFSDM_CHCFG7R2 {
        using DTRBS = Field<DFSDM_CHCFG7R2_Reg, 3, 5>; // Data right bit-shift for channel 7
        using OFFSET = Field<DFSDM_CHCFG7R2_Reg, 8, 24>; // 24-bit calibration offset for channel 7
    };

    // DFSDM analog watchdog and short-circuit detector register
    using DFSDM_AWSCD0R_Reg = Register<DFSDM_BASE + 0x8, ReadWrite>;
    struct _DFSDM_AWSCD0R {
        using SCDT = Field<DFSDM_AWSCD0R_Reg, 0, 8>; // short-circuit detector threshold for channel 0
        using BKSCD = Field<DFSDM_AWSCD0R_Reg, 12, 4>; // Break signal assignment for short-circuit detector on channel 0
        using AWFOSR = Field<DFSDM_AWSCD0R_Reg, 16, 5>; // Analog watchdog filter oversampling ratio (decimation rate) on channel 0
        using AWFORD = Field<DFSDM_AWSCD0R_Reg, 22, 2>; // Analog watchdog Sinc filter order on channel 0
    };

    // DFSDM analog watchdog and short-circuit detector register
    using DFSDM_AWSCD1R_Reg = Register<DFSDM_BASE + 0x28, ReadWrite>;
    struct _DFSDM_AWSCD1R {
        using SCDT = Field<DFSDM_AWSCD1R_Reg, 0, 8>; // short-circuit detector threshold for channel 1
        using BKSCD = Field<DFSDM_AWSCD1R_Reg, 12, 4>; // Break signal assignment for short-circuit detector on channel 1
        using AWFOSR = Field<DFSDM_AWSCD1R_Reg, 16, 5>; // Analog watchdog filter oversampling ratio (decimation rate) on channel 1
        using AWFORD = Field<DFSDM_AWSCD1R_Reg, 22, 2>; // Analog watchdog Sinc filter order on channel 1
    };

    // DFSDM analog watchdog and short-circuit detector register
    using DFSDM_AWSCD2R_Reg = Register<DFSDM_BASE + 0x48, ReadWrite>;
    struct _DFSDM_AWSCD2R {
        using SCDT = Field<DFSDM_AWSCD2R_Reg, 0, 8>; // short-circuit detector threshold for channel 2
        using BKSCD = Field<DFSDM_AWSCD2R_Reg, 12, 4>; // Break signal assignment for short-circuit detector on channel 2
        using AWFOSR = Field<DFSDM_AWSCD2R_Reg, 16, 5>; // Analog watchdog filter oversampling ratio (decimation rate) on channel 2
        using AWFORD = Field<DFSDM_AWSCD2R_Reg, 22, 2>; // Analog watchdog Sinc filter order on channel 2
    };

    // DFSDM analog watchdog and short-circuit detector register
    using DFSDM_AWSCD3R_Reg = Register<DFSDM_BASE + 0x68, ReadWrite>;
    struct _DFSDM_AWSCD3R {
        using SCDT = Field<DFSDM_AWSCD3R_Reg, 0, 8>; // short-circuit detector threshold for channel 3
        using BKSCD = Field<DFSDM_AWSCD3R_Reg, 12, 4>; // Break signal assignment for short-circuit detector on channel 3
        using AWFOSR = Field<DFSDM_AWSCD3R_Reg, 16, 5>; // Analog watchdog filter oversampling ratio (decimation rate) on channel 3
        using AWFORD = Field<DFSDM_AWSCD3R_Reg, 22, 2>; // Analog watchdog Sinc filter order on channel 3
    };

    // DFSDM analog watchdog and short-circuit detector register
    using DFSDM_AWSCD4R_Reg = Register<DFSDM_BASE + 0x88, ReadWrite>;
    struct _DFSDM_AWSCD4R {
        using SCDT = Field<DFSDM_AWSCD4R_Reg, 0, 8>; // short-circuit detector threshold for channel 4
        using BKSCD = Field<DFSDM_AWSCD4R_Reg, 12, 4>; // Break signal assignment for short-circuit detector on channel 4
        using AWFOSR = Field<DFSDM_AWSCD4R_Reg, 16, 5>; // Analog watchdog filter oversampling ratio (decimation rate) on channel 4
        using AWFORD = Field<DFSDM_AWSCD4R_Reg, 22, 2>; // Analog watchdog Sinc filter order on channel 4
    };

    // DFSDM analog watchdog and short-circuit detector register
    using DFSDM_AWSCD5R_Reg = Register<DFSDM_BASE + 0xA8, ReadWrite>;
    struct _DFSDM_AWSCD5R {
        using SCDT = Field<DFSDM_AWSCD5R_Reg, 0, 8>; // short-circuit detector threshold for channel 5
        using BKSCD = Field<DFSDM_AWSCD5R_Reg, 12, 4>; // Break signal assignment for short-circuit detector on channel 5
        using AWFOSR = Field<DFSDM_AWSCD5R_Reg, 16, 5>; // Analog watchdog filter oversampling ratio (decimation rate) on channel 5
        using AWFORD = Field<DFSDM_AWSCD5R_Reg, 22, 2>; // Analog watchdog Sinc filter order on channel 5
    };

    // DFSDM analog watchdog and short-circuit detector register
    using DFSDM_AWSCD6R_Reg = Register<DFSDM_BASE + 0xC8, ReadWrite>;
    struct _DFSDM_AWSCD6R {
        using SCDT = Field<DFSDM_AWSCD6R_Reg, 0, 8>; // short-circuit detector threshold for channel 6
        using BKSCD = Field<DFSDM_AWSCD6R_Reg, 12, 4>; // Break signal assignment for short-circuit detector on channel 6
        using AWFOSR = Field<DFSDM_AWSCD6R_Reg, 16, 5>; // Analog watchdog filter oversampling ratio (decimation rate) on channel 6
        using AWFORD = Field<DFSDM_AWSCD6R_Reg, 22, 2>; // Analog watchdog Sinc filter order on channel 6
    };

    // DFSDM analog watchdog and short-circuit detector register
    using DFSDM_AWSCD7R_Reg = Register<DFSDM_BASE + 0xE8, ReadWrite>;
    struct _DFSDM_AWSCD7R {
        using SCDT = Field<DFSDM_AWSCD7R_Reg, 0, 8>; // short-circuit detector threshold for channel 7
        using BKSCD = Field<DFSDM_AWSCD7R_Reg, 12, 4>; // Break signal assignment for short-circuit detector on channel 7
        using AWFOSR = Field<DFSDM_AWSCD7R_Reg, 16, 5>; // Analog watchdog filter oversampling ratio (decimation rate) on channel 7
        using AWFORD = Field<DFSDM_AWSCD7R_Reg, 22, 2>; // Analog watchdog Sinc filter order on channel 7
    };

    // DFSDM channel watchdog filter data register
    using DFSDM_CHWDAT0R_Reg = Register<DFSDM_BASE + 0xC, ReadOnly>;
    struct _DFSDM_CHWDAT0R {
        using WDATA = Field<DFSDM_CHWDAT0R_Reg, 0, 16>; // Input channel y watchdog data
    };

    // DFSDM channel watchdog filter data register
    using DFSDM_CHWDAT1R_Reg = Register<DFSDM_BASE + 0x2C, ReadOnly>;
    struct _DFSDM_CHWDAT1R {
        using WDATA = Field<DFSDM_CHWDAT1R_Reg, 0, 16>; // Input channel y watchdog data
    };

    // DFSDM channel watchdog filter data register
    using DFSDM_CHWDAT2R_Reg = Register<DFSDM_BASE + 0x4C, ReadOnly>;
    struct _DFSDM_CHWDAT2R {
        using WDATA = Field<DFSDM_CHWDAT2R_Reg, 0, 16>; // Input channel y watchdog data
    };

    // DFSDM channel watchdog filter data register
    using DFSDM_CHWDAT3R_Reg = Register<DFSDM_BASE + 0x6C, ReadOnly>;
    struct _DFSDM_CHWDAT3R {
        using WDATA = Field<DFSDM_CHWDAT3R_Reg, 0, 16>; // Input channel y watchdog data
    };

    // DFSDM channel watchdog filter data register
    using DFSDM_CHWDAT4R_Reg = Register<DFSDM_BASE + 0x8C, ReadOnly>;
    struct _DFSDM_CHWDAT4R {
        using WDATA = Field<DFSDM_CHWDAT4R_Reg, 0, 16>; // Input channel y watchdog data
    };

    // DFSDM channel watchdog filter data register
    using DFSDM_CHWDAT5R_Reg = Register<DFSDM_BASE + 0xAC, ReadOnly>;
    struct _DFSDM_CHWDAT5R {
        using WDATA = Field<DFSDM_CHWDAT5R_Reg, 0, 16>; // Input channel y watchdog data
    };

    // DFSDM channel watchdog filter data register
    using DFSDM_CHWDAT6R_Reg = Register<DFSDM_BASE + 0xCC, ReadOnly>;
    struct _DFSDM_CHWDAT6R {
        using WDATA = Field<DFSDM_CHWDAT6R_Reg, 0, 16>; // Input channel y watchdog data
    };

    // DFSDM channel watchdog filter data register
    using DFSDM_CHWDAT7R_Reg = Register<DFSDM_BASE + 0xEC, ReadOnly>;
    struct _DFSDM_CHWDAT7R {
        using WDATA = Field<DFSDM_CHWDAT7R_Reg, 0, 16>; // Input channel y watchdog data
    };

    // DFSDM channel data input register
    using DFSDM_CHDATIN0R_Reg = Register<DFSDM_BASE + 0x10, ReadWrite>;
    struct _DFSDM_CHDATIN0R {
        using INDAT0 = Field<DFSDM_CHDATIN0R_Reg, 0, 16>; // Input data for channel 0
        using INDAT1 = Field<DFSDM_CHDATIN0R_Reg, 16, 16>; // Input data for channel 1
    };

    // DFSDM channel data input register
    using DFSDM_CHDATIN1R_Reg = Register<DFSDM_BASE + 0x30, ReadWrite>;
    struct _DFSDM_CHDATIN1R {
        using INDAT0 = Field<DFSDM_CHDATIN1R_Reg, 0, 16>; // Input data for channel 1
        using INDAT1 = Field<DFSDM_CHDATIN1R_Reg, 16, 16>; // Input data for channel 2
    };

    // DFSDM channel data input register
    using DFSDM_CHDATIN2R_Reg = Register<DFSDM_BASE + 0x50, ReadWrite>;
    struct _DFSDM_CHDATIN2R {
        using INDAT0 = Field<DFSDM_CHDATIN2R_Reg, 0, 16>; // Input data for channel 2
        using INDAT1 = Field<DFSDM_CHDATIN2R_Reg, 16, 16>; // Input data for channel 3
    };

    // DFSDM channel data input register
    using DFSDM_CHDATIN3R_Reg = Register<DFSDM_BASE + 0x70, ReadWrite>;
    struct _DFSDM_CHDATIN3R {
        using INDAT0 = Field<DFSDM_CHDATIN3R_Reg, 0, 16>; // Input data for channel 3
        using INDAT1 = Field<DFSDM_CHDATIN3R_Reg, 16, 16>; // Input data for channel 4
    };

    // DFSDM channel data input register
    using DFSDM_CHDATIN4R_Reg = Register<DFSDM_BASE + 0x90, ReadWrite>;
    struct _DFSDM_CHDATIN4R {
        using INDAT0 = Field<DFSDM_CHDATIN4R_Reg, 0, 16>; // Input data for channel 4
        using INDAT1 = Field<DFSDM_CHDATIN4R_Reg, 16, 16>; // Input data for channel 5
    };

    // DFSDM channel data input register
    using DFSDM_CHDATIN5R_Reg = Register<DFSDM_BASE + 0xB0, ReadWrite>;
    struct _DFSDM_CHDATIN5R {
        using INDAT0 = Field<DFSDM_CHDATIN5R_Reg, 0, 16>; // Input data for channel 5
        using INDAT1 = Field<DFSDM_CHDATIN5R_Reg, 16, 16>; // Input data for channel 6
    };

    // DFSDM channel data input register
    using DFSDM_CHDATIN6R_Reg = Register<DFSDM_BASE + 0xD0, ReadWrite>;
    struct _DFSDM_CHDATIN6R {
        using INDAT0 = Field<DFSDM_CHDATIN6R_Reg, 0, 16>; // Input data for channel 6
        using INDAT1 = Field<DFSDM_CHDATIN6R_Reg, 16, 16>; // Input data for channel 7
    };

    // DFSDM channel data input register
    using DFSDM_CHDATIN7R_Reg = Register<DFSDM_BASE + 0xF0, ReadWrite>;
    struct _DFSDM_CHDATIN7R {
        using INDAT0 = Field<DFSDM_CHDATIN7R_Reg, 0, 16>; // Input data for channel 7
        using INDAT1 = Field<DFSDM_CHDATIN7R_Reg, 16, 16>; // Input data for channel 8
    };

    // DFSDM control register 1
    using DFSDM0_CR1_Reg = Register<DFSDM_BASE + 0x100, ReadWrite>;
    struct _DFSDM0_CR1 {
        using DFEN = Field<DFSDM0_CR1_Reg, 0, 1>; // DFSDM enable
        using JSWSTART = Field<DFSDM0_CR1_Reg, 1, 1>; // Start a conversion of the injected group of channels
        using JSYNC = Field<DFSDM0_CR1_Reg, 3, 1>; // Launch an injected conversion synchronously with the DFSDM0 JSWSTART trigger
        using JSCAN = Field<DFSDM0_CR1_Reg, 4, 1>; // Scanning conversion mode for injected conversions
        using JDMAEN = Field<DFSDM0_CR1_Reg, 5, 1>; // DMA channel enabled to read data for the injected channel group
        using JEXTSEL = Field<DFSDM0_CR1_Reg, 8, 5>; // Trigger signal selection for launching injected conversions
        using JEXTEN = Field<DFSDM0_CR1_Reg, 13, 2>; // Trigger enable and trigger edge selection for injected conversions
        using RSWSTART = Field<DFSDM0_CR1_Reg, 17, 1>; // Software start of a conversion on the regular channel
        using RCONT = Field<DFSDM0_CR1_Reg, 18, 1>; // Continuous mode selection for regular conversions
        using RSYNC = Field<DFSDM0_CR1_Reg, 19, 1>; // Launch regular conversion synchronously with DFSDM0
        using RDMAEN = Field<DFSDM0_CR1_Reg, 21, 1>; // DMA channel enabled to read data for the regular conversion
        using RCH = Field<DFSDM0_CR1_Reg, 24, 3>; // Regular channel selection
        using FAST = Field<DFSDM0_CR1_Reg, 29, 1>; // Fast conversion mode selection for regular conversions
        using AWFSEL = Field<DFSDM0_CR1_Reg, 30, 1>; // Analog watchdog fast mode select
    };

    // DFSDM control register 1
    using DFSDM1_CR1_Reg = Register<DFSDM_BASE + 0x180, ReadWrite>;
    struct _DFSDM1_CR1 {
        using DFEN = Field<DFSDM1_CR1_Reg, 0, 1>; // DFSDM enable
        using JSWSTART = Field<DFSDM1_CR1_Reg, 1, 1>; // Start a conversion of the injected group of channels
        using JSYNC = Field<DFSDM1_CR1_Reg, 3, 1>; // Launch an injected conversion synchronously with the DFSDM0 JSWSTART trigger
        using JSCAN = Field<DFSDM1_CR1_Reg, 4, 1>; // Scanning conversion mode for injected conversions
        using JDMAEN = Field<DFSDM1_CR1_Reg, 5, 1>; // DMA channel enabled to read data for the injected channel group
        using JEXTSEL = Field<DFSDM1_CR1_Reg, 8, 5>; // Trigger signal selection for launching injected conversions
        using JEXTEN = Field<DFSDM1_CR1_Reg, 13, 2>; // Trigger enable and trigger edge selection for injected conversions
        using RSWSTART = Field<DFSDM1_CR1_Reg, 17, 1>; // Software start of a conversion on the regular channel
        using RCONT = Field<DFSDM1_CR1_Reg, 18, 1>; // Continuous mode selection for regular conversions
        using RSYNC = Field<DFSDM1_CR1_Reg, 19, 1>; // Launch regular conversion synchronously with DFSDM0
        using RDMAEN = Field<DFSDM1_CR1_Reg, 21, 1>; // DMA channel enabled to read data for the regular conversion
        using RCH = Field<DFSDM1_CR1_Reg, 24, 3>; // Regular channel selection
        using FAST = Field<DFSDM1_CR1_Reg, 29, 1>; // Fast conversion mode selection for regular conversions
        using AWFSEL = Field<DFSDM1_CR1_Reg, 30, 1>; // Analog watchdog fast mode select
    };

    // DFSDM control register 1
    using DFSDM2_CR1_Reg = Register<DFSDM_BASE + 0x200, ReadWrite>;
    struct _DFSDM2_CR1 {
        using DFEN = Field<DFSDM2_CR1_Reg, 0, 1>; // DFSDM enable
        using JSWSTART = Field<DFSDM2_CR1_Reg, 1, 1>; // Start a conversion of the injected group of channels
        using JSYNC = Field<DFSDM2_CR1_Reg, 3, 1>; // Launch an injected conversion synchronously with the DFSDM0 JSWSTART trigger
        using JSCAN = Field<DFSDM2_CR1_Reg, 4, 1>; // Scanning conversion mode for injected conversions
        using JDMAEN = Field<DFSDM2_CR1_Reg, 5, 1>; // DMA channel enabled to read data for the injected channel group
        using JEXTSEL = Field<DFSDM2_CR1_Reg, 8, 5>; // Trigger signal selection for launching injected conversions
        using JEXTEN = Field<DFSDM2_CR1_Reg, 13, 2>; // Trigger enable and trigger edge selection for injected conversions
        using RSWSTART = Field<DFSDM2_CR1_Reg, 17, 1>; // Software start of a conversion on the regular channel
        using RCONT = Field<DFSDM2_CR1_Reg, 18, 1>; // Continuous mode selection for regular conversions
        using RSYNC = Field<DFSDM2_CR1_Reg, 19, 1>; // Launch regular conversion synchronously with DFSDM0
        using RDMAEN = Field<DFSDM2_CR1_Reg, 21, 1>; // DMA channel enabled to read data for the regular conversion
        using RCH = Field<DFSDM2_CR1_Reg, 24, 3>; // Regular channel selection
        using FAST = Field<DFSDM2_CR1_Reg, 29, 1>; // Fast conversion mode selection for regular conversions
        using AWFSEL = Field<DFSDM2_CR1_Reg, 30, 1>; // Analog watchdog fast mode select
    };

    // DFSDM control register 1
    using DFSDM3_CR1_Reg = Register<DFSDM_BASE + 0x380, ReadWrite>;
    struct _DFSDM3_CR1 {
        using DFEN = Field<DFSDM3_CR1_Reg, 0, 1>; // DFSDM enable
        using JSWSTART = Field<DFSDM3_CR1_Reg, 1, 1>; // Start a conversion of the injected group of channels
        using JSYNC = Field<DFSDM3_CR1_Reg, 3, 1>; // Launch an injected conversion synchronously with the DFSDM0 JSWSTART trigger
        using JSCAN = Field<DFSDM3_CR1_Reg, 4, 1>; // Scanning conversion mode for injected conversions
        using JDMAEN = Field<DFSDM3_CR1_Reg, 5, 1>; // DMA channel enabled to read data for the injected channel group
        using JEXTSEL = Field<DFSDM3_CR1_Reg, 8, 5>; // Trigger signal selection for launching injected conversions
        using JEXTEN = Field<DFSDM3_CR1_Reg, 13, 2>; // Trigger enable and trigger edge selection for injected conversions
        using RSWSTART = Field<DFSDM3_CR1_Reg, 17, 1>; // Software start of a conversion on the regular channel
        using RCONT = Field<DFSDM3_CR1_Reg, 18, 1>; // Continuous mode selection for regular conversions
        using RSYNC = Field<DFSDM3_CR1_Reg, 19, 1>; // Launch regular conversion synchronously with DFSDM0
        using RDMAEN = Field<DFSDM3_CR1_Reg, 21, 1>; // DMA channel enabled to read data for the regular conversion
        using RCH = Field<DFSDM3_CR1_Reg, 24, 3>; // Regular channel selection
        using FAST = Field<DFSDM3_CR1_Reg, 29, 1>; // Fast conversion mode selection for regular conversions
        using AWFSEL = Field<DFSDM3_CR1_Reg, 30, 1>; // Analog watchdog fast mode select
    };

    // DFSDM control register 2
    using DFSDM0_CR2_Reg = Register<DFSDM_BASE + 0x104, ReadWrite>;
    struct _DFSDM0_CR2 {
        using JEOCIE = Field<DFSDM0_CR2_Reg, 0, 1>; // Injected end of conversion interrupt enable
        using REOCIE = Field<DFSDM0_CR2_Reg, 1, 1>; // Regular end of conversion interrupt enable
        using JOVRIE = Field<DFSDM0_CR2_Reg, 2, 1>; // Injected data overrun interrupt enable
        using ROVRIE = Field<DFSDM0_CR2_Reg, 3, 1>; // Regular data overrun interrupt enable
        using AWDIE = Field<DFSDM0_CR2_Reg, 4, 1>; // Analog watchdog interrupt enable
        using SCDIE = Field<DFSDM0_CR2_Reg, 5, 1>; // Short-circuit detector interrupt enable
        using CKABIE = Field<DFSDM0_CR2_Reg, 6, 1>; // Clock absence interrupt enable
        using EXCH = Field<DFSDM0_CR2_Reg, 8, 8>; // Extremes detector channel selection
        using AWDCH = Field<DFSDM0_CR2_Reg, 16, 8>; // Analog watchdog channel selection
    };

    // DFSDM control register 2
    using DFSDM1_CR2_Reg = Register<DFSDM_BASE + 0x184, ReadWrite>;
    struct _DFSDM1_CR2 {
        using JEOCIE = Field<DFSDM1_CR2_Reg, 0, 1>; // Injected end of conversion interrupt enable
        using REOCIE = Field<DFSDM1_CR2_Reg, 1, 1>; // Regular end of conversion interrupt enable
        using JOVRIE = Field<DFSDM1_CR2_Reg, 2, 1>; // Injected data overrun interrupt enable
        using ROVRIE = Field<DFSDM1_CR2_Reg, 3, 1>; // Regular data overrun interrupt enable
        using AWDIE = Field<DFSDM1_CR2_Reg, 4, 1>; // Analog watchdog interrupt enable
        using SCDIE = Field<DFSDM1_CR2_Reg, 5, 1>; // Short-circuit detector interrupt enable
        using CKABIE = Field<DFSDM1_CR2_Reg, 6, 1>; // Clock absence interrupt enable
        using EXCH = Field<DFSDM1_CR2_Reg, 8, 8>; // Extremes detector channel selection
        using AWDCH = Field<DFSDM1_CR2_Reg, 16, 8>; // Analog watchdog channel selection
    };

    // DFSDM control register 2
    using DFSDM2_CR2_Reg = Register<DFSDM_BASE + 0x204, ReadWrite>;
    struct _DFSDM2_CR2 {
        using JEOCIE = Field<DFSDM2_CR2_Reg, 0, 1>; // Injected end of conversion interrupt enable
        using REOCIE = Field<DFSDM2_CR2_Reg, 1, 1>; // Regular end of conversion interrupt enable
        using JOVRIE = Field<DFSDM2_CR2_Reg, 2, 1>; // Injected data overrun interrupt enable
        using ROVRIE = Field<DFSDM2_CR2_Reg, 3, 1>; // Regular data overrun interrupt enable
        using AWDIE = Field<DFSDM2_CR2_Reg, 4, 1>; // Analog watchdog interrupt enable
        using SCDIE = Field<DFSDM2_CR2_Reg, 5, 1>; // Short-circuit detector interrupt enable
        using CKABIE = Field<DFSDM2_CR2_Reg, 6, 1>; // Clock absence interrupt enable
        using EXCH = Field<DFSDM2_CR2_Reg, 8, 8>; // Extremes detector channel selection
        using AWDCH = Field<DFSDM2_CR2_Reg, 16, 8>; // Analog watchdog channel selection
    };

    // DFSDM control register 2
    using DFSDM3_CR2_Reg = Register<DFSDM_BASE + 0x384, ReadWrite>;
    struct _DFSDM3_CR2 {
        using JEOCIE = Field<DFSDM3_CR2_Reg, 0, 1>; // Injected end of conversion interrupt enable
        using REOCIE = Field<DFSDM3_CR2_Reg, 1, 1>; // Regular end of conversion interrupt enable
        using JOVRIE = Field<DFSDM3_CR2_Reg, 2, 1>; // Injected data overrun interrupt enable
        using ROVRIE = Field<DFSDM3_CR2_Reg, 3, 1>; // Regular data overrun interrupt enable
        using AWDIE = Field<DFSDM3_CR2_Reg, 4, 1>; // Analog watchdog interrupt enable
        using SCDIE = Field<DFSDM3_CR2_Reg, 5, 1>; // Short-circuit detector interrupt enable
        using CKABIE = Field<DFSDM3_CR2_Reg, 6, 1>; // Clock absence interrupt enable
        using EXCH = Field<DFSDM3_CR2_Reg, 8, 8>; // Extremes detector channel selection
        using AWDCH = Field<DFSDM3_CR2_Reg, 16, 8>; // Analog watchdog channel selection
    };

    // DFSDM interrupt and status register
    using DFSDM0_ISR_Reg = Register<DFSDM_BASE + 0x108, ReadOnly>;
    struct _DFSDM0_ISR {
        using JEOCF = Field<DFSDM0_ISR_Reg, 0, 1>; // End of injected conversion flag
        using REOCF = Field<DFSDM0_ISR_Reg, 1, 1>; // End of regular conversion flag
        using JOVRF = Field<DFSDM0_ISR_Reg, 2, 1>; // Injected conversion overrun flag
        using ROVRF = Field<DFSDM0_ISR_Reg, 3, 1>; // Regular conversion overrun flag
        using AWDF = Field<DFSDM0_ISR_Reg, 4, 1>; // Analog watchdog
        using JCIP = Field<DFSDM0_ISR_Reg, 13, 1>; // Injected conversion in progress status
        using RCIP = Field<DFSDM0_ISR_Reg, 14, 1>; // Regular conversion in progress status
        using CKABF = Field<DFSDM0_ISR_Reg, 16, 8>; // Clock absence flag
        using SCDF = Field<DFSDM0_ISR_Reg, 24, 8>; // short-circuit detector flag
    };

    // DFSDM interrupt and status register
    using DFSDM1_ISR_Reg = Register<DFSDM_BASE + 0x188, ReadOnly>;
    struct _DFSDM1_ISR {
        using JEOCF = Field<DFSDM1_ISR_Reg, 0, 1>; // End of injected conversion flag
        using REOCF = Field<DFSDM1_ISR_Reg, 1, 1>; // End of regular conversion flag
        using JOVRF = Field<DFSDM1_ISR_Reg, 2, 1>; // Injected conversion overrun flag
        using ROVRF = Field<DFSDM1_ISR_Reg, 3, 1>; // Regular conversion overrun flag
        using AWDF = Field<DFSDM1_ISR_Reg, 4, 1>; // Analog watchdog
        using JCIP = Field<DFSDM1_ISR_Reg, 13, 1>; // Injected conversion in progress status
        using RCIP = Field<DFSDM1_ISR_Reg, 14, 1>; // Regular conversion in progress status
        using CKABF = Field<DFSDM1_ISR_Reg, 16, 8>; // Clock absence flag
        using SCDF = Field<DFSDM1_ISR_Reg, 24, 8>; // short-circuit detector flag
    };

    // DFSDM interrupt and status register
    using DFSDM2_ISR_Reg = Register<DFSDM_BASE + 0x208, ReadOnly>;
    struct _DFSDM2_ISR {
        using JEOCF = Field<DFSDM2_ISR_Reg, 0, 1>; // End of injected conversion flag
        using REOCF = Field<DFSDM2_ISR_Reg, 1, 1>; // End of regular conversion flag
        using JOVRF = Field<DFSDM2_ISR_Reg, 2, 1>; // Injected conversion overrun flag
        using ROVRF = Field<DFSDM2_ISR_Reg, 3, 1>; // Regular conversion overrun flag
        using AWDF = Field<DFSDM2_ISR_Reg, 4, 1>; // Analog watchdog
        using JCIP = Field<DFSDM2_ISR_Reg, 13, 1>; // Injected conversion in progress status
        using RCIP = Field<DFSDM2_ISR_Reg, 14, 1>; // Regular conversion in progress status
        using CKABF = Field<DFSDM2_ISR_Reg, 16, 8>; // Clock absence flag
        using SCDF = Field<DFSDM2_ISR_Reg, 24, 8>; // short-circuit detector flag
    };

    // DFSDM interrupt and status register
    using DFSDM3_ISR_Reg = Register<DFSDM_BASE + 0x388, ReadOnly>;
    struct _DFSDM3_ISR {
        using JEOCF = Field<DFSDM3_ISR_Reg, 0, 1>; // End of injected conversion flag
        using REOCF = Field<DFSDM3_ISR_Reg, 1, 1>; // End of regular conversion flag
        using JOVRF = Field<DFSDM3_ISR_Reg, 2, 1>; // Injected conversion overrun flag
        using ROVRF = Field<DFSDM3_ISR_Reg, 3, 1>; // Regular conversion overrun flag
        using AWDF = Field<DFSDM3_ISR_Reg, 4, 1>; // Analog watchdog
        using JCIP = Field<DFSDM3_ISR_Reg, 13, 1>; // Injected conversion in progress status
        using RCIP = Field<DFSDM3_ISR_Reg, 14, 1>; // Regular conversion in progress status
        using CKABF = Field<DFSDM3_ISR_Reg, 16, 8>; // Clock absence flag
        using SCDF = Field<DFSDM3_ISR_Reg, 24, 8>; // short-circuit detector flag
    };

    // DFSDM interrupt flag clear register
    using DFSDM0_ICR_Reg = Register<DFSDM_BASE + 0x10C, ReadWrite>;
    struct _DFSDM0_ICR {
        using CLRJOVRF = Field<DFSDM0_ICR_Reg, 2, 1>; // Clear the injected conversion overrun flag
        using CLRROVRF = Field<DFSDM0_ICR_Reg, 3, 1>; // Clear the regular conversion overrun flag
        using CLRCKABF = Field<DFSDM0_ICR_Reg, 16, 8>; // Clear the clock absence flag
        using CLRSCDF = Field<DFSDM0_ICR_Reg, 24, 8>; // Clear the short-circuit detector flag
    };

    // DFSDM interrupt flag clear register
    using DFSDM1_ICR_Reg = Register<DFSDM_BASE + 0x18C, ReadWrite>;
    struct _DFSDM1_ICR {
        using CLRJOVRF = Field<DFSDM1_ICR_Reg, 2, 1>; // Clear the injected conversion overrun flag
        using CLRROVRF = Field<DFSDM1_ICR_Reg, 3, 1>; // Clear the regular conversion overrun flag
        using CLRCKABF = Field<DFSDM1_ICR_Reg, 16, 8>; // Clear the clock absence flag
        using CLRSCDF = Field<DFSDM1_ICR_Reg, 24, 8>; // Clear the short-circuit detector flag
    };

    // DFSDM interrupt flag clear register
    using DFSDM2_ICR_Reg = Register<DFSDM_BASE + 0x20C, ReadWrite>;
    struct _DFSDM2_ICR {
        using CLRJOVRF = Field<DFSDM2_ICR_Reg, 2, 1>; // Clear the injected conversion overrun flag
        using CLRROVRF = Field<DFSDM2_ICR_Reg, 3, 1>; // Clear the regular conversion overrun flag
        using CLRCKABF = Field<DFSDM2_ICR_Reg, 16, 8>; // Clear the clock absence flag
        using CLRSCDF = Field<DFSDM2_ICR_Reg, 24, 8>; // Clear the short-circuit detector flag
    };

    // DFSDM interrupt flag clear register
    using DFSDM3_ICR_Reg = Register<DFSDM_BASE + 0x38C, ReadWrite>;
    struct _DFSDM3_ICR {
        using CLRJOVRF = Field<DFSDM3_ICR_Reg, 2, 1>; // Clear the injected conversion overrun flag
        using CLRROVRF = Field<DFSDM3_ICR_Reg, 3, 1>; // Clear the regular conversion overrun flag
        using CLRCKABF = Field<DFSDM3_ICR_Reg, 16, 8>; // Clear the clock absence flag
        using CLRSCDF = Field<DFSDM3_ICR_Reg, 24, 8>; // Clear the short-circuit detector flag
    };

    // DFSDM injected channel group selection register
    using DFSDM0_JCHGR_Reg = Register<DFSDM_BASE + 0x110, ReadWrite>;
    struct _DFSDM0_JCHGR {
        using JCHG = Field<DFSDM0_JCHGR_Reg, 0, 8>; // Injected channel group selection
    };

    // DFSDM injected channel group selection register
    using DFSDM1_JCHGR_Reg = Register<DFSDM_BASE + 0x190, ReadWrite>;
    struct _DFSDM1_JCHGR {
        using JCHG = Field<DFSDM1_JCHGR_Reg, 0, 8>; // Injected channel group selection
    };

    // DFSDM injected channel group selection register
    using DFSDM2_JCHGR_Reg = Register<DFSDM_BASE + 0x210, ReadWrite>;
    struct _DFSDM2_JCHGR {
        using JCHG = Field<DFSDM2_JCHGR_Reg, 0, 8>; // Injected channel group selection
    };

    // DFSDM injected channel group selection register
    using DFSDM3_JCHGR_Reg = Register<DFSDM_BASE + 0x310, ReadWrite>;
    struct _DFSDM3_JCHGR {
        using JCHG = Field<DFSDM3_JCHGR_Reg, 0, 8>; // Injected channel group selection
    };

    // DFSDM filter control register
    using DFSDM0_FCR_Reg = Register<DFSDM_BASE + 0x114, ReadWrite>;
    struct _DFSDM0_FCR {
        using IOSR = Field<DFSDM0_FCR_Reg, 0, 8>; // Integrator oversampling ratio (averaging length)
        using FOSR = Field<DFSDM0_FCR_Reg, 16, 10>; // Sinc filter oversampling ratio (decimation rate)
        using FORD = Field<DFSDM0_FCR_Reg, 29, 3>; // Sinc filter order
    };

    // DFSDM filter control register
    using DFSDM1_FCR_Reg = Register<DFSDM_BASE + 0x194, ReadWrite>;
    struct _DFSDM1_FCR {
        using IOSR = Field<DFSDM1_FCR_Reg, 0, 8>; // Integrator oversampling ratio (averaging length)
        using FOSR = Field<DFSDM1_FCR_Reg, 16, 10>; // Sinc filter oversampling ratio (decimation rate)
        using FORD = Field<DFSDM1_FCR_Reg, 29, 3>; // Sinc filter order
    };

    // DFSDM filter control register
    using DFSDM2_FCR_Reg = Register<DFSDM_BASE + 0x214, ReadWrite>;
    struct _DFSDM2_FCR {
        using IOSR = Field<DFSDM2_FCR_Reg, 0, 8>; // Integrator oversampling ratio (averaging length)
        using FOSR = Field<DFSDM2_FCR_Reg, 16, 10>; // Sinc filter oversampling ratio (decimation rate)
        using FORD = Field<DFSDM2_FCR_Reg, 29, 3>; // Sinc filter order
    };

    // DFSDM filter control register
    using DFSDM3_FCR_Reg = Register<DFSDM_BASE + 0x314, ReadWrite>;
    struct _DFSDM3_FCR {
        using IOSR = Field<DFSDM3_FCR_Reg, 0, 8>; // Integrator oversampling ratio (averaging length)
        using FOSR = Field<DFSDM3_FCR_Reg, 16, 10>; // Sinc filter oversampling ratio (decimation rate)
        using FORD = Field<DFSDM3_FCR_Reg, 29, 3>; // Sinc filter order
    };

    // DFSDM data register for injected group
    using DFSDM0_JDATAR_Reg = Register<DFSDM_BASE + 0x118, ReadOnly>;
    struct _DFSDM0_JDATAR {
        using JDATACH = Field<DFSDM0_JDATAR_Reg, 0, 3>; // Injected channel most recently converted
        using JDATA = Field<DFSDM0_JDATAR_Reg, 8, 24>; // Injected group conversion data
    };

    // DFSDM data register for injected group
    using DFSDM1_JDATAR_Reg = Register<DFSDM_BASE + 0x198, ReadOnly>;
    struct _DFSDM1_JDATAR {
        using JDATACH = Field<DFSDM1_JDATAR_Reg, 0, 3>; // Injected channel most recently converted
        using JDATA = Field<DFSDM1_JDATAR_Reg, 8, 24>; // Injected group conversion data
    };

    // DFSDM data register for injected group
    using DFSDM2_JDATAR_Reg = Register<DFSDM_BASE + 0x218, ReadOnly>;
    struct _DFSDM2_JDATAR {
        using JDATACH = Field<DFSDM2_JDATAR_Reg, 0, 3>; // Injected channel most recently converted
        using JDATA = Field<DFSDM2_JDATAR_Reg, 8, 24>; // Injected group conversion data
    };

    // DFSDM data register for injected group
    using DFSDM3_JDATAR_Reg = Register<DFSDM_BASE + 0x318, ReadOnly>;
    struct _DFSDM3_JDATAR {
        using JDATACH = Field<DFSDM3_JDATAR_Reg, 0, 3>; // Injected channel most recently converted
        using JDATA = Field<DFSDM3_JDATAR_Reg, 8, 24>; // Injected group conversion data
    };

    // DFSDM data register for the regular channel
    using DFSDM0_RDATAR_Reg = Register<DFSDM_BASE + 0x11C, ReadOnly>;
    struct _DFSDM0_RDATAR {
        using RDATACH = Field<DFSDM0_RDATAR_Reg, 0, 3>; // Regular channel most recently converted
        using RPEND = Field<DFSDM0_RDATAR_Reg, 4, 1>; // Regular channel pending data
        using RDATA = Field<DFSDM0_RDATAR_Reg, 8, 24>; // Regular channel conversion data
    };

    // DFSDM data register for the regular channel
    using DFSDM1_RDATAR_Reg = Register<DFSDM_BASE + 0x198, ReadOnly>;
    struct _DFSDM1_RDATAR {
        using RDATACH = Field<DFSDM1_RDATAR_Reg, 0, 3>; // Regular channel most recently converted
        using RPEND = Field<DFSDM1_RDATAR_Reg, 4, 1>; // Regular channel pending data
        using RDATA = Field<DFSDM1_RDATAR_Reg, 8, 24>; // Regular channel conversion data
    };

    // DFSDM data register for the regular channel
    using DFSDM2_RDATAR_Reg = Register<DFSDM_BASE + 0x218, ReadOnly>;
    struct _DFSDM2_RDATAR {
        using RDATACH = Field<DFSDM2_RDATAR_Reg, 0, 3>; // Regular channel most recently converted
        using RPEND = Field<DFSDM2_RDATAR_Reg, 4, 1>; // Regular channel pending data
        using RDATA = Field<DFSDM2_RDATAR_Reg, 8, 24>; // Regular channel conversion data
    };

    // DFSDM data register for the regular channel
    using DFSDM3_RDATAR_Reg = Register<DFSDM_BASE + 0x318, ReadOnly>;
    struct _DFSDM3_RDATAR {
        using RDATACH = Field<DFSDM3_RDATAR_Reg, 0, 3>; // Regular channel most recently converted
        using RPEND = Field<DFSDM3_RDATAR_Reg, 4, 1>; // Regular channel pending data
        using RDATA = Field<DFSDM3_RDATAR_Reg, 8, 24>; // Regular channel conversion data
    };

    // DFSDM analog watchdog high threshold register
    using DFSDM0_AWHTR_Reg = Register<DFSDM_BASE + 0x120, ReadWrite>;
    struct _DFSDM0_AWHTR {
        using BKAWH = Field<DFSDM0_AWHTR_Reg, 0, 4>; // Break signal assignment to analog watchdog high threshold event
        using AWHT = Field<DFSDM0_AWHTR_Reg, 8, 24>; // Analog watchdog high threshold
    };

    // DFSDM analog watchdog high threshold register
    using DFSDM1_AWHTR_Reg = Register<DFSDM_BASE + 0x1A0, ReadWrite>;
    struct _DFSDM1_AWHTR {
        using BKAWH = Field<DFSDM1_AWHTR_Reg, 0, 4>; // Break signal assignment to analog watchdog high threshold event
        using AWHT = Field<DFSDM1_AWHTR_Reg, 8, 24>; // Analog watchdog high threshold
    };

    // DFSDM analog watchdog high threshold register
    using DFSDM2_AWHTR_Reg = Register<DFSDM_BASE + 0x220, ReadWrite>;
    struct _DFSDM2_AWHTR {
        using BKAWH = Field<DFSDM2_AWHTR_Reg, 0, 4>; // Break signal assignment to analog watchdog high threshold event
        using AWHT = Field<DFSDM2_AWHTR_Reg, 8, 24>; // Analog watchdog high threshold
    };

    // DFSDM analog watchdog high threshold register
    using DFSDM3_AWHTR_Reg = Register<DFSDM_BASE + 0x2A0, ReadWrite>;
    struct _DFSDM3_AWHTR {
        using BKAWH = Field<DFSDM3_AWHTR_Reg, 0, 4>; // Break signal assignment to analog watchdog high threshold event
        using AWHT = Field<DFSDM3_AWHTR_Reg, 8, 24>; // Analog watchdog high threshold
    };

    // DFSDM analog watchdog low threshold register
    using DFSDM0_AWLTR_Reg = Register<DFSDM_BASE + 0x124, ReadWrite>;
    struct _DFSDM0_AWLTR {
        using BKAWL = Field<DFSDM0_AWLTR_Reg, 0, 4>; // Break signal assignment to analog watchdog low threshold event
        using AWLT = Field<DFSDM0_AWLTR_Reg, 8, 24>; // Analog watchdog low threshold
    };

    // DFSDM analog watchdog low threshold register
    using DFSDM1_AWLTR_Reg = Register<DFSDM_BASE + 0x1A4, ReadWrite>;
    struct _DFSDM1_AWLTR {
        using BKAWL = Field<DFSDM1_AWLTR_Reg, 0, 4>; // Break signal assignment to analog watchdog low threshold event
        using AWLT = Field<DFSDM1_AWLTR_Reg, 8, 24>; // Analog watchdog low threshold
    };

    // DFSDM analog watchdog low threshold register
    using DFSDM2_AWLTR_Reg = Register<DFSDM_BASE + 0x224, ReadWrite>;
    struct _DFSDM2_AWLTR {
        using BKAWL = Field<DFSDM2_AWLTR_Reg, 0, 4>; // Break signal assignment to analog watchdog low threshold event
        using AWLT = Field<DFSDM2_AWLTR_Reg, 8, 24>; // Analog watchdog low threshold
    };

    // DFSDM analog watchdog low threshold register
    using DFSDM3_AWLTR_Reg = Register<DFSDM_BASE + 0x2A4, ReadWrite>;
    struct _DFSDM3_AWLTR {
        using BKAWL = Field<DFSDM3_AWLTR_Reg, 0, 4>; // Break signal assignment to analog watchdog low threshold event
        using AWLT = Field<DFSDM3_AWLTR_Reg, 8, 24>; // Analog watchdog low threshold
    };

    // DFSDM analog watchdog status register
    using DFSDM0_AWSR_Reg = Register<DFSDM_BASE + 0x128, ReadOnly>;
    struct _DFSDM0_AWSR {
        using AWLTF = Field<DFSDM0_AWSR_Reg, 0, 8>; // Analog watchdog low threshold flag
        using AWHTF = Field<DFSDM0_AWSR_Reg, 8, 8>; // Analog watchdog high threshold flag
    };

    // DFSDM analog watchdog status register
    using DFSDM1_AWSR_Reg = Register<DFSDM_BASE + 0x1A8, ReadOnly>;
    struct _DFSDM1_AWSR {
        using AWLTF = Field<DFSDM1_AWSR_Reg, 0, 8>; // Analog watchdog low threshold flag
        using AWHTF = Field<DFSDM1_AWSR_Reg, 8, 8>; // Analog watchdog high threshold flag
    };

    // DFSDM analog watchdog status register
    using DFSDM2_AWSR_Reg = Register<DFSDM_BASE + 0x228, ReadOnly>;
    struct _DFSDM2_AWSR {
        using AWLTF = Field<DFSDM2_AWSR_Reg, 0, 8>; // Analog watchdog low threshold flag
        using AWHTF = Field<DFSDM2_AWSR_Reg, 8, 8>; // Analog watchdog high threshold flag
    };

    // DFSDM analog watchdog status register
    using DFSDM3_AWSR_Reg = Register<DFSDM_BASE + 0x2A8, ReadOnly>;
    struct _DFSDM3_AWSR {
        using AWLTF = Field<DFSDM3_AWSR_Reg, 0, 8>; // Analog watchdog low threshold flag
        using AWHTF = Field<DFSDM3_AWSR_Reg, 8, 8>; // Analog watchdog high threshold flag
    };

    // DFSDM analog watchdog clear flag register
    using DFSDM0_AWCFR_Reg = Register<DFSDM_BASE + 0x12C, ReadWrite>;
    struct _DFSDM0_AWCFR {
        using CLRAWLTF = Field<DFSDM0_AWCFR_Reg, 0, 8>; // Clear the analog watchdog low threshold flag
        using CLRAWHTF = Field<DFSDM0_AWCFR_Reg, 8, 8>; // Clear the analog watchdog high threshold flag
    };

    // DFSDM analog watchdog clear flag register
    using DFSDM1_AWCFR_Reg = Register<DFSDM_BASE + 0x1AC, ReadWrite>;
    struct _DFSDM1_AWCFR {
        using CLRAWLTF = Field<DFSDM1_AWCFR_Reg, 0, 8>; // Clear the analog watchdog low threshold flag
        using CLRAWHTF = Field<DFSDM1_AWCFR_Reg, 8, 8>; // Clear the analog watchdog high threshold flag
    };

    // DFSDM analog watchdog clear flag register
    using DFSDM2_AWCFR_Reg = Register<DFSDM_BASE + 0x22C, ReadWrite>;
    struct _DFSDM2_AWCFR {
        using CLRAWLTF = Field<DFSDM2_AWCFR_Reg, 0, 8>; // Clear the analog watchdog low threshold flag
        using CLRAWHTF = Field<DFSDM2_AWCFR_Reg, 8, 8>; // Clear the analog watchdog high threshold flag
    };

    // DFSDM analog watchdog clear flag register
    using DFSDM3_AWCFR_Reg = Register<DFSDM_BASE + 0x2AC, ReadWrite>;
    struct _DFSDM3_AWCFR {
        using CLRAWLTF = Field<DFSDM3_AWCFR_Reg, 0, 8>; // Clear the analog watchdog low threshold flag
        using CLRAWHTF = Field<DFSDM3_AWCFR_Reg, 8, 8>; // Clear the analog watchdog high threshold flag
    };

    // DFSDM Extremes detector maximum register
    using DFSDM0_EXMAX_Reg = Register<DFSDM_BASE + 0x130, ReadOnly>;
    struct _DFSDM0_EXMAX {
        using EXMAXCH = Field<DFSDM0_EXMAX_Reg, 0, 3>; // Extremes detector maximum data channel
        using EXMAX = Field<DFSDM0_EXMAX_Reg, 8, 24>; // Extremes detector maximum value
    };

    // DFSDM Extremes detector maximum register
    using DFSDM1_EXMAX_Reg = Register<DFSDM_BASE + 0x1B0, ReadOnly>;
    struct _DFSDM1_EXMAX {
        using EXMAXCH = Field<DFSDM1_EXMAX_Reg, 0, 3>; // Extremes detector maximum data channel
        using EXMAX = Field<DFSDM1_EXMAX_Reg, 8, 24>; // Extremes detector maximum value
    };

    // DFSDM Extremes detector maximum register
    using DFSDM2_EXMAX_Reg = Register<DFSDM_BASE + 0x230, ReadOnly>;
    struct _DFSDM2_EXMAX {
        using EXMAXCH = Field<DFSDM2_EXMAX_Reg, 0, 3>; // Extremes detector maximum data channel
        using EXMAX = Field<DFSDM2_EXMAX_Reg, 8, 24>; // Extremes detector maximum value
    };

    // DFSDM Extremes detector maximum register
    using DFSDM3_EXMAX_Reg = Register<DFSDM_BASE + 0x2B0, ReadOnly>;
    struct _DFSDM3_EXMAX {
        using EXMAXCH = Field<DFSDM3_EXMAX_Reg, 0, 3>; // Extremes detector maximum data channel
        using EXMAX = Field<DFSDM3_EXMAX_Reg, 8, 24>; // Extremes detector maximum value
    };

    // DFSDM Extremes detector minimum register
    using DFSDM0_EXMIN_Reg = Register<DFSDM_BASE + 0x134, ReadOnly>;
    struct _DFSDM0_EXMIN {
        using EXMINCH = Field<DFSDM0_EXMIN_Reg, 0, 3>; // Extremes detector minimum data channel
        using EXMIN = Field<DFSDM0_EXMIN_Reg, 8, 24>; // Extremes detector minimum value
    };

    // DFSDM Extremes detector minimum register
    using DFSDM1_EXMIN_Reg = Register<DFSDM_BASE + 0x1B4, ReadOnly>;
    struct _DFSDM1_EXMIN {
        using EXMINCH = Field<DFSDM1_EXMIN_Reg, 0, 3>; // Extremes detector minimum data channel
        using EXMIN = Field<DFSDM1_EXMIN_Reg, 8, 24>; // Extremes detector minimum value
    };

    // DFSDM Extremes detector minimum register
    using DFSDM2_EXMIN_Reg = Register<DFSDM_BASE + 0x234, ReadOnly>;
    struct _DFSDM2_EXMIN {
        using EXMINCH = Field<DFSDM2_EXMIN_Reg, 0, 3>; // Extremes detector minimum data channel
        using EXMIN = Field<DFSDM2_EXMIN_Reg, 8, 24>; // Extremes detector minimum value
    };

    // DFSDM Extremes detector minimum register
    using DFSDM3_EXMIN_Reg = Register<DFSDM_BASE + 0x2B4, ReadOnly>;
    struct _DFSDM3_EXMIN {
        using EXMINCH = Field<DFSDM3_EXMIN_Reg, 0, 3>; // Extremes detector minimum data channel
        using EXMIN = Field<DFSDM3_EXMIN_Reg, 8, 24>; // Extremes detector minimum value
    };

    // DFSDM conversion timer register
    using DFSDM0_CNVTIMR_Reg = Register<DFSDM_BASE + 0x138, ReadOnly>;
    struct _DFSDM0_CNVTIMR {
        using CNVCNT = Field<DFSDM0_CNVTIMR_Reg, 4, 28>; // 28-bit timer counting conversion time
    };

    // DFSDM conversion timer register
    using DFSDM1_CNVTIMR_Reg = Register<DFSDM_BASE + 0x1B8, ReadOnly>;
    struct _DFSDM1_CNVTIMR {
        using CNVCNT = Field<DFSDM1_CNVTIMR_Reg, 4, 28>; // 28-bit timer counting conversion time
    };

    // DFSDM conversion timer register
    using DFSDM2_CNVTIMR_Reg = Register<DFSDM_BASE + 0x238, ReadOnly>;
    struct _DFSDM2_CNVTIMR {
        using CNVCNT = Field<DFSDM2_CNVTIMR_Reg, 4, 28>; // 28-bit timer counting conversion time
    };

    // DFSDM conversion timer register
    using DFSDM3_CNVTIMR_Reg = Register<DFSDM_BASE + 0x2B8, ReadOnly>;
    struct _DFSDM3_CNVTIMR {
        using CNVCNT = Field<DFSDM3_CNVTIMR_Reg, 4, 28>; // 28-bit timer counting conversion time
    };

} // namespace DFSDM

// --------------------------------------------
// JPEG: JPEG codec
// Base address: 0x50051000
// --------------------------------------------

namespace JPEG {
    static constexpr uint32_t JPEG_BASE = 0x50051000;

    // JPEG codec configuration register 0
    using JPEG_CONFR0_Reg = Register<JPEG_BASE + 0x0, WriteOnly>;
    struct _JPEG_CONFR0 {
        using START = Field<JPEG_CONFR0_Reg, 0, 1>; // Start
    };

    // JPEG codec configuration register 1
    using JPEG_CONFR1_Reg = Register<JPEG_BASE + 0x4, ReadWrite>;
    struct _JPEG_CONFR1 {
        using NF = Field<JPEG_CONFR1_Reg, 0, 2>; // Number of color components
        using DE = Field<JPEG_CONFR1_Reg, 3, 1>; // Decoding Enable
        using COLORSPACE = Field<JPEG_CONFR1_Reg, 4, 2>; // Color Space
        using NS = Field<JPEG_CONFR1_Reg, 6, 2>; // Number of components for Scan
        using HDR = Field<JPEG_CONFR1_Reg, 8, 1>; // Header Processing
        using YSIZE = Field<JPEG_CONFR1_Reg, 16, 16>; // Y Size
    };

    // JPEG codec configuration register 2
    using JPEG_CONFR2_Reg = Register<JPEG_BASE + 0x8, ReadWrite>;
    struct _JPEG_CONFR2 {
        using NMCU = Field<JPEG_CONFR2_Reg, 0, 26>; // Number of MCU
    };

    // JPEG codec configuration register 3
    using JPEG_CONFR3_Reg = Register<JPEG_BASE + 0xC, ReadWrite>;
    struct _JPEG_CONFR3 {
        using XSIZE = Field<JPEG_CONFR3_Reg, 16, 16>; // X size
    };

    // JPEG codec configuration register 4
    using JPEG_CONFR4_Reg = Register<JPEG_BASE + 0x10, ReadWrite>;
    struct _JPEG_CONFR4 {
        using HD = Field<JPEG_CONFR4_Reg, 0, 1>; // Huffman DC
        using HA = Field<JPEG_CONFR4_Reg, 1, 1>; // Huffman AC
        using QT = Field<JPEG_CONFR4_Reg, 2, 2>; // Quantization Table
        using NB = Field<JPEG_CONFR4_Reg, 4, 4>; // Number of Block
        using VSF = Field<JPEG_CONFR4_Reg, 8, 4>; // Vertical Sampling Factor
        using HSF = Field<JPEG_CONFR4_Reg, 12, 4>; // Horizontal Sampling Factor
    };

    // JPEG codec configuration register 5
    using JPEG_CONFR5_Reg = Register<JPEG_BASE + 0x14, ReadWrite>;
    struct _JPEG_CONFR5 {
        using HD = Field<JPEG_CONFR5_Reg, 0, 1>; // Huffman DC
        using HA = Field<JPEG_CONFR5_Reg, 1, 1>; // Huffman AC
        using QT = Field<JPEG_CONFR5_Reg, 2, 2>; // Quantization Table
        using NB = Field<JPEG_CONFR5_Reg, 4, 4>; // Number of Block
        using VSF = Field<JPEG_CONFR5_Reg, 8, 4>; // Vertical Sampling Factor
        using HSF = Field<JPEG_CONFR5_Reg, 12, 4>; // Horizontal Sampling Factor
    };

    // JPEG codec configuration register 6
    using JPEG_CONFR6_Reg = Register<JPEG_BASE + 0x18, ReadWrite>;
    struct _JPEG_CONFR6 {
        using HD = Field<JPEG_CONFR6_Reg, 0, 1>; // Huffman DC
        using HA = Field<JPEG_CONFR6_Reg, 1, 1>; // Huffman AC
        using QT = Field<JPEG_CONFR6_Reg, 2, 2>; // Quantization Table
        using NB = Field<JPEG_CONFR6_Reg, 4, 4>; // Number of Block
        using VSF = Field<JPEG_CONFR6_Reg, 8, 4>; // Vertical Sampling Factor
        using HSF = Field<JPEG_CONFR6_Reg, 12, 4>; // Horizontal Sampling Factor
    };

    // JPEG codec configuration register 7
    using JPEG_CONFR7_Reg = Register<JPEG_BASE + 0x1C, ReadWrite>;
    struct _JPEG_CONFR7 {
        using HD = Field<JPEG_CONFR7_Reg, 0, 1>; // Huffman DC
        using HA = Field<JPEG_CONFR7_Reg, 1, 1>; // Huffman AC
        using QT = Field<JPEG_CONFR7_Reg, 2, 2>; // Quantization Table
        using NB = Field<JPEG_CONFR7_Reg, 4, 4>; // Number of Block
        using VSF = Field<JPEG_CONFR7_Reg, 8, 4>; // Vertical Sampling Factor
        using HSF = Field<JPEG_CONFR7_Reg, 12, 4>; // Horizontal Sampling Factor
    };

    // JPEG control register
    using JPEG_CR_Reg = Register<JPEG_BASE + 0x30, ReadWrite>;
    struct _JPEG_CR {
        using JCEN = Field<JPEG_CR_Reg, 0, 1>; // JPEG Core Enable
        using IFTIE = Field<JPEG_CR_Reg, 1, 1>; // Input FIFO Threshold Interrupt Enable
        using IFNFIE = Field<JPEG_CR_Reg, 2, 1>; // Input FIFO Not Full Interrupt Enable
        using OFTIE = Field<JPEG_CR_Reg, 3, 1>; // Output FIFO Threshold Interrupt Enable
        using OFNEIE = Field<JPEG_CR_Reg, 4, 1>; // Output FIFO Not Empty Interrupt Enable
        using EOCIE = Field<JPEG_CR_Reg, 5, 1>; // End of Conversion Interrupt Enable
        using HPDIE = Field<JPEG_CR_Reg, 6, 1>; // Header Parsing Done Interrupt Enable
        using IDMAEN = Field<JPEG_CR_Reg, 11, 1>; // Input DMA Enable
        using ODMAEN = Field<JPEG_CR_Reg, 12, 1>; // Output DMA Enable
        using IFF = Field<JPEG_CR_Reg, 13, 1>; // Input FIFO Flush
        using OFF = Field<JPEG_CR_Reg, 14, 1>; // Output FIFO Flush
    };

    // JPEG status register
    using JPEG_SR_Reg = Register<JPEG_BASE + 0x34, ReadOnly>;
    struct _JPEG_SR {
        using IFTF = Field<JPEG_SR_Reg, 1, 1>; // Input FIFO Threshold Flag
        using IFNFF = Field<JPEG_SR_Reg, 2, 1>; // Input FIFO Not Full Flag
        using OFTF = Field<JPEG_SR_Reg, 3, 1>; // Output FIFO Threshold Flag
        using OFNEF = Field<JPEG_SR_Reg, 4, 1>; // Output FIFO Not Empty Flag
        using EOCF = Field<JPEG_SR_Reg, 5, 1>; // End of Conversion Flag
        using HPDF = Field<JPEG_SR_Reg, 6, 1>; // Header Parsing Done Flag
        using COF = Field<JPEG_SR_Reg, 7, 1>; // Codec Operation Flag
    };

    // JPEG clear flag register
    using JPEG_CFR_Reg = Register<JPEG_BASE + 0x38, WriteOnly>;
    struct _JPEG_CFR {
        using CEOCF = Field<JPEG_CFR_Reg, 5, 1>; // Clear End of Conversion Flag
        using CHPDF = Field<JPEG_CFR_Reg, 6, 1>; // Clear Header Parsing Done Flag
    };

    // JPEG data input register
    using JPEG_DIR_Reg = Register<JPEG_BASE + 0x40, WriteOnly>;
    struct _JPEG_DIR {
        using DATAIN = Field<JPEG_DIR_Reg, 0, 32>; // Data Input FIFO
    };

    // JPEG data output register
    using JPEG_DOR_Reg = Register<JPEG_BASE + 0x44, ReadOnly>;
    struct _JPEG_DOR {
        using DATAOUT = Field<JPEG_DOR_Reg, 0, 32>; // Data Output FIFO
    };

    // JPEG quantization tables
    using QMEM0_0_Reg = Register<JPEG_BASE + 0x50, ReadWrite>;
    struct _QMEM0_0 {
        using QMem_RAM = Field<QMEM0_0_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM0_1_Reg = Register<JPEG_BASE + 0x54, ReadWrite>;
    struct _QMEM0_1 {
        using QMem_RAM = Field<QMEM0_1_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM0_2_Reg = Register<JPEG_BASE + 0x58, ReadWrite>;
    struct _QMEM0_2 {
        using QMem_RAM = Field<QMEM0_2_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM0_3_Reg = Register<JPEG_BASE + 0x5C, ReadWrite>;
    struct _QMEM0_3 {
        using QMem_RAM = Field<QMEM0_3_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM0_4_Reg = Register<JPEG_BASE + 0x60, ReadWrite>;
    struct _QMEM0_4 {
        using QMem_RAM = Field<QMEM0_4_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM0_5_Reg = Register<JPEG_BASE + 0x64, ReadWrite>;
    struct _QMEM0_5 {
        using QMem_RAM = Field<QMEM0_5_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM0_6_Reg = Register<JPEG_BASE + 0x68, ReadWrite>;
    struct _QMEM0_6 {
        using QMem_RAM = Field<QMEM0_6_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM0_7_Reg = Register<JPEG_BASE + 0x6C, ReadWrite>;
    struct _QMEM0_7 {
        using QMem_RAM = Field<QMEM0_7_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM0_8_Reg = Register<JPEG_BASE + 0x70, ReadWrite>;
    struct _QMEM0_8 {
        using QMem_RAM = Field<QMEM0_8_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM0_9_Reg = Register<JPEG_BASE + 0x74, ReadWrite>;
    struct _QMEM0_9 {
        using QMem_RAM = Field<QMEM0_9_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM0_10_Reg = Register<JPEG_BASE + 0x78, ReadWrite>;
    struct _QMEM0_10 {
        using QMem_RAM = Field<QMEM0_10_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM0_11_Reg = Register<JPEG_BASE + 0x7C, ReadWrite>;
    struct _QMEM0_11 {
        using QMem_RAM = Field<QMEM0_11_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM0_12_Reg = Register<JPEG_BASE + 0x80, ReadWrite>;
    struct _QMEM0_12 {
        using QMem_RAM = Field<QMEM0_12_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM0_13_Reg = Register<JPEG_BASE + 0x84, ReadWrite>;
    struct _QMEM0_13 {
        using QMem_RAM = Field<QMEM0_13_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM0_14_Reg = Register<JPEG_BASE + 0x88, ReadWrite>;
    struct _QMEM0_14 {
        using QMem_RAM = Field<QMEM0_14_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM0_15_Reg = Register<JPEG_BASE + 0x8C, ReadWrite>;
    struct _QMEM0_15 {
        using QMem_RAM = Field<QMEM0_15_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM1_0_Reg = Register<JPEG_BASE + 0x90, ReadWrite>;
    struct _QMEM1_0 {
        using QMem_RAM = Field<QMEM1_0_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM1_1_Reg = Register<JPEG_BASE + 0x94, ReadWrite>;
    struct _QMEM1_1 {
        using QMem_RAM = Field<QMEM1_1_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM1_2_Reg = Register<JPEG_BASE + 0x98, ReadWrite>;
    struct _QMEM1_2 {
        using QMem_RAM = Field<QMEM1_2_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM1_3_Reg = Register<JPEG_BASE + 0x9C, ReadWrite>;
    struct _QMEM1_3 {
        using QMem_RAM = Field<QMEM1_3_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM1_4_Reg = Register<JPEG_BASE + 0xA0, ReadWrite>;
    struct _QMEM1_4 {
        using QMem_RAM = Field<QMEM1_4_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM1_5_Reg = Register<JPEG_BASE + 0xA4, ReadWrite>;
    struct _QMEM1_5 {
        using QMem_RAM = Field<QMEM1_5_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM1_6_Reg = Register<JPEG_BASE + 0xA8, ReadWrite>;
    struct _QMEM1_6 {
        using QMem_RAM = Field<QMEM1_6_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM1_7_Reg = Register<JPEG_BASE + 0xAC, ReadWrite>;
    struct _QMEM1_7 {
        using QMem_RAM = Field<QMEM1_7_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM1_8_Reg = Register<JPEG_BASE + 0xB0, ReadWrite>;
    struct _QMEM1_8 {
        using QMem_RAM = Field<QMEM1_8_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM1_9_Reg = Register<JPEG_BASE + 0xB4, ReadWrite>;
    struct _QMEM1_9 {
        using QMem_RAM = Field<QMEM1_9_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM1_10_Reg = Register<JPEG_BASE + 0xB8, ReadWrite>;
    struct _QMEM1_10 {
        using QMem_RAM = Field<QMEM1_10_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM1_11_Reg = Register<JPEG_BASE + 0xBC, ReadWrite>;
    struct _QMEM1_11 {
        using QMem_RAM = Field<QMEM1_11_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM1_12_Reg = Register<JPEG_BASE + 0xC0, ReadWrite>;
    struct _QMEM1_12 {
        using QMem_RAM = Field<QMEM1_12_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM1_13_Reg = Register<JPEG_BASE + 0xC4, ReadWrite>;
    struct _QMEM1_13 {
        using QMem_RAM = Field<QMEM1_13_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM1_14_Reg = Register<JPEG_BASE + 0xC8, ReadWrite>;
    struct _QMEM1_14 {
        using QMem_RAM = Field<QMEM1_14_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM1_15_Reg = Register<JPEG_BASE + 0xCC, ReadWrite>;
    struct _QMEM1_15 {
        using QMem_RAM = Field<QMEM1_15_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM2_0_Reg = Register<JPEG_BASE + 0xD0, ReadWrite>;
    struct _QMEM2_0 {
        using QMem_RAM = Field<QMEM2_0_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM2_1_Reg = Register<JPEG_BASE + 0xD4, ReadWrite>;
    struct _QMEM2_1 {
        using QMem_RAM = Field<QMEM2_1_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM2_2_Reg = Register<JPEG_BASE + 0xD8, ReadWrite>;
    struct _QMEM2_2 {
        using QMem_RAM = Field<QMEM2_2_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM2_3_Reg = Register<JPEG_BASE + 0xDC, ReadWrite>;
    struct _QMEM2_3 {
        using QMem_RAM = Field<QMEM2_3_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM2_4_Reg = Register<JPEG_BASE + 0xE0, ReadWrite>;
    struct _QMEM2_4 {
        using QMem_RAM = Field<QMEM2_4_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM2_5_Reg = Register<JPEG_BASE + 0xE4, ReadWrite>;
    struct _QMEM2_5 {
        using QMem_RAM = Field<QMEM2_5_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM2_6_Reg = Register<JPEG_BASE + 0xE8, ReadWrite>;
    struct _QMEM2_6 {
        using QMem_RAM = Field<QMEM2_6_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM2_7_Reg = Register<JPEG_BASE + 0xEC, ReadWrite>;
    struct _QMEM2_7 {
        using QMem_RAM = Field<QMEM2_7_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM2_8_Reg = Register<JPEG_BASE + 0xF0, ReadWrite>;
    struct _QMEM2_8 {
        using QMem_RAM = Field<QMEM2_8_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM2_9_Reg = Register<JPEG_BASE + 0xF4, ReadWrite>;
    struct _QMEM2_9 {
        using QMem_RAM = Field<QMEM2_9_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM2_10_Reg = Register<JPEG_BASE + 0xF8, ReadWrite>;
    struct _QMEM2_10 {
        using QMem_RAM = Field<QMEM2_10_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM2_11_Reg = Register<JPEG_BASE + 0xFC, ReadWrite>;
    struct _QMEM2_11 {
        using QMem_RAM = Field<QMEM2_11_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM2_12_Reg = Register<JPEG_BASE + 0x100, ReadWrite>;
    struct _QMEM2_12 {
        using QMem_RAM = Field<QMEM2_12_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM2_13_Reg = Register<JPEG_BASE + 0x104, ReadWrite>;
    struct _QMEM2_13 {
        using QMem_RAM = Field<QMEM2_13_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM2_14_Reg = Register<JPEG_BASE + 0x108, ReadWrite>;
    struct _QMEM2_14 {
        using QMem_RAM = Field<QMEM2_14_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM2_15_Reg = Register<JPEG_BASE + 0x10C, ReadWrite>;
    struct _QMEM2_15 {
        using QMem_RAM = Field<QMEM2_15_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM3_0_Reg = Register<JPEG_BASE + 0x110, ReadWrite>;
    struct _QMEM3_0 {
        using QMem_RAM = Field<QMEM3_0_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM3_1_Reg = Register<JPEG_BASE + 0x114, ReadWrite>;
    struct _QMEM3_1 {
        using QMem_RAM = Field<QMEM3_1_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM3_2_Reg = Register<JPEG_BASE + 0x118, ReadWrite>;
    struct _QMEM3_2 {
        using QMem_RAM = Field<QMEM3_2_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM3_3_Reg = Register<JPEG_BASE + 0x11C, ReadWrite>;
    struct _QMEM3_3 {
        using QMem_RAM = Field<QMEM3_3_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM3_4_Reg = Register<JPEG_BASE + 0x120, ReadWrite>;
    struct _QMEM3_4 {
        using QMem_RAM = Field<QMEM3_4_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM3_5_Reg = Register<JPEG_BASE + 0x124, ReadWrite>;
    struct _QMEM3_5 {
        using QMem_RAM = Field<QMEM3_5_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM3_6_Reg = Register<JPEG_BASE + 0x128, ReadWrite>;
    struct _QMEM3_6 {
        using QMem_RAM = Field<QMEM3_6_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM3_7_Reg = Register<JPEG_BASE + 0x12C, ReadWrite>;
    struct _QMEM3_7 {
        using QMem_RAM = Field<QMEM3_7_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM3_8_Reg = Register<JPEG_BASE + 0x130, ReadWrite>;
    struct _QMEM3_8 {
        using QMem_RAM = Field<QMEM3_8_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM3_9_Reg = Register<JPEG_BASE + 0x134, ReadWrite>;
    struct _QMEM3_9 {
        using QMem_RAM = Field<QMEM3_9_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM3_10_Reg = Register<JPEG_BASE + 0x138, ReadWrite>;
    struct _QMEM3_10 {
        using QMem_RAM = Field<QMEM3_10_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM3_11_Reg = Register<JPEG_BASE + 0x13C, ReadWrite>;
    struct _QMEM3_11 {
        using QMem_RAM = Field<QMEM3_11_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM3_12_Reg = Register<JPEG_BASE + 0x140, ReadWrite>;
    struct _QMEM3_12 {
        using QMem_RAM = Field<QMEM3_12_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM3_13_Reg = Register<JPEG_BASE + 0x144, ReadWrite>;
    struct _QMEM3_13 {
        using QMem_RAM = Field<QMEM3_13_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM3_14_Reg = Register<JPEG_BASE + 0x148, ReadWrite>;
    struct _QMEM3_14 {
        using QMem_RAM = Field<QMEM3_14_Reg, 0, 32>; // QMem RAM
    };

    // JPEG quantization tables
    using QMEM3_15_Reg = Register<JPEG_BASE + 0x14C, ReadWrite>;
    struct _QMEM3_15 {
        using QMem_RAM = Field<QMEM3_15_Reg, 0, 32>; // QMem RAM
    };

    // JPEG HuffMin tables
    using HUFFMIN_0_Reg = Register<JPEG_BASE + 0x150, ReadWrite>;
    struct _HUFFMIN_0 {
        using HuffMin_RAM = Field<HUFFMIN_0_Reg, 0, 32>; // HuffMin RAM
    };

    // JPEG HuffMin tables
    using HUFFMIN_1_Reg = Register<JPEG_BASE + 0x154, ReadWrite>;
    struct _HUFFMIN_1 {
        using HuffMin_RAM = Field<HUFFMIN_1_Reg, 0, 32>; // HuffMin RAM
    };

    // JPEG HuffMin tables
    using HUFFMIN_2_Reg = Register<JPEG_BASE + 0x158, ReadWrite>;
    struct _HUFFMIN_2 {
        using HuffMin_RAM = Field<HUFFMIN_2_Reg, 0, 32>; // HuffMin RAM
    };

    // JPEG HuffMin tables
    using HUFFMIN_3_Reg = Register<JPEG_BASE + 0x15C, ReadWrite>;
    struct _HUFFMIN_3 {
        using HuffMin_RAM = Field<HUFFMIN_3_Reg, 0, 32>; // HuffMin RAM
    };

    // JPEG HuffMin tables
    using HUFFMIN_4_Reg = Register<JPEG_BASE + 0x160, ReadWrite>;
    struct _HUFFMIN_4 {
        using HuffMin_RAM = Field<HUFFMIN_4_Reg, 0, 32>; // HuffMin RAM
    };

    // JPEG HuffMin tables
    using HUFFMIN_5_Reg = Register<JPEG_BASE + 0x164, ReadWrite>;
    struct _HUFFMIN_5 {
        using HuffMin_RAM = Field<HUFFMIN_5_Reg, 0, 32>; // HuffMin RAM
    };

    // JPEG HuffMin tables
    using HUFFMIN_6_Reg = Register<JPEG_BASE + 0x168, ReadWrite>;
    struct _HUFFMIN_6 {
        using HuffMin_RAM = Field<HUFFMIN_6_Reg, 0, 32>; // HuffMin RAM
    };

    // JPEG HuffMin tables
    using HUFFMIN_7_Reg = Register<JPEG_BASE + 0x16C, ReadWrite>;
    struct _HUFFMIN_7 {
        using HuffMin_RAM = Field<HUFFMIN_7_Reg, 0, 32>; // HuffMin RAM
    };

    // JPEG HuffMin tables
    using HUFFMIN_8_Reg = Register<JPEG_BASE + 0x170, ReadWrite>;
    struct _HUFFMIN_8 {
        using HuffMin_RAM = Field<HUFFMIN_8_Reg, 0, 32>; // HuffMin RAM
    };

    // JPEG HuffMin tables
    using HUFFMIN_9_Reg = Register<JPEG_BASE + 0x174, ReadWrite>;
    struct _HUFFMIN_9 {
        using HuffMin_RAM = Field<HUFFMIN_9_Reg, 0, 32>; // HuffMin RAM
    };

    // JPEG HuffMin tables
    using HUFFMIN_10_Reg = Register<JPEG_BASE + 0x178, ReadWrite>;
    struct _HUFFMIN_10 {
        using HuffMin_RAM = Field<HUFFMIN_10_Reg, 0, 32>; // HuffMin RAM
    };

    // JPEG HuffMin tables
    using HUFFMIN_11_Reg = Register<JPEG_BASE + 0x17C, ReadWrite>;
    struct _HUFFMIN_11 {
        using HuffMin_RAM = Field<HUFFMIN_11_Reg, 0, 32>; // HuffMin RAM
    };

    // JPEG HuffMin tables
    using HUFFMIN_12_Reg = Register<JPEG_BASE + 0x180, ReadWrite>;
    struct _HUFFMIN_12 {
        using HuffMin_RAM = Field<HUFFMIN_12_Reg, 0, 32>; // HuffMin RAM
    };

    // JPEG HuffMin tables
    using HUFFMIN_13_Reg = Register<JPEG_BASE + 0x184, ReadWrite>;
    struct _HUFFMIN_13 {
        using HuffMin_RAM = Field<HUFFMIN_13_Reg, 0, 32>; // HuffMin RAM
    };

    // JPEG HuffMin tables
    using HUFFMIN_14_Reg = Register<JPEG_BASE + 0x188, ReadWrite>;
    struct _HUFFMIN_14 {
        using HuffMin_RAM = Field<HUFFMIN_14_Reg, 0, 32>; // HuffMin RAM
    };

    // JPEG HuffMin tables
    using HUFFMIN_15_Reg = Register<JPEG_BASE + 0x18C, ReadWrite>;
    struct _HUFFMIN_15 {
        using HuffMin_RAM = Field<HUFFMIN_15_Reg, 0, 32>; // HuffMin RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE0_Reg = Register<JPEG_BASE + 0x190, ReadWrite>;
    struct _HUFFBASE0 {
        using HuffBase_RAM_0 = Field<HUFFBASE0_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE0_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE1_Reg = Register<JPEG_BASE + 0x194, ReadWrite>;
    struct _HUFFBASE1 {
        using HuffBase_RAM_0 = Field<HUFFBASE1_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE1_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE2_Reg = Register<JPEG_BASE + 0x198, ReadWrite>;
    struct _HUFFBASE2 {
        using HuffBase_RAM_0 = Field<HUFFBASE2_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE2_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE3_Reg = Register<JPEG_BASE + 0x19C, ReadWrite>;
    struct _HUFFBASE3 {
        using HuffBase_RAM_0 = Field<HUFFBASE3_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE3_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE4_Reg = Register<JPEG_BASE + 0x1A0, ReadWrite>;
    struct _HUFFBASE4 {
        using HuffBase_RAM_0 = Field<HUFFBASE4_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE4_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE5_Reg = Register<JPEG_BASE + 0x1A4, ReadWrite>;
    struct _HUFFBASE5 {
        using HuffBase_RAM_0 = Field<HUFFBASE5_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE5_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE6_Reg = Register<JPEG_BASE + 0x1A8, ReadWrite>;
    struct _HUFFBASE6 {
        using HuffBase_RAM_0 = Field<HUFFBASE6_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE6_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE7_Reg = Register<JPEG_BASE + 0x1AC, ReadWrite>;
    struct _HUFFBASE7 {
        using HuffBase_RAM_0 = Field<HUFFBASE7_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE7_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE8_Reg = Register<JPEG_BASE + 0x1B0, ReadWrite>;
    struct _HUFFBASE8 {
        using HuffBase_RAM_0 = Field<HUFFBASE8_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE8_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE9_Reg = Register<JPEG_BASE + 0x1B4, ReadWrite>;
    struct _HUFFBASE9 {
        using HuffBase_RAM_0 = Field<HUFFBASE9_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE9_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE10_Reg = Register<JPEG_BASE + 0x1B8, ReadWrite>;
    struct _HUFFBASE10 {
        using HuffBase_RAM_0 = Field<HUFFBASE10_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE10_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE11_Reg = Register<JPEG_BASE + 0x1BC, ReadWrite>;
    struct _HUFFBASE11 {
        using HuffBase_RAM_0 = Field<HUFFBASE11_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE11_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE12_Reg = Register<JPEG_BASE + 0x1C0, ReadWrite>;
    struct _HUFFBASE12 {
        using HuffBase_RAM_0 = Field<HUFFBASE12_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE12_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE13_Reg = Register<JPEG_BASE + 0x1C4, ReadWrite>;
    struct _HUFFBASE13 {
        using HuffBase_RAM_0 = Field<HUFFBASE13_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE13_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE14_Reg = Register<JPEG_BASE + 0x1C8, ReadWrite>;
    struct _HUFFBASE14 {
        using HuffBase_RAM_0 = Field<HUFFBASE14_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE14_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE15_Reg = Register<JPEG_BASE + 0x1CC, ReadWrite>;
    struct _HUFFBASE15 {
        using HuffBase_RAM_0 = Field<HUFFBASE15_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE15_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE16_Reg = Register<JPEG_BASE + 0x1D0, ReadWrite>;
    struct _HUFFBASE16 {
        using HuffBase_RAM_0 = Field<HUFFBASE16_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE16_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE17_Reg = Register<JPEG_BASE + 0x1D4, ReadWrite>;
    struct _HUFFBASE17 {
        using HuffBase_RAM_0 = Field<HUFFBASE17_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE17_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE18_Reg = Register<JPEG_BASE + 0x1D8, ReadWrite>;
    struct _HUFFBASE18 {
        using HuffBase_RAM_0 = Field<HUFFBASE18_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE18_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE19_Reg = Register<JPEG_BASE + 0x1DC, ReadWrite>;
    struct _HUFFBASE19 {
        using HuffBase_RAM_0 = Field<HUFFBASE19_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE19_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE20_Reg = Register<JPEG_BASE + 0x1E0, ReadWrite>;
    struct _HUFFBASE20 {
        using HuffBase_RAM_0 = Field<HUFFBASE20_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE20_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE21_Reg = Register<JPEG_BASE + 0x1E4, ReadWrite>;
    struct _HUFFBASE21 {
        using HuffBase_RAM_0 = Field<HUFFBASE21_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE21_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE22_Reg = Register<JPEG_BASE + 0x1E8, ReadWrite>;
    struct _HUFFBASE22 {
        using HuffBase_RAM_0 = Field<HUFFBASE22_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE22_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE23_Reg = Register<JPEG_BASE + 0x1EC, ReadWrite>;
    struct _HUFFBASE23 {
        using HuffBase_RAM_0 = Field<HUFFBASE23_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE23_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE24_Reg = Register<JPEG_BASE + 0x1F0, ReadWrite>;
    struct _HUFFBASE24 {
        using HuffBase_RAM_0 = Field<HUFFBASE24_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE24_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE25_Reg = Register<JPEG_BASE + 0x1F4, ReadWrite>;
    struct _HUFFBASE25 {
        using HuffBase_RAM_0 = Field<HUFFBASE25_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE25_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE26_Reg = Register<JPEG_BASE + 0x1F8, ReadWrite>;
    struct _HUFFBASE26 {
        using HuffBase_RAM_0 = Field<HUFFBASE26_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE26_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE27_Reg = Register<JPEG_BASE + 0x1FC, ReadWrite>;
    struct _HUFFBASE27 {
        using HuffBase_RAM_0 = Field<HUFFBASE27_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE27_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE28_Reg = Register<JPEG_BASE + 0x200, ReadWrite>;
    struct _HUFFBASE28 {
        using HuffBase_RAM_0 = Field<HUFFBASE28_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE28_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE29_Reg = Register<JPEG_BASE + 0x204, ReadWrite>;
    struct _HUFFBASE29 {
        using HuffBase_RAM_0 = Field<HUFFBASE29_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE29_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE30_Reg = Register<JPEG_BASE + 0x208, ReadWrite>;
    struct _HUFFBASE30 {
        using HuffBase_RAM_0 = Field<HUFFBASE30_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE30_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HuffSymb tables
    using HUFFBASE31_Reg = Register<JPEG_BASE + 0x20C, ReadWrite>;
    struct _HUFFBASE31 {
        using HuffBase_RAM_0 = Field<HUFFBASE31_Reg, 0, 9>; // HuffBase RAM
        using HuffBase_RAM_1 = Field<HUFFBASE31_Reg, 16, 9>; // HuffBase RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB0_Reg = Register<JPEG_BASE + 0x210, ReadWrite>;
    struct _HUFFSYMB0 {
        using HuffSymb_RAM = Field<HUFFSYMB0_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB1_Reg = Register<JPEG_BASE + 0x214, ReadWrite>;
    struct _HUFFSYMB1 {
        using HuffSymb_RAM = Field<HUFFSYMB1_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB2_Reg = Register<JPEG_BASE + 0x218, ReadWrite>;
    struct _HUFFSYMB2 {
        using HuffSymb_RAM = Field<HUFFSYMB2_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB3_Reg = Register<JPEG_BASE + 0x21C, ReadWrite>;
    struct _HUFFSYMB3 {
        using HuffSymb_RAM = Field<HUFFSYMB3_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB4_Reg = Register<JPEG_BASE + 0x220, ReadWrite>;
    struct _HUFFSYMB4 {
        using HuffSymb_RAM = Field<HUFFSYMB4_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB5_Reg = Register<JPEG_BASE + 0x224, ReadWrite>;
    struct _HUFFSYMB5 {
        using HuffSymb_RAM = Field<HUFFSYMB5_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB6_Reg = Register<JPEG_BASE + 0x228, ReadWrite>;
    struct _HUFFSYMB6 {
        using HuffSymb_RAM = Field<HUFFSYMB6_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB7_Reg = Register<JPEG_BASE + 0x22C, ReadWrite>;
    struct _HUFFSYMB7 {
        using HuffSymb_RAM = Field<HUFFSYMB7_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB8_Reg = Register<JPEG_BASE + 0x230, ReadWrite>;
    struct _HUFFSYMB8 {
        using HuffSymb_RAM = Field<HUFFSYMB8_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB9_Reg = Register<JPEG_BASE + 0x234, ReadWrite>;
    struct _HUFFSYMB9 {
        using HuffSymb_RAM = Field<HUFFSYMB9_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB10_Reg = Register<JPEG_BASE + 0x238, ReadWrite>;
    struct _HUFFSYMB10 {
        using HuffSymb_RAM = Field<HUFFSYMB10_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB11_Reg = Register<JPEG_BASE + 0x23C, ReadWrite>;
    struct _HUFFSYMB11 {
        using HuffSymb_RAM = Field<HUFFSYMB11_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB12_Reg = Register<JPEG_BASE + 0x240, ReadWrite>;
    struct _HUFFSYMB12 {
        using HuffSymb_RAM = Field<HUFFSYMB12_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB13_Reg = Register<JPEG_BASE + 0x244, ReadWrite>;
    struct _HUFFSYMB13 {
        using HuffSymb_RAM = Field<HUFFSYMB13_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB14_Reg = Register<JPEG_BASE + 0x248, ReadWrite>;
    struct _HUFFSYMB14 {
        using HuffSymb_RAM = Field<HUFFSYMB14_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB15_Reg = Register<JPEG_BASE + 0x24C, ReadWrite>;
    struct _HUFFSYMB15 {
        using HuffSymb_RAM = Field<HUFFSYMB15_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB16_Reg = Register<JPEG_BASE + 0x250, ReadWrite>;
    struct _HUFFSYMB16 {
        using HuffSymb_RAM = Field<HUFFSYMB16_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB17_Reg = Register<JPEG_BASE + 0x254, ReadWrite>;
    struct _HUFFSYMB17 {
        using HuffSymb_RAM = Field<HUFFSYMB17_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB18_Reg = Register<JPEG_BASE + 0x258, ReadWrite>;
    struct _HUFFSYMB18 {
        using HuffSymb_RAM = Field<HUFFSYMB18_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB19_Reg = Register<JPEG_BASE + 0x25C, ReadWrite>;
    struct _HUFFSYMB19 {
        using HuffSymb_RAM = Field<HUFFSYMB19_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB20_Reg = Register<JPEG_BASE + 0x260, ReadWrite>;
    struct _HUFFSYMB20 {
        using HuffSymb_RAM = Field<HUFFSYMB20_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB21_Reg = Register<JPEG_BASE + 0x264, ReadWrite>;
    struct _HUFFSYMB21 {
        using HuffSymb_RAM = Field<HUFFSYMB21_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB22_Reg = Register<JPEG_BASE + 0x268, ReadWrite>;
    struct _HUFFSYMB22 {
        using HuffSymb_RAM = Field<HUFFSYMB22_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB23_Reg = Register<JPEG_BASE + 0x26C, ReadWrite>;
    struct _HUFFSYMB23 {
        using HuffSymb_RAM = Field<HUFFSYMB23_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB24_Reg = Register<JPEG_BASE + 0x270, ReadWrite>;
    struct _HUFFSYMB24 {
        using HuffSymb_RAM = Field<HUFFSYMB24_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB25_Reg = Register<JPEG_BASE + 0x274, ReadWrite>;
    struct _HUFFSYMB25 {
        using HuffSymb_RAM = Field<HUFFSYMB25_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB26_Reg = Register<JPEG_BASE + 0x278, ReadWrite>;
    struct _HUFFSYMB26 {
        using HuffSymb_RAM = Field<HUFFSYMB26_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB27_Reg = Register<JPEG_BASE + 0x27C, ReadWrite>;
    struct _HUFFSYMB27 {
        using HuffSymb_RAM = Field<HUFFSYMB27_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB28_Reg = Register<JPEG_BASE + 0x280, ReadWrite>;
    struct _HUFFSYMB28 {
        using HuffSymb_RAM = Field<HUFFSYMB28_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB29_Reg = Register<JPEG_BASE + 0x284, ReadWrite>;
    struct _HUFFSYMB29 {
        using HuffSymb_RAM = Field<HUFFSYMB29_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB30_Reg = Register<JPEG_BASE + 0x288, ReadWrite>;
    struct _HUFFSYMB30 {
        using HuffSymb_RAM = Field<HUFFSYMB30_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB31_Reg = Register<JPEG_BASE + 0x28C, ReadWrite>;
    struct _HUFFSYMB31 {
        using HuffSymb_RAM = Field<HUFFSYMB31_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB32_Reg = Register<JPEG_BASE + 0x290, ReadWrite>;
    struct _HUFFSYMB32 {
        using HuffSymb_RAM = Field<HUFFSYMB32_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB33_Reg = Register<JPEG_BASE + 0x294, ReadWrite>;
    struct _HUFFSYMB33 {
        using HuffSymb_RAM = Field<HUFFSYMB33_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB34_Reg = Register<JPEG_BASE + 0x298, ReadWrite>;
    struct _HUFFSYMB34 {
        using HuffSymb_RAM = Field<HUFFSYMB34_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB35_Reg = Register<JPEG_BASE + 0x29C, ReadWrite>;
    struct _HUFFSYMB35 {
        using HuffSymb_RAM = Field<HUFFSYMB35_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB36_Reg = Register<JPEG_BASE + 0x2A0, ReadWrite>;
    struct _HUFFSYMB36 {
        using HuffSymb_RAM = Field<HUFFSYMB36_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB37_Reg = Register<JPEG_BASE + 0x2A4, ReadWrite>;
    struct _HUFFSYMB37 {
        using HuffSymb_RAM = Field<HUFFSYMB37_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB38_Reg = Register<JPEG_BASE + 0x2A8, ReadWrite>;
    struct _HUFFSYMB38 {
        using HuffSymb_RAM = Field<HUFFSYMB38_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB39_Reg = Register<JPEG_BASE + 0x2AC, ReadWrite>;
    struct _HUFFSYMB39 {
        using HuffSymb_RAM = Field<HUFFSYMB39_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB40_Reg = Register<JPEG_BASE + 0x2B0, ReadWrite>;
    struct _HUFFSYMB40 {
        using HuffSymb_RAM = Field<HUFFSYMB40_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB41_Reg = Register<JPEG_BASE + 0x2B4, ReadWrite>;
    struct _HUFFSYMB41 {
        using HuffSymb_RAM = Field<HUFFSYMB41_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB42_Reg = Register<JPEG_BASE + 0x2B8, ReadWrite>;
    struct _HUFFSYMB42 {
        using HuffSymb_RAM = Field<HUFFSYMB42_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB43_Reg = Register<JPEG_BASE + 0x2BC, ReadWrite>;
    struct _HUFFSYMB43 {
        using HuffSymb_RAM = Field<HUFFSYMB43_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB44_Reg = Register<JPEG_BASE + 0x2C0, ReadWrite>;
    struct _HUFFSYMB44 {
        using HuffSymb_RAM = Field<HUFFSYMB44_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB45_Reg = Register<JPEG_BASE + 0x2C4, ReadWrite>;
    struct _HUFFSYMB45 {
        using HuffSymb_RAM = Field<HUFFSYMB45_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB46_Reg = Register<JPEG_BASE + 0x2C8, ReadWrite>;
    struct _HUFFSYMB46 {
        using HuffSymb_RAM = Field<HUFFSYMB46_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB47_Reg = Register<JPEG_BASE + 0x2CC, ReadWrite>;
    struct _HUFFSYMB47 {
        using HuffSymb_RAM = Field<HUFFSYMB47_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB48_Reg = Register<JPEG_BASE + 0x2D0, ReadWrite>;
    struct _HUFFSYMB48 {
        using HuffSymb_RAM = Field<HUFFSYMB48_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB49_Reg = Register<JPEG_BASE + 0x2D4, ReadWrite>;
    struct _HUFFSYMB49 {
        using HuffSymb_RAM = Field<HUFFSYMB49_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB50_Reg = Register<JPEG_BASE + 0x2D8, ReadWrite>;
    struct _HUFFSYMB50 {
        using HuffSymb_RAM = Field<HUFFSYMB50_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB51_Reg = Register<JPEG_BASE + 0x2DC, ReadWrite>;
    struct _HUFFSYMB51 {
        using HuffSymb_RAM = Field<HUFFSYMB51_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB52_Reg = Register<JPEG_BASE + 0x2E0, ReadWrite>;
    struct _HUFFSYMB52 {
        using HuffSymb_RAM = Field<HUFFSYMB52_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB53_Reg = Register<JPEG_BASE + 0x2E4, ReadWrite>;
    struct _HUFFSYMB53 {
        using HuffSymb_RAM = Field<HUFFSYMB53_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB54_Reg = Register<JPEG_BASE + 0x2E8, ReadWrite>;
    struct _HUFFSYMB54 {
        using HuffSymb_RAM = Field<HUFFSYMB54_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB55_Reg = Register<JPEG_BASE + 0x2EC, ReadWrite>;
    struct _HUFFSYMB55 {
        using HuffSymb_RAM = Field<HUFFSYMB55_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB56_Reg = Register<JPEG_BASE + 0x2F0, ReadWrite>;
    struct _HUFFSYMB56 {
        using HuffSymb_RAM = Field<HUFFSYMB56_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB57_Reg = Register<JPEG_BASE + 0x2F4, ReadWrite>;
    struct _HUFFSYMB57 {
        using HuffSymb_RAM = Field<HUFFSYMB57_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB58_Reg = Register<JPEG_BASE + 0x2F8, ReadWrite>;
    struct _HUFFSYMB58 {
        using HuffSymb_RAM = Field<HUFFSYMB58_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB59_Reg = Register<JPEG_BASE + 0x2FC, ReadWrite>;
    struct _HUFFSYMB59 {
        using HuffSymb_RAM = Field<HUFFSYMB59_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB60_Reg = Register<JPEG_BASE + 0x300, ReadWrite>;
    struct _HUFFSYMB60 {
        using HuffSymb_RAM = Field<HUFFSYMB60_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB61_Reg = Register<JPEG_BASE + 0x304, ReadWrite>;
    struct _HUFFSYMB61 {
        using HuffSymb_RAM = Field<HUFFSYMB61_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB62_Reg = Register<JPEG_BASE + 0x308, ReadWrite>;
    struct _HUFFSYMB62 {
        using HuffSymb_RAM = Field<HUFFSYMB62_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB63_Reg = Register<JPEG_BASE + 0x30C, ReadWrite>;
    struct _HUFFSYMB63 {
        using HuffSymb_RAM = Field<HUFFSYMB63_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB64_Reg = Register<JPEG_BASE + 0x310, ReadWrite>;
    struct _HUFFSYMB64 {
        using HuffSymb_RAM = Field<HUFFSYMB64_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB65_Reg = Register<JPEG_BASE + 0x314, ReadWrite>;
    struct _HUFFSYMB65 {
        using HuffSymb_RAM = Field<HUFFSYMB65_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB66_Reg = Register<JPEG_BASE + 0x318, ReadWrite>;
    struct _HUFFSYMB66 {
        using HuffSymb_RAM = Field<HUFFSYMB66_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB67_Reg = Register<JPEG_BASE + 0x31C, ReadWrite>;
    struct _HUFFSYMB67 {
        using HuffSymb_RAM = Field<HUFFSYMB67_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB68_Reg = Register<JPEG_BASE + 0x320, ReadWrite>;
    struct _HUFFSYMB68 {
        using HuffSymb_RAM = Field<HUFFSYMB68_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB69_Reg = Register<JPEG_BASE + 0x324, ReadWrite>;
    struct _HUFFSYMB69 {
        using HuffSymb_RAM = Field<HUFFSYMB69_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB70_Reg = Register<JPEG_BASE + 0x328, ReadWrite>;
    struct _HUFFSYMB70 {
        using HuffSymb_RAM = Field<HUFFSYMB70_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB71_Reg = Register<JPEG_BASE + 0x32C, ReadWrite>;
    struct _HUFFSYMB71 {
        using HuffSymb_RAM = Field<HUFFSYMB71_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB72_Reg = Register<JPEG_BASE + 0x330, ReadWrite>;
    struct _HUFFSYMB72 {
        using HuffSymb_RAM = Field<HUFFSYMB72_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB73_Reg = Register<JPEG_BASE + 0x334, ReadWrite>;
    struct _HUFFSYMB73 {
        using HuffSymb_RAM = Field<HUFFSYMB73_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB74_Reg = Register<JPEG_BASE + 0x338, ReadWrite>;
    struct _HUFFSYMB74 {
        using HuffSymb_RAM = Field<HUFFSYMB74_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB75_Reg = Register<JPEG_BASE + 0x33C, ReadWrite>;
    struct _HUFFSYMB75 {
        using HuffSymb_RAM = Field<HUFFSYMB75_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB76_Reg = Register<JPEG_BASE + 0x340, ReadWrite>;
    struct _HUFFSYMB76 {
        using HuffSymb_RAM = Field<HUFFSYMB76_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB77_Reg = Register<JPEG_BASE + 0x344, ReadWrite>;
    struct _HUFFSYMB77 {
        using HuffSymb_RAM = Field<HUFFSYMB77_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB78_Reg = Register<JPEG_BASE + 0x348, ReadWrite>;
    struct _HUFFSYMB78 {
        using HuffSymb_RAM = Field<HUFFSYMB78_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB79_Reg = Register<JPEG_BASE + 0x34C, ReadWrite>;
    struct _HUFFSYMB79 {
        using HuffSymb_RAM = Field<HUFFSYMB79_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB80_Reg = Register<JPEG_BASE + 0x350, ReadWrite>;
    struct _HUFFSYMB80 {
        using HuffSymb_RAM = Field<HUFFSYMB80_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB81_Reg = Register<JPEG_BASE + 0x354, ReadWrite>;
    struct _HUFFSYMB81 {
        using HuffSymb_RAM = Field<HUFFSYMB81_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB82_Reg = Register<JPEG_BASE + 0x358, ReadWrite>;
    struct _HUFFSYMB82 {
        using HuffSymb_RAM = Field<HUFFSYMB82_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG HUFFSYMB tables
    using HUFFSYMB83_Reg = Register<JPEG_BASE + 0x35C, ReadWrite>;
    struct _HUFFSYMB83 {
        using HuffSymb_RAM = Field<HUFFSYMB83_Reg, 0, 32>; // DHTSymb RAM
    };

    // JPEG DHTMem tables
    using DHTMEM0_Reg = Register<JPEG_BASE + 0x360, ReadWrite>;
    struct _DHTMEM0 {
        using DHTMem_RAM = Field<DHTMEM0_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM2_Reg = Register<JPEG_BASE + 0x364, ReadWrite>;
    struct _DHTMEM2 {
        using DHTMem_RAM = Field<DHTMEM2_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM3_Reg = Register<JPEG_BASE + 0x368, ReadWrite>;
    struct _DHTMEM3 {
        using DHTMem_RAM = Field<DHTMEM3_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM4_Reg = Register<JPEG_BASE + 0x36C, ReadWrite>;
    struct _DHTMEM4 {
        using DHTMem_RAM = Field<DHTMEM4_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM5_Reg = Register<JPEG_BASE + 0x370, ReadWrite>;
    struct _DHTMEM5 {
        using DHTMem_RAM = Field<DHTMEM5_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM6_Reg = Register<JPEG_BASE + 0x374, ReadWrite>;
    struct _DHTMEM6 {
        using DHTMem_RAM = Field<DHTMEM6_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM7_Reg = Register<JPEG_BASE + 0x378, ReadWrite>;
    struct _DHTMEM7 {
        using DHTMem_RAM = Field<DHTMEM7_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM8_Reg = Register<JPEG_BASE + 0x37C, ReadWrite>;
    struct _DHTMEM8 {
        using DHTMem_RAM = Field<DHTMEM8_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM9_Reg = Register<JPEG_BASE + 0x380, ReadWrite>;
    struct _DHTMEM9 {
        using DHTMem_RAM = Field<DHTMEM9_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM10_Reg = Register<JPEG_BASE + 0x384, ReadWrite>;
    struct _DHTMEM10 {
        using DHTMem_RAM = Field<DHTMEM10_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM11_Reg = Register<JPEG_BASE + 0x388, ReadWrite>;
    struct _DHTMEM11 {
        using DHTMem_RAM = Field<DHTMEM11_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM12_Reg = Register<JPEG_BASE + 0x38C, ReadWrite>;
    struct _DHTMEM12 {
        using DHTMem_RAM = Field<DHTMEM12_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM13_Reg = Register<JPEG_BASE + 0x390, ReadWrite>;
    struct _DHTMEM13 {
        using DHTMem_RAM = Field<DHTMEM13_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM14_Reg = Register<JPEG_BASE + 0x394, ReadWrite>;
    struct _DHTMEM14 {
        using DHTMem_RAM = Field<DHTMEM14_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM15_Reg = Register<JPEG_BASE + 0x398, ReadWrite>;
    struct _DHTMEM15 {
        using DHTMem_RAM = Field<DHTMEM15_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM16_Reg = Register<JPEG_BASE + 0x39C, ReadWrite>;
    struct _DHTMEM16 {
        using DHTMem_RAM = Field<DHTMEM16_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM17_Reg = Register<JPEG_BASE + 0x3A0, ReadWrite>;
    struct _DHTMEM17 {
        using DHTMem_RAM = Field<DHTMEM17_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM18_Reg = Register<JPEG_BASE + 0x3A4, ReadWrite>;
    struct _DHTMEM18 {
        using DHTMem_RAM = Field<DHTMEM18_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM19_Reg = Register<JPEG_BASE + 0x3A8, ReadWrite>;
    struct _DHTMEM19 {
        using DHTMem_RAM = Field<DHTMEM19_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM20_Reg = Register<JPEG_BASE + 0x3AC, ReadWrite>;
    struct _DHTMEM20 {
        using DHTMem_RAM = Field<DHTMEM20_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM21_Reg = Register<JPEG_BASE + 0x3B0, ReadWrite>;
    struct _DHTMEM21 {
        using DHTMem_RAM = Field<DHTMEM21_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM22_Reg = Register<JPEG_BASE + 0x3B4, ReadWrite>;
    struct _DHTMEM22 {
        using DHTMem_RAM = Field<DHTMEM22_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM23_Reg = Register<JPEG_BASE + 0x3B8, ReadWrite>;
    struct _DHTMEM23 {
        using DHTMem_RAM = Field<DHTMEM23_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM24_Reg = Register<JPEG_BASE + 0x3BC, ReadWrite>;
    struct _DHTMEM24 {
        using DHTMem_RAM = Field<DHTMEM24_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM25_Reg = Register<JPEG_BASE + 0x3C0, ReadWrite>;
    struct _DHTMEM25 {
        using DHTMem_RAM = Field<DHTMEM25_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM26_Reg = Register<JPEG_BASE + 0x3C4, ReadWrite>;
    struct _DHTMEM26 {
        using DHTMem_RAM = Field<DHTMEM26_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM27_Reg = Register<JPEG_BASE + 0x3C8, ReadWrite>;
    struct _DHTMEM27 {
        using DHTMem_RAM = Field<DHTMEM27_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM28_Reg = Register<JPEG_BASE + 0x3CC, ReadWrite>;
    struct _DHTMEM28 {
        using DHTMem_RAM = Field<DHTMEM28_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM29_Reg = Register<JPEG_BASE + 0x3D0, ReadWrite>;
    struct _DHTMEM29 {
        using DHTMem_RAM = Field<DHTMEM29_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM30_Reg = Register<JPEG_BASE + 0x3D4, ReadWrite>;
    struct _DHTMEM30 {
        using DHTMem_RAM = Field<DHTMEM30_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM31_Reg = Register<JPEG_BASE + 0x3D8, ReadWrite>;
    struct _DHTMEM31 {
        using DHTMem_RAM = Field<DHTMEM31_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM32_Reg = Register<JPEG_BASE + 0x3DC, ReadWrite>;
    struct _DHTMEM32 {
        using DHTMem_RAM = Field<DHTMEM32_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM33_Reg = Register<JPEG_BASE + 0x3E0, ReadWrite>;
    struct _DHTMEM33 {
        using DHTMem_RAM = Field<DHTMEM33_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM34_Reg = Register<JPEG_BASE + 0x3E4, ReadWrite>;
    struct _DHTMEM34 {
        using DHTMem_RAM = Field<DHTMEM34_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM35_Reg = Register<JPEG_BASE + 0x3E8, ReadWrite>;
    struct _DHTMEM35 {
        using DHTMem_RAM = Field<DHTMEM35_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM36_Reg = Register<JPEG_BASE + 0x3EC, ReadWrite>;
    struct _DHTMEM36 {
        using DHTMem_RAM = Field<DHTMEM36_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM37_Reg = Register<JPEG_BASE + 0x3F0, ReadWrite>;
    struct _DHTMEM37 {
        using DHTMem_RAM = Field<DHTMEM37_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM38_Reg = Register<JPEG_BASE + 0x3F4, ReadWrite>;
    struct _DHTMEM38 {
        using DHTMem_RAM = Field<DHTMEM38_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM39_Reg = Register<JPEG_BASE + 0x3F8, ReadWrite>;
    struct _DHTMEM39 {
        using DHTMem_RAM = Field<DHTMEM39_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM40_Reg = Register<JPEG_BASE + 0x3FC, ReadWrite>;
    struct _DHTMEM40 {
        using DHTMem_RAM = Field<DHTMEM40_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM41_Reg = Register<JPEG_BASE + 0x400, ReadWrite>;
    struct _DHTMEM41 {
        using DHTMem_RAM = Field<DHTMEM41_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM42_Reg = Register<JPEG_BASE + 0x404, ReadWrite>;
    struct _DHTMEM42 {
        using DHTMem_RAM = Field<DHTMEM42_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM43_Reg = Register<JPEG_BASE + 0x408, ReadWrite>;
    struct _DHTMEM43 {
        using DHTMem_RAM = Field<DHTMEM43_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM44_Reg = Register<JPEG_BASE + 0x40C, ReadWrite>;
    struct _DHTMEM44 {
        using DHTMem_RAM = Field<DHTMEM44_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM45_Reg = Register<JPEG_BASE + 0x410, ReadWrite>;
    struct _DHTMEM45 {
        using DHTMem_RAM = Field<DHTMEM45_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM46_Reg = Register<JPEG_BASE + 0x414, ReadWrite>;
    struct _DHTMEM46 {
        using DHTMem_RAM = Field<DHTMEM46_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM47_Reg = Register<JPEG_BASE + 0x418, ReadWrite>;
    struct _DHTMEM47 {
        using DHTMem_RAM = Field<DHTMEM47_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM48_Reg = Register<JPEG_BASE + 0x41C, ReadWrite>;
    struct _DHTMEM48 {
        using DHTMem_RAM = Field<DHTMEM48_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM49_Reg = Register<JPEG_BASE + 0x420, ReadWrite>;
    struct _DHTMEM49 {
        using DHTMem_RAM = Field<DHTMEM49_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM50_Reg = Register<JPEG_BASE + 0x424, ReadWrite>;
    struct _DHTMEM50 {
        using DHTMem_RAM = Field<DHTMEM50_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM51_Reg = Register<JPEG_BASE + 0x428, ReadWrite>;
    struct _DHTMEM51 {
        using DHTMem_RAM = Field<DHTMEM51_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM52_Reg = Register<JPEG_BASE + 0x42C, ReadWrite>;
    struct _DHTMEM52 {
        using DHTMem_RAM = Field<DHTMEM52_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM53_Reg = Register<JPEG_BASE + 0x430, ReadWrite>;
    struct _DHTMEM53 {
        using DHTMem_RAM = Field<DHTMEM53_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM54_Reg = Register<JPEG_BASE + 0x434, ReadWrite>;
    struct _DHTMEM54 {
        using DHTMem_RAM = Field<DHTMEM54_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM55_Reg = Register<JPEG_BASE + 0x438, ReadWrite>;
    struct _DHTMEM55 {
        using DHTMem_RAM = Field<DHTMEM55_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM56_Reg = Register<JPEG_BASE + 0x43C, ReadWrite>;
    struct _DHTMEM56 {
        using DHTMem_RAM = Field<DHTMEM56_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM57_Reg = Register<JPEG_BASE + 0x440, ReadWrite>;
    struct _DHTMEM57 {
        using DHTMem_RAM = Field<DHTMEM57_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM58_Reg = Register<JPEG_BASE + 0x444, ReadWrite>;
    struct _DHTMEM58 {
        using DHTMem_RAM = Field<DHTMEM58_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM59_Reg = Register<JPEG_BASE + 0x448, ReadWrite>;
    struct _DHTMEM59 {
        using DHTMem_RAM = Field<DHTMEM59_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM60_Reg = Register<JPEG_BASE + 0x44C, ReadWrite>;
    struct _DHTMEM60 {
        using DHTMem_RAM = Field<DHTMEM60_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM61_Reg = Register<JPEG_BASE + 0x450, ReadWrite>;
    struct _DHTMEM61 {
        using DHTMem_RAM = Field<DHTMEM61_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM62_Reg = Register<JPEG_BASE + 0x454, ReadWrite>;
    struct _DHTMEM62 {
        using DHTMem_RAM = Field<DHTMEM62_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM63_Reg = Register<JPEG_BASE + 0x458, ReadWrite>;
    struct _DHTMEM63 {
        using DHTMem_RAM = Field<DHTMEM63_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM64_Reg = Register<JPEG_BASE + 0x45C, ReadWrite>;
    struct _DHTMEM64 {
        using DHTMem_RAM = Field<DHTMEM64_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM65_Reg = Register<JPEG_BASE + 0x460, ReadWrite>;
    struct _DHTMEM65 {
        using DHTMem_RAM = Field<DHTMEM65_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM66_Reg = Register<JPEG_BASE + 0x464, ReadWrite>;
    struct _DHTMEM66 {
        using DHTMem_RAM = Field<DHTMEM66_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM67_Reg = Register<JPEG_BASE + 0x468, ReadWrite>;
    struct _DHTMEM67 {
        using DHTMem_RAM = Field<DHTMEM67_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM68_Reg = Register<JPEG_BASE + 0x46C, ReadWrite>;
    struct _DHTMEM68 {
        using DHTMem_RAM = Field<DHTMEM68_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM69_Reg = Register<JPEG_BASE + 0x470, ReadWrite>;
    struct _DHTMEM69 {
        using DHTMem_RAM = Field<DHTMEM69_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM70_Reg = Register<JPEG_BASE + 0x474, ReadWrite>;
    struct _DHTMEM70 {
        using DHTMem_RAM = Field<DHTMEM70_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM71_Reg = Register<JPEG_BASE + 0x478, ReadWrite>;
    struct _DHTMEM71 {
        using DHTMem_RAM = Field<DHTMEM71_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM72_Reg = Register<JPEG_BASE + 0x47C, ReadWrite>;
    struct _DHTMEM72 {
        using DHTMem_RAM = Field<DHTMEM72_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM73_Reg = Register<JPEG_BASE + 0x480, ReadWrite>;
    struct _DHTMEM73 {
        using DHTMem_RAM = Field<DHTMEM73_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM74_Reg = Register<JPEG_BASE + 0x484, ReadWrite>;
    struct _DHTMEM74 {
        using DHTMem_RAM = Field<DHTMEM74_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM75_Reg = Register<JPEG_BASE + 0x488, ReadWrite>;
    struct _DHTMEM75 {
        using DHTMem_RAM = Field<DHTMEM75_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM76_Reg = Register<JPEG_BASE + 0x48C, ReadWrite>;
    struct _DHTMEM76 {
        using DHTMem_RAM = Field<DHTMEM76_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM77_Reg = Register<JPEG_BASE + 0x490, ReadWrite>;
    struct _DHTMEM77 {
        using DHTMem_RAM = Field<DHTMEM77_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM78_Reg = Register<JPEG_BASE + 0x494, ReadWrite>;
    struct _DHTMEM78 {
        using DHTMem_RAM = Field<DHTMEM78_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM79_Reg = Register<JPEG_BASE + 0x498, ReadWrite>;
    struct _DHTMEM79 {
        using DHTMem_RAM = Field<DHTMEM79_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM80_Reg = Register<JPEG_BASE + 0x49C, ReadWrite>;
    struct _DHTMEM80 {
        using DHTMem_RAM = Field<DHTMEM80_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM81_Reg = Register<JPEG_BASE + 0x4A0, ReadWrite>;
    struct _DHTMEM81 {
        using DHTMem_RAM = Field<DHTMEM81_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM82_Reg = Register<JPEG_BASE + 0x4A4, ReadWrite>;
    struct _DHTMEM82 {
        using DHTMem_RAM = Field<DHTMEM82_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM83_Reg = Register<JPEG_BASE + 0x4A8, ReadWrite>;
    struct _DHTMEM83 {
        using DHTMem_RAM = Field<DHTMEM83_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM84_Reg = Register<JPEG_BASE + 0x4AC, ReadWrite>;
    struct _DHTMEM84 {
        using DHTMem_RAM = Field<DHTMEM84_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM85_Reg = Register<JPEG_BASE + 0x4B0, ReadWrite>;
    struct _DHTMEM85 {
        using DHTMem_RAM = Field<DHTMEM85_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM86_Reg = Register<JPEG_BASE + 0x4B4, ReadWrite>;
    struct _DHTMEM86 {
        using DHTMem_RAM = Field<DHTMEM86_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM87_Reg = Register<JPEG_BASE + 0x4B8, ReadWrite>;
    struct _DHTMEM87 {
        using DHTMem_RAM = Field<DHTMEM87_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM88_Reg = Register<JPEG_BASE + 0x4BC, ReadWrite>;
    struct _DHTMEM88 {
        using DHTMem_RAM = Field<DHTMEM88_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM89_Reg = Register<JPEG_BASE + 0x4C0, ReadWrite>;
    struct _DHTMEM89 {
        using DHTMem_RAM = Field<DHTMEM89_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM90_Reg = Register<JPEG_BASE + 0x4C4, ReadWrite>;
    struct _DHTMEM90 {
        using DHTMem_RAM = Field<DHTMEM90_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM91_Reg = Register<JPEG_BASE + 0x4C8, ReadWrite>;
    struct _DHTMEM91 {
        using DHTMem_RAM = Field<DHTMEM91_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM92_Reg = Register<JPEG_BASE + 0x4CC, ReadWrite>;
    struct _DHTMEM92 {
        using DHTMem_RAM = Field<DHTMEM92_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM93_Reg = Register<JPEG_BASE + 0x4D0, ReadWrite>;
    struct _DHTMEM93 {
        using DHTMem_RAM = Field<DHTMEM93_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM94_Reg = Register<JPEG_BASE + 0x4D4, ReadWrite>;
    struct _DHTMEM94 {
        using DHTMem_RAM = Field<DHTMEM94_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM95_Reg = Register<JPEG_BASE + 0x4D8, ReadWrite>;
    struct _DHTMEM95 {
        using DHTMem_RAM = Field<DHTMEM95_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM96_Reg = Register<JPEG_BASE + 0x4DC, ReadWrite>;
    struct _DHTMEM96 {
        using DHTMem_RAM = Field<DHTMEM96_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM97_Reg = Register<JPEG_BASE + 0x4E0, ReadWrite>;
    struct _DHTMEM97 {
        using DHTMem_RAM = Field<DHTMEM97_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM98_Reg = Register<JPEG_BASE + 0x4E4, ReadWrite>;
    struct _DHTMEM98 {
        using DHTMem_RAM = Field<DHTMEM98_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM99_Reg = Register<JPEG_BASE + 0x4E8, ReadWrite>;
    struct _DHTMEM99 {
        using DHTMem_RAM = Field<DHTMEM99_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM100_Reg = Register<JPEG_BASE + 0x4EC, ReadWrite>;
    struct _DHTMEM100 {
        using DHTMem_RAM = Field<DHTMEM100_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM101_Reg = Register<JPEG_BASE + 0x4F0, ReadWrite>;
    struct _DHTMEM101 {
        using DHTMem_RAM = Field<DHTMEM101_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM102_Reg = Register<JPEG_BASE + 0x4F4, ReadWrite>;
    struct _DHTMEM102 {
        using DHTMem_RAM = Field<DHTMEM102_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG DHTMem tables
    using DHTMEM103_Reg = Register<JPEG_BASE + 0x4F8, ReadWrite>;
    struct _DHTMEM103 {
        using DHTMem_RAM = Field<DHTMEM103_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_0_Reg = Register<JPEG_BASE + 0x500, ReadWrite>;
    struct _HUFFENC_AC0_0 {
        using DHTMem_RAM = Field<HUFFENC_AC0_0_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_1_Reg = Register<JPEG_BASE + 0x504, ReadWrite>;
    struct _HUFFENC_AC0_1 {
        using DHTMem_RAM = Field<HUFFENC_AC0_1_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_2_Reg = Register<JPEG_BASE + 0x508, ReadWrite>;
    struct _HUFFENC_AC0_2 {
        using DHTMem_RAM = Field<HUFFENC_AC0_2_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_3_Reg = Register<JPEG_BASE + 0x50C, ReadWrite>;
    struct _HUFFENC_AC0_3 {
        using DHTMem_RAM = Field<HUFFENC_AC0_3_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_4_Reg = Register<JPEG_BASE + 0x510, ReadWrite>;
    struct _HUFFENC_AC0_4 {
        using DHTMem_RAM = Field<HUFFENC_AC0_4_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_5_Reg = Register<JPEG_BASE + 0x514, ReadWrite>;
    struct _HUFFENC_AC0_5 {
        using DHTMem_RAM = Field<HUFFENC_AC0_5_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_6_Reg = Register<JPEG_BASE + 0x518, ReadWrite>;
    struct _HUFFENC_AC0_6 {
        using DHTMem_RAM = Field<HUFFENC_AC0_6_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_7_Reg = Register<JPEG_BASE + 0x51C, ReadWrite>;
    struct _HUFFENC_AC0_7 {
        using DHTMem_RAM = Field<HUFFENC_AC0_7_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_8_Reg = Register<JPEG_BASE + 0x520, ReadWrite>;
    struct _HUFFENC_AC0_8 {
        using DHTMem_RAM = Field<HUFFENC_AC0_8_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_9_Reg = Register<JPEG_BASE + 0x524, ReadWrite>;
    struct _HUFFENC_AC0_9 {
        using DHTMem_RAM = Field<HUFFENC_AC0_9_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_10_Reg = Register<JPEG_BASE + 0x528, ReadWrite>;
    struct _HUFFENC_AC0_10 {
        using DHTMem_RAM = Field<HUFFENC_AC0_10_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_11_Reg = Register<JPEG_BASE + 0x52C, ReadWrite>;
    struct _HUFFENC_AC0_11 {
        using DHTMem_RAM = Field<HUFFENC_AC0_11_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_12_Reg = Register<JPEG_BASE + 0x530, ReadWrite>;
    struct _HUFFENC_AC0_12 {
        using DHTMem_RAM = Field<HUFFENC_AC0_12_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_13_Reg = Register<JPEG_BASE + 0x534, ReadWrite>;
    struct _HUFFENC_AC0_13 {
        using DHTMem_RAM = Field<HUFFENC_AC0_13_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_14_Reg = Register<JPEG_BASE + 0x538, ReadWrite>;
    struct _HUFFENC_AC0_14 {
        using DHTMem_RAM = Field<HUFFENC_AC0_14_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_15_Reg = Register<JPEG_BASE + 0x53C, ReadWrite>;
    struct _HUFFENC_AC0_15 {
        using DHTMem_RAM = Field<HUFFENC_AC0_15_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_16_Reg = Register<JPEG_BASE + 0x540, ReadWrite>;
    struct _HUFFENC_AC0_16 {
        using DHTMem_RAM = Field<HUFFENC_AC0_16_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_17_Reg = Register<JPEG_BASE + 0x544, ReadWrite>;
    struct _HUFFENC_AC0_17 {
        using DHTMem_RAM = Field<HUFFENC_AC0_17_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_18_Reg = Register<JPEG_BASE + 0x548, ReadWrite>;
    struct _HUFFENC_AC0_18 {
        using DHTMem_RAM = Field<HUFFENC_AC0_18_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_19_Reg = Register<JPEG_BASE + 0x54C, ReadWrite>;
    struct _HUFFENC_AC0_19 {
        using DHTMem_RAM = Field<HUFFENC_AC0_19_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_20_Reg = Register<JPEG_BASE + 0x550, ReadWrite>;
    struct _HUFFENC_AC0_20 {
        using DHTMem_RAM = Field<HUFFENC_AC0_20_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_21_Reg = Register<JPEG_BASE + 0x554, ReadWrite>;
    struct _HUFFENC_AC0_21 {
        using DHTMem_RAM = Field<HUFFENC_AC0_21_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_22_Reg = Register<JPEG_BASE + 0x558, ReadWrite>;
    struct _HUFFENC_AC0_22 {
        using DHTMem_RAM = Field<HUFFENC_AC0_22_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_23_Reg = Register<JPEG_BASE + 0x55C, ReadWrite>;
    struct _HUFFENC_AC0_23 {
        using DHTMem_RAM = Field<HUFFENC_AC0_23_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_24_Reg = Register<JPEG_BASE + 0x560, ReadWrite>;
    struct _HUFFENC_AC0_24 {
        using DHTMem_RAM = Field<HUFFENC_AC0_24_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_25_Reg = Register<JPEG_BASE + 0x564, ReadWrite>;
    struct _HUFFENC_AC0_25 {
        using DHTMem_RAM = Field<HUFFENC_AC0_25_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_26_Reg = Register<JPEG_BASE + 0x568, ReadWrite>;
    struct _HUFFENC_AC0_26 {
        using DHTMem_RAM = Field<HUFFENC_AC0_26_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_27_Reg = Register<JPEG_BASE + 0x56C, ReadWrite>;
    struct _HUFFENC_AC0_27 {
        using DHTMem_RAM = Field<HUFFENC_AC0_27_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_28_Reg = Register<JPEG_BASE + 0x570, ReadWrite>;
    struct _HUFFENC_AC0_28 {
        using DHTMem_RAM = Field<HUFFENC_AC0_28_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_29_Reg = Register<JPEG_BASE + 0x574, ReadWrite>;
    struct _HUFFENC_AC0_29 {
        using DHTMem_RAM = Field<HUFFENC_AC0_29_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_30_Reg = Register<JPEG_BASE + 0x578, ReadWrite>;
    struct _HUFFENC_AC0_30 {
        using DHTMem_RAM = Field<HUFFENC_AC0_30_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_31_Reg = Register<JPEG_BASE + 0x57C, ReadWrite>;
    struct _HUFFENC_AC0_31 {
        using DHTMem_RAM = Field<HUFFENC_AC0_31_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_32_Reg = Register<JPEG_BASE + 0x580, ReadWrite>;
    struct _HUFFENC_AC0_32 {
        using DHTMem_RAM = Field<HUFFENC_AC0_32_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_33_Reg = Register<JPEG_BASE + 0x584, ReadWrite>;
    struct _HUFFENC_AC0_33 {
        using DHTMem_RAM = Field<HUFFENC_AC0_33_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_34_Reg = Register<JPEG_BASE + 0x588, ReadWrite>;
    struct _HUFFENC_AC0_34 {
        using DHTMem_RAM = Field<HUFFENC_AC0_34_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_35_Reg = Register<JPEG_BASE + 0x58C, ReadWrite>;
    struct _HUFFENC_AC0_35 {
        using DHTMem_RAM = Field<HUFFENC_AC0_35_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_36_Reg = Register<JPEG_BASE + 0x590, ReadWrite>;
    struct _HUFFENC_AC0_36 {
        using DHTMem_RAM = Field<HUFFENC_AC0_36_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_37_Reg = Register<JPEG_BASE + 0x594, ReadWrite>;
    struct _HUFFENC_AC0_37 {
        using DHTMem_RAM = Field<HUFFENC_AC0_37_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_38_Reg = Register<JPEG_BASE + 0x598, ReadWrite>;
    struct _HUFFENC_AC0_38 {
        using DHTMem_RAM = Field<HUFFENC_AC0_38_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_39_Reg = Register<JPEG_BASE + 0x59C, ReadWrite>;
    struct _HUFFENC_AC0_39 {
        using DHTMem_RAM = Field<HUFFENC_AC0_39_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_40_Reg = Register<JPEG_BASE + 0x5A0, ReadWrite>;
    struct _HUFFENC_AC0_40 {
        using DHTMem_RAM = Field<HUFFENC_AC0_40_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_41_Reg = Register<JPEG_BASE + 0x5A4, ReadWrite>;
    struct _HUFFENC_AC0_41 {
        using DHTMem_RAM = Field<HUFFENC_AC0_41_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_42_Reg = Register<JPEG_BASE + 0x5A8, ReadWrite>;
    struct _HUFFENC_AC0_42 {
        using DHTMem_RAM = Field<HUFFENC_AC0_42_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_43_Reg = Register<JPEG_BASE + 0x5AC, ReadWrite>;
    struct _HUFFENC_AC0_43 {
        using DHTMem_RAM = Field<HUFFENC_AC0_43_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_44_Reg = Register<JPEG_BASE + 0x5B0, ReadWrite>;
    struct _HUFFENC_AC0_44 {
        using DHTMem_RAM = Field<HUFFENC_AC0_44_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_45_Reg = Register<JPEG_BASE + 0x5B4, ReadWrite>;
    struct _HUFFENC_AC0_45 {
        using DHTMem_RAM = Field<HUFFENC_AC0_45_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_46_Reg = Register<JPEG_BASE + 0x5B8, ReadWrite>;
    struct _HUFFENC_AC0_46 {
        using DHTMem_RAM = Field<HUFFENC_AC0_46_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_47_Reg = Register<JPEG_BASE + 0x5BC, ReadWrite>;
    struct _HUFFENC_AC0_47 {
        using DHTMem_RAM = Field<HUFFENC_AC0_47_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_48_Reg = Register<JPEG_BASE + 0x5C0, ReadWrite>;
    struct _HUFFENC_AC0_48 {
        using DHTMem_RAM = Field<HUFFENC_AC0_48_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_49_Reg = Register<JPEG_BASE + 0x5C4, ReadWrite>;
    struct _HUFFENC_AC0_49 {
        using DHTMem_RAM = Field<HUFFENC_AC0_49_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_50_Reg = Register<JPEG_BASE + 0x5C8, ReadWrite>;
    struct _HUFFENC_AC0_50 {
        using DHTMem_RAM = Field<HUFFENC_AC0_50_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_51_Reg = Register<JPEG_BASE + 0x5CC, ReadWrite>;
    struct _HUFFENC_AC0_51 {
        using DHTMem_RAM = Field<HUFFENC_AC0_51_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_52_Reg = Register<JPEG_BASE + 0x5D0, ReadWrite>;
    struct _HUFFENC_AC0_52 {
        using DHTMem_RAM = Field<HUFFENC_AC0_52_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_53_Reg = Register<JPEG_BASE + 0x5D4, ReadWrite>;
    struct _HUFFENC_AC0_53 {
        using DHTMem_RAM = Field<HUFFENC_AC0_53_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_54_Reg = Register<JPEG_BASE + 0x5D8, ReadWrite>;
    struct _HUFFENC_AC0_54 {
        using DHTMem_RAM = Field<HUFFENC_AC0_54_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_55_Reg = Register<JPEG_BASE + 0x5DC, ReadWrite>;
    struct _HUFFENC_AC0_55 {
        using DHTMem_RAM = Field<HUFFENC_AC0_55_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_56_Reg = Register<JPEG_BASE + 0x5E0, ReadWrite>;
    struct _HUFFENC_AC0_56 {
        using DHTMem_RAM = Field<HUFFENC_AC0_56_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_57_Reg = Register<JPEG_BASE + 0x5E4, ReadWrite>;
    struct _HUFFENC_AC0_57 {
        using DHTMem_RAM = Field<HUFFENC_AC0_57_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_58_Reg = Register<JPEG_BASE + 0x5E8, ReadWrite>;
    struct _HUFFENC_AC0_58 {
        using DHTMem_RAM = Field<HUFFENC_AC0_58_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_59_Reg = Register<JPEG_BASE + 0x5EC, ReadWrite>;
    struct _HUFFENC_AC0_59 {
        using DHTMem_RAM = Field<HUFFENC_AC0_59_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_60_Reg = Register<JPEG_BASE + 0x5F0, ReadWrite>;
    struct _HUFFENC_AC0_60 {
        using DHTMem_RAM = Field<HUFFENC_AC0_60_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_61_Reg = Register<JPEG_BASE + 0x5F4, ReadWrite>;
    struct _HUFFENC_AC0_61 {
        using DHTMem_RAM = Field<HUFFENC_AC0_61_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_62_Reg = Register<JPEG_BASE + 0x5F8, ReadWrite>;
    struct _HUFFENC_AC0_62 {
        using DHTMem_RAM = Field<HUFFENC_AC0_62_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_63_Reg = Register<JPEG_BASE + 0x5FC, ReadWrite>;
    struct _HUFFENC_AC0_63 {
        using DHTMem_RAM = Field<HUFFENC_AC0_63_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_64_Reg = Register<JPEG_BASE + 0x600, ReadWrite>;
    struct _HUFFENC_AC0_64 {
        using DHTMem_RAM = Field<HUFFENC_AC0_64_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_65_Reg = Register<JPEG_BASE + 0x604, ReadWrite>;
    struct _HUFFENC_AC0_65 {
        using DHTMem_RAM = Field<HUFFENC_AC0_65_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_66_Reg = Register<JPEG_BASE + 0x608, ReadWrite>;
    struct _HUFFENC_AC0_66 {
        using DHTMem_RAM = Field<HUFFENC_AC0_66_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_67_Reg = Register<JPEG_BASE + 0x60C, ReadWrite>;
    struct _HUFFENC_AC0_67 {
        using DHTMem_RAM = Field<HUFFENC_AC0_67_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_68_Reg = Register<JPEG_BASE + 0x610, ReadWrite>;
    struct _HUFFENC_AC0_68 {
        using DHTMem_RAM = Field<HUFFENC_AC0_68_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_69_Reg = Register<JPEG_BASE + 0x614, ReadWrite>;
    struct _HUFFENC_AC0_69 {
        using DHTMem_RAM = Field<HUFFENC_AC0_69_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_70_Reg = Register<JPEG_BASE + 0x618, ReadWrite>;
    struct _HUFFENC_AC0_70 {
        using DHTMem_RAM = Field<HUFFENC_AC0_70_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_71_Reg = Register<JPEG_BASE + 0x61C, ReadWrite>;
    struct _HUFFENC_AC0_71 {
        using DHTMem_RAM = Field<HUFFENC_AC0_71_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_72_Reg = Register<JPEG_BASE + 0x620, ReadWrite>;
    struct _HUFFENC_AC0_72 {
        using DHTMem_RAM = Field<HUFFENC_AC0_72_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_73_Reg = Register<JPEG_BASE + 0x624, ReadWrite>;
    struct _HUFFENC_AC0_73 {
        using DHTMem_RAM = Field<HUFFENC_AC0_73_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_74_Reg = Register<JPEG_BASE + 0x628, ReadWrite>;
    struct _HUFFENC_AC0_74 {
        using DHTMem_RAM = Field<HUFFENC_AC0_74_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_75_Reg = Register<JPEG_BASE + 0x62C, ReadWrite>;
    struct _HUFFENC_AC0_75 {
        using DHTMem_RAM = Field<HUFFENC_AC0_75_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_76_Reg = Register<JPEG_BASE + 0x630, ReadWrite>;
    struct _HUFFENC_AC0_76 {
        using DHTMem_RAM = Field<HUFFENC_AC0_76_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_77_Reg = Register<JPEG_BASE + 0x634, ReadWrite>;
    struct _HUFFENC_AC0_77 {
        using DHTMem_RAM = Field<HUFFENC_AC0_77_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_78_Reg = Register<JPEG_BASE + 0x638, ReadWrite>;
    struct _HUFFENC_AC0_78 {
        using DHTMem_RAM = Field<HUFFENC_AC0_78_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_79_Reg = Register<JPEG_BASE + 0x63C, ReadWrite>;
    struct _HUFFENC_AC0_79 {
        using DHTMem_RAM = Field<HUFFENC_AC0_79_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_80_Reg = Register<JPEG_BASE + 0x640, ReadWrite>;
    struct _HUFFENC_AC0_80 {
        using DHTMem_RAM = Field<HUFFENC_AC0_80_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_81_Reg = Register<JPEG_BASE + 0x644, ReadWrite>;
    struct _HUFFENC_AC0_81 {
        using DHTMem_RAM = Field<HUFFENC_AC0_81_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_82_Reg = Register<JPEG_BASE + 0x648, ReadWrite>;
    struct _HUFFENC_AC0_82 {
        using DHTMem_RAM = Field<HUFFENC_AC0_82_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_83_Reg = Register<JPEG_BASE + 0x64C, ReadWrite>;
    struct _HUFFENC_AC0_83 {
        using DHTMem_RAM = Field<HUFFENC_AC0_83_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_84_Reg = Register<JPEG_BASE + 0x650, ReadWrite>;
    struct _HUFFENC_AC0_84 {
        using DHTMem_RAM = Field<HUFFENC_AC0_84_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_85_Reg = Register<JPEG_BASE + 0x654, ReadWrite>;
    struct _HUFFENC_AC0_85 {
        using DHTMem_RAM = Field<HUFFENC_AC0_85_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_86_Reg = Register<JPEG_BASE + 0x658, ReadWrite>;
    struct _HUFFENC_AC0_86 {
        using DHTMem_RAM = Field<HUFFENC_AC0_86_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 0
    using HUFFENC_AC0_87_Reg = Register<JPEG_BASE + 0x65C, ReadWrite>;
    struct _HUFFENC_AC0_87 {
        using DHTMem_RAM = Field<HUFFENC_AC0_87_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_0_Reg = Register<JPEG_BASE + 0x660, ReadWrite>;
    struct _HUFFENC_AC1_0 {
        using DHTMem_RAM = Field<HUFFENC_AC1_0_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_1_Reg = Register<JPEG_BASE + 0x664, ReadWrite>;
    struct _HUFFENC_AC1_1 {
        using DHTMem_RAM = Field<HUFFENC_AC1_1_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_2_Reg = Register<JPEG_BASE + 0x668, ReadWrite>;
    struct _HUFFENC_AC1_2 {
        using DHTMem_RAM = Field<HUFFENC_AC1_2_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_3_Reg = Register<JPEG_BASE + 0x66C, ReadWrite>;
    struct _HUFFENC_AC1_3 {
        using DHTMem_RAM = Field<HUFFENC_AC1_3_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_4_Reg = Register<JPEG_BASE + 0x670, ReadWrite>;
    struct _HUFFENC_AC1_4 {
        using DHTMem_RAM = Field<HUFFENC_AC1_4_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_5_Reg = Register<JPEG_BASE + 0x674, ReadWrite>;
    struct _HUFFENC_AC1_5 {
        using DHTMem_RAM = Field<HUFFENC_AC1_5_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_6_Reg = Register<JPEG_BASE + 0x678, ReadWrite>;
    struct _HUFFENC_AC1_6 {
        using DHTMem_RAM = Field<HUFFENC_AC1_6_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_7_Reg = Register<JPEG_BASE + 0x67C, ReadWrite>;
    struct _HUFFENC_AC1_7 {
        using DHTMem_RAM = Field<HUFFENC_AC1_7_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_8_Reg = Register<JPEG_BASE + 0x680, ReadWrite>;
    struct _HUFFENC_AC1_8 {
        using DHTMem_RAM = Field<HUFFENC_AC1_8_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_9_Reg = Register<JPEG_BASE + 0x684, ReadWrite>;
    struct _HUFFENC_AC1_9 {
        using DHTMem_RAM = Field<HUFFENC_AC1_9_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_10_Reg = Register<JPEG_BASE + 0x688, ReadWrite>;
    struct _HUFFENC_AC1_10 {
        using DHTMem_RAM = Field<HUFFENC_AC1_10_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_11_Reg = Register<JPEG_BASE + 0x68C, ReadWrite>;
    struct _HUFFENC_AC1_11 {
        using DHTMem_RAM = Field<HUFFENC_AC1_11_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_12_Reg = Register<JPEG_BASE + 0x690, ReadWrite>;
    struct _HUFFENC_AC1_12 {
        using DHTMem_RAM = Field<HUFFENC_AC1_12_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_13_Reg = Register<JPEG_BASE + 0x694, ReadWrite>;
    struct _HUFFENC_AC1_13 {
        using DHTMem_RAM = Field<HUFFENC_AC1_13_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_14_Reg = Register<JPEG_BASE + 0x698, ReadWrite>;
    struct _HUFFENC_AC1_14 {
        using DHTMem_RAM = Field<HUFFENC_AC1_14_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_15_Reg = Register<JPEG_BASE + 0x69C, ReadWrite>;
    struct _HUFFENC_AC1_15 {
        using DHTMem_RAM = Field<HUFFENC_AC1_15_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_16_Reg = Register<JPEG_BASE + 0x6A0, ReadWrite>;
    struct _HUFFENC_AC1_16 {
        using DHTMem_RAM = Field<HUFFENC_AC1_16_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_17_Reg = Register<JPEG_BASE + 0x6A4, ReadWrite>;
    struct _HUFFENC_AC1_17 {
        using DHTMem_RAM = Field<HUFFENC_AC1_17_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_18_Reg = Register<JPEG_BASE + 0x6A8, ReadWrite>;
    struct _HUFFENC_AC1_18 {
        using DHTMem_RAM = Field<HUFFENC_AC1_18_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_19_Reg = Register<JPEG_BASE + 0x6AC, ReadWrite>;
    struct _HUFFENC_AC1_19 {
        using DHTMem_RAM = Field<HUFFENC_AC1_19_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_20_Reg = Register<JPEG_BASE + 0x6B0, ReadWrite>;
    struct _HUFFENC_AC1_20 {
        using DHTMem_RAM = Field<HUFFENC_AC1_20_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_21_Reg = Register<JPEG_BASE + 0x6B4, ReadWrite>;
    struct _HUFFENC_AC1_21 {
        using DHTMem_RAM = Field<HUFFENC_AC1_21_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_22_Reg = Register<JPEG_BASE + 0x6B8, ReadWrite>;
    struct _HUFFENC_AC1_22 {
        using DHTMem_RAM = Field<HUFFENC_AC1_22_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_23_Reg = Register<JPEG_BASE + 0x6BC, ReadWrite>;
    struct _HUFFENC_AC1_23 {
        using DHTMem_RAM = Field<HUFFENC_AC1_23_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_24_Reg = Register<JPEG_BASE + 0x6C0, ReadWrite>;
    struct _HUFFENC_AC1_24 {
        using DHTMem_RAM = Field<HUFFENC_AC1_24_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_25_Reg = Register<JPEG_BASE + 0x6C4, ReadWrite>;
    struct _HUFFENC_AC1_25 {
        using DHTMem_RAM = Field<HUFFENC_AC1_25_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_26_Reg = Register<JPEG_BASE + 0x6C8, ReadWrite>;
    struct _HUFFENC_AC1_26 {
        using DHTMem_RAM = Field<HUFFENC_AC1_26_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_27_Reg = Register<JPEG_BASE + 0x6CC, ReadWrite>;
    struct _HUFFENC_AC1_27 {
        using DHTMem_RAM = Field<HUFFENC_AC1_27_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_28_Reg = Register<JPEG_BASE + 0x6D0, ReadWrite>;
    struct _HUFFENC_AC1_28 {
        using DHTMem_RAM = Field<HUFFENC_AC1_28_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_29_Reg = Register<JPEG_BASE + 0x6D4, ReadWrite>;
    struct _HUFFENC_AC1_29 {
        using DHTMem_RAM = Field<HUFFENC_AC1_29_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_30_Reg = Register<JPEG_BASE + 0x6D8, ReadWrite>;
    struct _HUFFENC_AC1_30 {
        using DHTMem_RAM = Field<HUFFENC_AC1_30_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_31_Reg = Register<JPEG_BASE + 0x6DC, ReadWrite>;
    struct _HUFFENC_AC1_31 {
        using DHTMem_RAM = Field<HUFFENC_AC1_31_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_32_Reg = Register<JPEG_BASE + 0x6E0, ReadWrite>;
    struct _HUFFENC_AC1_32 {
        using DHTMem_RAM = Field<HUFFENC_AC1_32_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_33_Reg = Register<JPEG_BASE + 0x6E4, ReadWrite>;
    struct _HUFFENC_AC1_33 {
        using DHTMem_RAM = Field<HUFFENC_AC1_33_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_34_Reg = Register<JPEG_BASE + 0x6E8, ReadWrite>;
    struct _HUFFENC_AC1_34 {
        using DHTMem_RAM = Field<HUFFENC_AC1_34_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_35_Reg = Register<JPEG_BASE + 0x6EC, ReadWrite>;
    struct _HUFFENC_AC1_35 {
        using DHTMem_RAM = Field<HUFFENC_AC1_35_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_36_Reg = Register<JPEG_BASE + 0x6F0, ReadWrite>;
    struct _HUFFENC_AC1_36 {
        using DHTMem_RAM = Field<HUFFENC_AC1_36_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_37_Reg = Register<JPEG_BASE + 0x6F4, ReadWrite>;
    struct _HUFFENC_AC1_37 {
        using DHTMem_RAM = Field<HUFFENC_AC1_37_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_38_Reg = Register<JPEG_BASE + 0x6F8, ReadWrite>;
    struct _HUFFENC_AC1_38 {
        using DHTMem_RAM = Field<HUFFENC_AC1_38_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_39_Reg = Register<JPEG_BASE + 0x6FC, ReadWrite>;
    struct _HUFFENC_AC1_39 {
        using DHTMem_RAM = Field<HUFFENC_AC1_39_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_40_Reg = Register<JPEG_BASE + 0x700, ReadWrite>;
    struct _HUFFENC_AC1_40 {
        using DHTMem_RAM = Field<HUFFENC_AC1_40_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_41_Reg = Register<JPEG_BASE + 0x704, ReadWrite>;
    struct _HUFFENC_AC1_41 {
        using DHTMem_RAM = Field<HUFFENC_AC1_41_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_42_Reg = Register<JPEG_BASE + 0x708, ReadWrite>;
    struct _HUFFENC_AC1_42 {
        using DHTMem_RAM = Field<HUFFENC_AC1_42_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_43_Reg = Register<JPEG_BASE + 0x70C, ReadWrite>;
    struct _HUFFENC_AC1_43 {
        using DHTMem_RAM = Field<HUFFENC_AC1_43_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_44_Reg = Register<JPEG_BASE + 0x710, ReadWrite>;
    struct _HUFFENC_AC1_44 {
        using DHTMem_RAM = Field<HUFFENC_AC1_44_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_45_Reg = Register<JPEG_BASE + 0x714, ReadWrite>;
    struct _HUFFENC_AC1_45 {
        using DHTMem_RAM = Field<HUFFENC_AC1_45_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_46_Reg = Register<JPEG_BASE + 0x718, ReadWrite>;
    struct _HUFFENC_AC1_46 {
        using DHTMem_RAM = Field<HUFFENC_AC1_46_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_47_Reg = Register<JPEG_BASE + 0x71C, ReadWrite>;
    struct _HUFFENC_AC1_47 {
        using DHTMem_RAM = Field<HUFFENC_AC1_47_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_48_Reg = Register<JPEG_BASE + 0x720, ReadWrite>;
    struct _HUFFENC_AC1_48 {
        using DHTMem_RAM = Field<HUFFENC_AC1_48_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_49_Reg = Register<JPEG_BASE + 0x724, ReadWrite>;
    struct _HUFFENC_AC1_49 {
        using DHTMem_RAM = Field<HUFFENC_AC1_49_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_50_Reg = Register<JPEG_BASE + 0x728, ReadWrite>;
    struct _HUFFENC_AC1_50 {
        using DHTMem_RAM = Field<HUFFENC_AC1_50_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_51_Reg = Register<JPEG_BASE + 0x72C, ReadWrite>;
    struct _HUFFENC_AC1_51 {
        using DHTMem_RAM = Field<HUFFENC_AC1_51_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_52_Reg = Register<JPEG_BASE + 0x730, ReadWrite>;
    struct _HUFFENC_AC1_52 {
        using DHTMem_RAM = Field<HUFFENC_AC1_52_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_53_Reg = Register<JPEG_BASE + 0x734, ReadWrite>;
    struct _HUFFENC_AC1_53 {
        using DHTMem_RAM = Field<HUFFENC_AC1_53_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_54_Reg = Register<JPEG_BASE + 0x738, ReadWrite>;
    struct _HUFFENC_AC1_54 {
        using DHTMem_RAM = Field<HUFFENC_AC1_54_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_55_Reg = Register<JPEG_BASE + 0x73C, ReadWrite>;
    struct _HUFFENC_AC1_55 {
        using DHTMem_RAM = Field<HUFFENC_AC1_55_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_56_Reg = Register<JPEG_BASE + 0x740, ReadWrite>;
    struct _HUFFENC_AC1_56 {
        using DHTMem_RAM = Field<HUFFENC_AC1_56_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_57_Reg = Register<JPEG_BASE + 0x744, ReadWrite>;
    struct _HUFFENC_AC1_57 {
        using DHTMem_RAM = Field<HUFFENC_AC1_57_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_58_Reg = Register<JPEG_BASE + 0x748, ReadWrite>;
    struct _HUFFENC_AC1_58 {
        using DHTMem_RAM = Field<HUFFENC_AC1_58_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_59_Reg = Register<JPEG_BASE + 0x74C, ReadWrite>;
    struct _HUFFENC_AC1_59 {
        using DHTMem_RAM = Field<HUFFENC_AC1_59_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_60_Reg = Register<JPEG_BASE + 0x750, ReadWrite>;
    struct _HUFFENC_AC1_60 {
        using DHTMem_RAM = Field<HUFFENC_AC1_60_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_61_Reg = Register<JPEG_BASE + 0x754, ReadWrite>;
    struct _HUFFENC_AC1_61 {
        using DHTMem_RAM = Field<HUFFENC_AC1_61_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_62_Reg = Register<JPEG_BASE + 0x758, ReadWrite>;
    struct _HUFFENC_AC1_62 {
        using DHTMem_RAM = Field<HUFFENC_AC1_62_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_63_Reg = Register<JPEG_BASE + 0x75C, ReadWrite>;
    struct _HUFFENC_AC1_63 {
        using DHTMem_RAM = Field<HUFFENC_AC1_63_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_64_Reg = Register<JPEG_BASE + 0x760, ReadWrite>;
    struct _HUFFENC_AC1_64 {
        using DHTMem_RAM = Field<HUFFENC_AC1_64_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_65_Reg = Register<JPEG_BASE + 0x764, ReadWrite>;
    struct _HUFFENC_AC1_65 {
        using DHTMem_RAM = Field<HUFFENC_AC1_65_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_66_Reg = Register<JPEG_BASE + 0x768, ReadWrite>;
    struct _HUFFENC_AC1_66 {
        using DHTMem_RAM = Field<HUFFENC_AC1_66_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_67_Reg = Register<JPEG_BASE + 0x76C, ReadWrite>;
    struct _HUFFENC_AC1_67 {
        using DHTMem_RAM = Field<HUFFENC_AC1_67_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_68_Reg = Register<JPEG_BASE + 0x770, ReadWrite>;
    struct _HUFFENC_AC1_68 {
        using DHTMem_RAM = Field<HUFFENC_AC1_68_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_69_Reg = Register<JPEG_BASE + 0x774, ReadWrite>;
    struct _HUFFENC_AC1_69 {
        using DHTMem_RAM = Field<HUFFENC_AC1_69_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_70_Reg = Register<JPEG_BASE + 0x778, ReadWrite>;
    struct _HUFFENC_AC1_70 {
        using DHTMem_RAM = Field<HUFFENC_AC1_70_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_71_Reg = Register<JPEG_BASE + 0x77C, ReadWrite>;
    struct _HUFFENC_AC1_71 {
        using DHTMem_RAM = Field<HUFFENC_AC1_71_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_72_Reg = Register<JPEG_BASE + 0x780, ReadWrite>;
    struct _HUFFENC_AC1_72 {
        using DHTMem_RAM = Field<HUFFENC_AC1_72_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_73_Reg = Register<JPEG_BASE + 0x784, ReadWrite>;
    struct _HUFFENC_AC1_73 {
        using DHTMem_RAM = Field<HUFFENC_AC1_73_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_74_Reg = Register<JPEG_BASE + 0x788, ReadWrite>;
    struct _HUFFENC_AC1_74 {
        using DHTMem_RAM = Field<HUFFENC_AC1_74_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_75_Reg = Register<JPEG_BASE + 0x78C, ReadWrite>;
    struct _HUFFENC_AC1_75 {
        using DHTMem_RAM = Field<HUFFENC_AC1_75_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_76_Reg = Register<JPEG_BASE + 0x790, ReadWrite>;
    struct _HUFFENC_AC1_76 {
        using DHTMem_RAM = Field<HUFFENC_AC1_76_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_77_Reg = Register<JPEG_BASE + 0x794, ReadWrite>;
    struct _HUFFENC_AC1_77 {
        using DHTMem_RAM = Field<HUFFENC_AC1_77_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_78_Reg = Register<JPEG_BASE + 0x798, ReadWrite>;
    struct _HUFFENC_AC1_78 {
        using DHTMem_RAM = Field<HUFFENC_AC1_78_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_79_Reg = Register<JPEG_BASE + 0x79C, ReadWrite>;
    struct _HUFFENC_AC1_79 {
        using DHTMem_RAM = Field<HUFFENC_AC1_79_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_80_Reg = Register<JPEG_BASE + 0x7A0, ReadWrite>;
    struct _HUFFENC_AC1_80 {
        using DHTMem_RAM = Field<HUFFENC_AC1_80_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_81_Reg = Register<JPEG_BASE + 0x7A4, ReadWrite>;
    struct _HUFFENC_AC1_81 {
        using DHTMem_RAM = Field<HUFFENC_AC1_81_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_82_Reg = Register<JPEG_BASE + 0x7A8, ReadWrite>;
    struct _HUFFENC_AC1_82 {
        using DHTMem_RAM = Field<HUFFENC_AC1_82_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_83_Reg = Register<JPEG_BASE + 0x7AC, ReadWrite>;
    struct _HUFFENC_AC1_83 {
        using DHTMem_RAM = Field<HUFFENC_AC1_83_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_84_Reg = Register<JPEG_BASE + 0x7B0, ReadWrite>;
    struct _HUFFENC_AC1_84 {
        using DHTMem_RAM = Field<HUFFENC_AC1_84_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_85_Reg = Register<JPEG_BASE + 0x7B4, ReadWrite>;
    struct _HUFFENC_AC1_85 {
        using DHTMem_RAM = Field<HUFFENC_AC1_85_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_86_Reg = Register<JPEG_BASE + 0x7B8, ReadWrite>;
    struct _HUFFENC_AC1_86 {
        using DHTMem_RAM = Field<HUFFENC_AC1_86_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, AC Huffman table 1
    using HUFFENC_AC1_87_Reg = Register<JPEG_BASE + 0x7BC, ReadWrite>;
    struct _HUFFENC_AC1_87 {
        using DHTMem_RAM = Field<HUFFENC_AC1_87_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, DC Huffman table 0
    using HUFFENC_DC0_0_Reg = Register<JPEG_BASE + 0x7C0, ReadWrite>;
    struct _HUFFENC_DC0_0 {
        using DHTMem_RAM = Field<HUFFENC_DC0_0_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, DC Huffman table 0
    using HUFFENC_DC0_1_Reg = Register<JPEG_BASE + 0x7C4, ReadWrite>;
    struct _HUFFENC_DC0_1 {
        using DHTMem_RAM = Field<HUFFENC_DC0_1_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, DC Huffman table 0
    using HUFFENC_DC0_2_Reg = Register<JPEG_BASE + 0x7C8, ReadWrite>;
    struct _HUFFENC_DC0_2 {
        using DHTMem_RAM = Field<HUFFENC_DC0_2_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, DC Huffman table 0
    using HUFFENC_DC0_3_Reg = Register<JPEG_BASE + 0x7CC, ReadWrite>;
    struct _HUFFENC_DC0_3 {
        using DHTMem_RAM = Field<HUFFENC_DC0_3_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, DC Huffman table 0
    using HUFFENC_DC0_4_Reg = Register<JPEG_BASE + 0x7D0, ReadWrite>;
    struct _HUFFENC_DC0_4 {
        using DHTMem_RAM = Field<HUFFENC_DC0_4_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, DC Huffman table 0
    using HUFFENC_DC0_5_Reg = Register<JPEG_BASE + 0x7D4, ReadWrite>;
    struct _HUFFENC_DC0_5 {
        using DHTMem_RAM = Field<HUFFENC_DC0_5_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, DC Huffman table 0
    using HUFFENC_DC0_6_Reg = Register<JPEG_BASE + 0x7D8, ReadWrite>;
    struct _HUFFENC_DC0_6 {
        using DHTMem_RAM = Field<HUFFENC_DC0_6_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, DC Huffman table 0
    using HUFFENC_DC0_7_Reg = Register<JPEG_BASE + 0x7DC, ReadWrite>;
    struct _HUFFENC_DC0_7 {
        using DHTMem_RAM = Field<HUFFENC_DC0_7_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, DC Huffman table 1
    using HUFFENC_DC1_0_Reg = Register<JPEG_BASE + 0x7E0, ReadWrite>;
    struct _HUFFENC_DC1_0 {
        using DHTMem_RAM = Field<HUFFENC_DC1_0_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, DC Huffman table 1
    using HUFFENC_DC1_1_Reg = Register<JPEG_BASE + 0x7E4, ReadWrite>;
    struct _HUFFENC_DC1_1 {
        using DHTMem_RAM = Field<HUFFENC_DC1_1_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, DC Huffman table 1
    using HUFFENC_DC1_2_Reg = Register<JPEG_BASE + 0x7E8, ReadWrite>;
    struct _HUFFENC_DC1_2 {
        using DHTMem_RAM = Field<HUFFENC_DC1_2_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, DC Huffman table 1
    using HUFFENC_DC1_3_Reg = Register<JPEG_BASE + 0x7EC, ReadWrite>;
    struct _HUFFENC_DC1_3 {
        using DHTMem_RAM = Field<HUFFENC_DC1_3_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, DC Huffman table 1
    using HUFFENC_DC1_4_Reg = Register<JPEG_BASE + 0x7F0, ReadWrite>;
    struct _HUFFENC_DC1_4 {
        using DHTMem_RAM = Field<HUFFENC_DC1_4_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, DC Huffman table 1
    using HUFFENC_DC1_5_Reg = Register<JPEG_BASE + 0x7F4, ReadWrite>;
    struct _HUFFENC_DC1_5 {
        using DHTMem_RAM = Field<HUFFENC_DC1_5_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, DC Huffman table 1
    using HUFFENC_DC1_6_Reg = Register<JPEG_BASE + 0x7F8, ReadWrite>;
    struct _HUFFENC_DC1_6 {
        using DHTMem_RAM = Field<HUFFENC_DC1_6_Reg, 0, 32>; // DHTMem RAM
    };

    // JPEG encoder, DC Huffman table 1
    using HUFFENC_DC1_7_Reg = Register<JPEG_BASE + 0x7FC, ReadWrite>;
    struct _HUFFENC_DC1_7 {
        using DHTMem_RAM = Field<HUFFENC_DC1_7_Reg, 0, 32>; // DHTMem RAM
    };

} // namespace JPEG

// --------------------------------------------
// Ethernet_MMC: Ethernet: MAC management counters
// Base address: 0x40028100
// --------------------------------------------

namespace Ethernet_MMC {
    static constexpr uint32_t Ethernet_MMC_BASE = 0x40028100;

    // Ethernet MMC control register
    using MMCCR_Reg = Register<Ethernet_MMC_BASE + 0x0, ReadWrite>;
    struct _MMCCR {
        using CR = Field<MMCCR_Reg, 0, 1>; // CR
        using CSR = Field<MMCCR_Reg, 1, 1>; // CSR
        using ROR = Field<MMCCR_Reg, 2, 1>; // ROR
        using MCF = Field<MMCCR_Reg, 3, 1>; // MCF
        using MCP = Field<MMCCR_Reg, 4, 1>; // MCP
        using MCFHP = Field<MMCCR_Reg, 5, 1>; // MCFHP
    };

    // Ethernet MMC receive interrupt register
    using MMCRIR_Reg = Register<Ethernet_MMC_BASE + 0x4, ReadWrite>;
    struct _MMCRIR {
        using RFCES = Field<MMCRIR_Reg, 5, 1>; // RFCES
        using RFAES = Field<MMCRIR_Reg, 6, 1>; // RFAES
        using RGUFS = Field<MMCRIR_Reg, 17, 1>; // RGUFS
    };

    // Ethernet MMC transmit interrupt register
    using MMCTIR_Reg = Register<Ethernet_MMC_BASE + 0x8, ReadOnly>;
    struct _MMCTIR {
        using TGFSCS = Field<MMCTIR_Reg, 14, 1>; // TGFSCS
        using TGFMSCS = Field<MMCTIR_Reg, 15, 1>; // TGFMSCS
        using TGFS = Field<MMCTIR_Reg, 21, 1>; // TGFS
    };

    // Ethernet MMC receive interrupt mask register
    using MMCRIMR_Reg = Register<Ethernet_MMC_BASE + 0xC, ReadWrite>;
    struct _MMCRIMR {
        using RFCEM = Field<MMCRIMR_Reg, 5, 1>; // RFCEM
        using RFAEM = Field<MMCRIMR_Reg, 6, 1>; // RFAEM
        using RGUFM = Field<MMCRIMR_Reg, 17, 1>; // RGUFM
    };

    // Ethernet MMC transmit interrupt mask register
    using MMCTIMR_Reg = Register<Ethernet_MMC_BASE + 0x10, ReadWrite>;
    struct _MMCTIMR {
        using TGFSCM = Field<MMCTIMR_Reg, 14, 1>; // TGFSCM
        using TGFMSCM = Field<MMCTIMR_Reg, 15, 1>; // TGFMSCM
        using TGFM = Field<MMCTIMR_Reg, 16, 1>; // TGFM
    };

    // Ethernet MMC transmitted good frames after a single collision counter
    using MMCTGFSCCR_Reg = Register<Ethernet_MMC_BASE + 0x4C, ReadOnly>;
    struct _MMCTGFSCCR {
        using TGFSCC = Field<MMCTGFSCCR_Reg, 0, 32>; // TGFSCC
    };

    // Ethernet MMC transmitted good frames after more than a single collision
    using MMCTGFMSCCR_Reg = Register<Ethernet_MMC_BASE + 0x50, ReadOnly>;
    struct _MMCTGFMSCCR {
        using TGFMSCC = Field<MMCTGFMSCCR_Reg, 0, 32>; // TGFMSCC
    };

    // Ethernet MMC transmitted good frames counter register
    using MMCTGFCR_Reg = Register<Ethernet_MMC_BASE + 0x68, ReadOnly>;
    struct _MMCTGFCR {
        using TGFC = Field<MMCTGFCR_Reg, 0, 32>; // HTL
    };

    // Ethernet MMC received frames with CRC error counter register
    using MMCRFCECR_Reg = Register<Ethernet_MMC_BASE + 0x94, ReadOnly>;
    struct _MMCRFCECR {
        using RFCFC = Field<MMCRFCECR_Reg, 0, 32>; // RFCFC
    };

    // Ethernet MMC received frames with alignment error counter register
    using MMCRFAECR_Reg = Register<Ethernet_MMC_BASE + 0x98, ReadOnly>;
    struct _MMCRFAECR {
        using RFAEC = Field<MMCRFAECR_Reg, 0, 32>; // RFAEC
    };

    // MMC received good unicast frames counter register
    using MMCRGUFCR_Reg = Register<Ethernet_MMC_BASE + 0xC4, ReadOnly>;
    struct _MMCRGUFCR {
        using RGUFC = Field<MMCRGUFCR_Reg, 0, 32>; // RGUFC
    };

} // namespace Ethernet_MMC

// --------------------------------------------
// Ethernet_PTP: Ethernet: Precision time protocol
// Base address: 0x40028700
// --------------------------------------------

namespace Ethernet_PTP {
    static constexpr uint32_t Ethernet_PTP_BASE = 0x40028700;

    // Ethernet PTP time stamp control register
    using PTPTSCR_Reg = Register<Ethernet_PTP_BASE + 0x0, ReadWrite>;
    struct _PTPTSCR {
        using TSE = Field<PTPTSCR_Reg, 0, 1>; // TSE
        using TSFCU = Field<PTPTSCR_Reg, 1, 1>; // TSFCU
        using TSPTPPSV2E = Field<PTPTSCR_Reg, 10, 1>; // TSPTPPSV2E
        using TSSPTPOEFE = Field<PTPTSCR_Reg, 11, 1>; // TSSPTPOEFE
        using TSSIPV6FE = Field<PTPTSCR_Reg, 12, 1>; // TSSIPV6FE
        using TSSIPV4FE = Field<PTPTSCR_Reg, 13, 1>; // TSSIPV4FE
        using TSSEME = Field<PTPTSCR_Reg, 14, 1>; // TSSEME
        using TSSMRME = Field<PTPTSCR_Reg, 15, 1>; // TSSMRME
        using TSCNT = Field<PTPTSCR_Reg, 16, 2>; // TSCNT
        using TSPFFMAE = Field<PTPTSCR_Reg, 18, 1>; // TSPFFMAE
        using TSSTI = Field<PTPTSCR_Reg, 2, 1>; // TSSTI
        using TSSTU = Field<PTPTSCR_Reg, 3, 1>; // TSSTU
        using TSITE = Field<PTPTSCR_Reg, 4, 1>; // TSITE
        using TTSARU = Field<PTPTSCR_Reg, 5, 1>; // TTSARU
        using TSSARFE = Field<PTPTSCR_Reg, 8, 1>; // TSSARFE
        using TSSSR = Field<PTPTSCR_Reg, 9, 1>; // TSSSR
    };

    // Ethernet PTP subsecond increment register
    using PTPSSIR_Reg = Register<Ethernet_PTP_BASE + 0x4, ReadWrite>;
    struct _PTPSSIR {
        using STSSI = Field<PTPSSIR_Reg, 0, 8>; // STSSI
    };

    // Ethernet PTP time stamp high register
    using PTPTSHR_Reg = Register<Ethernet_PTP_BASE + 0x8, ReadOnly>;
    struct _PTPTSHR {
        using STS = Field<PTPTSHR_Reg, 0, 32>; // STS
    };

    // Ethernet PTP time stamp low register
    using PTPTSLR_Reg = Register<Ethernet_PTP_BASE + 0xC, ReadOnly>;
    struct _PTPTSLR {
        using STSS = Field<PTPTSLR_Reg, 0, 31>; // STSS
        using STPNS = Field<PTPTSLR_Reg, 31, 1>; // STPNS
    };

    // Ethernet PTP time stamp high update register
    using PTPTSHUR_Reg = Register<Ethernet_PTP_BASE + 0x10, ReadWrite>;
    struct _PTPTSHUR {
        using TSUS = Field<PTPTSHUR_Reg, 0, 32>; // TSUS
    };

    // Ethernet PTP time stamp low update register
    using PTPTSLUR_Reg = Register<Ethernet_PTP_BASE + 0x14, ReadWrite>;
    struct _PTPTSLUR {
        using TSUSS = Field<PTPTSLUR_Reg, 0, 31>; // TSUSS
        using TSUPNS = Field<PTPTSLUR_Reg, 31, 1>; // TSUPNS
    };

    // Ethernet PTP time stamp addend register
    using PTPTSAR_Reg = Register<Ethernet_PTP_BASE + 0x18, ReadWrite>;
    struct _PTPTSAR {
        using TSA = Field<PTPTSAR_Reg, 0, 32>; // TSA
    };

    // Ethernet PTP target time high register
    using PTPTTHR_Reg = Register<Ethernet_PTP_BASE + 0x1C, ReadWrite>;
    struct _PTPTTHR {
        using TTSH = Field<PTPTTHR_Reg, 0, 32>; // 0
    };

    // Ethernet PTP target time low register
    using PTPTTLR_Reg = Register<Ethernet_PTP_BASE + 0x20, ReadWrite>;
    struct _PTPTTLR {
        using TTSL = Field<PTPTTLR_Reg, 0, 32>; // TTSL
    };

    // Ethernet PTP time stamp status register
    using PTPTSSR_Reg = Register<Ethernet_PTP_BASE + 0x28, ReadOnly>;
    struct _PTPTSSR {
        using TSSO = Field<PTPTSSR_Reg, 0, 1>; // TSSO
        using TSTTR = Field<PTPTSSR_Reg, 1, 1>; // TSTTR
    };

    // Ethernet PTP PPS control register
    using PTPPPSCR_Reg = Register<Ethernet_PTP_BASE + 0x2C, ReadOnly>;
    struct _PTPPPSCR {
        using TSSO = Field<PTPPPSCR_Reg, 0, 1>; // TSSO
        using TSTTR = Field<PTPPPSCR_Reg, 1, 1>; // TSTTR
    };

} // namespace Ethernet_PTP

// --------------------------------------------
// Ethernet_DMA: Ethernet: DMA controller operation
// Base address: 0x40029000
// --------------------------------------------

namespace Ethernet_DMA {
    static constexpr uint32_t Ethernet_DMA_BASE = 0x40029000;

    // Ethernet DMA bus mode register
    using DMABMR_Reg = Register<Ethernet_DMA_BASE + 0x0, ReadWrite>;
    struct _DMABMR {
        using SR = Field<DMABMR_Reg, 0, 1>; // SR
        using DA = Field<DMABMR_Reg, 1, 1>; // DA
        using DSL = Field<DMABMR_Reg, 2, 5>; // DSL
        using EDFE = Field<DMABMR_Reg, 7, 1>; // EDFE
        using PBL = Field<DMABMR_Reg, 8, 6>; // PBL
        using RTPR = Field<DMABMR_Reg, 14, 2>; // RTPR
        using FB = Field<DMABMR_Reg, 16, 1>; // FB
        using RDP = Field<DMABMR_Reg, 17, 6>; // RDP
        using USP = Field<DMABMR_Reg, 23, 1>; // USP
        using FPM = Field<DMABMR_Reg, 24, 1>; // FPM
        using AAB = Field<DMABMR_Reg, 25, 1>; // AAB
        using MB = Field<DMABMR_Reg, 26, 1>; // MB
    };

    // Ethernet DMA transmit poll demand register
    using DMATPDR_Reg = Register<Ethernet_DMA_BASE + 0x4, ReadWrite>;
    struct _DMATPDR {
        using TPD = Field<DMATPDR_Reg, 0, 32>; // TPD
    };

    // EHERNET DMA receive poll demand register
    using DMARPDR_Reg = Register<Ethernet_DMA_BASE + 0x8, ReadWrite>;
    struct _DMARPDR {
        using RPD = Field<DMARPDR_Reg, 0, 32>; // RPD
    };

    // Ethernet DMA receive descriptor list address register
    using DMARDLAR_Reg = Register<Ethernet_DMA_BASE + 0xC, ReadWrite>;
    struct _DMARDLAR {
        using SRL = Field<DMARDLAR_Reg, 0, 32>; // SRL
    };

    // Ethernet DMA transmit descriptor list address register
    using DMATDLAR_Reg = Register<Ethernet_DMA_BASE + 0x10, ReadWrite>;
    struct _DMATDLAR {
        using STL = Field<DMATDLAR_Reg, 0, 32>; // STL
    };

    // Ethernet DMA status register
    using DMASR_Reg = Register<Ethernet_DMA_BASE + 0x14, ReadWrite>;
    struct _DMASR {
        using TS = Field<DMASR_Reg, 0, 1>; // TS
        using TPSS = Field<DMASR_Reg, 1, 1>; // TPSS
        using TBUS = Field<DMASR_Reg, 2, 1>; // TBUS
        using TJTS = Field<DMASR_Reg, 3, 1>; // TJTS
        using ROS = Field<DMASR_Reg, 4, 1>; // ROS
        using TUS = Field<DMASR_Reg, 5, 1>; // TUS
        using RS = Field<DMASR_Reg, 6, 1>; // RS
        using RBUS = Field<DMASR_Reg, 7, 1>; // RBUS
        using RPSS = Field<DMASR_Reg, 8, 1>; // RPSS
        using PWTS = Field<DMASR_Reg, 9, 1>; // PWTS
        using ETS = Field<DMASR_Reg, 10, 1>; // ETS
        using FBES = Field<DMASR_Reg, 13, 1>; // FBES
        using ERS = Field<DMASR_Reg, 14, 1>; // ERS
        using AIS = Field<DMASR_Reg, 15, 1>; // AIS
        using NIS = Field<DMASR_Reg, 16, 1>; // NIS
        using RPS = Field<DMASR_Reg, 17, 3>; // RPS
        using TPS = Field<DMASR_Reg, 20, 3>; // TPS
        using EBS = Field<DMASR_Reg, 23, 3>; // EBS
        using MMCS = Field<DMASR_Reg, 27, 1>; // MMCS
        using PMTS = Field<DMASR_Reg, 28, 1>; // PMTS
        using TSTS = Field<DMASR_Reg, 29, 1>; // TSTS
    };

    // Ethernet DMA operation mode register
    using DMAOMR_Reg = Register<Ethernet_DMA_BASE + 0x18, ReadWrite>;
    struct _DMAOMR {
        using SR = Field<DMAOMR_Reg, 1, 1>; // SR
        using OSF = Field<DMAOMR_Reg, 2, 1>; // OSF
        using RTC = Field<DMAOMR_Reg, 3, 2>; // RTC
        using FUGF = Field<DMAOMR_Reg, 6, 1>; // FUGF
        using FEF = Field<DMAOMR_Reg, 7, 1>; // FEF
        using ST = Field<DMAOMR_Reg, 13, 1>; // ST
        using TTC = Field<DMAOMR_Reg, 14, 3>; // TTC
        using FTF = Field<DMAOMR_Reg, 20, 1>; // FTF
        using TSF = Field<DMAOMR_Reg, 21, 1>; // TSF
        using DFRF = Field<DMAOMR_Reg, 24, 1>; // DFRF
        using RSF = Field<DMAOMR_Reg, 25, 1>; // RSF
        using DTCEFD = Field<DMAOMR_Reg, 26, 1>; // DTCEFD
    };

    // Ethernet DMA interrupt enable register
    using DMAIER_Reg = Register<Ethernet_DMA_BASE + 0x1C, ReadWrite>;
    struct _DMAIER {
        using TIE = Field<DMAIER_Reg, 0, 1>; // TIE
        using TPSIE = Field<DMAIER_Reg, 1, 1>; // TPSIE
        using TBUIE = Field<DMAIER_Reg, 2, 1>; // TBUIE
        using TJTIE = Field<DMAIER_Reg, 3, 1>; // TJTIE
        using ROIE = Field<DMAIER_Reg, 4, 1>; // ROIE
        using TUIE = Field<DMAIER_Reg, 5, 1>; // TUIE
        using RIE = Field<DMAIER_Reg, 6, 1>; // RIE
        using RBUIE = Field<DMAIER_Reg, 7, 1>; // RBUIE
        using RPSIE = Field<DMAIER_Reg, 8, 1>; // RPSIE
        using RWTIE = Field<DMAIER_Reg, 9, 1>; // RWTIE
        using ETIE = Field<DMAIER_Reg, 10, 1>; // ETIE
        using FBEIE = Field<DMAIER_Reg, 13, 1>; // FBEIE
        using ERIE = Field<DMAIER_Reg, 14, 1>; // ERIE
        using AISE = Field<DMAIER_Reg, 15, 1>; // AISE
        using NISE = Field<DMAIER_Reg, 16, 1>; // NISE
    };

    // Ethernet DMA missed frame and buffer overflow counter register
    using DMAMFBOCR_Reg = Register<Ethernet_DMA_BASE + 0x20, ReadWrite>;
    struct _DMAMFBOCR {
        using MFC = Field<DMAMFBOCR_Reg, 0, 16>; // MFC
        using OMFC = Field<DMAMFBOCR_Reg, 16, 1>; // OMFC
        using MFA = Field<DMAMFBOCR_Reg, 17, 11>; // MFA
        using OFOC = Field<DMAMFBOCR_Reg, 28, 1>; // OFOC
    };

    // Ethernet DMA receive status watchdog timer register
    using DMARSWTR_Reg = Register<Ethernet_DMA_BASE + 0x24, ReadWrite>;
    struct _DMARSWTR {
        using RSWTC = Field<DMARSWTR_Reg, 0, 8>; // RSWTC
    };

    // Ethernet DMA current host transmit descriptor register
    using DMACHTDR_Reg = Register<Ethernet_DMA_BASE + 0x48, ReadOnly>;
    struct _DMACHTDR {
        using HTDAP = Field<DMACHTDR_Reg, 0, 32>; // HTDAP
    };

    // Ethernet DMA current host receive descriptor register
    using DMACHRDR_Reg = Register<Ethernet_DMA_BASE + 0x4C, ReadOnly>;
    struct _DMACHRDR {
        using HRDAP = Field<DMACHRDR_Reg, 0, 32>; // HRDAP
    };

    // Ethernet DMA current host transmit buffer address register
    using DMACHTBAR_Reg = Register<Ethernet_DMA_BASE + 0x50, ReadOnly>;
    struct _DMACHTBAR {
        using HTBAP = Field<DMACHTBAR_Reg, 0, 32>; // HTBAP
    };

    // Ethernet DMA current host receive buffer address register
    using DMACHRBAR_Reg = Register<Ethernet_DMA_BASE + 0x54, ReadOnly>;
    struct _DMACHRBAR {
        using HRBAP = Field<DMACHRBAR_Reg, 0, 32>; // HRBAP
    };

} // namespace Ethernet_DMA

// --------------------------------------------
// OTG_FS_HOST: USB on the go full speed
// Base address: 0x50000400
// --------------------------------------------

namespace OTG_FS_HOST {
    static constexpr uint32_t OTG_FS_HOST_BASE = 0x50000400;

    // OTG_FS host configuration register (OTG_FS_HCFG)
    using OTG_FS_HCFG_Reg = Register<OTG_FS_HOST_BASE + 0x0, ReadWrite>;
    struct _OTG_FS_HCFG {
        using FSLSPCS = Field<OTG_FS_HCFG_Reg, 0, 2>; // FS/LS PHY clock select
        using FSLSS = Field<OTG_FS_HCFG_Reg, 2, 1>; // FS- and LS-only support
    };

    // OTG_FS Host frame interval register
    using OTG_FS_HFIR_Reg = Register<OTG_FS_HOST_BASE + 0x4, ReadWrite>;
    struct _OTG_FS_HFIR {
        using FRIVL = Field<OTG_FS_HFIR_Reg, 0, 16>; // Frame interval
    };

    // OTG_FS host frame number/frame time remaining register (OTG_FS_HFNUM)
    using OTG_FS_HFNUM_Reg = Register<OTG_FS_HOST_BASE + 0x8, ReadOnly>;
    struct _OTG_FS_HFNUM {
        using FRNUM = Field<OTG_FS_HFNUM_Reg, 0, 16>; // Frame number
        using FTREM = Field<OTG_FS_HFNUM_Reg, 16, 16>; // Frame time remaining
    };

    // OTG_FS_Host periodic transmit FIFO/queue status register (OTG_FS_HPTXSTS)
    using OTG_FS_HPTXSTS_Reg = Register<OTG_FS_HOST_BASE + 0x10, ReadWrite>;
    struct _OTG_FS_HPTXSTS {
        using PTXFSAVL = Field<OTG_FS_HPTXSTS_Reg, 0, 16>; // Periodic transmit data FIFO space available
        using PTXQSAV = Field<OTG_FS_HPTXSTS_Reg, 16, 8>; // Periodic transmit request queue space available
        using PTXQTOP = Field<OTG_FS_HPTXSTS_Reg, 24, 8>; // Top of the periodic transmit request queue
    };

    // OTG_FS Host all channels interrupt register
    using OTG_FS_HAINT_Reg = Register<OTG_FS_HOST_BASE + 0x14, ReadOnly>;
    struct _OTG_FS_HAINT {
        using HAINT = Field<OTG_FS_HAINT_Reg, 0, 16>; // Channel interrupts
    };

    // OTG_FS host all channels interrupt mask register
    using OTG_FS_HAINTMSK_Reg = Register<OTG_FS_HOST_BASE + 0x18, ReadWrite>;
    struct _OTG_FS_HAINTMSK {
        using HAINTM = Field<OTG_FS_HAINTMSK_Reg, 0, 16>; // Channel interrupt mask
    };

    // OTG_FS host port control and status register (OTG_FS_HPRT)
    using OTG_FS_HPRT_Reg = Register<OTG_FS_HOST_BASE + 0x40, ReadWrite>;
    struct _OTG_FS_HPRT {
        using PCSTS = Field<OTG_FS_HPRT_Reg, 0, 1>; // Port connect status
        using PCDET = Field<OTG_FS_HPRT_Reg, 1, 1>; // Port connect detected
        using PENA = Field<OTG_FS_HPRT_Reg, 2, 1>; // Port enable
        using PENCHNG = Field<OTG_FS_HPRT_Reg, 3, 1>; // Port enable/disable change
        using POCA = Field<OTG_FS_HPRT_Reg, 4, 1>; // Port overcurrent active
        using POCCHNG = Field<OTG_FS_HPRT_Reg, 5, 1>; // Port overcurrent change
        using PRES = Field<OTG_FS_HPRT_Reg, 6, 1>; // Port resume
        using PSUSP = Field<OTG_FS_HPRT_Reg, 7, 1>; // Port suspend
        using PRST = Field<OTG_FS_HPRT_Reg, 8, 1>; // Port reset
        using PLSTS = Field<OTG_FS_HPRT_Reg, 10, 2>; // Port line status
        using PPWR = Field<OTG_FS_HPRT_Reg, 12, 1>; // Port power
        using PTCTL = Field<OTG_FS_HPRT_Reg, 13, 4>; // Port test control
        using PSPD = Field<OTG_FS_HPRT_Reg, 17, 2>; // Port speed
    };

    // OTG_FS host channel-0 characteristics register (OTG_FS_HCCHAR0)
    using OTG_FS_HCCHAR0_Reg = Register<OTG_FS_HOST_BASE + 0x100, ReadWrite>;
    struct _OTG_FS_HCCHAR0 {
        using MPSIZ = Field<OTG_FS_HCCHAR0_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_FS_HCCHAR0_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_FS_HCCHAR0_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_FS_HCCHAR0_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_FS_HCCHAR0_Reg, 18, 2>; // Endpoint type
        using MCNT = Field<OTG_FS_HCCHAR0_Reg, 20, 2>; // Multicount
        using DAD = Field<OTG_FS_HCCHAR0_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_FS_HCCHAR0_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_FS_HCCHAR0_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_FS_HCCHAR0_Reg, 31, 1>; // Channel enable
    };

    // OTG_FS host channel-1 characteristics register (OTG_FS_HCCHAR1)
    using OTG_FS_HCCHAR1_Reg = Register<OTG_FS_HOST_BASE + 0x120, ReadWrite>;
    struct _OTG_FS_HCCHAR1 {
        using MPSIZ = Field<OTG_FS_HCCHAR1_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_FS_HCCHAR1_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_FS_HCCHAR1_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_FS_HCCHAR1_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_FS_HCCHAR1_Reg, 18, 2>; // Endpoint type
        using MCNT = Field<OTG_FS_HCCHAR1_Reg, 20, 2>; // Multicount
        using DAD = Field<OTG_FS_HCCHAR1_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_FS_HCCHAR1_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_FS_HCCHAR1_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_FS_HCCHAR1_Reg, 31, 1>; // Channel enable
    };

    // OTG_FS host channel-2 characteristics register (OTG_FS_HCCHAR2)
    using OTG_FS_HCCHAR2_Reg = Register<OTG_FS_HOST_BASE + 0x140, ReadWrite>;
    struct _OTG_FS_HCCHAR2 {
        using MPSIZ = Field<OTG_FS_HCCHAR2_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_FS_HCCHAR2_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_FS_HCCHAR2_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_FS_HCCHAR2_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_FS_HCCHAR2_Reg, 18, 2>; // Endpoint type
        using MCNT = Field<OTG_FS_HCCHAR2_Reg, 20, 2>; // Multicount
        using DAD = Field<OTG_FS_HCCHAR2_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_FS_HCCHAR2_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_FS_HCCHAR2_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_FS_HCCHAR2_Reg, 31, 1>; // Channel enable
    };

    // OTG_FS host channel-3 characteristics register (OTG_FS_HCCHAR3)
    using OTG_FS_HCCHAR3_Reg = Register<OTG_FS_HOST_BASE + 0x160, ReadWrite>;
    struct _OTG_FS_HCCHAR3 {
        using MPSIZ = Field<OTG_FS_HCCHAR3_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_FS_HCCHAR3_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_FS_HCCHAR3_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_FS_HCCHAR3_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_FS_HCCHAR3_Reg, 18, 2>; // Endpoint type
        using MCNT = Field<OTG_FS_HCCHAR3_Reg, 20, 2>; // Multicount
        using DAD = Field<OTG_FS_HCCHAR3_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_FS_HCCHAR3_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_FS_HCCHAR3_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_FS_HCCHAR3_Reg, 31, 1>; // Channel enable
    };

    // OTG_FS host channel-4 characteristics register (OTG_FS_HCCHAR4)
    using OTG_FS_HCCHAR4_Reg = Register<OTG_FS_HOST_BASE + 0x180, ReadWrite>;
    struct _OTG_FS_HCCHAR4 {
        using MPSIZ = Field<OTG_FS_HCCHAR4_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_FS_HCCHAR4_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_FS_HCCHAR4_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_FS_HCCHAR4_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_FS_HCCHAR4_Reg, 18, 2>; // Endpoint type
        using MCNT = Field<OTG_FS_HCCHAR4_Reg, 20, 2>; // Multicount
        using DAD = Field<OTG_FS_HCCHAR4_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_FS_HCCHAR4_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_FS_HCCHAR4_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_FS_HCCHAR4_Reg, 31, 1>; // Channel enable
    };

    // OTG_FS host channel-5 characteristics register (OTG_FS_HCCHAR5)
    using OTG_FS_HCCHAR5_Reg = Register<OTG_FS_HOST_BASE + 0x1A0, ReadWrite>;
    struct _OTG_FS_HCCHAR5 {
        using MPSIZ = Field<OTG_FS_HCCHAR5_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_FS_HCCHAR5_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_FS_HCCHAR5_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_FS_HCCHAR5_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_FS_HCCHAR5_Reg, 18, 2>; // Endpoint type
        using MCNT = Field<OTG_FS_HCCHAR5_Reg, 20, 2>; // Multicount
        using DAD = Field<OTG_FS_HCCHAR5_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_FS_HCCHAR5_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_FS_HCCHAR5_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_FS_HCCHAR5_Reg, 31, 1>; // Channel enable
    };

    // OTG_FS host channel-6 characteristics register (OTG_FS_HCCHAR6)
    using OTG_FS_HCCHAR6_Reg = Register<OTG_FS_HOST_BASE + 0x1C0, ReadWrite>;
    struct _OTG_FS_HCCHAR6 {
        using MPSIZ = Field<OTG_FS_HCCHAR6_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_FS_HCCHAR6_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_FS_HCCHAR6_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_FS_HCCHAR6_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_FS_HCCHAR6_Reg, 18, 2>; // Endpoint type
        using MCNT = Field<OTG_FS_HCCHAR6_Reg, 20, 2>; // Multicount
        using DAD = Field<OTG_FS_HCCHAR6_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_FS_HCCHAR6_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_FS_HCCHAR6_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_FS_HCCHAR6_Reg, 31, 1>; // Channel enable
    };

    // OTG_FS host channel-7 characteristics register (OTG_FS_HCCHAR7)
    using OTG_FS_HCCHAR7_Reg = Register<OTG_FS_HOST_BASE + 0x1E0, ReadWrite>;
    struct _OTG_FS_HCCHAR7 {
        using MPSIZ = Field<OTG_FS_HCCHAR7_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_FS_HCCHAR7_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_FS_HCCHAR7_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_FS_HCCHAR7_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_FS_HCCHAR7_Reg, 18, 2>; // Endpoint type
        using MCNT = Field<OTG_FS_HCCHAR7_Reg, 20, 2>; // Multicount
        using DAD = Field<OTG_FS_HCCHAR7_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_FS_HCCHAR7_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_FS_HCCHAR7_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_FS_HCCHAR7_Reg, 31, 1>; // Channel enable
    };

    // OTG_FS host channel-0 interrupt register (OTG_FS_HCINT0)
    using OTG_FS_HCINT0_Reg = Register<OTG_FS_HOST_BASE + 0x108, ReadWrite>;
    struct _OTG_FS_HCINT0 {
        using XFRC = Field<OTG_FS_HCINT0_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_FS_HCINT0_Reg, 1, 1>; // Channel halted
        using STALL = Field<OTG_FS_HCINT0_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_FS_HCINT0_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_FS_HCINT0_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using TXERR = Field<OTG_FS_HCINT0_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_FS_HCINT0_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_FS_HCINT0_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_FS_HCINT0_Reg, 10, 1>; // Data toggle error
    };

    // OTG_FS host channel-1 interrupt register (OTG_FS_HCINT1)
    using OTG_FS_HCINT1_Reg = Register<OTG_FS_HOST_BASE + 0x128, ReadWrite>;
    struct _OTG_FS_HCINT1 {
        using XFRC = Field<OTG_FS_HCINT1_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_FS_HCINT1_Reg, 1, 1>; // Channel halted
        using STALL = Field<OTG_FS_HCINT1_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_FS_HCINT1_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_FS_HCINT1_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using TXERR = Field<OTG_FS_HCINT1_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_FS_HCINT1_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_FS_HCINT1_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_FS_HCINT1_Reg, 10, 1>; // Data toggle error
    };

    // OTG_FS host channel-2 interrupt register (OTG_FS_HCINT2)
    using OTG_FS_HCINT2_Reg = Register<OTG_FS_HOST_BASE + 0x148, ReadWrite>;
    struct _OTG_FS_HCINT2 {
        using XFRC = Field<OTG_FS_HCINT2_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_FS_HCINT2_Reg, 1, 1>; // Channel halted
        using STALL = Field<OTG_FS_HCINT2_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_FS_HCINT2_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_FS_HCINT2_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using TXERR = Field<OTG_FS_HCINT2_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_FS_HCINT2_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_FS_HCINT2_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_FS_HCINT2_Reg, 10, 1>; // Data toggle error
    };

    // OTG_FS host channel-3 interrupt register (OTG_FS_HCINT3)
    using OTG_FS_HCINT3_Reg = Register<OTG_FS_HOST_BASE + 0x168, ReadWrite>;
    struct _OTG_FS_HCINT3 {
        using XFRC = Field<OTG_FS_HCINT3_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_FS_HCINT3_Reg, 1, 1>; // Channel halted
        using STALL = Field<OTG_FS_HCINT3_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_FS_HCINT3_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_FS_HCINT3_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using TXERR = Field<OTG_FS_HCINT3_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_FS_HCINT3_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_FS_HCINT3_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_FS_HCINT3_Reg, 10, 1>; // Data toggle error
    };

    // OTG_FS host channel-4 interrupt register (OTG_FS_HCINT4)
    using OTG_FS_HCINT4_Reg = Register<OTG_FS_HOST_BASE + 0x188, ReadWrite>;
    struct _OTG_FS_HCINT4 {
        using XFRC = Field<OTG_FS_HCINT4_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_FS_HCINT4_Reg, 1, 1>; // Channel halted
        using STALL = Field<OTG_FS_HCINT4_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_FS_HCINT4_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_FS_HCINT4_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using TXERR = Field<OTG_FS_HCINT4_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_FS_HCINT4_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_FS_HCINT4_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_FS_HCINT4_Reg, 10, 1>; // Data toggle error
    };

    // OTG_FS host channel-5 interrupt register (OTG_FS_HCINT5)
    using OTG_FS_HCINT5_Reg = Register<OTG_FS_HOST_BASE + 0x1A8, ReadWrite>;
    struct _OTG_FS_HCINT5 {
        using XFRC = Field<OTG_FS_HCINT5_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_FS_HCINT5_Reg, 1, 1>; // Channel halted
        using STALL = Field<OTG_FS_HCINT5_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_FS_HCINT5_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_FS_HCINT5_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using TXERR = Field<OTG_FS_HCINT5_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_FS_HCINT5_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_FS_HCINT5_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_FS_HCINT5_Reg, 10, 1>; // Data toggle error
    };

    // OTG_FS host channel-6 interrupt register (OTG_FS_HCINT6)
    using OTG_FS_HCINT6_Reg = Register<OTG_FS_HOST_BASE + 0x1C8, ReadWrite>;
    struct _OTG_FS_HCINT6 {
        using XFRC = Field<OTG_FS_HCINT6_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_FS_HCINT6_Reg, 1, 1>; // Channel halted
        using STALL = Field<OTG_FS_HCINT6_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_FS_HCINT6_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_FS_HCINT6_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using TXERR = Field<OTG_FS_HCINT6_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_FS_HCINT6_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_FS_HCINT6_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_FS_HCINT6_Reg, 10, 1>; // Data toggle error
    };

    // OTG_FS host channel-7 interrupt register (OTG_FS_HCINT7)
    using OTG_FS_HCINT7_Reg = Register<OTG_FS_HOST_BASE + 0x1E8, ReadWrite>;
    struct _OTG_FS_HCINT7 {
        using XFRC = Field<OTG_FS_HCINT7_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_FS_HCINT7_Reg, 1, 1>; // Channel halted
        using STALL = Field<OTG_FS_HCINT7_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_FS_HCINT7_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_FS_HCINT7_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using TXERR = Field<OTG_FS_HCINT7_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_FS_HCINT7_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_FS_HCINT7_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_FS_HCINT7_Reg, 10, 1>; // Data toggle error
    };

    // OTG_FS host channel-0 mask register (OTG_FS_HCINTMSK0)
    using OTG_FS_HCINTMSK0_Reg = Register<OTG_FS_HOST_BASE + 0x10C, ReadWrite>;
    struct _OTG_FS_HCINTMSK0 {
        using XFRCM = Field<OTG_FS_HCINTMSK0_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_FS_HCINTMSK0_Reg, 1, 1>; // Channel halted mask
        using STALLM = Field<OTG_FS_HCINTMSK0_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_FS_HCINTMSK0_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_FS_HCINTMSK0_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_FS_HCINTMSK0_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_FS_HCINTMSK0_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_FS_HCINTMSK0_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_FS_HCINTMSK0_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_FS_HCINTMSK0_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_FS host channel-1 mask register (OTG_FS_HCINTMSK1)
    using OTG_FS_HCINTMSK1_Reg = Register<OTG_FS_HOST_BASE + 0x12C, ReadWrite>;
    struct _OTG_FS_HCINTMSK1 {
        using XFRCM = Field<OTG_FS_HCINTMSK1_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_FS_HCINTMSK1_Reg, 1, 1>; // Channel halted mask
        using STALLM = Field<OTG_FS_HCINTMSK1_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_FS_HCINTMSK1_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_FS_HCINTMSK1_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_FS_HCINTMSK1_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_FS_HCINTMSK1_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_FS_HCINTMSK1_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_FS_HCINTMSK1_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_FS_HCINTMSK1_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_FS host channel-2 mask register (OTG_FS_HCINTMSK2)
    using OTG_FS_HCINTMSK2_Reg = Register<OTG_FS_HOST_BASE + 0x14C, ReadWrite>;
    struct _OTG_FS_HCINTMSK2 {
        using XFRCM = Field<OTG_FS_HCINTMSK2_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_FS_HCINTMSK2_Reg, 1, 1>; // Channel halted mask
        using STALLM = Field<OTG_FS_HCINTMSK2_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_FS_HCINTMSK2_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_FS_HCINTMSK2_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_FS_HCINTMSK2_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_FS_HCINTMSK2_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_FS_HCINTMSK2_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_FS_HCINTMSK2_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_FS_HCINTMSK2_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_FS host channel-3 mask register (OTG_FS_HCINTMSK3)
    using OTG_FS_HCINTMSK3_Reg = Register<OTG_FS_HOST_BASE + 0x16C, ReadWrite>;
    struct _OTG_FS_HCINTMSK3 {
        using XFRCM = Field<OTG_FS_HCINTMSK3_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_FS_HCINTMSK3_Reg, 1, 1>; // Channel halted mask
        using STALLM = Field<OTG_FS_HCINTMSK3_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_FS_HCINTMSK3_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_FS_HCINTMSK3_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_FS_HCINTMSK3_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_FS_HCINTMSK3_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_FS_HCINTMSK3_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_FS_HCINTMSK3_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_FS_HCINTMSK3_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_FS host channel-4 mask register (OTG_FS_HCINTMSK4)
    using OTG_FS_HCINTMSK4_Reg = Register<OTG_FS_HOST_BASE + 0x18C, ReadWrite>;
    struct _OTG_FS_HCINTMSK4 {
        using XFRCM = Field<OTG_FS_HCINTMSK4_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_FS_HCINTMSK4_Reg, 1, 1>; // Channel halted mask
        using STALLM = Field<OTG_FS_HCINTMSK4_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_FS_HCINTMSK4_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_FS_HCINTMSK4_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_FS_HCINTMSK4_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_FS_HCINTMSK4_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_FS_HCINTMSK4_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_FS_HCINTMSK4_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_FS_HCINTMSK4_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_FS host channel-5 mask register (OTG_FS_HCINTMSK5)
    using OTG_FS_HCINTMSK5_Reg = Register<OTG_FS_HOST_BASE + 0x1AC, ReadWrite>;
    struct _OTG_FS_HCINTMSK5 {
        using XFRCM = Field<OTG_FS_HCINTMSK5_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_FS_HCINTMSK5_Reg, 1, 1>; // Channel halted mask
        using STALLM = Field<OTG_FS_HCINTMSK5_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_FS_HCINTMSK5_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_FS_HCINTMSK5_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_FS_HCINTMSK5_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_FS_HCINTMSK5_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_FS_HCINTMSK5_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_FS_HCINTMSK5_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_FS_HCINTMSK5_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_FS host channel-6 mask register (OTG_FS_HCINTMSK6)
    using OTG_FS_HCINTMSK6_Reg = Register<OTG_FS_HOST_BASE + 0x1CC, ReadWrite>;
    struct _OTG_FS_HCINTMSK6 {
        using XFRCM = Field<OTG_FS_HCINTMSK6_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_FS_HCINTMSK6_Reg, 1, 1>; // Channel halted mask
        using STALLM = Field<OTG_FS_HCINTMSK6_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_FS_HCINTMSK6_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_FS_HCINTMSK6_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_FS_HCINTMSK6_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_FS_HCINTMSK6_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_FS_HCINTMSK6_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_FS_HCINTMSK6_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_FS_HCINTMSK6_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_FS host channel-7 mask register (OTG_FS_HCINTMSK7)
    using OTG_FS_HCINTMSK7_Reg = Register<OTG_FS_HOST_BASE + 0x1EC, ReadWrite>;
    struct _OTG_FS_HCINTMSK7 {
        using XFRCM = Field<OTG_FS_HCINTMSK7_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_FS_HCINTMSK7_Reg, 1, 1>; // Channel halted mask
        using STALLM = Field<OTG_FS_HCINTMSK7_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_FS_HCINTMSK7_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_FS_HCINTMSK7_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_FS_HCINTMSK7_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_FS_HCINTMSK7_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_FS_HCINTMSK7_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_FS_HCINTMSK7_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_FS_HCINTMSK7_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_FS host channel-0 transfer size register
    using OTG_FS_HCTSIZ0_Reg = Register<OTG_FS_HOST_BASE + 0x110, ReadWrite>;
    struct _OTG_FS_HCTSIZ0 {
        using XFRSIZ = Field<OTG_FS_HCTSIZ0_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_FS_HCTSIZ0_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_FS_HCTSIZ0_Reg, 29, 2>; // Data PID
    };

    // OTG_FS host channel-1 transfer size register
    using OTG_FS_HCTSIZ1_Reg = Register<OTG_FS_HOST_BASE + 0x130, ReadWrite>;
    struct _OTG_FS_HCTSIZ1 {
        using XFRSIZ = Field<OTG_FS_HCTSIZ1_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_FS_HCTSIZ1_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_FS_HCTSIZ1_Reg, 29, 2>; // Data PID
    };

    // OTG_FS host channel-2 transfer size register
    using OTG_FS_HCTSIZ2_Reg = Register<OTG_FS_HOST_BASE + 0x150, ReadWrite>;
    struct _OTG_FS_HCTSIZ2 {
        using XFRSIZ = Field<OTG_FS_HCTSIZ2_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_FS_HCTSIZ2_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_FS_HCTSIZ2_Reg, 29, 2>; // Data PID
    };

    // OTG_FS host channel-3 transfer size register
    using OTG_FS_HCTSIZ3_Reg = Register<OTG_FS_HOST_BASE + 0x170, ReadWrite>;
    struct _OTG_FS_HCTSIZ3 {
        using XFRSIZ = Field<OTG_FS_HCTSIZ3_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_FS_HCTSIZ3_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_FS_HCTSIZ3_Reg, 29, 2>; // Data PID
    };

    // OTG_FS host channel-x transfer size register
    using OTG_FS_HCTSIZ4_Reg = Register<OTG_FS_HOST_BASE + 0x190, ReadWrite>;
    struct _OTG_FS_HCTSIZ4 {
        using XFRSIZ = Field<OTG_FS_HCTSIZ4_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_FS_HCTSIZ4_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_FS_HCTSIZ4_Reg, 29, 2>; // Data PID
    };

    // OTG_FS host channel-5 transfer size register
    using OTG_FS_HCTSIZ5_Reg = Register<OTG_FS_HOST_BASE + 0x1B0, ReadWrite>;
    struct _OTG_FS_HCTSIZ5 {
        using XFRSIZ = Field<OTG_FS_HCTSIZ5_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_FS_HCTSIZ5_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_FS_HCTSIZ5_Reg, 29, 2>; // Data PID
    };

    // OTG_FS host channel-6 transfer size register
    using OTG_FS_HCTSIZ6_Reg = Register<OTG_FS_HOST_BASE + 0x1D0, ReadWrite>;
    struct _OTG_FS_HCTSIZ6 {
        using XFRSIZ = Field<OTG_FS_HCTSIZ6_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_FS_HCTSIZ6_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_FS_HCTSIZ6_Reg, 29, 2>; // Data PID
    };

    // OTG_FS host channel-7 transfer size register
    using OTG_FS_HCTSIZ7_Reg = Register<OTG_FS_HOST_BASE + 0x1F0, ReadWrite>;
    struct _OTG_FS_HCTSIZ7 {
        using XFRSIZ = Field<OTG_FS_HCTSIZ7_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_FS_HCTSIZ7_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_FS_HCTSIZ7_Reg, 29, 2>; // Data PID
    };

    // OTG_FS host channel-8 characteristics register
    using OTG_FS_HCCHAR8_Reg = Register<OTG_FS_HOST_BASE + 0x1F4, ReadWrite>;
    struct _OTG_FS_HCCHAR8 {
        using MPSIZ = Field<OTG_FS_HCCHAR8_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_FS_HCCHAR8_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_FS_HCCHAR8_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_FS_HCCHAR8_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_FS_HCCHAR8_Reg, 18, 2>; // Endpoint type
        using MCNT = Field<OTG_FS_HCCHAR8_Reg, 20, 2>; // Multicount
        using DAD = Field<OTG_FS_HCCHAR8_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_FS_HCCHAR8_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_FS_HCCHAR8_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_FS_HCCHAR8_Reg, 31, 1>; // Channel enable
    };

    // OTG_FS host channel-8 interrupt register
    using OTG_FS_HCINT8_Reg = Register<OTG_FS_HOST_BASE + 0x1F8, ReadWrite>;
    struct _OTG_FS_HCINT8 {
        using XFRC = Field<OTG_FS_HCINT8_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_FS_HCINT8_Reg, 1, 1>; // Channel halted
        using STALL = Field<OTG_FS_HCINT8_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_FS_HCINT8_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_FS_HCINT8_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using TXERR = Field<OTG_FS_HCINT8_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_FS_HCINT8_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_FS_HCINT8_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_FS_HCINT8_Reg, 10, 1>; // Data toggle error
    };

    // OTG_FS host channel-8 mask register
    using OTG_FS_HCINTMSK8_Reg = Register<OTG_FS_HOST_BASE + 0x1FC, ReadWrite>;
    struct _OTG_FS_HCINTMSK8 {
        using XFRCM = Field<OTG_FS_HCINTMSK8_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_FS_HCINTMSK8_Reg, 1, 1>; // Channel halted mask
        using STALLM = Field<OTG_FS_HCINTMSK8_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_FS_HCINTMSK8_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_FS_HCINTMSK8_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_FS_HCINTMSK8_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_FS_HCINTMSK8_Reg, 7, 1>; // Transaction error
        using BBERRM = Field<OTG_FS_HCINTMSK8_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_FS_HCINTMSK8_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_FS_HCINTMSK8_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_FS host channel-8 transfer size register
    using OTG_FS_HCTSIZ8_Reg = Register<OTG_FS_HOST_BASE + 0x200, ReadWrite>;
    struct _OTG_FS_HCTSIZ8 {
        using XFRSIZ = Field<OTG_FS_HCTSIZ8_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_FS_HCTSIZ8_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_FS_HCTSIZ8_Reg, 29, 2>; // Data PID
    };

    // OTG_FS host channel-9 characteristics register
    using OTG_FS_HCCHAR9_Reg = Register<OTG_FS_HOST_BASE + 0x204, ReadWrite>;
    struct _OTG_FS_HCCHAR9 {
        using MPSIZ = Field<OTG_FS_HCCHAR9_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_FS_HCCHAR9_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_FS_HCCHAR9_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_FS_HCCHAR9_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_FS_HCCHAR9_Reg, 18, 2>; // Endpoint type
        using MCNT = Field<OTG_FS_HCCHAR9_Reg, 20, 2>; // Multicount
        using DAD = Field<OTG_FS_HCCHAR9_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_FS_HCCHAR9_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_FS_HCCHAR9_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_FS_HCCHAR9_Reg, 31, 1>; // Channel enable
    };

    // OTG_FS host channel-9 interrupt register
    using OTG_FS_HCINT9_Reg = Register<OTG_FS_HOST_BASE + 0x208, ReadWrite>;
    struct _OTG_FS_HCINT9 {
        using XFRC = Field<OTG_FS_HCINT9_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_FS_HCINT9_Reg, 1, 1>; // Channel halted
        using STALL = Field<OTG_FS_HCINT9_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_FS_HCINT9_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_FS_HCINT9_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using TXERR = Field<OTG_FS_HCINT9_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_FS_HCINT9_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_FS_HCINT9_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_FS_HCINT9_Reg, 10, 1>; // Data toggle error
    };

    // OTG_FS host channel-9 mask register
    using OTG_FS_HCINTMSK9_Reg = Register<OTG_FS_HOST_BASE + 0x20C, ReadWrite>;
    struct _OTG_FS_HCINTMSK9 {
        using XFRCM = Field<OTG_FS_HCINTMSK9_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_FS_HCINTMSK9_Reg, 1, 1>; // Channel halted mask
        using STALLM = Field<OTG_FS_HCINTMSK9_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_FS_HCINTMSK9_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_FS_HCINTMSK9_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_FS_HCINTMSK9_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_FS_HCINTMSK9_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_FS_HCINTMSK9_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_FS_HCINTMSK9_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_FS_HCINTMSK9_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_FS host channel-9 transfer size register
    using OTG_FS_HCTSIZ9_Reg = Register<OTG_FS_HOST_BASE + 0x210, ReadWrite>;
    struct _OTG_FS_HCTSIZ9 {
        using XFRSIZ = Field<OTG_FS_HCTSIZ9_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_FS_HCTSIZ9_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_FS_HCTSIZ9_Reg, 29, 2>; // Data PID
    };

    // OTG_FS host channel-10 characteristics register
    using OTG_FS_HCCHAR10_Reg = Register<OTG_FS_HOST_BASE + 0x214, ReadWrite>;
    struct _OTG_FS_HCCHAR10 {
        using MPSIZ = Field<OTG_FS_HCCHAR10_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_FS_HCCHAR10_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_FS_HCCHAR10_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_FS_HCCHAR10_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_FS_HCCHAR10_Reg, 18, 2>; // Endpoint type
        using MCNT = Field<OTG_FS_HCCHAR10_Reg, 20, 2>; // Multicount
        using DAD = Field<OTG_FS_HCCHAR10_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_FS_HCCHAR10_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_FS_HCCHAR10_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_FS_HCCHAR10_Reg, 31, 1>; // Channel enable
    };

    // OTG_FS host channel-10 interrupt register
    using OTG_FS_HCINT10_Reg = Register<OTG_FS_HOST_BASE + 0x218, ReadWrite>;
    struct _OTG_FS_HCINT10 {
        using XFRC = Field<OTG_FS_HCINT10_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_FS_HCINT10_Reg, 1, 1>; // Channel halted
        using STALL = Field<OTG_FS_HCINT10_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_FS_HCINT10_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_FS_HCINT10_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using TXERR = Field<OTG_FS_HCINT10_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_FS_HCINT10_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_FS_HCINT10_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_FS_HCINT10_Reg, 10, 1>; // Data toggle error
    };

    // OTG_FS host channel-10 mask register
    using OTG_FS_HCINTMSK10_Reg = Register<OTG_FS_HOST_BASE + 0x21C, ReadWrite>;
    struct _OTG_FS_HCINTMSK10 {
        using XFRCM = Field<OTG_FS_HCINTMSK10_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_FS_HCINTMSK10_Reg, 1, 1>; // Channel halted mask
        using STALLM = Field<OTG_FS_HCINTMSK10_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_FS_HCINTMSK10_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_FS_HCINTMSK10_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_FS_HCINTMSK10_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_FS_HCINTMSK10_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_FS_HCINTMSK10_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_FS_HCINTMSK10_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_FS_HCINTMSK10_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_FS host channel-10 transfer size register
    using OTG_FS_HCTSIZ10_Reg = Register<OTG_FS_HOST_BASE + 0x220, ReadWrite>;
    struct _OTG_FS_HCTSIZ10 {
        using XFRSIZ = Field<OTG_FS_HCTSIZ10_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_FS_HCTSIZ10_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_FS_HCTSIZ10_Reg, 29, 2>; // Data PID
    };

    // OTG_FS host channel-11 characteristics register
    using OTG_FS_HCCHAR11_Reg = Register<OTG_FS_HOST_BASE + 0x224, ReadWrite>;
    struct _OTG_FS_HCCHAR11 {
        using MPSIZ = Field<OTG_FS_HCCHAR11_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_FS_HCCHAR11_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_FS_HCCHAR11_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_FS_HCCHAR11_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_FS_HCCHAR11_Reg, 18, 2>; // Endpoint type
        using MCNT = Field<OTG_FS_HCCHAR11_Reg, 20, 2>; // Multicount
        using DAD = Field<OTG_FS_HCCHAR11_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_FS_HCCHAR11_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_FS_HCCHAR11_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_FS_HCCHAR11_Reg, 31, 1>; // Channel enable
    };

    // OTG_FS host channel-11 interrupt register
    using OTG_FS_HCINT11_Reg = Register<OTG_FS_HOST_BASE + 0x228, ReadWrite>;
    struct _OTG_FS_HCINT11 {
        using XFRC = Field<OTG_FS_HCINT11_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_FS_HCINT11_Reg, 1, 1>; // Channel halted
        using STALL = Field<OTG_FS_HCINT11_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_FS_HCINT11_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_FS_HCINT11_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using TXERR = Field<OTG_FS_HCINT11_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_FS_HCINT11_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_FS_HCINT11_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_FS_HCINT11_Reg, 10, 1>; // Data toggle error
    };

    // OTG_FS host channel-11 mask register
    using OTG_FS_HCINTMSK11_Reg = Register<OTG_FS_HOST_BASE + 0x22C, ReadWrite>;
    struct _OTG_FS_HCINTMSK11 {
        using XFRCM = Field<OTG_FS_HCINTMSK11_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_FS_HCINTMSK11_Reg, 1, 1>; // Channel halted mask
        using STALLM = Field<OTG_FS_HCINTMSK11_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_FS_HCINTMSK11_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_FS_HCINTMSK11_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_FS_HCINTMSK11_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_FS_HCINTMSK11_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_FS_HCINTMSK11_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_FS_HCINTMSK11_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_FS_HCINTMSK11_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_FS host channel-11 transfer size register
    using OTG_FS_HCTSIZ11_Reg = Register<OTG_FS_HOST_BASE + 0x230, ReadWrite>;
    struct _OTG_FS_HCTSIZ11 {
        using XFRSIZ = Field<OTG_FS_HCTSIZ11_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_FS_HCTSIZ11_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_FS_HCTSIZ11_Reg, 29, 2>; // Data PID
    };

} // namespace OTG_FS_HOST

// --------------------------------------------
// OTG_FS_DEVICE: USB on the go full speed
// Base address: 0x50000800
// --------------------------------------------

namespace OTG_FS_DEVICE {
    static constexpr uint32_t OTG_FS_DEVICE_BASE = 0x50000800;

    // OTG_FS device configuration register (OTG_FS_DCFG)
    using OTG_FS_DCFG_Reg = Register<OTG_FS_DEVICE_BASE + 0x0, ReadWrite>;
    struct _OTG_FS_DCFG {
        using DSPD = Field<OTG_FS_DCFG_Reg, 0, 2>; // Device speed
        using NZLSOHSK = Field<OTG_FS_DCFG_Reg, 2, 1>; // Non-zero-length status OUT handshake
        using DAD = Field<OTG_FS_DCFG_Reg, 4, 7>; // Device address
        using PFIVL = Field<OTG_FS_DCFG_Reg, 11, 2>; // Periodic frame interval
    };

    // OTG_FS device control register (OTG_FS_DCTL)
    using OTG_FS_DCTL_Reg = Register<OTG_FS_DEVICE_BASE + 0x4, ReadWrite>;
    struct _OTG_FS_DCTL {
        using RWUSIG = Field<OTG_FS_DCTL_Reg, 0, 1>; // Remote wakeup signaling
        using SDIS = Field<OTG_FS_DCTL_Reg, 1, 1>; // Soft disconnect
        using GINSTS = Field<OTG_FS_DCTL_Reg, 2, 1>; // Global IN NAK status
        using GONSTS = Field<OTG_FS_DCTL_Reg, 3, 1>; // Global OUT NAK status
        using TCTL = Field<OTG_FS_DCTL_Reg, 4, 3>; // Test control
        using SGINAK = Field<OTG_FS_DCTL_Reg, 7, 1>; // Set global IN NAK
        using CGINAK = Field<OTG_FS_DCTL_Reg, 8, 1>; // Clear global IN NAK
        using SGONAK = Field<OTG_FS_DCTL_Reg, 9, 1>; // Set global OUT NAK
        using CGONAK = Field<OTG_FS_DCTL_Reg, 10, 1>; // Clear global OUT NAK
        using POPRGDNE = Field<OTG_FS_DCTL_Reg, 11, 1>; // Power-on programming done
    };

    // OTG_FS device status register (OTG_FS_DSTS)
    using OTG_FS_DSTS_Reg = Register<OTG_FS_DEVICE_BASE + 0x8, ReadOnly>;
    struct _OTG_FS_DSTS {
        using SUSPSTS = Field<OTG_FS_DSTS_Reg, 0, 1>; // Suspend status
        using ENUMSPD = Field<OTG_FS_DSTS_Reg, 1, 2>; // Enumerated speed
        using EERR = Field<OTG_FS_DSTS_Reg, 3, 1>; // Erratic error
        using FNSOF = Field<OTG_FS_DSTS_Reg, 8, 14>; // Frame number of the received SOF
    };

    // OTG_FS device IN endpoint common interrupt mask register (OTG_FS_DIEPMSK)
    using OTG_FS_DIEPMSK_Reg = Register<OTG_FS_DEVICE_BASE + 0x10, ReadWrite>;
    struct _OTG_FS_DIEPMSK {
        using XFRCM = Field<OTG_FS_DIEPMSK_Reg, 0, 1>; // Transfer completed interrupt mask
        using EPDM = Field<OTG_FS_DIEPMSK_Reg, 1, 1>; // Endpoint disabled interrupt mask
        using TOM = Field<OTG_FS_DIEPMSK_Reg, 3, 1>; // Timeout condition mask (Non-isochronous endpoints)
        using ITTXFEMSK = Field<OTG_FS_DIEPMSK_Reg, 4, 1>; // IN token received when TxFIFO empty mask
        using INEPNMM = Field<OTG_FS_DIEPMSK_Reg, 5, 1>; // IN token received with EP mismatch mask
        using INEPNEM = Field<OTG_FS_DIEPMSK_Reg, 6, 1>; // IN endpoint NAK effective mask
    };

    // OTG_FS device OUT endpoint common interrupt mask register (OTG_FS_DOEPMSK)
    using OTG_FS_DOEPMSK_Reg = Register<OTG_FS_DEVICE_BASE + 0x14, ReadWrite>;
    struct _OTG_FS_DOEPMSK {
        using XFRCM = Field<OTG_FS_DOEPMSK_Reg, 0, 1>; // Transfer completed interrupt mask
        using EPDM = Field<OTG_FS_DOEPMSK_Reg, 1, 1>; // Endpoint disabled interrupt mask
        using STUPM = Field<OTG_FS_DOEPMSK_Reg, 3, 1>; // SETUP phase done mask
        using OTEPDM = Field<OTG_FS_DOEPMSK_Reg, 4, 1>; // OUT token received when endpoint disabled mask
    };

    // OTG_FS device all endpoints interrupt register (OTG_FS_DAINT)
    using OTG_FS_DAINT_Reg = Register<OTG_FS_DEVICE_BASE + 0x18, ReadOnly>;
    struct _OTG_FS_DAINT {
        using IEPINT = Field<OTG_FS_DAINT_Reg, 0, 16>; // IN endpoint interrupt bits
        using OEPINT = Field<OTG_FS_DAINT_Reg, 16, 16>; // OUT endpoint interrupt bits
    };

    // OTG_FS all endpoints interrupt mask register (OTG_FS_DAINTMSK)
    using OTG_FS_DAINTMSK_Reg = Register<OTG_FS_DEVICE_BASE + 0x1C, ReadWrite>;
    struct _OTG_FS_DAINTMSK {
        using IEPM = Field<OTG_FS_DAINTMSK_Reg, 0, 16>; // IN EP interrupt mask bits
        using OEPINT = Field<OTG_FS_DAINTMSK_Reg, 16, 16>; // OUT endpoint interrupt bits
    };

    // OTG_FS device VBUS discharge time register
    using OTG_FS_DVBUSDIS_Reg = Register<OTG_FS_DEVICE_BASE + 0x28, ReadWrite>;
    struct _OTG_FS_DVBUSDIS {
        using VBUSDT = Field<OTG_FS_DVBUSDIS_Reg, 0, 16>; // Device VBUS discharge time
    };

    // OTG_FS device VBUS pulsing time register
    using OTG_FS_DVBUSPULSE_Reg = Register<OTG_FS_DEVICE_BASE + 0x2C, ReadWrite>;
    struct _OTG_FS_DVBUSPULSE {
        using DVBUSP = Field<OTG_FS_DVBUSPULSE_Reg, 0, 12>; // Device VBUS pulsing time
    };

    // OTG_FS device IN endpoint FIFO empty interrupt mask register
    using OTG_FS_DIEPEMPMSK_Reg = Register<OTG_FS_DEVICE_BASE + 0x34, ReadWrite>;
    struct _OTG_FS_DIEPEMPMSK {
        using INEPTXFEM = Field<OTG_FS_DIEPEMPMSK_Reg, 0, 16>; // IN EP Tx FIFO empty interrupt mask bits
    };

    // OTG_FS device control IN endpoint 0 control register (OTG_FS_DIEPCTL0)
    using OTG_FS_DIEPCTL0_Reg = Register<OTG_FS_DEVICE_BASE + 0x100, ReadWrite>;
    struct _OTG_FS_DIEPCTL0 {
        using MPSIZ = Field<OTG_FS_DIEPCTL0_Reg, 0, 2>; // Maximum packet size
        using USBAEP = Field<OTG_FS_DIEPCTL0_Reg, 15, 1>; // USB active endpoint
        using NAKSTS = Field<OTG_FS_DIEPCTL0_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_FS_DIEPCTL0_Reg, 18, 2>; // Endpoint type
        using STALL = Field<OTG_FS_DIEPCTL0_Reg, 21, 1>; // STALL handshake
        using TXFNUM = Field<OTG_FS_DIEPCTL0_Reg, 22, 4>; // TxFIFO number
        using CNAK = Field<OTG_FS_DIEPCTL0_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_FS_DIEPCTL0_Reg, 27, 1>; // Set NAK
        using EPDIS = Field<OTG_FS_DIEPCTL0_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_FS_DIEPCTL0_Reg, 31, 1>; // Endpoint enable
    };

    // OTG device endpoint-1 control register
    using OTG_FS_DIEPCTL1_Reg = Register<OTG_FS_DEVICE_BASE + 0x120, ReadWrite>;
    struct _OTG_FS_DIEPCTL1 {
        using EPENA = Field<OTG_FS_DIEPCTL1_Reg, 31, 1>; // EPENA
        using EPDIS = Field<OTG_FS_DIEPCTL1_Reg, 30, 1>; // EPDIS
        using SODDFRM_SD1PID = Field<OTG_FS_DIEPCTL1_Reg, 29, 1>; // SODDFRM/SD1PID
        using SD0PID_SEVNFRM = Field<OTG_FS_DIEPCTL1_Reg, 28, 1>; // SD0PID/SEVNFRM
        using SNAK = Field<OTG_FS_DIEPCTL1_Reg, 27, 1>; // SNAK
        using CNAK = Field<OTG_FS_DIEPCTL1_Reg, 26, 1>; // CNAK
        using TXFNUM = Field<OTG_FS_DIEPCTL1_Reg, 22, 4>; // TXFNUM
        using Stall = Field<OTG_FS_DIEPCTL1_Reg, 21, 1>; // Stall
        using EPTYP = Field<OTG_FS_DIEPCTL1_Reg, 18, 2>; // EPTYP
        using NAKSTS = Field<OTG_FS_DIEPCTL1_Reg, 17, 1>; // NAKSTS
        using EONUM_DPID = Field<OTG_FS_DIEPCTL1_Reg, 16, 1>; // EONUM/DPID
        using USBAEP = Field<OTG_FS_DIEPCTL1_Reg, 15, 1>; // USBAEP
        using MPSIZ = Field<OTG_FS_DIEPCTL1_Reg, 0, 11>; // MPSIZ
    };

    // OTG device endpoint-2 control register
    using OTG_FS_DIEPCTL2_Reg = Register<OTG_FS_DEVICE_BASE + 0x140, ReadWrite>;
    struct _OTG_FS_DIEPCTL2 {
        using EPENA = Field<OTG_FS_DIEPCTL2_Reg, 31, 1>; // EPENA
        using EPDIS = Field<OTG_FS_DIEPCTL2_Reg, 30, 1>; // EPDIS
        using SODDFRM = Field<OTG_FS_DIEPCTL2_Reg, 29, 1>; // SODDFRM
        using SD0PID_SEVNFRM = Field<OTG_FS_DIEPCTL2_Reg, 28, 1>; // SD0PID/SEVNFRM
        using SNAK = Field<OTG_FS_DIEPCTL2_Reg, 27, 1>; // SNAK
        using CNAK = Field<OTG_FS_DIEPCTL2_Reg, 26, 1>; // CNAK
        using TXFNUM = Field<OTG_FS_DIEPCTL2_Reg, 22, 4>; // TXFNUM
        using Stall = Field<OTG_FS_DIEPCTL2_Reg, 21, 1>; // Stall
        using EPTYP = Field<OTG_FS_DIEPCTL2_Reg, 18, 2>; // EPTYP
        using NAKSTS = Field<OTG_FS_DIEPCTL2_Reg, 17, 1>; // NAKSTS
        using EONUM_DPID = Field<OTG_FS_DIEPCTL2_Reg, 16, 1>; // EONUM/DPID
        using USBAEP = Field<OTG_FS_DIEPCTL2_Reg, 15, 1>; // USBAEP
        using MPSIZ = Field<OTG_FS_DIEPCTL2_Reg, 0, 11>; // MPSIZ
    };

    // OTG device endpoint-3 control register
    using OTG_FS_DIEPCTL3_Reg = Register<OTG_FS_DEVICE_BASE + 0x160, ReadWrite>;
    struct _OTG_FS_DIEPCTL3 {
        using EPENA = Field<OTG_FS_DIEPCTL3_Reg, 31, 1>; // EPENA
        using EPDIS = Field<OTG_FS_DIEPCTL3_Reg, 30, 1>; // EPDIS
        using SODDFRM = Field<OTG_FS_DIEPCTL3_Reg, 29, 1>; // SODDFRM
        using SD0PID_SEVNFRM = Field<OTG_FS_DIEPCTL3_Reg, 28, 1>; // SD0PID/SEVNFRM
        using SNAK = Field<OTG_FS_DIEPCTL3_Reg, 27, 1>; // SNAK
        using CNAK = Field<OTG_FS_DIEPCTL3_Reg, 26, 1>; // CNAK
        using TXFNUM = Field<OTG_FS_DIEPCTL3_Reg, 22, 4>; // TXFNUM
        using Stall = Field<OTG_FS_DIEPCTL3_Reg, 21, 1>; // Stall
        using EPTYP = Field<OTG_FS_DIEPCTL3_Reg, 18, 2>; // EPTYP
        using NAKSTS = Field<OTG_FS_DIEPCTL3_Reg, 17, 1>; // NAKSTS
        using EONUM_DPID = Field<OTG_FS_DIEPCTL3_Reg, 16, 1>; // EONUM/DPID
        using USBAEP = Field<OTG_FS_DIEPCTL3_Reg, 15, 1>; // USBAEP
        using MPSIZ = Field<OTG_FS_DIEPCTL3_Reg, 0, 11>; // MPSIZ
    };

    // device endpoint-0 control register
    using OTG_FS_DOEPCTL0_Reg = Register<OTG_FS_DEVICE_BASE + 0x300, ReadWrite>;
    struct _OTG_FS_DOEPCTL0 {
        using EPENA = Field<OTG_FS_DOEPCTL0_Reg, 31, 1>; // EPENA
        using EPDIS = Field<OTG_FS_DOEPCTL0_Reg, 30, 1>; // EPDIS
        using SNAK = Field<OTG_FS_DOEPCTL0_Reg, 27, 1>; // SNAK
        using CNAK = Field<OTG_FS_DOEPCTL0_Reg, 26, 1>; // CNAK
        using Stall = Field<OTG_FS_DOEPCTL0_Reg, 21, 1>; // Stall
        using SNPM = Field<OTG_FS_DOEPCTL0_Reg, 20, 1>; // SNPM
        using EPTYP = Field<OTG_FS_DOEPCTL0_Reg, 18, 2>; // EPTYP
        using NAKSTS = Field<OTG_FS_DOEPCTL0_Reg, 17, 1>; // NAKSTS
        using USBAEP = Field<OTG_FS_DOEPCTL0_Reg, 15, 1>; // USBAEP
        using MPSIZ = Field<OTG_FS_DOEPCTL0_Reg, 0, 2>; // MPSIZ
    };

    // device endpoint-1 control register
    using OTG_FS_DOEPCTL1_Reg = Register<OTG_FS_DEVICE_BASE + 0x320, ReadWrite>;
    struct _OTG_FS_DOEPCTL1 {
        using EPENA = Field<OTG_FS_DOEPCTL1_Reg, 31, 1>; // EPENA
        using EPDIS = Field<OTG_FS_DOEPCTL1_Reg, 30, 1>; // EPDIS
        using SODDFRM = Field<OTG_FS_DOEPCTL1_Reg, 29, 1>; // SODDFRM
        using SD0PID_SEVNFRM = Field<OTG_FS_DOEPCTL1_Reg, 28, 1>; // SD0PID/SEVNFRM
        using SNAK = Field<OTG_FS_DOEPCTL1_Reg, 27, 1>; // SNAK
        using CNAK = Field<OTG_FS_DOEPCTL1_Reg, 26, 1>; // CNAK
        using Stall = Field<OTG_FS_DOEPCTL1_Reg, 21, 1>; // Stall
        using SNPM = Field<OTG_FS_DOEPCTL1_Reg, 20, 1>; // SNPM
        using EPTYP = Field<OTG_FS_DOEPCTL1_Reg, 18, 2>; // EPTYP
        using NAKSTS = Field<OTG_FS_DOEPCTL1_Reg, 17, 1>; // NAKSTS
        using EONUM_DPID = Field<OTG_FS_DOEPCTL1_Reg, 16, 1>; // EONUM/DPID
        using USBAEP = Field<OTG_FS_DOEPCTL1_Reg, 15, 1>; // USBAEP
        using MPSIZ = Field<OTG_FS_DOEPCTL1_Reg, 0, 11>; // MPSIZ
    };

    // device endpoint-2 control register
    using OTG_FS_DOEPCTL2_Reg = Register<OTG_FS_DEVICE_BASE + 0x340, ReadWrite>;
    struct _OTG_FS_DOEPCTL2 {
        using EPENA = Field<OTG_FS_DOEPCTL2_Reg, 31, 1>; // EPENA
        using EPDIS = Field<OTG_FS_DOEPCTL2_Reg, 30, 1>; // EPDIS
        using SODDFRM = Field<OTG_FS_DOEPCTL2_Reg, 29, 1>; // SODDFRM
        using SD0PID_SEVNFRM = Field<OTG_FS_DOEPCTL2_Reg, 28, 1>; // SD0PID/SEVNFRM
        using SNAK = Field<OTG_FS_DOEPCTL2_Reg, 27, 1>; // SNAK
        using CNAK = Field<OTG_FS_DOEPCTL2_Reg, 26, 1>; // CNAK
        using Stall = Field<OTG_FS_DOEPCTL2_Reg, 21, 1>; // Stall
        using SNPM = Field<OTG_FS_DOEPCTL2_Reg, 20, 1>; // SNPM
        using EPTYP = Field<OTG_FS_DOEPCTL2_Reg, 18, 2>; // EPTYP
        using NAKSTS = Field<OTG_FS_DOEPCTL2_Reg, 17, 1>; // NAKSTS
        using EONUM_DPID = Field<OTG_FS_DOEPCTL2_Reg, 16, 1>; // EONUM/DPID
        using USBAEP = Field<OTG_FS_DOEPCTL2_Reg, 15, 1>; // USBAEP
        using MPSIZ = Field<OTG_FS_DOEPCTL2_Reg, 0, 11>; // MPSIZ
    };

    // device endpoint-3 control register
    using OTG_FS_DOEPCTL3_Reg = Register<OTG_FS_DEVICE_BASE + 0x360, ReadWrite>;
    struct _OTG_FS_DOEPCTL3 {
        using EPENA = Field<OTG_FS_DOEPCTL3_Reg, 31, 1>; // EPENA
        using EPDIS = Field<OTG_FS_DOEPCTL3_Reg, 30, 1>; // EPDIS
        using SODDFRM = Field<OTG_FS_DOEPCTL3_Reg, 29, 1>; // SODDFRM
        using SD0PID_SEVNFRM = Field<OTG_FS_DOEPCTL3_Reg, 28, 1>; // SD0PID/SEVNFRM
        using SNAK = Field<OTG_FS_DOEPCTL3_Reg, 27, 1>; // SNAK
        using CNAK = Field<OTG_FS_DOEPCTL3_Reg, 26, 1>; // CNAK
        using Stall = Field<OTG_FS_DOEPCTL3_Reg, 21, 1>; // Stall
        using SNPM = Field<OTG_FS_DOEPCTL3_Reg, 20, 1>; // SNPM
        using EPTYP = Field<OTG_FS_DOEPCTL3_Reg, 18, 2>; // EPTYP
        using NAKSTS = Field<OTG_FS_DOEPCTL3_Reg, 17, 1>; // NAKSTS
        using EONUM_DPID = Field<OTG_FS_DOEPCTL3_Reg, 16, 1>; // EONUM/DPID
        using USBAEP = Field<OTG_FS_DOEPCTL3_Reg, 15, 1>; // USBAEP
        using MPSIZ = Field<OTG_FS_DOEPCTL3_Reg, 0, 11>; // MPSIZ
    };

    // device endpoint-x interrupt register
    using OTG_FS_DIEPINT0_Reg = Register<OTG_FS_DEVICE_BASE + 0x108, ReadWrite>;
    struct _OTG_FS_DIEPINT0 {
        using TXFE = Field<OTG_FS_DIEPINT0_Reg, 7, 1>; // TXFE
        using INEPNE = Field<OTG_FS_DIEPINT0_Reg, 6, 1>; // INEPNE
        using ITTXFE = Field<OTG_FS_DIEPINT0_Reg, 4, 1>; // ITTXFE
        using TOC = Field<OTG_FS_DIEPINT0_Reg, 3, 1>; // TOC
        using EPDISD = Field<OTG_FS_DIEPINT0_Reg, 1, 1>; // EPDISD
        using XFRC = Field<OTG_FS_DIEPINT0_Reg, 0, 1>; // XFRC
    };

    // device endpoint-1 interrupt register
    using OTG_FS_DIEPINT1_Reg = Register<OTG_FS_DEVICE_BASE + 0x128, ReadWrite>;
    struct _OTG_FS_DIEPINT1 {
        using TXFE = Field<OTG_FS_DIEPINT1_Reg, 7, 1>; // TXFE
        using INEPNE = Field<OTG_FS_DIEPINT1_Reg, 6, 1>; // INEPNE
        using ITTXFE = Field<OTG_FS_DIEPINT1_Reg, 4, 1>; // ITTXFE
        using TOC = Field<OTG_FS_DIEPINT1_Reg, 3, 1>; // TOC
        using EPDISD = Field<OTG_FS_DIEPINT1_Reg, 1, 1>; // EPDISD
        using XFRC = Field<OTG_FS_DIEPINT1_Reg, 0, 1>; // XFRC
    };

    // device endpoint-2 interrupt register
    using OTG_FS_DIEPINT2_Reg = Register<OTG_FS_DEVICE_BASE + 0x148, ReadWrite>;
    struct _OTG_FS_DIEPINT2 {
        using TXFE = Field<OTG_FS_DIEPINT2_Reg, 7, 1>; // TXFE
        using INEPNE = Field<OTG_FS_DIEPINT2_Reg, 6, 1>; // INEPNE
        using ITTXFE = Field<OTG_FS_DIEPINT2_Reg, 4, 1>; // ITTXFE
        using TOC = Field<OTG_FS_DIEPINT2_Reg, 3, 1>; // TOC
        using EPDISD = Field<OTG_FS_DIEPINT2_Reg, 1, 1>; // EPDISD
        using XFRC = Field<OTG_FS_DIEPINT2_Reg, 0, 1>; // XFRC
    };

    // device endpoint-3 interrupt register
    using OTG_FS_DIEPINT3_Reg = Register<OTG_FS_DEVICE_BASE + 0x168, ReadWrite>;
    struct _OTG_FS_DIEPINT3 {
        using TXFE = Field<OTG_FS_DIEPINT3_Reg, 7, 1>; // TXFE
        using INEPNE = Field<OTG_FS_DIEPINT3_Reg, 6, 1>; // INEPNE
        using ITTXFE = Field<OTG_FS_DIEPINT3_Reg, 4, 1>; // ITTXFE
        using TOC = Field<OTG_FS_DIEPINT3_Reg, 3, 1>; // TOC
        using EPDISD = Field<OTG_FS_DIEPINT3_Reg, 1, 1>; // EPDISD
        using XFRC = Field<OTG_FS_DIEPINT3_Reg, 0, 1>; // XFRC
    };

    // device endpoint-0 interrupt register
    using OTG_FS_DOEPINT0_Reg = Register<OTG_FS_DEVICE_BASE + 0x308, ReadWrite>;
    struct _OTG_FS_DOEPINT0 {
        using B2BSTUP = Field<OTG_FS_DOEPINT0_Reg, 6, 1>; // B2BSTUP
        using OTEPDIS = Field<OTG_FS_DOEPINT0_Reg, 4, 1>; // OTEPDIS
        using STUP = Field<OTG_FS_DOEPINT0_Reg, 3, 1>; // STUP
        using EPDISD = Field<OTG_FS_DOEPINT0_Reg, 1, 1>; // EPDISD
        using XFRC = Field<OTG_FS_DOEPINT0_Reg, 0, 1>; // XFRC
    };

    // device endpoint-1 interrupt register
    using OTG_FS_DOEPINT1_Reg = Register<OTG_FS_DEVICE_BASE + 0x328, ReadWrite>;
    struct _OTG_FS_DOEPINT1 {
        using B2BSTUP = Field<OTG_FS_DOEPINT1_Reg, 6, 1>; // B2BSTUP
        using OTEPDIS = Field<OTG_FS_DOEPINT1_Reg, 4, 1>; // OTEPDIS
        using STUP = Field<OTG_FS_DOEPINT1_Reg, 3, 1>; // STUP
        using EPDISD = Field<OTG_FS_DOEPINT1_Reg, 1, 1>; // EPDISD
        using XFRC = Field<OTG_FS_DOEPINT1_Reg, 0, 1>; // XFRC
    };

    // device endpoint-2 interrupt register
    using OTG_FS_DOEPINT2_Reg = Register<OTG_FS_DEVICE_BASE + 0x348, ReadWrite>;
    struct _OTG_FS_DOEPINT2 {
        using B2BSTUP = Field<OTG_FS_DOEPINT2_Reg, 6, 1>; // B2BSTUP
        using OTEPDIS = Field<OTG_FS_DOEPINT2_Reg, 4, 1>; // OTEPDIS
        using STUP = Field<OTG_FS_DOEPINT2_Reg, 3, 1>; // STUP
        using EPDISD = Field<OTG_FS_DOEPINT2_Reg, 1, 1>; // EPDISD
        using XFRC = Field<OTG_FS_DOEPINT2_Reg, 0, 1>; // XFRC
    };

    // device endpoint-3 interrupt register
    using OTG_FS_DOEPINT3_Reg = Register<OTG_FS_DEVICE_BASE + 0x368, ReadWrite>;
    struct _OTG_FS_DOEPINT3 {
        using B2BSTUP = Field<OTG_FS_DOEPINT3_Reg, 6, 1>; // B2BSTUP
        using OTEPDIS = Field<OTG_FS_DOEPINT3_Reg, 4, 1>; // OTEPDIS
        using STUP = Field<OTG_FS_DOEPINT3_Reg, 3, 1>; // STUP
        using EPDISD = Field<OTG_FS_DOEPINT3_Reg, 1, 1>; // EPDISD
        using XFRC = Field<OTG_FS_DOEPINT3_Reg, 0, 1>; // XFRC
    };

    // device endpoint-0 transfer size register
    using OTG_FS_DIEPTSIZ0_Reg = Register<OTG_FS_DEVICE_BASE + 0x110, ReadWrite>;
    struct _OTG_FS_DIEPTSIZ0 {
        using PKTCNT = Field<OTG_FS_DIEPTSIZ0_Reg, 19, 2>; // Packet count
        using XFRSIZ = Field<OTG_FS_DIEPTSIZ0_Reg, 0, 7>; // Transfer size
    };

    // device OUT endpoint-0 transfer size register
    using OTG_FS_DOEPTSIZ0_Reg = Register<OTG_FS_DEVICE_BASE + 0x310, ReadWrite>;
    struct _OTG_FS_DOEPTSIZ0 {
        using STUPCNT = Field<OTG_FS_DOEPTSIZ0_Reg, 29, 2>; // SETUP packet count
        using PKTCNT = Field<OTG_FS_DOEPTSIZ0_Reg, 19, 1>; // Packet count
        using XFRSIZ = Field<OTG_FS_DOEPTSIZ0_Reg, 0, 7>; // Transfer size
    };

    // device endpoint-1 transfer size register
    using OTG_FS_DIEPTSIZ1_Reg = Register<OTG_FS_DEVICE_BASE + 0x130, ReadWrite>;
    struct _OTG_FS_DIEPTSIZ1 {
        using MCNT = Field<OTG_FS_DIEPTSIZ1_Reg, 29, 2>; // Multi count
        using PKTCNT = Field<OTG_FS_DIEPTSIZ1_Reg, 19, 10>; // Packet count
        using XFRSIZ = Field<OTG_FS_DIEPTSIZ1_Reg, 0, 19>; // Transfer size
    };

    // device endpoint-2 transfer size register
    using OTG_FS_DIEPTSIZ2_Reg = Register<OTG_FS_DEVICE_BASE + 0x150, ReadWrite>;
    struct _OTG_FS_DIEPTSIZ2 {
        using MCNT = Field<OTG_FS_DIEPTSIZ2_Reg, 29, 2>; // Multi count
        using PKTCNT = Field<OTG_FS_DIEPTSIZ2_Reg, 19, 10>; // Packet count
        using XFRSIZ = Field<OTG_FS_DIEPTSIZ2_Reg, 0, 19>; // Transfer size
    };

    // device endpoint-3 transfer size register
    using OTG_FS_DIEPTSIZ3_Reg = Register<OTG_FS_DEVICE_BASE + 0x170, ReadWrite>;
    struct _OTG_FS_DIEPTSIZ3 {
        using MCNT = Field<OTG_FS_DIEPTSIZ3_Reg, 29, 2>; // Multi count
        using PKTCNT = Field<OTG_FS_DIEPTSIZ3_Reg, 19, 10>; // Packet count
        using XFRSIZ = Field<OTG_FS_DIEPTSIZ3_Reg, 0, 19>; // Transfer size
    };

    // OTG_FS device IN endpoint transmit FIFO status register
    using OTG_FS_DTXFSTS0_Reg = Register<OTG_FS_DEVICE_BASE + 0x118, ReadOnly>;
    struct _OTG_FS_DTXFSTS0 {
        using INEPTFSAV = Field<OTG_FS_DTXFSTS0_Reg, 0, 16>; // IN endpoint TxFIFO space available
    };

    // OTG_FS device IN endpoint transmit FIFO status register
    using OTG_FS_DTXFSTS1_Reg = Register<OTG_FS_DEVICE_BASE + 0x138, ReadOnly>;
    struct _OTG_FS_DTXFSTS1 {
        using INEPTFSAV = Field<OTG_FS_DTXFSTS1_Reg, 0, 16>; // IN endpoint TxFIFO space available
    };

    // OTG_FS device IN endpoint transmit FIFO status register
    using OTG_FS_DTXFSTS2_Reg = Register<OTG_FS_DEVICE_BASE + 0x158, ReadOnly>;
    struct _OTG_FS_DTXFSTS2 {
        using INEPTFSAV = Field<OTG_FS_DTXFSTS2_Reg, 0, 16>; // IN endpoint TxFIFO space available
    };

    // OTG_FS device IN endpoint transmit FIFO status register
    using OTG_FS_DTXFSTS3_Reg = Register<OTG_FS_DEVICE_BASE + 0x178, ReadOnly>;
    struct _OTG_FS_DTXFSTS3 {
        using INEPTFSAV = Field<OTG_FS_DTXFSTS3_Reg, 0, 16>; // IN endpoint TxFIFO space available
    };

    // device OUT endpoint-1 transfer size register
    using OTG_FS_DOEPTSIZ1_Reg = Register<OTG_FS_DEVICE_BASE + 0x330, ReadWrite>;
    struct _OTG_FS_DOEPTSIZ1 {
        using RXDPID_STUPCNT = Field<OTG_FS_DOEPTSIZ1_Reg, 29, 2>; // Received data PID/SETUP packet count
        using PKTCNT = Field<OTG_FS_DOEPTSIZ1_Reg, 19, 10>; // Packet count
        using XFRSIZ = Field<OTG_FS_DOEPTSIZ1_Reg, 0, 19>; // Transfer size
    };

    // device OUT endpoint-2 transfer size register
    using OTG_FS_DOEPTSIZ2_Reg = Register<OTG_FS_DEVICE_BASE + 0x350, ReadWrite>;
    struct _OTG_FS_DOEPTSIZ2 {
        using RXDPID_STUPCNT = Field<OTG_FS_DOEPTSIZ2_Reg, 29, 2>; // Received data PID/SETUP packet count
        using PKTCNT = Field<OTG_FS_DOEPTSIZ2_Reg, 19, 10>; // Packet count
        using XFRSIZ = Field<OTG_FS_DOEPTSIZ2_Reg, 0, 19>; // Transfer size
    };

    // device OUT endpoint-3 transfer size register
    using OTG_FS_DOEPTSIZ3_Reg = Register<OTG_FS_DEVICE_BASE + 0x370, ReadWrite>;
    struct _OTG_FS_DOEPTSIZ3 {
        using RXDPID_STUPCNT = Field<OTG_FS_DOEPTSIZ3_Reg, 29, 2>; // Received data PID/SETUP packet count
        using PKTCNT = Field<OTG_FS_DOEPTSIZ3_Reg, 19, 10>; // Packet count
        using XFRSIZ = Field<OTG_FS_DOEPTSIZ3_Reg, 0, 19>; // Transfer size
    };

    // OTG device endpoint-4 control register
    using OTG_FS_DIEPCTL4_Reg = Register<OTG_FS_DEVICE_BASE + 0x180, ReadWrite>;
    struct _OTG_FS_DIEPCTL4 {
        using EPENA = Field<OTG_FS_DIEPCTL4_Reg, 31, 1>; // EPENA
        using EPDIS = Field<OTG_FS_DIEPCTL4_Reg, 30, 1>; // EPDIS
        using SODDFRM = Field<OTG_FS_DIEPCTL4_Reg, 29, 1>; // SODDFRM
        using SD0PID_SEVNFRM = Field<OTG_FS_DIEPCTL4_Reg, 28, 1>; // SD0PID/SEVNFRM
        using SNAK = Field<OTG_FS_DIEPCTL4_Reg, 27, 1>; // SNAK
        using CNAK = Field<OTG_FS_DIEPCTL4_Reg, 26, 1>; // CNAK
        using TXFNUM = Field<OTG_FS_DIEPCTL4_Reg, 22, 4>; // TXFNUM
        using Stall = Field<OTG_FS_DIEPCTL4_Reg, 21, 1>; // Stall
        using EPTYP = Field<OTG_FS_DIEPCTL4_Reg, 18, 2>; // EPTYP
        using NAKSTS = Field<OTG_FS_DIEPCTL4_Reg, 17, 1>; // NAKSTS
        using EONUM_DPID = Field<OTG_FS_DIEPCTL4_Reg, 16, 1>; // EONUM/DPID
        using USBAEP = Field<OTG_FS_DIEPCTL4_Reg, 15, 1>; // USBAEP
        using MPSIZ = Field<OTG_FS_DIEPCTL4_Reg, 0, 11>; // MPSIZ
    };

    // device endpoint-4 interrupt register
    using OTG_FS_DIEPINT4_Reg = Register<OTG_FS_DEVICE_BASE + 0x188, ReadWrite>;
    struct _OTG_FS_DIEPINT4 {
        using TXFE = Field<OTG_FS_DIEPINT4_Reg, 7, 1>; // TXFE
        using INEPNE = Field<OTG_FS_DIEPINT4_Reg, 6, 1>; // INEPNE
        using ITTXFE = Field<OTG_FS_DIEPINT4_Reg, 4, 1>; // ITTXFE
        using TOC = Field<OTG_FS_DIEPINT4_Reg, 3, 1>; // TOC
        using EPDISD = Field<OTG_FS_DIEPINT4_Reg, 1, 1>; // EPDISD
        using XFRC = Field<OTG_FS_DIEPINT4_Reg, 0, 1>; // XFRC
    };

    // device endpoint-4 transfer size register
    using OTG_FS_DIEPTSIZ4_Reg = Register<OTG_FS_DEVICE_BASE + 0x194, ReadWrite>;
    struct _OTG_FS_DIEPTSIZ4 {
        using MCNT = Field<OTG_FS_DIEPTSIZ4_Reg, 29, 2>; // Multi count
        using PKTCNT = Field<OTG_FS_DIEPTSIZ4_Reg, 19, 10>; // Packet count
        using XFRSIZ = Field<OTG_FS_DIEPTSIZ4_Reg, 0, 19>; // Transfer size
    };

    // OTG_FS device IN endpoint transmit FIFO status register
    using OTG_FS_DTXFSTS4_Reg = Register<OTG_FS_DEVICE_BASE + 0x19C, ReadWrite>;
    struct _OTG_FS_DTXFSTS4 {
        using INEPTFSAV = Field<OTG_FS_DTXFSTS4_Reg, 0, 16>; // IN endpoint TxFIFO space available
    };

    // OTG device endpoint-5 control register
    using OTG_FS_DIEPCTL5_Reg = Register<OTG_FS_DEVICE_BASE + 0x1A0, ReadWrite>;
    struct _OTG_FS_DIEPCTL5 {
        using EPENA = Field<OTG_FS_DIEPCTL5_Reg, 31, 1>; // EPENA
        using EPDIS = Field<OTG_FS_DIEPCTL5_Reg, 30, 1>; // EPDIS
        using SODDFRM = Field<OTG_FS_DIEPCTL5_Reg, 29, 1>; // SODDFRM
        using SD0PID_SEVNFRM = Field<OTG_FS_DIEPCTL5_Reg, 28, 1>; // SD0PID/SEVNFRM
        using SNAK = Field<OTG_FS_DIEPCTL5_Reg, 27, 1>; // SNAK
        using CNAK = Field<OTG_FS_DIEPCTL5_Reg, 26, 1>; // CNAK
        using TXFNUM = Field<OTG_FS_DIEPCTL5_Reg, 22, 4>; // TXFNUM
        using Stall = Field<OTG_FS_DIEPCTL5_Reg, 21, 1>; // Stall
        using EPTYP = Field<OTG_FS_DIEPCTL5_Reg, 18, 2>; // EPTYP
        using NAKSTS = Field<OTG_FS_DIEPCTL5_Reg, 17, 1>; // NAKSTS
        using EONUM_DPID = Field<OTG_FS_DIEPCTL5_Reg, 16, 1>; // EONUM/DPID
        using USBAEP = Field<OTG_FS_DIEPCTL5_Reg, 15, 1>; // USBAEP
        using MPSIZ = Field<OTG_FS_DIEPCTL5_Reg, 0, 11>; // MPSIZ
    };

    // device endpoint-5 interrupt register
    using OTG_FS_DIEPINT5_Reg = Register<OTG_FS_DEVICE_BASE + 0x1A8, ReadWrite>;
    struct _OTG_FS_DIEPINT5 {
        using TXFE = Field<OTG_FS_DIEPINT5_Reg, 7, 1>; // TXFE
        using INEPNE = Field<OTG_FS_DIEPINT5_Reg, 6, 1>; // INEPNE
        using ITTXFE = Field<OTG_FS_DIEPINT5_Reg, 4, 1>; // ITTXFE
        using TOC = Field<OTG_FS_DIEPINT5_Reg, 3, 1>; // TOC
        using EPDISD = Field<OTG_FS_DIEPINT5_Reg, 1, 1>; // EPDISD
        using XFRC = Field<OTG_FS_DIEPINT5_Reg, 0, 1>; // XFRC
    };

    // device endpoint-5 transfer size register
    using OTG_FS_DIEPTSIZ55_Reg = Register<OTG_FS_DEVICE_BASE + 0x1B0, ReadWrite>;
    struct _OTG_FS_DIEPTSIZ55 {
        using MCNT = Field<OTG_FS_DIEPTSIZ55_Reg, 29, 2>; // Multi count
        using PKTCNT = Field<OTG_FS_DIEPTSIZ55_Reg, 19, 10>; // Packet count
        using XFRSIZ = Field<OTG_FS_DIEPTSIZ55_Reg, 0, 19>; // Transfer size
    };

    // OTG_FS device IN endpoint transmit FIFO status register
    using OTG_FS_DTXFSTS55_Reg = Register<OTG_FS_DEVICE_BASE + 0x1B8, ReadWrite>;
    struct _OTG_FS_DTXFSTS55 {
        using INEPTFSAV = Field<OTG_FS_DTXFSTS55_Reg, 0, 16>; // IN endpoint TxFIFO space available
    };

    // device endpoint-4 control register
    using OTG_FS_DOEPCTL4_Reg = Register<OTG_FS_DEVICE_BASE + 0x378, ReadWrite>;
    struct _OTG_FS_DOEPCTL4 {
        using EPENA = Field<OTG_FS_DOEPCTL4_Reg, 31, 1>; // EPENA
        using EPDIS = Field<OTG_FS_DOEPCTL4_Reg, 30, 1>; // EPDIS
        using SODDFRM = Field<OTG_FS_DOEPCTL4_Reg, 29, 1>; // SODDFRM
        using SD0PID_SEVNFRM = Field<OTG_FS_DOEPCTL4_Reg, 28, 1>; // SD0PID/SEVNFRM
        using SNAK = Field<OTG_FS_DOEPCTL4_Reg, 27, 1>; // SNAK
        using CNAK = Field<OTG_FS_DOEPCTL4_Reg, 26, 1>; // CNAK
        using Stall = Field<OTG_FS_DOEPCTL4_Reg, 21, 1>; // Stall
        using SNPM = Field<OTG_FS_DOEPCTL4_Reg, 20, 1>; // SNPM
        using EPTYP = Field<OTG_FS_DOEPCTL4_Reg, 18, 2>; // EPTYP
        using NAKSTS = Field<OTG_FS_DOEPCTL4_Reg, 17, 1>; // NAKSTS
        using EONUM_DPID = Field<OTG_FS_DOEPCTL4_Reg, 16, 1>; // EONUM/DPID
        using USBAEP = Field<OTG_FS_DOEPCTL4_Reg, 15, 1>; // USBAEP
        using MPSIZ = Field<OTG_FS_DOEPCTL4_Reg, 0, 11>; // MPSIZ
    };

    // device endpoint-4 interrupt register
    using OTG_FS_DOEPINT4_Reg = Register<OTG_FS_DEVICE_BASE + 0x380, ReadWrite>;
    struct _OTG_FS_DOEPINT4 {
        using B2BSTUP = Field<OTG_FS_DOEPINT4_Reg, 6, 1>; // B2BSTUP
        using OTEPDIS = Field<OTG_FS_DOEPINT4_Reg, 4, 1>; // OTEPDIS
        using STUP = Field<OTG_FS_DOEPINT4_Reg, 3, 1>; // STUP
        using EPDISD = Field<OTG_FS_DOEPINT4_Reg, 1, 1>; // EPDISD
        using XFRC = Field<OTG_FS_DOEPINT4_Reg, 0, 1>; // XFRC
    };

    // device OUT endpoint-4 transfer size register
    using OTG_FS_DOEPTSIZ4_Reg = Register<OTG_FS_DEVICE_BASE + 0x388, ReadWrite>;
    struct _OTG_FS_DOEPTSIZ4 {
        using RXDPID_STUPCNT = Field<OTG_FS_DOEPTSIZ4_Reg, 29, 2>; // Received data PID/SETUP packet count
        using PKTCNT = Field<OTG_FS_DOEPTSIZ4_Reg, 19, 10>; // Packet count
        using XFRSIZ = Field<OTG_FS_DOEPTSIZ4_Reg, 0, 19>; // Transfer size
    };

    // device endpoint-5 control register
    using OTG_FS_DOEPCTL5_Reg = Register<OTG_FS_DEVICE_BASE + 0x390, ReadWrite>;
    struct _OTG_FS_DOEPCTL5 {
        using EPENA = Field<OTG_FS_DOEPCTL5_Reg, 31, 1>; // EPENA
        using EPDIS = Field<OTG_FS_DOEPCTL5_Reg, 30, 1>; // EPDIS
        using SODDFRM = Field<OTG_FS_DOEPCTL5_Reg, 29, 1>; // SODDFRM
        using SD0PID_SEVNFRM = Field<OTG_FS_DOEPCTL5_Reg, 28, 1>; // SD0PID/SEVNFRM
        using SNAK = Field<OTG_FS_DOEPCTL5_Reg, 27, 1>; // SNAK
        using CNAK = Field<OTG_FS_DOEPCTL5_Reg, 26, 1>; // CNAK
        using Stall = Field<OTG_FS_DOEPCTL5_Reg, 21, 1>; // Stall
        using SNPM = Field<OTG_FS_DOEPCTL5_Reg, 20, 1>; // SNPM
        using EPTYP = Field<OTG_FS_DOEPCTL5_Reg, 18, 2>; // EPTYP
        using NAKSTS = Field<OTG_FS_DOEPCTL5_Reg, 17, 1>; // NAKSTS
        using EONUM_DPID = Field<OTG_FS_DOEPCTL5_Reg, 16, 1>; // EONUM/DPID
        using USBAEP = Field<OTG_FS_DOEPCTL5_Reg, 15, 1>; // USBAEP
        using MPSIZ = Field<OTG_FS_DOEPCTL5_Reg, 0, 11>; // MPSIZ
    };

    // device endpoint-5 interrupt register
    using OTG_FS_DOEPINT5_Reg = Register<OTG_FS_DEVICE_BASE + 0x398, ReadWrite>;
    struct _OTG_FS_DOEPINT5 {
        using B2BSTUP = Field<OTG_FS_DOEPINT5_Reg, 6, 1>; // B2BSTUP
        using OTEPDIS = Field<OTG_FS_DOEPINT5_Reg, 4, 1>; // OTEPDIS
        using STUP = Field<OTG_FS_DOEPINT5_Reg, 3, 1>; // STUP
        using EPDISD = Field<OTG_FS_DOEPINT5_Reg, 1, 1>; // EPDISD
        using XFRC = Field<OTG_FS_DOEPINT5_Reg, 0, 1>; // XFRC
    };

    // device OUT endpoint-5 transfer size register
    using OTG_FS_DOEPTSIZ5_Reg = Register<OTG_FS_DEVICE_BASE + 0x3A0, ReadWrite>;
    struct _OTG_FS_DOEPTSIZ5 {
        using RXDPID_STUPCNT = Field<OTG_FS_DOEPTSIZ5_Reg, 29, 2>; // Received data PID/SETUP packet count
        using PKTCNT = Field<OTG_FS_DOEPTSIZ5_Reg, 19, 10>; // Packet count
        using XFRSIZ = Field<OTG_FS_DOEPTSIZ5_Reg, 0, 19>; // Transfer size
    };

} // namespace OTG_FS_DEVICE

// --------------------------------------------
// OTG_FS_PWRCLK: USB on the go full speed
// Base address: 0x50000E00
// --------------------------------------------

namespace OTG_FS_PWRCLK {
    static constexpr uint32_t OTG_FS_PWRCLK_BASE = 0x50000E00;

    // OTG_FS power and clock gating control register (OTG_FS_PCGCCTL)
    using OTG_FS_PCGCCTL_Reg = Register<OTG_FS_PWRCLK_BASE + 0x0, ReadWrite>;
    struct _OTG_FS_PCGCCTL {
        using STPPCLK = Field<OTG_FS_PCGCCTL_Reg, 0, 1>; // Stop PHY clock
        using GATEHCLK = Field<OTG_FS_PCGCCTL_Reg, 1, 1>; // Gate HCLK
        using PHYSUSP = Field<OTG_FS_PCGCCTL_Reg, 4, 1>; // PHY Suspended
    };

} // namespace OTG_FS_PWRCLK

// --------------------------------------------
// OTG_HS_HOST: USB on the go high speed
// Base address: 0x40040400
// --------------------------------------------

namespace OTG_HS_HOST {
    static constexpr uint32_t OTG_HS_HOST_BASE = 0x40040400;

    // OTG_HS host configuration register
    using OTG_HS_HCFG_Reg = Register<OTG_HS_HOST_BASE + 0x0, ReadWrite>;
    struct _OTG_HS_HCFG {
        using FSLSPCS = Field<OTG_HS_HCFG_Reg, 0, 2>; // FS/LS PHY clock select
        using FSLSS = Field<OTG_HS_HCFG_Reg, 2, 1>; // FS- and LS-only support
    };

    // OTG_HS Host frame interval register
    using OTG_HS_HFIR_Reg = Register<OTG_HS_HOST_BASE + 0x4, ReadWrite>;
    struct _OTG_HS_HFIR {
        using FRIVL = Field<OTG_HS_HFIR_Reg, 0, 16>; // Frame interval
    };

    // OTG_HS host frame number/frame time remaining register
    using OTG_HS_HFNUM_Reg = Register<OTG_HS_HOST_BASE + 0x8, ReadOnly>;
    struct _OTG_HS_HFNUM {
        using FRNUM = Field<OTG_HS_HFNUM_Reg, 0, 16>; // Frame number
        using FTREM = Field<OTG_HS_HFNUM_Reg, 16, 16>; // Frame time remaining
    };

    // OTG_HS_Host periodic transmit FIFO/queue status register
    using OTG_HS_HPTXSTS_Reg = Register<OTG_HS_HOST_BASE + 0x10, ReadWrite>;
    struct _OTG_HS_HPTXSTS {
        using PTXFSAVL = Field<OTG_HS_HPTXSTS_Reg, 0, 16>; // Periodic transmit data FIFO space available
        using PTXQSAV = Field<OTG_HS_HPTXSTS_Reg, 16, 8>; // Periodic transmit request queue space available
        using PTXQTOP = Field<OTG_HS_HPTXSTS_Reg, 24, 8>; // Top of the periodic transmit request queue
    };

    // OTG_HS Host all channels interrupt register
    using OTG_HS_HAINT_Reg = Register<OTG_HS_HOST_BASE + 0x14, ReadOnly>;
    struct _OTG_HS_HAINT {
        using HAINT = Field<OTG_HS_HAINT_Reg, 0, 16>; // Channel interrupts
    };

    // OTG_HS host all channels interrupt mask register
    using OTG_HS_HAINTMSK_Reg = Register<OTG_HS_HOST_BASE + 0x18, ReadWrite>;
    struct _OTG_HS_HAINTMSK {
        using HAINTM = Field<OTG_HS_HAINTMSK_Reg, 0, 16>; // Channel interrupt mask
    };

    // OTG_HS host port control and status register
    using OTG_HS_HPRT_Reg = Register<OTG_HS_HOST_BASE + 0x40, ReadWrite>;
    struct _OTG_HS_HPRT {
        using PCSTS = Field<OTG_HS_HPRT_Reg, 0, 1>; // Port connect status
        using PCDET = Field<OTG_HS_HPRT_Reg, 1, 1>; // Port connect detected
        using PENA = Field<OTG_HS_HPRT_Reg, 2, 1>; // Port enable
        using PENCHNG = Field<OTG_HS_HPRT_Reg, 3, 1>; // Port enable/disable change
        using POCA = Field<OTG_HS_HPRT_Reg, 4, 1>; // Port overcurrent active
        using POCCHNG = Field<OTG_HS_HPRT_Reg, 5, 1>; // Port overcurrent change
        using PRES = Field<OTG_HS_HPRT_Reg, 6, 1>; // Port resume
        using PSUSP = Field<OTG_HS_HPRT_Reg, 7, 1>; // Port suspend
        using PRST = Field<OTG_HS_HPRT_Reg, 8, 1>; // Port reset
        using PLSTS = Field<OTG_HS_HPRT_Reg, 10, 2>; // Port line status
        using PPWR = Field<OTG_HS_HPRT_Reg, 12, 1>; // Port power
        using PTCTL = Field<OTG_HS_HPRT_Reg, 13, 4>; // Port test control
        using PSPD = Field<OTG_HS_HPRT_Reg, 17, 2>; // Port speed
    };

    // OTG_HS host channel-0 characteristics register
    using OTG_HS_HCCHAR0_Reg = Register<OTG_HS_HOST_BASE + 0x100, ReadWrite>;
    struct _OTG_HS_HCCHAR0 {
        using MPSIZ = Field<OTG_HS_HCCHAR0_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_HS_HCCHAR0_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_HS_HCCHAR0_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_HS_HCCHAR0_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_HS_HCCHAR0_Reg, 18, 2>; // Endpoint type
        using MC = Field<OTG_HS_HCCHAR0_Reg, 20, 2>; // Multi Count (MC) / Error Count (EC)
        using DAD = Field<OTG_HS_HCCHAR0_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_HS_HCCHAR0_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_HS_HCCHAR0_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_HS_HCCHAR0_Reg, 31, 1>; // Channel enable
    };

    // OTG_HS host channel-1 characteristics register
    using OTG_HS_HCCHAR1_Reg = Register<OTG_HS_HOST_BASE + 0x120, ReadWrite>;
    struct _OTG_HS_HCCHAR1 {
        using MPSIZ = Field<OTG_HS_HCCHAR1_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_HS_HCCHAR1_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_HS_HCCHAR1_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_HS_HCCHAR1_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_HS_HCCHAR1_Reg, 18, 2>; // Endpoint type
        using MC = Field<OTG_HS_HCCHAR1_Reg, 20, 2>; // Multi Count (MC) / Error Count (EC)
        using DAD = Field<OTG_HS_HCCHAR1_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_HS_HCCHAR1_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_HS_HCCHAR1_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_HS_HCCHAR1_Reg, 31, 1>; // Channel enable
    };

    // OTG_HS host channel-2 characteristics register
    using OTG_HS_HCCHAR2_Reg = Register<OTG_HS_HOST_BASE + 0x140, ReadWrite>;
    struct _OTG_HS_HCCHAR2 {
        using MPSIZ = Field<OTG_HS_HCCHAR2_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_HS_HCCHAR2_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_HS_HCCHAR2_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_HS_HCCHAR2_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_HS_HCCHAR2_Reg, 18, 2>; // Endpoint type
        using MC = Field<OTG_HS_HCCHAR2_Reg, 20, 2>; // Multi Count (MC) / Error Count (EC)
        using DAD = Field<OTG_HS_HCCHAR2_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_HS_HCCHAR2_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_HS_HCCHAR2_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_HS_HCCHAR2_Reg, 31, 1>; // Channel enable
    };

    // OTG_HS host channel-3 characteristics register
    using OTG_HS_HCCHAR3_Reg = Register<OTG_HS_HOST_BASE + 0x160, ReadWrite>;
    struct _OTG_HS_HCCHAR3 {
        using MPSIZ = Field<OTG_HS_HCCHAR3_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_HS_HCCHAR3_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_HS_HCCHAR3_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_HS_HCCHAR3_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_HS_HCCHAR3_Reg, 18, 2>; // Endpoint type
        using MC = Field<OTG_HS_HCCHAR3_Reg, 20, 2>; // Multi Count (MC) / Error Count (EC)
        using DAD = Field<OTG_HS_HCCHAR3_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_HS_HCCHAR3_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_HS_HCCHAR3_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_HS_HCCHAR3_Reg, 31, 1>; // Channel enable
    };

    // OTG_HS host channel-4 characteristics register
    using OTG_HS_HCCHAR4_Reg = Register<OTG_HS_HOST_BASE + 0x180, ReadWrite>;
    struct _OTG_HS_HCCHAR4 {
        using MPSIZ = Field<OTG_HS_HCCHAR4_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_HS_HCCHAR4_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_HS_HCCHAR4_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_HS_HCCHAR4_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_HS_HCCHAR4_Reg, 18, 2>; // Endpoint type
        using MC = Field<OTG_HS_HCCHAR4_Reg, 20, 2>; // Multi Count (MC) / Error Count (EC)
        using DAD = Field<OTG_HS_HCCHAR4_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_HS_HCCHAR4_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_HS_HCCHAR4_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_HS_HCCHAR4_Reg, 31, 1>; // Channel enable
    };

    // OTG_HS host channel-5 characteristics register
    using OTG_HS_HCCHAR5_Reg = Register<OTG_HS_HOST_BASE + 0x1A0, ReadWrite>;
    struct _OTG_HS_HCCHAR5 {
        using MPSIZ = Field<OTG_HS_HCCHAR5_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_HS_HCCHAR5_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_HS_HCCHAR5_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_HS_HCCHAR5_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_HS_HCCHAR5_Reg, 18, 2>; // Endpoint type
        using MC = Field<OTG_HS_HCCHAR5_Reg, 20, 2>; // Multi Count (MC) / Error Count (EC)
        using DAD = Field<OTG_HS_HCCHAR5_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_HS_HCCHAR5_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_HS_HCCHAR5_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_HS_HCCHAR5_Reg, 31, 1>; // Channel enable
    };

    // OTG_HS host channel-6 characteristics register
    using OTG_HS_HCCHAR6_Reg = Register<OTG_HS_HOST_BASE + 0x1C0, ReadWrite>;
    struct _OTG_HS_HCCHAR6 {
        using MPSIZ = Field<OTG_HS_HCCHAR6_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_HS_HCCHAR6_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_HS_HCCHAR6_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_HS_HCCHAR6_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_HS_HCCHAR6_Reg, 18, 2>; // Endpoint type
        using MC = Field<OTG_HS_HCCHAR6_Reg, 20, 2>; // Multi Count (MC) / Error Count (EC)
        using DAD = Field<OTG_HS_HCCHAR6_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_HS_HCCHAR6_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_HS_HCCHAR6_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_HS_HCCHAR6_Reg, 31, 1>; // Channel enable
    };

    // OTG_HS host channel-7 characteristics register
    using OTG_HS_HCCHAR7_Reg = Register<OTG_HS_HOST_BASE + 0x1E0, ReadWrite>;
    struct _OTG_HS_HCCHAR7 {
        using MPSIZ = Field<OTG_HS_HCCHAR7_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_HS_HCCHAR7_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_HS_HCCHAR7_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_HS_HCCHAR7_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_HS_HCCHAR7_Reg, 18, 2>; // Endpoint type
        using MC = Field<OTG_HS_HCCHAR7_Reg, 20, 2>; // Multi Count (MC) / Error Count (EC)
        using DAD = Field<OTG_HS_HCCHAR7_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_HS_HCCHAR7_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_HS_HCCHAR7_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_HS_HCCHAR7_Reg, 31, 1>; // Channel enable
    };

    // OTG_HS host channel-8 characteristics register
    using OTG_HS_HCCHAR8_Reg = Register<OTG_HS_HOST_BASE + 0x200, ReadWrite>;
    struct _OTG_HS_HCCHAR8 {
        using MPSIZ = Field<OTG_HS_HCCHAR8_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_HS_HCCHAR8_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_HS_HCCHAR8_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_HS_HCCHAR8_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_HS_HCCHAR8_Reg, 18, 2>; // Endpoint type
        using MC = Field<OTG_HS_HCCHAR8_Reg, 20, 2>; // Multi Count (MC) / Error Count (EC)
        using DAD = Field<OTG_HS_HCCHAR8_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_HS_HCCHAR8_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_HS_HCCHAR8_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_HS_HCCHAR8_Reg, 31, 1>; // Channel enable
    };

    // OTG_HS host channel-9 characteristics register
    using OTG_HS_HCCHAR9_Reg = Register<OTG_HS_HOST_BASE + 0x220, ReadWrite>;
    struct _OTG_HS_HCCHAR9 {
        using MPSIZ = Field<OTG_HS_HCCHAR9_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_HS_HCCHAR9_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_HS_HCCHAR9_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_HS_HCCHAR9_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_HS_HCCHAR9_Reg, 18, 2>; // Endpoint type
        using MC = Field<OTG_HS_HCCHAR9_Reg, 20, 2>; // Multi Count (MC) / Error Count (EC)
        using DAD = Field<OTG_HS_HCCHAR9_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_HS_HCCHAR9_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_HS_HCCHAR9_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_HS_HCCHAR9_Reg, 31, 1>; // Channel enable
    };

    // OTG_HS host channel-10 characteristics register
    using OTG_HS_HCCHAR10_Reg = Register<OTG_HS_HOST_BASE + 0x240, ReadWrite>;
    struct _OTG_HS_HCCHAR10 {
        using MPSIZ = Field<OTG_HS_HCCHAR10_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_HS_HCCHAR10_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_HS_HCCHAR10_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_HS_HCCHAR10_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_HS_HCCHAR10_Reg, 18, 2>; // Endpoint type
        using MC = Field<OTG_HS_HCCHAR10_Reg, 20, 2>; // Multi Count (MC) / Error Count (EC)
        using DAD = Field<OTG_HS_HCCHAR10_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_HS_HCCHAR10_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_HS_HCCHAR10_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_HS_HCCHAR10_Reg, 31, 1>; // Channel enable
    };

    // OTG_HS host channel-11 characteristics register
    using OTG_HS_HCCHAR11_Reg = Register<OTG_HS_HOST_BASE + 0x260, ReadWrite>;
    struct _OTG_HS_HCCHAR11 {
        using MPSIZ = Field<OTG_HS_HCCHAR11_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_HS_HCCHAR11_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_HS_HCCHAR11_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_HS_HCCHAR11_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_HS_HCCHAR11_Reg, 18, 2>; // Endpoint type
        using MC = Field<OTG_HS_HCCHAR11_Reg, 20, 2>; // Multi Count (MC) / Error Count (EC)
        using DAD = Field<OTG_HS_HCCHAR11_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_HS_HCCHAR11_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_HS_HCCHAR11_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_HS_HCCHAR11_Reg, 31, 1>; // Channel enable
    };

    // OTG_HS host channel-0 split control register
    using OTG_HS_HCSPLT0_Reg = Register<OTG_HS_HOST_BASE + 0x104, ReadWrite>;
    struct _OTG_HS_HCSPLT0 {
        using PRTADDR = Field<OTG_HS_HCSPLT0_Reg, 0, 7>; // Port address
        using HUBADDR = Field<OTG_HS_HCSPLT0_Reg, 7, 7>; // Hub address
        using XACTPOS = Field<OTG_HS_HCSPLT0_Reg, 14, 2>; // XACTPOS
        using COMPLSPLT = Field<OTG_HS_HCSPLT0_Reg, 16, 1>; // Do complete split
        using SPLITEN = Field<OTG_HS_HCSPLT0_Reg, 31, 1>; // Split enable
    };

    // OTG_HS host channel-1 split control register
    using OTG_HS_HCSPLT1_Reg = Register<OTG_HS_HOST_BASE + 0x124, ReadWrite>;
    struct _OTG_HS_HCSPLT1 {
        using PRTADDR = Field<OTG_HS_HCSPLT1_Reg, 0, 7>; // Port address
        using HUBADDR = Field<OTG_HS_HCSPLT1_Reg, 7, 7>; // Hub address
        using XACTPOS = Field<OTG_HS_HCSPLT1_Reg, 14, 2>; // XACTPOS
        using COMPLSPLT = Field<OTG_HS_HCSPLT1_Reg, 16, 1>; // Do complete split
        using SPLITEN = Field<OTG_HS_HCSPLT1_Reg, 31, 1>; // Split enable
    };

    // OTG_HS host channel-2 split control register
    using OTG_HS_HCSPLT2_Reg = Register<OTG_HS_HOST_BASE + 0x144, ReadWrite>;
    struct _OTG_HS_HCSPLT2 {
        using PRTADDR = Field<OTG_HS_HCSPLT2_Reg, 0, 7>; // Port address
        using HUBADDR = Field<OTG_HS_HCSPLT2_Reg, 7, 7>; // Hub address
        using XACTPOS = Field<OTG_HS_HCSPLT2_Reg, 14, 2>; // XACTPOS
        using COMPLSPLT = Field<OTG_HS_HCSPLT2_Reg, 16, 1>; // Do complete split
        using SPLITEN = Field<OTG_HS_HCSPLT2_Reg, 31, 1>; // Split enable
    };

    // OTG_HS host channel-3 split control register
    using OTG_HS_HCSPLT3_Reg = Register<OTG_HS_HOST_BASE + 0x164, ReadWrite>;
    struct _OTG_HS_HCSPLT3 {
        using PRTADDR = Field<OTG_HS_HCSPLT3_Reg, 0, 7>; // Port address
        using HUBADDR = Field<OTG_HS_HCSPLT3_Reg, 7, 7>; // Hub address
        using XACTPOS = Field<OTG_HS_HCSPLT3_Reg, 14, 2>; // XACTPOS
        using COMPLSPLT = Field<OTG_HS_HCSPLT3_Reg, 16, 1>; // Do complete split
        using SPLITEN = Field<OTG_HS_HCSPLT3_Reg, 31, 1>; // Split enable
    };

    // OTG_HS host channel-4 split control register
    using OTG_HS_HCSPLT4_Reg = Register<OTG_HS_HOST_BASE + 0x184, ReadWrite>;
    struct _OTG_HS_HCSPLT4 {
        using PRTADDR = Field<OTG_HS_HCSPLT4_Reg, 0, 7>; // Port address
        using HUBADDR = Field<OTG_HS_HCSPLT4_Reg, 7, 7>; // Hub address
        using XACTPOS = Field<OTG_HS_HCSPLT4_Reg, 14, 2>; // XACTPOS
        using COMPLSPLT = Field<OTG_HS_HCSPLT4_Reg, 16, 1>; // Do complete split
        using SPLITEN = Field<OTG_HS_HCSPLT4_Reg, 31, 1>; // Split enable
    };

    // OTG_HS host channel-5 split control register
    using OTG_HS_HCSPLT5_Reg = Register<OTG_HS_HOST_BASE + 0x1A4, ReadWrite>;
    struct _OTG_HS_HCSPLT5 {
        using PRTADDR = Field<OTG_HS_HCSPLT5_Reg, 0, 7>; // Port address
        using HUBADDR = Field<OTG_HS_HCSPLT5_Reg, 7, 7>; // Hub address
        using XACTPOS = Field<OTG_HS_HCSPLT5_Reg, 14, 2>; // XACTPOS
        using COMPLSPLT = Field<OTG_HS_HCSPLT5_Reg, 16, 1>; // Do complete split
        using SPLITEN = Field<OTG_HS_HCSPLT5_Reg, 31, 1>; // Split enable
    };

    // OTG_HS host channel-6 split control register
    using OTG_HS_HCSPLT6_Reg = Register<OTG_HS_HOST_BASE + 0x1C4, ReadWrite>;
    struct _OTG_HS_HCSPLT6 {
        using PRTADDR = Field<OTG_HS_HCSPLT6_Reg, 0, 7>; // Port address
        using HUBADDR = Field<OTG_HS_HCSPLT6_Reg, 7, 7>; // Hub address
        using XACTPOS = Field<OTG_HS_HCSPLT6_Reg, 14, 2>; // XACTPOS
        using COMPLSPLT = Field<OTG_HS_HCSPLT6_Reg, 16, 1>; // Do complete split
        using SPLITEN = Field<OTG_HS_HCSPLT6_Reg, 31, 1>; // Split enable
    };

    // OTG_HS host channel-7 split control register
    using OTG_HS_HCSPLT7_Reg = Register<OTG_HS_HOST_BASE + 0x1E4, ReadWrite>;
    struct _OTG_HS_HCSPLT7 {
        using PRTADDR = Field<OTG_HS_HCSPLT7_Reg, 0, 7>; // Port address
        using HUBADDR = Field<OTG_HS_HCSPLT7_Reg, 7, 7>; // Hub address
        using XACTPOS = Field<OTG_HS_HCSPLT7_Reg, 14, 2>; // XACTPOS
        using COMPLSPLT = Field<OTG_HS_HCSPLT7_Reg, 16, 1>; // Do complete split
        using SPLITEN = Field<OTG_HS_HCSPLT7_Reg, 31, 1>; // Split enable
    };

    // OTG_HS host channel-8 split control register
    using OTG_HS_HCSPLT8_Reg = Register<OTG_HS_HOST_BASE + 0x204, ReadWrite>;
    struct _OTG_HS_HCSPLT8 {
        using PRTADDR = Field<OTG_HS_HCSPLT8_Reg, 0, 7>; // Port address
        using HUBADDR = Field<OTG_HS_HCSPLT8_Reg, 7, 7>; // Hub address
        using XACTPOS = Field<OTG_HS_HCSPLT8_Reg, 14, 2>; // XACTPOS
        using COMPLSPLT = Field<OTG_HS_HCSPLT8_Reg, 16, 1>; // Do complete split
        using SPLITEN = Field<OTG_HS_HCSPLT8_Reg, 31, 1>; // Split enable
    };

    // OTG_HS host channel-9 split control register
    using OTG_HS_HCSPLT9_Reg = Register<OTG_HS_HOST_BASE + 0x224, ReadWrite>;
    struct _OTG_HS_HCSPLT9 {
        using PRTADDR = Field<OTG_HS_HCSPLT9_Reg, 0, 7>; // Port address
        using HUBADDR = Field<OTG_HS_HCSPLT9_Reg, 7, 7>; // Hub address
        using XACTPOS = Field<OTG_HS_HCSPLT9_Reg, 14, 2>; // XACTPOS
        using COMPLSPLT = Field<OTG_HS_HCSPLT9_Reg, 16, 1>; // Do complete split
        using SPLITEN = Field<OTG_HS_HCSPLT9_Reg, 31, 1>; // Split enable
    };

    // OTG_HS host channel-10 split control register
    using OTG_HS_HCSPLT10_Reg = Register<OTG_HS_HOST_BASE + 0x244, ReadWrite>;
    struct _OTG_HS_HCSPLT10 {
        using PRTADDR = Field<OTG_HS_HCSPLT10_Reg, 0, 7>; // Port address
        using HUBADDR = Field<OTG_HS_HCSPLT10_Reg, 7, 7>; // Hub address
        using XACTPOS = Field<OTG_HS_HCSPLT10_Reg, 14, 2>; // XACTPOS
        using COMPLSPLT = Field<OTG_HS_HCSPLT10_Reg, 16, 1>; // Do complete split
        using SPLITEN = Field<OTG_HS_HCSPLT10_Reg, 31, 1>; // Split enable
    };

    // OTG_HS host channel-11 split control register
    using OTG_HS_HCSPLT11_Reg = Register<OTG_HS_HOST_BASE + 0x264, ReadWrite>;
    struct _OTG_HS_HCSPLT11 {
        using PRTADDR = Field<OTG_HS_HCSPLT11_Reg, 0, 7>; // Port address
        using HUBADDR = Field<OTG_HS_HCSPLT11_Reg, 7, 7>; // Hub address
        using XACTPOS = Field<OTG_HS_HCSPLT11_Reg, 14, 2>; // XACTPOS
        using COMPLSPLT = Field<OTG_HS_HCSPLT11_Reg, 16, 1>; // Do complete split
        using SPLITEN = Field<OTG_HS_HCSPLT11_Reg, 31, 1>; // Split enable
    };

    // OTG_HS host channel-11 interrupt register
    using OTG_HS_HCINT0_Reg = Register<OTG_HS_HOST_BASE + 0x108, ReadWrite>;
    struct _OTG_HS_HCINT0 {
        using XFRC = Field<OTG_HS_HCINT0_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_HS_HCINT0_Reg, 1, 1>; // Channel halted
        using AHBERR = Field<OTG_HS_HCINT0_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINT0_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_HS_HCINT0_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_HS_HCINT0_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using NYET = Field<OTG_HS_HCINT0_Reg, 6, 1>; // Response received interrupt
        using TXERR = Field<OTG_HS_HCINT0_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_HS_HCINT0_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_HS_HCINT0_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_HS_HCINT0_Reg, 10, 1>; // Data toggle error
    };

    // OTG_HS host channel-1 interrupt register
    using OTG_HS_HCINT1_Reg = Register<OTG_HS_HOST_BASE + 0x128, ReadWrite>;
    struct _OTG_HS_HCINT1 {
        using XFRC = Field<OTG_HS_HCINT1_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_HS_HCINT1_Reg, 1, 1>; // Channel halted
        using AHBERR = Field<OTG_HS_HCINT1_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINT1_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_HS_HCINT1_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_HS_HCINT1_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using NYET = Field<OTG_HS_HCINT1_Reg, 6, 1>; // Response received interrupt
        using TXERR = Field<OTG_HS_HCINT1_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_HS_HCINT1_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_HS_HCINT1_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_HS_HCINT1_Reg, 10, 1>; // Data toggle error
    };

    // OTG_HS host channel-2 interrupt register
    using OTG_HS_HCINT2_Reg = Register<OTG_HS_HOST_BASE + 0x148, ReadWrite>;
    struct _OTG_HS_HCINT2 {
        using XFRC = Field<OTG_HS_HCINT2_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_HS_HCINT2_Reg, 1, 1>; // Channel halted
        using AHBERR = Field<OTG_HS_HCINT2_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINT2_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_HS_HCINT2_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_HS_HCINT2_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using NYET = Field<OTG_HS_HCINT2_Reg, 6, 1>; // Response received interrupt
        using TXERR = Field<OTG_HS_HCINT2_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_HS_HCINT2_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_HS_HCINT2_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_HS_HCINT2_Reg, 10, 1>; // Data toggle error
    };

    // OTG_HS host channel-3 interrupt register
    using OTG_HS_HCINT3_Reg = Register<OTG_HS_HOST_BASE + 0x168, ReadWrite>;
    struct _OTG_HS_HCINT3 {
        using XFRC = Field<OTG_HS_HCINT3_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_HS_HCINT3_Reg, 1, 1>; // Channel halted
        using AHBERR = Field<OTG_HS_HCINT3_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINT3_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_HS_HCINT3_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_HS_HCINT3_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using NYET = Field<OTG_HS_HCINT3_Reg, 6, 1>; // Response received interrupt
        using TXERR = Field<OTG_HS_HCINT3_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_HS_HCINT3_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_HS_HCINT3_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_HS_HCINT3_Reg, 10, 1>; // Data toggle error
    };

    // OTG_HS host channel-4 interrupt register
    using OTG_HS_HCINT4_Reg = Register<OTG_HS_HOST_BASE + 0x188, ReadWrite>;
    struct _OTG_HS_HCINT4 {
        using XFRC = Field<OTG_HS_HCINT4_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_HS_HCINT4_Reg, 1, 1>; // Channel halted
        using AHBERR = Field<OTG_HS_HCINT4_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINT4_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_HS_HCINT4_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_HS_HCINT4_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using NYET = Field<OTG_HS_HCINT4_Reg, 6, 1>; // Response received interrupt
        using TXERR = Field<OTG_HS_HCINT4_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_HS_HCINT4_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_HS_HCINT4_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_HS_HCINT4_Reg, 10, 1>; // Data toggle error
    };

    // OTG_HS host channel-5 interrupt register
    using OTG_HS_HCINT5_Reg = Register<OTG_HS_HOST_BASE + 0x1A8, ReadWrite>;
    struct _OTG_HS_HCINT5 {
        using XFRC = Field<OTG_HS_HCINT5_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_HS_HCINT5_Reg, 1, 1>; // Channel halted
        using AHBERR = Field<OTG_HS_HCINT5_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINT5_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_HS_HCINT5_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_HS_HCINT5_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using NYET = Field<OTG_HS_HCINT5_Reg, 6, 1>; // Response received interrupt
        using TXERR = Field<OTG_HS_HCINT5_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_HS_HCINT5_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_HS_HCINT5_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_HS_HCINT5_Reg, 10, 1>; // Data toggle error
    };

    // OTG_HS host channel-6 interrupt register
    using OTG_HS_HCINT6_Reg = Register<OTG_HS_HOST_BASE + 0x1C8, ReadWrite>;
    struct _OTG_HS_HCINT6 {
        using XFRC = Field<OTG_HS_HCINT6_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_HS_HCINT6_Reg, 1, 1>; // Channel halted
        using AHBERR = Field<OTG_HS_HCINT6_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINT6_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_HS_HCINT6_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_HS_HCINT6_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using NYET = Field<OTG_HS_HCINT6_Reg, 6, 1>; // Response received interrupt
        using TXERR = Field<OTG_HS_HCINT6_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_HS_HCINT6_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_HS_HCINT6_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_HS_HCINT6_Reg, 10, 1>; // Data toggle error
    };

    // OTG_HS host channel-7 interrupt register
    using OTG_HS_HCINT7_Reg = Register<OTG_HS_HOST_BASE + 0x1E8, ReadWrite>;
    struct _OTG_HS_HCINT7 {
        using XFRC = Field<OTG_HS_HCINT7_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_HS_HCINT7_Reg, 1, 1>; // Channel halted
        using AHBERR = Field<OTG_HS_HCINT7_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINT7_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_HS_HCINT7_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_HS_HCINT7_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using NYET = Field<OTG_HS_HCINT7_Reg, 6, 1>; // Response received interrupt
        using TXERR = Field<OTG_HS_HCINT7_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_HS_HCINT7_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_HS_HCINT7_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_HS_HCINT7_Reg, 10, 1>; // Data toggle error
    };

    // OTG_HS host channel-8 interrupt register
    using OTG_HS_HCINT8_Reg = Register<OTG_HS_HOST_BASE + 0x208, ReadWrite>;
    struct _OTG_HS_HCINT8 {
        using XFRC = Field<OTG_HS_HCINT8_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_HS_HCINT8_Reg, 1, 1>; // Channel halted
        using AHBERR = Field<OTG_HS_HCINT8_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINT8_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_HS_HCINT8_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_HS_HCINT8_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using NYET = Field<OTG_HS_HCINT8_Reg, 6, 1>; // Response received interrupt
        using TXERR = Field<OTG_HS_HCINT8_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_HS_HCINT8_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_HS_HCINT8_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_HS_HCINT8_Reg, 10, 1>; // Data toggle error
    };

    // OTG_HS host channel-9 interrupt register
    using OTG_HS_HCINT9_Reg = Register<OTG_HS_HOST_BASE + 0x228, ReadWrite>;
    struct _OTG_HS_HCINT9 {
        using XFRC = Field<OTG_HS_HCINT9_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_HS_HCINT9_Reg, 1, 1>; // Channel halted
        using AHBERR = Field<OTG_HS_HCINT9_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINT9_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_HS_HCINT9_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_HS_HCINT9_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using NYET = Field<OTG_HS_HCINT9_Reg, 6, 1>; // Response received interrupt
        using TXERR = Field<OTG_HS_HCINT9_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_HS_HCINT9_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_HS_HCINT9_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_HS_HCINT9_Reg, 10, 1>; // Data toggle error
    };

    // OTG_HS host channel-10 interrupt register
    using OTG_HS_HCINT10_Reg = Register<OTG_HS_HOST_BASE + 0x248, ReadWrite>;
    struct _OTG_HS_HCINT10 {
        using XFRC = Field<OTG_HS_HCINT10_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_HS_HCINT10_Reg, 1, 1>; // Channel halted
        using AHBERR = Field<OTG_HS_HCINT10_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINT10_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_HS_HCINT10_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_HS_HCINT10_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using NYET = Field<OTG_HS_HCINT10_Reg, 6, 1>; // Response received interrupt
        using TXERR = Field<OTG_HS_HCINT10_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_HS_HCINT10_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_HS_HCINT10_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_HS_HCINT10_Reg, 10, 1>; // Data toggle error
    };

    // OTG_HS host channel-11 interrupt register
    using OTG_HS_HCINT11_Reg = Register<OTG_HS_HOST_BASE + 0x268, ReadWrite>;
    struct _OTG_HS_HCINT11 {
        using XFRC = Field<OTG_HS_HCINT11_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_HS_HCINT11_Reg, 1, 1>; // Channel halted
        using AHBERR = Field<OTG_HS_HCINT11_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINT11_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_HS_HCINT11_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_HS_HCINT11_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using NYET = Field<OTG_HS_HCINT11_Reg, 6, 1>; // Response received interrupt
        using TXERR = Field<OTG_HS_HCINT11_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_HS_HCINT11_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_HS_HCINT11_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_HS_HCINT11_Reg, 10, 1>; // Data toggle error
    };

    // OTG_HS host channel-11 interrupt mask register
    using OTG_HS_HCINTMSK0_Reg = Register<OTG_HS_HOST_BASE + 0x10C, ReadWrite>;
    struct _OTG_HS_HCINTMSK0 {
        using XFRCM = Field<OTG_HS_HCINTMSK0_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_HS_HCINTMSK0_Reg, 1, 1>; // Channel halted mask
        using AHBERR = Field<OTG_HS_HCINTMSK0_Reg, 2, 1>; // AHB error
        using STALLM = Field<OTG_HS_HCINTMSK0_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_HS_HCINTMSK0_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_HS_HCINTMSK0_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_HS_HCINTMSK0_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_HS_HCINTMSK0_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_HS_HCINTMSK0_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_HS_HCINTMSK0_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_HS_HCINTMSK0_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_HS host channel-1 interrupt mask register
    using OTG_HS_HCINTMSK1_Reg = Register<OTG_HS_HOST_BASE + 0x12C, ReadWrite>;
    struct _OTG_HS_HCINTMSK1 {
        using XFRCM = Field<OTG_HS_HCINTMSK1_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_HS_HCINTMSK1_Reg, 1, 1>; // Channel halted mask
        using AHBERR = Field<OTG_HS_HCINTMSK1_Reg, 2, 1>; // AHB error
        using STALLM = Field<OTG_HS_HCINTMSK1_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_HS_HCINTMSK1_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_HS_HCINTMSK1_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_HS_HCINTMSK1_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_HS_HCINTMSK1_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_HS_HCINTMSK1_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_HS_HCINTMSK1_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_HS_HCINTMSK1_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_HS host channel-2 interrupt mask register
    using OTG_HS_HCINTMSK2_Reg = Register<OTG_HS_HOST_BASE + 0x14C, ReadWrite>;
    struct _OTG_HS_HCINTMSK2 {
        using XFRCM = Field<OTG_HS_HCINTMSK2_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_HS_HCINTMSK2_Reg, 1, 1>; // Channel halted mask
        using AHBERR = Field<OTG_HS_HCINTMSK2_Reg, 2, 1>; // AHB error
        using STALLM = Field<OTG_HS_HCINTMSK2_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_HS_HCINTMSK2_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_HS_HCINTMSK2_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_HS_HCINTMSK2_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_HS_HCINTMSK2_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_HS_HCINTMSK2_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_HS_HCINTMSK2_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_HS_HCINTMSK2_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_HS host channel-3 interrupt mask register
    using OTG_HS_HCINTMSK3_Reg = Register<OTG_HS_HOST_BASE + 0x16C, ReadWrite>;
    struct _OTG_HS_HCINTMSK3 {
        using XFRCM = Field<OTG_HS_HCINTMSK3_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_HS_HCINTMSK3_Reg, 1, 1>; // Channel halted mask
        using AHBERR = Field<OTG_HS_HCINTMSK3_Reg, 2, 1>; // AHB error
        using STALLM = Field<OTG_HS_HCINTMSK3_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_HS_HCINTMSK3_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_HS_HCINTMSK3_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_HS_HCINTMSK3_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_HS_HCINTMSK3_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_HS_HCINTMSK3_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_HS_HCINTMSK3_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_HS_HCINTMSK3_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_HS host channel-4 interrupt mask register
    using OTG_HS_HCINTMSK4_Reg = Register<OTG_HS_HOST_BASE + 0x18C, ReadWrite>;
    struct _OTG_HS_HCINTMSK4 {
        using XFRCM = Field<OTG_HS_HCINTMSK4_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_HS_HCINTMSK4_Reg, 1, 1>; // Channel halted mask
        using AHBERR = Field<OTG_HS_HCINTMSK4_Reg, 2, 1>; // AHB error
        using STALLM = Field<OTG_HS_HCINTMSK4_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_HS_HCINTMSK4_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_HS_HCINTMSK4_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_HS_HCINTMSK4_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_HS_HCINTMSK4_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_HS_HCINTMSK4_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_HS_HCINTMSK4_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_HS_HCINTMSK4_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_HS host channel-5 interrupt mask register
    using OTG_HS_HCINTMSK5_Reg = Register<OTG_HS_HOST_BASE + 0x1AC, ReadWrite>;
    struct _OTG_HS_HCINTMSK5 {
        using XFRCM = Field<OTG_HS_HCINTMSK5_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_HS_HCINTMSK5_Reg, 1, 1>; // Channel halted mask
        using AHBERR = Field<OTG_HS_HCINTMSK5_Reg, 2, 1>; // AHB error
        using STALLM = Field<OTG_HS_HCINTMSK5_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_HS_HCINTMSK5_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_HS_HCINTMSK5_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_HS_HCINTMSK5_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_HS_HCINTMSK5_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_HS_HCINTMSK5_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_HS_HCINTMSK5_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_HS_HCINTMSK5_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_HS host channel-6 interrupt mask register
    using OTG_HS_HCINTMSK6_Reg = Register<OTG_HS_HOST_BASE + 0x1CC, ReadWrite>;
    struct _OTG_HS_HCINTMSK6 {
        using XFRCM = Field<OTG_HS_HCINTMSK6_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_HS_HCINTMSK6_Reg, 1, 1>; // Channel halted mask
        using AHBERR = Field<OTG_HS_HCINTMSK6_Reg, 2, 1>; // AHB error
        using STALLM = Field<OTG_HS_HCINTMSK6_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_HS_HCINTMSK6_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_HS_HCINTMSK6_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_HS_HCINTMSK6_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_HS_HCINTMSK6_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_HS_HCINTMSK6_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_HS_HCINTMSK6_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_HS_HCINTMSK6_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_HS host channel-7 interrupt mask register
    using OTG_HS_HCINTMSK7_Reg = Register<OTG_HS_HOST_BASE + 0x1EC, ReadWrite>;
    struct _OTG_HS_HCINTMSK7 {
        using XFRCM = Field<OTG_HS_HCINTMSK7_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_HS_HCINTMSK7_Reg, 1, 1>; // Channel halted mask
        using AHBERR = Field<OTG_HS_HCINTMSK7_Reg, 2, 1>; // AHB error
        using STALLM = Field<OTG_HS_HCINTMSK7_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_HS_HCINTMSK7_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_HS_HCINTMSK7_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_HS_HCINTMSK7_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_HS_HCINTMSK7_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_HS_HCINTMSK7_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_HS_HCINTMSK7_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_HS_HCINTMSK7_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_HS host channel-8 interrupt mask register
    using OTG_HS_HCINTMSK8_Reg = Register<OTG_HS_HOST_BASE + 0x20C, ReadWrite>;
    struct _OTG_HS_HCINTMSK8 {
        using XFRCM = Field<OTG_HS_HCINTMSK8_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_HS_HCINTMSK8_Reg, 1, 1>; // Channel halted mask
        using AHBERR = Field<OTG_HS_HCINTMSK8_Reg, 2, 1>; // AHB error
        using STALLM = Field<OTG_HS_HCINTMSK8_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_HS_HCINTMSK8_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_HS_HCINTMSK8_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_HS_HCINTMSK8_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_HS_HCINTMSK8_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_HS_HCINTMSK8_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_HS_HCINTMSK8_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_HS_HCINTMSK8_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_HS host channel-9 interrupt mask register
    using OTG_HS_HCINTMSK9_Reg = Register<OTG_HS_HOST_BASE + 0x22C, ReadWrite>;
    struct _OTG_HS_HCINTMSK9 {
        using XFRCM = Field<OTG_HS_HCINTMSK9_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_HS_HCINTMSK9_Reg, 1, 1>; // Channel halted mask
        using AHBERR = Field<OTG_HS_HCINTMSK9_Reg, 2, 1>; // AHB error
        using STALLM = Field<OTG_HS_HCINTMSK9_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_HS_HCINTMSK9_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_HS_HCINTMSK9_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_HS_HCINTMSK9_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_HS_HCINTMSK9_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_HS_HCINTMSK9_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_HS_HCINTMSK9_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_HS_HCINTMSK9_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_HS host channel-10 interrupt mask register
    using OTG_HS_HCINTMSK10_Reg = Register<OTG_HS_HOST_BASE + 0x24C, ReadWrite>;
    struct _OTG_HS_HCINTMSK10 {
        using XFRCM = Field<OTG_HS_HCINTMSK10_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_HS_HCINTMSK10_Reg, 1, 1>; // Channel halted mask
        using AHBERR = Field<OTG_HS_HCINTMSK10_Reg, 2, 1>; // AHB error
        using STALLM = Field<OTG_HS_HCINTMSK10_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_HS_HCINTMSK10_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_HS_HCINTMSK10_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_HS_HCINTMSK10_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_HS_HCINTMSK10_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_HS_HCINTMSK10_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_HS_HCINTMSK10_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_HS_HCINTMSK10_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_HS host channel-11 interrupt mask register
    using OTG_HS_HCINTMSK11_Reg = Register<OTG_HS_HOST_BASE + 0x26C, ReadWrite>;
    struct _OTG_HS_HCINTMSK11 {
        using XFRCM = Field<OTG_HS_HCINTMSK11_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_HS_HCINTMSK11_Reg, 1, 1>; // Channel halted mask
        using AHBERR = Field<OTG_HS_HCINTMSK11_Reg, 2, 1>; // AHB error
        using STALLM = Field<OTG_HS_HCINTMSK11_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_HS_HCINTMSK11_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_HS_HCINTMSK11_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_HS_HCINTMSK11_Reg, 6, 1>; // response received interrupt mask
        using TXERRM = Field<OTG_HS_HCINTMSK11_Reg, 7, 1>; // Transaction error mask
        using BBERRM = Field<OTG_HS_HCINTMSK11_Reg, 8, 1>; // Babble error mask
        using FRMORM = Field<OTG_HS_HCINTMSK11_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_HS_HCINTMSK11_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_HS host channel-11 transfer size register
    using OTG_HS_HCTSIZ0_Reg = Register<OTG_HS_HOST_BASE + 0x110, ReadWrite>;
    struct _OTG_HS_HCTSIZ0 {
        using XFRSIZ = Field<OTG_HS_HCTSIZ0_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_HCTSIZ0_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_HS_HCTSIZ0_Reg, 29, 2>; // Data PID
    };

    // OTG_HS host channel-1 transfer size register
    using OTG_HS_HCTSIZ1_Reg = Register<OTG_HS_HOST_BASE + 0x130, ReadWrite>;
    struct _OTG_HS_HCTSIZ1 {
        using XFRSIZ = Field<OTG_HS_HCTSIZ1_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_HCTSIZ1_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_HS_HCTSIZ1_Reg, 29, 2>; // Data PID
    };

    // OTG_HS host channel-2 transfer size register
    using OTG_HS_HCTSIZ2_Reg = Register<OTG_HS_HOST_BASE + 0x150, ReadWrite>;
    struct _OTG_HS_HCTSIZ2 {
        using XFRSIZ = Field<OTG_HS_HCTSIZ2_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_HCTSIZ2_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_HS_HCTSIZ2_Reg, 29, 2>; // Data PID
    };

    // OTG_HS host channel-3 transfer size register
    using OTG_HS_HCTSIZ3_Reg = Register<OTG_HS_HOST_BASE + 0x170, ReadWrite>;
    struct _OTG_HS_HCTSIZ3 {
        using XFRSIZ = Field<OTG_HS_HCTSIZ3_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_HCTSIZ3_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_HS_HCTSIZ3_Reg, 29, 2>; // Data PID
    };

    // OTG_HS host channel-4 transfer size register
    using OTG_HS_HCTSIZ4_Reg = Register<OTG_HS_HOST_BASE + 0x190, ReadWrite>;
    struct _OTG_HS_HCTSIZ4 {
        using XFRSIZ = Field<OTG_HS_HCTSIZ4_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_HCTSIZ4_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_HS_HCTSIZ4_Reg, 29, 2>; // Data PID
    };

    // OTG_HS host channel-5 transfer size register
    using OTG_HS_HCTSIZ5_Reg = Register<OTG_HS_HOST_BASE + 0x1B0, ReadWrite>;
    struct _OTG_HS_HCTSIZ5 {
        using XFRSIZ = Field<OTG_HS_HCTSIZ5_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_HCTSIZ5_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_HS_HCTSIZ5_Reg, 29, 2>; // Data PID
    };

    // OTG_HS host channel-6 transfer size register
    using OTG_HS_HCTSIZ6_Reg = Register<OTG_HS_HOST_BASE + 0x1D0, ReadWrite>;
    struct _OTG_HS_HCTSIZ6 {
        using XFRSIZ = Field<OTG_HS_HCTSIZ6_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_HCTSIZ6_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_HS_HCTSIZ6_Reg, 29, 2>; // Data PID
    };

    // OTG_HS host channel-7 transfer size register
    using OTG_HS_HCTSIZ7_Reg = Register<OTG_HS_HOST_BASE + 0x1F0, ReadWrite>;
    struct _OTG_HS_HCTSIZ7 {
        using XFRSIZ = Field<OTG_HS_HCTSIZ7_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_HCTSIZ7_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_HS_HCTSIZ7_Reg, 29, 2>; // Data PID
    };

    // OTG_HS host channel-8 transfer size register
    using OTG_HS_HCTSIZ8_Reg = Register<OTG_HS_HOST_BASE + 0x210, ReadWrite>;
    struct _OTG_HS_HCTSIZ8 {
        using XFRSIZ = Field<OTG_HS_HCTSIZ8_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_HCTSIZ8_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_HS_HCTSIZ8_Reg, 29, 2>; // Data PID
    };

    // OTG_HS host channel-9 transfer size register
    using OTG_HS_HCTSIZ9_Reg = Register<OTG_HS_HOST_BASE + 0x230, ReadWrite>;
    struct _OTG_HS_HCTSIZ9 {
        using XFRSIZ = Field<OTG_HS_HCTSIZ9_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_HCTSIZ9_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_HS_HCTSIZ9_Reg, 29, 2>; // Data PID
    };

    // OTG_HS host channel-10 transfer size register
    using OTG_HS_HCTSIZ10_Reg = Register<OTG_HS_HOST_BASE + 0x250, ReadWrite>;
    struct _OTG_HS_HCTSIZ10 {
        using XFRSIZ = Field<OTG_HS_HCTSIZ10_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_HCTSIZ10_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_HS_HCTSIZ10_Reg, 29, 2>; // Data PID
    };

    // OTG_HS host channel-11 transfer size register
    using OTG_HS_HCTSIZ11_Reg = Register<OTG_HS_HOST_BASE + 0x270, ReadWrite>;
    struct _OTG_HS_HCTSIZ11 {
        using XFRSIZ = Field<OTG_HS_HCTSIZ11_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_HCTSIZ11_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_HS_HCTSIZ11_Reg, 29, 2>; // Data PID
    };

    // OTG_HS host channel-0 DMA address register
    using OTG_HS_HCDMA0_Reg = Register<OTG_HS_HOST_BASE + 0x114, ReadWrite>;
    struct _OTG_HS_HCDMA0 {
        using DMAADDR = Field<OTG_HS_HCDMA0_Reg, 0, 32>; // DMA address
    };

    // OTG_HS host channel-1 DMA address register
    using OTG_HS_HCDMA1_Reg = Register<OTG_HS_HOST_BASE + 0x134, ReadWrite>;
    struct _OTG_HS_HCDMA1 {
        using DMAADDR = Field<OTG_HS_HCDMA1_Reg, 0, 32>; // DMA address
    };

    // OTG_HS host channel-2 DMA address register
    using OTG_HS_HCDMA2_Reg = Register<OTG_HS_HOST_BASE + 0x154, ReadWrite>;
    struct _OTG_HS_HCDMA2 {
        using DMAADDR = Field<OTG_HS_HCDMA2_Reg, 0, 32>; // DMA address
    };

    // OTG_HS host channel-3 DMA address register
    using OTG_HS_HCDMA3_Reg = Register<OTG_HS_HOST_BASE + 0x174, ReadWrite>;
    struct _OTG_HS_HCDMA3 {
        using DMAADDR = Field<OTG_HS_HCDMA3_Reg, 0, 32>; // DMA address
    };

    // OTG_HS host channel-4 DMA address register
    using OTG_HS_HCDMA4_Reg = Register<OTG_HS_HOST_BASE + 0x194, ReadWrite>;
    struct _OTG_HS_HCDMA4 {
        using DMAADDR = Field<OTG_HS_HCDMA4_Reg, 0, 32>; // DMA address
    };

    // OTG_HS host channel-5 DMA address register
    using OTG_HS_HCDMA5_Reg = Register<OTG_HS_HOST_BASE + 0x1B4, ReadWrite>;
    struct _OTG_HS_HCDMA5 {
        using DMAADDR = Field<OTG_HS_HCDMA5_Reg, 0, 32>; // DMA address
    };

    // OTG_HS host channel-6 DMA address register
    using OTG_HS_HCDMA6_Reg = Register<OTG_HS_HOST_BASE + 0x1D4, ReadWrite>;
    struct _OTG_HS_HCDMA6 {
        using DMAADDR = Field<OTG_HS_HCDMA6_Reg, 0, 32>; // DMA address
    };

    // OTG_HS host channel-7 DMA address register
    using OTG_HS_HCDMA7_Reg = Register<OTG_HS_HOST_BASE + 0x1F4, ReadWrite>;
    struct _OTG_HS_HCDMA7 {
        using DMAADDR = Field<OTG_HS_HCDMA7_Reg, 0, 32>; // DMA address
    };

    // OTG_HS host channel-8 DMA address register
    using OTG_HS_HCDMA8_Reg = Register<OTG_HS_HOST_BASE + 0x214, ReadWrite>;
    struct _OTG_HS_HCDMA8 {
        using DMAADDR = Field<OTG_HS_HCDMA8_Reg, 0, 32>; // DMA address
    };

    // OTG_HS host channel-9 DMA address register
    using OTG_HS_HCDMA9_Reg = Register<OTG_HS_HOST_BASE + 0x234, ReadWrite>;
    struct _OTG_HS_HCDMA9 {
        using DMAADDR = Field<OTG_HS_HCDMA9_Reg, 0, 32>; // DMA address
    };

    // OTG_HS host channel-10 DMA address register
    using OTG_HS_HCDMA10_Reg = Register<OTG_HS_HOST_BASE + 0x254, ReadWrite>;
    struct _OTG_HS_HCDMA10 {
        using DMAADDR = Field<OTG_HS_HCDMA10_Reg, 0, 32>; // DMA address
    };

    // OTG_HS host channel-11 DMA address register
    using OTG_HS_HCDMA11_Reg = Register<OTG_HS_HOST_BASE + 0x274, ReadWrite>;
    struct _OTG_HS_HCDMA11 {
        using DMAADDR = Field<OTG_HS_HCDMA11_Reg, 0, 32>; // DMA address
    };

    // OTG_HS host channel-12 characteristics register
    using OTG_HS_HCCHAR12_Reg = Register<OTG_HS_HOST_BASE + 0x278, ReadWrite>;
    struct _OTG_HS_HCCHAR12 {
        using MPSIZ = Field<OTG_HS_HCCHAR12_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_HS_HCCHAR12_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_HS_HCCHAR12_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_HS_HCCHAR12_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_HS_HCCHAR12_Reg, 18, 2>; // Endpoint type
        using MC = Field<OTG_HS_HCCHAR12_Reg, 20, 2>; // Multi Count (MC) / Error Count (EC)
        using DAD = Field<OTG_HS_HCCHAR12_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_HS_HCCHAR12_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_HS_HCCHAR12_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_HS_HCCHAR12_Reg, 31, 1>; // Channel enable
    };

    // OTG_HS host channel-12 split control register
    using OTG_HS_HCSPLT12_Reg = Register<OTG_HS_HOST_BASE + 0x27C, ReadWrite>;
    struct _OTG_HS_HCSPLT12 {
        using PRTADDR = Field<OTG_HS_HCSPLT12_Reg, 0, 7>; // Port address
        using HUBADDR = Field<OTG_HS_HCSPLT12_Reg, 7, 7>; // Hub address
        using XACTPOS = Field<OTG_HS_HCSPLT12_Reg, 14, 2>; // XACTPOS
        using COMPLSPLT = Field<OTG_HS_HCSPLT12_Reg, 16, 1>; // Do complete split
        using SPLITEN = Field<OTG_HS_HCSPLT12_Reg, 31, 1>; // Split enable
    };

    // OTG_HS host channel-12 interrupt register
    using OTG_HS_HCINT12_Reg = Register<OTG_HS_HOST_BASE + 0x280, ReadWrite>;
    struct _OTG_HS_HCINT12 {
        using XFRC = Field<OTG_HS_HCINT12_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_HS_HCINT12_Reg, 1, 1>; // Channel halted
        using AHBERR = Field<OTG_HS_HCINT12_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINT12_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_HS_HCINT12_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_HS_HCINT12_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using NYET = Field<OTG_HS_HCINT12_Reg, 6, 1>; // Response received interrupt
        using TXERR = Field<OTG_HS_HCINT12_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_HS_HCINT12_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_HS_HCINT12_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_HS_HCINT12_Reg, 10, 1>; // Data toggle error
    };

    // OTG_HS host channel-12 interrupt mask register
    using OTG_HS_HCINTMSK12_Reg = Register<OTG_HS_HOST_BASE + 0x284, ReadWrite>;
    struct _OTG_HS_HCINTMSK12 {
        using XFRCM = Field<OTG_HS_HCINTMSK12_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_HS_HCINTMSK12_Reg, 1, 1>; // Channel halted mask
        using AHBERR = Field<OTG_HS_HCINTMSK12_Reg, 2, 1>; // AHB error
        using STALLM = Field<OTG_HS_HCINTMSK12_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_HS_HCINTMSK12_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_HS_HCINTMSK12_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_HS_HCINTMSK12_Reg, 6, 1>; // Response received interrupt
        using TXERRM = Field<OTG_HS_HCINTMSK12_Reg, 7, 1>; // Transaction error
        using BBERRM = Field<OTG_HS_HCINTMSK12_Reg, 8, 1>; // Babble error
        using FRMORM = Field<OTG_HS_HCINTMSK12_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_HS_HCINTMSK12_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_HS host channel-12 transfer size register
    using OTG_HS_HCTSIZ12_Reg = Register<OTG_HS_HOST_BASE + 0x288, ReadWrite>;
    struct _OTG_HS_HCTSIZ12 {
        using XFRSIZ = Field<OTG_HS_HCTSIZ12_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_HCTSIZ12_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_HS_HCTSIZ12_Reg, 29, 2>; // Data PID
    };

    // OTG_HS host channel-12 DMA address register
    using OTG_HS_HCDMA12_Reg = Register<OTG_HS_HOST_BASE + 0x28C, ReadWrite>;
    struct _OTG_HS_HCDMA12 {
        using DMAADDR = Field<OTG_HS_HCDMA12_Reg, 0, 32>; // DMA address
    };

    // OTG_HS host channel-13 characteristics register
    using OTG_HS_HCCHAR13_Reg = Register<OTG_HS_HOST_BASE + 0x290, ReadWrite>;
    struct _OTG_HS_HCCHAR13 {
        using MPSIZ = Field<OTG_HS_HCCHAR13_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_HS_HCCHAR13_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_HS_HCCHAR13_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_HS_HCCHAR13_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_HS_HCCHAR13_Reg, 18, 2>; // Endpoint type
        using MC = Field<OTG_HS_HCCHAR13_Reg, 20, 2>; // Multi Count (MC) / Error Count (EC)
        using DAD = Field<OTG_HS_HCCHAR13_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_HS_HCCHAR13_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_HS_HCCHAR13_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_HS_HCCHAR13_Reg, 31, 1>; // Channel enable
    };

    // OTG_HS host channel-13 split control register
    using OTG_HS_HCSPLT13_Reg = Register<OTG_HS_HOST_BASE + 0x294, ReadWrite>;
    struct _OTG_HS_HCSPLT13 {
        using PRTADDR = Field<OTG_HS_HCSPLT13_Reg, 0, 7>; // Port address
        using HUBADDR = Field<OTG_HS_HCSPLT13_Reg, 7, 7>; // Hub address
        using XACTPOS = Field<OTG_HS_HCSPLT13_Reg, 14, 2>; // XACTPOS
        using COMPLSPLT = Field<OTG_HS_HCSPLT13_Reg, 16, 1>; // Do complete split
        using SPLITEN = Field<OTG_HS_HCSPLT13_Reg, 31, 1>; // Split enable
    };

    // OTG_HS host channel-13 interrupt register
    using OTG_HS_HCINT13_Reg = Register<OTG_HS_HOST_BASE + 0x298, ReadWrite>;
    struct _OTG_HS_HCINT13 {
        using XFRC = Field<OTG_HS_HCINT13_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_HS_HCINT13_Reg, 1, 1>; // Channel halted
        using AHBERR = Field<OTG_HS_HCINT13_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINT13_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_HS_HCINT13_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_HS_HCINT13_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using NYET = Field<OTG_HS_HCINT13_Reg, 6, 1>; // Response received interrupt
        using TXERR = Field<OTG_HS_HCINT13_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_HS_HCINT13_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_HS_HCINT13_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_HS_HCINT13_Reg, 10, 1>; // Data toggle error
    };

    // OTG_HS host channel-13 interrupt mask register
    using OTG_HS_HCINTMSK13_Reg = Register<OTG_HS_HOST_BASE + 0x29C, ReadWrite>;
    struct _OTG_HS_HCINTMSK13 {
        using XFRCM = Field<OTG_HS_HCINTMSK13_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_HS_HCINTMSK13_Reg, 1, 1>; // Channel halted mask
        using AHBERR = Field<OTG_HS_HCINTMSK13_Reg, 2, 1>; // AHB error
        using STALLM = Field<OTG_HS_HCINTMSK13_Reg, 3, 1>; // STALLM response received interrupt mask
        using NAKM = Field<OTG_HS_HCINTMSK13_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_HS_HCINTMSK13_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_HS_HCINTMSK13_Reg, 6, 1>; // Response received interrupt
        using TXERRM = Field<OTG_HS_HCINTMSK13_Reg, 7, 1>; // Transaction error
        using BBERRM = Field<OTG_HS_HCINTMSK13_Reg, 8, 1>; // Babble error
        using FRMORM = Field<OTG_HS_HCINTMSK13_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_HS_HCINTMSK13_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_HS host channel-13 transfer size register
    using OTG_HS_HCTSIZ13_Reg = Register<OTG_HS_HOST_BASE + 0x2A0, ReadWrite>;
    struct _OTG_HS_HCTSIZ13 {
        using XFRSIZ = Field<OTG_HS_HCTSIZ13_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_HCTSIZ13_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_HS_HCTSIZ13_Reg, 29, 2>; // Data PID
    };

    // OTG_HS host channel-13 DMA address register
    using OTG_HS_HCDMA13_Reg = Register<OTG_HS_HOST_BASE + 0x2A4, ReadWrite>;
    struct _OTG_HS_HCDMA13 {
        using DMAADDR = Field<OTG_HS_HCDMA13_Reg, 0, 32>; // DMA address
    };

    // OTG_HS host channel-14 characteristics register
    using OTG_HS_HCCHAR14_Reg = Register<OTG_HS_HOST_BASE + 0x2A8, ReadWrite>;
    struct _OTG_HS_HCCHAR14 {
        using MPSIZ = Field<OTG_HS_HCCHAR14_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_HS_HCCHAR14_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_HS_HCCHAR14_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_HS_HCCHAR14_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_HS_HCCHAR14_Reg, 18, 2>; // Endpoint type
        using MC = Field<OTG_HS_HCCHAR14_Reg, 20, 2>; // Multi Count (MC) / Error Count (EC)
        using DAD = Field<OTG_HS_HCCHAR14_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_HS_HCCHAR14_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_HS_HCCHAR14_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_HS_HCCHAR14_Reg, 31, 1>; // Channel enable
    };

    // OTG_HS host channel-14 split control register
    using OTG_HS_HCSPLT14_Reg = Register<OTG_HS_HOST_BASE + 0x2AC, ReadWrite>;
    struct _OTG_HS_HCSPLT14 {
        using PRTADDR = Field<OTG_HS_HCSPLT14_Reg, 0, 7>; // Port address
        using HUBADDR = Field<OTG_HS_HCSPLT14_Reg, 7, 7>; // Hub address
        using XACTPOS = Field<OTG_HS_HCSPLT14_Reg, 14, 2>; // XACTPOS
        using COMPLSPLT = Field<OTG_HS_HCSPLT14_Reg, 16, 1>; // Do complete split
        using SPLITEN = Field<OTG_HS_HCSPLT14_Reg, 31, 1>; // Split enable
    };

    // OTG_HS host channel-14 interrupt register
    using OTG_HS_HCINT14_Reg = Register<OTG_HS_HOST_BASE + 0x2B0, ReadWrite>;
    struct _OTG_HS_HCINT14 {
        using XFRC = Field<OTG_HS_HCINT14_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_HS_HCINT14_Reg, 1, 1>; // Channel halted
        using AHBERR = Field<OTG_HS_HCINT14_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINT14_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_HS_HCINT14_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_HS_HCINT14_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using NYET = Field<OTG_HS_HCINT14_Reg, 6, 1>; // Response received interrupt
        using TXERR = Field<OTG_HS_HCINT14_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_HS_HCINT14_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_HS_HCINT14_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_HS_HCINT14_Reg, 10, 1>; // Data toggle error
    };

    // OTG_HS host channel-14 interrupt mask register
    using OTG_HS_HCINTMSK14_Reg = Register<OTG_HS_HOST_BASE + 0x2B4, ReadWrite>;
    struct _OTG_HS_HCINTMSK14 {
        using XFRCM = Field<OTG_HS_HCINTMSK14_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_HS_HCINTMSK14_Reg, 1, 1>; // Channel halted mask
        using AHBERR = Field<OTG_HS_HCINTMSK14_Reg, 2, 1>; // AHB error
        using STALLM = Field<OTG_HS_HCINTMSK14_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_HS_HCINTMSK14_Reg, 4, 1>; // NAKM response received interrupt mask
        using ACKM = Field<OTG_HS_HCINTMSK14_Reg, 5, 1>; // ACKM response received/transmitted interrupt mask
        using NYET = Field<OTG_HS_HCINTMSK14_Reg, 6, 1>; // Response received interrupt
        using TXERRM = Field<OTG_HS_HCINTMSK14_Reg, 7, 1>; // Transaction error
        using BBERRM = Field<OTG_HS_HCINTMSK14_Reg, 8, 1>; // Babble error
        using FRMORM = Field<OTG_HS_HCINTMSK14_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_HS_HCINTMSK14_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_HS host channel-14 transfer size register
    using OTG_HS_HCTSIZ14_Reg = Register<OTG_HS_HOST_BASE + 0x2B8, ReadWrite>;
    struct _OTG_HS_HCTSIZ14 {
        using XFRSIZ = Field<OTG_HS_HCTSIZ14_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_HCTSIZ14_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_HS_HCTSIZ14_Reg, 29, 2>; // Data PID
    };

    // OTG_HS host channel-14 DMA address register
    using OTG_HS_HCDMA14_Reg = Register<OTG_HS_HOST_BASE + 0x2BC, ReadWrite>;
    struct _OTG_HS_HCDMA14 {
        using DMAADDR = Field<OTG_HS_HCDMA14_Reg, 0, 32>; // DMA address
    };

    // OTG_HS host channel-15 characteristics register
    using OTG_HS_HCCHAR15_Reg = Register<OTG_HS_HOST_BASE + 0x2C0, ReadWrite>;
    struct _OTG_HS_HCCHAR15 {
        using MPSIZ = Field<OTG_HS_HCCHAR15_Reg, 0, 11>; // Maximum packet size
        using EPNUM = Field<OTG_HS_HCCHAR15_Reg, 11, 4>; // Endpoint number
        using EPDIR = Field<OTG_HS_HCCHAR15_Reg, 15, 1>; // Endpoint direction
        using LSDEV = Field<OTG_HS_HCCHAR15_Reg, 17, 1>; // Low-speed device
        using EPTYP = Field<OTG_HS_HCCHAR15_Reg, 18, 2>; // Endpoint type
        using MC = Field<OTG_HS_HCCHAR15_Reg, 20, 2>; // Multi Count (MC) / Error Count (EC)
        using DAD = Field<OTG_HS_HCCHAR15_Reg, 22, 7>; // Device address
        using ODDFRM = Field<OTG_HS_HCCHAR15_Reg, 29, 1>; // Odd frame
        using CHDIS = Field<OTG_HS_HCCHAR15_Reg, 30, 1>; // Channel disable
        using CHENA = Field<OTG_HS_HCCHAR15_Reg, 31, 1>; // Channel enable
    };

    // OTG_HS host channel-15 split control register
    using OTG_HS_HCSPLT15_Reg = Register<OTG_HS_HOST_BASE + 0x2C4, ReadWrite>;
    struct _OTG_HS_HCSPLT15 {
        using PRTADDR = Field<OTG_HS_HCSPLT15_Reg, 0, 7>; // Port address
        using HUBADDR = Field<OTG_HS_HCSPLT15_Reg, 7, 7>; // Hub address
        using XACTPOS = Field<OTG_HS_HCSPLT15_Reg, 14, 2>; // XACTPOS
        using COMPLSPLT = Field<OTG_HS_HCSPLT15_Reg, 16, 1>; // Do complete split
        using SPLITEN = Field<OTG_HS_HCSPLT15_Reg, 31, 1>; // Split enable
    };

    // OTG_HS host channel-15 interrupt register
    using OTG_HS_HCINT15_Reg = Register<OTG_HS_HOST_BASE + 0x2C8, ReadWrite>;
    struct _OTG_HS_HCINT15 {
        using XFRC = Field<OTG_HS_HCINT15_Reg, 0, 1>; // Transfer completed
        using CHH = Field<OTG_HS_HCINT15_Reg, 1, 1>; // Channel halted
        using AHBERR = Field<OTG_HS_HCINT15_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINT15_Reg, 3, 1>; // STALL response received interrupt
        using NAK = Field<OTG_HS_HCINT15_Reg, 4, 1>; // NAK response received interrupt
        using ACK = Field<OTG_HS_HCINT15_Reg, 5, 1>; // ACK response received/transmitted interrupt
        using NYET = Field<OTG_HS_HCINT15_Reg, 6, 1>; // Response received interrupt
        using TXERR = Field<OTG_HS_HCINT15_Reg, 7, 1>; // Transaction error
        using BBERR = Field<OTG_HS_HCINT15_Reg, 8, 1>; // Babble error
        using FRMOR = Field<OTG_HS_HCINT15_Reg, 9, 1>; // Frame overrun
        using DTERR = Field<OTG_HS_HCINT15_Reg, 10, 1>; // Data toggle error
    };

    // OTG_HS host channel-15 interrupt mask register
    using OTG_HS_HCINTMSK15_Reg = Register<OTG_HS_HOST_BASE + 0x2CC, ReadWrite>;
    struct _OTG_HS_HCINTMSK15 {
        using XFRCM = Field<OTG_HS_HCINTMSK15_Reg, 0, 1>; // Transfer completed mask
        using CHHM = Field<OTG_HS_HCINTMSK15_Reg, 1, 1>; // Channel halted mask
        using AHBERR = Field<OTG_HS_HCINTMSK15_Reg, 2, 1>; // AHB error
        using STALL = Field<OTG_HS_HCINTMSK15_Reg, 3, 1>; // STALL response received interrupt mask
        using NAKM = Field<OTG_HS_HCINTMSK15_Reg, 4, 1>; // NAK response received interrupt mask
        using ACKM = Field<OTG_HS_HCINTMSK15_Reg, 5, 1>; // ACK response received/transmitted interrupt mask
        using NYET = Field<OTG_HS_HCINTMSK15_Reg, 6, 1>; // Response received interrupt
        using TXERRM = Field<OTG_HS_HCINTMSK15_Reg, 7, 1>; // Transaction error
        using BBERRM = Field<OTG_HS_HCINTMSK15_Reg, 8, 1>; // Babble error
        using FRMORM = Field<OTG_HS_HCINTMSK15_Reg, 9, 1>; // Frame overrun mask
        using DTERRM = Field<OTG_HS_HCINTMSK15_Reg, 10, 1>; // Data toggle error mask
    };

    // OTG_HS host channel-15 transfer size register
    using OTG_HS_HCTSIZ15_Reg = Register<OTG_HS_HOST_BASE + 0x2D0, ReadWrite>;
    struct _OTG_HS_HCTSIZ15 {
        using XFRSIZ = Field<OTG_HS_HCTSIZ15_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_HCTSIZ15_Reg, 19, 10>; // Packet count
        using DPID = Field<OTG_HS_HCTSIZ15_Reg, 29, 2>; // Data PID
    };

    // OTG_HS host channel-15 DMA address register
    using OTG_HS_HCDMA15_Reg = Register<OTG_HS_HOST_BASE + 0x2D4, ReadWrite>;
    struct _OTG_HS_HCDMA15 {
        using DMAADDR = Field<OTG_HS_HCDMA15_Reg, 0, 32>; // DMA address
    };

} // namespace OTG_HS_HOST

// --------------------------------------------
// OTG_HS_DEVICE: USB on the go high speed
// Base address: 0x40040800
// --------------------------------------------

namespace OTG_HS_DEVICE {
    static constexpr uint32_t OTG_HS_DEVICE_BASE = 0x40040800;

    // OTG_HS device configuration register
    using OTG_HS_DCFG_Reg = Register<OTG_HS_DEVICE_BASE + 0x0, ReadWrite>;
    struct _OTG_HS_DCFG {
        using DSPD = Field<OTG_HS_DCFG_Reg, 0, 2>; // Device speed
        using NZLSOHSK = Field<OTG_HS_DCFG_Reg, 2, 1>; // Nonzero-length status OUT handshake
        using DAD = Field<OTG_HS_DCFG_Reg, 4, 7>; // Device address
        using PFIVL = Field<OTG_HS_DCFG_Reg, 11, 2>; // Periodic (micro)frame interval
        using PERSCHIVL = Field<OTG_HS_DCFG_Reg, 24, 2>; // Periodic scheduling interval
    };

    // OTG_HS device control register
    using OTG_HS_DCTL_Reg = Register<OTG_HS_DEVICE_BASE + 0x4, ReadWrite>;
    struct _OTG_HS_DCTL {
        using RWUSIG = Field<OTG_HS_DCTL_Reg, 0, 1>; // Remote wakeup signaling
        using SDIS = Field<OTG_HS_DCTL_Reg, 1, 1>; // Soft disconnect
        using GINSTS = Field<OTG_HS_DCTL_Reg, 2, 1>; // Global IN NAK status
        using GONSTS = Field<OTG_HS_DCTL_Reg, 3, 1>; // Global OUT NAK status
        using TCTL = Field<OTG_HS_DCTL_Reg, 4, 3>; // Test control
        using SGINAK = Field<OTG_HS_DCTL_Reg, 7, 1>; // Set global IN NAK
        using CGINAK = Field<OTG_HS_DCTL_Reg, 8, 1>; // Clear global IN NAK
        using SGONAK = Field<OTG_HS_DCTL_Reg, 9, 1>; // Set global OUT NAK
        using CGONAK = Field<OTG_HS_DCTL_Reg, 10, 1>; // Clear global OUT NAK
        using POPRGDNE = Field<OTG_HS_DCTL_Reg, 11, 1>; // Power-on programming done
    };

    // OTG_HS device status register
    using OTG_HS_DSTS_Reg = Register<OTG_HS_DEVICE_BASE + 0x8, ReadOnly>;
    struct _OTG_HS_DSTS {
        using SUSPSTS = Field<OTG_HS_DSTS_Reg, 0, 1>; // Suspend status
        using ENUMSPD = Field<OTG_HS_DSTS_Reg, 1, 2>; // Enumerated speed
        using EERR = Field<OTG_HS_DSTS_Reg, 3, 1>; // Erratic error
        using FNSOF = Field<OTG_HS_DSTS_Reg, 8, 14>; // Frame number of the received SOF
    };

    // OTG_HS device IN endpoint common interrupt mask register
    using OTG_HS_DIEPMSK_Reg = Register<OTG_HS_DEVICE_BASE + 0x10, ReadWrite>;
    struct _OTG_HS_DIEPMSK {
        using XFRCM = Field<OTG_HS_DIEPMSK_Reg, 0, 1>; // Transfer completed interrupt mask
        using EPDM = Field<OTG_HS_DIEPMSK_Reg, 1, 1>; // Endpoint disabled interrupt mask
        using TOM = Field<OTG_HS_DIEPMSK_Reg, 3, 1>; // Timeout condition mask (nonisochronous endpoints)
        using ITTXFEMSK = Field<OTG_HS_DIEPMSK_Reg, 4, 1>; // IN token received when TxFIFO empty mask
        using INEPNMM = Field<OTG_HS_DIEPMSK_Reg, 5, 1>; // IN token received with EP mismatch mask
        using INEPNEM = Field<OTG_HS_DIEPMSK_Reg, 6, 1>; // IN endpoint NAK effective mask
        using TXFURM = Field<OTG_HS_DIEPMSK_Reg, 8, 1>; // FIFO underrun mask
        using BIM = Field<OTG_HS_DIEPMSK_Reg, 9, 1>; // BNA interrupt mask
    };

    // OTG_HS device OUT endpoint common interrupt mask register
    using OTG_HS_DOEPMSK_Reg = Register<OTG_HS_DEVICE_BASE + 0x14, ReadWrite>;
    struct _OTG_HS_DOEPMSK {
        using XFRCM = Field<OTG_HS_DOEPMSK_Reg, 0, 1>; // Transfer completed interrupt mask
        using EPDM = Field<OTG_HS_DOEPMSK_Reg, 1, 1>; // Endpoint disabled interrupt mask
        using STUPM = Field<OTG_HS_DOEPMSK_Reg, 3, 1>; // SETUP phase done mask
        using OTEPDM = Field<OTG_HS_DOEPMSK_Reg, 4, 1>; // OUT token received when endpoint disabled mask
        using B2BSTUP = Field<OTG_HS_DOEPMSK_Reg, 6, 1>; // Back-to-back SETUP packets received mask
        using OPEM = Field<OTG_HS_DOEPMSK_Reg, 8, 1>; // OUT packet error mask
        using BOIM = Field<OTG_HS_DOEPMSK_Reg, 9, 1>; // BNA interrupt mask
    };

    // OTG_HS device all endpoints interrupt register
    using OTG_HS_DAINT_Reg = Register<OTG_HS_DEVICE_BASE + 0x18, ReadOnly>;
    struct _OTG_HS_DAINT {
        using IEPINT = Field<OTG_HS_DAINT_Reg, 0, 16>; // IN endpoint interrupt bits
        using OEPINT = Field<OTG_HS_DAINT_Reg, 16, 16>; // OUT endpoint interrupt bits
    };

    // OTG_HS all endpoints interrupt mask register
    using OTG_HS_DAINTMSK_Reg = Register<OTG_HS_DEVICE_BASE + 0x1C, ReadWrite>;
    struct _OTG_HS_DAINTMSK {
        using IEPM = Field<OTG_HS_DAINTMSK_Reg, 0, 16>; // IN EP interrupt mask bits
        using OEPM = Field<OTG_HS_DAINTMSK_Reg, 16, 16>; // OUT EP interrupt mask bits
    };

    // OTG_HS device VBUS discharge time register
    using OTG_HS_DVBUSDIS_Reg = Register<OTG_HS_DEVICE_BASE + 0x28, ReadWrite>;
    struct _OTG_HS_DVBUSDIS {
        using VBUSDT = Field<OTG_HS_DVBUSDIS_Reg, 0, 16>; // Device VBUS discharge time
    };

    // OTG_HS device VBUS pulsing time register
    using OTG_HS_DVBUSPULSE_Reg = Register<OTG_HS_DEVICE_BASE + 0x2C, ReadWrite>;
    struct _OTG_HS_DVBUSPULSE {
        using DVBUSP = Field<OTG_HS_DVBUSPULSE_Reg, 0, 12>; // Device VBUS pulsing time
    };

    // OTG_HS Device threshold control register
    using OTG_HS_DTHRCTL_Reg = Register<OTG_HS_DEVICE_BASE + 0x30, ReadWrite>;
    struct _OTG_HS_DTHRCTL {
        using NONISOTHREN = Field<OTG_HS_DTHRCTL_Reg, 0, 1>; // Nonisochronous IN endpoints threshold enable
        using ISOTHREN = Field<OTG_HS_DTHRCTL_Reg, 1, 1>; // ISO IN endpoint threshold enable
        using TXTHRLEN = Field<OTG_HS_DTHRCTL_Reg, 2, 9>; // Transmit threshold length
        using RXTHREN = Field<OTG_HS_DTHRCTL_Reg, 16, 1>; // Receive threshold enable
        using RXTHRLEN = Field<OTG_HS_DTHRCTL_Reg, 17, 9>; // Receive threshold length
        using ARPEN = Field<OTG_HS_DTHRCTL_Reg, 27, 1>; // Arbiter parking enable
    };

    // OTG_HS device IN endpoint FIFO empty interrupt mask register
    using OTG_HS_DIEPEMPMSK_Reg = Register<OTG_HS_DEVICE_BASE + 0x34, ReadWrite>;
    struct _OTG_HS_DIEPEMPMSK {
        using INEPTXFEM = Field<OTG_HS_DIEPEMPMSK_Reg, 0, 16>; // IN EP Tx FIFO empty interrupt mask bits
    };

    // OTG_HS device each endpoint interrupt register
    using OTG_HS_DEACHINT_Reg = Register<OTG_HS_DEVICE_BASE + 0x38, ReadWrite>;
    struct _OTG_HS_DEACHINT {
        using IEP1INT = Field<OTG_HS_DEACHINT_Reg, 1, 1>; // IN endpoint 1interrupt bit
        using OEP1INT = Field<OTG_HS_DEACHINT_Reg, 17, 1>; // OUT endpoint 1 interrupt bit
    };

    // OTG_HS device each endpoint interrupt register mask
    using OTG_HS_DEACHINTMSK_Reg = Register<OTG_HS_DEVICE_BASE + 0x3C, ReadWrite>;
    struct _OTG_HS_DEACHINTMSK {
        using IEP1INTM = Field<OTG_HS_DEACHINTMSK_Reg, 1, 1>; // IN Endpoint 1 interrupt mask bit
        using OEP1INTM = Field<OTG_HS_DEACHINTMSK_Reg, 17, 1>; // OUT Endpoint 1 interrupt mask bit
    };

    // OTG device endpoint-0 control register
    using OTG_HS_DIEPCTL0_Reg = Register<OTG_HS_DEVICE_BASE + 0x100, ReadWrite>;
    struct _OTG_HS_DIEPCTL0 {
        using MPSIZ = Field<OTG_HS_DIEPCTL0_Reg, 0, 11>; // Maximum packet size
        using USBAEP = Field<OTG_HS_DIEPCTL0_Reg, 15, 1>; // USB active endpoint
        using EONUM_DPID = Field<OTG_HS_DIEPCTL0_Reg, 16, 1>; // Even/odd frame
        using NAKSTS = Field<OTG_HS_DIEPCTL0_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_HS_DIEPCTL0_Reg, 18, 2>; // Endpoint type
        using Stall = Field<OTG_HS_DIEPCTL0_Reg, 21, 1>; // STALL handshake
        using TXFNUM = Field<OTG_HS_DIEPCTL0_Reg, 22, 4>; // TxFIFO number
        using CNAK = Field<OTG_HS_DIEPCTL0_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_HS_DIEPCTL0_Reg, 27, 1>; // Set NAK
        using SD0PID_SEVNFRM = Field<OTG_HS_DIEPCTL0_Reg, 28, 1>; // Set DATA0 PID
        using SODDFRM = Field<OTG_HS_DIEPCTL0_Reg, 29, 1>; // Set odd frame
        using EPDIS = Field<OTG_HS_DIEPCTL0_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_HS_DIEPCTL0_Reg, 31, 1>; // Endpoint enable
    };

    // OTG device endpoint-1 control register
    using OTG_HS_DIEPCTL1_Reg = Register<OTG_HS_DEVICE_BASE + 0x120, ReadWrite>;
    struct _OTG_HS_DIEPCTL1 {
        using MPSIZ = Field<OTG_HS_DIEPCTL1_Reg, 0, 11>; // Maximum packet size
        using USBAEP = Field<OTG_HS_DIEPCTL1_Reg, 15, 1>; // USB active endpoint
        using EONUM_DPID = Field<OTG_HS_DIEPCTL1_Reg, 16, 1>; // Even/odd frame
        using NAKSTS = Field<OTG_HS_DIEPCTL1_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_HS_DIEPCTL1_Reg, 18, 2>; // Endpoint type
        using Stall = Field<OTG_HS_DIEPCTL1_Reg, 21, 1>; // STALL handshake
        using TXFNUM = Field<OTG_HS_DIEPCTL1_Reg, 22, 4>; // TxFIFO number
        using CNAK = Field<OTG_HS_DIEPCTL1_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_HS_DIEPCTL1_Reg, 27, 1>; // Set NAK
        using SD0PID_SEVNFRM = Field<OTG_HS_DIEPCTL1_Reg, 28, 1>; // Set DATA0 PID
        using SODDFRM = Field<OTG_HS_DIEPCTL1_Reg, 29, 1>; // Set odd frame
        using EPDIS = Field<OTG_HS_DIEPCTL1_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_HS_DIEPCTL1_Reg, 31, 1>; // Endpoint enable
    };

    // OTG device endpoint-2 control register
    using OTG_HS_DIEPCTL2_Reg = Register<OTG_HS_DEVICE_BASE + 0x140, ReadWrite>;
    struct _OTG_HS_DIEPCTL2 {
        using MPSIZ = Field<OTG_HS_DIEPCTL2_Reg, 0, 11>; // Maximum packet size
        using USBAEP = Field<OTG_HS_DIEPCTL2_Reg, 15, 1>; // USB active endpoint
        using EONUM_DPID = Field<OTG_HS_DIEPCTL2_Reg, 16, 1>; // Even/odd frame
        using NAKSTS = Field<OTG_HS_DIEPCTL2_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_HS_DIEPCTL2_Reg, 18, 2>; // Endpoint type
        using Stall = Field<OTG_HS_DIEPCTL2_Reg, 21, 1>; // STALL handshake
        using TXFNUM = Field<OTG_HS_DIEPCTL2_Reg, 22, 4>; // TxFIFO number
        using CNAK = Field<OTG_HS_DIEPCTL2_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_HS_DIEPCTL2_Reg, 27, 1>; // Set NAK
        using SD0PID_SEVNFRM = Field<OTG_HS_DIEPCTL2_Reg, 28, 1>; // Set DATA0 PID
        using SODDFRM = Field<OTG_HS_DIEPCTL2_Reg, 29, 1>; // Set odd frame
        using EPDIS = Field<OTG_HS_DIEPCTL2_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_HS_DIEPCTL2_Reg, 31, 1>; // Endpoint enable
    };

    // OTG device endpoint-3 control register
    using OTG_HS_DIEPCTL3_Reg = Register<OTG_HS_DEVICE_BASE + 0x160, ReadWrite>;
    struct _OTG_HS_DIEPCTL3 {
        using MPSIZ = Field<OTG_HS_DIEPCTL3_Reg, 0, 11>; // Maximum packet size
        using USBAEP = Field<OTG_HS_DIEPCTL3_Reg, 15, 1>; // USB active endpoint
        using EONUM_DPID = Field<OTG_HS_DIEPCTL3_Reg, 16, 1>; // Even/odd frame
        using NAKSTS = Field<OTG_HS_DIEPCTL3_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_HS_DIEPCTL3_Reg, 18, 2>; // Endpoint type
        using Stall = Field<OTG_HS_DIEPCTL3_Reg, 21, 1>; // STALL handshake
        using TXFNUM = Field<OTG_HS_DIEPCTL3_Reg, 22, 4>; // TxFIFO number
        using CNAK = Field<OTG_HS_DIEPCTL3_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_HS_DIEPCTL3_Reg, 27, 1>; // Set NAK
        using SD0PID_SEVNFRM = Field<OTG_HS_DIEPCTL3_Reg, 28, 1>; // Set DATA0 PID
        using SODDFRM = Field<OTG_HS_DIEPCTL3_Reg, 29, 1>; // Set odd frame
        using EPDIS = Field<OTG_HS_DIEPCTL3_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_HS_DIEPCTL3_Reg, 31, 1>; // Endpoint enable
    };

    // OTG device endpoint-4 control register
    using OTG_HS_DIEPCTL4_Reg = Register<OTG_HS_DEVICE_BASE + 0x180, ReadWrite>;
    struct _OTG_HS_DIEPCTL4 {
        using MPSIZ = Field<OTG_HS_DIEPCTL4_Reg, 0, 11>; // Maximum packet size
        using USBAEP = Field<OTG_HS_DIEPCTL4_Reg, 15, 1>; // USB active endpoint
        using EONUM_DPID = Field<OTG_HS_DIEPCTL4_Reg, 16, 1>; // Even/odd frame
        using NAKSTS = Field<OTG_HS_DIEPCTL4_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_HS_DIEPCTL4_Reg, 18, 2>; // Endpoint type
        using Stall = Field<OTG_HS_DIEPCTL4_Reg, 21, 1>; // STALL handshake
        using TXFNUM = Field<OTG_HS_DIEPCTL4_Reg, 22, 4>; // TxFIFO number
        using CNAK = Field<OTG_HS_DIEPCTL4_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_HS_DIEPCTL4_Reg, 27, 1>; // Set NAK
        using SD0PID_SEVNFRM = Field<OTG_HS_DIEPCTL4_Reg, 28, 1>; // Set DATA0 PID
        using SODDFRM = Field<OTG_HS_DIEPCTL4_Reg, 29, 1>; // Set odd frame
        using EPDIS = Field<OTG_HS_DIEPCTL4_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_HS_DIEPCTL4_Reg, 31, 1>; // Endpoint enable
    };

    // OTG device endpoint-5 control register
    using OTG_HS_DIEPCTL5_Reg = Register<OTG_HS_DEVICE_BASE + 0x1A0, ReadWrite>;
    struct _OTG_HS_DIEPCTL5 {
        using MPSIZ = Field<OTG_HS_DIEPCTL5_Reg, 0, 11>; // Maximum packet size
        using USBAEP = Field<OTG_HS_DIEPCTL5_Reg, 15, 1>; // USB active endpoint
        using EONUM_DPID = Field<OTG_HS_DIEPCTL5_Reg, 16, 1>; // Even/odd frame
        using NAKSTS = Field<OTG_HS_DIEPCTL5_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_HS_DIEPCTL5_Reg, 18, 2>; // Endpoint type
        using Stall = Field<OTG_HS_DIEPCTL5_Reg, 21, 1>; // STALL handshake
        using TXFNUM = Field<OTG_HS_DIEPCTL5_Reg, 22, 4>; // TxFIFO number
        using CNAK = Field<OTG_HS_DIEPCTL5_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_HS_DIEPCTL5_Reg, 27, 1>; // Set NAK
        using SD0PID_SEVNFRM = Field<OTG_HS_DIEPCTL5_Reg, 28, 1>; // Set DATA0 PID
        using SODDFRM = Field<OTG_HS_DIEPCTL5_Reg, 29, 1>; // Set odd frame
        using EPDIS = Field<OTG_HS_DIEPCTL5_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_HS_DIEPCTL5_Reg, 31, 1>; // Endpoint enable
    };

    // OTG device endpoint-6 control register
    using OTG_HS_DIEPCTL6_Reg = Register<OTG_HS_DEVICE_BASE + 0x1C0, ReadWrite>;
    struct _OTG_HS_DIEPCTL6 {
        using MPSIZ = Field<OTG_HS_DIEPCTL6_Reg, 0, 11>; // Maximum packet size
        using USBAEP = Field<OTG_HS_DIEPCTL6_Reg, 15, 1>; // USB active endpoint
        using EONUM_DPID = Field<OTG_HS_DIEPCTL6_Reg, 16, 1>; // Even/odd frame
        using NAKSTS = Field<OTG_HS_DIEPCTL6_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_HS_DIEPCTL6_Reg, 18, 2>; // Endpoint type
        using Stall = Field<OTG_HS_DIEPCTL6_Reg, 21, 1>; // STALL handshake
        using TXFNUM = Field<OTG_HS_DIEPCTL6_Reg, 22, 4>; // TxFIFO number
        using CNAK = Field<OTG_HS_DIEPCTL6_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_HS_DIEPCTL6_Reg, 27, 1>; // Set NAK
        using SD0PID_SEVNFRM = Field<OTG_HS_DIEPCTL6_Reg, 28, 1>; // Set DATA0 PID
        using SODDFRM = Field<OTG_HS_DIEPCTL6_Reg, 29, 1>; // Set odd frame
        using EPDIS = Field<OTG_HS_DIEPCTL6_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_HS_DIEPCTL6_Reg, 31, 1>; // Endpoint enable
    };

    // OTG device endpoint-7 control register
    using OTG_HS_DIEPCTL7_Reg = Register<OTG_HS_DEVICE_BASE + 0x1E0, ReadWrite>;
    struct _OTG_HS_DIEPCTL7 {
        using MPSIZ = Field<OTG_HS_DIEPCTL7_Reg, 0, 11>; // Maximum packet size
        using USBAEP = Field<OTG_HS_DIEPCTL7_Reg, 15, 1>; // USB active endpoint
        using EONUM_DPID = Field<OTG_HS_DIEPCTL7_Reg, 16, 1>; // Even/odd frame
        using NAKSTS = Field<OTG_HS_DIEPCTL7_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_HS_DIEPCTL7_Reg, 18, 2>; // Endpoint type
        using Stall = Field<OTG_HS_DIEPCTL7_Reg, 21, 1>; // STALL handshake
        using TXFNUM = Field<OTG_HS_DIEPCTL7_Reg, 22, 4>; // TxFIFO number
        using CNAK = Field<OTG_HS_DIEPCTL7_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_HS_DIEPCTL7_Reg, 27, 1>; // Set NAK
        using SD0PID_SEVNFRM = Field<OTG_HS_DIEPCTL7_Reg, 28, 1>; // Set DATA0 PID
        using SODDFRM = Field<OTG_HS_DIEPCTL7_Reg, 29, 1>; // Set odd frame
        using EPDIS = Field<OTG_HS_DIEPCTL7_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_HS_DIEPCTL7_Reg, 31, 1>; // Endpoint enable
    };

    // OTG device endpoint-0 interrupt register
    using OTG_HS_DIEPINT0_Reg = Register<OTG_HS_DEVICE_BASE + 0x108, ReadWrite>;
    struct _OTG_HS_DIEPINT0 {
        using XFRC = Field<OTG_HS_DIEPINT0_Reg, 0, 1>; // Transfer completed interrupt
        using EPDISD = Field<OTG_HS_DIEPINT0_Reg, 1, 1>; // Endpoint disabled interrupt
        using TOC = Field<OTG_HS_DIEPINT0_Reg, 3, 1>; // Timeout condition
        using ITTXFE = Field<OTG_HS_DIEPINT0_Reg, 4, 1>; // IN token received when TxFIFO is empty
        using INEPNE = Field<OTG_HS_DIEPINT0_Reg, 6, 1>; // IN endpoint NAK effective
        using TXFE = Field<OTG_HS_DIEPINT0_Reg, 7, 1>; // Transmit FIFO empty
        using TXFIFOUDRN = Field<OTG_HS_DIEPINT0_Reg, 8, 1>; // Transmit Fifo Underrun
        using BNA = Field<OTG_HS_DIEPINT0_Reg, 9, 1>; // Buffer not available interrupt
        using PKTDRPSTS = Field<OTG_HS_DIEPINT0_Reg, 11, 1>; // Packet dropped status
        using BERR = Field<OTG_HS_DIEPINT0_Reg, 12, 1>; // Babble error interrupt
        using NAK = Field<OTG_HS_DIEPINT0_Reg, 13, 1>; // NAK interrupt
    };

    // OTG device endpoint-1 interrupt register
    using OTG_HS_DIEPINT1_Reg = Register<OTG_HS_DEVICE_BASE + 0x128, ReadWrite>;
    struct _OTG_HS_DIEPINT1 {
        using XFRC = Field<OTG_HS_DIEPINT1_Reg, 0, 1>; // Transfer completed interrupt
        using EPDISD = Field<OTG_HS_DIEPINT1_Reg, 1, 1>; // Endpoint disabled interrupt
        using TOC = Field<OTG_HS_DIEPINT1_Reg, 3, 1>; // Timeout condition
        using ITTXFE = Field<OTG_HS_DIEPINT1_Reg, 4, 1>; // IN token received when TxFIFO is empty
        using INEPNE = Field<OTG_HS_DIEPINT1_Reg, 6, 1>; // IN endpoint NAK effective
        using TXFE = Field<OTG_HS_DIEPINT1_Reg, 7, 1>; // Transmit FIFO empty
        using TXFIFOUDRN = Field<OTG_HS_DIEPINT1_Reg, 8, 1>; // Transmit Fifo Underrun
        using BNA = Field<OTG_HS_DIEPINT1_Reg, 9, 1>; // Buffer not available interrupt
        using PKTDRPSTS = Field<OTG_HS_DIEPINT1_Reg, 11, 1>; // Packet dropped status
        using BERR = Field<OTG_HS_DIEPINT1_Reg, 12, 1>; // Babble error interrupt
        using NAK = Field<OTG_HS_DIEPINT1_Reg, 13, 1>; // NAK interrupt
    };

    // OTG device endpoint-2 interrupt register
    using OTG_HS_DIEPINT2_Reg = Register<OTG_HS_DEVICE_BASE + 0x148, ReadWrite>;
    struct _OTG_HS_DIEPINT2 {
        using XFRC = Field<OTG_HS_DIEPINT2_Reg, 0, 1>; // Transfer completed interrupt
        using EPDISD = Field<OTG_HS_DIEPINT2_Reg, 1, 1>; // Endpoint disabled interrupt
        using TOC = Field<OTG_HS_DIEPINT2_Reg, 3, 1>; // Timeout condition
        using ITTXFE = Field<OTG_HS_DIEPINT2_Reg, 4, 1>; // IN token received when TxFIFO is empty
        using INEPNE = Field<OTG_HS_DIEPINT2_Reg, 6, 1>; // IN endpoint NAK effective
        using TXFE = Field<OTG_HS_DIEPINT2_Reg, 7, 1>; // Transmit FIFO empty
        using TXFIFOUDRN = Field<OTG_HS_DIEPINT2_Reg, 8, 1>; // Transmit Fifo Underrun
        using BNA = Field<OTG_HS_DIEPINT2_Reg, 9, 1>; // Buffer not available interrupt
        using PKTDRPSTS = Field<OTG_HS_DIEPINT2_Reg, 11, 1>; // Packet dropped status
        using BERR = Field<OTG_HS_DIEPINT2_Reg, 12, 1>; // Babble error interrupt
        using NAK = Field<OTG_HS_DIEPINT2_Reg, 13, 1>; // NAK interrupt
    };

    // OTG device endpoint-3 interrupt register
    using OTG_HS_DIEPINT3_Reg = Register<OTG_HS_DEVICE_BASE + 0x168, ReadWrite>;
    struct _OTG_HS_DIEPINT3 {
        using XFRC = Field<OTG_HS_DIEPINT3_Reg, 0, 1>; // Transfer completed interrupt
        using EPDISD = Field<OTG_HS_DIEPINT3_Reg, 1, 1>; // Endpoint disabled interrupt
        using TOC = Field<OTG_HS_DIEPINT3_Reg, 3, 1>; // Timeout condition
        using ITTXFE = Field<OTG_HS_DIEPINT3_Reg, 4, 1>; // IN token received when TxFIFO is empty
        using INEPNE = Field<OTG_HS_DIEPINT3_Reg, 6, 1>; // IN endpoint NAK effective
        using TXFE = Field<OTG_HS_DIEPINT3_Reg, 7, 1>; // Transmit FIFO empty
        using TXFIFOUDRN = Field<OTG_HS_DIEPINT3_Reg, 8, 1>; // Transmit Fifo Underrun
        using BNA = Field<OTG_HS_DIEPINT3_Reg, 9, 1>; // Buffer not available interrupt
        using PKTDRPSTS = Field<OTG_HS_DIEPINT3_Reg, 11, 1>; // Packet dropped status
        using BERR = Field<OTG_HS_DIEPINT3_Reg, 12, 1>; // Babble error interrupt
        using NAK = Field<OTG_HS_DIEPINT3_Reg, 13, 1>; // NAK interrupt
    };

    // OTG device endpoint-4 interrupt register
    using OTG_HS_DIEPINT4_Reg = Register<OTG_HS_DEVICE_BASE + 0x188, ReadWrite>;
    struct _OTG_HS_DIEPINT4 {
        using XFRC = Field<OTG_HS_DIEPINT4_Reg, 0, 1>; // Transfer completed interrupt
        using EPDISD = Field<OTG_HS_DIEPINT4_Reg, 1, 1>; // Endpoint disabled interrupt
        using TOC = Field<OTG_HS_DIEPINT4_Reg, 3, 1>; // Timeout condition
        using ITTXFE = Field<OTG_HS_DIEPINT4_Reg, 4, 1>; // IN token received when TxFIFO is empty
        using INEPNE = Field<OTG_HS_DIEPINT4_Reg, 6, 1>; // IN endpoint NAK effective
        using TXFE = Field<OTG_HS_DIEPINT4_Reg, 7, 1>; // Transmit FIFO empty
        using TXFIFOUDRN = Field<OTG_HS_DIEPINT4_Reg, 8, 1>; // Transmit Fifo Underrun
        using BNA = Field<OTG_HS_DIEPINT4_Reg, 9, 1>; // Buffer not available interrupt
        using PKTDRPSTS = Field<OTG_HS_DIEPINT4_Reg, 11, 1>; // Packet dropped status
        using BERR = Field<OTG_HS_DIEPINT4_Reg, 12, 1>; // Babble error interrupt
        using NAK = Field<OTG_HS_DIEPINT4_Reg, 13, 1>; // NAK interrupt
    };

    // OTG device endpoint-5 interrupt register
    using OTG_HS_DIEPINT5_Reg = Register<OTG_HS_DEVICE_BASE + 0x1A8, ReadWrite>;
    struct _OTG_HS_DIEPINT5 {
        using XFRC = Field<OTG_HS_DIEPINT5_Reg, 0, 1>; // Transfer completed interrupt
        using EPDISD = Field<OTG_HS_DIEPINT5_Reg, 1, 1>; // Endpoint disabled interrupt
        using TOC = Field<OTG_HS_DIEPINT5_Reg, 3, 1>; // Timeout condition
        using ITTXFE = Field<OTG_HS_DIEPINT5_Reg, 4, 1>; // IN token received when TxFIFO is empty
        using INEPNE = Field<OTG_HS_DIEPINT5_Reg, 6, 1>; // IN endpoint NAK effective
        using TXFE = Field<OTG_HS_DIEPINT5_Reg, 7, 1>; // Transmit FIFO empty
        using TXFIFOUDRN = Field<OTG_HS_DIEPINT5_Reg, 8, 1>; // Transmit Fifo Underrun
        using BNA = Field<OTG_HS_DIEPINT5_Reg, 9, 1>; // Buffer not available interrupt
        using PKTDRPSTS = Field<OTG_HS_DIEPINT5_Reg, 11, 1>; // Packet dropped status
        using BERR = Field<OTG_HS_DIEPINT5_Reg, 12, 1>; // Babble error interrupt
        using NAK = Field<OTG_HS_DIEPINT5_Reg, 13, 1>; // NAK interrupt
    };

    // OTG device endpoint-6 interrupt register
    using OTG_HS_DIEPINT6_Reg = Register<OTG_HS_DEVICE_BASE + 0x1C8, ReadWrite>;
    struct _OTG_HS_DIEPINT6 {
        using XFRC = Field<OTG_HS_DIEPINT6_Reg, 0, 1>; // Transfer completed interrupt
        using EPDISD = Field<OTG_HS_DIEPINT6_Reg, 1, 1>; // Endpoint disabled interrupt
        using TOC = Field<OTG_HS_DIEPINT6_Reg, 3, 1>; // Timeout condition
        using ITTXFE = Field<OTG_HS_DIEPINT6_Reg, 4, 1>; // IN token received when TxFIFO is empty
        using INEPNE = Field<OTG_HS_DIEPINT6_Reg, 6, 1>; // IN endpoint NAK effective
        using TXFE = Field<OTG_HS_DIEPINT6_Reg, 7, 1>; // Transmit FIFO empty
        using TXFIFOUDRN = Field<OTG_HS_DIEPINT6_Reg, 8, 1>; // Transmit Fifo Underrun
        using BNA = Field<OTG_HS_DIEPINT6_Reg, 9, 1>; // Buffer not available interrupt
        using PKTDRPSTS = Field<OTG_HS_DIEPINT6_Reg, 11, 1>; // Packet dropped status
        using BERR = Field<OTG_HS_DIEPINT6_Reg, 12, 1>; // Babble error interrupt
        using NAK = Field<OTG_HS_DIEPINT6_Reg, 13, 1>; // NAK interrupt
    };

    // OTG device endpoint-7 interrupt register
    using OTG_HS_DIEPINT7_Reg = Register<OTG_HS_DEVICE_BASE + 0x1E8, ReadWrite>;
    struct _OTG_HS_DIEPINT7 {
        using XFRC = Field<OTG_HS_DIEPINT7_Reg, 0, 1>; // Transfer completed interrupt
        using EPDISD = Field<OTG_HS_DIEPINT7_Reg, 1, 1>; // Endpoint disabled interrupt
        using TOC = Field<OTG_HS_DIEPINT7_Reg, 3, 1>; // Timeout condition
        using ITTXFE = Field<OTG_HS_DIEPINT7_Reg, 4, 1>; // IN token received when TxFIFO is empty
        using INEPNE = Field<OTG_HS_DIEPINT7_Reg, 6, 1>; // IN endpoint NAK effective
        using TXFE = Field<OTG_HS_DIEPINT7_Reg, 7, 1>; // Transmit FIFO empty
        using TXFIFOUDRN = Field<OTG_HS_DIEPINT7_Reg, 8, 1>; // Transmit Fifo Underrun
        using BNA = Field<OTG_HS_DIEPINT7_Reg, 9, 1>; // Buffer not available interrupt
        using PKTDRPSTS = Field<OTG_HS_DIEPINT7_Reg, 11, 1>; // Packet dropped status
        using BERR = Field<OTG_HS_DIEPINT7_Reg, 12, 1>; // Babble error interrupt
        using NAK = Field<OTG_HS_DIEPINT7_Reg, 13, 1>; // NAK interrupt
    };

    // OTG_HS device IN endpoint 0 transfer size register
    using OTG_HS_DIEPTSIZ0_Reg = Register<OTG_HS_DEVICE_BASE + 0x110, ReadWrite>;
    struct _OTG_HS_DIEPTSIZ0 {
        using XFRSIZ = Field<OTG_HS_DIEPTSIZ0_Reg, 0, 7>; // Transfer size
        using PKTCNT = Field<OTG_HS_DIEPTSIZ0_Reg, 19, 2>; // Packet count
    };

    // OTG_HS device endpoint-1 DMA address register
    using OTG_HS_DIEPDMA1_Reg = Register<OTG_HS_DEVICE_BASE + 0x114, ReadWrite>;
    struct _OTG_HS_DIEPDMA1 {
        using DMAADDR = Field<OTG_HS_DIEPDMA1_Reg, 0, 32>; // DMA address
    };

    // OTG_HS device endpoint-2 DMA address register
    using OTG_HS_DIEPDMA2_Reg = Register<OTG_HS_DEVICE_BASE + 0x134, ReadWrite>;
    struct _OTG_HS_DIEPDMA2 {
        using DMAADDR = Field<OTG_HS_DIEPDMA2_Reg, 0, 32>; // DMA address
    };

    // OTG_HS device endpoint-3 DMA address register
    using OTG_HS_DIEPDMA3_Reg = Register<OTG_HS_DEVICE_BASE + 0x154, ReadWrite>;
    struct _OTG_HS_DIEPDMA3 {
        using DMAADDR = Field<OTG_HS_DIEPDMA3_Reg, 0, 32>; // DMA address
    };

    // OTG_HS device endpoint-4 DMA address register
    using OTG_HS_DIEPDMA4_Reg = Register<OTG_HS_DEVICE_BASE + 0x174, ReadWrite>;
    struct _OTG_HS_DIEPDMA4 {
        using DMAADDR = Field<OTG_HS_DIEPDMA4_Reg, 0, 32>; // DMA address
    };

    // OTG_HS device endpoint-5 DMA address register
    using OTG_HS_DIEPDMA5_Reg = Register<OTG_HS_DEVICE_BASE + 0x194, ReadWrite>;
    struct _OTG_HS_DIEPDMA5 {
        using DMAADDR = Field<OTG_HS_DIEPDMA5_Reg, 0, 32>; // DMA address
    };

    // OTG_HS device IN endpoint transmit FIFO status register
    using OTG_HS_DTXFSTS0_Reg = Register<OTG_HS_DEVICE_BASE + 0x118, ReadOnly>;
    struct _OTG_HS_DTXFSTS0 {
        using INEPTFSAV = Field<OTG_HS_DTXFSTS0_Reg, 0, 16>; // IN endpoint TxFIFO space avail
    };

    // OTG_HS device IN endpoint transmit FIFO status register
    using OTG_HS_DTXFSTS1_Reg = Register<OTG_HS_DEVICE_BASE + 0x138, ReadOnly>;
    struct _OTG_HS_DTXFSTS1 {
        using INEPTFSAV = Field<OTG_HS_DTXFSTS1_Reg, 0, 16>; // IN endpoint TxFIFO space avail
    };

    // OTG_HS device IN endpoint transmit FIFO status register
    using OTG_HS_DTXFSTS2_Reg = Register<OTG_HS_DEVICE_BASE + 0x158, ReadOnly>;
    struct _OTG_HS_DTXFSTS2 {
        using INEPTFSAV = Field<OTG_HS_DTXFSTS2_Reg, 0, 16>; // IN endpoint TxFIFO space avail
    };

    // OTG_HS device IN endpoint transmit FIFO status register
    using OTG_HS_DTXFSTS3_Reg = Register<OTG_HS_DEVICE_BASE + 0x178, ReadOnly>;
    struct _OTG_HS_DTXFSTS3 {
        using INEPTFSAV = Field<OTG_HS_DTXFSTS3_Reg, 0, 16>; // IN endpoint TxFIFO space avail
    };

    // OTG_HS device IN endpoint transmit FIFO status register
    using OTG_HS_DTXFSTS4_Reg = Register<OTG_HS_DEVICE_BASE + 0x198, ReadOnly>;
    struct _OTG_HS_DTXFSTS4 {
        using INEPTFSAV = Field<OTG_HS_DTXFSTS4_Reg, 0, 16>; // IN endpoint TxFIFO space avail
    };

    // OTG_HS device IN endpoint transmit FIFO status register
    using OTG_HS_DTXFSTS5_Reg = Register<OTG_HS_DEVICE_BASE + 0x1B8, ReadOnly>;
    struct _OTG_HS_DTXFSTS5 {
        using INEPTFSAV = Field<OTG_HS_DTXFSTS5_Reg, 0, 16>; // IN endpoint TxFIFO space avail
    };

    // OTG_HS device endpoint transfer size register
    using OTG_HS_DIEPTSIZ1_Reg = Register<OTG_HS_DEVICE_BASE + 0x130, ReadWrite>;
    struct _OTG_HS_DIEPTSIZ1 {
        using XFRSIZ = Field<OTG_HS_DIEPTSIZ1_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_DIEPTSIZ1_Reg, 19, 10>; // Packet count
        using MCNT = Field<OTG_HS_DIEPTSIZ1_Reg, 29, 2>; // Multi count
    };

    // OTG_HS device endpoint transfer size register
    using OTG_HS_DIEPTSIZ2_Reg = Register<OTG_HS_DEVICE_BASE + 0x150, ReadWrite>;
    struct _OTG_HS_DIEPTSIZ2 {
        using XFRSIZ = Field<OTG_HS_DIEPTSIZ2_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_DIEPTSIZ2_Reg, 19, 10>; // Packet count
        using MCNT = Field<OTG_HS_DIEPTSIZ2_Reg, 29, 2>; // Multi count
    };

    // OTG_HS device endpoint transfer size register
    using OTG_HS_DIEPTSIZ3_Reg = Register<OTG_HS_DEVICE_BASE + 0x170, ReadWrite>;
    struct _OTG_HS_DIEPTSIZ3 {
        using XFRSIZ = Field<OTG_HS_DIEPTSIZ3_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_DIEPTSIZ3_Reg, 19, 10>; // Packet count
        using MCNT = Field<OTG_HS_DIEPTSIZ3_Reg, 29, 2>; // Multi count
    };

    // OTG_HS device endpoint transfer size register
    using OTG_HS_DIEPTSIZ4_Reg = Register<OTG_HS_DEVICE_BASE + 0x190, ReadWrite>;
    struct _OTG_HS_DIEPTSIZ4 {
        using XFRSIZ = Field<OTG_HS_DIEPTSIZ4_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_DIEPTSIZ4_Reg, 19, 10>; // Packet count
        using MCNT = Field<OTG_HS_DIEPTSIZ4_Reg, 29, 2>; // Multi count
    };

    // OTG_HS device endpoint transfer size register
    using OTG_HS_DIEPTSIZ5_Reg = Register<OTG_HS_DEVICE_BASE + 0x1B0, ReadWrite>;
    struct _OTG_HS_DIEPTSIZ5 {
        using XFRSIZ = Field<OTG_HS_DIEPTSIZ5_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_DIEPTSIZ5_Reg, 19, 10>; // Packet count
        using MCNT = Field<OTG_HS_DIEPTSIZ5_Reg, 29, 2>; // Multi count
    };

    // OTG_HS device control OUT endpoint 0 control register
    using OTG_HS_DOEPCTL0_Reg = Register<OTG_HS_DEVICE_BASE + 0x300, ReadWrite>;
    struct _OTG_HS_DOEPCTL0 {
        using MPSIZ = Field<OTG_HS_DOEPCTL0_Reg, 0, 2>; // Maximum packet size
        using USBAEP = Field<OTG_HS_DOEPCTL0_Reg, 15, 1>; // USB active endpoint
        using NAKSTS = Field<OTG_HS_DOEPCTL0_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_HS_DOEPCTL0_Reg, 18, 2>; // Endpoint type
        using SNPM = Field<OTG_HS_DOEPCTL0_Reg, 20, 1>; // Snoop mode
        using Stall = Field<OTG_HS_DOEPCTL0_Reg, 21, 1>; // STALL handshake
        using CNAK = Field<OTG_HS_DOEPCTL0_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_HS_DOEPCTL0_Reg, 27, 1>; // Set NAK
        using EPDIS = Field<OTG_HS_DOEPCTL0_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_HS_DOEPCTL0_Reg, 31, 1>; // Endpoint enable
    };

    // OTG device endpoint-1 control register
    using OTG_HS_DOEPCTL1_Reg = Register<OTG_HS_DEVICE_BASE + 0x320, ReadWrite>;
    struct _OTG_HS_DOEPCTL1 {
        using MPSIZ = Field<OTG_HS_DOEPCTL1_Reg, 0, 11>; // Maximum packet size
        using USBAEP = Field<OTG_HS_DOEPCTL1_Reg, 15, 1>; // USB active endpoint
        using EONUM_DPID = Field<OTG_HS_DOEPCTL1_Reg, 16, 1>; // Even odd frame/Endpoint data PID
        using NAKSTS = Field<OTG_HS_DOEPCTL1_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_HS_DOEPCTL1_Reg, 18, 2>; // Endpoint type
        using SNPM = Field<OTG_HS_DOEPCTL1_Reg, 20, 1>; // Snoop mode
        using Stall = Field<OTG_HS_DOEPCTL1_Reg, 21, 1>; // STALL handshake
        using CNAK = Field<OTG_HS_DOEPCTL1_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_HS_DOEPCTL1_Reg, 27, 1>; // Set NAK
        using SD0PID_SEVNFRM = Field<OTG_HS_DOEPCTL1_Reg, 28, 1>; // Set DATA0 PID/Set even frame
        using SODDFRM = Field<OTG_HS_DOEPCTL1_Reg, 29, 1>; // Set odd frame
        using EPDIS = Field<OTG_HS_DOEPCTL1_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_HS_DOEPCTL1_Reg, 31, 1>; // Endpoint enable
    };

    // OTG device endpoint-2 control register
    using OTG_HS_DOEPCTL2_Reg = Register<OTG_HS_DEVICE_BASE + 0x340, ReadWrite>;
    struct _OTG_HS_DOEPCTL2 {
        using MPSIZ = Field<OTG_HS_DOEPCTL2_Reg, 0, 11>; // Maximum packet size
        using USBAEP = Field<OTG_HS_DOEPCTL2_Reg, 15, 1>; // USB active endpoint
        using EONUM_DPID = Field<OTG_HS_DOEPCTL2_Reg, 16, 1>; // Even odd frame/Endpoint data PID
        using NAKSTS = Field<OTG_HS_DOEPCTL2_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_HS_DOEPCTL2_Reg, 18, 2>; // Endpoint type
        using SNPM = Field<OTG_HS_DOEPCTL2_Reg, 20, 1>; // Snoop mode
        using Stall = Field<OTG_HS_DOEPCTL2_Reg, 21, 1>; // STALL handshake
        using CNAK = Field<OTG_HS_DOEPCTL2_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_HS_DOEPCTL2_Reg, 27, 1>; // Set NAK
        using SD0PID_SEVNFRM = Field<OTG_HS_DOEPCTL2_Reg, 28, 1>; // Set DATA0 PID/Set even frame
        using SODDFRM = Field<OTG_HS_DOEPCTL2_Reg, 29, 1>; // Set odd frame
        using EPDIS = Field<OTG_HS_DOEPCTL2_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_HS_DOEPCTL2_Reg, 31, 1>; // Endpoint enable
    };

    // OTG device endpoint-3 control register
    using OTG_HS_DOEPCTL3_Reg = Register<OTG_HS_DEVICE_BASE + 0x360, ReadWrite>;
    struct _OTG_HS_DOEPCTL3 {
        using MPSIZ = Field<OTG_HS_DOEPCTL3_Reg, 0, 11>; // Maximum packet size
        using USBAEP = Field<OTG_HS_DOEPCTL3_Reg, 15, 1>; // USB active endpoint
        using EONUM_DPID = Field<OTG_HS_DOEPCTL3_Reg, 16, 1>; // Even odd frame/Endpoint data PID
        using NAKSTS = Field<OTG_HS_DOEPCTL3_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_HS_DOEPCTL3_Reg, 18, 2>; // Endpoint type
        using SNPM = Field<OTG_HS_DOEPCTL3_Reg, 20, 1>; // Snoop mode
        using Stall = Field<OTG_HS_DOEPCTL3_Reg, 21, 1>; // STALL handshake
        using CNAK = Field<OTG_HS_DOEPCTL3_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_HS_DOEPCTL3_Reg, 27, 1>; // Set NAK
        using SD0PID_SEVNFRM = Field<OTG_HS_DOEPCTL3_Reg, 28, 1>; // Set DATA0 PID/Set even frame
        using SODDFRM = Field<OTG_HS_DOEPCTL3_Reg, 29, 1>; // Set odd frame
        using EPDIS = Field<OTG_HS_DOEPCTL3_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_HS_DOEPCTL3_Reg, 31, 1>; // Endpoint enable
    };

    // OTG_HS device endpoint-0 interrupt register
    using OTG_HS_DOEPINT0_Reg = Register<OTG_HS_DEVICE_BASE + 0x308, ReadWrite>;
    struct _OTG_HS_DOEPINT0 {
        using XFRC = Field<OTG_HS_DOEPINT0_Reg, 0, 1>; // Transfer completed interrupt
        using EPDISD = Field<OTG_HS_DOEPINT0_Reg, 1, 1>; // Endpoint disabled interrupt
        using STUP = Field<OTG_HS_DOEPINT0_Reg, 3, 1>; // SETUP phase done
        using OTEPDIS = Field<OTG_HS_DOEPINT0_Reg, 4, 1>; // OUT token received when endpoint disabled
        using B2BSTUP = Field<OTG_HS_DOEPINT0_Reg, 6, 1>; // Back-to-back SETUP packets received
        using NYET = Field<OTG_HS_DOEPINT0_Reg, 14, 1>; // NYET interrupt
    };

    // OTG_HS device endpoint-1 interrupt register
    using OTG_HS_DOEPINT1_Reg = Register<OTG_HS_DEVICE_BASE + 0x328, ReadWrite>;
    struct _OTG_HS_DOEPINT1 {
        using XFRC = Field<OTG_HS_DOEPINT1_Reg, 0, 1>; // Transfer completed interrupt
        using EPDISD = Field<OTG_HS_DOEPINT1_Reg, 1, 1>; // Endpoint disabled interrupt
        using STUP = Field<OTG_HS_DOEPINT1_Reg, 3, 1>; // SETUP phase done
        using OTEPDIS = Field<OTG_HS_DOEPINT1_Reg, 4, 1>; // OUT token received when endpoint disabled
        using B2BSTUP = Field<OTG_HS_DOEPINT1_Reg, 6, 1>; // Back-to-back SETUP packets received
        using NYET = Field<OTG_HS_DOEPINT1_Reg, 14, 1>; // NYET interrupt
    };

    // OTG_HS device endpoint-2 interrupt register
    using OTG_HS_DOEPINT2_Reg = Register<OTG_HS_DEVICE_BASE + 0x348, ReadWrite>;
    struct _OTG_HS_DOEPINT2 {
        using XFRC = Field<OTG_HS_DOEPINT2_Reg, 0, 1>; // Transfer completed interrupt
        using EPDISD = Field<OTG_HS_DOEPINT2_Reg, 1, 1>; // Endpoint disabled interrupt
        using STUP = Field<OTG_HS_DOEPINT2_Reg, 3, 1>; // SETUP phase done
        using OTEPDIS = Field<OTG_HS_DOEPINT2_Reg, 4, 1>; // OUT token received when endpoint disabled
        using B2BSTUP = Field<OTG_HS_DOEPINT2_Reg, 6, 1>; // Back-to-back SETUP packets received
        using NYET = Field<OTG_HS_DOEPINT2_Reg, 14, 1>; // NYET interrupt
    };

    // OTG_HS device endpoint-3 interrupt register
    using OTG_HS_DOEPINT3_Reg = Register<OTG_HS_DEVICE_BASE + 0x368, ReadWrite>;
    struct _OTG_HS_DOEPINT3 {
        using XFRC = Field<OTG_HS_DOEPINT3_Reg, 0, 1>; // Transfer completed interrupt
        using EPDISD = Field<OTG_HS_DOEPINT3_Reg, 1, 1>; // Endpoint disabled interrupt
        using STUP = Field<OTG_HS_DOEPINT3_Reg, 3, 1>; // SETUP phase done
        using OTEPDIS = Field<OTG_HS_DOEPINT3_Reg, 4, 1>; // OUT token received when endpoint disabled
        using B2BSTUP = Field<OTG_HS_DOEPINT3_Reg, 6, 1>; // Back-to-back SETUP packets received
        using NYET = Field<OTG_HS_DOEPINT3_Reg, 14, 1>; // NYET interrupt
    };

    // OTG_HS device endpoint-4 interrupt register
    using OTG_HS_DOEPINT4_Reg = Register<OTG_HS_DEVICE_BASE + 0x388, ReadWrite>;
    struct _OTG_HS_DOEPINT4 {
        using XFRC = Field<OTG_HS_DOEPINT4_Reg, 0, 1>; // Transfer completed interrupt
        using EPDISD = Field<OTG_HS_DOEPINT4_Reg, 1, 1>; // Endpoint disabled interrupt
        using STUP = Field<OTG_HS_DOEPINT4_Reg, 3, 1>; // SETUP phase done
        using OTEPDIS = Field<OTG_HS_DOEPINT4_Reg, 4, 1>; // OUT token received when endpoint disabled
        using B2BSTUP = Field<OTG_HS_DOEPINT4_Reg, 6, 1>; // Back-to-back SETUP packets received
        using NYET = Field<OTG_HS_DOEPINT4_Reg, 14, 1>; // NYET interrupt
    };

    // OTG_HS device endpoint-5 interrupt register
    using OTG_HS_DOEPINT5_Reg = Register<OTG_HS_DEVICE_BASE + 0x3A8, ReadWrite>;
    struct _OTG_HS_DOEPINT5 {
        using XFRC = Field<OTG_HS_DOEPINT5_Reg, 0, 1>; // Transfer completed interrupt
        using EPDISD = Field<OTG_HS_DOEPINT5_Reg, 1, 1>; // Endpoint disabled interrupt
        using STUP = Field<OTG_HS_DOEPINT5_Reg, 3, 1>; // SETUP phase done
        using OTEPDIS = Field<OTG_HS_DOEPINT5_Reg, 4, 1>; // OUT token received when endpoint disabled
        using B2BSTUP = Field<OTG_HS_DOEPINT5_Reg, 6, 1>; // Back-to-back SETUP packets received
        using NYET = Field<OTG_HS_DOEPINT5_Reg, 14, 1>; // NYET interrupt
    };

    // OTG_HS device endpoint-6 interrupt register
    using OTG_HS_DOEPINT6_Reg = Register<OTG_HS_DEVICE_BASE + 0x3C8, ReadWrite>;
    struct _OTG_HS_DOEPINT6 {
        using XFRC = Field<OTG_HS_DOEPINT6_Reg, 0, 1>; // Transfer completed interrupt
        using EPDISD = Field<OTG_HS_DOEPINT6_Reg, 1, 1>; // Endpoint disabled interrupt
        using STUP = Field<OTG_HS_DOEPINT6_Reg, 3, 1>; // SETUP phase done
        using OTEPDIS = Field<OTG_HS_DOEPINT6_Reg, 4, 1>; // OUT token received when endpoint disabled
        using B2BSTUP = Field<OTG_HS_DOEPINT6_Reg, 6, 1>; // Back-to-back SETUP packets received
        using NYET = Field<OTG_HS_DOEPINT6_Reg, 14, 1>; // NYET interrupt
    };

    // OTG_HS device endpoint-7 interrupt register
    using OTG_HS_DOEPINT7_Reg = Register<OTG_HS_DEVICE_BASE + 0x3E8, ReadWrite>;
    struct _OTG_HS_DOEPINT7 {
        using XFRC = Field<OTG_HS_DOEPINT7_Reg, 0, 1>; // Transfer completed interrupt
        using EPDISD = Field<OTG_HS_DOEPINT7_Reg, 1, 1>; // Endpoint disabled interrupt
        using STUP = Field<OTG_HS_DOEPINT7_Reg, 3, 1>; // SETUP phase done
        using OTEPDIS = Field<OTG_HS_DOEPINT7_Reg, 4, 1>; // OUT token received when endpoint disabled
        using B2BSTUP = Field<OTG_HS_DOEPINT7_Reg, 6, 1>; // Back-to-back SETUP packets received
        using NYET = Field<OTG_HS_DOEPINT7_Reg, 14, 1>; // NYET interrupt
    };

    // OTG_HS device endpoint-0 transfer size register
    using OTG_HS_DOEPTSIZ0_Reg = Register<OTG_HS_DEVICE_BASE + 0x310, ReadWrite>;
    struct _OTG_HS_DOEPTSIZ0 {
        using XFRSIZ = Field<OTG_HS_DOEPTSIZ0_Reg, 0, 7>; // Transfer size
        using PKTCNT = Field<OTG_HS_DOEPTSIZ0_Reg, 19, 1>; // Packet count
        using STUPCNT = Field<OTG_HS_DOEPTSIZ0_Reg, 29, 2>; // SETUP packet count
    };

    // OTG_HS device endpoint-1 transfer size register
    using OTG_HS_DOEPTSIZ1_Reg = Register<OTG_HS_DEVICE_BASE + 0x330, ReadWrite>;
    struct _OTG_HS_DOEPTSIZ1 {
        using XFRSIZ = Field<OTG_HS_DOEPTSIZ1_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_DOEPTSIZ1_Reg, 19, 10>; // Packet count
        using RXDPID_STUPCNT = Field<OTG_HS_DOEPTSIZ1_Reg, 29, 2>; // Received data PID/SETUP packet count
    };

    // OTG_HS device endpoint-2 transfer size register
    using OTG_HS_DOEPTSIZ2_Reg = Register<OTG_HS_DEVICE_BASE + 0x350, ReadWrite>;
    struct _OTG_HS_DOEPTSIZ2 {
        using XFRSIZ = Field<OTG_HS_DOEPTSIZ2_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_DOEPTSIZ2_Reg, 19, 10>; // Packet count
        using RXDPID_STUPCNT = Field<OTG_HS_DOEPTSIZ2_Reg, 29, 2>; // Received data PID/SETUP packet count
    };

    // OTG_HS device endpoint-3 transfer size register
    using OTG_HS_DOEPTSIZ3_Reg = Register<OTG_HS_DEVICE_BASE + 0x370, ReadWrite>;
    struct _OTG_HS_DOEPTSIZ3 {
        using XFRSIZ = Field<OTG_HS_DOEPTSIZ3_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_DOEPTSIZ3_Reg, 19, 10>; // Packet count
        using RXDPID_STUPCNT = Field<OTG_HS_DOEPTSIZ3_Reg, 29, 2>; // Received data PID/SETUP packet count
    };

    // OTG_HS device endpoint-4 transfer size register
    using OTG_HS_DOEPTSIZ4_Reg = Register<OTG_HS_DEVICE_BASE + 0x390, ReadWrite>;
    struct _OTG_HS_DOEPTSIZ4 {
        using XFRSIZ = Field<OTG_HS_DOEPTSIZ4_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_DOEPTSIZ4_Reg, 19, 10>; // Packet count
        using RXDPID_STUPCNT = Field<OTG_HS_DOEPTSIZ4_Reg, 29, 2>; // Received data PID/SETUP packet count
    };

    // OTG_HS device endpoint transfer size register
    using OTG_HS_DIEPTSIZ6_Reg = Register<OTG_HS_DEVICE_BASE + 0x1A0, ReadWrite>;
    struct _OTG_HS_DIEPTSIZ6 {
        using XFRSIZ = Field<OTG_HS_DIEPTSIZ6_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_DIEPTSIZ6_Reg, 19, 10>; // Packet count
        using MCNT = Field<OTG_HS_DIEPTSIZ6_Reg, 29, 2>; // Multi count
    };

    // OTG_HS device IN endpoint transmit FIFO status register
    using OTG_HS_DTXFSTS6_Reg = Register<OTG_HS_DEVICE_BASE + 0x1A4, ReadWrite>;
    struct _OTG_HS_DTXFSTS6 {
        using INEPTFSAV = Field<OTG_HS_DTXFSTS6_Reg, 0, 16>; // IN endpoint TxFIFO space avail
    };

    // OTG_HS device endpoint transfer size register
    using OTG_HS_DIEPTSIZ7_Reg = Register<OTG_HS_DEVICE_BASE + 0x1A8, ReadWrite>;
    struct _OTG_HS_DIEPTSIZ7 {
        using XFRSIZ = Field<OTG_HS_DIEPTSIZ7_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_DIEPTSIZ7_Reg, 19, 10>; // Packet count
        using MCNT = Field<OTG_HS_DIEPTSIZ7_Reg, 29, 2>; // Multi count
    };

    // OTG_HS device IN endpoint transmit FIFO status register
    using OTG_HS_DTXFSTS7_Reg = Register<OTG_HS_DEVICE_BASE + 0x1AC, ReadWrite>;
    struct _OTG_HS_DTXFSTS7 {
        using INEPTFSAV = Field<OTG_HS_DTXFSTS7_Reg, 0, 16>; // IN endpoint TxFIFO space avail
    };

    // OTG device endpoint-4 control register
    using OTG_HS_DOEPCTL4_Reg = Register<OTG_HS_DEVICE_BASE + 0x380, ReadWrite>;
    struct _OTG_HS_DOEPCTL4 {
        using MPSIZ = Field<OTG_HS_DOEPCTL4_Reg, 0, 11>; // Maximum packet size
        using USBAEP = Field<OTG_HS_DOEPCTL4_Reg, 15, 1>; // USB active endpoint
        using EONUM_DPID = Field<OTG_HS_DOEPCTL4_Reg, 16, 1>; // Even odd frame/Endpoint data PID
        using NAKSTS = Field<OTG_HS_DOEPCTL4_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_HS_DOEPCTL4_Reg, 18, 2>; // Endpoint type
        using SNPM = Field<OTG_HS_DOEPCTL4_Reg, 20, 1>; // Snoop mode
        using Stall = Field<OTG_HS_DOEPCTL4_Reg, 21, 1>; // STALL handshake
        using CNAK = Field<OTG_HS_DOEPCTL4_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_HS_DOEPCTL4_Reg, 27, 1>; // Set NAK
        using SD0PID_SEVNFRM = Field<OTG_HS_DOEPCTL4_Reg, 28, 1>; // Set DATA0 PID/Set even frame
        using SODDFRM = Field<OTG_HS_DOEPCTL4_Reg, 29, 1>; // Set odd frame
        using EPDIS = Field<OTG_HS_DOEPCTL4_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_HS_DOEPCTL4_Reg, 31, 1>; // Endpoint enable
    };

    // OTG device endpoint-5 control register
    using OTG_HS_DOEPCTL5_Reg = Register<OTG_HS_DEVICE_BASE + 0x3A0, ReadWrite>;
    struct _OTG_HS_DOEPCTL5 {
        using MPSIZ = Field<OTG_HS_DOEPCTL5_Reg, 0, 11>; // Maximum packet size
        using USBAEP = Field<OTG_HS_DOEPCTL5_Reg, 15, 1>; // USB active endpoint
        using EONUM_DPID = Field<OTG_HS_DOEPCTL5_Reg, 16, 1>; // Even odd frame/Endpoint data PID
        using NAKSTS = Field<OTG_HS_DOEPCTL5_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_HS_DOEPCTL5_Reg, 18, 2>; // Endpoint type
        using SNPM = Field<OTG_HS_DOEPCTL5_Reg, 20, 1>; // Snoop mode
        using Stall = Field<OTG_HS_DOEPCTL5_Reg, 21, 1>; // STALL handshake
        using CNAK = Field<OTG_HS_DOEPCTL5_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_HS_DOEPCTL5_Reg, 27, 1>; // Set NAK
        using SD0PID_SEVNFRM = Field<OTG_HS_DOEPCTL5_Reg, 28, 1>; // Set DATA0 PID/Set even frame
        using SODDFRM = Field<OTG_HS_DOEPCTL5_Reg, 29, 1>; // Set odd frame
        using EPDIS = Field<OTG_HS_DOEPCTL5_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_HS_DOEPCTL5_Reg, 31, 1>; // Endpoint enable
    };

    // OTG device endpoint-6 control register
    using OTG_HS_DOEPCTL6_Reg = Register<OTG_HS_DEVICE_BASE + 0x3C0, ReadWrite>;
    struct _OTG_HS_DOEPCTL6 {
        using MPSIZ = Field<OTG_HS_DOEPCTL6_Reg, 0, 11>; // Maximum packet size
        using USBAEP = Field<OTG_HS_DOEPCTL6_Reg, 15, 1>; // USB active endpoint
        using EONUM_DPID = Field<OTG_HS_DOEPCTL6_Reg, 16, 1>; // Even odd frame/Endpoint data PID
        using NAKSTS = Field<OTG_HS_DOEPCTL6_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_HS_DOEPCTL6_Reg, 18, 2>; // Endpoint type
        using SNPM = Field<OTG_HS_DOEPCTL6_Reg, 20, 1>; // Snoop mode
        using Stall = Field<OTG_HS_DOEPCTL6_Reg, 21, 1>; // STALL handshake
        using CNAK = Field<OTG_HS_DOEPCTL6_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_HS_DOEPCTL6_Reg, 27, 1>; // Set NAK
        using SD0PID_SEVNFRM = Field<OTG_HS_DOEPCTL6_Reg, 28, 1>; // Set DATA0 PID/Set even frame
        using SODDFRM = Field<OTG_HS_DOEPCTL6_Reg, 29, 1>; // Set odd frame
        using EPDIS = Field<OTG_HS_DOEPCTL6_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_HS_DOEPCTL6_Reg, 31, 1>; // Endpoint enable
    };

    // OTG device endpoint-7 control register
    using OTG_HS_DOEPCTL7_Reg = Register<OTG_HS_DEVICE_BASE + 0x3E0, ReadWrite>;
    struct _OTG_HS_DOEPCTL7 {
        using MPSIZ = Field<OTG_HS_DOEPCTL7_Reg, 0, 11>; // Maximum packet size
        using USBAEP = Field<OTG_HS_DOEPCTL7_Reg, 15, 1>; // USB active endpoint
        using EONUM_DPID = Field<OTG_HS_DOEPCTL7_Reg, 16, 1>; // Even odd frame/Endpoint data PID
        using NAKSTS = Field<OTG_HS_DOEPCTL7_Reg, 17, 1>; // NAK status
        using EPTYP = Field<OTG_HS_DOEPCTL7_Reg, 18, 2>; // Endpoint type
        using SNPM = Field<OTG_HS_DOEPCTL7_Reg, 20, 1>; // Snoop mode
        using Stall = Field<OTG_HS_DOEPCTL7_Reg, 21, 1>; // STALL handshake
        using CNAK = Field<OTG_HS_DOEPCTL7_Reg, 26, 1>; // Clear NAK
        using SNAK = Field<OTG_HS_DOEPCTL7_Reg, 27, 1>; // Set NAK
        using SD0PID_SEVNFRM = Field<OTG_HS_DOEPCTL7_Reg, 28, 1>; // Set DATA0 PID/Set even frame
        using SODDFRM = Field<OTG_HS_DOEPCTL7_Reg, 29, 1>; // Set odd frame
        using EPDIS = Field<OTG_HS_DOEPCTL7_Reg, 30, 1>; // Endpoint disable
        using EPENA = Field<OTG_HS_DOEPCTL7_Reg, 31, 1>; // Endpoint enable
    };

    // OTG_HS device endpoint-5 transfer size register
    using OTG_HS_DOEPTSIZ5_Reg = Register<OTG_HS_DEVICE_BASE + 0x3B0, ReadWrite>;
    struct _OTG_HS_DOEPTSIZ5 {
        using XFRSIZ = Field<OTG_HS_DOEPTSIZ5_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_DOEPTSIZ5_Reg, 19, 10>; // Packet count
        using RXDPID_STUPCNT = Field<OTG_HS_DOEPTSIZ5_Reg, 29, 2>; // Received data PID/SETUP packet count
    };

    // OTG_HS device endpoint-6 transfer size register
    using OTG_HS_DOEPTSIZ6_Reg = Register<OTG_HS_DEVICE_BASE + 0x3D0, ReadWrite>;
    struct _OTG_HS_DOEPTSIZ6 {
        using XFRSIZ = Field<OTG_HS_DOEPTSIZ6_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_DOEPTSIZ6_Reg, 19, 10>; // Packet count
        using RXDPID_STUPCNT = Field<OTG_HS_DOEPTSIZ6_Reg, 29, 2>; // Received data PID/SETUP packet count
    };

    // OTG_HS device endpoint-7 transfer size register
    using OTG_HS_DOEPTSIZ7_Reg = Register<OTG_HS_DEVICE_BASE + 0x3F0, ReadWrite>;
    struct _OTG_HS_DOEPTSIZ7 {
        using XFRSIZ = Field<OTG_HS_DOEPTSIZ7_Reg, 0, 19>; // Transfer size
        using PKTCNT = Field<OTG_HS_DOEPTSIZ7_Reg, 19, 10>; // Packet count
        using RXDPID_STUPCNT = Field<OTG_HS_DOEPTSIZ7_Reg, 29, 2>; // Received data PID/SETUP packet count
    };

} // namespace OTG_HS_DEVICE

// --------------------------------------------
// OTG_HS_PWRCLK: USB on the go high speed
// Base address: 0x40040E00
// --------------------------------------------

namespace OTG_HS_PWRCLK {
    static constexpr uint32_t OTG_HS_PWRCLK_BASE = 0x40040E00;

    // Power and clock gating control register
    using OTG_HS_PCGCR_Reg = Register<OTG_HS_PWRCLK_BASE + 0x0, ReadWrite>;
    struct _OTG_HS_PCGCR {
        using STPPCLK = Field<OTG_HS_PCGCR_Reg, 0, 1>; // Stop PHY clock
        using GATEHCLK = Field<OTG_HS_PCGCR_Reg, 1, 1>; // Gate HCLK
        using PHYSUSP = Field<OTG_HS_PCGCR_Reg, 4, 1>; // PHY suspended
    };

} // namespace OTG_HS_PWRCLK

// --------------------------------------------
// DSI: DSI Host
// Base address: 0x40016C00
// --------------------------------------------

namespace DSI {
    static constexpr uint32_t DSI_BASE = 0x40016C00;

    // DSI Host Version Register
    using DSI_VR_Reg = Register<DSI_BASE + 0x0, ReadOnly>;
    struct _DSI_VR {
        using VERSION = Field<DSI_VR_Reg, 0, 32>; // Version of the DSI Host
    };

    // DSI Host Control Register
    using DSI_CR_Reg = Register<DSI_BASE + 0x4, ReadWrite>;
    struct _DSI_CR {
        using EN = Field<DSI_CR_Reg, 0, 1>; // Enable
    };

    // DSI HOST Clock Control Register
    using DSI_CCR_Reg = Register<DSI_BASE + 0x8, ReadWrite>;
    struct _DSI_CCR {
        using TXECKDIV = Field<DSI_CCR_Reg, 0, 8>; // TX Escape Clock Division
        using TOCKDIV = Field<DSI_CCR_Reg, 8, 8>; // Timeout Clock Division
    };

    // DSI Host LTDC VCID Register
    using DSI_LVCIDR_Reg = Register<DSI_BASE + 0xC, ReadWrite>;
    struct _DSI_LVCIDR {
        using VCID = Field<DSI_LVCIDR_Reg, 0, 2>; // Virtual Channel ID
    };

    // DSI Host LTDC Color Coding Register
    using DSI_LCOLCR_Reg = Register<DSI_BASE + 0x10, ReadWrite>;
    struct _DSI_LCOLCR {
        using COLC = Field<DSI_LCOLCR_Reg, 0, 4>; // Color Coding
        using LPE = Field<DSI_LCOLCR_Reg, 8, 1>; // Loosely Packet Enable
    };

    // DSI Host LTDC Polarity Configuration Register
    using DSI_LPCR_Reg = Register<DSI_BASE + 0x14, ReadWrite>;
    struct _DSI_LPCR {
        using DEP = Field<DSI_LPCR_Reg, 0, 1>; // Data Enable Polarity
        using VSP = Field<DSI_LPCR_Reg, 1, 1>; // VSYNC Polarity
        using HSP = Field<DSI_LPCR_Reg, 2, 1>; // HSYNC Polarity
    };

    // DSI Host Low-Power mode Configuration Register
    using DSI_LPMCR_Reg = Register<DSI_BASE + 0x18, ReadWrite>;
    struct _DSI_LPMCR {
        using VLPSIZE = Field<DSI_LPMCR_Reg, 0, 8>; // VACT Largest Packet Size
        using LPSIZE = Field<DSI_LPMCR_Reg, 16, 8>; // Largest Packet Size
    };

    // DSI Host Protocol Configuration Register
    using DSI_PCR_Reg = Register<DSI_BASE + 0x2C, ReadWrite>;
    struct _DSI_PCR {
        using ETTXE = Field<DSI_PCR_Reg, 0, 1>; // EoTp Transmission Enable
        using ETRXE = Field<DSI_PCR_Reg, 1, 1>; // EoTp Reception Enable
        using BTAE = Field<DSI_PCR_Reg, 2, 1>; // Bus Turn Around Enable
        using ECCRXE = Field<DSI_PCR_Reg, 3, 1>; // ECC Reception Enable
        using CRCRXE = Field<DSI_PCR_Reg, 4, 1>; // CRC Reception Enable
    };

    // DSI Host Generic VCID Register
    using DSI_GVCIDR_Reg = Register<DSI_BASE + 0x30, ReadWrite>;
    struct _DSI_GVCIDR {
        using VCID = Field<DSI_GVCIDR_Reg, 0, 2>; // Virtual Channel ID
    };

    // DSI Host mode Configuration Register
    using DSI_MCR_Reg = Register<DSI_BASE + 0x34, ReadWrite>;
    struct _DSI_MCR {
        using CMDM = Field<DSI_MCR_Reg, 0, 1>; // Command mode
    };

    // DSI Host Video mode Configuration Register
    using DSI_VMCR_Reg = Register<DSI_BASE + 0x38, ReadWrite>;
    struct _DSI_VMCR {
        using VMT = Field<DSI_VMCR_Reg, 0, 2>; // Video mode Type
        using LPVSAE = Field<DSI_VMCR_Reg, 8, 1>; // Low-Power Vertical Sync Active Enable
        using LPVBPE = Field<DSI_VMCR_Reg, 9, 1>; // Low-power Vertical Back-Porch Enable
        using LPVFPE = Field<DSI_VMCR_Reg, 10, 1>; // Low-power Vertical Front-porch Enable
        using LPVAE = Field<DSI_VMCR_Reg, 11, 1>; // Low-Power Vertical Active Enable
        using LPHBPE = Field<DSI_VMCR_Reg, 12, 1>; // Low-Power Horizontal Back-Porch Enable
        using LPHFPE = Field<DSI_VMCR_Reg, 13, 1>; // Low-Power Horizontal Front-Porch Enable
        using FBTAAE = Field<DSI_VMCR_Reg, 14, 1>; // Frame Bus-Turn-Around Acknowledge Enable
        using LPCE = Field<DSI_VMCR_Reg, 15, 1>; // Low-Power Command Enable
        using PGE = Field<DSI_VMCR_Reg, 16, 1>; // Pattern Generator Enable
        using PGM = Field<DSI_VMCR_Reg, 20, 1>; // Pattern Generator mode
        using PGO = Field<DSI_VMCR_Reg, 24, 1>; // Pattern Generator Orientation
    };

    // DSI Host Video Packet Configuration Register
    using DSI_VPCR_Reg = Register<DSI_BASE + 0x3C, ReadWrite>;
    struct _DSI_VPCR {
        using VPSIZE = Field<DSI_VPCR_Reg, 0, 14>; // Video Packet Size
    };

    // DSI Host Video Chunks Configuration Register
    using DSI_VCCR_Reg = Register<DSI_BASE + 0x40, ReadWrite>;
    struct _DSI_VCCR {
        using NUMC = Field<DSI_VCCR_Reg, 0, 13>; // Number of Chunks
    };

    // DSI Host Video Null Packet Configuration Register
    using DSI_VNPCR_Reg = Register<DSI_BASE + 0x44, ReadWrite>;
    struct _DSI_VNPCR {
        using NPSIZE = Field<DSI_VNPCR_Reg, 0, 13>; // Null Packet Size
    };

    // DSI Host Video HSA Configuration Register
    using DSI_VHSACR_Reg = Register<DSI_BASE + 0x48, ReadWrite>;
    struct _DSI_VHSACR {
        using HSA = Field<DSI_VHSACR_Reg, 0, 12>; // Horizontal Synchronism Active duration
    };

    // DSI Host Video HBP Configuration Register
    using DSI_VHBPCR_Reg = Register<DSI_BASE + 0x4C, ReadWrite>;
    struct _DSI_VHBPCR {
        using HBP = Field<DSI_VHBPCR_Reg, 0, 12>; // Horizontal Back-Porch duration
    };

    // DSI Host Video Line Configuration Register
    using DSI_VLCR_Reg = Register<DSI_BASE + 0x50, ReadWrite>;
    struct _DSI_VLCR {
        using HLINE = Field<DSI_VLCR_Reg, 0, 15>; // Horizontal Line duration
    };

    // DSI Host Video VSA Configuration Register
    using DSI_VVSACR_Reg = Register<DSI_BASE + 0x54, ReadWrite>;
    struct _DSI_VVSACR {
        using VSA = Field<DSI_VVSACR_Reg, 0, 10>; // Vertical Synchronism Active duration
    };

    // DSI Host Video VBP Configuration Register
    using DSI_VVBPCR_Reg = Register<DSI_BASE + 0x58, ReadWrite>;
    struct _DSI_VVBPCR {
        using VBP = Field<DSI_VVBPCR_Reg, 0, 10>; // Vertical Back-Porch duration
    };

    // DSI Host Video VFP Configuration Register
    using DSI_VVFPCR_Reg = Register<DSI_BASE + 0x5C, ReadWrite>;
    struct _DSI_VVFPCR {
        using VFP = Field<DSI_VVFPCR_Reg, 0, 10>; // Vertical Front-Porch duration
    };

    // DSI Host Video VA Configuration Register
    using DSI_VVACR_Reg = Register<DSI_BASE + 0x60, ReadWrite>;
    struct _DSI_VVACR {
        using VA = Field<DSI_VVACR_Reg, 0, 14>; // Vertical Active duration
    };

    // DSI Host LTDC Command Configuration Register
    using DSI_LCCR_Reg = Register<DSI_BASE + 0x64, ReadWrite>;
    struct _DSI_LCCR {
        using CMDSIZE = Field<DSI_LCCR_Reg, 0, 16>; // Command Size
    };

    // DSI Host Command mode Configuration Register
    using DSI_CMCR_Reg = Register<DSI_BASE + 0x68, ReadWrite>;
    struct _DSI_CMCR {
        using TEARE = Field<DSI_CMCR_Reg, 0, 1>; // Tearing Effect Acknowledge Request Enable
        using ARE = Field<DSI_CMCR_Reg, 1, 1>; // Acknowledge Request Enable
        using GSW0TX = Field<DSI_CMCR_Reg, 8, 1>; // Generic Short Write Zero parameters Transmission
        using GSW1TX = Field<DSI_CMCR_Reg, 9, 1>; // Generic Short Write One parameters Transmission
        using GSW2TX = Field<DSI_CMCR_Reg, 10, 1>; // Generic Short Write Two parameters Transmission
        using GSR0TX = Field<DSI_CMCR_Reg, 11, 1>; // Generic Short Read Zero parameters Transmission
        using GSR1TX = Field<DSI_CMCR_Reg, 12, 1>; // Generic Short Read One parameters Transmission
        using GSR2TX = Field<DSI_CMCR_Reg, 13, 1>; // Generic Short Read Two parameters Transmission
        using GLWTX = Field<DSI_CMCR_Reg, 14, 1>; // Generic Long Write Transmission
        using DSW0TX = Field<DSI_CMCR_Reg, 16, 1>; // DCS Short Write Zero parameter Transmission
        using DSW1TX = Field<DSI_CMCR_Reg, 17, 1>; // DCS Short Read One parameter Transmission
        using DSR0TX = Field<DSI_CMCR_Reg, 18, 1>; // DCS Short Read Zero parameter Transmission
        using DLWTX = Field<DSI_CMCR_Reg, 19, 1>; // DCS Long Write Transmission
        using MRDPS = Field<DSI_CMCR_Reg, 24, 1>; // Maximum Read Packet Size
    };

    // DSI Host Generic Header Configuration Register
    using DSI_GHCR_Reg = Register<DSI_BASE + 0x6C, ReadWrite>;
    struct _DSI_GHCR {
        using DT = Field<DSI_GHCR_Reg, 0, 6>; // Type
        using VCID = Field<DSI_GHCR_Reg, 6, 2>; // Channel
        using WCLSB = Field<DSI_GHCR_Reg, 8, 8>; // WordCount LSB
        using WCMSB = Field<DSI_GHCR_Reg, 16, 8>; // WordCount MSB
    };

    // DSI Host Generic Payload Data Register
    using DSI_GPDR_Reg = Register<DSI_BASE + 0x70, ReadWrite>;
    struct _DSI_GPDR {
        using DATA1 = Field<DSI_GPDR_Reg, 0, 8>; // Payload Byte 1
        using DATA2 = Field<DSI_GPDR_Reg, 8, 8>; // Payload Byte 2
        using DATA3 = Field<DSI_GPDR_Reg, 16, 8>; // Payload Byte 3
        using DATA4 = Field<DSI_GPDR_Reg, 24, 8>; // Payload Byte 4
    };

    // DSI Host Generic Packet Status Register
    using DSI_GPSR_Reg = Register<DSI_BASE + 0x74, ReadOnly>;
    struct _DSI_GPSR {
        using CMDFE = Field<DSI_GPSR_Reg, 0, 1>; // Command FIFO Empty
        using CMDFF = Field<DSI_GPSR_Reg, 1, 1>; // Command FIFO Full
        using PWRFE = Field<DSI_GPSR_Reg, 2, 1>; // Payload Write FIFO Empty
        using PWRFF = Field<DSI_GPSR_Reg, 3, 1>; // Payload Write FIFO Full
        using PRDFE = Field<DSI_GPSR_Reg, 4, 1>; // Payload Read FIFO Empty
        using PRDFF = Field<DSI_GPSR_Reg, 5, 1>; // Payload Read FIFO Full
        using RCB = Field<DSI_GPSR_Reg, 6, 1>; // Read Command Busy
    };

    // DSI Host Timeout Counter Configuration Register 0
    using DSI_TCCR0_Reg = Register<DSI_BASE + 0x78, ReadWrite>;
    struct _DSI_TCCR0 {
        using LPRX_TOCNT = Field<DSI_TCCR0_Reg, 0, 16>; // Low-power Reception Timeout Counter
        using HSTX_TOCNT = Field<DSI_TCCR0_Reg, 16, 16>; // High-Speed Transmission Timeout Counter
    };

    // DSI Host Timeout Counter Configuration Register 1
    using DSI_TCCR1_Reg = Register<DSI_BASE + 0x7C, ReadWrite>;
    struct _DSI_TCCR1 {
        using HSRD_TOCNT = Field<DSI_TCCR1_Reg, 0, 16>; // High-Speed Read Timeout Counter
    };

    // DSI Host Timeout Counter Configuration Register 2
    using DSI_TCCR2_Reg = Register<DSI_BASE + 0x80, ReadWrite>;
    struct _DSI_TCCR2 {
        using LPRD_TOCNT = Field<DSI_TCCR2_Reg, 0, 16>; // Low-Power Read Timeout Counter
    };

    // DSI Host Timeout Counter Configuration Register 3
    using DSI_TCCR3_Reg = Register<DSI_BASE + 0x84, ReadWrite>;
    struct _DSI_TCCR3 {
        using HSWR_TOCNT = Field<DSI_TCCR3_Reg, 0, 16>; // High-Speed Write Timeout Counter
        using PM = Field<DSI_TCCR3_Reg, 24, 1>; // Presp mode
    };

    // DSI Host Timeout Counter Configuration Register 4
    using DSI_TCCR4_Reg = Register<DSI_BASE + 0x88, ReadWrite>;
    struct _DSI_TCCR4 {
        using LSWR_TOCNT = Field<DSI_TCCR4_Reg, 0, 16>; // Low-Power Write Timeout Counter
    };

    // DSI Host Timeout Counter Configuration Register 5
    using DSI_TCCR5_Reg = Register<DSI_BASE + 0x8C, ReadWrite>;
    struct _DSI_TCCR5 {
        using BTA_TOCNT = Field<DSI_TCCR5_Reg, 0, 16>; // Bus-Turn-Around Timeout Counter
    };

    // DSI Host Clock Lane Configuration Register
    using DSI_CLCR_Reg = Register<DSI_BASE + 0x94, ReadWrite>;
    struct _DSI_CLCR {
        using DPCC = Field<DSI_CLCR_Reg, 0, 1>; // D-PHY Clock Control
        using ACR = Field<DSI_CLCR_Reg, 1, 1>; // Automatic Clock lane Control
    };

    // DSI Host Clock Lane Timer Configuration Register
    using DSI_CLTCR_Reg = Register<DSI_BASE + 0x98, ReadWrite>;
    struct _DSI_CLTCR {
        using LP2HS_TIME = Field<DSI_CLTCR_Reg, 0, 10>; // Low-Power to High-Speed Time
        using HS2LP_TIME = Field<DSI_CLTCR_Reg, 16, 10>; // High-Speed to Low-Power Time
    };

    // DSI Host Data Lane Timer Configuration Register
    using DSI_DLTCR_Reg = Register<DSI_BASE + 0x9C, ReadWrite>;
    struct _DSI_DLTCR {
        using MRD_TIME = Field<DSI_DLTCR_Reg, 0, 15>; // Maximum Read Time
        using LP2HS_TIME = Field<DSI_DLTCR_Reg, 16, 8>; // Low-Power To High-Speed Time
        using HS2LP_TIME = Field<DSI_DLTCR_Reg, 24, 8>; // High-Speed To Low-Power Time
    };

    // DSI Host PHY Control Register
    using DSI_PCTLR_Reg = Register<DSI_BASE + 0xA0, ReadWrite>;
    struct _DSI_PCTLR {
        using DEN = Field<DSI_PCTLR_Reg, 1, 1>; // Digital Enable
        using CKE = Field<DSI_PCTLR_Reg, 2, 1>; // Clock Enable
    };

    // DSI Host PHY Configuration Register
    using DSI_PCONFR_Reg = Register<DSI_BASE + 0xA4, ReadWrite>;
    struct _DSI_PCONFR {
        using NL = Field<DSI_PCONFR_Reg, 0, 2>; // Number of Lanes
        using SW_TIME = Field<DSI_PCONFR_Reg, 8, 8>; // Stop Wait Time
    };

    // DSI Host PHY ULPS Control Register
    using DSI_PUCR_Reg = Register<DSI_BASE + 0xA8, ReadWrite>;
    struct _DSI_PUCR {
        using URCL = Field<DSI_PUCR_Reg, 0, 1>; // ULPS Request on Clock Lane
        using UECL = Field<DSI_PUCR_Reg, 1, 1>; // ULPS Exit on Clock Lane
        using URDL = Field<DSI_PUCR_Reg, 2, 1>; // ULPS Request on Data Lane
        using UEDL = Field<DSI_PUCR_Reg, 3, 1>; // ULPS Exit on Data Lane
    };

    // DSI Host PHY TX Triggers Configuration Register
    using DSI_PTTCR_Reg = Register<DSI_BASE + 0xAC, ReadWrite>;
    struct _DSI_PTTCR {
        using TX_TRIG = Field<DSI_PTTCR_Reg, 0, 4>; // Transmission Trigger
    };

    // DSI Host PHY Status Register
    using DSI_PSR_Reg = Register<DSI_BASE + 0xB0, ReadOnly>;
    struct _DSI_PSR {
        using PD = Field<DSI_PSR_Reg, 1, 1>; // PHY Direction
        using PSSC = Field<DSI_PSR_Reg, 2, 1>; // PHY Stop State Clock lane
        using UANC = Field<DSI_PSR_Reg, 3, 1>; // ULPS Active Not Clock lane
        using PSS0 = Field<DSI_PSR_Reg, 4, 1>; // PHY Stop State lane 0
        using UAN0 = Field<DSI_PSR_Reg, 5, 1>; // ULPS Active Not lane 1
        using RUE0 = Field<DSI_PSR_Reg, 6, 1>; // RX ULPS Escape lane 0
        using PSS1 = Field<DSI_PSR_Reg, 7, 1>; // PHY Stop State lane 1
        using UAN1 = Field<DSI_PSR_Reg, 8, 1>; // ULPS Active Not lane 1
    };

    // DSI Host Interrupt & Status Register 0
    using DSI_ISR0_Reg = Register<DSI_BASE + 0xBC, ReadOnly>;
    struct _DSI_ISR0 {
        using AE0 = Field<DSI_ISR0_Reg, 0, 1>; // Acknowledge Error 0
        using AE1 = Field<DSI_ISR0_Reg, 1, 1>; // Acknowledge Error 1
        using AE2 = Field<DSI_ISR0_Reg, 2, 1>; // Acknowledge Error 2
        using AE3 = Field<DSI_ISR0_Reg, 3, 1>; // Acknowledge Error 3
        using AE4 = Field<DSI_ISR0_Reg, 4, 1>; // Acknowledge Error 4
        using AE5 = Field<DSI_ISR0_Reg, 5, 1>; // Acknowledge Error 5
        using AE6 = Field<DSI_ISR0_Reg, 6, 1>; // Acknowledge Error 6
        using AE7 = Field<DSI_ISR0_Reg, 7, 1>; // Acknowledge Error 7
        using AE8 = Field<DSI_ISR0_Reg, 8, 1>; // Acknowledge Error 8
        using AE9 = Field<DSI_ISR0_Reg, 9, 1>; // Acknowledge Error 9
        using AE10 = Field<DSI_ISR0_Reg, 10, 1>; // Acknowledge Error 10
        using AE11 = Field<DSI_ISR0_Reg, 11, 1>; // Acknowledge Error 11
        using AE12 = Field<DSI_ISR0_Reg, 12, 1>; // Acknowledge Error 12
        using AE13 = Field<DSI_ISR0_Reg, 13, 1>; // Acknowledge Error 13
        using AE14 = Field<DSI_ISR0_Reg, 14, 1>; // Acknowledge Error 14
        using AE15 = Field<DSI_ISR0_Reg, 15, 1>; // Acknowledge Error 15
        using PE0 = Field<DSI_ISR0_Reg, 16, 1>; // PHY Error 0
        using PE1 = Field<DSI_ISR0_Reg, 17, 1>; // PHY Error 1
        using PE2 = Field<DSI_ISR0_Reg, 18, 1>; // PHY Error 2
        using PE3 = Field<DSI_ISR0_Reg, 19, 1>; // PHY Error 3
        using PE4 = Field<DSI_ISR0_Reg, 20, 1>; // PHY Error 4
    };

    // DSI Host Interrupt & Status Register 1
    using DSI_ISR1_Reg = Register<DSI_BASE + 0xC0, ReadOnly>;
    struct _DSI_ISR1 {
        using TOHSTX = Field<DSI_ISR1_Reg, 0, 1>; // Timeout High-Speed Transmission
        using TOLPRX = Field<DSI_ISR1_Reg, 1, 1>; // Timeout Low-Power Reception
        using ECCSE = Field<DSI_ISR1_Reg, 2, 1>; // ECC Single-bit Error
        using ECCME = Field<DSI_ISR1_Reg, 3, 1>; // ECC Multi-bit Error
        using CRCE = Field<DSI_ISR1_Reg, 4, 1>; // CRC Error
        using PSE = Field<DSI_ISR1_Reg, 5, 1>; // Packet Size Error
        using EOTPE = Field<DSI_ISR1_Reg, 6, 1>; // EoTp Error
        using LPWRE = Field<DSI_ISR1_Reg, 7, 1>; // LTDC Payload Write Error
        using GCWRE = Field<DSI_ISR1_Reg, 8, 1>; // Generic Command Write Error
        using GPWRE = Field<DSI_ISR1_Reg, 9, 1>; // Generic Payload Write Error
        using GPTXE = Field<DSI_ISR1_Reg, 10, 1>; // Generic Payload Transmit Error
        using GPRDE = Field<DSI_ISR1_Reg, 11, 1>; // Generic Payload Read Error
        using GPRXE = Field<DSI_ISR1_Reg, 12, 1>; // Generic Payload Receive Error
    };

    // DSI Host Interrupt Enable Register 0
    using DSI_IER0_Reg = Register<DSI_BASE + 0xC4, ReadWrite>;
    struct _DSI_IER0 {
        using AE0IE = Field<DSI_IER0_Reg, 0, 1>; // Acknowledge Error 0 Interrupt Enable
        using AE1IE = Field<DSI_IER0_Reg, 1, 1>; // Acknowledge Error 1 Interrupt Enable
        using AE2IE = Field<DSI_IER0_Reg, 2, 1>; // Acknowledge Error 2 Interrupt Enable
        using AE3IE = Field<DSI_IER0_Reg, 3, 1>; // Acknowledge Error 3 Interrupt Enable
        using AE4IE = Field<DSI_IER0_Reg, 4, 1>; // Acknowledge Error 4 Interrupt Enable
        using AE5IE = Field<DSI_IER0_Reg, 5, 1>; // Acknowledge Error 5 Interrupt Enable
        using AE6IE = Field<DSI_IER0_Reg, 6, 1>; // Acknowledge Error 6 Interrupt Enable
        using AE7IE = Field<DSI_IER0_Reg, 7, 1>; // Acknowledge Error 7 Interrupt Enable
        using AE8IE = Field<DSI_IER0_Reg, 8, 1>; // Acknowledge Error 8 Interrupt Enable
        using AE9IE = Field<DSI_IER0_Reg, 9, 1>; // Acknowledge Error 9 Interrupt Enable
        using AE10IE = Field<DSI_IER0_Reg, 10, 1>; // Acknowledge Error 10 Interrupt Enable
        using AE11IE = Field<DSI_IER0_Reg, 11, 1>; // Acknowledge Error 11 Interrupt Enable
        using AE12IE = Field<DSI_IER0_Reg, 12, 1>; // Acknowledge Error 12 Interrupt Enable
        using AE13IE = Field<DSI_IER0_Reg, 13, 1>; // Acknowledge Error 13 Interrupt Enable
        using AE14IE = Field<DSI_IER0_Reg, 14, 1>; // Acknowledge Error 14 Interrupt Enable
        using AE15IE = Field<DSI_IER0_Reg, 15, 1>; // Acknowledge Error 15 Interrupt Enable
        using PE0IE = Field<DSI_IER0_Reg, 16, 1>; // PHY Error 0 Interrupt Enable
        using PE1IE = Field<DSI_IER0_Reg, 17, 1>; // PHY Error 1 Interrupt Enable
        using PE2IE = Field<DSI_IER0_Reg, 18, 1>; // PHY Error 2 Interrupt Enable
        using PE3IE = Field<DSI_IER0_Reg, 19, 1>; // PHY Error 3 Interrupt Enable
        using PE4IE = Field<DSI_IER0_Reg, 20, 1>; // PHY Error 4 Interrupt Enable
    };

    // DSI Host Interrupt Enable Register 1
    using DSI_IER1_Reg = Register<DSI_BASE + 0xC8, ReadWrite>;
    struct _DSI_IER1 {
        using TOHSTXIE = Field<DSI_IER1_Reg, 0, 1>; // Timeout High-Speed Transmission Interrupt Enable
        using TOLPRXIE = Field<DSI_IER1_Reg, 1, 1>; // Timeout Low-Power Reception Interrupt Enable
        using ECCSEIE = Field<DSI_IER1_Reg, 2, 1>; // ECC Single-bit Error Interrupt Enable
        using ECCMEIE = Field<DSI_IER1_Reg, 3, 1>; // ECC Multi-bit Error Interrupt Enable
        using CRCEIE = Field<DSI_IER1_Reg, 4, 1>; // CRC Error Interrupt Enable
        using PSEIE = Field<DSI_IER1_Reg, 5, 1>; // Packet Size Error Interrupt Enable
        using EOTPEIE = Field<DSI_IER1_Reg, 6, 1>; // EoTp Error Interrupt Enable
        using LPWREIE = Field<DSI_IER1_Reg, 7, 1>; // LTDC Payload Write Error Interrupt Enable
        using GCWREIE = Field<DSI_IER1_Reg, 8, 1>; // Generic Command Write Error Interrupt Enable
        using GPWREIE = Field<DSI_IER1_Reg, 9, 1>; // Generic Payload Write Error Interrupt Enable
        using GPTXEIE = Field<DSI_IER1_Reg, 10, 1>; // Generic Payload Transmit Error Interrupt Enable
        using GPRDEIE = Field<DSI_IER1_Reg, 11, 1>; // Generic Payload Read Error Interrupt Enable
        using GPRXEIE = Field<DSI_IER1_Reg, 12, 1>; // Generic Payload Receive Error Interrupt Enable
    };

    // DSI Host Force Interrupt Register 0
    using DSI_FIR0_Reg = Register<DSI_BASE + 0xD8, WriteOnly>;
    struct _DSI_FIR0 {
        using FAE0 = Field<DSI_FIR0_Reg, 0, 1>; // Force Acknowledge Error 0
        using FAE1 = Field<DSI_FIR0_Reg, 1, 1>; // Force Acknowledge Error 1
        using FAE2 = Field<DSI_FIR0_Reg, 2, 1>; // Force Acknowledge Error 2
        using FAE3 = Field<DSI_FIR0_Reg, 3, 1>; // Force Acknowledge Error 3
        using FAE4 = Field<DSI_FIR0_Reg, 4, 1>; // Force Acknowledge Error 4
        using FAE5 = Field<DSI_FIR0_Reg, 5, 1>; // Force Acknowledge Error 5
        using FAE6 = Field<DSI_FIR0_Reg, 6, 1>; // Force Acknowledge Error 6
        using FAE7 = Field<DSI_FIR0_Reg, 7, 1>; // Force Acknowledge Error 7
        using FAE8 = Field<DSI_FIR0_Reg, 8, 1>; // Force Acknowledge Error 8
        using FAE9 = Field<DSI_FIR0_Reg, 9, 1>; // Force Acknowledge Error 9
        using FAE10 = Field<DSI_FIR0_Reg, 10, 1>; // Force Acknowledge Error 10
        using FAE11 = Field<DSI_FIR0_Reg, 11, 1>; // Force Acknowledge Error 11
        using FAE12 = Field<DSI_FIR0_Reg, 12, 1>; // Force Acknowledge Error 12
        using FAE13 = Field<DSI_FIR0_Reg, 13, 1>; // Force Acknowledge Error 13
        using FAE14 = Field<DSI_FIR0_Reg, 14, 1>; // Force Acknowledge Error 14
        using FAE15 = Field<DSI_FIR0_Reg, 15, 1>; // Force Acknowledge Error 15
        using FPE0 = Field<DSI_FIR0_Reg, 16, 1>; // Force PHY Error 0
        using FPE1 = Field<DSI_FIR0_Reg, 17, 1>; // Force PHY Error 1
        using FPE2 = Field<DSI_FIR0_Reg, 18, 1>; // Force PHY Error 2
        using FPE3 = Field<DSI_FIR0_Reg, 19, 1>; // Force PHY Error 3
        using FPE4 = Field<DSI_FIR0_Reg, 20, 1>; // Force PHY Error 4
    };

    // DSI Host Force Interrupt Register 1
    using DSI_FIR1_Reg = Register<DSI_BASE + 0xDC, WriteOnly>;
    struct _DSI_FIR1 {
        using FTOHSTX = Field<DSI_FIR1_Reg, 0, 1>; // Force Timeout High-Speed Transmission
        using FTOLPRX = Field<DSI_FIR1_Reg, 1, 1>; // Force Timeout Low-Power Reception
        using FECCSE = Field<DSI_FIR1_Reg, 2, 1>; // Force ECC Single-bit Error
        using FECCME = Field<DSI_FIR1_Reg, 3, 1>; // Force ECC Multi-bit Error
        using FCRCE = Field<DSI_FIR1_Reg, 4, 1>; // Force CRC Error
        using FPSE = Field<DSI_FIR1_Reg, 5, 1>; // Force Packet Size Error
        using FEOTPE = Field<DSI_FIR1_Reg, 6, 1>; // Force EoTp Error
        using FLPWRE = Field<DSI_FIR1_Reg, 7, 1>; // Force LTDC Payload Write Error
        using FGCWRE = Field<DSI_FIR1_Reg, 8, 1>; // Force Generic Command Write Error
        using FGPWRE = Field<DSI_FIR1_Reg, 9, 1>; // Force Generic Payload Write Error
        using FGPTXE = Field<DSI_FIR1_Reg, 10, 1>; // Force Generic Payload Transmit Error
        using FGPRDE = Field<DSI_FIR1_Reg, 11, 1>; // Force Generic Payload Read Error
        using FGPRXE = Field<DSI_FIR1_Reg, 12, 1>; // Force Generic Payload Receive Error
    };

    // DSI Host Video Shadow Control Register
    using DSI_VSCR_Reg = Register<DSI_BASE + 0x100, ReadWrite>;
    struct _DSI_VSCR {
        using EN = Field<DSI_VSCR_Reg, 0, 1>; // Enable
        using UR = Field<DSI_VSCR_Reg, 8, 1>; // Update Register
    };

    // DSI Host LTDC Current VCID Register
    using DSI_LCVCIDR_Reg = Register<DSI_BASE + 0x10C, ReadOnly>;
    struct _DSI_LCVCIDR {
        using VCID = Field<DSI_LCVCIDR_Reg, 0, 2>; // Virtual Channel ID
    };

    // DSI Host LTDC Current Color Coding Register
    using DSI_LCCCR_Reg = Register<DSI_BASE + 0x110, ReadOnly>;
    struct _DSI_LCCCR {
        using COLC = Field<DSI_LCCCR_Reg, 0, 4>; // Color Coding
        using LPE = Field<DSI_LCCCR_Reg, 8, 1>; // Loosely Packed Enable
    };

    // DSI Host Low-Power mode Current Configuration Register
    using DSI_LPMCCR_Reg = Register<DSI_BASE + 0x118, ReadOnly>;
    struct _DSI_LPMCCR {
        using VLPSIZE = Field<DSI_LPMCCR_Reg, 0, 8>; // VACT Largest Packet Size
        using LPSIZE = Field<DSI_LPMCCR_Reg, 16, 8>; // Largest Packet Size
    };

    // DSI Host Video mode Current Configuration Register
    using DSI_VMCCR_Reg = Register<DSI_BASE + 0x138, ReadOnly>;
    struct _DSI_VMCCR {
        using VMT = Field<DSI_VMCCR_Reg, 0, 2>; // Video mode Type
        using LPVSAE = Field<DSI_VMCCR_Reg, 2, 1>; // Low-Power Vertical Sync time Enable
        using LPVBPE = Field<DSI_VMCCR_Reg, 3, 1>; // Low-power Vertical Back-Porch Enable
        using LPVFPE = Field<DSI_VMCCR_Reg, 4, 1>; // Low-power Vertical Front-Porch Enable
        using LPVAE = Field<DSI_VMCCR_Reg, 5, 1>; // Low-Power Vertical Active Enable
        using LPHBPE = Field<DSI_VMCCR_Reg, 6, 1>; // Low-power Horizontal Back-Porch Enable
        using LPHFE = Field<DSI_VMCCR_Reg, 7, 1>; // Low-Power Horizontal Front-Porch Enable
        using FBTAAE = Field<DSI_VMCCR_Reg, 8, 1>; // Frame BTA Acknowledge Enable
        using LPCE = Field<DSI_VMCCR_Reg, 9, 1>; // Low-Power Command Enable
    };

    // DSI Host Video Packet Current Configuration Register
    using DSI_VPCCR_Reg = Register<DSI_BASE + 0x13C, ReadOnly>;
    struct _DSI_VPCCR {
        using VPSIZE = Field<DSI_VPCCR_Reg, 0, 14>; // Video Packet Size
    };

    // DSI Host Video Chunks Current Configuration Register
    using DSI_VCCCR_Reg = Register<DSI_BASE + 0x140, ReadOnly>;
    struct _DSI_VCCCR {
        using NUMC = Field<DSI_VCCCR_Reg, 0, 13>; // Number of Chunks
    };

    // DSI Host Video Null Packet Current Configuration Register
    using DSI_VNPCCR_Reg = Register<DSI_BASE + 0x144, ReadOnly>;
    struct _DSI_VNPCCR {
        using NPSIZE = Field<DSI_VNPCCR_Reg, 0, 13>; // Null Packet Size
    };

    // DSI Host Video HSA Current Configuration Register
    using DSI_VHSACCR_Reg = Register<DSI_BASE + 0x148, ReadOnly>;
    struct _DSI_VHSACCR {
        using HSA = Field<DSI_VHSACCR_Reg, 0, 12>; // Horizontal Synchronism Active duration
    };

    // DSI Host Video HBP Current Configuration Register
    using DSI_VHBPCCR_Reg = Register<DSI_BASE + 0x14C, ReadOnly>;
    struct _DSI_VHBPCCR {
        using HBP = Field<DSI_VHBPCCR_Reg, 0, 12>; // Horizontal Back-Porch duration
    };

    // DSI Host Video Line Current Configuration Register
    using DSI_VLCCR_Reg = Register<DSI_BASE + 0x150, ReadOnly>;
    struct _DSI_VLCCR {
        using HLINE = Field<DSI_VLCCR_Reg, 0, 15>; // Horizontal Line duration
    };

    // DSI Host Video VSA Current Configuration Register
    using DSI_VVSACCR_Reg = Register<DSI_BASE + 0x154, ReadOnly>;
    struct _DSI_VVSACCR {
        using VSA = Field<DSI_VVSACCR_Reg, 0, 10>; // Vertical Synchronism Active duration
    };

    // DSI Host Video VBP Current Configuration Register
    using DSI_VVBPCCR_Reg = Register<DSI_BASE + 0x158, ReadOnly>;
    struct _DSI_VVBPCCR {
        using VBP = Field<DSI_VVBPCCR_Reg, 0, 10>; // Vertical Back-Porch duration
    };

    // DSI Host Video VFP Current Configuration Register
    using DSI_VVFPCCR_Reg = Register<DSI_BASE + 0x15C, ReadOnly>;
    struct _DSI_VVFPCCR {
        using VFP = Field<DSI_VVFPCCR_Reg, 0, 10>; // Vertical Front-Porch duration
    };

    // DSI Host Video VA Current Configuration Register
    using DSI_VVACCR_Reg = Register<DSI_BASE + 0x160, ReadOnly>;
    struct _DSI_VVACCR {
        using VA = Field<DSI_VVACCR_Reg, 0, 14>; // Vertical Active duration
    };

    // DSI Wrapper Configuration Register
    using DSI_WCFGR_Reg = Register<DSI_BASE + 0x400, ReadWrite>;
    struct _DSI_WCFGR {
        using VSPOL = Field<DSI_WCFGR_Reg, 7, 1>; // VSync Polarity
        using AR = Field<DSI_WCFGR_Reg, 6, 1>; // Automatic Refresh
        using TEPOL = Field<DSI_WCFGR_Reg, 5, 1>; // TE Polarity
        using TESRC = Field<DSI_WCFGR_Reg, 4, 1>; // TE Source
        using COLMUX = Field<DSI_WCFGR_Reg, 1, 3>; // Color Multiplexing
        using DSIM = Field<DSI_WCFGR_Reg, 0, 1>; // DSI Mode
    };

    // DSI Wrapper Control Register
    using DSI_WCR_Reg = Register<DSI_BASE + 0x404, ReadWrite>;
    struct _DSI_WCR {
        using DSIEN = Field<DSI_WCR_Reg, 3, 1>; // DSI Enable
        using LTDCEN = Field<DSI_WCR_Reg, 2, 1>; // LTDC Enable
        using SHTDN = Field<DSI_WCR_Reg, 1, 1>; // Shutdown
        using COLM = Field<DSI_WCR_Reg, 0, 1>; // Color Mode
    };

    // DSI Wrapper Interrupt Enable Register
    using DSI_WIER_Reg = Register<DSI_BASE + 0x408, ReadWrite>;
    struct _DSI_WIER {
        using RRIE = Field<DSI_WIER_Reg, 13, 1>; // Regulator Ready Interrupt Enable
        using PLLUIE = Field<DSI_WIER_Reg, 10, 1>; // PLL Unlock Interrupt Enable
        using PLLLIE = Field<DSI_WIER_Reg, 9, 1>; // PLL Lock Interrupt Enable
        using ERIE = Field<DSI_WIER_Reg, 1, 1>; // End of Refresh Interrupt Enable
        using TEIE = Field<DSI_WIER_Reg, 0, 1>; // Tearing Effect Interrupt Enable
    };

    // DSI Wrapper Interrupt & Status Register
    using DSI_WISR_Reg = Register<DSI_BASE + 0x40C, ReadOnly>;
    struct _DSI_WISR {
        using RRIF = Field<DSI_WISR_Reg, 13, 1>; // Regulator Ready Interrupt Flag
        using RRS = Field<DSI_WISR_Reg, 12, 1>; // Regulator Ready Status
        using PLLUIF = Field<DSI_WISR_Reg, 10, 1>; // PLL Unlock Interrupt Flag
        using PLLLIF = Field<DSI_WISR_Reg, 9, 1>; // PLL Lock Interrupt Flag
        using PLLLS = Field<DSI_WISR_Reg, 8, 1>; // PLL Lock Status
        using BUSY = Field<DSI_WISR_Reg, 2, 1>; // Busy Flag
        using ERIF = Field<DSI_WISR_Reg, 1, 1>; // End of Refresh Interrupt Flag
        using TEIF = Field<DSI_WISR_Reg, 0, 1>; // Tearing Effect Interrupt Flag
    };

    // DSI Wrapper Interrupt Flag Clear Register
    using DSI_WIFCR_Reg = Register<DSI_BASE + 0x410, ReadWrite>;
    struct _DSI_WIFCR {
        using CRRIF = Field<DSI_WIFCR_Reg, 13, 1>; // Clear Regulator Ready Interrupt Flag
        using CPLLUIF = Field<DSI_WIFCR_Reg, 10, 1>; // Clear PLL Unlock Interrupt Flag
        using CPLLLIF = Field<DSI_WIFCR_Reg, 9, 1>; // Clear PLL Lock Interrupt Flag
        using CERIF = Field<DSI_WIFCR_Reg, 1, 1>; // Clear End of Refresh Interrupt Flag
        using CTEIF = Field<DSI_WIFCR_Reg, 0, 1>; // Clear Tearing Effect Interrupt Flag
    };

    // DSI Wrapper PHY Configuration Register 1
    using DSI_WPCR1_Reg = Register<DSI_BASE + 0x418, ReadWrite>;
    struct _DSI_WPCR1 {
        using TCLKPOSTEN = Field<DSI_WPCR1_Reg, 27, 1>; // custom time for tCLK-POST Enable
        using TLPXCEN = Field<DSI_WPCR1_Reg, 26, 1>; // custom time for tLPX for Clock lane Enable
        using THSEXITEN = Field<DSI_WPCR1_Reg, 25, 1>; // custom time for tHS-EXIT Enable
        using TLPXDEN = Field<DSI_WPCR1_Reg, 24, 1>; // custom time for tLPX for Data lanes Enable
        using THSZEROEN = Field<DSI_WPCR1_Reg, 23, 1>; // custom time for tHS-ZERO Enable
        using THSTRAILEN = Field<DSI_WPCR1_Reg, 22, 1>; // custom time for tHS-TRAIL Enable
        using THSPREPEN = Field<DSI_WPCR1_Reg, 21, 1>; // custom time for tHS-PREPARE Enable
        using TCLKZEROEN = Field<DSI_WPCR1_Reg, 20, 1>; // custom time for tCLK-ZERO Enable
        using TCLKPREPEN = Field<DSI_WPCR1_Reg, 19, 1>; // custom time for tCLK-PREPARE Enable
        using PDEN = Field<DSI_WPCR1_Reg, 18, 1>; // Pull-Down Enable
        using TDDL = Field<DSI_WPCR1_Reg, 16, 1>; // Turn Disable Data Lanes
        using CDOFFDL = Field<DSI_WPCR1_Reg, 14, 1>; // Contention Detection OFF on Data Lanes
        using FTXSMDL = Field<DSI_WPCR1_Reg, 13, 1>; // Force in TX Stop Mode the Data Lanes
        using FTXSMCL = Field<DSI_WPCR1_Reg, 12, 1>; // Force in TX Stop Mode the Clock Lane
        using HSIDL1 = Field<DSI_WPCR1_Reg, 11, 1>; // Invert the High-Speed data signal on Data Lane 1
        using HSIDL0 = Field<DSI_WPCR1_Reg, 10, 1>; // Invert the Hight-Speed data signal on Data Lane 0
        using HSICL = Field<DSI_WPCR1_Reg, 9, 1>; // Invert Hight-Speed data signal on Clock Lane
        using SWDL1 = Field<DSI_WPCR1_Reg, 8, 1>; // Swap Data Lane 1 pins
        using SWDL0 = Field<DSI_WPCR1_Reg, 7, 1>; // Swap Data Lane 0 pins
        using SWCL = Field<DSI_WPCR1_Reg, 6, 1>; // Swap Clock Lane pins
        using UIX4 = Field<DSI_WPCR1_Reg, 0, 6>; // Unit Interval multiplied by 4
    };

    // DSI Wrapper PHY Configuration Register 2
    using DSI_WPCR2_Reg = Register<DSI_BASE + 0x41C, ReadWrite>;
    struct _DSI_WPCR2 {
        using LPRXFT = Field<DSI_WPCR2_Reg, 25, 2>; // Low-Power RX low-pass Filtering Tuning
        using FLPRXLPM = Field<DSI_WPCR2_Reg, 22, 1>; // Forces LP Receiver in Low-Power Mode
        using HSTXSRCDL = Field<DSI_WPCR2_Reg, 18, 2>; // High-Speed Transmission Slew Rate Control on Data Lanes
        using HSTXSRCCL = Field<DSI_WPCR2_Reg, 16, 2>; // High-Speed Transmission Slew Rate Control on Clock Lane
        using SDCC = Field<DSI_WPCR2_Reg, 12, 1>; // SDD Control
        using LPSRDL = Field<DSI_WPCR2_Reg, 8, 2>; // Low-Power transmission Slew Rate Compensation on Data Lanes
        using LPSRCL = Field<DSI_WPCR2_Reg, 6, 2>; // Low-Power transmission Slew Rate Compensation on Clock Lane
        using HSTXDLL = Field<DSI_WPCR2_Reg, 2, 2>; // High-Speed Transmission Delay on Data Lanes
        using HSTXDCL = Field<DSI_WPCR2_Reg, 0, 2>; // High-Speed Transmission Delay on Clock Lane
    };

    // DSI Wrapper PHY Configuration Register 3
    using DSI_WPCR3_Reg = Register<DSI_BASE + 0x420, ReadWrite>;
    struct _DSI_WPCR3 {
        using THSTRAIL = Field<DSI_WPCR3_Reg, 24, 8>; // tHSTRAIL
        using THSPREP = Field<DSI_WPCR3_Reg, 16, 8>; // tHS-PREPARE
        using TCLKZEO = Field<DSI_WPCR3_Reg, 8, 8>; // tCLK-ZERO
        using TCLKPREP = Field<DSI_WPCR3_Reg, 0, 8>; // tCLK-PREPARE
    };

    // DSI_WPCR4
    using DSI_WPCR4_Reg = Register<DSI_BASE + 0x424, ReadWrite>;
    struct _DSI_WPCR4 {
        using TLPXC = Field<DSI_WPCR4_Reg, 24, 8>; // tLPXC for Clock lane
        using THSEXIT = Field<DSI_WPCR4_Reg, 16, 8>; // tHSEXIT
        using TLPXD = Field<DSI_WPCR4_Reg, 8, 8>; // tLPX for Data lanes
        using THSZERO = Field<DSI_WPCR4_Reg, 0, 8>; // tHS-ZERO
    };

    // DSI Wrapper PHY Configuration Register 5
    using DSI_WPCR5_Reg = Register<DSI_BASE + 0x428, ReadWrite>;
    struct _DSI_WPCR5 {
        using THSZERO = Field<DSI_WPCR5_Reg, 0, 8>; // tCLK-POST
    };

    // DSI Wrapper Regulator and PLL Control Register
    using DSI_WRPCR_Reg = Register<DSI_BASE + 0x430, ReadWrite>;
    struct _DSI_WRPCR {
        using REGEN = Field<DSI_WRPCR_Reg, 24, 1>; // Regulator Enable
        using ODF = Field<DSI_WRPCR_Reg, 16, 2>; // PLL Output Division Factor
        using IDF = Field<DSI_WRPCR_Reg, 11, 4>; // PLL Input Division Factor
        using NDIV = Field<DSI_WRPCR_Reg, 2, 7>; // PLL Loop Division Factor
        using PLLEN = Field<DSI_WRPCR_Reg, 0, 1>; // PLL Enable
    };

} // namespace DSI

// --------------------------------------------
// DBGMCU: MCU debug component
// Base address: 0xE0042000
// --------------------------------------------

namespace DBGMCU {
    static constexpr uint32_t DBGMCU_BASE = 0xE0042000;

    // DBGMCU_IDCODE
    using IDCODE_Reg = Register<DBGMCU_BASE + 0x0, ReadOnly>;
    struct _IDCODE {
        using DEV_ID = Field<IDCODE_Reg, 0, 12>; // Device identifier
        using REV_ID = Field<IDCODE_Reg, 16, 16>; // Revision identifie
    };

} // namespace DBGMCU


 // stm32f767
