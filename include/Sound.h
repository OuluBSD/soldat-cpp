#ifndef SOUND_H
#define SOUND_H

//*******************************************************************************
//                                                                              
//       Sound Unit for SOLDAT                                                     
//                                                                              
//       Copyright (c) 2002-2003 Michal Marcinkowski       
//                                                                              
//*******************************************************************************

#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <cmath>
#include <AL/al.h>
#include <AL/alc.h>
#include <SDL2/SDL.h>
#include <PhysFS.h>
#include "Vector.h"

// Type definitions
struct SoundSample {
    bool Loaded;
    ALuint Buffer;
    
    SoundSample() : Loaded(false), Buffer(0) {}
};

struct TScriptSound {
    std::string Name;
    SoundSample Samp;
};

// Constants
const int MAX_SOURCES = 256;
const int RESERVED_SOURCES = 128;
const int MAX_SAMPLES = 163;
const int CHANNEL_WEATHER = 127;

// Function declarations
bool InitSound();
short SoundNameToID(const std::string& Name);
SoundSample LoadSample(const char* Name, SoundSample samp);
float ScaleVolumeSetting(uint8_t VolumeSetting);
void LoadSounds(const std::string& ModDir);
void CloseSound();
void PlaySound(int SampleNum, float ListenerX, float ListenerY, float EmitterX, 
               float EmitterY, int Chan); // FPlaySound in original
void PlaySound(int Sample); // overload
void PlaySound(int Sample, int Channel); // overload
void PlaySound(int Sample, const TVector2& Emitter); // overload
void PlaySound(int Sample, const TVector2& Emitter, int& Channel); // overload
bool StopSound(int Channel);
bool SetSoundPaused(int Channel, bool Paused);
bool SetVolume(int Channel, float Volume);

#ifdef STEAM_CODE
void PlayVoiceData(void* Data, uint16_t DataLength, uint8_t SpriteNum);
#endif

// Global variables
extern SoundSample Samp[MAX_SAMPLES + 1];  // Pascal arrays start from 1
extern std::vector<TScriptSound> ScriptSamp;
extern float VolumeInternal;
extern ALCdevice* ALDevice;
extern ALCcontext* ALContext;
extern ALuint Sources[MAX_SOURCES];
extern int DefaultChannel;

namespace SoundImpl {
    inline bool InitSound() {
        ALDevice = alcOpenDevice(nullptr);
        if (!ALDevice) {
            return false;
        }
        
        ALContext = alcCreateContext(ALDevice, nullptr);
        if (!ALContext) {
            alcCloseDevice(ALDevice);
            return false;
        }
        
        if (!alcMakeContextCurrent(ALContext)) {
            alcDestroyContext(ALContext);
            alcCloseDevice(ALDevice);
            return false;
        }

        alDistanceModel(AL_NONE);

        alGenSources(MAX_SOURCES, Sources);
        if (alGetError() != AL_NO_ERROR) {
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(ALContext);
            alcCloseDevice(ALDevice);
            return false;
        }

        for (int i = 1; i <= MAX_SAMPLES; i++) {
            Samp[i].Loaded = false;
        }

        return true;
    }

    inline SoundSample LoadSample(const char* Name, SoundSample samp) {
        SoundSample result = samp;  // Default to the input sample
        
        if (!samp.Loaded) {
            result.Loaded = false;
            return result;
        }

        // Read file into memory using PhysFS
        PHYSFS_File* file = PHYSFS_openRead(Name);
        if (!file) {
            // File doesn't exist
            return result;
        }

        PHYSFS_uint64 fileLength = PHYSFS_fileLength(file);
        std::vector<char> fileBuffer(fileLength);
        PHYSFS_readBytes(file, fileBuffer.data(), fileLength);
        PHYSFS_close(file);

        // Create SDL_RWops from memory buffer
        SDL_RWops* rwOps = SDL_RWFromConstMem(fileBuffer.data(), static_cast<int>(fileLength));
        if (!rwOps) {
            return result;
        }

        SDL_AudioSpec spec;
        Uint8* audioData = nullptr;
        Uint32 audioLength;
        
        SDL_AudioSpec* wavSpec = SDL_LoadWAV_RW(rwOps, 1, &spec, &audioData, &audioLength);
        if (!wavSpec) {
            SDL_RWclose(rwOps);
            return result;
        }

        // Determine OpenAL format based on SDL audio format
        ALenum format = AL_FORMAT_MONO8;
        bool badFormat = false;
        
        if (spec.channels == 1) {
            switch (spec.format) {
                case AUDIO_U8:
                    format = AL_FORMAT_MONO8;
                    break;
                case AUDIO_S16LSB:
                case AUDIO_S16MSB:
                    format = AL_FORMAT_MONO16;
                    break;
                case AUDIO_F32LSB:
                case AUDIO_F32MSB:
                    format = AL_FORMAT_MONO_FLOAT32;
                    break;
                default:
                    badFormat = true;
                    break;
            }
        } else if (spec.channels == 2) {
            switch (spec.format) {
                case AUDIO_U8:
                    format = AL_FORMAT_STEREO8;
                    break;
                case AUDIO_S16LSB:
                case AUDIO_S16MSB:
                    format = AL_FORMAT_STEREO16;
                    break;
                case AUDIO_F32LSB:
                case AUDIO_F32MSB:
                    format = AL_FORMAT_STEREO_FLOAT32;
                    break;
                default:
                    badFormat = true;
                    break;
            }
        } else {
            badFormat = true;
        }

        if (!badFormat) {
            if (samp.Loaded) {
                alDeleteBuffers(1, &samp.Buffer);
            }
            
            ALuint buffer;
            alGenBuffers(1, &buffer);
            alBufferData(buffer, format, audioData, audioLength, spec.freq);
            
            result.Buffer = buffer;
            result.Loaded = true;
        }

        SDL_FreeWAV(audioData);
        SDL_RWclose(rwOps);
        
        return result;
    }

    inline short SoundNameToID(const std::string& Name) {
        short result = -1;
        if (ScriptSamp.empty()) {
            return result;
        }
        
        std::string upperName = Name;
        std::transform(upperName.begin(), upperName.end(), upperName.begin(), ::toupper);
        
        for (size_t i = 0; i < ScriptSamp.size(); i++) {
            std::string upperScriptName = ScriptSamp[i].Name;
            std::transform(upperScriptName.begin(), upperScriptName.end(), upperScriptName.begin(), ::toupper);
            
            if (upperScriptName == upperName) {
                result = static_cast<short>(i);
                break;
            }
        }
        
        return result;
    }

    inline float ScaleVolumeSetting(uint8_t VolumeSetting) {
        // Volume scaling formula from original
        return (std::pow(1.0404f, VolumeSetting) - 1.0f) / (1.0404f - 1.0f) / 1275.0f;
    }

    inline void LoadSounds(const std::string& ModDir) {
        const std::vector<std::string> SAMPLE_FILES = {
            "",  // Placeholder for index 0
            "ak74-fire.wav",
            "rocketz.wav",
            "ak74-reload.wav",
            "",  // empty.wav - no longer used
            "m249-fire.wav",
            "ruger77-fire.wav",
            "ruger77-reload.wav",
            "m249-reload.wav",
            "mp5-fire.wav",
            "mp5-reload.wav",
            "spas12-fire.wav",
            "spas12-reload.wav",
            "standup.wav",
            "fall.wav",
            "spawn.wav",
            "m79-fire.wav",
            "m79-explosion.wav",
            "m79-reload.wav",
            "grenade-throw.wav",
            "grenade-explosion.wav",
            "grenade-bounce.wav",
            "bryzg.wav",
            "infiltmus.wav",
            "headchop.wav",
            "explosion-erg.wav",
            "water-step.wav",
            "bulletby.wav",
            "bodyfall.wav",
            "deserteagle-fire.wav",
            "deserteagle-reload.wav",
            "steyraug-fire.wav",
            "steyraug-reload.wav",
            "barretm82-fire.wav",
            "barretm82-reload.wav",
            "minigun-fire.wav",
            "minigun-reload.wav",
            "minigun-start.wav",
            "minigun-end.wav",
            "pickupgun.wav",
            "capture.wav",
            "colt1911-fire.wav",
            "colt1911-reload.wav",
            "changeweapon.wav",
            "shell.wav",
            "shell2.wav",
            "dead-hit.wav",
            "throwgun.wav",
            "bow-fire.wav",
            "takebow.wav",
            "takemedikit.wav",
            "wermusic.wav",
            "ts.wav",
            "ctf.wav",
            "berserker.wav",
            "godflame.wav",
            "flamer.wav",
            "predator.wav",
            "killberserk.wav",
            "vesthit.wav",
            "burn.wav",
            "vesttake.wav",
            "clustergrenade.wav",
            "cluster-explosion.wav",
            "grenade-pullout.wav",
            "spit.wav",
            "stuff.wav",
            "smoke.wav",
            "match.wav",
            "roar.wav",
            "step.wav",
            "step2.wav",
            "step3.wav",
            "step4.wav",
            "hum.wav",
            "ric.wav",
            "ric2.wav",
            "ric3.wav",
            "ric4.wav",
            "dist-m79.wav",
            "dist-grenade.wav",
            "dist-gun1.wav",
            "dist-gun2.wav",
            "dist-gun3.wav",
            "dist-gun4.wav",
            "death.wav",
            "death2.wav",
            "death3.wav",
            "crouch-move.wav",
            "hit-arg.wav",
            "hit-arg2.wav",
            "hit-arg3.wav",
            "goprone.wav",
            "roll.wav",
            "fall-hard.wav",
            "onfire.wav",
            "firecrack.wav",
            "scope.wav",
            "scopeback.wav",
            "playerdeath.wav",
            "changespin.wav",
            "arg.wav",
            "lava.wav",
            "regenerate.wav",
            "prone-move.wav",
            "jump.wav",
            "crouch.wav",
            "crouch-movel.wav",
            "step5.wav",
            "step6.wav",
            "step7.wav",
            "step8.wav",
            "stop.wav",
            "bulletby2.wav",
            "bulletby3.wav",
            "bulletby4.wav",
            "bulletby5.wav",
            "weaponhit.wav",
            "clipfall.wav",
            "bonecrack.wav",
            "gaugeshell.wav",
            "colliderhit.wav",
            "kit-fall.wav",
            "kit-fall2.wav",
            "flag.wav",
            "flag2.wav",
            "takegun.wav",
            "infilt-point.wav",
            "menuclick.wav",
            "knife.wav",
            "slash.wav",
            "chainsaw-d.wav",
            "chainsaw-m.wav",
            "chainsaw-r.wav",
            "piss.wav",
            "law.wav",
            "chainsaw-o.wav",
            "m2fire.wav",
            "m2explode.wav",
            "m2overheat.wav",
            "signal.wav",
            "m2use.wav",
            "scoperun.wav",
            "mercy.wav",
            "ric5.wav",
            "ric6.wav",
            "ric7.wav",
            "law-start.wav",
            "law-end.wav",
            "boomheadshot.wav",
            "snapshot.wav",
            "radio/efcup.wav",
            "radio/efcmid.wav",
            "radio/efcdown.wav",
            "radio/ffcup.wav",
            "radio/ffcmid.wav",
            "radio/ffcdown.wav",
            "radio/esup.wav",
            "radio/esmid.wav",
            "radio/esdown.wav",
            "bounce.wav",
            "sfx_rain.wav",
            "sfx_snow.wav",
            "sfx_wind.wav"
        };

        std::string sfxPath = ModDir + "sfx/";

        // Sound effects
        // MainConsole.Console("Loading sound effects", DEBUG_MESSAGE_COLOR);

        for (size_t i = 1; i < SAMPLE_FILES.size() && i <= MAX_SAMPLES; i++) {
            if (!SAMPLE_FILES[i].empty()) {
                std::string fullPath = sfxPath + SAMPLE_FILES[i];
                // Samp[i] = LoadSample(fullPath.c_str(), Samp[i]);
                if (!Samp[i].Loaded) {
                    // MainConsole.Console("Unable to load file " + fullPath, DEBUG_MESSAGE_COLOR);
                }
            }
        }
    }

    inline void CloseSound() {
        for (int i = 1; i <= MAX_SAMPLES; i++) {
            if (Samp[i].Loaded) {
                alDeleteBuffers(1, &Samp[i].Buffer);
                Samp[i].Loaded = false;
            }
        }
        
        alDeleteSources(MAX_SOURCES, Sources);
        
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(ALContext);
        alcCloseDevice(ALDevice);
    }

    inline void FPlaySound(int SampleNum, float ListenerX, float ListenerY, float EmitterX, 
                          float EmitterY, int Chan) {
        // This function would implement the 3D positional sound functionality
        // For now I'll add a skeleton implementation
        if (SampleNum < 1 || SampleNum > MAX_SAMPLES || !Samp[SampleNum].Loaded) {
            return;
        }

        // Calculate listener position if following a sprite
        // This assumes CameraFollowSprite and Spriteparts.Pos are defined elsewhere
        // if (CameraFollowSprite > 0) {
        //     ListenerX = Spriteparts.Pos[CameraFollowSprite].x;
        //     ListenerY = Spriteparts.Pos[CameraFollowSprite].y;
        // }

        float dist = std::sqrt((EmitterX - ListenerX) * (EmitterX - ListenerX) + 
                              (EmitterY - ListenerY) * (EmitterY - ListenerY)) / SOUND_MAXDIST;

        // Process distant sounds if enabled
        // if ((dist > 0.5) && (snd_effects_battle.Value())) {
        //     // Handle distant sound effects based on the sample number
        // }

        // Adjust for grenade effects
        // if ((GrenadeEffectTimer > 0) && (SampleNum != SFX_HUM)) {
        //     dist = (dist + 10) * (GrenadeEffectTimer / 7);
        // }

        if (dist > 1.0f) {
            return; // Too far to play
        }

        // Determine if sound should loop
        ALint playMode = (SampleNum == SFX_ROCKETZ || SampleNum == SFX_CHAINSAW_R || SampleNum == SFX_FLAMER) 
                         ? AL_TRUE : AL_FALSE;

        // Handle channel assignment
        if (Chan >= RESERVED_SOURCES) {
            return;
        }

        if (Chan == -1) {
            // Find free source
            for (int i = RESERVED_SOURCES; i < MAX_SOURCES; i++) {
                ALint state;
                alGetSourcei(Sources[i], AL_SOURCE_STATE, &state);
                if (state != AL_PLAYING) {
                    Chan = i;
                    break;
                }
            }
        }

        if (Chan != -1) {
            alSourcei(Sources[Chan], AL_LOOPING, playMode);
            float volume = VolumeInternal * (1.0f - dist);
            alSourcef(Sources[Chan], AL_GAIN, volume);
            
            // Set 3D position
            alSource3f(Sources[Chan], AL_POSITION,
                      (EmitterX - ListenerX) / SOUND_METERLENGTH,
                      (EmitterY - ListenerY) / SOUND_METERLENGTH,
                      -SOUND_PANWIDTH / SOUND_METERLENGTH);
            
            ALint state;
            alGetSourcei(Sources[Chan], AL_SOURCE_STATE, &state);
            if (state == AL_PLAYING) {
                return;
            }
            if (state == AL_PAUSED) {
                alSourceStop(Sources[Chan]);
            }
            
            alSourcei(Sources[Chan], AL_BUFFER, Samp[SampleNum].Buffer);
            alSourcePlay(Sources[Chan]);
        }
    }

    inline void PlaySound(int Sample) {
        FPlaySound(Sample, CameraX, CameraY, CameraX, CameraY, DefaultChannel);
    }

    inline void PlaySound(int Sample, int Channel) {
        FPlaySound(Sample, CameraX, CameraY, CameraX, CameraY, Channel);
    }

    inline void PlaySound(int Sample, const TVector2& Emitter) {
        FPlaySound(Sample, CameraX, CameraY, Emitter.x, Emitter.y, DefaultChannel);
    }

    inline void PlaySound(int Sample, const TVector2& Emitter, int& Channel) {
        FPlaySound(Sample, CameraX, CameraY, Emitter.x, Emitter.y, Channel);
    }

    inline bool StopSound(int Channel) {
        if (Channel >= 0 && Channel < MAX_SOURCES) {
            alSourceStop(Sources[Channel]);
        }
        return false;
    }

    inline bool SetSoundPaused(int Channel, bool Paused) {
        if (Channel < 0 || Channel >= MAX_SOURCES) {
            return false;
        }
        
        ALint state;
        alGetSourcei(Sources[Channel], AL_SOURCE_STATE, &state);
        
        if ((state == AL_PLAYING) && Paused) {
            alSourcePause(Sources[Channel]);
        } else if ((state == AL_PAUSED) && !Paused) {
            alSourcePlay(Sources[Channel]);
        }

        return false;
    }

    inline bool SetVolume(int Channel, float Volume) {
        if (Channel == -1) {
            for (int i = 0; i < MAX_SOURCES; i++) {
                alSourcef(Sources[i], AL_GAIN, Volume);
            }
        } else {
            if (Channel >= 0 && Channel < MAX_SOURCES) {
                alSourcef(Sources[Channel], AL_GAIN, Volume);
            }
        }
        return true;
    }
}

// Using declarations to bring into global namespace
using SoundImpl::SoundSample;
using SoundImpl::TScriptSound;
using SoundImpl::Samp;
using SoundImpl::ScriptSamp;
using SoundImpl::VolumeInternal;
using SoundImpl::ALDevice;
using SoundImpl::ALContext;
using SoundImpl::Sources;
using SoundImpl::DefaultChannel;
using SoundImpl::InitSound;
using SoundImpl::SoundNameToID;
using SoundImpl::LoadSample;
using SoundImpl::ScaleVolumeSetting;
using SoundImpl::LoadSounds;
using SoundImpl::CloseSound;
using SoundImpl::FPlaySound;
using SoundImpl::PlaySound;
using SoundImpl::StopSound;
using SoundImpl::SetSoundPaused;
using SoundImpl::SetVolume;

// Global variable definitions
extern SoundSample Samp[MAX_SAMPLES + 1];
extern std::vector<TScriptSound> ScriptSamp;
extern float VolumeInternal = 1.0f;
extern ALCdevice* ALDevice = nullptr;
extern ALCcontext* ALContext = nullptr;
extern ALuint Sources[MAX_SOURCES];
extern int DefaultChannel = -1;

#endif // SOUND_H