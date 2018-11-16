#pragma once
#include "stdio.h"
#include "stdint.h"
#include<string.h>
#include <tuple>
struct xbox
{
	uint32_t size;
	uint32_t type;
};

struct xbox_file_type
{
	uint32_t size;
	uint32_t type;
	uint32_t major_brand;//must be  qt
	uint32_t minor_version;// qt 版本发布日期。
	uint32_t **compatible_brands;//一般包括qt，代表此文件和qt兼容；否则与qt不兼容
	uint32_t compatible_brands_count;

};

struct xbox_moov_mvhd
{
	uint32_t size;
	uint32_t type;
	uint8_t version;
	uint8_t flags[3];
	uint32_t creationtime;
	uint32_t mofificatiotime;
	uint32_t timescale;
	uint32_t duration;
	float preferredrate;  // 32 bit fixed-point  16bit/16bit
	float preferredvolume;// 16 bit fixed-point 8bit/8bit
	uint8_t reserved[10];
	uint32_t matrixstructure[9];
	uint32_t previewtime;
	uint32_t previewduration;
	uint32_t postertime;
	uint32_t selectiontime;
	uint32_t selectionduration;
	uint32_t currenttime;
	uint32_t nexttrackid;
};

struct xbox_moov_trak_tkhd
{
	uint32_t size;
	uint32_t type;
	uint8_t version;
	uint8_t flags[3];
	uint32_t creationtime;
	uint32_t mofificatiotime;
	uint32_t trackid;
	uint8_t reserved1_4[4];
	uint32_t duration;
	uint8_t reserved2_8[8];
	uint16_t layer;
	uint16_t alternate_group;
	uint16_t volume;
	uint8_t reserved3_2[2];
	uint32_t matrixstructure[9];
	float track_width; // 32 bit fixed-point
	float track_height;// 32 bit fixed-point
};


struct xbox_moov_trak_mdia_mdhd
{
	uint32_t size;
	uint32_t type;
	uint8_t version;
	uint8_t flags[3];
	uint32_t creationtime;
	uint32_t mofificatiotime;
	uint32_t timescale;
	uint32_t duration;
	uint16_t language;
	uint16_t quality;
};

struct xbox_moov_trak_x_hdlr
{
	uint32_t size;
	uint32_t type;
	uint8_t version;
	uint8_t flags[3];
	uint32_t componenttype;//mhlr - media handler(video/sound);dhlr- data handler
	uint32_t componentsubtype;//vide-video data;soun-sound data.
	uint32_t componentmaufacturer;//reserved,set to 0
	uint32_t componentflags;//reserved,set to 0
	uint16_t componentflagsmask;//reserved,set to 0
	char* componentname;
	uint32_t private_componentnamelenght;
};



struct xbox_moov_trak_mdia_minf_vmhd
{
	uint32_t size;
	uint32_t type;
	uint8_t version;
	uint8_t flags[3];
	uint16_t graphicsmode;
	uint16_t opcolorred;
	uint16_t opcolorgreen;
	uint16_t opcolorblue;
};


struct xbox_moov_trak_mdia_minf_smhd
{
	uint32_t size;
	uint32_t type;
	uint8_t version;
	uint8_t flags[3];
	uint16_t balance;
	uint16_t reserved;
};


struct xbox_moov_trak_mdia_minf_dinf_dref
{
	uint32_t size;
	uint32_t type;
	uint8_t version;
	uint8_t flags[3];
	uint32_t number;
};

struct xbox_moov_trak_mdia_minf_dinf_dref_data
{
	uint32_t size;
	uint32_t type;
	uint8_t version;
	uint8_t flags[3];
	uint8_t *data;
	uint32_t private_datasize;
};


struct videosampledescription
{
	uint32_t size;
	uint32_t vendor;
	uint64_t version;
	uint16_t revisionlevel;
	uint16_t datarefindex;
	uint8_t handler[12];
	uint16_t width;
	uint16_t height;
	uint8_t *   description;
	uint32_t private_descriptionsize;
};

struct xbox_moov_trak_mdia_minf_stbl_stsd_for_video
{
	uint32_t size;
	uint32_t type;
	uint8_t version;
	uint8_t flags[3];
	uint32_t number;
	videosampledescription *sampledescription;
};


struct soundsampledescription
{
	uint32_t size;
	uint32_t vendor;
	uint16_t datarefindex;
	uint16_t version;
	uint16_t revisionlevel;
	uint32_t numberofchannels;
	uint32_t siamplesize;
	uint64_t compressionid;
	uint16_t packetsize;
	uint32_t samplerate;
};

struct xbox_moov_trak_mdia_minf_stbl_stsd_for_sound
{
	uint32_t size;
	uint32_t type;
	uint8_t version;
	uint8_t flags[3];
	uint32_t number;
	soundsampledescription *sampledescription;
};


struct time_to_sample
{
	uint32_t samplecount;
	uint32_t sampleduration;
};
struct xbox_moov_trak_mdia_minf_stbl_stts //time to sample
{
	uint32_t size;
	uint32_t type;
	uint8_t version;
	uint8_t flags[3];
	uint32_t number;
	time_to_sample * timetosample;
};


//计算本table item的chunk包含多少的sample算法：用下一个item的fistchunk 减去本item的firstchunk 再乘以本item的samplesperchunk。
struct sample_to_chunk_table
{
	uint32_t firstchunk;// 本table item容纳的第一个chunk是这个tack中总chunk数的第几个chunk。
	uint32_t samplesperchunk; // 本table item，每个chunk能容纳多少个sample。
	uint32_t sampledescriptionid;
};

struct xbox_moov_trak_mdia_minf_stbl_stsc //sample to chunk
{
	uint32_t size;
	uint32_t type;
	uint8_t version;
	uint8_t flags[3];
	uint32_t number;
	sample_to_chunk_table * sampletochunk;
};


struct xbox_moov_trak_mdia_minf_stbl_stsz //sample size atom
{
	uint32_t size;
	uint32_t type;
	uint8_t version;
	uint8_t flags[3];
	uint32_t samplesize;
	uint32_t number;
	uint32_t * samplesizetable;
};

struct xbox_moov_trak_mdia_minf_stbl_co64 //chunk offset
{
	uint32_t size;
	uint32_t type;
	uint8_t version;
	uint8_t flags[3];
	uint32_t number;
	uint64_t *chunkoffsettable;
};


struct x_mov_track
{
	xbox_moov_trak_tkhd tkhd;
	xbox_moov_trak_mdia_minf_stbl_stts stts;
	xbox_moov_trak_mdia_minf_stbl_stsc stsc;
	xbox_moov_trak_mdia_minf_stbl_stsz stsz;
	xbox_moov_trak_mdia_minf_stbl_co64 co64;
};

struct x_mov_file
{
	char szFilename[1024];
	uint32_t video_frames;
	uint32_t audio_channels;
	uint64_t duration;
	uint32_t timescale;
	uint16_t samplesize;
	x_mov_track vtrack;
	x_mov_track atrack[16];
	FILE *pf;
};

int prototype_parser(char *pszfilename);


int prototype_parser(char *pszfilename);

int x_mov_open(x_mov_file *file, char *pszfilename);

int x_mov_get_video_package(x_mov_file *file, uint64_t ui64frameindex,uint64_t *out_pui64offset, uint64_t*out_pui64size);

int x_mov_get_audio_package(x_mov_file *file, uint64_t ui64frameindex,uint32_t ui32channelindex, uint64_t *out_pui64offset, uint64_t*out_pui64size);



void* memcpy_b2l(void*pDst, void*pSrc, size_t size);
void* memcpy_l2b(void*pDst, void*pSrc, size_t size);

