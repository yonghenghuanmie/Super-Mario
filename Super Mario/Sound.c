#include <stdlib.h>
#include <stdbool.h>
#include <mfapi.h>
#include <mfidl.h>
#include "Sound.h"

#pragma comment(lib,"Mfplat.lib")
#pragma comment(lib,"Mf.lib")
#pragma comment(lib,"Mfuuid.lib")

HRESULT __stdcall AsyncCallbackQueryInterface(IMFAsyncCallback * This,REFIID riid,void **ppvObject);
ULONG __stdcall AsyncCallbackAddRef(IMFAsyncCallback * This);
ULONG __stdcall AsyncCallbackRelease(IMFAsyncCallback * This);
HRESULT __stdcall AsyncCallbackGetParameters(IMFAsyncCallback * This,DWORD *pdwFlags,DWORD *pdwQueue);
HRESULT __stdcall AsyncCallbackInvoke(IMFAsyncCallback * This,IMFAsyncResult *pAsyncResult);

Sound * SoundConstructor()
{
	bool success=false;
	Sound *sound=calloc(gameover+1,sizeof(Sound));
	if(sound)
	{
		wchar_t *URL[gameover+1]=
		{
			L"Resources\\CaiSiGuaiWu.wav",
			L"Resources\\DingChuMoGuHuoHua.wav",
			L"Resources\\DingYingZhuanKuai.wav",
			L"Resources\\DingPoZhuan.wav",
			L"Resources\\EatCoin.wav",
			L"Resources\\EatMushroomOrFlower.wav",
			L"Resources\\HuoQiuDaDaoGuaiWu.wav",
			L"Resources\\Jump.wav",
			L"Resources\\QiZiLuoXia.wav",
			L"Resources\\RengHuoQiu.wav",
			L"Resources\\SuoXiao.wav",
			L"Resources\\YuDaoGuaiWuSi.wav",
			L"Resources\\OnLand.wav",
			L"Resources\\GameOver.wav"
		};
		for(int i=0;i<gameover+1;i++)
		{
			sound[i].playing=false;
			IMFTopology *Topology;
			if(SUCCEEDED(MFCreateMediaSession(NULL,&sound[i].session))&&SUCCEEDED(MFCreateTopology(&Topology)))
			{
				sound[i].AsyncCallbackVtbl.QueryInterface=AsyncCallbackQueryInterface;
				sound[i].AsyncCallbackVtbl.AddRef=AsyncCallbackAddRef;
				sound[i].AsyncCallbackVtbl.Release=AsyncCallbackRelease;
				sound[i].AsyncCallbackVtbl.GetParameters=AsyncCallbackGetParameters;
				sound[i].AsyncCallbackVtbl.Invoke=AsyncCallbackInvoke;
				sound[i].AsyncCallback.lpVtbl=&sound[i].AsyncCallbackVtbl;
				IMFSourceResolver *Resolver;
				if(SUCCEEDED(sound[i].session->lpVtbl->BeginGetEvent(sound[i].session,&sound[i].AsyncCallback,NULL))&&
					SUCCEEDED(MFCreateSourceResolver(&Resolver)))
				{
					MF_OBJECT_TYPE ObjectType;
					if(SUCCEEDED(Resolver->lpVtbl->CreateObjectFromURL(Resolver,URL[i],MF_RESOLUTION_MEDIASOURCE,
						NULL,&ObjectType,(IUnknown**)&sound[i].source)))
					{
						IMFPresentationDescriptor *PresentationDescriptor;
						if(SUCCEEDED(sound[i].source->lpVtbl->CreatePresentationDescriptor(sound[i].source,&PresentationDescriptor)))
						{
							DWORD count;
							if(SUCCEEDED(PresentationDescriptor->lpVtbl->GetStreamDescriptorCount(PresentationDescriptor,&count)))
								for(DWORD j=0;j<count;j++)
								{
									BOOL selected=false;
									IMFStreamDescriptor *StreamDescriptor;
									IMFTopologyNode *SourceNode;
									if(SUCCEEDED(PresentationDescriptor->lpVtbl->GetStreamDescriptorByIndex(PresentationDescriptor,j,&selected,&StreamDescriptor))&&
										selected&&SUCCEEDED(MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE,&SourceNode))&&
										SUCCEEDED(SourceNode->lpVtbl->SetUnknown(SourceNode,&MF_TOPONODE_SOURCE,(IUnknown*)sound[i].source))&&
										SUCCEEDED(SourceNode->lpVtbl->SetUnknown(SourceNode,&MF_TOPONODE_PRESENTATION_DESCRIPTOR,(IUnknown*)PresentationDescriptor))&&
										SUCCEEDED(SourceNode->lpVtbl->SetUnknown(SourceNode,&MF_TOPONODE_STREAM_DESCRIPTOR,(IUnknown*)StreamDescriptor))&&
										SUCCEEDED(Topology->lpVtbl->AddNode(Topology,SourceNode)))
									{
										GUID guid;
										IMFMediaTypeHandler *MediaTypeHandler;
										if(SUCCEEDED(StreamDescriptor->lpVtbl->GetMediaTypeHandler(StreamDescriptor,&MediaTypeHandler))&&
											SUCCEEDED(MediaTypeHandler->lpVtbl->GetMajorType(MediaTypeHandler,&guid)))
										{
											IMFActivate *Activate;
											if(!memcmp(&guid,&MFMediaType_Audio,sizeof(GUID))&&SUCCEEDED(MFCreateAudioRendererActivate(&Activate)))
												success=true;

											if(success)
											{
												success=false;
												IMFTopologyNode *OutputNode;
												if(SUCCEEDED(MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE,&OutputNode))&&
													SUCCEEDED(OutputNode->lpVtbl->SetObject(OutputNode,(IUnknown*)Activate))&&
													SUCCEEDED(Topology->lpVtbl->AddNode(Topology,OutputNode))&&
													SUCCEEDED(SourceNode->lpVtbl->ConnectOutput(SourceNode,0,OutputNode,0)))
												{
													success=true;
													OutputNode->lpVtbl->Release(OutputNode);
													Activate->lpVtbl->Release(Activate);
												}
											}
											MediaTypeHandler->lpVtbl->Release(MediaTypeHandler);
										}
										SourceNode->lpVtbl->Release(SourceNode);
										StreamDescriptor->lpVtbl->Release(StreamDescriptor);
									}
								}
							PresentationDescriptor->lpVtbl->Release(PresentationDescriptor);
						}
					}
					Resolver->lpVtbl->Release(Resolver);
				}
				if(success)
				{
					success=false;
					if(SUCCEEDED(sound[i].session->lpVtbl->SetTopology(sound[i].session,0,Topology)))
						success=true;
				}
				Topology->lpVtbl->Release(Topology);
			}
			if(!success)
				break;
		}
	}
	if(success)
		return sound;
	SoundDestrutor(sound);
	return NULL;
}

HRESULT __stdcall AsyncCallbackQueryInterface(IMFAsyncCallback * This,REFIID riid,void ** ppvObject)
{
	return S_OK;
}

ULONG __stdcall AsyncCallbackAddRef(IMFAsyncCallback * This)
{
	return S_OK;
}

ULONG __stdcall AsyncCallbackRelease(IMFAsyncCallback * This)
{
	return S_OK;
}

HRESULT __stdcall AsyncCallbackGetParameters(IMFAsyncCallback * This,DWORD * pdwFlags,DWORD * pdwQueue)
{
	return E_NOTIMPL;
}

HRESULT __stdcall AsyncCallbackInvoke(IMFAsyncCallback * This,IMFAsyncResult * pAsyncResult)
{
	Sound *sound=(Sound*)This;
	IMFMediaEvent *MediaEvent;
	if(SUCCEEDED(sound->session->lpVtbl->EndGetEvent(sound->session,pAsyncResult,&MediaEvent)))
	{
		MediaEventType type;
		if(SUCCEEDED(MediaEvent->lpVtbl->GetType(MediaEvent,&type)))
			switch(type)
			{
				case MEEndOfPresentation:
					sound->playing=false;
					break;

				case MESessionClosed:
					return S_OK;
			}
		if(SUCCEEDED(sound->session->lpVtbl->BeginGetEvent(sound->session,This,NULL)))
			return S_OK;
	}
	return E_UNEXPECTED;
}

void SoundDestrutor(Sound *sound)
{
	for(int i=0;i<gameover+1;i++)
		if(sound[i].session)
		{
			sound[i].session->lpVtbl->Close(sound[i].session);
			if(sound[i].source)
			{
				sound[i].source->lpVtbl->Shutdown(sound[i].source);
				sound[i].source->lpVtbl->Release(sound[i].source);
			}
			sound[i].session->lpVtbl->Shutdown(sound[i].session);
			sound[i].session->lpVtbl->Release(sound[i].session);
		}
	free(sound);
}