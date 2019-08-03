# Teensy wav player modified
These code is originally from Teensy Audio Library(https://github.com/PaulStoffregen/Audio).
The purpose of this modification is to play `.wav` starting from the wanted offset(i.e from the 0:30 to the end).

## To do:
- The offset should be in `bytes`. This will be changed into `milliseconds`. 
- `positionMillis()` and `lengthMillis()` does not work well. Should be fixed.

## How to add
**for mac**: 

1. Go to `Applications/Arduino/Contents/Java/hardware/teensy/avr/libraries/Audio`.

2. Replace `play_sd_wav.h` and `play_sd_wav.cpp` with given files.

    (Don't forget to backup. Just in case there're original files included in this repository.)
    
    2-1. If path is not found, go to the path where the Arduino Application is. Then if you do `Show Package Contents` the path will be there.

3. In the .ino file, be sure to use the offset like below:

    ```arduino
    playWav1.play('TEST.WAV', 100000)
    ```

**for windows**:

TBA


## New Variables / Functions
`bool didSkip`

this checks if the file should be skipped or not(if offset is 0 or not)
    
`uint32_t play_offset`

currently this is used to seek wavfile as bytes.

will be changed to get millis input and convert into bytes.

(not used)`uint64_t file_offset`

this will be used to seek wavfile when millis is converted into bytes.


## Modified Variables / Functions
`bool AudioPlaySdWav::play(const char *filename, uint32_t offset)`

    ```arduino
    bool AudioPlaySdWav::play(const char *filename, uint32_t offset)
    {
        ...
        header_offset = 0;
        //
        didSkip = false;
        if (offset == 0) {
            didSkip = true;
        }
        else {
            didSkip = false;
        }
        play_offset = offset;
        //
        state = STATE_PARSE1;
        return true;
    }
    ```

- added `uint32_t offset` to get the offset input.
- `bool didSkip` shows whether the file should be skipped or not.

---

`void AudioPlaySdWav::consume(void)`

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
                    wavfile.seek(play_offset);
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
modified and written by Leo Kim(https://www.github.com/gh.leokim)