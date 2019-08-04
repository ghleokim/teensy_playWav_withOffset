# Teensy wav player modified
These code is originally from Teensy Audio Library(https://github.com/PaulStoffregen/Audio).
The purpose of this modification is to play `.wav` starting from the wanted offset(i.e from the 0:30 to the end).

## Done:
- 04Aug2019
    - ~~The offset should be in `bytes`. This will be changed into `milliseconds`.~~
    - ~~`positionMillis()` and `lengthMillis()` does not work well. Should be fixed.~~

## How to add
### **for mac**: 

1. Go to `Applications/Arduino/Contents/Java/hardware/teensy/avr/libraries/Audio`.

2. Replace `Audio.h`, `play_sd_wav.h` and `play_sd_wav.cpp` with given files.

    (Don't forget to backup. Just in case there're original files included in this repository.)
    
    2-1. If path is not found, go to the path where the Arduino Application is. Then if you do `Show Package Contents` the path will be there.

3. In the .ino file, the function can be used as it was, but also millisecond input can be used as well.(teensy example/WavFilePlayer is the reference.)

    ```arduino
    //both available
    playWav1.play('TEST.WAV')
    playWav1.play('TEST.WAV', 60000)
    ```

### **for windows**:

TBA


## New Variables / Functions
### `bool didSkip`

this checks if the file should be skipped or not(if offset is 0 or not)
    
### `uint32_t play_offset`

this is converted into file_offset with bytes.

this is used to seek wavfile as bytes(if not converted).

### `uint32_t file_offset`

this is used to seek wavfile when millis is converted into bytes.


## Modified Variables / Functions
### `bool AudioPlaySdWav::play(const char *filename, uint32_t offset = 0)`

    ```arduino
    bool AudioPlaySdWav::play(const char *filename, uint32_t offset)
    {
        ...
        header_offset = 0;
        //could use play_offset as identifier but used bool to be clear
        if (offset == 0) didSkip = true;
        else didSkip = false;
        play_offset = offset;
        state = STATE_PARSE1;
        return true;
    }
    ```

- added `uint32_t offset` to get the offset input.
- `bool didSkip` shows whether the file should be skipped or not.

---

### `void AudioPlaySdWav::consume(void)`

    ```arduino
    bool AudioPlaySdWav::consume(uint32_t size)
    {
        uint32_t len;
        uint8_t lsb, msb;
        const uint8_t *p;

        p = buffer + buffer_offset;
    start:
        if (size == 0) return false;

    ...

        // find the data chunk
        case STATE_PARSE3: // 10
            ...
            if (header[0] == 0x61746164) {
                //Serial.print("wav: found data chunk, len=");
                //Serial.println(data_length);
                // TODO: verify offset in file is an even number
                // as required by WAV format.  abort if odd.  Code
                // below will depend upon this and fail if not even.
                
                if (didSkip == false) {
                    //skip by using millis input
                    file_offset = offsetMillis2byte(play_offset);
                    wavfile.seek(file_offset);
                    //skip by using bytes input
                    //to skip by using bytes input, uncomment below
                    //wavfile.seek(play_offset);
                    data_length = 8;
                    header_offset = 8;
                    state = STATE_PARSE3;
                    didSkip = true;
                    goto start;
                }

                leftover_bytes = 0;
                state = state_play;
                if (state & 1) {
                    // if we're going to start stereo
                    // better allocate another output block
                    block_right = allocate();
                    if (!block_right) return false;
                }
                total_length = data_length;
            } else {
                state = STATE_PARSE4;
            }
            goto start;

    ...
    }
    ```

- added a step to fetch data chunk from the wanted offset in STATE_PARSE3. This step will only be done once if `offset` is not 0.

---
	
### `uint32_t AudioPlaySdWav::offsetMillis2byte(uint32_t offset_length)`

    ```arduino
    uint32_t AudioPlaySdWav::offsetMillis2byte(uint32_t offset_length)
    {
        // convert millis input to bytes with b2m and AUDIO_BLOCK_SAMPLES.
        // to avoid wrong offset length(odd number),
        // offset_length is scaled down by AUDIO_BLOCK_SAMPLES, converted, and scaled up again.
        uint32_t b2m = *(volatile uint32_t *)&bytes2millis;
        uint32_t offset_bytes = (uint32_t)(double)(offset_length * 4294967296.0 / (AUDIO_BLOCK_SAMPLES * b2m));
        offset_bytes *= AUDIO_BLOCK_SAMPLES;
        return offset_bytes;
    }
    ```
- added a function to convert millisecond input to byte offset.

---

modified and written by Leo Kim(https://www.github.com/gh.leokim)