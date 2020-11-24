#ifndef LICENSETYPE_H
#define LICENSETYPE_H

enum LicenseType
{
    LicenseType_Invalid,
    LicenseType_SpriteMill,
    LicenseType_SquiggleMarkPhoto,
    LicenseType_SquiggleMarkPhotoVideo,
    LicenseType_Max
};
const char LICENSE_TYPE[LicenseType_Max][64] = {
    "invalid",
    "oosman\\'s iphone asset generator",
    "lele\\'s squigglemark app",
    "lele\\'s squigglemark for photo and video"
};


#endif // LICENSETYPE_H
