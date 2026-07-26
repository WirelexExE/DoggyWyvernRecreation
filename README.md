# Doggy Wyvern Recreation
A recreation of Pixel's unreleased game

![Screenshot](screenshot.png)

## For Building
```
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded
```

## Progress
- [ ] Export all stages as files (currently hardcoded)
- [ ] Stage 1 Intro Sequence
- [x] Stage 1 Layout (missing only a few things)
- [ ] Stage 1 Sequence
- [ ] Stage 2 Layout
- [ ] Stage 2 Sequence
- [ ] Stage 2 Boss (Graphics/Function)
- [ ] Rest of the sounds
- [ ] Fix the flash during the "Clear!" screen
- [ ] Fix Doggy Ears/Tail
- [ ] Fix "Stage X Start!" animation
- [ ] Rewriting the framework to use legacy directdraw code using the 8-bit palette

## Credits
* Pixel and other colleagues (the original game + assets)
* CSE2/Ikachan/Piyopiyo Decomp (snippits from the original framework)
* AsperD (graphic recreations)
* motorola68000 (graphic recreations)
* RootLM (upcoming Stage 1 theme)
* SSNTails (contributed the Stage 2 theme)
* (others that i've probably forgot to add here, lemme know asap)

## Licensing

Few snippits of CSE2/Ikachan/Piyopiyo decomp code and the assets in this project are proprietary
and belongs to Daisuke "Pixel" Amaya.