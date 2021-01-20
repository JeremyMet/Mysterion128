#ifndef __MYSTERION__
#define __MYSTERION__

#include "mysterion.h"

const m_state colonne_0 = {.state = { 1936946035, 1819044972, 741092396, 4210752250}};
const m_state colonne_1 = {.state = { 3149642683, 50529027, 1532713819, 2004318071}};
const m_state colonne_2 = {.state = { 134744072, 656877351, 3503345872, 2391707278}};
const m_state colonne_3 = {.state = { 2896997548, 2475922323, 2728567458, 3772834016}};
const m_state colonne_4 = {.state = { 437918234, 3840206052, 572662306, 2206434179}};
const m_state colonne_5 = {.state = { 2290649224, 4076008178, 2240120197, 3099113656}};
const m_state colonne_6 = {.state = { 1852730990, 1616928864, 3991793133, 4160223223}};
const m_state colonne_7 = {.state = { 3890735079, 454761243, 437918234, 791621423}};

const m_state colonne_inv_0 = {.state = { 3469659854, 909522486, 875836468, 1600085855}};
const m_state colonne_inv_1 = {.state = { 3890735079, 3638089944, 1482184792, 4109694196}};
const m_state colonne_inv_2 = {.state = { 1987475062, 101058054, 3082270647, 4025479151}};
const m_state colonne_inv_3 = {.state = { 286331153, 1330597711, 2711724449, 488447261}};
const m_state colonne_inv_4 = {.state = { 1482184792, 656877351, 1145324612, 3250700737}};
const m_state colonne_inv_5 = {.state = { 892679477, 3385444809, 1162167621, 117901063}};
const m_state colonne_inv_6 = {.state = { 269488144, 3840206052, 185273099, 1903260017}};
const m_state colonne_inv_7 = {.state = { 3722304989, 3233857728, 3671775962, 4008636142}};

const m_state L[8] = {colonne_0,
                colonne_1,
                colonne_2,
                colonne_3,
                colonne_4,
                colonne_5,
                colonne_6,
                colonne_7};

const m_state L_Inv[8] = {colonne_inv_0,
                colonne_inv_1,
                colonne_inv_2,
                colonne_inv_3,
                colonne_inv_4,
                colonne_inv_5,
                colonne_inv_6,
                colonne_inv_7};

const uint32_t C[NB_ROUND*4] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f};
const uint32_t C_Inv[NB_ROUND*4] = {0x2f, 0x2e, 0x2d, 0x2c, 0x2b, 0x2a, 0x29, 0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x9, 0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1, 0x0};

#endif
