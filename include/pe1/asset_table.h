#ifndef PE1_ASSET_TABLE_H
#define PE1_ASSET_TABLE_H

/* Finds the 8-byte record with a 32-bit key in an asset table. */
void *Asset_FindTable08ByU32Key(void *base, int key);

#endif
