#include "ScreenUtils.h"

#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library

#define BUFFPIXEL 64

ScreenUtils::ScreenUtils(Adafruit_ILI9341* adafruitTft) {
	tft = adafruitTft;
}

void ScreenUtils::drawBatteryStateOfCharge(float soc)
{
	tft->setFont(NULL);

	uint16_t drawColor;
	if (101 <= soc) {
		drawColor = ILI9341_PINK;
	} else if (75 <= soc && soc < 101) {
		drawColor = ILI9341_DARKGREEN;
	} else if (50 <= soc && soc < 75) {
		drawColor = ILI9341_GREEN;
	} else if (25 <= soc && soc < 50) {
		drawColor = ILI9341_YELLOW;
	} else if (soc < 25) {
		drawColor = ILI9341_RED;
	}

	tft->fillRect(194, 298, 6, 9, drawColor);
	tft->drawRect(200, 295, 30, 15, drawColor);
	tft->drawRect(199, 294, 32, 17, drawColor);

	tft->setTextSize(1);
	tft->setTextWrap(false);
	tft->setTextColor(drawColor, ILI9341_BLACK);
	tft->setCursor(207, 299);
	if (100 < soc) {
		tft->print(F("OOO"));
	} else {
		tft->print(soc, 0);
		tft->print(F("%"));
	}
}

void ScreenUtils::drawButtonFrame(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, bool selected) {
	uint16_t drawColor;
	selected ? drawColor = ILI9341_YELLOW : drawColor = ILI9341_LIGHTGREY;

	tft->fillRect(x, y, w, r, drawColor);     //Top
	tft->fillRect(x, y, r, h, drawColor);     //Left
	tft->fillRect(x + w - r, y, r, h, drawColor); //Right
	tft->fillRect(x, y + h - r, w, r, drawColor); //Bottom
}

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.
void ScreenUtils::drawBmpFile(char *filename, int16_t x, int16_t y) {

	SdFile bmpFile;
	int bmpWidth, bmpHeight;   // W+H in pixels
	uint8_t bmpDepth;              // Bit depth (currently must be 24)
	uint32_t bmpImageoffset;        // Start of image data in file
	uint32_t rowSize;               // Not always = bmpWidth; may have padding
	uint8_t sdbuffer[3 * BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
	uint8_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
	boolean goodBmp = false;       // Set to true on valid header parse
	boolean flip = true;        // BMP is stored bottom-to-top
	int w, h, row, col, x2, y2, bx1, by1;
	uint8_t r, g, b;
	uint32_t pos = 0, startTime = millis();

	if ((x >= tft->width()) || (y >= tft->height()))
		return;

	Serial.println();
	Serial.print(F("Loading image '"));
	Serial.print(filename);
	Serial.println('\'');

	// Open requested file on SD card

	if (!bmpFile.open(filename, O_READ)) {
		Serial.print(F("File not found"));
		return;
	}

	// Parse BMP header
	if (read16(bmpFile) == 0x4D42) { // BMP signature
		Serial.print(F("File size: "));
		Serial.println(read32(bmpFile));
		(void) read32(bmpFile); // Read & ignore creator bytes
		bmpImageoffset = read32(bmpFile); // Start of image data
		Serial.print(F("Image Offset: "));
		Serial.println(bmpImageoffset, DEC);
		// Read DIB header
		Serial.print(F("Header size: "));
		Serial.println(read32(bmpFile));
		bmpWidth = read32(bmpFile);
		bmpHeight = read32(bmpFile);
		if (read16(bmpFile) == 1) { // # planes -- must be '1'
			bmpDepth = read16(bmpFile); // bits per pixel
			Serial.print(F("Bit Depth: "));
			Serial.println(bmpDepth);
			if ((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

				goodBmp = true; // Supported BMP format -- proceed!
				Serial.print(F("Image size: "));
				Serial.print(bmpWidth);
				Serial.print('x');
				Serial.println(bmpHeight);

				// BMP rows are padded (if needed) to 4-byte boundary
				rowSize = (bmpWidth * 3 + 3) & ~3;

				// If bmpHeight is negative, image is in top-down order.
				// This is not canon but has been observed in the wild.
				if (bmpHeight < 0) {
					bmpHeight = -bmpHeight;
					flip = false;
				}

				// Crop area to be loaded
				x2 = x + bmpWidth - 1; // Lower-right corner
				y2 = y + bmpHeight - 1;
				if ((x2 >= 0) && (y2 >= 0)) { // On screen?
					w = bmpWidth; // Width/height of section to load/display
					h = bmpHeight;
					bx1 = by1 = 0; // UL coordinate in BMP file
					if (x < 0) { // Clip left
						bx1 = -x;
						x = 0;
						w = x2 + 1;
					}
					if (y < 0) { // Clip top
						by1 = -y;
						y = 0;
						h = y2 + 1;
					}
					if (x2 >= tft->width())
						w = tft->width() - x; // Clip right
					if (y2 >= tft->height())
						h = tft->height() - y; // Clip bottom

					// Set TFT address window to clipped image bounds
					tft->startWrite(); // Requires start/end transaction now
					tft->setAddrWindow(x, y, w, h);

					for (row = 0; row < h; row++) { // For each scanline...

						// Seek to start of scan line.  It might seem labor-
						// intensive to be doing this on every line, but this
						// method covers a lot of gritty details like cropping
						// and scanline padding.  Also, the seek only takes
						// place if the file position actually needs to change
						// (avoids a lot of cluster math in SD library).
						if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
							pos = bmpImageoffset
									+ (bmpHeight - 1 - (row + by1)) * rowSize;
						else
							// Bitmap is stored top-to-bottom
							pos = bmpImageoffset + (row + by1) * rowSize;
						pos += bx1 * 3; // Factor in starting column (bx1)
						if (bmpFile.curPosition() != pos) { // Need seek?
							tft->endWrite(); // End TFT transaction
							bmpFile.seekSet(pos);
							buffidx = sizeof(sdbuffer); // Force buffer reload
							tft->startWrite(); // Start new TFT transaction
						}
						for (col = 0; col < w; col++) { // For each pixel...
							// Time to read more pixel data?
							if (buffidx >= sizeof(sdbuffer)) { // Indeed
								tft->endWrite(); // End TFT transaction
								bmpFile.read(sdbuffer, sizeof(sdbuffer));
								buffidx = 0; // Set index to beginning
								tft->startWrite(); // Start new TFT transaction
							}
							// Convert pixel from BMP to TFT format, push to display
							b = sdbuffer[buffidx++];
							g = sdbuffer[buffidx++];
							r = sdbuffer[buffidx++];
							tft->writePixel(tft->color565(r, g, b));
						} // end pixel
					} // end scanline
					tft->endWrite(); // End last TFT transaction
				} // end onscreen
				Serial.print(F("Loaded in "));
				Serial.print(millis() - startTime);
				Serial.println(" ms");
			} // end goodBmp
		}
	}

	bmpFile.close();
	if (!goodBmp)
		Serial.println(F("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t ScreenUtils::read16(SdFile &f) {
	uint16_t result;
	((uint8_t *) &result)[0] = f.read(); // LSB
	((uint8_t *) &result)[1] = f.read(); // MSB
	return result;
}

uint32_t ScreenUtils::read32(SdFile &f) {
	uint32_t result;
	((uint8_t *) &result)[0] = f.read(); // LSB
	((uint8_t *) &result)[1] = f.read();
	((uint8_t *) &result)[2] = f.read();
	((uint8_t *) &result)[3] = f.read(); // MSB
	return result;
}




