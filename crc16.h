/*
 * This file is part of the Aaru Data Preservation Suite.
 * Copyright (c) 2019-2023 Natalia Portillo.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AARU_CHECKSUMS_NATIVE_CRC16_H
#define AARU_CHECKSUMS_NATIVE_CRC16_H

typedef struct
{
    uint16_t crc;
} crc16_ctx;

const uint16_t crc16_table[8][256] = {
        {0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1,
                0xC481, 0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40, 0x0A00, 0xCAC1, 0xCB81, 0x0B40,
                0xC901, 0x09C0, 0x0880, 0xC841, 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1,
                0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
                0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1,
                0xF281, 0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441, 0x3C00, 0xFCC1, 0xFD81, 0x3D40,
                0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840, 0x2800, 0xE8C1,
                0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
                0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0,
                0x2080, 0xE041, 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740,
                0xA501, 0x65C0, 0x6480, 0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 0xAA01, 0x6AC0,
                0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840, 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
                0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1,
                0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041, 0x5000, 0x90C1, 0x9181, 0x5140,
                0x9301, 0x53C0, 0x5280, 0x9241, 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440, 0x9C01, 0x5CC0,
                0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
                0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0,
                0x4C80, 0x8C41, 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641, 0x8201, 0x42C0, 0x4380, 0x8341,
                0x4100, 0x81C1, 0x8081, 0x4040},
        {0x0000, 0x9001, 0x6001, 0xF000, 0xC002, 0x5003, 0xA003, 0x3002, 0xC007, 0x5006, 0xA006, 0x3007, 0x0005, 0x9004,
                0x6004, 0xF005, 0xC00D, 0x500C, 0xA00C, 0x300D, 0x000F, 0x900E, 0x600E, 0xF00F, 0x000A, 0x900B, 0x600B, 0xF00A,
                0xC008, 0x5009, 0xA009, 0x3008, 0xC019, 0x5018, 0xA018, 0x3019, 0x001B, 0x901A, 0x601A, 0xF01B, 0x001E, 0x901F,
                0x601F, 0xF01E, 0xC01C, 0x501D, 0xA01D, 0x301C, 0x0014, 0x9015, 0x6015, 0xF014, 0xC016, 0x5017, 0xA017, 0x3016,
                0xC013, 0x5012, 0xA012, 0x3013, 0x0011, 0x9010, 0x6010, 0xF011, 0xC031, 0x5030, 0xA030, 0x3031, 0x0033, 0x9032,
                0x6032, 0xF033, 0x0036, 0x9037, 0x6037, 0xF036, 0xC034, 0x5035, 0xA035, 0x3034, 0x003C, 0x903D, 0x603D, 0xF03C,
                0xC03E, 0x503F, 0xA03F, 0x303E, 0xC03B, 0x503A, 0xA03A, 0x303B, 0x0039, 0x9038, 0x6038, 0xF039, 0x0028, 0x9029,
                0x6029, 0xF028, 0xC02A, 0x502B, 0xA02B, 0x302A, 0xC02F, 0x502E, 0xA02E, 0x302F, 0x002D, 0x902C, 0x602C, 0xF02D,
                0xC025, 0x5024, 0xA024, 0x3025, 0x0027, 0x9026, 0x6026, 0xF027, 0x0022, 0x9023, 0x6023, 0xF022, 0xC020, 0x5021,
                0xA021, 0x3020, 0xC061, 0x5060, 0xA060, 0x3061, 0x0063, 0x9062, 0x6062, 0xF063, 0x0066, 0x9067, 0x6067, 0xF066,
                0xC064, 0x5065, 0xA065, 0x3064, 0x006C, 0x906D, 0x606D, 0xF06C, 0xC06E, 0x506F, 0xA06F, 0x306E, 0xC06B, 0x506A,
                0xA06A, 0x306B, 0x0069, 0x9068, 0x6068, 0xF069, 0x0078, 0x9079, 0x6079, 0xF078, 0xC07A, 0x507B, 0xA07B, 0x307A,
                0xC07F, 0x507E, 0xA07E, 0x307F, 0x007D, 0x907C, 0x607C, 0xF07D, 0xC075, 0x5074, 0xA074, 0x3075, 0x0077, 0x9076,
                0x6076, 0xF077, 0x0072, 0x9073, 0x6073, 0xF072, 0xC070, 0x5071, 0xA071, 0x3070, 0x0050, 0x9051, 0x6051, 0xF050,
                0xC052, 0x5053, 0xA053, 0x3052, 0xC057, 0x5056, 0xA056, 0x3057, 0x0055, 0x9054, 0x6054, 0xF055, 0xC05D, 0x505C,
                0xA05C, 0x305D, 0x005F, 0x905E, 0x605E, 0xF05F, 0x005A, 0x905B, 0x605B, 0xF05A, 0xC058, 0x5059, 0xA059, 0x3058,
                0xC049, 0x5048, 0xA048, 0x3049, 0x004B, 0x904A, 0x604A, 0xF04B, 0x004E, 0x904F, 0x604F, 0xF04E, 0xC04C, 0x504D,
                0xA04D, 0x304C, 0x0044, 0x9045, 0x6045, 0xF044, 0xC046, 0x5047, 0xA047, 0x3046, 0xC043, 0x5042, 0xA042, 0x3043,
                0x0041, 0x9040, 0x6040, 0xF041},
        {0x0000, 0xC051, 0xC0A1, 0x00F0, 0xC141, 0x0110, 0x01E0, 0xC1B1, 0xC281, 0x02D0, 0x0220, 0xC271, 0x03C0, 0xC391,
                0xC361, 0x0330, 0xC501, 0x0550, 0x05A0, 0xC5F1, 0x0440, 0xC411, 0xC4E1, 0x04B0, 0x0780, 0xC7D1, 0xC721, 0x0770,
                0xC6C1, 0x0690, 0x0660, 0xC631, 0xCA01, 0x0A50, 0x0AA0, 0xCAF1, 0x0B40, 0xCB11, 0xCBE1, 0x0BB0, 0x0880, 0xC8D1,
                0xC821, 0x0870, 0xC9C1, 0x0990, 0x0960, 0xC931, 0x0F00, 0xCF51, 0xCFA1, 0x0FF0, 0xCE41, 0x0E10, 0x0EE0, 0xCEB1,
                0xCD81, 0x0DD0, 0x0D20, 0xCD71, 0x0CC0, 0xCC91, 0xCC61, 0x0C30, 0xD401, 0x1450, 0x14A0, 0xD4F1, 0x1540, 0xD511,
                0xD5E1, 0x15B0, 0x1680, 0xD6D1, 0xD621, 0x1670, 0xD7C1, 0x1790, 0x1760, 0xD731, 0x1100, 0xD151, 0xD1A1, 0x11F0,
                0xD041, 0x1010, 0x10E0, 0xD0B1, 0xD381, 0x13D0, 0x1320, 0xD371, 0x12C0, 0xD291, 0xD261, 0x1230, 0x1E00, 0xDE51,
                0xDEA1, 0x1EF0, 0xDF41, 0x1F10, 0x1FE0, 0xDFB1, 0xDC81, 0x1CD0, 0x1C20, 0xDC71, 0x1DC0, 0xDD91, 0xDD61, 0x1D30,
                0xDB01, 0x1B50, 0x1BA0, 0xDBF1, 0x1A40, 0xDA11, 0xDAE1, 0x1AB0, 0x1980, 0xD9D1, 0xD921, 0x1970, 0xD8C1, 0x1890,
                0x1860, 0xD831, 0xE801, 0x2850, 0x28A0, 0xE8F1, 0x2940, 0xE911, 0xE9E1, 0x29B0, 0x2A80, 0xEAD1, 0xEA21, 0x2A70,
                0xEBC1, 0x2B90, 0x2B60, 0xEB31, 0x2D00, 0xED51, 0xEDA1, 0x2DF0, 0xEC41, 0x2C10, 0x2CE0, 0xECB1, 0xEF81, 0x2FD0,
                0x2F20, 0xEF71, 0x2EC0, 0xEE91, 0xEE61, 0x2E30, 0x2200, 0xE251, 0xE2A1, 0x22F0, 0xE341, 0x2310, 0x23E0, 0xE3B1,
                0xE081, 0x20D0, 0x2020, 0xE071, 0x21C0, 0xE191, 0xE161, 0x2130, 0xE701, 0x2750, 0x27A0, 0xE7F1, 0x2640, 0xE611,
                0xE6E1, 0x26B0, 0x2580, 0xE5D1, 0xE521, 0x2570, 0xE4C1, 0x2490, 0x2460, 0xE431, 0x3C00, 0xFC51, 0xFCA1, 0x3CF0,
                0xFD41, 0x3D10, 0x3DE0, 0xFDB1, 0xFE81, 0x3ED0, 0x3E20, 0xFE71, 0x3FC0, 0xFF91, 0xFF61, 0x3F30, 0xF901, 0x3950,
                0x39A0, 0xF9F1, 0x3840, 0xF811, 0xF8E1, 0x38B0, 0x3B80, 0xFBD1, 0xFB21, 0x3B70, 0xFAC1, 0x3A90, 0x3A60, 0xFA31,
                0xF601, 0x3650, 0x36A0, 0xF6F1, 0x3740, 0xF711, 0xF7E1, 0x37B0, 0x3480, 0xF4D1, 0xF421, 0x3470, 0xF5C1, 0x3590,
                0x3560, 0xF531, 0x3300, 0xF351, 0xF3A1, 0x33F0, 0xF241, 0x3210, 0x32E0, 0xF2B1, 0xF181, 0x31D0, 0x3120, 0xF171,
                0x30C0, 0xF091, 0xF061, 0x3030},
        {0x0000, 0xFC01, 0xB801, 0x4400, 0x3001, 0xCC00, 0x8800, 0x7401, 0x6002, 0x9C03, 0xD803, 0x2402, 0x5003, 0xAC02,
                0xE802, 0x1403, 0xC004, 0x3C05, 0x7805, 0x8404, 0xF005, 0x0C04, 0x4804, 0xB405, 0xA006, 0x5C07, 0x1807, 0xE406,
                0x9007, 0x6C06, 0x2806, 0xD407, 0xC00B, 0x3C0A, 0x780A, 0x840B, 0xF00A, 0x0C0B, 0x480B, 0xB40A, 0xA009, 0x5C08,
                0x1808, 0xE409, 0x9008, 0x6C09, 0x2809, 0xD408, 0x000F, 0xFC0E, 0xB80E, 0x440F, 0x300E, 0xCC0F, 0x880F, 0x740E,
                0x600D, 0x9C0C, 0xD80C, 0x240D, 0x500C, 0xAC0D, 0xE80D, 0x140C, 0xC015, 0x3C14, 0x7814, 0x8415, 0xF014, 0x0C15,
                0x4815, 0xB414, 0xA017, 0x5C16, 0x1816, 0xE417, 0x9016, 0x6C17, 0x2817, 0xD416, 0x0011, 0xFC10, 0xB810, 0x4411,
                0x3010, 0xCC11, 0x8811, 0x7410, 0x6013, 0x9C12, 0xD812, 0x2413, 0x5012, 0xAC13, 0xE813, 0x1412, 0x001E, 0xFC1F,
                0xB81F, 0x441E, 0x301F, 0xCC1E, 0x881E, 0x741F, 0x601C, 0x9C1D, 0xD81D, 0x241C, 0x501D, 0xAC1C, 0xE81C, 0x141D,
                0xC01A, 0x3C1B, 0x781B, 0x841A, 0xF01B, 0x0C1A, 0x481A, 0xB41B, 0xA018, 0x5C19, 0x1819, 0xE418, 0x9019, 0x6C18,
                0x2818, 0xD419, 0xC029, 0x3C28, 0x7828, 0x8429, 0xF028, 0x0C29, 0x4829, 0xB428, 0xA02B, 0x5C2A, 0x182A, 0xE42B,
                0x902A, 0x6C2B, 0x282B, 0xD42A, 0x002D, 0xFC2C, 0xB82C, 0x442D, 0x302C, 0xCC2D, 0x882D, 0x742C, 0x602F, 0x9C2E,
                0xD82E, 0x242F, 0x502E, 0xAC2F, 0xE82F, 0x142E, 0x0022, 0xFC23, 0xB823, 0x4422, 0x3023, 0xCC22, 0x8822, 0x7423,
                0x6020, 0x9C21, 0xD821, 0x2420, 0x5021, 0xAC20, 0xE820, 0x1421, 0xC026, 0x3C27, 0x7827, 0x8426, 0xF027, 0x0C26,
                0x4826, 0xB427, 0xA024, 0x5C25, 0x1825, 0xE424, 0x9025, 0x6C24, 0x2824, 0xD425, 0x003C, 0xFC3D, 0xB83D, 0x443C,
                0x303D, 0xCC3C, 0x883C, 0x743D, 0x603E, 0x9C3F, 0xD83F, 0x243E, 0x503F, 0xAC3E, 0xE83E, 0x143F, 0xC038, 0x3C39,
                0x7839, 0x8438, 0xF039, 0x0C38, 0x4838, 0xB439, 0xA03A, 0x5C3B, 0x183B, 0xE43A, 0x903B, 0x6C3A, 0x283A, 0xD43B,
                0xC037, 0x3C36, 0x7836, 0x8437, 0xF036, 0x0C37, 0x4837, 0xB436, 0xA035, 0x5C34, 0x1834, 0xE435, 0x9034, 0x6C35,
                0x2835, 0xD434, 0x0033, 0xFC32, 0xB832, 0x4433, 0x3032, 0xCC33, 0x8833, 0x7432, 0x6031, 0x9C30, 0xD830, 0x2431,
                0x5030, 0xAC31, 0xE831, 0x1430},
        {0x0000, 0xC03D, 0xC079, 0x0044, 0xC0F1, 0x00CC, 0x0088, 0xC0B5, 0xC1E1, 0x01DC, 0x0198, 0xC1A5, 0x0110, 0xC12D,
                0xC169, 0x0154, 0xC3C1, 0x03FC, 0x03B8, 0xC385, 0x0330, 0xC30D, 0xC349, 0x0374, 0x0220, 0xC21D, 0xC259, 0x0264,
                0xC2D1, 0x02EC, 0x02A8, 0xC295, 0xC781, 0x07BC, 0x07F8, 0xC7C5, 0x0770, 0xC74D, 0xC709, 0x0734, 0x0660, 0xC65D,
                0xC619, 0x0624, 0xC691, 0x06AC, 0x06E8, 0xC6D5, 0x0440, 0xC47D, 0xC439, 0x0404, 0xC4B1, 0x048C, 0x04C8, 0xC4F5,
                0xC5A1, 0x059C, 0x05D8, 0xC5E5, 0x0550, 0xC56D, 0xC529, 0x0514, 0xCF01, 0x0F3C, 0x0F78, 0xCF45, 0x0FF0, 0xCFCD,
                0xCF89, 0x0FB4, 0x0EE0, 0xCEDD, 0xCE99, 0x0EA4, 0xCE11, 0x0E2C, 0x0E68, 0xCE55, 0x0CC0, 0xCCFD, 0xCCB9, 0x0C84,
                0xCC31, 0x0C0C, 0x0C48, 0xCC75, 0xCD21, 0x0D1C, 0x0D58, 0xCD65, 0x0DD0, 0xCDED, 0xCDA9, 0x0D94, 0x0880, 0xC8BD,
                0xC8F9, 0x08C4, 0xC871, 0x084C, 0x0808, 0xC835, 0xC961, 0x095C, 0x0918, 0xC925, 0x0990, 0xC9AD, 0xC9E9, 0x09D4,
                0xCB41, 0x0B7C, 0x0B38, 0xCB05, 0x0BB0, 0xCB8D, 0xCBC9, 0x0BF4, 0x0AA0, 0xCA9D, 0xCAD9, 0x0AE4, 0xCA51, 0x0A6C,
                0x0A28, 0xCA15, 0xDE01, 0x1E3C, 0x1E78, 0xDE45, 0x1EF0, 0xDECD, 0xDE89, 0x1EB4, 0x1FE0, 0xDFDD, 0xDF99, 0x1FA4,
                0xDF11, 0x1F2C, 0x1F68, 0xDF55, 0x1DC0, 0xDDFD, 0xDDB9, 0x1D84, 0xDD31, 0x1D0C, 0x1D48, 0xDD75, 0xDC21, 0x1C1C,
                0x1C58, 0xDC65, 0x1CD0, 0xDCED, 0xDCA9, 0x1C94, 0x1980, 0xD9BD, 0xD9F9, 0x19C4, 0xD971, 0x194C, 0x1908, 0xD935,
                0xD861, 0x185C, 0x1818, 0xD825, 0x1890, 0xD8AD, 0xD8E9, 0x18D4, 0xDA41, 0x1A7C, 0x1A38, 0xDA05, 0x1AB0, 0xDA8D,
                0xDAC9, 0x1AF4, 0x1BA0, 0xDB9D, 0xDBD9, 0x1BE4, 0xDB51, 0x1B6C, 0x1B28, 0xDB15, 0x1100, 0xD13D, 0xD179, 0x1144,
                0xD1F1, 0x11CC, 0x1188, 0xD1B5, 0xD0E1, 0x10DC, 0x1098, 0xD0A5, 0x1010, 0xD02D, 0xD069, 0x1054, 0xD2C1, 0x12FC,
                0x12B8, 0xD285, 0x1230, 0xD20D, 0xD249, 0x1274, 0x1320, 0xD31D, 0xD359, 0x1364, 0xD3D1, 0x13EC, 0x13A8, 0xD395,
                0xD681, 0x16BC, 0x16F8, 0xD6C5, 0x1670, 0xD64D, 0xD609, 0x1634, 0x1760, 0xD75D, 0xD719, 0x1724, 0xD791, 0x17AC,
                0x17E8, 0xD7D5, 0x1540, 0xD57D, 0xD539, 0x1504, 0xD5B1, 0x158C, 0x15C8, 0xD5F5, 0xD4A1, 0x149C, 0x14D8, 0xD4E5,
                0x1450, 0xD46D, 0xD429, 0x1414},
        {0x0000, 0xD101, 0xE201, 0x3300, 0x8401, 0x5500, 0x6600, 0xB701, 0x4801, 0x9900, 0xAA00, 0x7B01, 0xCC00, 0x1D01,
                0x2E01, 0xFF00, 0x9002, 0x4103, 0x7203, 0xA302, 0x1403, 0xC502, 0xF602, 0x2703, 0xD803, 0x0902, 0x3A02, 0xEB03,
                0x5C02, 0x8D03, 0xBE03, 0x6F02, 0x6007, 0xB106, 0x8206, 0x5307, 0xE406, 0x3507, 0x0607, 0xD706, 0x2806, 0xF907,
                0xCA07, 0x1B06, 0xAC07, 0x7D06, 0x4E06, 0x9F07, 0xF005, 0x2104, 0x1204, 0xC305, 0x7404, 0xA505, 0x9605, 0x4704,
                0xB804, 0x6905, 0x5A05, 0x8B04, 0x3C05, 0xED04, 0xDE04, 0x0F05, 0xC00E, 0x110F, 0x220F, 0xF30E, 0x440F, 0x950E,
                0xA60E, 0x770F, 0x880F, 0x590E, 0x6A0E, 0xBB0F, 0x0C0E, 0xDD0F, 0xEE0F, 0x3F0E, 0x500C, 0x810D, 0xB20D, 0x630C,
                0xD40D, 0x050C, 0x360C, 0xE70D, 0x180D, 0xC90C, 0xFA0C, 0x2B0D, 0x9C0C, 0x4D0D, 0x7E0D, 0xAF0C, 0xA009, 0x7108,
                0x4208, 0x9309, 0x2408, 0xF509, 0xC609, 0x1708, 0xE808, 0x3909, 0x0A09, 0xDB08, 0x6C09, 0xBD08, 0x8E08, 0x5F09,
                0x300B, 0xE10A, 0xD20A, 0x030B, 0xB40A, 0x650B, 0x560B, 0x870A, 0x780A, 0xA90B, 0x9A0B, 0x4B0A, 0xFC0B, 0x2D0A,
                0x1E0A, 0xCF0B, 0xC01F, 0x111E, 0x221E, 0xF31F, 0x441E, 0x951F, 0xA61F, 0x771E, 0x881E, 0x591F, 0x6A1F, 0xBB1E,
                0x0C1F, 0xDD1E, 0xEE1E, 0x3F1F, 0x501D, 0x811C, 0xB21C, 0x631D, 0xD41C, 0x051D, 0x361D, 0xE71C, 0x181C, 0xC91D,
                0xFA1D, 0x2B1C, 0x9C1D, 0x4D1C, 0x7E1C, 0xAF1D, 0xA018, 0x7119, 0x4219, 0x9318, 0x2419, 0xF518, 0xC618, 0x1719,
                0xE819, 0x3918, 0x0A18, 0xDB19, 0x6C18, 0xBD19, 0x8E19, 0x5F18, 0x301A, 0xE11B, 0xD21B, 0x031A, 0xB41B, 0x651A,
                0x561A, 0x871B, 0x781B, 0xA91A, 0x9A1A, 0x4B1B, 0xFC1A, 0x2D1B, 0x1E1B, 0xCF1A, 0x0011, 0xD110, 0xE210, 0x3311,
                0x8410, 0x5511, 0x6611, 0xB710, 0x4810, 0x9911, 0xAA11, 0x7B10, 0xCC11, 0x1D10, 0x2E10, 0xFF11, 0x9013, 0x4112,
                0x7212, 0xA313, 0x1412, 0xC513, 0xF613, 0x2712, 0xD812, 0x0913, 0x3A13, 0xEB12, 0x5C13, 0x8D12, 0xBE12, 0x6F13,
                0x6016, 0xB117, 0x8217, 0x5316, 0xE417, 0x3516, 0x0616, 0xD717, 0x2817, 0xF916, 0xCA16, 0x1B17, 0xAC16, 0x7D17,
                0x4E17, 0x9F16, 0xF014, 0x2115, 0x1215, 0xC314, 0x7415, 0xA514, 0x9614, 0x4715, 0xB815, 0x6914, 0x5A14, 0x8B15,
                0x3C14, 0xED15, 0xDE15, 0x0F14},
        {0x0000, 0xC010, 0xC023, 0x0033, 0xC045, 0x0055, 0x0066, 0xC076, 0xC089, 0x0099, 0x00AA, 0xC0BA, 0x00CC, 0xC0DC,
                0xC0EF, 0x00FF, 0xC111, 0x0101, 0x0132, 0xC122, 0x0154, 0xC144, 0xC177, 0x0167, 0x0198, 0xC188, 0xC1BB, 0x01AB,
                0xC1DD, 0x01CD, 0x01FE, 0xC1EE, 0xC221, 0x0231, 0x0202, 0xC212, 0x0264, 0xC274, 0xC247, 0x0257, 0x02A8, 0xC2B8,
                0xC28B, 0x029B, 0xC2ED, 0x02FD, 0x02CE, 0xC2DE, 0x0330, 0xC320, 0xC313, 0x0303, 0xC375, 0x0365, 0x0356, 0xC346,
                0xC3B9, 0x03A9, 0x039A, 0xC38A, 0x03FC, 0xC3EC, 0xC3DF, 0x03CF, 0xC441, 0x0451, 0x0462, 0xC472, 0x0404, 0xC414,
                0xC427, 0x0437, 0x04C8, 0xC4D8, 0xC4EB, 0x04FB, 0xC48D, 0x049D, 0x04AE, 0xC4BE, 0x0550, 0xC540, 0xC573, 0x0563,
                0xC515, 0x0505, 0x0536, 0xC526, 0xC5D9, 0x05C9, 0x05FA, 0xC5EA, 0x059C, 0xC58C, 0xC5BF, 0x05AF, 0x0660, 0xC670,
                0xC643, 0x0653, 0xC625, 0x0635, 0x0606, 0xC616, 0xC6E9, 0x06F9, 0x06CA, 0xC6DA, 0x06AC, 0xC6BC, 0xC68F, 0x069F,
                0xC771, 0x0761, 0x0752, 0xC742, 0x0734, 0xC724, 0xC717, 0x0707, 0x07F8, 0xC7E8, 0xC7DB, 0x07CB, 0xC7BD, 0x07AD,
                0x079E, 0xC78E, 0xC881, 0x0891, 0x08A2, 0xC8B2, 0x08C4, 0xC8D4, 0xC8E7, 0x08F7, 0x0808, 0xC818, 0xC82B, 0x083B,
                0xC84D, 0x085D, 0x086E, 0xC87E, 0x0990, 0xC980, 0xC9B3, 0x09A3, 0xC9D5, 0x09C5, 0x09F6, 0xC9E6, 0xC919, 0x0909,
                0x093A, 0xC92A, 0x095C, 0xC94C, 0xC97F, 0x096F, 0x0AA0, 0xCAB0, 0xCA83, 0x0A93, 0xCAE5, 0x0AF5, 0x0AC6, 0xCAD6,
                0xCA29, 0x0A39, 0x0A0A, 0xCA1A, 0x0A6C, 0xCA7C, 0xCA4F, 0x0A5F, 0xCBB1, 0x0BA1, 0x0B92, 0xCB82, 0x0BF4, 0xCBE4,
                0xCBD7, 0x0BC7, 0x0B38, 0xCB28, 0xCB1B, 0x0B0B, 0xCB7D, 0x0B6D, 0x0B5E, 0xCB4E, 0x0CC0, 0xCCD0, 0xCCE3, 0x0CF3,
                0xCC85, 0x0C95, 0x0CA6, 0xCCB6, 0xCC49, 0x0C59, 0x0C6A, 0xCC7A, 0x0C0C, 0xCC1C, 0xCC2F, 0x0C3F, 0xCDD1, 0x0DC1,
                0x0DF2, 0xCDE2, 0x0D94, 0xCD84, 0xCDB7, 0x0DA7, 0x0D58, 0xCD48, 0xCD7B, 0x0D6B, 0xCD1D, 0x0D0D, 0x0D3E, 0xCD2E,
                0xCEE1, 0x0EF1, 0x0EC2, 0xCED2, 0x0EA4, 0xCEB4, 0xCE87, 0x0E97, 0x0E68, 0xCE78, 0xCE4B, 0x0E5B, 0xCE2D, 0x0E3D,
                0x0E0E, 0xCE1E, 0x0FF0, 0xCFE0, 0xCFD3, 0x0FC3, 0xCFB5, 0x0FA5, 0x0F96, 0xCF86, 0xCF79, 0x0F69, 0x0F5A, 0xCF4A,
                0x0F3C, 0xCF2C, 0xCF1F, 0x0F0F},
        {0x0000, 0xCCC1, 0xD981, 0x1540, 0xF301, 0x3FC0, 0x2A80, 0xE641, 0xA601, 0x6AC0, 0x7F80, 0xB341, 0x5500, 0x99C1,
                0x8C81, 0x4040, 0x0C01, 0xC0C0, 0xD580, 0x1941, 0xFF00, 0x33C1, 0x2681, 0xEA40, 0xAA00, 0x66C1, 0x7381, 0xBF40,
                0x5901, 0x95C0, 0x8080, 0x4C41, 0x1802, 0xD4C3, 0xC183, 0x0D42, 0xEB03, 0x27C2, 0x3282, 0xFE43, 0xBE03, 0x72C2,
                0x6782, 0xAB43, 0x4D02, 0x81C3, 0x9483, 0x5842, 0x1403, 0xD8C2, 0xCD82, 0x0143, 0xE702, 0x2BC3, 0x3E83, 0xF242,
                0xB202, 0x7EC3, 0x6B83, 0xA742, 0x4103, 0x8DC2, 0x9882, 0x5443, 0x3004, 0xFCC5, 0xE985, 0x2544, 0xC305, 0x0FC4,
                0x1A84, 0xD645, 0x9605, 0x5AC4, 0x4F84, 0x8345, 0x6504, 0xA9C5, 0xBC85, 0x7044, 0x3C05, 0xF0C4, 0xE584, 0x2945,
                0xCF04, 0x03C5, 0x1685, 0xDA44, 0x9A04, 0x56C5, 0x4385, 0x8F44, 0x6905, 0xA5C4, 0xB084, 0x7C45, 0x2806, 0xE4C7,
                0xF187, 0x3D46, 0xDB07, 0x17C6, 0x0286, 0xCE47, 0x8E07, 0x42C6, 0x5786, 0x9B47, 0x7D06, 0xB1C7, 0xA487, 0x6846,
                0x2407, 0xE8C6, 0xFD86, 0x3147, 0xD706, 0x1BC7, 0x0E87, 0xC246, 0x8206, 0x4EC7, 0x5B87, 0x9746, 0x7107, 0xBDC6,
                0xA886, 0x6447, 0x6008, 0xACC9, 0xB989, 0x7548, 0x9309, 0x5FC8, 0x4A88, 0x8649, 0xC609, 0x0AC8, 0x1F88, 0xD349,
                0x3508, 0xF9C9, 0xEC89, 0x2048, 0x6C09, 0xA0C8, 0xB588, 0x7949, 0x9F08, 0x53C9, 0x4689, 0x8A48, 0xCA08, 0x06C9,
                0x1389, 0xDF48, 0x3909, 0xF5C8, 0xE088, 0x2C49, 0x780A, 0xB4CB, 0xA18B, 0x6D4A, 0x8B0B, 0x47CA, 0x528A, 0x9E4B,
                0xDE0B, 0x12CA, 0x078A, 0xCB4B, 0x2D0A, 0xE1CB, 0xF48B, 0x384A, 0x740B, 0xB8CA, 0xAD8A, 0x614B, 0x870A, 0x4BCB,
                0x5E8B, 0x924A, 0xD20A, 0x1ECB, 0x0B8B, 0xC74A, 0x210B, 0xEDCA, 0xF88A, 0x344B, 0x500C, 0x9CCD, 0x898D, 0x454C,
                0xA30D, 0x6FCC, 0x7A8C, 0xB64D, 0xF60D, 0x3ACC, 0x2F8C, 0xE34D, 0x050C, 0xC9CD, 0xDC8D, 0x104C, 0x5C0D, 0x90CC,
                0x858C, 0x494D, 0xAF0C, 0x63CD, 0x768D, 0xBA4C, 0xFA0C, 0x36CD, 0x238D, 0xEF4C, 0x090D, 0xC5CC, 0xD08C, 0x1C4D,
                0x480E, 0x84CF, 0x918F, 0x5D4E, 0xBB0F, 0x77CE, 0x628E, 0xAE4F, 0xEE0F, 0x22CE, 0x378E, 0xFB4F, 0x1D0E, 0xD1CF,
                0xC48F, 0x084E, 0x440F, 0x88CE, 0x9D8E, 0x514F, 0xB70E, 0x7BCF, 0x6E8F, 0xA24E, 0xE20E, 0x2ECF, 0x3B8F, 0xF74E,
                0x110F, 0xDDCE, 0xC88E, 0x044F}};

AARU_EXPORT crc16_ctx *AARU_CALL crc16_init();
AARU_EXPORT int AARU_CALL crc16_update(crc16_ctx *ctx, const uint8_t *data, uint32_t len);
AARU_EXPORT int AARU_CALL crc16_final(crc16_ctx *ctx, uint16_t *crc);
AARU_EXPORT void AARU_CALL crc16_free(crc16_ctx *ctx);

#endif // AARU_CHECKSUMS_NATIVE_CRC16_H