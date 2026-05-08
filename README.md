# audio_cleaner
 
  A command-line WAV audio cleaner written in C.
  Reads a WAV file, filters out-of-band frequencies via FFT,
  normalises volume, and writes a clean WAV file.
 
  ## Requirements
  - macOS with Xcode Command Line Tools
    xcode-select --install
 
  ## Build
  cc audio_cleaner.c -o audio_cleaner -lm
 
  ## Run
  ./audio_cleaner input.wav output.wav
 
  ## Output
  Writes cleaned audio to output.wav.
  Prints a scan report to stdout.
 
  ## Current Status
  Proof-of-concept. WAV write, peak detection, and
  CLI validation are complete. FFT pipeline is planned.
 
  ## Project Structure
  audio_cleaner.c   — main source
  test/             — unit tests
  README.md         — this file

