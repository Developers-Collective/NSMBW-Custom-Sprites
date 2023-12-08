const char* SFXNameList [] = {
	"original",				// 1999, DON'T USE THIS ONE
	// add more sfx here	// 2000
	NULL
};

int currentSFX = -1;
u32 *currentPtr = 0;

extern void loadFileAtIndex(u32 *filePtr, u32 fileLength, u32* whereToPatch);

// static FileHandle handle;
extern u32* GetCurrentPC();


extern "C" u32 NewSFXTable[];
extern "C" u32 NewSFXIndexes;

void loadAllSFXs() {
	u32 currentIdx = (u32)&NewSFXIndexes;

	for(int sfxIndex = 0; sfxIndex < (sizeof(SFXNameList) - 1) / sizeof(SFXNameList[0]); sfxIndex++) {
		FileHandle handle;

		// OSReport("SFX > Loading %s...\n", SFXNameList[sfxIndex]);
		char nameWithSound[80] = "";
		snprintf(nameWithSound, 79, "/Sound/new/sfx/%s.rwav", SFXNameList[sfxIndex]);

		u32 filePtr = (u32)LoadFile(&handle, nameWithSound);

		NewSFXTable[sfxIndex] = currentIdx;
		loadFileAtIndex((u32*)filePtr, handle.length, (u32*)currentIdx);
		currentIdx += handle.length;
		currentIdx += (currentIdx % 0x10);
		FreeFile(&handle);
	}
}

int hijackSFX(int SFXNum) {
	int nameIndex = SFXNum - 1999;
	if(currentSFX == nameIndex) {
		return 189;
	}

	currentPtr = (u32*)NewSFXTable[nameIndex];
	currentSFX = nameIndex;

	return 189;
}

static nw4r::snd::StrmSoundHandle yoshiHandle;

void fuckingYoshiStuff() {
	PlaySoundWithFunctionB4(SoundRelatedClass, &yoshiHandle, 189, 1);
}
