// Copyright (c) Caleb Klomparens
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#ifndef UM_EXPORT_LIBRARY
#define UM_EXPORT_LIBRARY
#endif
#include "um.h"
#undef UM_EXPORT_LIBRARY

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <mmsystem.h>
#undef WIN32_LEAN_AND_MEAN

#include <stdio.h> // temp

void CALLBACK MyMidiCallback(
	HMIDIIN   hMidiIn,
	UINT      wMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2)
{
	(void)hMidiIn;
	(void)dwInstance;
	printf("MIDI message: %u %u %u\n", wMsg, (unsigned)dwParam1, (unsigned)dwParam2);
}
#elif defined(__APPLE__)

#include <CoreMIDI/MidiServices.h>

#include <stdio.h>

// ___________________________________________________________________________________________
// test program to echo MIDI In to Out
// ___________________________________________________________________________________________

MIDIPortRef     gOutPort = 0;
MIDIEndpointRef gDest = 0;
int             gChannel = 0;

static void MyReadProc(const MIDIPacketList *pktlist, void *refCon, void *connRefCon)
{
    if (gOutPort != 0 && gDest != 0) {
        MIDIPacket *packet = (MIDIPacket *)pktlist->packet; // remove const (!)
        for (unsigned int j = 0; j < pktlist->numPackets; ++j) {
            for (int i = 0; i < packet->length; ++i) {
//              printf("%02X ", packet->data[i]);

                // rechannelize status bytes
                if (packet->data[i] >= 0x80 && packet->data[i] < 0xF0)
                    packet->data[i] = (packet->data[i] & 0xF0) | (Byte)gChannel;
            }

//          printf("\n");
            packet = MIDIPacketNext(packet);
        }

        MIDISend(gOutPort, gDest, pktlist);
    }
}

#else
    // TODO: linux distros
#endif

void um_list_devices(um_Device** outDevices, unsigned* outNumDevices)
{
#if defined(_WIN32)
	const UINT NumInputDevices = midiInGetNumDevs();
	const UINT NumOutputDevices = midiOutGetNumDevs();
	
	printf("um: # input devices = %u, # output devices = %u\n", NumInputDevices, NumOutputDevices);
    for (UINT i = 0; i < NumInputDevices; ++i)
	{
		MIDIINCAPS incaps;
		midiInGetDevCaps(i, &incaps, sizeof(incaps));

		printf("%s\n", incaps.szPname);
	}
	
	HMIDIIN handle;
    midiInOpen(&handle, 0, (DWORD_PTR)MyMidiCallback, 0, CALLBACK_FUNCTION);
	midiInStart(handle);
	(void)outDevices;
	(void)outNumDevices;
#elif defined(__APPLE__)
    // create client and ports
    MIDIClientRef client = 0;
    MIDIClientCreate(CFSTR("MIDI Echo"), NULL, NULL, &client);

    MIDIPortRef inPort = 0;
    MIDIInputPortCreate(client, CFSTR("Input port"), MyReadProc, NULL, &inPort);
    MIDIOutputPortCreate(client, CFSTR("Output port"), &gOutPort);

    // enumerate devices (not really related to purpose of the echo program
    // but shows how to get information about devices)
    unsigned long i, n;
    CFStringRef pname, pmanuf, pmodel;
    char name[64], manuf[64], model[64];

    n = MIDIGetNumberOfDevices();
    for (i = 0; i < n; ++i) {
        MIDIDeviceRef dev = MIDIGetDevice(i);

        MIDIObjectGetStringProperty(dev, kMIDIPropertyName, &pname);
        MIDIObjectGetStringProperty(dev, kMIDIPropertyManufacturer, &pmanuf);
        MIDIObjectGetStringProperty(dev, kMIDIPropertyModel, &pmodel);

        CFStringGetCString(pname, name, sizeof(name), 0);
        CFStringGetCString(pmanuf, manuf, sizeof(manuf), 0);
        CFStringGetCString(pmodel, model, sizeof(model), 0);
        CFRelease(pname);
        CFRelease(pmanuf);
        CFRelease(pmodel);

        printf("name=%s, manuf=%s, model=%s\n", name, manuf, model);
    }

    // open connections from all sources
    n = MIDIGetNumberOfSources();
    printf("%d sources\n", (int)n);
    for (i = 0; i < n; ++i) {
        MIDIEndpointRef src = MIDIGetSource(i);
        MIDIPortConnectSource(inPort, src, NULL);
    }

    // find the first destination
    n = MIDIGetNumberOfDestinations();
    if (n > 0)
    {
        gDest = MIDIGetDestination(0);
    }

    if (gDest != 0)
    {
        MIDIObjectGetStringProperty(gDest, kMIDIPropertyName, &pname);
        CFStringGetCString(pname, name, sizeof(name), 0);
        CFRelease(pname);
        printf("Echoing to channel %d of %s\n", gChannel + 1, name);
    }
    else
    {
        printf("No MIDI destinations present\n");
    }
#else
    (void)outDevices;
    (void)outNumDevices;
    // TODO: linux distros
#endif
}

void um_open_device(um_Device* inDevice)
{
	(void)inDevice;
}

void um_close_device(um_Device* inDevice)
{
	(void)inDevice;
}
