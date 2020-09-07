## Graphic bitmaps

Bitmaps used to create the image arrays in bitmaps.h

An image is selected to show on the startup welcome message. Set the array variable name in this line in setup:

`display.drawBitmap(0, 30, GPO_logo, 104, 76, GxEPD_BLACK);  // Set the array name to the preferred bitmap image`

See the comments in bitmaps.h for help on using *Image2LCD* to convert the bitmaps to arrays.

**TRADEMARKS**

Some logos may, or may not, have trademark protection. Delete the relevant array if creating a fork and unsure about protection. Note the Bell logo has probably been 'out-of-commerce' (US) since 1969 (although the definition is very loose). The GPO logo is not a true copy and has also been 'out of commerce' (UK) since 1969.