// dont forget to define it BEFORE play()
// this work better with teensy 3.5 - 3.6.
#define B2M (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE / 2.0) // 97352592
// On older teensys use :
//#define B2M (uint32_t)((double)4294967296000.0 / AUDIO_SAMPLE_RATE_EXACT / 2.0) // 97352592


bool AudioPlaySdRaw::play(const char *filename, int32_t startPosMs ) // We pass the parameter "startPosMs" that will be used. dont forget to update the .h file too
{
	stop();
#if defined(HAS_KINETIS_SDHC)
	if (!(SIM_SCGC3 & SIM_SCGC3_SDHC)) AudioStartUsingSPI();
#else
	AudioStartUsingSPI();
#endif
	__disable_irq();
	rawfile = SD.open(filename);
	__enable_irq();
	if (!rawfile) {
		//Serial.println("unable to open file");
		#if defined(HAS_KINETIS_SDHC)
			if (!(SIM_SCGC3 & SIM_SCGC3_SDHC)) AudioStopUsingSPI();
		#else
			AudioStopUsingSPI();
		#endif
		return false;
	}
	
	// Get the file size in bytes

	file_size = rawfile.size();

	// init the file offset to zero at start, we will modify it later

	file_offset = 0;
	
	// Here we convert the total raw byte size to Milliseconds, assuming your sample rate is 44100khz
	// Now you get the total duration in MS

	uint64_t duration = ((uint64_t)file_size * B2M) >> 32;

	// Because of the byte nature of things, we need to seek the file in discrete increments. If not (arbitrary value) you will hear terrible distortion.
	// Here, the slice padding is 2 blocks, we calculate the number of blocks in the entire file.

	uint32_t dataSlicedBlocksNb = dataSize / AUDIO_BLOCK_SAMPLES * 2; // ---------------> You can also write : datasize >> 8;
	
	// Be carrefull of the limits! Here I just add 20ms padding on maximum, to prevent unpredicable results.

	if (startPosMs < duration - 20) {

		// Now map the range to adapt MS values to a block safe environement

  		file_offset = startPosMs * dataSlicedBlocksNb / duration; 

  		// Then run block by 2 blocks
  		
  		file_offset *= AUDIO_BLOCK_SAMPLES * 2; 

		// finally, seek the data

  		rawfile.seek(file_offset);

	} 

	playing = true;
	return true;
}