#ifndef _SOUND_MANAGER_H_
#define _SOUND_MANAGER_H_

#include "LoadDataXML.h"
#include "cocos2d.h"

#ifndef USE_NEW_SOUND
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif // USE_NEW_SOUND;

#include "RKThread_Code/RKThread.h"
#include "RKThread_Code/RKInterlock.h"
using namespace cocos2d;
namespace Utility
{
	namespace Sound
	{
		struct SoundDes
		{
			RKString ID_SOUND;
			float delayTime;
			int loop;
		public:
			SoundDes()
			{
				ID_SOUND = "";
				delayTime = 0.1f;
				loop = 1;
			}
			SoundDes(RKString id, float DelayTime, int pLoop)
			{
				ID_SOUND = id;
				delayTime = DelayTime;
				loop = pLoop;
			}
		};
		class SoundManager
		{
		private:
			bool p_IsInitialized;

			static SoundManager * p_Instance;
			std::string p_currentMusicPath;
			//thread update back ground
#ifdef USE_AUDIO_ON_THREAD
			RKUtils::RKCriticalSection*   m_criticalSection;

			RKUtils::RKThread *			  p_ThreadUpdatePushSound;
			RKUtils::RKThread *			  p_ThreadUpdatePushSoundTemp;

			RKUtils::RKInterlock*         m_exit;
			RKUtils::RKThreadCondition*   m_condition;

			RKUtils::RKInterlock*         m_exit_temp;
			RKUtils::RKThreadCondition*   m_condition_temp;


			bool p_IsOnUpdateThread;
			bool p_IsStartThread;

			std::vector<SoundDes*> p_list_id_sound_push;
			std::vector<SoundDes*> p_list_id_sound_push_temp;

			static uint32 ThreadUpdatePushSound(void* pThreadData);
			static uint32 ThreadUpdatePushToTemp(void* pThreadData);

			void ThreadPushSound();
			void ThreadPushSoundTemp();

			void ReleaseSoundThread();
#else
			unsigned int p_currentIdsoundfx;
#endif

#ifdef USE_NEW_SOUND
			int _current_audio_id;
			int _current_music_id;
			int _on_playing_music_id;

			float _current_duration_audio;
			float _current_duration_music;

			float _volume_sound;
			float _volume_music;
#endif
		public:
			static SoundManager * GetInstance()
			{
				if (!p_Instance)
				{
					p_Instance = new SoundManager();
				}
				return p_Instance;
			}


			SoundManager();
			virtual ~SoundManager();
			void InitSoundEngine();
			//
			//for the system interrupt
			//
			void ResumeSoundEngine();
			void PauseSoundEngine();
			//
			void PlaySoundEffect(RKString ID_SOUND, float delay = 0, int loop = 0,bool imediately = false);
			void PlayMusicEffect(RKString ID_MUSIC, bool loop = true, bool use_in_thread = true);
			//
			void PlayCurrentMusic();
			bool IsMusicPlaying();
			//
			void PauseTheCurrentSound();
			void StopTheCurrentSound();
			void ResumeTheCurrentSound();

			void PauseTheCurrentMusic();
			void StopTheCurrentMusic();
			void ResumeTheCurrentMusic();		
			//
			void MuteVolume(bool sound = true);
			void ResetVolume(bool sound = true);
			void SetVolume(float percent , bool sound = true);
			float GetVolume(bool sound = true);
#ifdef USE_AUDIO_ON_THREAD
			//
			void OnPushFromTempToMain();
			void OnStartThreadPushSound() 
			{ 
				p_IsStartThread = true; 
				RKThreadCondition_WakeAll(m_condition_temp);
			}

#endif
			bool IsInit() { return p_IsInitialized; }
		};
	}
	#define GetSound Sound::SoundManager::GetInstance()
}
#ifndef USE_NEW_SOUND
#define GetSimpleSound CocosDenshion::SimpleAudioEngine::getInstance() 
#endif

#endif//_SOUND_MANAGER_H_