#include <color.h>

// ---- 16 gray palette ----
static RgbaColor default16GrayColors[] = {
    {0, 0, 0, 255},
    {17, 17, 17, 255},
    {34, 34, 34, 255},
    {51, 51, 51, 255},
    {68, 68, 68, 255},
    {85, 85, 85, 255},
    {102, 102, 102, 255},
    {119, 119, 119, 255},
    {136, 136, 136, 255},
    {153, 153, 153, 255},
    {170, 170, 170, 255},
    {187, 187, 187, 255},
    {204, 204, 204, 255},
    {221, 221, 221, 255},
    {238, 238, 238, 255},
    {255, 255, 255, 255},
};
ColorPalette kDefault16GrayPalette = {
  .size = 16,
  .fillsize = 16,
  .data = default16GrayColors
};

// ---- 16 console palette ----
static RgbaColor default16ConsColors[] = {
    {0, 0, 0, 255},
    {0, 0, 128, 255},
    {0, 128, 0, 255},
    {0, 128, 128, 255},
    {128, 0, 0, 255},
    {128, 0, 128, 255},
    {128, 128, 0, 255},
    {192, 192, 192, 255},
    {128, 128, 128, 255},
    {0, 0, 255, 255},
    {0, 255, 0, 255},
    {0, 255, 255, 255},
    {255, 0, 0, 255},
    {255, 0, 255, 255},
    {255, 255, 0, 255},
    {255, 255, 255, 255},
};
ColorPalette kDefault16ConsPalette = {
  .size = 16,
  .fillsize = 16,
  .data = default16ConsColors
};

// ---- 256 grayscale palette ----
static RgbaColor default256GrayColors[] = {
    {0, 0, 0, 255},
    {1, 1, 1, 255},
    {2, 2, 2, 255},
    {3, 3, 3, 255},
    {4, 4, 4, 255},
    {5, 5, 5, 255},
    {6, 6, 6, 255},
    {7, 7, 7, 255},
    {8, 8, 8, 255},
    {9, 9, 9, 255},
    {10, 10, 10, 255},
    {11, 11, 11, 255},
    {12, 12, 12, 255},
    {13, 13, 13, 255},
    {14, 14, 14, 255},
    {15, 15, 15, 255},
    {16, 16, 16, 255},
    {17, 17, 17, 255},
    {18, 18, 18, 255},
    {19, 19, 19, 255},
    {20, 20, 20, 255},
    {21, 21, 21, 255},
    {22, 22, 22, 255},
    {23, 23, 23, 255},
    {24, 24, 24, 255},
    {25, 25, 25, 255},
    {26, 26, 26, 255},
    {27, 27, 27, 255},
    {28, 28, 28, 255},
    {29, 29, 29, 255},
    {30, 30, 30, 255},
    {31, 31, 31, 255},
    {32, 32, 32, 255},
    {33, 33, 33, 255},
    {34, 34, 34, 255},
    {35, 35, 35, 255},
    {36, 36, 36, 255},
    {37, 37, 37, 255},
    {38, 38, 38, 255},
    {39, 39, 39, 255},
    {40, 40, 40, 255},
    {41, 41, 41, 255},
    {42, 42, 42, 255},
    {43, 43, 43, 255},
    {44, 44, 44, 255},
    {45, 45, 45, 255},
    {46, 46, 46, 255},
    {47, 47, 47, 255},
    {48, 48, 48, 255},
    {49, 49, 49, 255},
    {50, 50, 50, 255},
    {51, 51, 51, 255},
    {52, 52, 52, 255},
    {53, 53, 53, 255},
    {54, 54, 54, 255},
    {55, 55, 55, 255},
    {56, 56, 56, 255},
    {57, 57, 57, 255},
    {58, 58, 58, 255},
    {59, 59, 59, 255},
    {60, 60, 60, 255},
    {61, 61, 61, 255},
    {62, 62, 62, 255},
    {63, 63, 63, 255},
    {64, 64, 64, 255},
    {65, 65, 65, 255},
    {66, 66, 66, 255},
    {67, 67, 67, 255},
    {68, 68, 68, 255},
    {69, 69, 69, 255},
    {70, 70, 70, 255},
    {71, 71, 71, 255},
    {72, 72, 72, 255},
    {73, 73, 73, 255},
    {74, 74, 74, 255},
    {75, 75, 75, 255},
    {76, 76, 76, 255},
    {77, 77, 77, 255},
    {78, 78, 78, 255},
    {79, 79, 79, 255},
    {80, 80, 80, 255},
    {81, 81, 81, 255},
    {82, 82, 82, 255},
    {83, 83, 83, 255},
    {84, 84, 84, 255},
    {85, 85, 85, 255},
    {86, 86, 86, 255},
    {87, 87, 87, 255},
    {88, 88, 88, 255},
    {89, 89, 89, 255},
    {90, 90, 90, 255},
    {91, 91, 91, 255},
    {92, 92, 92, 255},
    {93, 93, 93, 255},
    {94, 94, 94, 255},
    {95, 95, 95, 255},
    {96, 96, 96, 255},
    {97, 97, 97, 255},
    {98, 98, 98, 255},
    {99, 99, 99, 255},
    {100, 100, 100, 255},
    {101, 101, 101, 255},
    {102, 102, 102, 255},
    {103, 103, 103, 255},
    {104, 104, 104, 255},
    {105, 105, 105, 255},
    {106, 106, 106, 255},
    {107, 107, 107, 255},
    {108, 108, 108, 255},
    {109, 109, 109, 255},
    {110, 110, 110, 255},
    {111, 111, 111, 255},
    {112, 112, 112, 255},
    {113, 113, 113, 255},
    {114, 114, 114, 255},
    {115, 115, 115, 255},
    {116, 116, 116, 255},
    {117, 117, 117, 255},
    {118, 118, 118, 255},
    {119, 119, 119, 255},
    {120, 120, 120, 255},
    {121, 121, 121, 255},
    {122, 122, 122, 255},
    {123, 123, 123, 255},
    {124, 124, 124, 255},
    {125, 125, 125, 255},
    {126, 126, 126, 255},
    {127, 127, 127, 255},
    {128, 128, 128, 255},
    {129, 129, 129, 255},
    {130, 130, 130, 255},
    {131, 131, 131, 255},
    {132, 132, 132, 255},
    {133, 133, 133, 255},
    {134, 134, 134, 255},
    {135, 135, 135, 255},
    {136, 136, 136, 255},
    {137, 137, 137, 255},
    {138, 138, 138, 255},
    {139, 139, 139, 255},
    {140, 140, 140, 255},
    {141, 141, 141, 255},
    {142, 142, 142, 255},
    {143, 143, 143, 255},
    {144, 144, 144, 255},
    {145, 145, 145, 255},
    {146, 146, 146, 255},
    {147, 147, 147, 255},
    {148, 148, 148, 255},
    {149, 149, 149, 255},
    {150, 150, 150, 255},
    {151, 151, 151, 255},
    {152, 152, 152, 255},
    {153, 153, 153, 255},
    {154, 154, 154, 255},
    {155, 155, 155, 255},
    {156, 156, 156, 255},
    {157, 157, 157, 255},
    {158, 158, 158, 255},
    {159, 159, 159, 255},
    {160, 160, 160, 255},
    {161, 161, 161, 255},
    {162, 162, 162, 255},
    {163, 163, 163, 255},
    {164, 164, 164, 255},
    {165, 165, 165, 255},
    {166, 166, 166, 255},
    {167, 167, 167, 255},
    {168, 168, 168, 255},
    {169, 169, 169, 255},
    {170, 170, 170, 255},
    {171, 171, 171, 255},
    {172, 172, 172, 255},
    {173, 173, 173, 255},
    {174, 174, 174, 255},
    {175, 175, 175, 255},
    {176, 176, 176, 255},
    {177, 177, 177, 255},
    {178, 178, 178, 255},
    {179, 179, 179, 255},
    {180, 180, 180, 255},
    {181, 181, 181, 255},
    {182, 182, 182, 255},
    {183, 183, 183, 255},
    {184, 184, 184, 255},
    {185, 185, 185, 255},
    {186, 186, 186, 255},
    {187, 187, 187, 255},
    {188, 188, 188, 255},
    {189, 189, 189, 255},
    {190, 190, 190, 255},
    {191, 191, 191, 255},
    {192, 192, 192, 255},
    {193, 193, 193, 255},
    {194, 194, 194, 255},
    {195, 195, 195, 255},
    {196, 196, 196, 255},
    {197, 197, 197, 255},
    {198, 198, 198, 255},
    {199, 199, 199, 255},
    {200, 200, 200, 255},
    {201, 201, 201, 255},
    {202, 202, 202, 255},
    {203, 203, 203, 255},
    {204, 204, 204, 255},
    {205, 205, 205, 255},
    {206, 206, 206, 255},
    {207, 207, 207, 255},
    {208, 208, 208, 255},
    {209, 209, 209, 255},
    {210, 210, 210, 255},
    {211, 211, 211, 255},
    {212, 212, 212, 255},
    {213, 213, 213, 255},
    {214, 214, 214, 255},
    {215, 215, 215, 255},
    {216, 216, 216, 255},
    {217, 217, 217, 255},
    {218, 218, 218, 255},
    {219, 219, 219, 255},
    {220, 220, 220, 255},
    {221, 221, 221, 255},
    {222, 222, 222, 255},
    {223, 223, 223, 255},
    {224, 224, 224, 255},
    {225, 225, 225, 255},
    {226, 226, 226, 255},
    {227, 227, 227, 255},
    {228, 228, 228, 255},
    {229, 229, 229, 255},
    {230, 230, 230, 255},
    {231, 231, 231, 255},
    {232, 232, 232, 255},
    {233, 233, 233, 255},
    {234, 234, 234, 255},
    {235, 235, 235, 255},
    {236, 236, 236, 255},
    {237, 237, 237, 255},
    {238, 238, 238, 255},
    {239, 239, 239, 255},
    {240, 240, 240, 255},
    {241, 241, 241, 255},
    {242, 242, 242, 255},
    {243, 243, 243, 255},
    {244, 244, 244, 255},
    {245, 245, 245, 255},
    {246, 246, 246, 255},
    {247, 247, 247, 255},
    {248, 248, 248, 255},
    {249, 249, 249, 255},
    {250, 250, 250, 255},
    {251, 251, 251, 255},
    {252, 252, 252, 255},
    {253, 253, 253, 255},
    {254, 254, 254, 255},
    {255, 255, 255, 255},
};
ColorPalette kDefault256GrayPalette = {
  .size = 256,
  .fillsize = 256,
  .data = default256GrayColors
};

// ---- 256 rgb palette ----
static RgbaColor default256RgbColors[] = {
    {0, 0, 0, 255},
    {0, 0, 63, 255},
    {0, 0, 127, 255},
    {0, 0, 191, 255},
    {0, 0, 255, 255},
    {0, 36, 0, 255},
    {0, 36, 63, 255},
    {0, 36, 127, 255},
    {0, 36, 191, 255},
    {0, 36, 255, 255},
    {0, 72, 0, 255},
    {0, 72, 63, 255},
    {0, 72, 127, 255},
    {0, 72, 191, 255},
    {0, 72, 255, 255},
    {0, 109, 0, 255},
    {0, 109, 63, 255},
    {0, 109, 127, 255},
    {0, 109, 191, 255},
    {0, 109, 255, 255},
    {0, 145, 0, 255},
    {0, 145, 63, 255},
    {0, 145, 127, 255},
    {0, 145, 191, 255},
    {0, 145, 255, 255},
    {0, 182, 0, 255},
    {0, 182, 63, 255},
    {0, 182, 127, 255},
    {0, 182, 191, 255},
    {0, 182, 255, 255},
    {0, 218, 0, 255},
    {0, 218, 63, 255},
    {0, 218, 127, 255},
    {0, 218, 191, 255},
    {0, 218, 255, 255},
    {0, 255, 0, 255},
    {0, 255, 63, 255},
    {0, 255, 127, 255},
    {0, 255, 191, 255},
    {0, 255, 255, 255},
    {51, 0, 0, 255},
    {51, 0, 63, 255},
    {51, 0, 127, 255},
    {51, 0, 191, 255},
    {51, 0, 255, 255},
    {51, 36, 0, 255},
    {51, 36, 63, 255},
    {51, 36, 127, 255},
    {51, 36, 191, 255},
    {51, 36, 255, 255},
    {51, 72, 0, 255},
    {51, 72, 63, 255},
    {51, 72, 127, 255},
    {51, 72, 191, 255},
    {51, 72, 255, 255},
    {51, 109, 0, 255},
    {51, 109, 63, 255},
    {51, 109, 127, 255},
    {51, 109, 191, 255},
    {51, 109, 255, 255},
    {51, 145, 0, 255},
    {51, 145, 63, 255},
    {51, 145, 127, 255},
    {51, 145, 191, 255},
    {51, 145, 255, 255},
    {51, 182, 0, 255},
    {51, 182, 63, 255},
    {51, 182, 127, 255},
    {51, 182, 191, 255},
    {51, 182, 255, 255},
    {51, 218, 0, 255},
    {51, 218, 63, 255},
    {51, 218, 127, 255},
    {51, 218, 191, 255},
    {51, 218, 255, 255},
    {51, 255, 0, 255},
    {51, 255, 63, 255},
    {51, 255, 127, 255},
    {51, 255, 191, 255},
    {51, 255, 255, 255},
    {102, 0, 0, 255},
    {102, 0, 63, 255},
    {102, 0, 127, 255},
    {102, 0, 191, 255},
    {102, 0, 255, 255},
    {102, 36, 0, 255},
    {102, 36, 63, 255},
    {102, 36, 127, 255},
    {102, 36, 191, 255},
    {102, 36, 255, 255},
    {102, 72, 0, 255},
    {102, 72, 63, 255},
    {102, 72, 127, 255},
    {102, 72, 191, 255},
    {102, 72, 255, 255},
    {102, 109, 0, 255},
    {102, 109, 63, 255},
    {102, 109, 127, 255},
    {102, 109, 191, 255},
    {102, 109, 255, 255},
    {102, 145, 0, 255},
    {102, 145, 63, 255},
    {102, 145, 127, 255},
    {102, 145, 191, 255},
    {102, 145, 255, 255},
    {102, 182, 0, 255},
    {102, 182, 63, 255},
    {102, 182, 127, 255},
    {102, 182, 191, 255},
    {102, 182, 255, 255},
    {102, 218, 0, 255},
    {102, 218, 63, 255},
    {102, 218, 127, 255},
    {102, 218, 191, 255},
    {102, 218, 255, 255},
    {102, 255, 0, 255},
    {102, 255, 63, 255},
    {102, 255, 127, 255},
    {102, 255, 191, 255},
    {102, 255, 255, 255},
    {153, 0, 0, 255},
    {153, 0, 63, 255},
    {153, 0, 127, 255},
    {153, 0, 191, 255},
    {153, 0, 255, 255},
    {153, 36, 0, 255},
    {153, 36, 63, 255},
    {153, 36, 127, 255},
    {153, 36, 191, 255},
    {153, 36, 255, 255},
    {153, 72, 0, 255},
    {153, 72, 63, 255},
    {153, 72, 127, 255},
    {153, 72, 191, 255},
    {153, 72, 255, 255},
    {153, 109, 0, 255},
    {153, 109, 63, 255},
    {153, 109, 127, 255},
    {153, 109, 191, 255},
    {153, 109, 255, 255},
    {153, 145, 0, 255},
    {153, 145, 63, 255},
    {153, 145, 127, 255},
    {153, 145, 191, 255},
    {153, 145, 255, 255},
    {153, 182, 0, 255},
    {153, 182, 63, 255},
    {153, 182, 127, 255},
    {153, 182, 191, 255},
    {153, 182, 255, 255},
    {153, 218, 0, 255},
    {153, 218, 63, 255},
    {153, 218, 127, 255},
    {153, 218, 191, 255},
    {153, 218, 255, 255},
    {153, 255, 0, 255},
    {153, 255, 63, 255},
    {153, 255, 127, 255},
    {153, 255, 191, 255},
    {153, 255, 255, 255},
    {204, 0, 0, 255},
    {204, 0, 63, 255},
    {204, 0, 127, 255},
    {204, 0, 191, 255},
    {204, 0, 255, 255},
    {204, 36, 0, 255},
    {204, 36, 63, 255},
    {204, 36, 127, 255},
    {204, 36, 191, 255},
    {204, 36, 255, 255},
    {204, 72, 0, 255},
    {204, 72, 63, 255},
    {204, 72, 127, 255},
    {204, 72, 191, 255},
    {204, 72, 255, 255},
    {204, 109, 0, 255},
    {204, 109, 63, 255},
    {204, 109, 127, 255},
    {204, 109, 191, 255},
    {204, 109, 255, 255},
    {204, 145, 0, 255},
    {204, 145, 63, 255},
    {204, 145, 127, 255},
    {204, 145, 191, 255},
    {204, 145, 255, 255},
    {204, 182, 0, 255},
    {204, 182, 63, 255},
    {204, 182, 127, 255},
    {204, 182, 191, 255},
    {204, 182, 255, 255},
    {204, 218, 0, 255},
    {204, 218, 63, 255},
    {204, 218, 127, 255},
    {204, 218, 191, 255},
    {204, 218, 255, 255},
    {204, 255, 0, 255},
    {204, 255, 63, 255},
    {204, 255, 127, 255},
    {204, 255, 191, 255},
    {204, 255, 255, 255},
    {255, 0, 0, 255},
    {255, 0, 63, 255},
    {255, 0, 127, 255},
    {255, 0, 191, 255},
    {255, 0, 255, 255},
    {255, 36, 0, 255},
    {255, 36, 63, 255},
    {255, 36, 127, 255},
    {255, 36, 191, 255},
    {255, 36, 255, 255},
    {255, 72, 0, 255},
    {255, 72, 63, 255},
    {255, 72, 127, 255},
    {255, 72, 191, 255},
    {255, 72, 255, 255},
    {255, 109, 0, 255},
    {255, 109, 63, 255},
    {255, 109, 127, 255},
    {255, 109, 191, 255},
    {255, 109, 255, 255},
    {255, 145, 0, 255},
    {255, 145, 63, 255},
    {255, 145, 127, 255},
    {255, 145, 191, 255},
    {255, 145, 255, 255},
    {255, 182, 0, 255},
    {255, 182, 63, 255},
    {255, 182, 127, 255},
    {255, 182, 191, 255},
    {255, 182, 255, 255},
    {255, 218, 0, 255},
    {255, 218, 63, 255},
    {255, 218, 127, 255},
    {255, 218, 191, 255},
    {255, 218, 255, 255},
    {255, 255, 0, 255},
    {255, 255, 63, 255},
    {255, 255, 127, 255},
    {255, 255, 191, 255},
    {255, 255, 255, 255},
    {36, 36, 36, 255},
    {72, 72, 72, 255},
    {109, 109, 109, 255},
    {145, 145, 145, 255},
    {182, 182, 182, 255},
    {218, 218, 218, 255},
    {255, 255, 255, 255},
    {0, 0, 0, 255},
    {0, 0, 0, 255},
    {0, 0, 0, 255},
    {0, 0, 0, 255},
    {0, 0, 0, 255},
    {0, 0, 0, 255},
    {0, 0, 0, 255},
    {0, 0, 0, 255},
    {0, 0, 0, 255},
};
ColorPalette kDefault256RgbPalette = {
  .size = 256,
  .fillsize = 256,
  .data = default256RgbColors
};

