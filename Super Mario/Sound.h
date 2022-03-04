#pragma once

typedef struct Sound
{
	IMFAsyncCallback AsyncCallback;
	IMFAsyncCallbackVtbl AsyncCallbackVtbl;
	IMFMediaSession *session;
	IMFMediaSource *source;
	bool playing;
}Sound;

typedef enum SoundNumber
{
	mushroommonstertrampled,
	mushroomappeared,
	touchedbrick,
	breakedbrick,
	eatcoin,
	eatmushroom,
	killedbybullet,
	mariojump,
	lowerflag,
	shot,
	mariosmaller,
	killedbymonster,
	backgroundmusic,
	gameover
}SoundNumber;

Sound* SoundConstructor();
void SoundDestrutor(Sound *sound);